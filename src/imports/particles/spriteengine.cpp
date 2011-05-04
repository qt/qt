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

#include "spriteengine.h"
#include "spritestate.h"
#include <QDebug>
#include <QPainter>
#include <QSet>
#include <QtOpenGL>

QT_BEGIN_NAMESPACE

SpriteEngine::SpriteEngine(QObject *parent) :
    QObject(parent), m_timeOffset(0)
{
    //Default size 1
    setCount(1);
    m_advanceTime.start();
}

SpriteEngine::SpriteEngine(QList<SpriteState*> states, QObject *parent) :
    QObject(parent), m_states(states), m_timeOffset(0)
{
    //Default size 1
    setCount(1);
    m_advanceTime.start();
}

SpriteEngine::~SpriteEngine()
{
}

int SpriteEngine::maxFrames()
{
   int max = 0;
   foreach(SpriteState* s, m_states)
       if(s->frames() > max)
           max = s->frames();
   return max;
}

void SpriteEngine::setGoal(int state, int sprite, bool jump)
{
    if(sprite >= m_sprites.count() || state >= m_states.count())
        return;
    if(!jump){
        m_goals[sprite] = state;
        return;
    }

    if(m_sprites[sprite] == state)
        return;//Already there
    m_sprites[sprite] = state;
    m_goals[sprite] = -1;
    restartSprite(sprite);
    return;
}

