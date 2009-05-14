
#include "qmlscriptparser_p.h"
#include "qmlparser_p.h"

#include "parser/javascriptengine_p.h"
#include "parser/javascriptparser_p.h"
#include "parser/javascriptlexer_p.h"
#include "parser/javascriptnodepool_p.h"
#include "parser/javascriptastvisitor_p.h"
#include "parser/javascriptast_p.h"

#include <QStack>
#include <QtDebug>

QT_BEGIN_NAMESPACE

using namespace JavaScript;
using namespace QmlParser;

namespace {

class ProcessAST: protected AST::Visitor
{
    struct State {
        State() : object(0), property(0) {}
        State(Object *o) : object(o), property(0) {}
        State(Object *o, Property *p) : object(o), property(p) {}

        Object *object;
        Property *property;
    };

    struct StateStack : public QStack<State>
    {
        void pushObject(Object *obj)
        {
            push(State(obj));
        }

        void pushProperty(const QString &name, const LocationSpan &location)
        {
            const State &state = top();
            if (state.property) {
                State s(state.property->getValue(),
                        state.property->getValue()->getProperty(name.toLatin1()));
                s.property->location = location;
                push(s);
            } else {
                State s(state.object,
                        state.object->getProperty(name.toLatin1()));

                s.property->location = location;
                push(s);
            }
        }
    };

public:
    ProcessAST(QmlScriptParser *parser);
    virtual ~ProcessAST();

    void operator()(const QString &code, AST::Node *node);

protected:
    Object *defineObjectBinding(int line,
                             AST::UiQualifiedId *propertyName,
                             const QString &objectType,
                             AST::SourceLocation typeLocation,
                             LocationSpan location,
                             AST::UiObjectInitializer *initializer = 0);
    Object *defineObjectBinding_helper(int line,
                             AST::UiQualifiedId *propertyName,
                             const QString &objectType,
                             AST::SourceLocation typeLocation,
                             LocationSpan location,
                             AST::UiObjectInitializer *initializer = 0);

    QmlParser::Variant getVariant(AST::ExpressionNode *expr);

    LocationSpan location(AST::SourceLocation start, AST::SourceLocation end);
    LocationSpan location(AST::UiQualifiedId *);

    using AST::Visitor::visit;
    using AST::Visitor::endVisit;

    virtual bool visit(AST::UiProgram *node);
    virtual bool visit(AST::UiImport *node);
    virtual bool visit(AST::UiObjectDefinition *node);
    virtual bool visit(AST::UiPublicMember *node);
    virtual bool visit(AST::UiObjectBinding *node);

    virtual bool visit(AST::UiScriptBinding *node);
    virtual bool visit(AST::UiArrayBinding *node);
    virtual bool visit(AST::UiSourceElement *node);

    void accept(AST::Node *node);

    QString asString(AST::UiQualifiedId *node) const;

    const State state() const;
    Object *currentObject() const;
    Property *currentProperty() const;

    QString qualifiedNameId() const;

    QString textAt(const AST::SourceLocation &loc) const
    { return _contents.mid(loc.offset, loc.length); }

    QString textAt(const AST::SourceLocation &first,
                   const AST::SourceLocation &last) const
    { return _contents.mid(first.offset, last.offset + last.length - first.offset); }

    QString asString(AST::ExpressionNode *expr) const
    {
        if (! expr)
            return QString();

        return textAt(expr->firstSourceLocation(), expr->lastSourceLocation());
    }

    QString asString(AST::Statement *stmt) const
    {
        if (! stmt)
            return QString();

        QString s = textAt(stmt->firstSourceLocation(), stmt->lastSourceLocation());
        s += QLatin1Char('\n');
        return s;
    }

private:
    QmlScriptParser *_parser;
    StateStack _stateStack;
    QStringList _scope;
    QString _contents;

