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
#include <qstatemachine.h>
#include <qstate.h>
#include <qabstracttransition.h>
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
    ClockState(QState *parent)
        : QState(parent) {}

protected:
    virtual void onEntry(QEvent *)
    {
        fprintf(stdout, "ClockState entered; posting the initial tick\n");
        machine()->postEvent(new ClockEvent());
    }
};

class ClockTransition : public QAbstractTransition
{
public:
    ClockTransition() {}

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition(QEvent *)
    {
        fprintf(stdout, "ClockTransition triggered; posting another tick with a delay of 1 second\n");
        machine()->postEvent(new ClockEvent(), 1000);
    }
};

class ClockListener : public QAbstractTransition
{
public:
    ClockListener() {}

protected:
    virtual bool eventTest(QEvent *e) const {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition(QEvent *)
    {
        fprintf(stdout, "ClockListener heard a tick!\n");
    }
};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStateMachine machine;
    QState *group = new QState(QState::ParallelStates);
    group->setObjectName("group");

    ClockState *clock = new ClockState(group);
    clock->setObjectName("clock");
    clock->addTransition(new ClockTransition());

    QState *listener = new QState(group);
    listener->setObjectName("listener");
    listener->addTransition(new ClockListener());

    machine.addState(group);
    machine.setInitialState(group);
    machine.start();

    return app.exec();
}
