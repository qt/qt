/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <stdio.h>
#ifdef QT_STATEMACHINE_SOLUTION
#include <qstate.h>
#include <qstatemachine.h>
#include <qabstracttransition.h>
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