    inline bool isSignalProperty(const QByteArray &propertyName) const {
        return (propertyName.length() >= 3 && propertyName.startsWith("on") &&
                ('A' <= propertyName.at(2) && 'Z' >= propertyName.at(2)));
    }

};

ProcessAST::ProcessAST(QmlScriptParser *parser)
    : _parser(parser)
{
}

ProcessAST::~ProcessAST()
{
}

void ProcessAST::operator()(const QString &code, AST::Node *node)
{
    _contents = code;
    accept(node);
}

void ProcessAST::accept(AST::Node *node)
{
    AST::Node::acceptChild(node, this);
}

const ProcessAST::State ProcessAST::state() const
{
    if (_stateStack.isEmpty())
        return State();

    return _stateStack.back();
}

Object *ProcessAST::currentObject() const
{
    return state().object;
}

Property *ProcessAST::currentProperty() const
{
    return state().property;
}

QString ProcessAST::qualifiedNameId() const
{
    return _scope.join(QLatin1String("/"));
}

QString ProcessAST::asString(AST::UiQualifiedId *node) const
{
    QString s;

    for (AST::UiQualifiedId *it = node; it; it = it->next) {
        s.append(it->name->asString());

        if (it->next)
            s.append(QLatin1Char('.'));
    }

    return s;
}

Object *
ProcessAST::defineObjectBinding_helper(int line,
                                       AST::UiQualifiedId *propertyName,
                                       const QString &objectType,
                                       AST::SourceLocation typeLocation,
                                       LocationSpan location,
                                       AST::UiObjectInitializer *initializer)
{
    bool isType = !objectType.isEmpty() && objectType.at(0).isUpper() && !objectType.contains(QLatin1Char('.'));

    int propertyCount = 0;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(), 
                                 this->location(propertyName));
    }

    if (!isType) {

        if(propertyCount || !currentObject()) {
            QmlError error;
            error.setDescription("Expected type name");
            error.setLine(typeLocation.startLine);
            error.setColumn(typeLocation.startColumn);
            _parser->_errors << error;
            return 0;
        }

        LocationSpan loc = ProcessAST::location(typeLocation, typeLocation);
        if (propertyName)
            loc = ProcessAST::location(propertyName);

        _stateStack.pushProperty(objectType, loc);
       accept(initializer);
        _stateStack.pop();

        return 0;

    } else {

        // Class
        const int typeId = _parser->findOrCreateTypeId(objectType);

        Object *obj = new Object;
        obj->type = typeId;
        _scope.append(objectType);
        obj->typeName = qualifiedNameId().toLatin1();
        _scope.removeLast();
        obj->location = location;

        if (propertyCount) {

            Property *prop = currentProperty();
            Value *v = new Value;
            v->object = obj;
            v->location = obj->location;
            prop->addValue(v);

            while (propertyCount--)
                _stateStack.pop();

        } else {

            if (! _parser->tree()) {
                _parser->setTree(obj);
            } else {
                const State state = _stateStack.top();
                Value *v = new Value;
                v->object = obj;
                v->location = obj->location;
                if (state.property)
                    state.property->addValue(v);
                else
                    state.object->getDefaultProperty()->addValue(v);
            }
        }

        _stateStack.pushObject(obj);
        accept(initializer);
        _stateStack.pop();

        return obj;
    }
}

