#ifndef SEEK_AI_H
#define SEEK_AI_H

#include <tankgame/plugin.h>

#include <QState>
#include <QFinalState>
#include <QSignalTransition>
#include <QSignalEvent>
#include <QVariant>
#include <QLineF>
#include <QDebug>

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
    void onEntry(QEvent *)
    {
        connect(m_tank, SIGNAL(actionCompleted()), this, SLOT(turnAlittle()));
        turnAlittle();
    }

    void onExit(QEvent *)
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
        : QSignalTransition(tank, SIGNAL(collision(QLineF))),
          m_tank(tank),
          m_turnTo(turnTo)
    {
        setTargetState(turnTo);
    }

protected:
    bool eventTest(QEvent *event) const
    {
        bool b = QSignalTransition::eventTest(event);
        if (b) {
            QSignalEvent *se = static_cast<QSignalEvent *>(event);
            m_lastLine = se->arguments().at(0).toLineF();
        }
        return b;
    }

    void onTransition(QEvent *)
    {
        qreal angleOfWall = m_lastLine.angle();

        qreal newDirection;
        if (qrand() % 2 == 0)
            newDirection = angleOfWall;
        else
            newDirection = angleOfWall - 180.0; 

        m_turnTo->assignProperty(m_tank, "direction", newDirection);
    }

private:
    mutable QLineF m_lastLine;
    QObject *m_tank;
    QState *m_turnTo;
};

class ChaseState: public QState
{
    class GoToLocationState: public QState
    {
    public:
        GoToLocationState(QObject *tank, QState *parentState = 0) 
            : QState(parentState), m_tank(tank), m_distance(0.0)
        {
        }

        void setDistance(qreal distance) { m_distance = distance; }

    protected:
        void onEntry() 
        {
            QMetaObject::invokeMethod(m_tank, "moveForwards", Q_ARG(qreal, m_distance));
        }

    private:
        QObject *m_tank;
        qreal m_distance;
    };

public:
    ChaseState(QObject *tank, QState *parentState = 0) : QState(parentState), m_tank(tank)
    {
        QState *fireCannon = new QState(this);
        fireCannon->setObjectName("fireCannon");
        connect(fireCannon, SIGNAL(entered()), tank, SLOT(fireCannon()));
        setInitialState(fireCannon);

        m_goToLocation = new GoToLocationState(tank, this);
        m_goToLocation->setObjectName("goToLocation");
        fireCannon->addTransition(tank, SIGNAL(actionCompleted()), m_goToLocation);

        m_turnToDirection = new QState(this);
        m_turnToDirection->setObjectName("turnToDirection");
        m_goToLocation->addTransition(tank, SIGNAL(actionCompleted()), m_turnToDirection);

        QFinalState *finalState = new QFinalState(this);
        finalState->setObjectName("finalState");
        m_turnToDirection->addTransition(tank, SIGNAL(actionCompleted()), finalState);
    }

    void setDirection(qreal direction)
    {
        m_turnToDirection->assignProperty(m_tank, "direction", direction);
    }

    void setDistance(qreal distance)
    {
        m_goToLocation->setDistance(distance);
    }

private:
    QObject *m_tank;
    GoToLocationState *m_goToLocation;
    QState *m_turnToDirection;

};

class TankSpottedTransition: public QSignalTransition
{
public:
    TankSpottedTransition(QObject *tank, ChaseState *target) : QSignalTransition(tank, SIGNAL(tankSpotted(qreal,qreal))), m_chase(target)
    {
        setTargetState(target);
    }

protected:
    bool eventTest(QEvent *event) const
    {
        bool b = QSignalTransition::eventTest(event);
        if (b) {
            QSignalEvent *se = static_cast<QSignalEvent *>(event);
            m_chase->setDirection(se->arguments().at(0).toDouble());
            m_chase->setDistance(se->arguments().at(1).toDouble());
        }
        return b;
    }

private:
    ChaseState *m_chase;
};

class SeekAi: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    SeekAi() { setObjectName("Seek and destroy"); }

    virtual QState *create(QState *parentState, QObject *tank);
};

#endif
