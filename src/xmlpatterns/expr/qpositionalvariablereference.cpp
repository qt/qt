/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qcommonsequencetypes_p.h"
#include "qinteger_p.h"

#include "qpositionalvariablereference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

PositionalVariableReference::PositionalVariableReference(const VariableSlotID s) : VariableReference(s)
{
}

Item PositionalVariableReference::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    Q_ASSERT(context->positionIterator(slot()));
    return Integer::fromValue(context->positionIterator(slot())->position());
}

bool PositionalVariableReference::evaluateEBV(const DynamicContext::Ptr &) const
{
    return true;
}

SequenceType::Ptr PositionalVariableReference::staticType() const
{
    return CommonSequenceTypes::ExactlyOneInteger;
}

Expression::Properties PositionalVariableReference::properties() const
{
    return DependsOnLocalVariable;
}

ExpressionVisitorResult::Ptr

PositionalVariableReference::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

QT_END_NAMESPACE