Object *ProcessAST::defineObjectBinding(int line,
                                     AST::UiQualifiedId *qualifiedId,
                                     const QString &objectType,
                                     AST::SourceLocation typeLocation,
                                     LocationSpan location,
                                     AST::UiObjectInitializer *initializer)
{
    if (objectType == QLatin1String("Connection")) {

        Object *obj = defineObjectBinding_helper(line, 0, objectType, typeLocation, location);

        _stateStack.pushObject(obj);

        AST::UiObjectMemberList *it = initializer->members;
        for (; it; it = it->next) {
            AST::UiScriptBinding *scriptBinding = AST::cast<AST::UiScriptBinding *>(it->member);
            if (! scriptBinding)
                continue;

            QString propertyName = asString(scriptBinding->qualifiedId);
            if (propertyName == QLatin1String("script")) {
                QString script;

                if (AST::ExpressionStatement *stmt = AST::cast<AST::ExpressionStatement *>(scriptBinding->statement)) {
                    script = getVariant(stmt->expression).asScript();
                } else {
                    script = asString(scriptBinding->statement);
                }

                LocationSpan l = this->location(scriptBinding->statement->firstSourceLocation(), 
                                                scriptBinding->statement->lastSourceLocation());

                _stateStack.pushProperty(QLatin1String("script"), l);
                Value *value = new Value;
                value->value = QmlParser::Variant(script);
                value->location = l;
                currentProperty()->addValue(value);
                _stateStack.pop();
            } else {
                accept(it->member);
            }
        }

        _stateStack.pop(); // object

        return obj;
    }

    return defineObjectBinding_helper(line, qualifiedId, objectType, typeLocation, location, initializer);
}

LocationSpan ProcessAST::location(AST::UiQualifiedId *id)
{
    return location(id->identifierToken, id->identifierToken);
}

LocationSpan ProcessAST::location(AST::SourceLocation start, AST::SourceLocation end)
{
    LocationSpan rv;
    rv.start.line = start.startLine;
    rv.start.column = start.startColumn;
    rv.end.line = end.startLine;
    rv.end.column = end.startColumn + end.length - 1;
    rv.range.offset = start.offset;
    rv.range.length = end.offset + end.length - start.offset;
    return rv;
}

// UiProgram: UiImportListOpt UiObjectMemberList ;
bool ProcessAST::visit(AST::UiProgram *node)
{
    accept(node->imports);
    accept(node->members->member);
    return false;
}

// UiImport: T_IMPORT T_STRING_LITERAL ;
bool ProcessAST::visit(AST::UiImport *node)
{
    QString fileName = node->fileName->asString();
    _parser->addNamespacePath(fileName);

    AST::SourceLocation startLoc = node->importToken;
    AST::SourceLocation endLoc = node->semicolonToken;

    QmlScriptParser::Import import;
    import.location = location(startLoc, endLoc);
    import.uri = fileName;

    _parser->_imports << import;

    return false;
}

bool ProcessAST::visit(AST::UiPublicMember *node)
{
    if(node->type == AST::UiPublicMember::Signal) {
        const QString name = node->name->asString();

        Object::DynamicSignal signal;
        signal.name = name.toUtf8();

        _stateStack.top().object->dynamicSignals << signal;
    } else {
        const QString memberType = node->memberType->asString();
        const QString name = node->name->asString();

        const struct TypeNameToType {
            const char *name;
            Object::DynamicProperty::Type type;
        } propTypeNameToTypes[] = {
            { "int", Object::DynamicProperty::Int },
            { "bool", Object::DynamicProperty::Bool },
            { "double", Object::DynamicProperty::Real },
            { "real", Object::DynamicProperty::Real },
            { "string", Object::DynamicProperty::String },
            { "color", Object::DynamicProperty::Color },
            { "date", Object::DynamicProperty::Date },
            { "var", Object::DynamicProperty::Variant },
            { "variant", Object::DynamicProperty::Variant }
        };
        const int propTypeNameToTypesCount = sizeof(propTypeNameToTypes) / 
                                             sizeof(propTypeNameToTypes[0]);

        bool typeFound = false;
        Object::DynamicProperty::Type type;
        for(int ii = 0; !typeFound && ii < propTypeNameToTypesCount; ++ii) {
            if(QLatin1String(propTypeNameToTypes[ii].name) == memberType) {
                type = propTypeNameToTypes[ii].type;
                typeFound = true;
            }
        }
        
        if(!typeFound) {
            QmlError error;
            error.setDescription("Expected property type");
            error.setLine(node->typeToken.startLine);
            error.setColumn(node->typeToken.startColumn);
            _parser->_errors << error;
            return false;
        }

        Object::DynamicProperty property;
        property.isDefaultProperty = node->isDefaultMember;
        property.type = type;
        property.name = name.toUtf8();

        if (node->expression) { // default value
            property.defaultValue = new Property;
            Value *value = new Value;
            value->location = location(node->expression->firstSourceLocation(),
                                       node->expression->lastSourceLocation());
            value->value = getVariant(node->expression);
            property.defaultValue->values << value;
        }

        _stateStack.top().object->dynamicProperties << property;
    }

    return true;
}


