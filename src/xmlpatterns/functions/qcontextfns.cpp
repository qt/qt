/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qanyuri_p.h"
#include "qdate_p.h"
#include "qschemadatetime_p.h"
#include "qdaytimeduration_p.h"
#include "qinteger_p.h"
#include "qliteral_p.h"
#include "qatomicstring_p.h"
#include "qschematime_p.h"

#include "qcontextfns_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

Item PositionFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    return Integer::fromValue(context->contextPosition());
}

Item LastFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    Q_ASSERT(context);
    return Integer::fromValue(context->contextSize());
}

Item ImplicitTimezoneFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(context->implicitTimezone());
}

Item CurrentDateTimeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(DateTime::fromDateTime(context->currentDateTime()));
}

Item CurrentDateFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(Date::fromDateTime(context->currentDateTime()));
}

Item CurrentTimeFN::evaluateSingleton(const DynamicContext::Ptr &context) const
{
    return toItem(SchemaTime::fromDateTime(context->currentDateTime()));
}

Expression::Ptr StaticBaseURIFN::typeCheck(const StaticContext::Ptr &context,
                                           const SequenceType::Ptr &reqType)
{
    /* Our base URI can never be undefined. */
    return wrapLiteral(toItem(AnyURI::fromValue(context->baseURI())), context, this)->typeCheck(context, reqType);
}

Expression::Ptr DefaultCollationFN::typeCheck(const StaticContext::Ptr &context,
                                              const SequenceType::Ptr &reqType)
{
    return wrapLiteral(AtomicString::fromValue(context->defaultCollation().toString()), context, this)->typeCheck(context, reqType);
}

QT_END_NAMESPACE
