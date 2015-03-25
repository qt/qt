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

#include "qdynamiccontext_p.h"
#include "qvalidationerror_p.h"
#include "qitem_p.h"

#include "qanyuri_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

AnyURI::AnyURI(const QString &s) : AtomicString(s)
{
}

AnyURI::Ptr AnyURI::fromValue(const QString &value)
{
    return AnyURI::Ptr(new AnyURI(value));
}

AnyURI::Ptr AnyURI::fromValue(const QUrl &uri)
{
    return AnyURI::Ptr(new AnyURI(uri.toString()));
}

AnyURI::Ptr AnyURI::resolveURI(const QString &relative,
                               const QString &base)
{
    const QUrl urlBase(base);
    return AnyURI::fromValue(urlBase.resolved(relative).toString());
}

ItemType::Ptr AnyURI::type() const
{
    return BuiltinTypes::xsAnyURI;
}

AnyURI::Ptr AnyURI::fromLexical(const QString &value)
{
    bool isValid;
    /* The error code doesn't matter, because we never raise error. */
    const QUrl retval(toQUrl<ReportContext::FORG0001>(value,
                                                      DynamicContext::Ptr(),
                                                      0,
                                                      &isValid,
                                                      false));
    if(isValid)
        return fromValue(retval);
    else
        return ValidationError::createError();
}

bool AnyURI::isValid(const QString &candidate)
{
    bool isOk = false;
    /* The error code doesn't matter, because we never raise error. */
    toQUrl<ReportContext::FORG0001>(candidate,
                                    ReportContext::Ptr(),
                                    0,
                                    &isOk,
                                    false);
    return isOk;
}
QT_END_NAMESPACE
