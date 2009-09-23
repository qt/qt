/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <qtest.h>
#include <qtesteventloop.h>

class PingPong : public QObject
{
public:
    void setPeer(QObject *peer);

protected:
    bool event(QEvent *e);

private:
    QObject *m_peer;
    int m_counter;
};

void PingPong::setPeer(QObject *peer)
{
    m_peer = peer;
    m_counter = 100;
}

bool PingPong::event(QEvent *)
{
    --m_counter;
    if (m_counter > 0) {
        QEvent *e = new QEvent(QEvent::User);
        QCoreApplication::postEvent(m_peer, e);
    } else {
        QCoreApplication::quit();
    }
    return true;
}

class EventTester : public QObject
{
public:
    int foo(int bar);

protected:
    bool event(QEvent *e);
};

bool EventTester::event(QEvent *e)
{
    if (e->type() == QEvent::User+1)
        return foo(e->type()) != 0;
    return false;
}

int EventTester::foo(int bar)
{
    return bar + 1;
}

class EventsBench : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void noEvent();
    void sendEvent_data();
    void sendEvent();
    void postEvent_data();
    void postEvent();
};

void EventsBench::initTestCase()
{
}

void EventsBench::cleanupTestCase()
{
}

void EventsBench::noEvent()
{
    EventTester tst;
    int val = 0;
    QBENCHMARK {
        val += tst.foo(1);
    }
}

void EventsBench::sendEvent_data()
{
    QTest::addColumn<bool>("filterEvents");
    QTest::newRow("no eventfilter") << false;
    QTest::newRow("eventfilter") << true;
}

void EventsBench::sendEvent()
{
    QFETCH(bool, filterEvents);
    EventTester tst;
    if (filterEvents)
        tst.installEventFilter(this);
    QEvent evt(QEvent::Type(QEvent::User+1));
    QBENCHMARK {
        QCoreApplication::sendEvent(&tst, &evt);
    }
}

void EventsBench::postEvent_data()
{
    QTest::addColumn<bool>("filterEvents");
    QTest::newRow("no eventfilter") << false;
    QTest::newRow("eventfilter") << true;
}

void EventsBench::postEvent()
{
    QFETCH(bool, filterEvents);
    PingPong ping;
    PingPong pong;
    ping.setPeer(&pong);
    pong.setPeer(&ping);
    if (filterEvents) {
        ping.installEventFilter(this);
        pong.installEventFilter(this);
    }
    QEvent *e = new QEvent(QEvent::User);
    QBENCHMARK {
        QCoreApplication::postEvent(&ping, e);
        QTestEventLoop::instance().enterLoop( 61 );
    }
}

QTEST_MAIN(EventsBench)

#include "main.moc"
