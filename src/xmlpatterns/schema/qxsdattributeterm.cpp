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

#include "qxsdattributeterm_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

bool XsdAttributeTerm::isAttributeUse() const
{
    return false;
}

bool XsdAttributeTerm::isReference() const
{
    return false;
}

QT_END_NAMESPACE