// UiObjectMember: T_IDENTIFIER UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectDefinition *node)
{
    LocationSpan l = location(node->firstSourceLocation(),
                              node->lastSourceLocation());;

    defineObjectBinding(node->identifierToken.startLine,
                        0,
                        node->name->asString(),
                        node->identifierToken,
                        l,
                        node->initializer);

    return false;
}


// UiObjectMember: UiQualifiedId T_COLON T_IDENTIFIER UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectBinding *node)
{
    LocationSpan l;
    l = location(node->identifierToken, node->initializer->rbraceToken);

    defineObjectBinding(node->identifierToken.startLine,
                        node->qualifiedId,
                        node->name->asString(),
                        node->identifierToken,
                        l,
                        node->initializer);

    return false;
}

QmlParser::Variant ProcessAST::getVariant(AST::ExpressionNode *expr)
{
    if (AST::StringLiteral *lit = AST::cast<AST::StringLiteral *>(expr)) {
        return QmlParser::Variant(lit->value->asString());
    } else if (expr->kind == AST::Node::Kind_TrueLiteral) {
        return QmlParser::Variant(true);
    } else if (expr->kind == AST::Node::Kind_FalseLiteral) {
        return QmlParser::Variant(false);
    } else if (AST::NumericLiteral *lit = AST::cast<AST::NumericLiteral *>(expr)) {
        return QmlParser::Variant(lit->value, asString(expr));
    } else {

        if (AST::UnaryMinusExpression *unaryMinus = AST::cast<AST::UnaryMinusExpression *>(expr)) {
           if (AST::NumericLiteral *lit = AST::cast<AST::NumericLiteral *>(unaryMinus->expression)) {
               return QmlParser::Variant(-lit->value, asString(expr));
           }
        }

        return QmlParser::Variant(asString(expr), QmlParser::Variant::Script);
    }
}


// UiObjectMember: UiQualifiedId T_COLON Statement ;
bool ProcessAST::visit(AST::UiScriptBinding *node)
{
    int propertyCount = 0;
    AST::UiQualifiedId *propertyName = node->qualifiedId;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(), 
                                 location(propertyName));
    }

    Property *prop = currentProperty();

    QmlParser::Variant primitive;

    if (AST::ExpressionStatement *stmt = AST::cast<AST::ExpressionStatement *>(node->statement)) {
        primitive = getVariant(stmt->expression);
    } else { // do binding
        primitive = QmlParser::Variant(asString(node->statement), 
                                       QmlParser::Variant::Script);
    }

    Value *v = new Value;
    v->value = primitive;
    v->location = location(node->statement->firstSourceLocation(),
                           node->statement->lastSourceLocation());

    prop->addValue(v);

    while (propertyCount--)
        _stateStack.pop();

    return true;
}

// UiObjectMember: UiQualifiedId T_COLON T_LBRACKET UiObjectMemberList T_RBRACKET ;
bool ProcessAST::visit(AST::UiArrayBinding *node)
{
    int propertyCount = 0;
    AST::UiQualifiedId *propertyName = node->qualifiedId;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(), 
                                 location(propertyName));
    }

    accept(node->members);

    while (propertyCount--)
        _stateStack.pop();

    return false;
}

