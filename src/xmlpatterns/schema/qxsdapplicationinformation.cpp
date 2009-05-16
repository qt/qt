/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qxsdapplicationinformation_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdApplicationInformation::setSource(const AnyURI::Ptr &source)
{
    m_source = source;
}

AnyURI::Ptr XsdApplicationInformation::source() const
{
    return m_source;
}

void XsdApplicationInformation::setContent(const QString &content)
{
    m_content = content;
}

QString XsdApplicationInformation::content() const
{
    return m_content;
}

QT_END_NAMESPACE
