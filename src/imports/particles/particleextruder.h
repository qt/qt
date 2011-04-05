#ifndef PARTICLEEXTRUDER_H
#define PARTICLEEXTRUDER_H

#include <QObject>
#include <QRectF>
#include <QPointF>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleExtruder : public QObject
{
    Q_OBJECT
public:
    explicit ParticleExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);
signals:

public slots:

};

QT_END_NAMESPACE
QT_END_HEADER
#endif // PARTICLEEXTRUDER_H
