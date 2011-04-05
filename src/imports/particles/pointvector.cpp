#include "pointvector.h"
QT_BEGIN_NAMESPACE

PointVector::PointVector(QObject *parent) :
    VaryingVector(parent)
  , m_x(0)
  , m_y(0)
  , m_xVariation(0)
  , m_yVariation(0)
{
}

const QPointF &PointVector::sample(const QPointF &)
{
    m_ret.setX(m_x - m_xVariation + rand() / float(RAND_MAX) * m_xVariation * 2);
    m_ret.setY(m_y - m_yVariation + rand() / float(RAND_MAX) * m_yVariation * 2);
    return m_ret;
}

QT_END_NAMESPACE
