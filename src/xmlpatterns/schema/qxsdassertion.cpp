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

#include "qxsdassertion_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdAssertion::setTest(const XsdXPathExpression::Ptr &test)
{
    m_test = test;
}

XsdXPathExpression::Ptr XsdAssertion::test() const
{
    return m_test;
}

QT_END_NAMESPACE
