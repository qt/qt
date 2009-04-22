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
#include <qstatemachine.h>
#include <qstate.h>
#include <qtransition.h>
#endif

class ClockEvent : public QEvent
{
public:
    ClockEvent() : QEvent(QEvent::Type(QEvent::User+2))
        {}
};

class ClockState : public QState
{
public:
    ClockState(QStateMachine *machine, QState *parent)
        : QState(parent), m_machine(machine) {}

protected:
    virtual void onEntry()
    {
        fprintf(stdout, "ClockState entered; posting the initial tick\n");
        m_machine->postEvent(new ClockEvent());
    }

private:
    QStateMachine *m_machine;
};

class ClockTransition : public QAbstractTransition
{
public:
    ClockTransition(QStateMachine *machine)
        : QAbstractTransition(), m_machine(machine) { }

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition()
    {
        fprintf(stdout, "ClockTransition triggered; posting another tick with a delay of 1 second\n");
        m_machine->postEvent(new ClockEvent(), 1000);
    }

private:
    QStateMachine *m_machine;
};

class ClockListener : public QAbstractTransition
{
public:
    ClockListener()
        : QAbstractTransition() {}

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition()
    {
        fprintf(stdout, "ClockListener heard a tick!\n");
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStateMachine machine;
    QState *group = new QState(QState::ParallelGroup);
    group->setObjectName("group");

    ClockState *clock = new ClockState(&machine, group);
    clock->setObjectName("clock");
    clock->addTransition(new ClockTransition(&machine));

    QState *listener = new QState(group);
    listener->setObjectName("listener");
    listener->addTransition(new ClockListener());

    machine.addState(group);
    machine.setInitialState(group);
    machine.start();

    return app.exec();
}
