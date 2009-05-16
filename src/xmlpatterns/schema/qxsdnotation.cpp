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

#include "qxsdnotation_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdNotation::setPublicId(const DerivedString<TypeToken>::Ptr &id)
{
    m_publicId = id;
}

DerivedString<TypeToken>::Ptr XsdNotation::publicId() const
{
    return m_publicId;
}

void XsdNotation::setSystemId(const AnyURI::Ptr &id)
{
    m_systemId = id;
}

AnyURI::Ptr XsdNotation::systemId() const
{
    return m_systemId;
}

QT_END_NAMESPACE
