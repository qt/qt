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

#include "qnamedschemacomponent_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

NamedSchemaComponent::NamedSchemaComponent()
{
}

NamedSchemaComponent::~NamedSchemaComponent()
{
}

void NamedSchemaComponent::setName(const QXmlName &name)
{
    m_name = name;
}

QXmlName NamedSchemaComponent::name(const NamePool::Ptr&) const
{
    return m_name;
}

QString NamedSchemaComponent::displayName(const NamePool::Ptr &np) const
{
    return np->displayName(m_name);
}

QT_END_NAMESPACE
