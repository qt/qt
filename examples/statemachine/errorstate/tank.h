#ifndef TANK_H
#define TANK_H

#include <QObject>
#include <QLineF>

class Tank: public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal direction READ direction)
    Q_PROPERTY(qreal distanceToObstacle READ distanceToObstacle)
public:
    Tank(QObject *parent = 0) : QObject(parent) {}

    virtual qreal direction() const = 0;
    virtual qreal distanceToObstacle() const = 0;

signals:
    void collision(const QLineF &collidedLine) const;
    void actionCompleted();
    void tankSpotted(qreal otherTankDirection, qreal distance);

public slots:
    virtual void moveForwards(qreal length) = 0;
    virtual void moveBackwards(qreal length) = 0;
    virtual void turn(qreal newDirection) = 0;
    virtual void stop() = 0;
    virtual void fireCannon(qreal distance) = 0;
};

#endif
