#include "varyingvector.h"

QT_BEGIN_NAMESPACE

VaryingVector::VaryingVector(QObject *parent) :
    QObject(parent)
{
}

const QPointF &VaryingVector::sample(const QPointF &from)
{
    return m_ret;
}

QT_END_NAMESPACE
