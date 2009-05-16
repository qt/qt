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

#include "qxsdalternative_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdAlternative::setTest(const XsdXPathExpression::Ptr &test)
{
    m_test = test;
}

XsdXPathExpression::Ptr XsdAlternative::test() const
{
    return m_test;
}

void XsdAlternative::setType(const SchemaType::Ptr &type)
{
    m_type = type;
}

SchemaType::Ptr XsdAlternative::type() const
{
    return m_type;
}

QT_END_NAMESPACE
