/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <qcoreapplication.h>
#include <qdebug.h>

#include <QtTest/QtTest>
#include <QtDBus/QtDBus>

class BaseObject: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.BaseObject")
public:
    BaseObject(QObject *parent = 0) : QObject(parent) { }
public slots:
    void anotherMethod() { }
};

class MyObject: public BaseObject
{
    Q_OBJECT
public slots:
    void method(const QDBusMessage &msg);

public:
    static QString path;
    int callCount;
    MyObject(QObject *parent = 0) : BaseObject(parent), callCount(0) {}
};

void MyObject::method(const QDBusMessage &msg)
{
    path = msg.path();
    ++callCount;
    //qDebug() << msg;
}

class tst_QDBusConnection: public QObject
{
    Q_OBJECT

    int signalsReceived;
public slots:
    void oneSlot() { ++signalsReceived; }
    void exitLoop() { ++signalsReceived; QTestEventLoop::instance().exitLoop(); }
    void secondCallWithCallback();

private slots:
    void noConnection();
    void connectToBus();
    void connect();
    void send();
    void sendWithGui();
    void sendAsync();
    void sendSignal();

    void registerObject_data();
    void registerObject();
    void registerObject2();

    void registerQObjectChildren();

    void callSelf();
    void callSelfByAnotherName_data();
    void callSelfByAnotherName();
    void multipleInterfacesInQObject();

    void slotsWithLessParameters();
    void nestedCallWithCallback();

public:
    QString serviceName() const { return "com.trolltech.Qt.Autotests.QDBusConnection"; }
    bool callMethod(const QDBusConnection &conn, const QString &path);
};

class QDBusSpy: public QObject
{
    Q_OBJECT
public slots:
    void handlePing(const QString &str) { args.clear(); args << str; }
    void asyncReply(const QDBusMessage &msg) { args = msg.arguments(); }

public:
    QList<QVariant> args;
};

void tst_QDBusConnection::noConnection()
{
    QDBusConnection con = QDBusConnection::connectToBus("unix:path=/dev/null", "testconnection");
    QVERIFY(!con.isConnected());

    // try sending a message. This should fail
    QDBusMessage msg = QDBusMessage::createMethodCall("org.kde.selftest", "/org/kde/selftest",
                                                      "org.kde.selftest", "Ping");
    msg << QLatin1String("ping");

    QVERIFY(!con.send(msg));

    QDBusSpy spy;
    QVERIFY(con.callWithCallback(msg, &spy, SLOT(asyncReply)) == 0);

    QDBusMessage reply = con.call(msg);
    QVERIFY(reply.type() == QDBusMessage::ErrorMessage);

    QDBusReply<void> voidreply(reply);
    QVERIFY(!voidreply.isValid());

    QDBusConnection::disconnectFromBus("testconnection");
}

void tst_QDBusConnection::sendSignal()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createSignal("/org/kde/selftest", "org.kde.selftest",
                                                  "Ping");
    msg << QLatin1String("ping");

    QVERIFY(con.send(msg));

    QTest::qWait(1000);
}

void tst_QDBusConnection::send()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus",
        "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");

    QDBusMessage reply = con.call(msg);

    QCOMPARE(reply.arguments().count(), 1);
    QCOMPARE(reply.arguments().at(0).typeName(), "QStringList");
    QVERIFY(reply.arguments().at(0).toStringList().contains(con.baseService()));
}

void tst_QDBusConnection::sendWithGui()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    QVERIFY(con.isConnected());

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus",
        "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");

    QDBusMessage reply = con.call(msg, QDBus::BlockWithGui);

    QCOMPARE(reply.arguments().count(), 1);
    QCOMPARE(reply.arguments().at(0).typeName(), "QStringList");
    QVERIFY(reply.arguments().at(0).toStringList().contains(con.baseService()));
}

