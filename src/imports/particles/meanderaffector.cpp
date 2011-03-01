#include "meanderaffector.h"

MeanderAffector::MeanderAffector(QObject *parent) :
    ParticleAffector(parent)
{
}

bool MeanderAffector::affect(ParticleData *data, qreal dt)
{
    if(!m_xDrift && !m_yDrift)
        return false;
    qreal dx = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_xDrift * dt;
    qreal dy = (((qreal)qrand() / (qreal)RAND_MAX) - 0.5) * 2 * m_yDrift * dt;
    if(dx)
        data->setInstantaneousAX(data->pv.ax + dx);
    if(dy)
        data->setInstantaneousAY(data->pv.ay + dy);

    return true;
}
