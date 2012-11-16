/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef RPPEXPRESSIONBUILDER_H
#define RPPEXPRESSIONBUILDER_H

#include "tokens.h"
#include "tokenengine.h"
#include "smallobject.h"
#include "rpp.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE

namespace Rpp {

class ExpressionBuilder
{
public:
    ExpressionBuilder(const TokenEngine::TokenList &tokenList, const QVector<Type> &typeList, TypedPool<Item> *memoryPool);
    Rpp::Expression *parse();
private:

    inline bool hasNext() const { return (i < m_tokenList.count()); }
    Type next();
    bool test(int);
    bool moreTokens(int delta);
    inline void prev() {--i;}
    Type lookup(int k = 1);
    inline Type token() { return typeAt(i-1);}
    inline QByteArray lexem() { return m_tokenList.text(i-1);}
    inline Type typeAt(int t) { return m_typeList.at(m_tokenList.containerIndex(t));}

    Expression *conditional_expression();
    Expression *logical_OR_expression();
    Expression *logical_AND_expression();
    Expression *inclusive_OR_expression();
    Expression *exclusive_OR_expression();
    Expression *AND_expression();
    Expression *equality_expression();
    Expression *relational_expression();
    Expression *shift_expression();
    Expression *additive_expression();
    Expression *multiplicative_expression();
    Expression *unary_expression();
    Expression *primary_expression();

    bool unary_expression_lookup();
    bool primary_expression_lookup();

    UnaryExpression *createUnaryExpression(int op, Expression *expression);
    BinaryExpression *createBinaryExpression(int op, Expression *leftExpresson, Expression *rightExpression);
    ConditionalExpression *createConditionalExpression(Expression *condition, Expression *leftExpression, Expression *rightExpression);
    MacroReference *createMacroReference(MacroReference::Type type, TokenEngine::TokenList token);
    IntLiteral *createIntLiteral(const int arg);

    TokenEngine::TokenList createTokenList(int tokenIndex) const;

    int i;
    TokenEngine::TokenList m_tokenList;
    QVector<Type> m_typeList;
    TypedPool<Item> *m_memoryPool;
};

}

QT_END_NAMESPACE

#endif
