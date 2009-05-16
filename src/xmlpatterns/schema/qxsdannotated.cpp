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

#include "qxsdannotated_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

void XsdAnnotated::addAnnotation(const XsdAnnotation::Ptr &annotation)
{
    m_annotations.append(annotation);
}

void XsdAnnotated::addAnnotations(const XsdAnnotation::List &annotations)
{
    m_annotations << annotations;
}

XsdAnnotation::List XsdAnnotated::annotations() const
{
    return m_annotations;
}

QT_END_NAMESPACE
