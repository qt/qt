#include "particleextruder.h"
QT_BEGIN_NAMESPACE
ParticleExtruder::ParticleExtruder(QObject *parent) :
    QObject(parent)
{
}

QPointF ParticleExtruder::extrude(const QRectF &rect)
{
    return QPointF(((qreal)rand() / RAND_MAX) * rect.width() + rect.x(),
                   ((qreal)rand() / RAND_MAX) * rect.height() + rect.y());
}
QT_END_NAMESPACE
