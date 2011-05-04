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

#include "modelparticle.h"
#include <QtDeclarative/private/qsgvisualitemmodel_p.h>
#include <qsgnode.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

ModelParticle::ModelParticle(QSGItem *parent) :
    ParticleType(parent), m_ownModel(false), m_comp(0), m_model(0), m_fade(true)
{
    setFlag(QSGItem::ItemHasContents);
}

QVariant ModelParticle::model() const
{
    return m_dataSource;
}

void ModelParticle::setModel(const QVariant &arg)
{
    if(arg == m_dataSource)
        return;
    m_dataSource = arg;
    if(qobject_cast<QSGVisualDataModel*>(arg.value<QObject*>())) {
        if(m_ownModel && m_model)
            delete m_model;
        m_model = qobject_cast<QSGVisualDataModel*>(arg.value<QObject*>());
        m_ownModel = false;
    }else{
        if(!m_model || !m_ownModel)
            m_model = new QSGVisualDataModel(qmlContext(this));
        m_model->setModel(m_dataSource);
        m_ownModel = true;
    }
    if(m_comp)
        m_model->setDelegate(m_comp);
    emit modelChanged();
    emit modelCountChanged();
    connect(m_model, SIGNAL(countChanged()),
            this, SIGNAL(modelCountChanged()));
    m_available.clear();
    for(int i=0; i<m_model->count(); i++)
        m_available << i;//TODO: Track changes
}

QDeclarativeComponent *ModelParticle::delegate() const
{
    if(m_model)
        return m_model->delegate();
    return 0;
}

void ModelParticle::setDelegate(QDeclarativeComponent *comp)
{
    if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(m_model))
        if (comp == dataModel->delegate())
            return;
    m_comp = comp;
    if(m_model)
        m_model->setDelegate(comp);
    emit delegateChanged();
}

int ModelParticle::modelCount() const
{
    if(m_model)
        return m_model->count();
    return 0;
}


void ModelParticle::freeze(QSGItem* item)
{
    m_stasis << item;
}


void ModelParticle::unfreeze(QSGItem* item)
{
    m_stasis.remove(item);
}

void ModelParticle::take(QSGItem *item, bool prioritize)
{
    if(prioritize)
        m_pendingItems.push_front(item);
    else
        m_pendingItems.push_back(item);
}

void ModelParticle::give(QSGItem *item)
{
    //TODO: This
}

void ModelParticle::load(ParticleData* d)
{
    //if(!m_model || !m_model->count())//Not really a 'model' particle anymore
    //    return;
    int pos = particleTypeIndex(d);
    if(m_items[pos]){
        if(m_stasis.contains(m_items[pos]))
            qWarning() << "Current model particles prefers overwrite:false";
        //remove old item from the particle that is dying to make room for this one
        m_items[pos]->setOpacity(0.);
        if(m_idx[pos] >= 0){
            m_available << m_idx[pos];
            m_model->release(m_items[pos]);
        }else{
            ModelParticleAttached* mpa;
            if((mpa = qobject_cast<ModelParticleAttached*>(qmlAttachedPropertiesObject<ModelParticle>(m_items[pos]))))
                mpa->detach();//reparent as well?
        }
        m_idx[pos] = -1;
        m_items[pos] = 0;
        m_data[pos] = 0;
        m_activeCount--;
    }
    if(m_available.isEmpty() && m_pendingItems.isEmpty())
        return;
    if(m_pendingItems.isEmpty()){
        m_items[pos] = m_model->item(m_available.first());
        m_idx[pos] = m_available.first();
        m_available.pop_front();
    }else{
        m_items[pos] = m_pendingItems.front();
        m_pendingItems.pop_front();
        m_items[pos]->setX(d->curX() - m_items[pos]->width()/2);
        m_items[pos]->setY(d->curY() - m_items[pos]->height()/2);
        ModelParticleAttached* mpa;
        if((mpa = qobject_cast<ModelParticleAttached*>(qmlAttachedPropertiesObject<ModelParticle>(m_items[pos]))))
            mpa->attach();
        m_idx[pos] = -2;
    }
    m_items[pos]->setParentItem(this);
    m_data[pos] = d;
    m_activeCount++;
}

void ModelParticle::reload(ParticleData* d)
{
    //No-op unless we start copying the data.
}

void ModelParticle::setCount(int c)
{
    ParticleType::setCount(c);//###Do we need our own?
    m_particleCount = c;
    reset();
}

int ModelParticle::count()
{
    return m_particleCount;
}

void ModelParticle::reset()
{
    ParticleType::reset();
    //TODO: Cleanup items?
    m_items.resize(m_particleCount);
    m_data.resize(m_particleCount);
    m_idx.resize(m_particleCount);
    m_items.fill(0);
    m_data.fill(0);
    m_idx.fill(-1);
    m_available.clear();
    //m_pendingItems.clear();//TODO: Should this be done? If so, Emit signal?
    if(m_model)
        for(int i=0; i<m_model->count(); i++)
            m_available << i;//TODO: Track changes, then have this in the right place
}


QSGNode* ModelParticle::updatePaintNode(QSGNode* n, UpdatePaintNodeData* d)
{
    //Dummy update just to get painting tick
    if(m_pleaseReset){
        m_pleaseReset = false;
        reset();
    }
    prepareNextFrame();

    update();//Get called again
    if(n)
        n->markDirty(QSGNode::DirtyMaterial);
    return QSGItem::updatePaintNode(n,d);
}

void ModelParticle::prepareNextFrame()
{
    uint timeStamp = m_system->systemSync(this);
    qreal curT = timeStamp/1000.0;
    qreal dt = curT - m_lastT;
    m_lastT = curT;
    if(!m_activeCount)
        return;

    //TODO: Size, better fade?
    for(int i=0; i<m_particleCount; i++){
        QSGItem* item = m_items[i];
        ParticleData* data = m_data[i];
        if(!item || !data)
            continue;
        qreal t = ((timeStamp/1000.0) - data->pv.t) / data->pv.lifeSpan;
        if(m_stasis.contains(item)) {
            m_data[i]->pv.t += dt;//Stasis effect
            continue;
        }
        if(t >= 1.0){//Usually happens from load
            item->setOpacity(0.);
            if(m_idx[i] >= 0){
                m_available << m_idx[i];
                m_model->release(m_items[i]);
            }else{
                ModelParticleAttached* mpa;
                if((mpa = qobject_cast<ModelParticleAttached*>(qmlAttachedPropertiesObject<ModelParticle>(m_items[i]))))
                    mpa->detach();//reparent as well?
            }
            m_idx[i] = -1;
            m_items[i] = 0;
            m_data[i] = 0;
            m_activeCount--;
        }else{//Fade
            if(m_fade){
                qreal o = 1.;
                if(t<0.2)
                    o = t*5;
                if(t>0.8)
                    o = (1-t)*5;
                item->setOpacity(o);
            }else{
                item->setOpacity(1.);//###Without fade, it's just a binary toggle - if we turn it off we have to turn it back on
            }
        }
        item->setX(data->curX() - item->width()/2);
        item->setY(data->curY() - item->height()/2);
    }
}

ModelParticleAttached *ModelParticle::qmlAttachedProperties(QObject *object)
{
    return new ModelParticleAttached(object);
}

QT_END_NAMESPACE
