#ifndef PARTICLEEXTRUDER_H
#define PARTICLEEXTRUDER_H

#include <QObject>
#include <QRectF>
#include <QPointF>

class ParticleExtruder : public QObject
{
    Q_OBJECT
public:
    explicit ParticleExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);
signals:

public slots:

};

#endif // PARTICLEEXTRUDER_H