void tst_QDBusConnection::sendAsync()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    QDBusSpy spy;

    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus",
            "/org/freedesktop/DBus", "org.freedesktop.DBus", "ListNames");
    QVERIFY(con.callWithCallback(msg, &spy, SLOT(asyncReply(QDBusMessage))));

    QTest::qWait(1000);

    QCOMPARE(spy.args.value(0).typeName(), "QStringList");
    QVERIFY(spy.args.at(0).toStringList().contains(con.baseService()));
}

void tst_QDBusConnection::connect()
{
    QDBusSpy spy;

    QDBusConnection con = QDBusConnection::sessionBus();

    con.connect(con.baseService(), "/org/kde/selftest", "org.kde.selftest", "ping", &spy,
                 SLOT(handlePing(QString)));

    QDBusMessage msg = QDBusMessage::createSignal("/org/kde/selftest", "org.kde.selftest",
                                                  "ping");
    msg << QLatin1String("ping");

    QVERIFY(con.send(msg));

    QTest::qWait(1000);

    QCOMPARE(spy.args.count(), 1);
    QCOMPARE(spy.args.at(0).toString(), QString("ping"));
}

void tst_QDBusConnection::connectToBus()
{
    {
        QDBusConnection con = QDBusConnection::connectToBus(
                QDBusConnection::SessionBus, "bubu");

        QVERIFY(con.isConnected());
        QVERIFY(!con.lastError().isValid());

        QDBusConnection con2("foo");
        QVERIFY(!con2.isConnected());
        QVERIFY(!con2.lastError().isValid());

        con2 = con;
        QVERIFY(con.isConnected());
        QVERIFY(con2.isConnected());
        QVERIFY(!con.lastError().isValid());
        QVERIFY(!con2.lastError().isValid());
    }

    {
        QDBusConnection con("bubu");
        QVERIFY(con.isConnected());
        QVERIFY(!con.lastError().isValid());
    }

    QDBusConnection::disconnectFromBus("bubu");

    {
        QDBusConnection con("bubu");
        QVERIFY(!con.isConnected());
        QVERIFY(!con.lastError().isValid());
    }

    QByteArray address = qgetenv("DBUS_SESSION_BUS_ADDRESS");
    if (!address.isEmpty()) {
        QDBusConnection con = QDBusConnection::connectToBus(address, "newconn");
        QVERIFY(con.isConnected());
        QVERIFY(!con.lastError().isValid());

        QDBusConnection::disconnectFromBus("newconn");
    }
}

void tst_QDBusConnection::registerObject_data()
{
    QTest::addColumn<QString>("path");

    QTest::newRow("/") << "/";
    QTest::newRow("/p1") << "/p1";
    QTest::newRow("/p2") << "/p2";
    QTest::newRow("/p1/q") << "/p1/q";
    QTest::newRow("/p1/q/r") << "/p1/q/r";
}

void tst_QDBusConnection::registerObject()
{
    QFETCH(QString, path);

    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    //QVERIFY(!callMethod(con, path));
    {
        // register one object at root:
        MyObject obj;
        QVERIFY(con.registerObject(path, &obj, QDBusConnection::ExportAllSlots));
        QCOMPARE(con.objectRegisteredAt(path), &obj);
        QVERIFY(callMethod(con, path));
        QCOMPARE(obj.path, path);
    }
    // make sure it's gone
    QVERIFY(!callMethod(con, path));
}

