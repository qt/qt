
#include "qmlscriptparser_p.h"

#include "parser/javascriptengine_p.h"
#include "parser/javascriptparser_p.h"
#include "parser/javascriptlexer_p.h"
#include "parser/javascriptnodepool_p.h"
#include "parser/javascriptastvisitor_p.h"
#include "parser/javascriptast_p.h"
#include "parser/javascriptprettypretty_p.h"

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

        void pushProperty(const QString &name, int lineNumber)
        {
            const State &state = top();
            if (state.property) {
                State s(state.property->getValue(),
                        state.property->getValue()->getProperty(name.toLatin1()));
                s.property->line = lineNumber;
                push(s);
            } else {
                State s(state.object,
                        state.object->getProperty(name.toLatin1()));
                s.property->line = lineNumber;
                push(s);
            }
        }
    };

public:
    ProcessAST(QmlScriptParser *parser);
    virtual ~ProcessAST();

    void operator()(AST::Node *node);

protected:
    Object *defineObjectBinding(int line,
                             AST::UiQualifiedId *propertyName,
                             const QString &objectType,
                             AST::UiObjectInitializer *initializer = 0);
        QString getPrimitive(const QByteArray &propertyName, AST::ExpressionNode *expr);
        void defineProperty(const QString &propertyName, int line, const QString &primitive);

    using AST::Visitor::visit;
    using AST::Visitor::endVisit;

    virtual bool visit(AST::UiObjectDefinition *node);
    virtual bool visit(AST::UiPublicMember *node);
    virtual bool visit(AST::UiObjectBinding *node);

    virtual bool visit(AST::UiScriptBinding *node);
    virtual bool visit(AST::UiArrayBinding *node);

    void accept(AST::Node *node);

    QString asString(AST::UiQualifiedId *node) const;

    const State state() const;
    Object *currentObject() const;
    Property *currentProperty() const;

    QString qualifiedNameId() const;

private:
    QmlScriptParser *_parser;
    StateStack _stateStack;
    QStringList _scope;
};

ProcessAST::ProcessAST(QmlScriptParser *parser)
    : _parser(parser)
{
}

ProcessAST::~ProcessAST()
{
}

void ProcessAST::operator()(AST::Node *node)
{
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

Object *ProcessAST::defineObjectBinding(int line,
                                     AST::UiQualifiedId *propertyName,
                                     const QString &objectType,
                                     AST::UiObjectInitializer *initializer)
{
    bool isType = !objectType.isEmpty() && objectType.at(0).isUpper() && !objectType.contains(QLatin1Char('.'));
    if (!isType) {
        qWarning() << "bad name for a class"; // ### FIXME
        return false;
    }

    int propertyCount = 0;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(), propertyName->identifierToken.startLine);
    }

    // Class
    const int typeId = _parser->findOrCreateTypeId(objectType);

    Object *obj = new Object;
    obj->type = typeId;
    _scope.append(objectType);
    obj->typeName = qualifiedNameId().toLatin1();
    _scope.removeLast();
    obj->line = line;

    if (propertyCount) {
        Property *prop = currentProperty();
        Value *v = new Value;
        v->object = obj;
        v->line = line;
        prop->addValue(v);
    }

    while (propertyCount--)
        _stateStack.pop();


    if (! _parser->tree()) {
        _parser->setTree(obj);
        _stateStack.pushObject(obj);
    } else {
        const State state = _stateStack.top();
        Value *v = new Value;
        v->object = obj;
        v->line = line;
        if(state.property)
            state.property->addValue(v);
        else
            state.object->getDefaultProperty()->addValue(v);
        _stateStack.pushObject(obj);
    }

    accept(initializer);

    _stateStack.pop();

    return obj;
}

void ProcessAST::defineProperty(const QString &propertyName, int line, const QString &primitive)
{
    _stateStack.pushProperty(propertyName, line);
    Value *value = new Value;
    value->primitive = primitive;
    value->line = line;
    currentProperty()->addValue(value);
    _stateStack.pop();
}

// UiObjectMember: T_PUBLIC T_IDENTIFIER T_IDENTIFIER T_COLON Expression UiObjectInitializer ;
bool ProcessAST::visit(AST::UiPublicMember *node)
{
    const QString type = node->type->asString();
    const QString name = node->name->asString();

    if (type == QLatin1String("property")) {
        _stateStack.pushProperty(QLatin1String("properties"), node->publicToken.startLine);

        Object *obj = defineObjectBinding(node->identifierToken.startLine,
                                          0,
                                          QLatin1String("Property"));

        _stateStack.pushObject(obj);

        defineProperty(QLatin1String("name"), node->identifierToken.startLine, name);
        if (node->expression) // default value
            defineProperty(QLatin1String("value"), node->identifierToken.startLine, getPrimitive("value", node->expression));

        _stateStack.pop(); // object
        _stateStack.pop(); // properties

    } else {
        qWarning() << "bad public identifier" << type; // ### FIXME
    }


    // ### TODO drop initializer (unless some example needs differnet properties than name and type and value.

    return false;
}


// UiObjectMember: T_IDENTIFIER UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectDefinition *node)
{
    defineObjectBinding(node->identifierToken.startLine,
                        0,
                        node->name->asString(),
                        node->initializer);
    return false;
}


