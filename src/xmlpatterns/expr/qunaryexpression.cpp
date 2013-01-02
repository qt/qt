/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qarithmeticexpression_p.h"
#include "qcommonvalues_p.h"
#include "qliteral_p.h"
#include "qschemanumeric_p.h"

#include "qunaryexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

UnaryExpression::UnaryExpression(const AtomicMathematician::Operator op,
                                 const Expression::Ptr &operand,
                                 const StaticContext::Ptr &context) : ArithmeticExpression(wrapLiteral(CommonValues::IntegerZero, context, operand.data()),
                                                                                           op,
                                                                                           operand)
{
    Q_ASSERT(op == AtomicMathematician::Substract ||
             op == AtomicMathematician::Add);
    Q_ASSERT(context);
}

Item UnaryExpression::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    if(operatorID() == AtomicMathematician::Substract)
    {
        const Item item(m_operand2->evaluateSingleton(context));

        if(item)
            return item.as<Numeric>()->toNegated();
        else
            return Item();
    }
    else
        return m_operand2->evaluateSingleton(context);
}

QT_END_NAMESPACE
