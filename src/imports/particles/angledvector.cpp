#include "angledvector.h"
#include <cmath>
QT_BEGIN_NAMESPACE
const qreal CONV = 0.017453292519943295;
AngledVector::AngledVector(QObject *parent) :
    VaryingVector(parent)
  , m_angle(0)
  , m_magnitude(0)
  , m_angleVariation(0)
  , m_magnitudeVariation(0)
{

}

const QPointF &AngledVector::sample(const QPointF &from)
{
    //TODO: Faster
    qreal theta = m_angle*CONV - m_angleVariation*CONV + rand()/float(RAND_MAX) * m_angleVariation*CONV * 2;
    qreal mag = m_magnitude- m_magnitudeVariation + rand()/float(RAND_MAX) * m_magnitudeVariation * 2;
    m_ret.setX(mag * cos(theta));
    m_ret.setY(mag * sin(theta));
    return m_ret;
}

QT_END_NAMESPACE
