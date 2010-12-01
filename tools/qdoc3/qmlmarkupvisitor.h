/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef QMLVISITOR_H
#define QMLVISITOR_H

#include <QString>
#include "private/qdeclarativejsastvisitor_p.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

class QmlMarkupVisitor : public QDeclarativeJS::AST::Visitor
{
public:
    QmlMarkupVisitor(const QString &code, QDeclarativeJS::Engine *engine);
    virtual ~QmlMarkupVisitor();

    QString markedUpCode();

    virtual bool visit(QDeclarativeJS::AST::UiProgram *);
    virtual bool visit(QDeclarativeJS::AST::UiImportList *);
    virtual bool visit(QDeclarativeJS::AST::UiImport *);
    virtual bool visit(QDeclarativeJS::AST::UiPublicMember *);
    virtual bool visit(QDeclarativeJS::AST::UiSourceElement *);
    virtual bool visit(QDeclarativeJS::AST::UiObjectDefinition *);
    virtual bool visit(QDeclarativeJS::AST::UiObjectInitializer *);
    virtual bool visit(QDeclarativeJS::AST::UiObjectBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiScriptBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiArrayBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiObjectMemberList *);
    virtual bool visit(QDeclarativeJS::AST::UiArrayMemberList *);
    virtual bool visit(QDeclarativeJS::AST::UiQualifiedId *);
    virtual bool visit(QDeclarativeJS::AST::UiSignature *);
    virtual bool visit(QDeclarativeJS::AST::UiFormalList *);
    virtual bool visit(QDeclarativeJS::AST::UiFormal *);

    virtual void endVisit(QDeclarativeJS::AST::UiProgram *);
    virtual void endVisit(QDeclarativeJS::AST::UiImportList *);
    virtual void endVisit(QDeclarativeJS::AST::UiImport *);
    virtual void endVisit(QDeclarativeJS::AST::UiPublicMember *);
    virtual void endVisit(QDeclarativeJS::AST::UiSourceElement *);
    virtual void endVisit(QDeclarativeJS::AST::UiObjectDefinition *);
    virtual void endVisit(QDeclarativeJS::AST::UiObjectInitializer *);
    virtual void endVisit(QDeclarativeJS::AST::UiObjectBinding *);
    virtual void endVisit(QDeclarativeJS::AST::UiScriptBinding *);
    virtual void endVisit(QDeclarativeJS::AST::UiArrayBinding *);
    virtual void endVisit(QDeclarativeJS::AST::UiObjectMemberList *);
    virtual void endVisit(QDeclarativeJS::AST::UiArrayMemberList *);
    virtual void endVisit(QDeclarativeJS::AST::UiQualifiedId *);
    virtual void endVisit(QDeclarativeJS::AST::UiSignature *);
    virtual void endVisit(QDeclarativeJS::AST::UiFormalList *);
    virtual void endVisit(QDeclarativeJS::AST::UiFormal *);

    // QDeclarativeJS
    virtual bool visit(QDeclarativeJS::AST::ThisExpression *);
    virtual void endVisit(QDeclarativeJS::AST::ThisExpression *);

    virtual bool visit(QDeclarativeJS::AST::IdentifierExpression *);
    virtual void endVisit(QDeclarativeJS::AST::IdentifierExpression *);

    virtual bool visit(QDeclarativeJS::AST::NullExpression *);
    virtual void endVisit(QDeclarativeJS::AST::NullExpression *);

    virtual bool visit(QDeclarativeJS::AST::TrueLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::TrueLiteral *);

    virtual bool visit(QDeclarativeJS::AST::FalseLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::FalseLiteral *);

    virtual bool visit(QDeclarativeJS::AST::NumericLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::NumericLiteral *);

    virtual bool visit(QDeclarativeJS::AST::StringLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::StringLiteral *);

    virtual bool visit(QDeclarativeJS::AST::RegExpLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::RegExpLiteral *);

    virtual bool visit(QDeclarativeJS::AST::ArrayLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::ArrayLiteral *);

    virtual bool visit(QDeclarativeJS::AST::ObjectLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::ObjectLiteral *);

    virtual bool visit(QDeclarativeJS::AST::ElementList *);
    virtual void endVisit(QDeclarativeJS::AST::ElementList *);

    virtual bool visit(QDeclarativeJS::AST::Elision *);
    virtual void endVisit(QDeclarativeJS::AST::Elision *);

    virtual bool visit(QDeclarativeJS::AST::PropertyNameAndValueList *);
    virtual void endVisit(QDeclarativeJS::AST::PropertyNameAndValueList *);

    virtual bool visit(QDeclarativeJS::AST::NestedExpression *);
    virtual void endVisit(QDeclarativeJS::AST::NestedExpression *);

    virtual bool visit(QDeclarativeJS::AST::IdentifierPropertyName *);
    virtual void endVisit(QDeclarativeJS::AST::IdentifierPropertyName *);

    virtual bool visit(QDeclarativeJS::AST::StringLiteralPropertyName *);
    virtual void endVisit(QDeclarativeJS::AST::StringLiteralPropertyName *);

