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

#include "particle.h"
#include <QDebug>
QT_BEGIN_NAMESPACE
ParticleType::ParticleType(QSGItem *parent) :
    QSGItem(parent),
    m_system(0)
{
    connect(this, SIGNAL(xChanged()),
            this, SLOT(calcSystemOffset()));
    connect(this, SIGNAL(yChanged()),
            this, SLOT(calcSystemOffset()));
}

void ParticleType::componentComplete()
{
    if(!m_system)
        qWarning() << "Particle created without a particle system specified";//TODO: useful QML warnings, like line number?
    QSGItem::componentComplete();
}


void ParticleType::setSystem(ParticleSystem *arg)
{
    if (m_system != arg) {
        m_system = arg;
        if(m_system){
            m_system->registerParticleType(this);
            connect(m_system, SIGNAL(xChanged()),
                    this, SLOT(calcSystemOffset()));
            connect(m_system, SIGNAL(yChanged()),
                    this, SLOT(calcSystemOffset()));
            calcSystemOffset();
        }
        emit systemChanged(arg);
    }
}

void ParticleType::load(ParticleData*)
{
}

void ParticleType::reload(ParticleData*)
{
}

void ParticleType::reset()
{
    //Have to every time because what it's emitting may have changed and that affects particleTypeIndex
    m_particleStarts.clear();
    m_lastStart = 0;
}

void ParticleType::setCount(int c)
{
    if(c == m_count)
        return;
    m_count = c;
    emit countChanged();
}

int ParticleType::count()
{
    return m_count;
}


int ParticleType::particleTypeIndex(ParticleData* d)
{
    if(!m_particleStarts.contains(d->group)){
        m_particleStarts.insert(d->group, m_lastStart);
        m_lastStart += m_system->m_groupData[d->group]->size;
    }
    int ret = m_particleStarts[d->group] + d->particleIndex;
    Q_ASSERT(ret >=0 && ret < m_count);//XXX: Possibly shouldn't assert, but bugs here were hard to find in the past
    return ret;
}


void ParticleType::calcSystemOffset()
{
    if(!m_system)
        return;
    QPointF lastOffset = m_systemOffset;
    m_systemOffset = this->mapFromItem(m_system, QPointF());
    if(lastOffset != m_systemOffset){
        //Reload all particles
        foreach(const QString &g, m_particles){
            int gId = m_system->m_groupIds[g];
            for(int i=0; i<m_system->m_groupData[gId]->size; i++)
                reload(m_system->m_data[m_system->m_groupData[gId]->start + i]);
        }
    }
}
QT_END_NAMESPACE
