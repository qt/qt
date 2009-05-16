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

#include "qxsdattributegroup_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdAttributeGroup::setAttributeUses(const XsdAttributeUse::List &attributeUses)
{
    m_attributeUses = attributeUses;
}

void XsdAttributeGroup::addAttributeUse(const XsdAttributeUse::Ptr &attributeUse)
{
    m_attributeUses.append(attributeUse);
}

XsdAttributeUse::List XsdAttributeGroup::attributeUses() const
{
    return m_attributeUses;
}

void XsdAttributeGroup::setWildcard(const XsdWildcard::Ptr &wildcard)
{
    m_wildcard = wildcard;
}

XsdWildcard::Ptr XsdAttributeGroup::wildcard() const
{
    return m_wildcard;
}

QT_END_NAMESPACE
