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

#include "qxsdannotation_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdAnnotation::setId(const DerivedString<TypeID>::Ptr &id)
{
    m_id = id;
}

DerivedString<TypeID>::Ptr XsdAnnotation::id() const
{
    return m_id;
}

void XsdAnnotation::addApplicationInformation(const XsdApplicationInformation::Ptr &information)
{
    m_applicationInformation.append(information);
}

XsdApplicationInformation::List XsdAnnotation::applicationInformation() const
{
    return m_applicationInformation;
}

void XsdAnnotation::addDocumentation(const XsdDocumentation::Ptr &documentation)
{
    m_documentations.append(documentation);
}

XsdDocumentation::List XsdAnnotation::documentation() const
{
    return m_documentations;
}

QT_END_NAMESPACE
