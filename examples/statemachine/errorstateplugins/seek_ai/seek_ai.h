#ifndef SEEK_AI_H
#define SEEK_AI_H

#include <errorstate/plugin.h>

#include <QState>
#include <QSignalTransition>
#include <QSignalEvent>
#include <QVariant>
#include <QLineF>

class SearchState: public QState
{
    Q_OBJECT
public:
    SearchState(QObject *tank, QState *parentState = 0) 
        : QState(parentState), 
          m_tank(tank), 
          m_distanceToTurn(360.0), 
          m_nearestDistance(-1.0),
          m_directionOfNearestObstacle(0.0) 
    {
    }

public slots:
    void turnAlittle()
    {        
        qreal dist = m_tank->property("distanceToObstacle").toDouble();

        if (m_nearestDistance < 0.0 || dist < m_nearestDistance) {
            m_nearestDistance = dist;
            m_directionOfNearestObstacle = m_tank->property("direction").toDouble();
        }

        m_distanceToTurn -= 10.0;
        if (m_distanceToTurn < 0.0) {
            disconnect(m_tank, SIGNAL(actionCompleted()), this, SLOT(turnAlittle()));
            connect(m_tank, SIGNAL(actionCompleted()), this, SIGNAL(nearestObstacleStraightAhead()));
            m_tank->setProperty("direction", m_directionOfNearestObstacle);
        }

        qreal currentDirection = m_tank->property("direction").toDouble();
        m_tank->setProperty("direction", currentDirection + 10.0);
    }

signals:
    void nearestObstacleStraightAhead();

protected:
    void onEntry() 
    {
        connect(m_tank, SIGNAL(actionCompleted()), this, SLOT(turnAlittle()));
        turnAlittle();
    }

    void onExit() 
    {
        disconnect(m_tank, SIGNAL(actionCompleted()), this, SLOT(turnAlittle()));
        disconnect(m_tank, SIGNAL(actionCompleted()), this, SLOT(nearestObstacleStraightAhead()));
    }

private:
    QObject *m_tank;

    qreal m_distanceToTurn;
    qreal m_nearestDistance;
    qreal m_directionOfNearestObstacle;
};

class CollisionTransition: public QSignalTransition
{
public:
    CollisionTransition(QObject *tank, QState *turnTo) 
        : QSignalTransition(tank, SIGNAL(collision(QLineF)), turnTo),
          m_tank(tank),
          m_turnTo(turnTo)
    {
    }

protected:
    bool eventTest(QEvent *event) 
    {
        QSignalEvent *se = static_cast<QSignalEvent *>(event);
        m_lastLine = se->arguments().at(0).toLineF();

        return QSignalTransition::eventTest(event);
    }

    void onTransition()
    {
        qreal currentDirection = m_tank->property("direction").toDouble();
        qreal angleOfWall = m_lastLine.angle();

        qreal newDirection;
        if (qAbs(currentDirection - angleOfWall) < qAbs(angleOfWall - currentDirection))
            newDirection = angleOfWall;
        else
            newDirection = -angleOfWall; 

        m_turnTo->assignProperty(m_tank, "direction", newDirection);
    }

private:
    QLineF m_lastLine;
    QObject *m_tank;
    QState *m_turnTo;
};

class SeekAi: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    virtual QState *create(QState *parentState, QObject *tank);
};

#endif