bool ProcessAST::visit(AST::UiSourceElement *node)
{
    QmlParser::Object *obj = currentObject();

    bool isScript = (obj && obj->typeName == "Script");

    if (!isScript) {

        if (AST::FunctionDeclaration *funDecl = AST::cast<AST::FunctionDeclaration *>(node->sourceElement)) {

            if(funDecl->formals) {
                QmlError error;
                error.setDescription("Slot declarations must be parameterless");
                error.setLine(funDecl->lparenToken.startLine);
                error.setColumn(funDecl->lparenToken.startColumn);
                _parser->_errors << error;
                return false;
            }

            QString body = textAt(funDecl->lbraceToken, funDecl->rbraceToken);
            Object::DynamicSlot slot;
            slot.name = funDecl->name->asString().toUtf8();
            slot.body = body;
            obj->dynamicSlots << slot;
        } else {
            QmlError error;
            error.setDescription("JavaScript declaration outside Script element");
            error.setLine(node->firstSourceLocation().startLine);
            error.setColumn(node->firstSourceLocation().startColumn);
            _parser->_errors << error;
        }
        return false;

    } else {
        QString source;

        int line = 0;
        if (AST::FunctionDeclaration *funDecl = AST::cast<AST::FunctionDeclaration *>(node->sourceElement)) {
            line = funDecl->functionToken.startLine;
            source = asString(funDecl);
        } else if (AST::VariableStatement *varStmt = AST::cast<AST::VariableStatement *>(node->sourceElement)) {
            // ignore variable declarations
            line = varStmt->declarationKindToken.startLine;
        }

        Value *value = new Value;
        value->location = location(node->firstSourceLocation(), 
                                   node->lastSourceLocation());
        value->value = QmlParser::Variant(source);

        obj->getDefaultProperty()->addValue(value);
    }

    return false;
}

} // end of anonymous namespace


QmlScriptParser::QmlScriptParser()
: root(0)
{

}

QmlScriptParser::~QmlScriptParser()
{
}

bool QmlScriptParser::parse(const QByteArray &data, const QUrl &url)
{
    const QString fileName = url.toString();

    QTextStream stream(data, QIODevice::ReadOnly);
    const QString code = stream.readAll();

    Parser parser;
    Engine engine;

    NodePool nodePool(fileName, &engine);
    engine.setNodePool(&nodePool);

    Lexer lexer(&engine);
    lexer.setCode(code, /*line = */ 1);
    engine.setLexer(&lexer);

    if (! parser.parse(&engine) || !_errors.isEmpty()) {

        // Extract errors from the parser
        foreach (const Parser::DiagnosticMessage &m, parser.diagnosticMessages()) {

            if (m.isWarning())
                continue;

            QmlError error;
            error.setUrl(url);
            error.setDescription(m.message);
            error.setLine(m.loc.startLine);
            error.setColumn(m.loc.startColumn);
            _errors << error;

        }
    }

    if (_errors.isEmpty()) {
        ProcessAST process(this);
        process(code, parser.ast());

        // Set the url for process errors
        for(int ii = 0; ii < _errors.count(); ++ii) 
            _errors[ii].setUrl(url);
    }

    return _errors.isEmpty();
}

QMap<QString,QString> QmlScriptParser::nameSpacePaths() const
{
    return _nameSpacePaths;
}

QStringList QmlScriptParser::types() const
{
    return _typeNames;
}

Object *QmlScriptParser::tree() const
{
    return root;
}

QList<QmlScriptParser::Import> QmlScriptParser::imports() const
{
    return _imports;
}

QList<QmlError> QmlScriptParser::errors() const
{
    return _errors;
}

void QmlScriptParser::clear()
{
    if (root) {
        root->release();
        root = 0;
    }
    _nameSpacePaths.clear();
    _typeNames.clear();
    _errors.clear();
}

int QmlScriptParser::findOrCreateTypeId(const QString &name)
{
    int index = _typeNames.indexOf(name);

    if (index == -1) {
        index = _typeNames.size();
        _typeNames.append(name);
    }

    return index;
}

void QmlScriptParser::setTree(Object *tree)
{
    Q_ASSERT(! root);

    root = tree;
}

void QmlScriptParser::addNamespacePath(const QString &path)
{
    _nameSpacePaths.insertMulti(QString(), path);
}


QT_END_NAMESPACE