    virtual bool visit(QDeclarativeJS::AST::NumericLiteralPropertyName *);
    virtual void endVisit(QDeclarativeJS::AST::NumericLiteralPropertyName *);

    virtual bool visit(QDeclarativeJS::AST::ArrayMemberExpression *);
    virtual void endVisit(QDeclarativeJS::AST::ArrayMemberExpression *);

    virtual bool visit(QDeclarativeJS::AST::FieldMemberExpression *);
    virtual void endVisit(QDeclarativeJS::AST::FieldMemberExpression *);

    virtual bool visit(QDeclarativeJS::AST::NewMemberExpression *);
    virtual void endVisit(QDeclarativeJS::AST::NewMemberExpression *);

    virtual bool visit(QDeclarativeJS::AST::NewExpression *);
    virtual void endVisit(QDeclarativeJS::AST::NewExpression *);

    virtual bool visit(QDeclarativeJS::AST::CallExpression *);
    virtual void endVisit(QDeclarativeJS::AST::CallExpression *);

    virtual bool visit(QDeclarativeJS::AST::ArgumentList *);
    virtual void endVisit(QDeclarativeJS::AST::ArgumentList *);

    virtual bool visit(QDeclarativeJS::AST::PostIncrementExpression *);
    virtual void endVisit(QDeclarativeJS::AST::PostIncrementExpression *);

    virtual bool visit(QDeclarativeJS::AST::PostDecrementExpression *);
    virtual void endVisit(QDeclarativeJS::AST::PostDecrementExpression *);

    virtual bool visit(QDeclarativeJS::AST::DeleteExpression *);
    virtual void endVisit(QDeclarativeJS::AST::DeleteExpression *);

    virtual bool visit(QDeclarativeJS::AST::VoidExpression *);
    virtual void endVisit(QDeclarativeJS::AST::VoidExpression *);

    virtual bool visit(QDeclarativeJS::AST::TypeOfExpression *);
    virtual void endVisit(QDeclarativeJS::AST::TypeOfExpression *);

    virtual bool visit(QDeclarativeJS::AST::PreIncrementExpression *);
    virtual void endVisit(QDeclarativeJS::AST::PreIncrementExpression *);

    virtual bool visit(QDeclarativeJS::AST::PreDecrementExpression *);
    virtual void endVisit(QDeclarativeJS::AST::PreDecrementExpression *);

    virtual bool visit(QDeclarativeJS::AST::UnaryPlusExpression *);
    virtual void endVisit(QDeclarativeJS::AST::UnaryPlusExpression *);

    virtual bool visit(QDeclarativeJS::AST::UnaryMinusExpression *);
    virtual void endVisit(QDeclarativeJS::AST::UnaryMinusExpression *);

    virtual bool visit(QDeclarativeJS::AST::TildeExpression *);
    virtual void endVisit(QDeclarativeJS::AST::TildeExpression *);

    virtual bool visit(QDeclarativeJS::AST::NotExpression *);
    virtual void endVisit(QDeclarativeJS::AST::NotExpression *);

    virtual bool visit(QDeclarativeJS::AST::BinaryExpression *);
    virtual void endVisit(QDeclarativeJS::AST::BinaryExpression *);

    virtual bool visit(QDeclarativeJS::AST::ConditionalExpression *);
    virtual void endVisit(QDeclarativeJS::AST::ConditionalExpression *);

    virtual bool visit(QDeclarativeJS::AST::Expression *);
    virtual void endVisit(QDeclarativeJS::AST::Expression *);

    virtual bool visit(QDeclarativeJS::AST::Block *);
    virtual void endVisit(QDeclarativeJS::AST::Block *);

    virtual bool visit(QDeclarativeJS::AST::StatementList *);
    virtual void endVisit(QDeclarativeJS::AST::StatementList *);

    virtual bool visit(QDeclarativeJS::AST::VariableStatement *);
    virtual void endVisit(QDeclarativeJS::AST::VariableStatement *);

    virtual bool visit(QDeclarativeJS::AST::VariableDeclarationList *);
    virtual void endVisit(QDeclarativeJS::AST::VariableDeclarationList *);

    virtual bool visit(QDeclarativeJS::AST::VariableDeclaration *);
    virtual void endVisit(QDeclarativeJS::AST::VariableDeclaration *);

    virtual bool visit(QDeclarativeJS::AST::EmptyStatement *);
    virtual void endVisit(QDeclarativeJS::AST::EmptyStatement *);

    virtual bool visit(QDeclarativeJS::AST::ExpressionStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ExpressionStatement *);

    virtual bool visit(QDeclarativeJS::AST::IfStatement *);
    virtual void endVisit(QDeclarativeJS::AST::IfStatement *);

    virtual bool visit(QDeclarativeJS::AST::DoWhileStatement *);
    virtual void endVisit(QDeclarativeJS::AST::DoWhileStatement *);

