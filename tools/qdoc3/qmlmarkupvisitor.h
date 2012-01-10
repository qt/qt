/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "declarativeparser/qdeclarativejsastvisitor_p.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

class QmlMarkupVisitor : public QDeclarativeJS::AST::Visitor
{
public:
    enum ExtraType{
        Comment,
        Pragma
    };

    QmlMarkupVisitor(const QString &code,
                     const QList<QDeclarativeJS::AST::SourceLocation> &pragmas,
                     QDeclarativeJS::Engine *engine);
    virtual ~QmlMarkupVisitor();

    QString markedUpCode();

    virtual bool visit(QDeclarativeJS::AST::UiImport *);
    virtual void endVisit(QDeclarativeJS::AST::UiImport *);

    virtual bool visit(QDeclarativeJS::AST::UiPublicMember *);
    virtual bool visit(QDeclarativeJS::AST::UiObjectDefinition *);

    virtual bool visit(QDeclarativeJS::AST::UiObjectInitializer *);
    virtual void endVisit(QDeclarativeJS::AST::UiObjectInitializer *);

    virtual bool visit(QDeclarativeJS::AST::UiObjectBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiScriptBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiArrayBinding *);
    virtual bool visit(QDeclarativeJS::AST::UiArrayMemberList *);
    virtual bool visit(QDeclarativeJS::AST::UiQualifiedId *);

    virtual bool visit(QDeclarativeJS::AST::UiSignature *);
    virtual void endVisit(QDeclarativeJS::AST::UiSignature *);

    virtual bool visit(QDeclarativeJS::AST::UiFormal *);
    virtual bool visit(QDeclarativeJS::AST::ThisExpression *);
    virtual bool visit(QDeclarativeJS::AST::IdentifierExpression *);
    virtual bool visit(QDeclarativeJS::AST::NullExpression *);
    virtual bool visit(QDeclarativeJS::AST::TrueLiteral *);
    virtual bool visit(QDeclarativeJS::AST::FalseLiteral *);
    virtual bool visit(QDeclarativeJS::AST::NumericLiteral *);
    virtual bool visit(QDeclarativeJS::AST::StringLiteral *);
    virtual bool visit(QDeclarativeJS::AST::RegExpLiteral *);
    virtual bool visit(QDeclarativeJS::AST::ArrayLiteral *);

    virtual bool visit(QDeclarativeJS::AST::ObjectLiteral *);
    virtual void endVisit(QDeclarativeJS::AST::ObjectLiteral *);

    virtual bool visit(QDeclarativeJS::AST::ElementList *);
    virtual bool visit(QDeclarativeJS::AST::Elision *);
    virtual bool visit(QDeclarativeJS::AST::PropertyNameAndValueList *);
    virtual bool visit(QDeclarativeJS::AST::ArrayMemberExpression *);
    virtual bool visit(QDeclarativeJS::AST::FieldMemberExpression *);
    virtual bool visit(QDeclarativeJS::AST::NewMemberExpression *);
    virtual bool visit(QDeclarativeJS::AST::NewExpression *);
    virtual bool visit(QDeclarativeJS::AST::ArgumentList *);
    virtual bool visit(QDeclarativeJS::AST::PostIncrementExpression *);
    virtual bool visit(QDeclarativeJS::AST::PostDecrementExpression *);
    virtual bool visit(QDeclarativeJS::AST::DeleteExpression *);
    virtual bool visit(QDeclarativeJS::AST::VoidExpression *);
    virtual bool visit(QDeclarativeJS::AST::TypeOfExpression *);
    virtual bool visit(QDeclarativeJS::AST::PreIncrementExpression *);
    virtual bool visit(QDeclarativeJS::AST::PreDecrementExpression *);
    virtual bool visit(QDeclarativeJS::AST::UnaryPlusExpression *);
    virtual bool visit(QDeclarativeJS::AST::UnaryMinusExpression *);
    virtual bool visit(QDeclarativeJS::AST::TildeExpression *);
    virtual bool visit(QDeclarativeJS::AST::NotExpression *);
    virtual bool visit(QDeclarativeJS::AST::BinaryExpression *);
    virtual bool visit(QDeclarativeJS::AST::ConditionalExpression *);
    virtual bool visit(QDeclarativeJS::AST::Expression *);

    virtual bool visit(QDeclarativeJS::AST::Block *);
    virtual void endVisit(QDeclarativeJS::AST::Block *);

    virtual bool visit(QDeclarativeJS::AST::VariableStatement *);
    virtual bool visit(QDeclarativeJS::AST::VariableDeclarationList *);
    virtual bool visit(QDeclarativeJS::AST::VariableDeclaration *);
    virtual bool visit(QDeclarativeJS::AST::EmptyStatement *);
    virtual bool visit(QDeclarativeJS::AST::ExpressionStatement *);
    virtual bool visit(QDeclarativeJS::AST::IfStatement *);
    virtual bool visit(QDeclarativeJS::AST::DoWhileStatement *);
    virtual bool visit(QDeclarativeJS::AST::WhileStatement *);
    virtual bool visit(QDeclarativeJS::AST::ForStatement *);
    virtual bool visit(QDeclarativeJS::AST::LocalForStatement *);
    virtual bool visit(QDeclarativeJS::AST::ForEachStatement *);
    virtual bool visit(QDeclarativeJS::AST::LocalForEachStatement *);
    virtual bool visit(QDeclarativeJS::AST::ContinueStatement *);
    virtual bool visit(QDeclarativeJS::AST::BreakStatement *);
    virtual bool visit(QDeclarativeJS::AST::ReturnStatement *);
    virtual bool visit(QDeclarativeJS::AST::WithStatement *);

    virtual bool visit(QDeclarativeJS::AST::CaseBlock *);
    virtual void endVisit(QDeclarativeJS::AST::CaseBlock *);

    virtual bool visit(QDeclarativeJS::AST::SwitchStatement *);
    virtual bool visit(QDeclarativeJS::AST::CaseClause *);
    virtual bool visit(QDeclarativeJS::AST::DefaultClause *);
    virtual bool visit(QDeclarativeJS::AST::LabelledStatement *);
    virtual bool visit(QDeclarativeJS::AST::ThrowStatement *);
    virtual bool visit(QDeclarativeJS::AST::TryStatement *);
    virtual bool visit(QDeclarativeJS::AST::Catch *);
    virtual bool visit(QDeclarativeJS::AST::Finally *);
    virtual bool visit(QDeclarativeJS::AST::FunctionDeclaration *);
    virtual bool visit(QDeclarativeJS::AST::FunctionExpression *);
    virtual bool visit(QDeclarativeJS::AST::FormalParameterList *);
    virtual bool visit(QDeclarativeJS::AST::DebuggerStatement *);

protected:
    QString protect(const QString &string);

private:
    typedef QHash<QString, QString> StringHash;
    void addExtra(quint32 start, quint32 finish);
    void addMarkedUpToken(QDeclarativeJS::AST::SourceLocation &location,
                          const QString &text,
                          const StringHash &attributes = StringHash());
    void addVerbatim(QDeclarativeJS::AST::SourceLocation first,
                     QDeclarativeJS::AST::SourceLocation last = QDeclarativeJS::AST::SourceLocation());
    QString sourceText(QDeclarativeJS::AST::SourceLocation &location);

    QDeclarativeJS::Engine *engine;
    QList<ExtraType> extraTypes;
    QList<QDeclarativeJS::AST::SourceLocation> extraLocations;
    QString source;
    QString output;
    quint32 cursor;
    int extraIndex;
};

QT_END_NAMESPACE

#endif