void tst_QDBusConnection::registerObject2()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    // make sure nothing is using our paths:
     QVERIFY(!callMethod(con, "/"));
     QVERIFY(!callMethod(con, "/p1"));
     QVERIFY(!callMethod(con, "/p2"));
     QVERIFY(!callMethod(con, "/p1/q"));
     QVERIFY(!callMethod(con, "/p1/q/r"));

    {
        // register one object at root:
        MyObject obj;
        QVERIFY(con.registerObject("/", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(callMethod(con, "/"));
        qDebug() << obj.path;
        QCOMPARE(obj.path, QString("/"));
    }
    // make sure it's gone
    QVERIFY(!callMethod(con, "/"));

    {
        // register one at an element:
        MyObject obj;
        QVERIFY(con.registerObject("/p1", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(!callMethod(con, "/"));
        QVERIFY(callMethod(con, "/p1"));
        qDebug() << obj.path;
        QCOMPARE(obj.path, QString("/p1"));

        // re-register it somewhere else
        QVERIFY(con.registerObject("/p2", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(callMethod(con, "/p1"));
        QCOMPARE(obj.path, QString("/p1"));
        QVERIFY(callMethod(con, "/p2"));
        QCOMPARE(obj.path, QString("/p2"));
    }
    // make sure it's gone
    QVERIFY(!callMethod(con, "/p1"));
    QVERIFY(!callMethod(con, "/p2"));

    {
        // register at a deep path
        MyObject obj;
        QVERIFY(con.registerObject("/p1/q/r", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(!callMethod(con, "/"));
        QVERIFY(!callMethod(con, "/p1"));
        QVERIFY(!callMethod(con, "/p1/q"));
        QVERIFY(callMethod(con, "/p1/q/r"));
        QCOMPARE(obj.path, QString("/p1/q/r"));
    }
    // make sure it's gone
    QVERIFY(!callMethod(con, "/p1/q/r"));

    {
        MyObject obj;
        QVERIFY(con.registerObject("/p1/q2", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(callMethod(con, "/p1/q2"));
        QCOMPARE(obj.path, QString("/p1/q2"));

        // try unregistering
        con.unregisterObject("/p1/q2");
        QVERIFY(!callMethod(con, "/p1/q2"));

        // register it again
        QVERIFY(con.registerObject("/p1/q2", &obj, QDBusConnection::ExportAllSlots));
        QVERIFY(callMethod(con, "/p1/q2"));
        QCOMPARE(obj.path, QString("/p1/q2"));

        // now try removing things around it:
        con.unregisterObject("/p2");
        QVERIFY(callMethod(con, "/p1/q2")); // unrelated object shouldn't affect

        con.unregisterObject("/p1");
        QVERIFY(callMethod(con, "/p1/q2")); // unregistering just the parent shouldn't affect it

        con.unregisterObject("/p1/q2/r");
        QVERIFY(callMethod(con, "/p1/q2")); // unregistering non-existing child shouldn't affect it either

        con.unregisterObject("/p1/q");
        QVERIFY(callMethod(con, "/p1/q2")); // unregistering sibling (before) shouldn't affect

        con.unregisterObject("/p1/r");
        QVERIFY(callMethod(con, "/p1/q2")); // unregistering sibling (after) shouldn't affect

        // now remove it:
        con.unregisterObject("/p1", QDBusConnection::UnregisterTree);
        QVERIFY(!callMethod(con, "/p1/q2")); // we removed the full tree
    }
}

void tst_QDBusConnection::registerQObjectChildren()
{
    // make sure no one is there
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(!callMethod(con, "/p1"));

    {
        MyObject obj, *a, *b, *c, *cc;

        a = new MyObject(&obj);
        a->setObjectName("a");

        b = new MyObject(&obj);
        b->setObjectName("b");

        c = new MyObject(&obj);
        c->setObjectName("c");

        cc = new MyObject(c);
        cc->setObjectName("cc");

        con.registerObject("/p1", &obj, QDBusConnection::ExportAllSlots |
                           QDBusConnection::ExportChildObjects);

        // make calls
        QVERIFY(callMethod(con, "/p1"));
        QCOMPARE(obj.callCount, 1);
        QVERIFY(callMethod(con, "/p1/a"));
        QCOMPARE(a->callCount, 1);
        QVERIFY(callMethod(con, "/p1/b"));
        QCOMPARE(b->callCount, 1);
        QVERIFY(callMethod(con, "/p1/c"));
        QCOMPARE(c->callCount, 1);
        QVERIFY(callMethod(con, "/p1/c/cc"));
        QCOMPARE(cc->callCount, 1);

        QVERIFY(!callMethod(con, "/p1/d"));
        QVERIFY(!callMethod(con, "/p1/c/abc"));

        // pull an object, see if it goes away:
        delete b;
        QVERIFY(!callMethod(con, "/p1/b"));

        delete c;
        QVERIFY(!callMethod(con, "/p1/c"));
        QVERIFY(!callMethod(con, "/p1/c/cc"));
    }

    QVERIFY(!callMethod(con, "/p1"));
    QVERIFY(!callMethod(con, "/p1/a"));
    QVERIFY(!callMethod(con, "/p1/b"));
    QVERIFY(!callMethod(con, "/p1/c"));
    QVERIFY(!callMethod(con, "/p1/c/cc"));
}

bool tst_QDBusConnection::callMethod(const QDBusConnection &conn, const QString &path)
{
    QDBusMessage msg = QDBusMessage::createMethodCall(conn.baseService(), path, "", "method");
    QDBusMessage reply = conn.call(msg, QDBus::Block/*WithGui*/);

    if (reply.type() != QDBusMessage::ReplyMessage)
        return false;
    if (MyObject::path == path) {
        QTest::compare_helper(true, "COMPARE()", __FILE__, __LINE__);
    } else {
        QTest::compare_helper(false, "Compared values are not the same",
                              QTest::toString(MyObject::path), QTest::toString(path),
                              "MyObject::path", "path", __FILE__, __LINE__);
        return false;
    }

    return true;
}

class TestObject : public QObject
{
Q_OBJECT
public:
    TestObject(QObject *parent = 0) : QObject(parent) {}
    ~TestObject() {}

    QString func;

public slots:
    void test0() { func = "test0"; }
    void test1(int i) { func = "test1 " + QString::number(i); }
    int test2() { func = "test2"; return 43; }
    int test3(int i) { func = "test2"; return i + 1; }
};

void tst_QDBusConnection::callSelf()
{
    TestObject testObject;
    QDBusConnection connection = QDBusConnection::sessionBus();
    QVERIFY(connection.registerObject("/test", &testObject,
            QDBusConnection::ExportAllContents));
    QCOMPARE(connection.objectRegisteredAt("/test"), &testObject);
    QVERIFY(connection.registerService(serviceName()));
    QDBusInterface interface(serviceName(), "/test");
    QVERIFY(interface.isValid());

    interface.call(QDBus::Block, "test0");
    QCOMPARE(testObject.func, QString("test0"));
    interface.call(QDBus::Block, "test1", 42);
    QCOMPARE(testObject.func, QString("test1 42"));
    QDBusMessage reply = interface.call(QDBus::Block, "test2");
    QCOMPARE(testObject.func, QString("test2"));
    QCOMPARE(reply.arguments().value(0).toInt(), 43);

    QDBusMessage msg = QDBusMessage::createMethodCall(serviceName(), "/test",
                                                      QString(), "test3");
    msg << 44;
    reply = connection.call(msg);
    QCOMPARE(reply.arguments().value(0).toInt(), 45);
}

void tst_QDBusConnection::callSelfByAnotherName_data()
{
    QTest::addColumn<int>("registerMethod");
    QTest::newRow("connection") << 0;
    QTest::newRow("connection-interface") << 1;
    QTest::newRow("direct") << 2;
}

void tst_QDBusConnection::callSelfByAnotherName()
{
    static int counter = 0;
    QString sname = serviceName() + QString::number(counter++);

    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    TestObject testObject;
    QVERIFY(con.registerObject("/test", &testObject,
            QDBusConnection::ExportAllContents));
    con.connect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged",
                QStringList() << sname << "",
                QString(), &QTestEventLoop::instance(), SLOT(exitLoop()));

    // register the name
    QFETCH(int, registerMethod);
    switch (registerMethod) {
    case 0:
        QVERIFY(con.registerService(sname));
        break;

    case 1:
        QVERIFY(con.interface()->registerService(sname).value() == QDBusConnectionInterface::ServiceRegistered);
        break;

    case 2: {
            // flag is DBUS_NAME_FLAG_DO_NOT_QUEUE = 0x04
            // reply is DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER = 1
            QDBusReply<uint> reply = con.interface()->call("RequestName", sname, 4u);
            QVERIFY(reply.value() == 1);
        }
    }

    struct Deregisterer {
        QDBusConnection con;
        QString sname;
        Deregisterer(const QDBusConnection &con, const QString &sname) : con(con), sname(sname) {}
        ~Deregisterer() { con.interface()->unregisterService(sname); }
    } deregisterer(con, sname);

    // give the connection a chance to find out that we're good to go
    QTestEventLoop::instance().enterLoop(2);
    con.disconnect("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus", "NameOwnerChanged",
                 QStringList() << sname << "",
                 QString(), &QTestEventLoop::instance(), SLOT(exitLoop()));
    QVERIFY(!QTestEventLoop::instance().timeout());

    // make the call
    QDBusMessage msg = QDBusMessage::createMethodCall(sname, "/test",
                                                      QString(), "test0");
    QDBusMessage reply = con.call(msg, QDBus::Block, 1000);

    QVERIFY(reply.type() == QDBusMessage::ReplyMessage);
}

void tst_QDBusConnection::multipleInterfacesInQObject()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(!callMethod(con, "/p1"));

    MyObject obj;
    con.registerObject("/p1", &obj, QDBusConnection::ExportAllSlots);

    // check if we can call the BaseObject's interface
    QDBusMessage msg = QDBusMessage::createMethodCall(con.baseService(), "/p1",
                                                      "local.BaseObject", "anotherMethod");
    QDBusMessage reply = con.call(msg, QDBus::Block);
    QCOMPARE(reply.type(), QDBusMessage::ReplyMessage);
    QVERIFY(reply.arguments().count() == 0);
}

void tst_QDBusConnection::slotsWithLessParameters()
{
    QDBusConnection con = QDBusConnection::sessionBus();

    QDBusMessage signal = QDBusMessage::createSignal("/", "com.trolltech.TestCase",
                                                     "oneSignal");
    signal << "one parameter";

    signalsReceived = 0;
    QVERIFY(con.connect(con.baseService(), signal.path(), signal.interface(),
                        signal.member(), this, SLOT(oneSlot())));
    QVERIFY(con.send(signal));
    QTest::qWait(100);
    QCOMPARE(signalsReceived, 1);

    // disconnect and try with a signature
    signalsReceived = 0;
    QVERIFY(con.disconnect(con.baseService(), signal.path(), signal.interface(),
                           signal.member(), this, SLOT(oneSlot())));
    QVERIFY(con.connect(con.baseService(), signal.path(), signal.interface(),
                        signal.member(), "s", this, SLOT(oneSlot())));
    QVERIFY(con.send(signal));
    QTest::qWait(100);
    QCOMPARE(signalsReceived, 1);
}

void tst_QDBusConnection::secondCallWithCallback()
{
    qDebug("Hello");
    QDBusConnection con = QDBusConnection::sessionBus();
    QDBusMessage msg = QDBusMessage::createMethodCall(con.baseService(), "/test", QString(),
                                                      "test0");
    con.callWithCallback(msg, this, SLOT(exitLoop()), SLOT(secondCallWithCallback()));
}

void tst_QDBusConnection::nestedCallWithCallback()
{
    TestObject testObject;
    QDBusConnection connection = QDBusConnection::sessionBus();
    QVERIFY(connection.registerObject("/test", &testObject,
            QDBusConnection::ExportAllContents));

    QDBusMessage msg = QDBusMessage::createMethodCall(connection.baseService(), "/test", QString(),
                                                      "ThisFunctionDoesntExist");
    signalsReceived = 0;

    connection.callWithCallback(msg, this, SLOT(exitLoop()), SLOT(secondCallWithCallback()), 10);
    QTestEventLoop::instance().enterLoop(15);
    QVERIFY(!QTestEventLoop::instance().timeout());
    QCOMPARE(signalsReceived, 1);
}

QString MyObject::path;
QTEST_MAIN(tst_QDBusConnection)

#include "tst_qdbusconnection.moc"

