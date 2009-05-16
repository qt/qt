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

#include "qxsdattributereference_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool XsdAttributeReference::isAttributeUse() const
{
    return false;
}

bool XsdAttributeReference::isReference() const
{
    return true;
}

void XsdAttributeReference::setType(Type type)
{
    m_type = type;
}

XsdAttributeReference::Type XsdAttributeReference::type() const
{
    return m_type;
}

void XsdAttributeReference::setReferenceName(const QXmlName &referenceName)
{
    m_referenceName = referenceName;
}

QXmlName XsdAttributeReference::referenceName() const
{
    return m_referenceName;
}

void XsdAttributeReference::setSourceLocation(const QSourceLocation &location)
{
    m_sourceLocation = location;
}

QSourceLocation XsdAttributeReference::sourceLocation() const
{
    return m_sourceLocation;
}

QT_END_NAMESPACE
