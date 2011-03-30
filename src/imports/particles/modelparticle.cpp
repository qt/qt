#include "modelparticle.h"
#include <QtDeclarative/private/qsgvisualitemmodel_p.h>
#include <node.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

ModelParticle::ModelParticle(QSGItem *parent) :
    ParticleType(parent), m_ownModel(false), m_comp(0), m_model(0)
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


void ModelParticle::take(int idx)
{
    if(idx < 0 || idx >= modelCount())
        return;
    m_stasis << m_model->item(idx);//TODO: Does all this getting pointer via the model mess up its reference counting?
}

void ModelParticle::recover(int idx)
{
    if(idx < 0 || idx >= modelCount())
        return;
    m_stasis.remove(m_model->item(idx));
}

void ModelParticle::load(ParticleData* d)
{
    if(!m_model || !m_model->count())
        return;
    int pos = particleTypeIndex(d);
    if(m_available.isEmpty())
        return;
    m_items[pos] = m_model->item(m_available.first());
    m_idx[pos] = m_available.first();
    m_available.pop_front();
    m_items[pos]->setParentItem(this);
    m_data[pos] = d;
}

void ModelParticle::reload(ParticleData* d)
{
    //No-op unless we start copying the data.
}

void ModelParticle::setCount(int c)
{
    m_particleCount = c;
    reset();
}

int ModelParticle::count()
{
    return m_particleCount;
}

void ModelParticle::reset()
{
    //TODO: Cleanup items?
    m_items.resize(m_particleCount);
    m_data.resize(m_particleCount);
    m_idx.resize(m_particleCount);
    m_items.fill(0);
    m_data.fill(0);
    m_idx.fill(-1);
    m_available.clear();
    for(int i=0; i<m_model->count(); i++)
        m_available << i;//TODO: Track changes, then have this in the right place
}


Node* ModelParticle::updatePaintNode(Node* n, UpdatePaintNodeData* d)
{
    //Dummy update just to get painting tick
    if(m_pleaseReset){
        m_pleaseReset = false;
        reset();
    }
    prepareNextFrame();

    update();//Get called again
    if(n)
        n->markDirty(Node::DirtyMaterial);
    return QSGItem::updatePaintNode(n,d);
}

void ModelParticle::prepareNextFrame()
{
    if(!m_model || !m_model->count())
        return;
    uint timeStamp = m_system->systemSync(this);
    qreal curT = timeStamp/1000.0;
    qreal dt = curT - m_lastT;
    m_lastT = curT;

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
        if(t >= 1.0){
            item->setOpacity(0.);
            m_available << m_idx[i];
            m_model->release(m_items[i]);
            m_idx[i] = -1;
            m_items[i] = 0;
            m_data[i] = 0;
        }else{//Fade
            qreal o = 1.;
            if(t<0.2)
                o = t*5;
            if(t>0.8)
                o = (1-t)*5;
            item->setOpacity(o);
        }
        item->setX(data->curX() - item->width()/2);
        item->setY(data->curY() - item->height()/2);
    }
}

QT_END_NAMESPACE
