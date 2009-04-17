/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <stdio.h>
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstate.h>
#include <qstatemachine.h>
#include <qtransition.h>
#endif

class PingEvent : public QEvent
{
public:
    PingEvent() : QEvent(QEvent::Type(QEvent::User+2))
        {}
};

class PongEvent : public QEvent
{
public:
    PongEvent() : QEvent(QEvent::Type(QEvent::User+3))
        {}
};

class Pinger : public QState
{
public:
    Pinger(QStateMachine *machine, QState *parent)
        : QState(parent), m_machine(machine) {}

protected:
    virtual void onEntry()
    {
        m_machine->postEvent(new PingEvent());
        fprintf(stdout, "ping?\n");
    }

private:
    QStateMachine *m_machine;
};

class PongTransition : public QAbstractTransition
{
public:
    PongTransition(QStateMachine *machine)
        : QAbstractTransition(), m_machine(machine) {}

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+3);
    }
    virtual void onTransition()
    {
        m_machine->postEvent(new PingEvent(), 500);
        fprintf(stdout, "ping?\n");
    }

private:
    QStateMachine *m_machine;
};

class PingTransition : public QAbstractTransition
{
public:
    PingTransition(QStateMachine *machine)
        : QAbstractTransition(), m_machine(machine) {}

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition()
    {
        m_machine->postEvent(new PongEvent(), 500);
        fprintf(stdout, "pong!\n");
    }

private:
    QStateMachine *m_machine;
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStateMachine machine;
    QState *group = new QState(QState::ParallelGroup);
    group->setObjectName("group");

    Pinger *pinger = new Pinger(&machine, group);
    pinger->setObjectName("pinger");
    pinger->addTransition(new PongTransition(&machine));

    QState *ponger = new QState(group);
    ponger->setObjectName("ponger");
    ponger->addTransition(new PingTransition(&machine));

    machine.addState(group);
    machine.setInitialState(group);
    machine.start();

    return app.exec();
}
