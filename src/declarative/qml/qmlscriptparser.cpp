/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlscriptparser_p.h"
#include "qmlparser_p.h"

#include "parser/qmljsengine_p.h"
#include "parser/qmljsparser_p.h"
#include "parser/qmljslexer_p.h"
#include "parser/qmljsnodepool_p.h"
#include "parser/qmljsastvisitor_p.h"
#include "parser/qmljsast_p.h"

#include "rewriter/textwriter_p.h"

#include <QStack>
#include <QCoreApplication>
#include <QtDebug>

#include <qfxperf.h>

QT_BEGIN_NAMESPACE

using namespace QmlJS;
using namespace QmlParser;

namespace {

class RewriteNumericLiterals: protected AST::Visitor
{
    unsigned _position;
    TextWriter *_writer;

public:
    QString operator()(QString code, unsigned position, AST::Node *node)
    {
        TextWriter w;
        _writer = &w;
        _position = position;

        AST::Node::acceptChild(node, this);

        w.write(&code);

        return code;
    }

protected:
    using AST::Visitor::visit;

    virtual bool visit(AST::NumericLiteral *node)
    {
        if (node->suffix != AST::NumericLiteral::noSuffix) {
            const int suffixLength = AST::NumericLiteral::suffixLength[node->suffix];
            const char *suffixSpell = AST::NumericLiteral::suffixSpell[node->suffix];
            QString pre;
            pre += QLatin1String("qmlNumberFrom");
            pre += QChar(QLatin1Char(suffixSpell[0])).toUpper();
            pre += QLatin1String(&suffixSpell[1]);
            pre += QLatin1Char('(');
            _writer->replace(node->literalToken.begin() - _position, 0, pre);
            _writer->replace(node->literalToken.end() - _position - suffixLength,
                             suffixLength,
                             QLatin1String(")"));
        }

        return false;
    }
};

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
    Object *defineObjectBinding(AST::UiQualifiedId *propertyName,
                                AST::UiQualifiedId *objectTypeName,
                                LocationSpan location,
                                AST::UiObjectInitializer *initializer = 0);

