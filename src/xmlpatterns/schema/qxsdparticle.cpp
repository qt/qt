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

#include "qxsdparticle_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdParticle::XsdParticle()
    : m_minimumOccurs(1)
    , m_maximumOccurs(1)
    , m_maximumOccursUnbounded(false)
{
}

void XsdParticle::setMinimumOccurs(unsigned int occurs)
{
    m_minimumOccurs = occurs;
}

unsigned int XsdParticle::minimumOccurs() const
{
    return m_minimumOccurs;
}

void XsdParticle::setMaximumOccurs(unsigned int occurs)
{
    m_maximumOccurs = occurs;
}

unsigned int XsdParticle::maximumOccurs() const
{
    return m_maximumOccurs;
}

void XsdParticle::setMaximumOccursUnbounded(bool unbounded)
{
    m_maximumOccursUnbounded = unbounded;
}

bool XsdParticle::maximumOccursUnbounded() const
{
    return m_maximumOccursUnbounded;
}

void XsdParticle::setTerm(const XsdTerm::Ptr &term)
{
    m_term = term;
}

XsdTerm::Ptr XsdParticle::term() const
{
    return m_term;
}

QT_END_NAMESPACE
