#ifndef RANDOM_AI_PLUGIN_H
#define RANDOM_AI_PLUGIN_H

#include <QObject>
#include <QState>

#include <errorstate/plugin.h>

class SelectActionState: public QState
{
    Q_OBJECT
public:
    SelectActionState(QState *parent = 0) : QState(parent)
    {
    }

signals:
    void fireSelected();
    void moveForwardsSelected();
    void moveBackwardsSelected();
    void turnSelected();
    
protected:
    void onEntry()
    {
        int rand = qrand() % 4;
        switch (rand) {
        case 0: emit fireSelected(); break;
        case 1: emit moveForwardsSelected(); break;
        case 2: emit moveBackwardsSelected(); break;
        case 3: emit turnSelected(); break;
        };
    }
};

class RandomDistanceState: public QState
{
    Q_OBJECT
public:
    RandomDistanceState(QState *parent = 0) : QState(parent)
    {
    }

signals:
    void distanceComputed(qreal distance);

protected:
    void onEntry() 
    {
        emit distanceComputed(qreal(qrand() % 10));
    }
};

class RandomAiPlugin: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    virtual QState *create(QState *parentState, Tank *tank);
};

#endif // RANDOM_AI_PLUGIN_H
