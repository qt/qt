/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <stdio.h>

//! [0]
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
//! [0]

//! [1]
class Pinger : public QState
{
public:
    Pinger(QState *parent)
        : QState(parent) {}

protected:
    virtual void onEntry(QEvent *)
    {
        machine()->postEvent(new PingEvent());
        fprintf(stdout, "ping?\n");
    }
};
//! [1]

//! [3]
class PongTransition : public QAbstractTransition
{
public:
    PongTransition() {}

protected:
    virtual bool eventTest(QEvent *e) {
        return (e->type() == QEvent::User+3);
    }
    virtual void onTransition(QEvent *)
    {
        machine()->postDelayedEvent(new PingEvent(), 500);
        fprintf(stdout, "ping?\n");
    }
};
//! [3]

//! [2]
class PingTransition : public QAbstractTransition
{
public:
    PingTransition() {}

protected:
    virtual bool eventTest(QEvent *e) {
        return (e->type() == QEvent::User+2);
    }
    virtual void onTransition(QEvent *)
    {
        machine()->postDelayedEvent(new PongEvent(), 500);
        fprintf(stdout, "pong!\n");
    }
};
//! [2]

//! [4]
int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    QStateMachine machine;
    QState *group = new QState(QState::ParallelStates);
    group->setObjectName("group");
//! [4]

//! [5]
    Pinger *pinger = new Pinger(group);
    pinger->setObjectName("pinger");
    pinger->addTransition(new PongTransition());

    QState *ponger = new QState(group);
    ponger->setObjectName("ponger");
    ponger->addTransition(new PingTransition());
//! [5]

//! [6]
    machine.addState(group);
    machine.setInitialState(group);
    machine.start();

    return app.exec();
}
//! [6]