QImage SpriteEngine::assembledImage()
{
    int frameHeight = 0;
    int frameWidth = 0;
    m_maxFrames = 0;

    int maxSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

    foreach(SpriteState* state, m_states){
        if(state->frames() > m_maxFrames)
            m_maxFrames = state->frames();

        QImage img(state->source().toLocalFile());
        if (img.isNull()) {
            qWarning() << "SpriteEngine: loading image failed..." << state->source().toLocalFile();
            return QImage();
        }

        if(frameWidth){
            if(img.width() / state->frames() != frameWidth){
                qWarning() << "SpriteEngine: Irregular frame width..." << state->source().toLocalFile();
                return QImage();
            }
        }else{
            frameWidth = img.width() / state->frames();
        }
        if(img.width() > maxSize){
            qWarning() << "SpriteEngine: Animation too wide..." << state->source().toLocalFile();
            return QImage();
        }

        if(frameHeight){
            if(img.height()!=frameHeight){
                qWarning() << "SpriteEngine: Irregular frame height..." << state->source().toLocalFile();
                return QImage();
            }
        }else{
            frameHeight = img.height();
        }

        if(img.height() > maxSize){
            qWarning() << "SpriteEngine: Animation too tall..." << state->source().toLocalFile();
            return QImage();
        }
    }

    QImage image(frameWidth * m_maxFrames, frameHeight * m_states.count(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter p(&image);
    int y = 0;
    foreach(SpriteState* state, m_states){
        QImage img(state->source().toLocalFile());
        p.drawImage(0,y,img);
        y += frameHeight;
    }

    if(image.height() > maxSize){
        qWarning() << "SpriteEngine: Too many animations to fit in one texture...";
        return QImage();
    }
    return image;
}

void SpriteEngine::setCount(int c)
{
    m_sprites.resize(c);
    m_goals.resize(c);
    m_startTimes.resize(c);
}

void SpriteEngine::startSprite(int index)
{
    if(index >= m_sprites.count())
        return;
    m_sprites[index] = 0;
    m_goals[index] = -1;
    restartSprite(index);
}

void SpriteEngine::restartSprite(int index)
{
    m_startTimes[index] = m_timeOffset + m_advanceTime.elapsed();
    int time = m_states[m_sprites[index]]->duration() * m_states[m_sprites[index]]->frames() + m_startTimes[index];
    for(int i=0; i<m_stateUpdates.count(); i++)
        m_stateUpdates[i].second.removeAll(index);
    addToUpdateList(time, index);
}

uint SpriteEngine::updateSprites(uint time)
{
    //Sprite State Update;
    while(!m_stateUpdates.isEmpty() && time >= m_stateUpdates.first().first){
        foreach(int idx, m_stateUpdates.first().second){
            if(idx >= m_sprites.count())
                continue;//TODO: Proper fix(because this does happen and I'm just ignoring it)
            int stateIdx = m_sprites[idx];
            int nextIdx = -1;
            int goalPath = goalSeek(stateIdx, idx);
            if(goalPath == -1){//Random
                qreal r =(qreal) qrand() / (qreal) RAND_MAX;
                qreal total = 0.0;
                for(QVariantMap::const_iterator iter=m_states[stateIdx]->m_to.constBegin();
                    iter!=m_states[stateIdx]->m_to.constEnd(); iter++)
                    total += (*iter).toReal();
                r*=total;
                for(QVariantMap::const_iterator iter= m_states[stateIdx]->m_to.constBegin();
                        iter!=m_states[stateIdx]->m_to.constEnd(); iter++){
                    if(r < (*iter).toReal()){
                        bool superBreak = false;
                        for(int i=0; i<m_states.count(); i++){
                            if(m_states[i]->name() == iter.key()){
                                nextIdx = i;
                                superBreak = true;
                                break;
                            }
                        }
                        if(superBreak)
                            break;
                    }
                    r -= (*iter).toReal();
                }
            }else{//Random out of shortest paths to goal
                nextIdx = goalPath;
            }
            if(nextIdx == -1)//No to states means stay here
                nextIdx = stateIdx;

            m_sprites[idx] = nextIdx;
            m_startTimes[idx] = time;
            //TODO: emit something?
            addToUpdateList((m_states[nextIdx]->duration() * m_states[nextIdx]->frames()) + time, idx);
        }
        m_stateUpdates.pop_front();
    }

    m_timeOffset = time;
    m_advanceTime.start();
    if(m_stateUpdates.isEmpty())
        return -1;
    return m_stateUpdates.first().first;
}

int SpriteEngine::goalSeek(int curIdx, int spriteIdx, int dist)
{
    QString goalName;
    if(m_goals[spriteIdx] != -1)
        goalName = m_states[m_goals[spriteIdx]]->name();
    else
        goalName = m_globalGoal;
    if(goalName.isEmpty())
        return -1;
    //TODO: caching instead of excessively redoing iterative deepening (which was chosen arbitarily anyways)
    // Paraphrased - implement in an *efficient* manner
    for(int i=0; i<m_states.count(); i++)
        if(m_states[curIdx]->name() == goalName)
            return curIdx;
    if(dist < 0)
        dist = m_states.count();
    SpriteState* curState = m_states[curIdx];
    for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
        iter!=curState->m_to.constEnd(); iter++){
        if(iter.key() == goalName)
            for(int i=0; i<m_states.count(); i++)
                if(m_states[i]->name() == goalName)
                    return i;
    }
    QSet<int> options;
    for(int i=1; i<dist; i++){
        for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
            iter!=curState->m_to.constEnd(); iter++){
            int option = -1;
            for(int j=0; j<m_states.count(); j++)//One place that could be a lot more efficient...
                if(m_states[j]->name() == iter.key())
                    if(goalSeek(j, spriteIdx, i) != -1)
                        option = j;
            if(option != -1)
                options << option;
        }
        if(!options.isEmpty()){
            if(options.count()==1)
                return *(options.begin());
            int option = -1;
            qreal r =(qreal) qrand() / (qreal) RAND_MAX;
            qreal total;
            for(QSet<int>::const_iterator iter=options.constBegin();
                iter!=options.constEnd(); iter++)
                total += curState->m_to.value(m_states[(*iter)]->name()).toReal();
            r *= total;
            for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
                iter!=curState->m_to.constEnd(); iter++){
                bool superContinue = true;
                for(int j=0; j<m_states.count(); j++)
                    if(m_states[j]->name() == iter.key())
                        if(options.contains(j))
                            superContinue = false;
                if(superContinue)
                    continue;
                if(r < (*iter).toReal()){
                    bool superBreak = false;
                    for(int j=0; j<m_states.count(); j++){
                        if(m_states[j]->name() == iter.key()){
                            option = j;
                            superBreak = true;
                            break;
                        }
                    }
                    if(superBreak)
                        break;
                }
                r-=(*iter).toReal();
            }
            return option;
        }
    }
    return -1;
}

void SpriteEngine::addToUpdateList(uint t, int idx)
{
    for(int i=0; i<m_stateUpdates.count(); i++){
        if(m_stateUpdates[i].first==t){
            m_stateUpdates[i].second << idx;
            return;
        }else if(m_stateUpdates[i].first > t){
            QList<int> tmpList;
            tmpList << idx;
            m_stateUpdates.insert(i, qMakePair(t, tmpList));
            return;
        }
    }
    QList<int> tmpList;
    tmpList << idx;
    m_stateUpdates << qMakePair(t, tmpList);
}

QT_END_NAMESPACE