// UiObjectMember: UiQualifiedId T_COLON T_IDENTIFIER UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectBinding *node)
{
    if (asString(node->qualifiedId) == QLatin1String("propertyChangeSet")
        && node->name->asString() == QLatin1String("PropertyChangeSet")) {

        AST::UiObjectMemberList *it = node->initializer->members;
        for (; it; it = it->next) {
            if (it->member->kind != AST::Node::Kind_UiScriptBinding)
                continue; // ### TODO generate error
            AST::UiScriptBinding *scriptBinding = static_cast<AST::UiScriptBinding *>(it->member);


            QString target;
            QString property;
            QString value;

            int propertyCount = 0;
            AST::UiQualifiedId *propertyName = scriptBinding->qualifiedId;
            for (; propertyName; propertyName = propertyName->next){
                if (propertyName->next) {
                    if (!target.isEmpty())
                        target += QLatin1Char('.');
                    target += propertyName->name->asString();
                } else {
                    property = propertyName->name->asString();
                }
                ++propertyCount;
            }

            if (scriptBinding->statement->kind == AST::Node::Kind_ExpressionStatement) {
                AST::ExpressionStatement *stmt = static_cast<AST::ExpressionStatement *>(scriptBinding->statement);
                value = getPrimitive(property.toLatin1(), stmt->expression);
            } else {
                // #### TODO generate error
            }

            qDebug() << "SetProperty" << target << property << value;

            Object *obj = defineObjectBinding(node->identifierToken.startLine,
                                              0,
                                              QLatin1String("SetProperty"));

            _stateStack.pushObject(obj);
            if (!target.isEmpty())
                defineProperty(QLatin1String("target"), scriptBinding->colonToken.startLine, QLatin1Char('{') + target + QLatin1Char('}'));
            if (!property.isEmpty())
                defineProperty(QLatin1String("property"), scriptBinding->colonToken.startLine, property);
            if (!value.isEmpty())
                defineProperty(QLatin1String("value"), scriptBinding->colonToken.startLine, value);
            _stateStack.pop(); // object
        }
    } else {

        defineObjectBinding(node->identifierToken.startLine,
                            node->qualifiedId,
                            node->name->asString(),
                            node->initializer);
    }
    return false;
}

QString ProcessAST::getPrimitive(const QByteArray &propertyName, AST::ExpressionNode *expr)
{
    QString primitive;
    QTextStream out(&primitive);
    PrettyPretty pp(out);

    if(propertyName.length() >= 3 && propertyName.startsWith("on") &&
       ('A' <= propertyName.at(2) && 'Z' >= propertyName.at(2))) {
        pp(expr);

        // here comes a cruel hack until we support functions properly with arguments for signal properties
        if (primitive.startsWith(QLatin1String("function("))) {
            int brace = 0;
            for (;brace < primitive.size(); ++brace)
                if (primitive.at(brace) == QLatin1Char('{'))
                    break;
            primitive = primitive.mid(brace + 1, primitive.size() - brace - 2);
        }
        //end of hack

    } else if (propertyName == "id" && expr && expr->kind == AST::Node::Kind_IdentifierExpression) {
        primitive = static_cast<AST::IdentifierExpression *>(expr)->name->asString();
    } else if (expr->kind == AST::Node::Kind_StringLiteral) {
        // hack: emulate weird XML feature that string literals are not quoted.
        //This needs to be fixed in the qmlcompiler once xml goes away.
        primitive = static_cast<AST::StringLiteral *>(expr)->value->asString();
    } else if (expr->kind == AST::Node::Kind_TrueLiteral
               || expr->kind == AST::Node::Kind_FalseLiteral
               || expr->kind == AST::Node::Kind_NumericLiteral
               ) {
        pp(expr);
    } else {
        // create a binding
        out << "{";
        pp(expr);
        out << "}";
    }
    return primitive;
}


// UiObjectMember: UiQualifiedId T_COLON Statement ;
bool ProcessAST::visit(AST::UiScriptBinding *node)
{

    int propertyCount = 0;
    AST::UiQualifiedId *propertyName = node->qualifiedId;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(), propertyName->identifierToken.startLine);
    }

    Property *prop = currentProperty();
    QString primitive;
    QTextStream out(&primitive);
    PrettyPretty pp(out);

    if (node->statement->kind == AST::Node::Kind_ExpressionStatement) {
        AST::ExpressionStatement *stmt = static_cast<AST::ExpressionStatement *>(node->statement);
        primitive = getPrimitive(prop->name, stmt->expression);

    } else {
        out << '{';
        pp(node->statement);
        out << '}';
    }

    Value *v = new Value;
    v->primitive = primitive;
    v->line = node->colonToken.startLine;
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
        _stateStack.pushProperty(propertyName->name->asString(), propertyName->identifierToken.startLine);
    }

    accept(node->members);

    while (propertyCount--)
        _stateStack.pop();

    return false;
}

} // end of anonymous namespace


QmlScriptParser::QmlScriptParser()
    : root(0), _errorLine(-1)
{
}

QmlScriptParser::~QmlScriptParser()
{
}

bool QmlScriptParser::parse(const QByteArray &data, const QUrl &url)
{
    const QString fileName = url.toString();
    const QString code = QString::fromUtf8(data); // ### FIXME

    JavaScriptParser parser;
    JavaScriptEnginePrivate driver;

    NodePool nodePool(fileName, &driver);
    driver.setNodePool(&nodePool);

    Lexer lexer(&driver);
    lexer.setCode(code, /*line = */ 1);
    driver.setLexer(&lexer);

    if (! parser.parse(&driver)) {
        _error = parser.errorMessage();
        _errorLine = parser.errorLineNumber();
        return false;
    }

    ProcessAST process(this);
    process(parser.ast());

    return true;
}

QString QmlScriptParser::errorDescription() const
{
    return _error;
}

int QmlScriptParser::errorLine() const
{
    return _errorLine;
}

QMap<QString,QString> QmlScriptParser::nameSpacePaths() const
{
    qWarning() << Q_FUNC_INFO << "not implemented";
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


QT_END_NAMESPACE
