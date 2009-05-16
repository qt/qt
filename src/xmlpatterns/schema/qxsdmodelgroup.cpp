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

#include "qxsdmodelgroup_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

XsdModelGroup::XsdModelGroup()
    : m_compositor(SequenceCompositor)
{
}

bool XsdModelGroup::isModelGroup() const
{
    return true;
}

void XsdModelGroup::setCompositor(ModelCompositor compositor)
{
    m_compositor = compositor;
}

XsdModelGroup::ModelCompositor XsdModelGroup::compositor() const
{
    return m_compositor;
}

void XsdModelGroup::setParticles(const XsdParticle::List &particles)
{
    m_particles = particles;
}

XsdParticle::List XsdModelGroup::particles() const
{
    return m_particles;
}

QT_END_NAMESPACE
