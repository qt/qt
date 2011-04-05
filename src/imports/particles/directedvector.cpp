#include "directedvector.h"
#include <cmath>

QT_BEGIN_NAMESPACE
DirectedVector::DirectedVector(QObject *parent) :
    VaryingVector(parent)
  , m_targetX(0)
  , m_targetY(0)
  , m_targetVariation(0)
  , m_proportionalMagnitude(false)
  , m_magnitude(0)
  , m_magnitudeVariation(0)
{
}

const QPointF &DirectedVector::sample(const QPointF &from)
{
    //###This approach loses interpolating the last position of the target (like we could with the emitter) is it worthwhile?
    qreal targetX = m_targetX - from.x() - m_targetVariation + rand()/(float)RAND_MAX * m_targetVariation*2;
    qreal targetY = m_targetY - from.y() - m_targetVariation + rand()/(float)RAND_MAX * m_targetVariation*2;
    qreal theta = atan2(targetY, targetX);
    qreal mag = m_magnitude + rand()/(float)RAND_MAX * m_magnitudeVariation * 2 - m_magnitudeVariation;
    if(m_proportionalMagnitude)
        mag *= sqrt(targetX * targetX + targetY * targetY);
    m_ret.setX(mag * cos(theta));
    m_ret.setY(mag * sin(theta));
    return m_ret;
}

QT_END_NAMESPACE
