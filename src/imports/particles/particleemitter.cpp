/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "particleemitter.h"
QT_BEGIN_NAMESPACE
ParticleEmitter::ParticleEmitter(QSGItem *parent) :
    QSGItem(parent)
  , m_particlesPerSecond(10)
  , m_particleDuration(1000)
  , m_particleDurationVariation(0)
  , m_emitting(true)
  , m_system(0)
  , m_extruder(0)
  , m_defaultExtruder(0)
  , m_speed(&m_nullVector)
  , m_acceleration(&m_nullVector)
  , m_particleSize(16)
  , m_particleEndSize(-1)
  , m_particleSizeVariation(0)
  , m_maxParticleCount(-1)
  , m_burstLeft(0)
  , m_emitLeft(0)

{
    //TODO: Reset speed/acc back to null vector? Or allow null pointer?
    connect(this, SIGNAL(maxParticleCountChanged(int)),
            this, SIGNAL(particleCountChanged()));
    connect(this, SIGNAL(particlesPerSecondChanged(qreal)),
            this, SIGNAL(particleCountChanged()));
    connect(this, SIGNAL(particleDurationChanged(int)),
            this, SIGNAL(particleCountChanged()));
}

ParticleEmitter::~ParticleEmitter()
{
    if(m_defaultExtruder)
        delete m_defaultExtruder;
}

void ParticleEmitter::componentComplete()
{
    if(!m_system)
        qWarning() << "Emitter created without a particle system specified";//TODO: useful QML warnings, like line number?
    QSGItem::componentComplete();
}
void ParticleEmitter::emitWindow(int timeStamp)
{
    Q_UNUSED(timeStamp);
}


void ParticleEmitter::setEmitting(bool arg)
{
    if (m_emitting != arg) {
        m_emitting = arg;
        emit emittingChanged(arg);
    }
}


ParticleExtruder* ParticleEmitter::effectiveExtruder()
{
    if(m_extruder)
        return m_extruder;
    if(!m_defaultExtruder)
        m_defaultExtruder = new ParticleExtruder;
    return m_defaultExtruder;
}

void ParticleEmitter::pulse(qreal seconds)
{
    if(!particleCount())
        qWarning() << "pulse called on an emitter with a particle count of zero";
    if(!m_emitting)
        m_burstLeft = seconds*1000.0;//TODO: Change name to match
}

void ParticleEmitter::burst(int num)
{
    if(!particleCount())
        qWarning() << "burst called on an emitter with a particle count of zero";
    m_emitLeft += num;
}

void ParticleEmitter::setMaxParticleCount(int arg)
{
    if (m_maxParticleCount != arg) {
        if(arg < 0 && m_maxParticleCount >= 0){
            connect(this, SIGNAL(particlesPerSecondChanged(qreal)),
                    this, SIGNAL(particleCountChanged()));
            connect(this, SIGNAL(particleDurationChanged(int)),
                    this, SIGNAL(particleCountChanged()));
        }else if(arg >= 0 && m_maxParticleCount < 0){
            disconnect(this, SIGNAL(particlesPerSecondChanged(qreal)),
                    this, SIGNAL(particleCountChanged()));
            disconnect(this, SIGNAL(particleDurationChanged(int)),
                    this, SIGNAL(particleCountChanged()));
        }
        m_maxParticleCount = arg;
        emit maxParticleCountChanged(arg);
    }
}

int ParticleEmitter::particleCount() const
{
    if(m_maxParticleCount >= 0)
        return m_maxParticleCount;
    return m_particlesPerSecond*((m_particleDuration+m_particleDurationVariation)/1000.0);
}

QT_END_NAMESPACE
