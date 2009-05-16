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

#include "qxsdidentityconstraint_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdIdentityConstraint::setCategory(Category category)
{
    m_category = category;
}

XsdIdentityConstraint::Category XsdIdentityConstraint::category() const
{
    return m_category;
}

void XsdIdentityConstraint::setSelector(const XsdXPathExpression::Ptr &selector)
{
    m_selector = selector;
}

XsdXPathExpression::Ptr XsdIdentityConstraint::selector() const
{
    return m_selector;
}

void XsdIdentityConstraint::setFields(const XsdXPathExpression::List &fields)
{
    m_fields = fields;
}

void XsdIdentityConstraint::addField(const XsdXPathExpression::Ptr &field)
{
    m_fields.append(field);
}

XsdXPathExpression::List XsdIdentityConstraint::fields() const
{
    return m_fields;
}

void XsdIdentityConstraint::setReferencedKey(const XsdIdentityConstraint::Ptr &referencedKey)
{
    m_referencedKey = referencedKey;
}

XsdIdentityConstraint::Ptr XsdIdentityConstraint::referencedKey() const
{
    return m_referencedKey;
}

QT_END_NAMESPACE
