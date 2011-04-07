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

#include "spritegoalaffector.h"
#include "spriteparticle.h"
#include "spriteengine.h"
#include "spritestate.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

SpriteGoalAffector::SpriteGoalAffector(QSGItem *parent) :
    ParticleAffector(parent), m_goalIdx(-1), m_jump(false)
{
}

void SpriteGoalAffector::updateStateIndex(SpriteEngine* e)
{
    m_lastEngine = e;
    for(int i=0; i<e->stateCount(); i++){
        if(e->state(i)->name() == m_goalState){
            m_goalIdx = i;
            return;
        }
    }
    m_goalIdx = -1;//Can't find it
}

void SpriteGoalAffector::setGoalState(QString arg)
{
    if (m_goalState != arg) {
        m_goalState = arg;
        emit goalStateChanged(arg);
        if(m_goalState.isEmpty())
            m_goalIdx = -1;
        else
            m_goalIdx = -2;
    }
}

bool SpriteGoalAffector::affectParticle(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    //TODO: Affect all engines
    SpriteEngine *engine = 0;
    foreach(ParticleType *p, m_system->m_groupData[d->group]->types)
        if(qobject_cast<SpriteParticle*>(p))
            engine = qobject_cast<SpriteParticle*>(p)->spriteEngine();
    if(!engine)
        return false;

    if(m_goalIdx == -2 || engine != m_lastEngine)
        updateStateIndex(engine);
    if(engine->spriteState(d->particleIndex) != m_goalIdx){
        engine->setGoal(m_goalIdx, d->particleIndex, m_jump);
        emit affected(QPointF(d->curX(), d->curY()));//###Expensive if unconnected? Move to Affector?
        return true; //Doesn't affect particle data, but necessary for onceOff
    }
    return false;
}

QT_END_NAMESPACE
