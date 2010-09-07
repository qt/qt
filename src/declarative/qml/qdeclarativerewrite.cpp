/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativerewrite_p.h"

#include "private/qdeclarativeglobal_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(rewriteDump, QML_REWRITE_DUMP);

namespace QDeclarativeRewrite {

bool SharedBindingTester::isSharable(const QString &code)
{
    Engine engine;
    NodePool pool(QString(), &engine);
    Lexer lexer(&engine);
    Parser parser(&engine);
    lexer.setCode(code, 0);
    parser.parseStatement();
    if (!parser.statement()) 
        return false;

    _sharable = true;
    AST::Node::acceptChild(parser.statement(), this);
    return _sharable;
}

QString RewriteBinding::operator()(const QString &code, bool *ok)
{
    Engine engine;
    NodePool pool(QString(), &engine);
    Lexer lexer(&engine);
    Parser parser(&engine);
    lexer.setCode(code, 0);
    parser.parseStatement();
    if (!parser.statement()) {
        if (ok) *ok = false;
        return QString();
    } else {
        if (ok) *ok = true;
    }
    return rewrite(code, 0, parser.statement());
}

void RewriteBinding::accept(AST::Node *node)
{
    AST::Node::acceptChild(node, this);
}

QString RewriteBinding::rewrite(QString code, unsigned position,
                                AST::Statement *node)
{
    TextWriter w;
    _writer = &w;
    _position = position;
    _inLoop = 0;

    accept(node);

    unsigned startOfStatement = node->firstSourceLocation().begin() - _position;
    unsigned endOfStatement = node->lastSourceLocation().end() - _position;

    _writer->replace(startOfStatement, 0, QLatin1String("(function() { "));
    _writer->replace(endOfStatement, 0, QLatin1String(" })"));

    if (rewriteDump()) {
        qWarning() << "=============================================================";
        qWarning() << "Rewrote:";
        qWarning() << qPrintable(code);
    }

    w.write(&code);

    if (rewriteDump()) {
        qWarning() << "To:";
        qWarning() << qPrintable(code);
        qWarning() << "=============================================================";
    }

    return code;
}

bool RewriteBinding::visit(AST::Block *ast)
{
    for (AST::StatementList *it = ast->statements; it; it = it->next) {
        if (! it->next) {
            // we need to rewrite only the last statement of a block.
            accept(it->statement);
        }
    }

    return false;
}

bool RewriteBinding::visit(AST::ExpressionStatement *ast)
{
    if (! _inLoop) {
        unsigned startOfExpressionStatement = ast->firstSourceLocation().begin() - _position;
        _writer->replace(startOfExpressionStatement, 0, QLatin1String("return "));
    }

    return false;
}

bool RewriteBinding::visit(AST::DoWhileStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::DoWhileStatement *)
{
    --_inLoop;
}

bool RewriteBinding::visit(AST::WhileStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::WhileStatement *)
{
    --_inLoop;
}

bool RewriteBinding::visit(AST::ForStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::ForStatement *)
{
    --_inLoop;
}

bool RewriteBinding::visit(AST::LocalForStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::LocalForStatement *)
{
    --_inLoop;
}

bool RewriteBinding::visit(AST::ForEachStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::ForEachStatement *)
{
    --_inLoop;
}

bool RewriteBinding::visit(AST::LocalForEachStatement *)
{
    ++_inLoop;
    return true;
}

void RewriteBinding::endVisit(AST::LocalForEachStatement *)
{
    --_inLoop;
}

} // namespace QDeclarativeRewrite

QT_END_NAMESPACE
