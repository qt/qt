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
#include <QSignalSpy>
#include <QEventLoop>
#include <QPointer>
#include <QTimer>
#include <QThread>
#include <QTest>

#include <QtDeclarative/qmlengine.h>

#include <private/qmldebugclient_p.h>
#include <private/qmldebugservice_p.h>
#include <private/qmlgraphicsitem_p.h>


class QmlDebugTestData : public QObject
{
    Q_OBJECT
public:
    QmlDebugTestData(QEventLoop *el);

    ~QmlDebugTestData();

    QmlDebugConnection *conn;
    QmlEngine *engine;

    int exitCode;
    QEventLoop *loop;

    QList<QmlGraphicsItem *> items;

public slots:
    void testsFinished(int code);
};


class QmlTestFactory
{
public:
    QmlTestFactory() {}
    virtual ~QmlTestFactory() {}

    virtual QObject *createTest(QmlDebugTestData *data) = 0;
};


namespace QmlDebugTest {

    bool waitForSignal(QObject *receiver, const char *member, int timeout = 5000);

    int runTests(QmlTestFactory *factory, const QList<QByteArray> &qml = QList<QByteArray>());
}

class QmlDebugTestService : public QmlDebugService
{
    Q_OBJECT
public:
    QmlDebugTestService(const QString &s, QObject *parent = 0);
    bool enabled;

signals:
    void enabledStateChanged();

protected:
    virtual void messageReceived(const QByteArray &ba);

    virtual void enabledChanged(bool e);
};

class QmlDebugTestClient : public QmlDebugClient
{
    Q_OBJECT
public:
    QmlDebugTestClient(const QString &s, QmlDebugConnection *c);

    QByteArray waitForResponse();

signals:
    void serverMessage(const QByteArray &);

protected:
    virtual void messageReceived(const QByteArray &ba);

private:
    QByteArray lastMsg;
};

class tst_QmlDebug_Thread : public QThread
{
    Q_OBJECT
public:
    tst_QmlDebug_Thread(QmlDebugTestData *data, QmlTestFactory *factory);

    void run();

    bool m_ready;

signals:
    void testsFinished(int);

private:
    QmlDebugTestData *m_data;
    QmlTestFactory *m_factory;
};


