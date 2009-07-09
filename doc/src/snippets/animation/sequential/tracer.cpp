#include <QAbstractAnimation>
#include <QDebug>
#include <QPoint>
#include "tracer.h"

Tracer::Tracer(QObject *parent)
    : QObject(parent)
{
}

void Tracer::checkValue()
{
    QAbstractAnimation *animation = static_cast<QAbstractAnimation *>(sender());
    if (time != animation->duration()) {
        qDebug() << "Animation's last recorded time" << time;
        qDebug() << "Expected" << animation->duration();
    }
}

void Tracer::recordValue(const QVariant &value)
{
    QAbstractAnimation *animation = static_cast<QAbstractAnimation *>(sender());
    this->value = value;
    time = animation->currentTime();
}
