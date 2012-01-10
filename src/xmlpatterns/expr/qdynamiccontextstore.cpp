/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qboolean_p.h"
#include "qcommonsequencetypes_p.h"
#include "qcommonvalues_p.h"

#include "qdynamiccontextstore_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

DynamicContextStore::DynamicContextStore(const Expression::Ptr &operand,
                                         const DynamicContext::Ptr &context) : SingleContainer(operand),
                                                                               m_context(context.data())
{
    Q_ASSERT(context);
}

bool DynamicContextStore::evaluateEBV(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateEBV(DynamicContext::Ptr(m_context));
}

Item::Iterator::Ptr DynamicContextStore::evaluateSequence(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSequence(DynamicContext::Ptr(m_context));
}

Item DynamicContextStore::evaluateSingleton(const DynamicContext::Ptr &) const
{
    return m_operand->evaluateSingleton(DynamicContext::Ptr(m_context));
}

SequenceType::Ptr DynamicContextStore::staticType() const
{
    return m_operand->staticType();
}

SequenceType::List DynamicContextStore::expectedOperandTypes() const
{
    SequenceType::List result;
    result.append(CommonSequenceTypes::ZeroOrMoreItems);
    return result;
}

ExpressionVisitorResult::Ptr DynamicContextStore::accept(const ExpressionVisitor::Ptr &visitor) const
{
    return visitor->visit(this);
}

const SourceLocationReflection *DynamicContextStore::actualReflection() const
{
    return m_operand->actualReflection();
}

QT_END_NAMESPACE
