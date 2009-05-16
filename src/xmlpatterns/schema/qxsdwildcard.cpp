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

#include "qxsdwildcard_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

QString XsdWildcard::absentNamespace()
{
    return QLatin1String("__ns_absent");
}

void XsdWildcard::NamespaceConstraint::setVariety(Variety variety)
{
    m_variety = variety;
}

XsdWildcard::NamespaceConstraint::Variety XsdWildcard::NamespaceConstraint::variety() const
{
    return m_variety;
}

void XsdWildcard::NamespaceConstraint::setNamespaces(const QSet<QString> &namespaces)
{
    m_namespaces = namespaces;
}

QSet<QString> XsdWildcard::NamespaceConstraint::namespaces() const
{
    return m_namespaces;
}

void XsdWildcard::NamespaceConstraint::setDisallowedNames(const QSet<QString> &names)
{
    m_disallowedNames = names;
}

QSet<QString> XsdWildcard::NamespaceConstraint::disallowedNames() const
{
    return m_disallowedNames;
}

XsdWildcard::XsdWildcard()
    : m_namespaceConstraint(new NamespaceConstraint())
    , m_processContents(Strict)
{
    m_namespaceConstraint->setVariety(NamespaceConstraint::Any);
}

bool XsdWildcard::isWildcard() const
{
    return true;
}

void XsdWildcard::setNamespaceConstraint(const NamespaceConstraint::Ptr &namespaceConstraint)
{
    m_namespaceConstraint = namespaceConstraint;
}

XsdWildcard::NamespaceConstraint::Ptr XsdWildcard::namespaceConstraint() const
{
    return m_namespaceConstraint;
}

void XsdWildcard::setProcessContents(ProcessContents contents)
{
    m_processContents = contents;
}

XsdWildcard::ProcessContents XsdWildcard::processContents() const
{
    return m_processContents;
}

QT_END_NAMESPACE
