/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEV4IRBUILDER_P_H
#define QDECLARATIVEV4IRBUILDER_P_H

#include <QtCore/qglobal.h>

#include "qdeclarativev4ir_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDeclarativeV4IRBuilder : public QDeclarativeJS::AST::Visitor
{
public:
    QDeclarativeV4IRBuilder(const QDeclarativeV4Compiler::Expression *, QDeclarativeEnginePrivate *);

    QDeclarativeJS::IR::Function *operator()(QDeclarativeJS::IR::Module *, QDeclarativeJS::AST::Node *);

protected:
    struct ExprResult {
        enum Format {
            ex, // expression
            cx  // condition
        };

        QDeclarativeJS::IR::Expr *code;
        QDeclarativeJS::IR::BasicBlock *iftrue;
        QDeclarativeJS::IR::BasicBlock *iffalse;
        Format hint; // requested format
        Format format; // instruction format

        ExprResult(QDeclarativeJS::IR::Expr *expr = 0)
            : code(expr), iftrue(0), iffalse(0), hint(ex), format(ex) {}

        ExprResult(QDeclarativeJS::IR::BasicBlock *iftrue, QDeclarativeJS::IR::BasicBlock *iffalse)
            : code(0), iftrue(iftrue), iffalse(iffalse), hint(cx), format(ex) {}

        inline QDeclarativeJS::IR::Type type() const { return code ? code->type : QDeclarativeJS::IR::InvalidType; }

        inline QDeclarativeJS::IR::Expr *get() const { return code; }
        inline operator QDeclarativeJS::IR::Expr *() const  { return get(); }
        inline QDeclarativeJS::IR::Expr *operator->() const { return get(); }
        inline bool isValid() const { return code ? code->type != QDeclarativeJS::IR::InvalidType : false; }
        inline bool is(QDeclarativeJS::IR::Type t) const { return type() == t; }
        inline bool isNot(QDeclarativeJS::IR::Type t) const { return type() != t; }

        bool isPrimitive() const {
            switch (type()) {
            case QDeclarativeJS::IR::UndefinedType: // ### TODO
            case QDeclarativeJS::IR::NullType: // ### TODO
            case QDeclarativeJS::IR::UrlType: // ### TODO
                return false;

            case QDeclarativeJS::IR::StringType:
            case QDeclarativeJS::IR::BoolType:
            case QDeclarativeJS::IR::IntType:
            case QDeclarativeJS::IR::RealType:
            case QDeclarativeJS::IR::RealNaNType:
                return true;

            default:
                return false;
            } // switch
        }
    };

    inline void accept(QDeclarativeJS::AST::Node *ast) { QDeclarativeJS::AST::Node::accept(ast, this); }

    ExprResult expression(QDeclarativeJS::AST::ExpressionNode *ast);
    ExprResult statement(QDeclarativeJS::AST::Statement *ast);
    void sourceElement(QDeclarativeJS::AST::SourceElement *ast);
    void condition(QDeclarativeJS::AST::ExpressionNode *ast, QDeclarativeJS::IR::BasicBlock *iftrue, QDeclarativeJS::IR::BasicBlock *iffalse);
    void binop(QDeclarativeJS::AST::BinaryExpression *ast, ExprResult left, ExprResult right);

    void implicitCvt(ExprResult &expr, QDeclarativeJS::IR::Type type);

    // QML
    virtual bool visit(QDeclarativeJS::AST::UiProgram *ast);
    virtual bool visit(QDeclarativeJS::AST::UiImportList *ast);
    virtual bool visit(QDeclarativeJS::AST::UiImport *ast);
    virtual bool visit(QDeclarativeJS::AST::UiPublicMember *ast);
    virtual bool visit(QDeclarativeJS::AST::UiSourceElement *ast);
    virtual bool visit(QDeclarativeJS::AST::UiObjectDefinition *ast);
    virtual bool visit(QDeclarativeJS::AST::UiObjectInitializer *ast);
    virtual bool visit(QDeclarativeJS::AST::UiObjectBinding *ast);
    virtual bool visit(QDeclarativeJS::AST::UiScriptBinding *ast);
    virtual bool visit(QDeclarativeJS::AST::UiArrayBinding *ast);
    virtual bool visit(QDeclarativeJS::AST::UiObjectMemberList *ast);
    virtual bool visit(QDeclarativeJS::AST::UiArrayMemberList *ast);
    virtual bool visit(QDeclarativeJS::AST::UiQualifiedId *ast);
    virtual bool visit(QDeclarativeJS::AST::UiSignature *ast);
    virtual bool visit(QDeclarativeJS::AST::UiFormalList *ast);
    virtual bool visit(QDeclarativeJS::AST::UiFormal *ast);

    // JS
    virtual bool visit(QDeclarativeJS::AST::Program *ast);
    virtual bool visit(QDeclarativeJS::AST::SourceElements *ast);
    virtual bool visit(QDeclarativeJS::AST::FunctionSourceElement *ast);
    virtual bool visit(QDeclarativeJS::AST::StatementSourceElement *ast);

    // object literals
    virtual bool visit(QDeclarativeJS::AST::PropertyNameAndValueList *ast);
    virtual bool visit(QDeclarativeJS::AST::IdentifierPropertyName *ast);
    virtual bool visit(QDeclarativeJS::AST::StringLiteralPropertyName *ast);
    virtual bool visit(QDeclarativeJS::AST::NumericLiteralPropertyName *ast);

    // array literals
    virtual bool visit(QDeclarativeJS::AST::ElementList *ast);
    virtual bool visit(QDeclarativeJS::AST::Elision *ast);

    // function calls
    virtual bool visit(QDeclarativeJS::AST::ArgumentList *ast);

    // expressions
    virtual bool visit(QDeclarativeJS::AST::ObjectLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::ArrayLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::ThisExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::IdentifierExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::NullExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::TrueLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::FalseLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::StringLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::NumericLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::RegExpLiteral *ast);
    virtual bool visit(QDeclarativeJS::AST::NestedExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::ArrayMemberExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::FieldMemberExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::NewMemberExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::NewExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::CallExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::PostIncrementExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::PostDecrementExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::DeleteExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::VoidExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::TypeOfExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::PreIncrementExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::PreDecrementExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::UnaryPlusExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::UnaryMinusExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::TildeExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::NotExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::BinaryExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::ConditionalExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::Expression *ast);

    // statements
    virtual bool visit(QDeclarativeJS::AST::Block *ast);
    virtual bool visit(QDeclarativeJS::AST::StatementList *ast);
    virtual bool visit(QDeclarativeJS::AST::VariableStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::VariableDeclarationList *ast);
    virtual bool visit(QDeclarativeJS::AST::VariableDeclaration *ast);
    virtual bool visit(QDeclarativeJS::AST::EmptyStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ExpressionStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::IfStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::DoWhileStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::WhileStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ForStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::LocalForStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ForEachStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::LocalForEachStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ContinueStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::BreakStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ReturnStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::WithStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::SwitchStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::CaseBlock *ast);
    virtual bool visit(QDeclarativeJS::AST::CaseClauses *ast);
    virtual bool visit(QDeclarativeJS::AST::CaseClause *ast);
    virtual bool visit(QDeclarativeJS::AST::DefaultClause *ast);
    virtual bool visit(QDeclarativeJS::AST::LabelledStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::ThrowStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::TryStatement *ast);
    virtual bool visit(QDeclarativeJS::AST::Catch *ast);
    virtual bool visit(QDeclarativeJS::AST::Finally *ast);
    virtual bool visit(QDeclarativeJS::AST::FunctionDeclaration *ast);
    virtual bool visit(QDeclarativeJS::AST::FunctionExpression *ast);
    virtual bool visit(QDeclarativeJS::AST::FormalParameterList *ast);
    virtual bool visit(QDeclarativeJS::AST::FunctionBody *ast);
    virtual bool visit(QDeclarativeJS::AST::DebuggerStatement *ast);

private:
    bool buildName(QStringList &name, QDeclarativeJS::AST::Node *node, 
                   QList<QDeclarativeJS::AST::ExpressionNode *> *nodes);
    void discard();

    const QDeclarativeV4Compiler::Expression *m_expression;
    QDeclarativeEnginePrivate *m_engine;

    QDeclarativeJS::IR::Module *_module;
    QDeclarativeJS::IR::Function *_function;
    QDeclarativeJS::IR::BasicBlock *_block;
    bool _discard;

    ExprResult _expr;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEV4IRBUILDER_P_H 
