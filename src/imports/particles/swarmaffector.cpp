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

#include "swarmaffector.h"
#include "particle.h"
#include <cmath>
#include <QDebug>
QT_BEGIN_NAMESPACE

SwarmAffector::SwarmAffector(QSGItem *parent) :
    ParticleAffector(parent), m_strength(1), m_inited(false)
{
    connect(this, SIGNAL(leadersChanged(QStringList)),
            this, SLOT(updateGroupList()));
}

void SwarmAffector::ensureInit()
{
    if(m_inited)
        return;
    m_inited = true;
    updateGroupList();
    m_lastPos.resize(m_system->count());
}

const qreal epsilon = 0.0000001;
bool SwarmAffector::affectParticle(ParticleData *d, qreal dt)
{
    ensureInit();
    QPointF curPos(d->curX(), d->curY());
    if(m_leaders.isEmpty() || m_leadGroups.contains(d->group)){
        m_lastPos[d->systemIndex] = curPos;
        if(m_leadGroups.contains(d->group))
            return false;
    }

    qreal fx = 0.0;
    qreal fy = 0.0;
    for(int i=0; i < m_lastPos.count(); i++){
        if(m_lastPos[i].isNull())
            continue;
        QPointF diff = m_lastPos[i] - curPos;
        qreal r = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
        if(r == 0.0)
            continue;
        qreal f = m_strength * (1/r);
        if(f < epsilon)
            continue;
        qreal theta = atan2(diff.y(), diff.x());
        fx += cos(theta) * f;
        fy += sin(theta) * f;
    }
    if(!fx && !fy)
        return false;
    d->setInstantaneousSX(d->curSX()+fx * dt);
    d->setInstantaneousSY(d->curSY()+fy * dt);
    return true;
}

void SwarmAffector::reset(int systemIdx)
{
    if(!m_system)
        return;
    if(!m_lastPos[systemIdx].isNull())
        m_lastPos[systemIdx] = QPointF();
}

void SwarmAffector::updateGroupList()
{
    if(!m_system || !m_system->m_initialized)
        return;
    m_leadGroups.clear();
    foreach(const QString &s, m_leaders)
        m_leadGroups << m_system->m_groupIds[s];
}
QT_END_NAMESPACE
