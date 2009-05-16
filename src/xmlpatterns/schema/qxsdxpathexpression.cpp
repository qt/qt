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

#include "qxsdxpathexpression_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdXPathExpression::setNamespaceBindings(const QList<QXmlName> &set)
{
    m_namespaceBindings = set;
}

QList<QXmlName> XsdXPathExpression::namespaceBindings() const
{
    return m_namespaceBindings;
}

void XsdXPathExpression::setDefaultNamespace(const AnyURI::Ptr &defaultNs)
{
    m_defaultNamespace = defaultNs;
}

AnyURI::Ptr XsdXPathExpression::defaultNamespace() const
{
    return m_defaultNamespace;
}

void XsdXPathExpression::setBaseURI(const AnyURI::Ptr &uri)
{
    m_baseURI = uri;
}

AnyURI::Ptr XsdXPathExpression::baseURI() const
{
    return m_baseURI;
}

void XsdXPathExpression::setExpression(const QString &expression)
{
    m_expression = expression;
}

QString XsdXPathExpression::expression() const
{
    return m_expression;
}

QT_END_NAMESPACE
