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

#include "qxsddocumentation_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdDocumentation::XsdDocumentation()
{
}

XsdDocumentation::~XsdDocumentation()
{
}

void XsdDocumentation::setSource(const AnyURI::Ptr &source)
{
    m_source = source;
}

AnyURI::Ptr XsdDocumentation::source() const
{
    return m_source;
}

void XsdDocumentation::setLanguage(const DerivedString<TypeLanguage>::Ptr &language)
{
    m_language = language;
}

DerivedString<TypeLanguage>::Ptr XsdDocumentation::language() const
{
    return m_language;
}

void XsdDocumentation::setContent(const QString &content)
{
    m_content = content;
}

QString XsdDocumentation::content() const
{
    return m_content;
}

QT_END_NAMESPACE