    virtual bool visit(QDeclarativeJS::AST::WhileStatement *);
    virtual void endVisit(QDeclarativeJS::AST::WhileStatement *);

    virtual bool visit(QDeclarativeJS::AST::ForStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ForStatement *);

    virtual bool visit(QDeclarativeJS::AST::LocalForStatement *);
    virtual void endVisit(QDeclarativeJS::AST::LocalForStatement *);

    virtual bool visit(QDeclarativeJS::AST::ForEachStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ForEachStatement *);

    virtual bool visit(QDeclarativeJS::AST::LocalForEachStatement *);
    virtual void endVisit(QDeclarativeJS::AST::LocalForEachStatement *);

    virtual bool visit(QDeclarativeJS::AST::ContinueStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ContinueStatement *);

    virtual bool visit(QDeclarativeJS::AST::BreakStatement *);
    virtual void endVisit(QDeclarativeJS::AST::BreakStatement *);

    virtual bool visit(QDeclarativeJS::AST::ReturnStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ReturnStatement *);

    virtual bool visit(QDeclarativeJS::AST::WithStatement *);
    virtual void endVisit(QDeclarativeJS::AST::WithStatement *);

    virtual bool visit(QDeclarativeJS::AST::CaseBlock *);
    virtual void endVisit(QDeclarativeJS::AST::CaseBlock *);

    virtual bool visit(QDeclarativeJS::AST::SwitchStatement *);
    virtual void endVisit(QDeclarativeJS::AST::SwitchStatement *);

    virtual bool visit(QDeclarativeJS::AST::CaseClauses *);
    virtual void endVisit(QDeclarativeJS::AST::CaseClauses *);

    virtual bool visit(QDeclarativeJS::AST::CaseClause *);
    virtual void endVisit(QDeclarativeJS::AST::CaseClause *);

    virtual bool visit(QDeclarativeJS::AST::DefaultClause *);
    virtual void endVisit(QDeclarativeJS::AST::DefaultClause *);

    virtual bool visit(QDeclarativeJS::AST::LabelledStatement *);
    virtual void endVisit(QDeclarativeJS::AST::LabelledStatement *);

    virtual bool visit(QDeclarativeJS::AST::ThrowStatement *);
    virtual void endVisit(QDeclarativeJS::AST::ThrowStatement *);

    virtual bool visit(QDeclarativeJS::AST::TryStatement *);
    virtual void endVisit(QDeclarativeJS::AST::TryStatement *);

    virtual bool visit(QDeclarativeJS::AST::Catch *);
    virtual void endVisit(QDeclarativeJS::AST::Catch *);

    virtual bool visit(QDeclarativeJS::AST::Finally *);
    virtual void endVisit(QDeclarativeJS::AST::Finally *);

    virtual bool visit(QDeclarativeJS::AST::FunctionDeclaration *);
    virtual void endVisit(QDeclarativeJS::AST::FunctionDeclaration *);

    virtual bool visit(QDeclarativeJS::AST::FunctionExpression *);
    virtual void endVisit(QDeclarativeJS::AST::FunctionExpression *);

    virtual bool visit(QDeclarativeJS::AST::FormalParameterList *);
    virtual void endVisit(QDeclarativeJS::AST::FormalParameterList *);

    virtual bool visit(QDeclarativeJS::AST::FunctionBody *);
    virtual void endVisit(QDeclarativeJS::AST::FunctionBody *);

    virtual bool visit(QDeclarativeJS::AST::Program *);
    virtual void endVisit(QDeclarativeJS::AST::Program *);

    virtual bool visit(QDeclarativeJS::AST::SourceElements *);
    virtual void endVisit(QDeclarativeJS::AST::SourceElements *);

    virtual bool visit(QDeclarativeJS::AST::FunctionSourceElement *);
    virtual void endVisit(QDeclarativeJS::AST::FunctionSourceElement *);

    virtual bool visit(QDeclarativeJS::AST::StatementSourceElement *);
    virtual void endVisit(QDeclarativeJS::AST::StatementSourceElement *);

    virtual bool visit(QDeclarativeJS::AST::DebuggerStatement *);
    virtual void endVisit(QDeclarativeJS::AST::DebuggerStatement *);

    virtual bool visit(QDeclarativeJS::AST::UiParameterList *);
    virtual void endVisit(QDeclarativeJS::AST::UiParameterList *);

protected:
    QString protect(const QString &string);

private:
    void addExtra(quint32 start, quint32 finish);
    void addMarkedUpToken(QDeclarativeJS::AST::SourceLocation &location,
                          const QString &text);
    void addVerbatim(QDeclarativeJS::AST::SourceLocation first,
                     QDeclarativeJS::AST::SourceLocation last = QDeclarativeJS::AST::SourceLocation());
    QString sourceText(QDeclarativeJS::AST::SourceLocation &location);

    QDeclarativeJS::Engine *engine;
    QString source;
    QString output;
    quint32 cursor;
    quint32 commentIndex;
    int indent;
    QString debug;
};

QT_END_NAMESPACE

#endif
