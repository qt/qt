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

#include "qxsdreference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool XsdReference::isReference() const
{
    return true;
}

void XsdReference::setType(Type type)
{
    m_type = type;
}

XsdReference::Type XsdReference::type() const
{
    return m_type;
}

void XsdReference::setReferenceName(const QXmlName &referenceName)
{
    m_referenceName = referenceName;
}

QXmlName XsdReference::referenceName() const
{
    return m_referenceName;
}

void XsdReference::setSourceLocation(const QSourceLocation &location)
{
    m_sourceLocation = location;
}

QSourceLocation XsdReference::sourceLocation() const
{
    return m_sourceLocation;
}

QT_END_NAMESPACE
