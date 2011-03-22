#include "ellipseextruder.h"
#include <cmath>

EllipseExtruder::EllipseExtruder(QObject *parent) :
    ParticleExtruder(parent)
  , m_fill(true)
{
}

QPointF EllipseExtruder::extrude(const QRectF & r)
{
    qreal theta = ((qreal)rand()/RAND_MAX) * 6.2831853071795862;
    qreal mag = m_fill ? ((qreal)rand()/RAND_MAX) : 1;
    return QPointF(r.x() + r.width()/2 + mag * (r.width()/2) * cos(theta),
                   r.y() + r.height()/2 + mag * (r.height()/2) * sin(theta));
}