    Object *defineObjectBinding_helper(AST::UiQualifiedId *propertyName,
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

    virtual bool visit(AST::ExpressionStatement *node);

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

    RewriteNumericLiterals rewriteNumericLiterals;

    QString asString(AST::ExpressionNode *expr)
    {
        if (! expr)
            return QString();

        return rewriteNumericLiterals(textAt(expr->firstSourceLocation(), expr->lastSourceLocation()),
                                      expr->firstSourceLocation().offset, expr);
    }

    QString asString(AST::Statement *stmt)
    {
        if (! stmt)
            return QString();

        QString s = rewriteNumericLiterals(textAt(stmt->firstSourceLocation(), stmt->lastSourceLocation()),
                                           stmt->firstSourceLocation().offset, stmt);

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
ProcessAST::defineObjectBinding_helper(AST::UiQualifiedId *propertyName,
                                       const QString &objectType,
                                       AST::SourceLocation typeLocation,
                                       LocationSpan location,
                                       AST::UiObjectInitializer *initializer)
{
    int lastTypeDot = objectType.lastIndexOf(QLatin1Char('.'));
    bool isType = !objectType.isEmpty() &&
                    (objectType.at(0).isUpper() ||
                        (lastTypeDot >= 0 && objectType.at(lastTypeDot+1).isUpper()));

    int propertyCount = 0;
    for (; propertyName; propertyName = propertyName->next){
        ++propertyCount;
        _stateStack.pushProperty(propertyName->name->asString(),
                                 this->location(propertyName));
    }

    if (!isType) {

        if(propertyCount || !currentObject()) {
            QmlError error;
            error.setDescription(QCoreApplication::translate("QmlParser","Expected type name"));
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

        QString resolvableObjectType = objectType;
        if (lastTypeDot >= 0)
            resolvableObjectType.replace(QLatin1Char('.'),QLatin1Char('/'));
        const int typeId = _parser->findOrCreateTypeId(resolvableObjectType);

        Object *obj = new Object;
        obj->type = typeId;

        // XXX this doesn't do anything (_scope never builds up)
        _scope.append(resolvableObjectType);
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

Object *ProcessAST::defineObjectBinding(AST::UiQualifiedId *qualifiedId,
                                        AST::UiQualifiedId *objectTypeName,
                                        LocationSpan location,
                                        AST::UiObjectInitializer *initializer)
{
    const QString objectType = asString(objectTypeName);
    const AST::SourceLocation typeLocation = objectTypeName->identifierToken;

    if (objectType == QLatin1String("Connection")) {

        Object *obj = defineObjectBinding_helper(/*propertyName = */0, objectType, typeLocation, location);

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

    return defineObjectBinding_helper(qualifiedId, objectType, typeLocation, location, initializer);
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
            { "url", Object::DynamicProperty::Url },
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
            error.setDescription(QCoreApplication::translate("QmlParser","Expected property type"));
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
            property.defaultValue->parent = _stateStack.top().object;
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


// UiObjectMember: UiQualifiedId UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectDefinition *node)
{
    LocationSpan l = location(node->firstSourceLocation(),
                              node->lastSourceLocation());

    defineObjectBinding(/*propertyName = */ 0,
                        node->qualifiedTypeNameId,
                        l,
                        node->initializer);

    return false;
}


// UiObjectMember: UiQualifiedId T_COLON UiQualifiedId UiObjectInitializer ;
bool ProcessAST::visit(AST::UiObjectBinding *node)
{
    LocationSpan l = location(node->qualifiedTypeNameId->identifierToken,
                              node->initializer->rbraceToken);

    defineObjectBinding(node->qualifiedId,
                        node->qualifiedTypeNameId,
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
        if (lit->suffix == AST::NumericLiteral::noSuffix)
            return QmlParser::Variant(lit->value, asString(expr));
        else
            return QmlParser::Variant(asString(expr), expr);

    } else {

        if (AST::UnaryMinusExpression *unaryMinus = AST::cast<AST::UnaryMinusExpression *>(expr)) {
           if (AST::NumericLiteral *lit = AST::cast<AST::NumericLiteral *>(unaryMinus->expression)) {
               return QmlParser::Variant(-lit->value, asString(expr));
           }
        }

        return  QmlParser::Variant(asString(expr), expr);
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
                                       node->statement);
    }

    prop->location.range.length = prop->location.range.offset + prop->location.range.length - node->qualifiedId->identifierToken.offset;
    prop->location.range.offset = node->qualifiedId->identifierToken.offset;
    Value *v = new Value;
    v->value = primitive;
    v->location = location(node->statement->firstSourceLocation(),
                           node->statement->lastSourceLocation());

    prop->addValue(v);

    while (propertyCount--)
        _stateStack.pop();

    return true;
}

bool ProcessAST::visit(AST::ExpressionStatement *node)
{
    if (!node->semicolonToken.isValid())
        _parser->addAutomaticSemicolonOffset(node->semicolonToken.offset);

    return true;
}

static QList<int> collectCommas(AST::UiArrayMemberList *members)
{
    QList<int> commas;

    if (members) {
        for (AST::UiArrayMemberList *it = members->next; it; it = it->next) {
            commas.append(it->commaToken.offset);
        }
    }

    return commas;
}

// UiObjectMember: UiQualifiedId T_COLON T_LBRACKET UiArrayMemberList T_RBRACKET ;
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

    // For the DOM, store the position of the T_LBRACKET upto the T_RBRACKET as the range:
    Property* prop = currentProperty();
    prop->listValueRange.offset = node->lbracketToken.offset;
    prop->listValueRange.length = node->rbracketToken.offset + node->rbracketToken.length - node->lbracketToken.offset;

    // Store the positions of the comma token too, again for the DOM to be able to retreive it.
    prop->listCommaPositions = collectCommas(node->members);

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
                error.setDescription(QCoreApplication::translate("QmlParser","Slot declarations must be parameterless"));
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
            error.setDescription(QCoreApplication::translate("QmlParser","QmlJS declaration outside Script element"));
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
: root(0), data(0)
{

}

QmlScriptParser::~QmlScriptParser()
{
    clear();
}

class QmlScriptParserJsASTData
{
public:
    QmlScriptParserJsASTData(const QString &filename)
        : nodePool(filename, &engine) {}

    Engine engine;
    NodePool nodePool;
};

bool QmlScriptParser::parse(const QByteArray &qmldata, const QUrl &url)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::QmlParsing> pt;
#endif
    clear();

    const QString fileName = url.toString();

    QTextStream stream(qmldata, QIODevice::ReadOnly);
    const QString code = stream.readAll();

    data = new QmlScriptParserJsASTData(fileName);

    Lexer lexer(&data->engine);
    lexer.setCode(code, /*line = */ 1);

    Parser parser(&data->engine);

    if (! parser.parse() || !_errors.isEmpty()) {

        // Extract errors from the parser
        foreach (const DiagnosticMessage &m, parser.diagnosticMessages()) {

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
    _automaticSemicolonOffsets.clear();

    if (data) {
        delete data;
        data = 0;
    }
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
