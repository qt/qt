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

#include <QtDBus>

#include "../qdbusmarshall/common.h"

const char *slotSpy;
QString valueSpy;

QT_BEGIN_NAMESPACE
namespace QTest {
    char *toString(QDBusMessage::MessageType t)
    {
        switch (t)
        {
        case QDBusMessage::InvalidMessage:
            return qstrdup("InvalidMessage");
        case QDBusMessage::MethodCallMessage:
            return qstrdup("MethodCallMessage");
        case QDBusMessage::ReplyMessage:
            return qstrdup("ReplyMessage");
        case QDBusMessage::ErrorMessage:
            return qstrdup("ErrorMessage");
        case QDBusMessage::SignalMessage:
            return qstrdup("SignalMessage");
        default:
            return 0;
        }
    }
}
QT_END_NAMESPACE

class tst_QDBusAbstractAdaptor: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase() { commonInit(); }
    void methodCalls_data();
    void methodCalls();
    void methodCallScriptable();
    void signalEmissions_data();
    void signalEmissions();
    void sameSignalDifferentPaths();
    void sameObjectDifferentPaths();
    void scriptableSignalOrNot();
    void overloadedSignalEmission_data();
    void overloadedSignalEmission();
    void readProperties();
    void readPropertiesInvalidInterface();
    void readPropertiesEmptyInterface_data();
    void readPropertiesEmptyInterface();
    void readAllProperties();
    void readAllPropertiesInvalidInterface();
    void readAllPropertiesEmptyInterface_data();
    void readAllPropertiesEmptyInterface();
    void writeProperties();

    void typeMatching_data();
    void typeMatching();

    void methodWithMoreThanOneReturnValue();
};

class QDBusSignalSpy: public QObject
{
    Q_OBJECT

public slots:
    void slot(const QDBusMessage &msg)
    {
        ++count;
        interface = msg.interface();
        name = msg.member();
        signature = msg.signature();
        path = msg.path();
        value.clear();
        if (msg.arguments().count())
            value = msg.arguments().at(0);
    }

public:
    QDBusSignalSpy() : count(0) { }

    int count;
    QString interface;
    QString name;
    QString signature;
    QString path;
    QVariant value;
};

class Interface1: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.Interface1")
public:
    Interface1(QObject *parent) : QDBusAbstractAdaptor(parent)
    { }
};

class Interface2: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.Interface2")
    Q_PROPERTY(QString prop1 READ prop1)
    Q_PROPERTY(QString prop2 READ prop2 WRITE setProp2 SCRIPTABLE true)
    Q_PROPERTY(QUrl nonDBusProperty READ nonDBusProperty)
public:
    Interface2(QObject *parent) : QDBusAbstractAdaptor(parent)
    { setAutoRelaySignals(true); }

    QString prop1() const
    { return QLatin1String("QString Interface2::prop1() const"); }

    QString prop2() const
    { return QLatin1String("QString Interface2::prop2() const"); }

    void setProp2(const QString &value)
    {
        slotSpy = "void Interface2::setProp2(const QString &)";
        valueSpy = value;
    }

    QUrl nonDBusProperty() const
    { return QUrl(); }

    void emitSignal(const QString &, const QVariant &)
    { emit signal(); }

public slots:
    void method()
    {
        slotSpy = "void Interface2::method()";
    }

    Q_SCRIPTABLE void scriptableMethod()
    {
        slotSpy = "void Interface2::scriptableMethod()";
    }

signals:
    void signal();
};

class Interface3: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.Interface3")
    Q_PROPERTY(QString prop1 READ prop1)
    Q_PROPERTY(QString prop2 READ prop2 WRITE setProp2)
    Q_PROPERTY(QString interface3prop READ interface3prop)
public:
    Interface3(QObject *parent) : QDBusAbstractAdaptor(parent)
    { setAutoRelaySignals(true); }

    QString prop1() const
    { return QLatin1String("QString Interface3::prop1() const"); }

    QString prop2() const
    { return QLatin1String("QString Interface3::prop2() const"); }

    void setProp2(const QString &value)
    {
        slotSpy = "void Interface3::setProp2(const QString &)";
        valueSpy = value;
    }

    QString interface3prop() const
    { return QLatin1String("QString Interface3::interface3prop() const"); }

    void emitSignal(const QString &name, const QVariant &value)
    {
        if (name == "signalVoid")
            emit signalVoid();
        else if (name == "signalInt")
            emit signalInt(value.toInt());
        else if (name == "signalString")
            emit signalString(value.toString());
    }

public slots:
    void methodVoid() { slotSpy = "void Interface3::methodVoid()"; }
    void methodInt(int) { slotSpy = "void Interface3::methodInt(int)"; }
    void methodString(QString) { slotSpy = "void Interface3::methodString(QString)"; }

    int methodStringString(const QString &s, QString &out)
    {
        slotSpy = "int Interface3::methodStringString(const QString &, QString &)";
        out = s;
        return 42;
    }

signals:
    void signalVoid();
    void signalInt(int);
    void signalString(const QString &);
};

class Interface4: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.Interface4")
    Q_PROPERTY(QString prop1 READ prop1)
    Q_PROPERTY(QString prop2 READ prop2 WRITE setProp2)
    Q_PROPERTY(QString interface4prop READ interface4prop)
public:
    Interface4(QObject *parent) : QDBusAbstractAdaptor(parent)
    { setAutoRelaySignals(true); }

    QString prop1() const
    { return QLatin1String("QString Interface4::prop1() const"); }

    QString prop2() const
    { return QLatin1String("QString Interface4::prop2() const"); }

    QString interface4prop() const
    { return QLatin1String("QString Interface4::interface4prop() const"); }

    void setProp2(const QString &value)
    {
        slotSpy = "void Interface4::setProp2(const QString &)";
        valueSpy = value;
    }

    void emitSignal(const QString &, const QVariant &value)
    {
        switch (value.type())
        {
        case QVariant::Invalid:
            emit signal();
            break;
        case QVariant::Int:
            emit signal(value.toInt());
            break;
        case QVariant::String:
            emit signal(value.toString());
            break;
        default:
            break;
        }
    }

public slots:
    void method() { slotSpy = "void Interface4::method()"; }
    void method(int) { slotSpy = "void Interface4::method(int)"; }
    void method(QString) { slotSpy = "void Interface4::method(QString)"; }

signals:
    void signal();
    void signal(int);
    void signal(const QString &);
};

class MyObject: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.MyObject")
public:
    Interface1 *if1;
    Interface2 *if2;
    Interface3 *if3;
    Interface4 *if4;

    MyObject(int n = 4)
        : if1(0), if2(0), if3(0), if4(0)
    {
        switch (n)
        {
        case 4:
            if4 = new Interface4(this);
        case 3:
            if3 = new Interface3(this);
        case 2:
            if2 = new Interface2(this);
        case 1:
            if1 = new Interface1(this);
        }
    }

    void emitSignal(const QString &name, const QVariant &value)
    {
        if (name == "scriptableSignalVoid")
            emit scriptableSignalVoid();
        else if (name == "scriptableSignalInt")
            emit scriptableSignalInt(value.toInt());
        else if (name == "scriptableSignalString")
            emit scriptableSignalString(value.toString());
        else if (name == "nonScriptableSignalVoid")
            emit nonScriptableSignalVoid();
    }

signals:
    Q_SCRIPTABLE void scriptableSignalVoid();
    Q_SCRIPTABLE void scriptableSignalInt(int);
    Q_SCRIPTABLE void scriptableSignalString(QString);
    void nonScriptableSignalVoid();
};

class TypesInterface: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "local.TypesInterface")
public:
    TypesInterface(QObject *parent)
        : QDBusAbstractAdaptor(parent)
    { }

    union
    {
        bool b;
        uchar uc;
        short s;
        ushort us;
        int i;
        uint ui;
        qlonglong ll;
        qulonglong ull;
        double d;
    } dataSpy;
    QVariant variantSpy;
    QString stringSpy;
    QVariantList listSpy;
    QStringList stringlistSpy;
    QByteArray bytearraySpy;
    QVariantMap mapSpy;
    StringStringMap ssmapSpy;
    LLDateTimeMap lldtmapSpy;
    MyStruct structSpy;

public slots:
    void methodBool(bool b)
    {
        slotSpy = "void TypesInterface::methodBool(bool)";
        dataSpy.b = b;
    }

    void methodUChar(uchar uc)
    {
        slotSpy = "void TypesInterface::methodUChar(uchar)";
        dataSpy.uc = uc;
    }

    void methodShort(short s)
    {
        slotSpy = "void TypesInterface::methodShort(short)";
        dataSpy.s = s;
    }

    void methodUShort(ushort us)
    {
        slotSpy = "void TypesInterface::methodUShort(ushort)";
        dataSpy.us = us;
    }

    void methodInt(int i)
    {
        slotSpy = "void TypesInterface::methodInt(int)";
        dataSpy.i = i;
    }

    void methodUInt(uint ui)
    {
        slotSpy = "void TypesInterface::methodUInt(uint)";
        dataSpy.ui = ui;
    }

    void methodLongLong(qlonglong ll)
    {
        slotSpy = "void TypesInterface::methodLongLong(qlonglong)";
        dataSpy.ll = ll;
    }

    void methodULongLong(qulonglong ull)
    {
        slotSpy = "void TypesInterface::methodULongLong(qulonglong)";
        dataSpy.ull = ull;
    }

    void methodDouble(double d)
    {
        slotSpy = "void TypesInterface::methodDouble(double)";
        dataSpy.d = d;
    }

    void methodString(const QString &s)
    {
        slotSpy = "void TypesInterface::methodString(const QString &)";
        stringSpy = s;
    }

    void methodObjectPath(const QDBusObjectPath &op)
    {
        slotSpy = "void TypesInterface::methodObjectPath(const QDBusObjectPath &)";
        stringSpy = op.path();
    }

    void methodSignature(const QDBusSignature &s)
    {
        slotSpy = "void TypesInterface::methodSignature(const QDBusSignature &)";
        stringSpy = s.signature();
    }

    void methodVariant(const QDBusVariant &v)
    {
        slotSpy = "void TypesInterface::methodVariant(const QDBusVariant &)";
        variantSpy = v.variant();
    }

    void methodList(const QVariantList &l)
    {
        slotSpy = "void TypesInterface::methodList(const QVariantList &)";
        listSpy = l;
    }

    void methodStringList(const QStringList &sl)
    {
        slotSpy = "void TypesInterface::methodStringList(const QStringList &)";
        stringlistSpy = sl;
    }

    void methodByteArray(const QByteArray &ba)
    {
        slotSpy = "void TypesInterface::methodByteArray(const QByteArray &)";
        bytearraySpy = ba;
    }

    void methodMap(const QVariantMap &m)
    {
        slotSpy = "void TypesInterface::methodMap(const QVariantMap &)";
        mapSpy = m;
    }

    void methodSSMap(const StringStringMap &ssmap)
    {
        slotSpy = "void TypesInterface::methodSSMap(const StringStringMap &)";
        ssmapSpy = ssmap;
    }

    void methodLLDateTimeMap(const LLDateTimeMap &lldtmap)
    {
        slotSpy = "void TypesInterface::methodLLDateTimeMap(const LLDateTimeMap &)";
        lldtmapSpy = lldtmap;
    }

    void methodStruct(const MyStruct &s)
    {
        slotSpy = "void TypesInterface::methodStruct(const MyStruct &)";
        structSpy = s;
    }

    bool retrieveBool()
    {
        return dataSpy.b;
    }

    uchar retrieveUChar()
    {
        return dataSpy.uc;
    }

    short retrieveShort()
    {
        return dataSpy.s;
    }

    ushort retrieveUShort()
    {
        return dataSpy.us;
    }

    int retrieveInt()
    {
        return dataSpy.i;
    }

    uint retrieveUInt()
    {
        return dataSpy.ui;
    }

    qlonglong retrieveLongLong()
    {
        return dataSpy.ll;
    }

    qulonglong retrieveULongLong()
    {
        return dataSpy.ull;
    }

    double retrieveDouble()
    {
        return dataSpy.d;
    }

    QString retrieveString()
    {
        return stringSpy;
    }

    QDBusObjectPath retrieveObjectPath()
    {
        return QDBusObjectPath(stringSpy);
    }

    QDBusSignature retrieveSignature()
    {
        return QDBusSignature(stringSpy);
    }

    QDBusVariant retrieveVariant()
    {
        return QDBusVariant(variantSpy);
    }

    QVariantList retrieveList()
    {
        return listSpy;
    }

    QStringList retrieveStringList()
    {
        return stringlistSpy;
    }

    QByteArray retrieveByteArray()
    {
        return bytearraySpy;
    }

    QVariantMap retrieveMap()
    {
        return mapSpy;
    }

    StringStringMap retrieveSSMap()
    {
        return ssmapSpy;
    }

    LLDateTimeMap retrieveLLDateTimeMap()
    {
        return lldtmapSpy;
    }

    MyStruct retrieveStruct()
    {
        return structSpy;
    }
};

void tst_QDBusAbstractAdaptor::methodCalls_data()
{
    QTest::addColumn<int>("nInterfaces");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
    QTest::newRow("4") << 4;
}

void tst_QDBusAbstractAdaptor::methodCalls()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    //QDBusInterface emptycon.baseService(), "/", QString());

    {
        // must fail: no object
        QDBusInterface if1(con.baseService(), "/", "local.Interface1", con);
        QCOMPARE(if1.call(QDBus::BlockWithGui, "method").type(), QDBusMessage::ErrorMessage);
    }

    QFETCH(int, nInterfaces);
    MyObject obj(nInterfaces);
    con.registerObject("/", &obj);

    QDBusInterface if1(con.baseService(), "/", "local.Interface1", con);
    QDBusInterface if2(con.baseService(), "/", "local.Interface2", con);
    QDBusInterface if3(con.baseService(), "/", "local.Interface3", con);
    QDBusInterface if4(con.baseService(), "/", "local.Interface4", con);

    // must fail: no such method
    QCOMPARE(if1.call(QDBus::BlockWithGui, "method").type(), QDBusMessage::ErrorMessage);
    if (!nInterfaces--)
        return;
    if (!nInterfaces--)
        return;

    // simple call: one such method exists
    QCOMPARE(if2.call(QDBus::BlockWithGui, "method").type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface2::method()");
    if (!nInterfaces--)
        return;

    // multiple methods in multiple interfaces, no name overlap
    QCOMPARE(if1.call(QDBus::BlockWithGui, "methodVoid").type(), QDBusMessage::ErrorMessage);
    QCOMPARE(if1.call(QDBus::BlockWithGui, "methodInt").type(), QDBusMessage::ErrorMessage);
    QCOMPARE(if1.call(QDBus::BlockWithGui, "methodString").type(), QDBusMessage::ErrorMessage);
    QCOMPARE(if2.call(QDBus::BlockWithGui, "methodVoid").type(), QDBusMessage::ErrorMessage);
    QCOMPARE(if2.call(QDBus::BlockWithGui, "methodInt").type(), QDBusMessage::ErrorMessage);
    QCOMPARE(if2.call(QDBus::BlockWithGui, "methodString").type(), QDBusMessage::ErrorMessage);

    QCOMPARE(if3.call(QDBus::BlockWithGui, "methodVoid").type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface3::methodVoid()");
    QCOMPARE(if3.call(QDBus::BlockWithGui, "methodInt", 42).type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface3::methodInt(int)");
    QCOMPARE(if3.call(QDBus::BlockWithGui, "methodString", QString("")).type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface3::methodString(QString)");

    if (!nInterfaces--)
        return;

    // method overloading: different interfaces
    QCOMPARE(if4.call(QDBus::BlockWithGui, "method").type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface4::method()");

    // method overloading: different parameters
    QCOMPARE(if4.call(QDBus::BlockWithGui, "method.i", 42).type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface4::method(int)");
    QCOMPARE(if4.call(QDBus::BlockWithGui, "method.s", QString()).type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface4::method(QString)");

}

void tst_QDBusAbstractAdaptor::methodCallScriptable()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj(2);
    con.registerObject("/", &obj);

    QDBusInterface if2(con.baseService(), "/", "local.Interface2", con);

    QCOMPARE(if2.call(QDBus::BlockWithGui,"scriptableMethod").type(), QDBusMessage::ReplyMessage);
    QCOMPARE(slotSpy, "void Interface2::scriptableMethod()");
}

static void emitSignal(MyObject *obj, const QString &iface, const QString &name,
                       const QVariant &parameter)
{
    if (iface.endsWith('2'))
        obj->if2->emitSignal(name, parameter);
    else if (iface.endsWith('3'))
        obj->if3->emitSignal(name, parameter);
    else if (iface.endsWith('4'))
        obj->if4->emitSignal(name, parameter);
    else
        obj->emitSignal(name, parameter);

    QTest::qWait(200);
}

void tst_QDBusAbstractAdaptor::signalEmissions_data()
{
    QTest::addColumn<QString>("interface");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("signature");
    QTest::addColumn<QVariant>("parameter");

    QTest::newRow("Interface2.signal") << "local.Interface2" << "signal" << QString() << QVariant();
    QTest::newRow("Interface3.signalVoid") << "local.Interface3" << "signalVoid" << QString() << QVariant();
    QTest::newRow("Interface3.signalInt") << "local.Interface3" << "signalInt" << "i" << QVariant(1);
    QTest::newRow("Interface3.signalString") << "local.Interface3" << "signalString" << "s" << QVariant("foo");
    QTest::newRow("MyObject.scriptableSignalVoid") << "local.MyObject" << "scriptableSignalVoid" << QString() << QVariant();
    QTest::newRow("MyObject.scriptableSignalInt") << "local.MyObject" << "scriptableSignalInt" << "i" << QVariant(1);
    QTest::newRow("MyObject.nySignalString") << "local.MyObject" << "scriptableSignalString" << "s" << QVariant("foo");
}

void tst_QDBusAbstractAdaptor::signalEmissions()
{
    QFETCH(QString, interface);
    QFETCH(QString, name);
    QFETCH(QVariant, parameter);

    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());
    con.registerService("com.trolltech.tst_QDBusAbstractAdaptor");

    MyObject obj(3);
    con.registerObject("/", &obj, QDBusConnection::ExportAdaptors
                                  | QDBusConnection::ExportScriptableSignals);

    // connect all signals and emit only one
    {
        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/", "local.Interface2", "signal",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.Interface3", "signalVoid",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.Interface3", "signalInt",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.Interface3", "signalString",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.MyObject", "scriptableSignalVoid",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.MyObject", "scriptableSignalInt",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.MyObject", "scriptableSignalString",
                    &spy, SLOT(slot(QDBusMessage)));

        emitSignal(&obj, interface, name, parameter);

        QCOMPARE(spy.count, 1);
        QCOMPARE(spy.interface, interface);
        QCOMPARE(spy.name, name);
        QTEST(spy.signature, "signature");
        QCOMPARE(spy.value, parameter);
    }

    // connect one signal and emit them all
    {
        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/", interface, name, &spy, SLOT(slot(QDBusMessage)));
        emitSignal(&obj, "local.Interface2", "signal", QVariant());
        emitSignal(&obj, "local.Interface3", "signalVoid", QVariant());
        emitSignal(&obj, "local.Interface3", "signalInt", QVariant(1));
        emitSignal(&obj, "local.Interface3", "signalString", QVariant("foo"));
        emitSignal(&obj, "local.MyObject", "scriptableSignalVoid", QVariant());
        emitSignal(&obj, "local.MyObject", "scriptableSignalInt", QVariant(1));
        emitSignal(&obj, "local.MyObject", "scriptableSignalString", QVariant("foo"));

        QCOMPARE(spy.count, 1);
        QCOMPARE(spy.interface, interface);
        QCOMPARE(spy.name, name);
        QTEST(spy.signature, "signature");
        QCOMPARE(spy.value, parameter);
    }
}

void tst_QDBusAbstractAdaptor::sameSignalDifferentPaths()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj(2);

    con.registerObject("/p1",&obj);
    con.registerObject("/p2",&obj);

    QDBusSignalSpy spy;
    con.connect(con.baseService(), "/p1", "local.Interface2", "signal", &spy, SLOT(slot(QDBusMessage)));
    obj.if2->emitSignal(QString(), QVariant());
    QTest::qWait(200);

    QCOMPARE(spy.count, 1);
    QCOMPARE(spy.interface, QString("local.Interface2"));
    QCOMPARE(spy.name, QString("signal"));
    QVERIFY(spy.signature.isEmpty());

    // now connect the other one
    spy.count = 0;
    con.connect(con.baseService(), "/p2", "local.Interface2", "signal", &spy, SLOT(slot(QDBusMessage)));
    obj.if2->emitSignal(QString(), QVariant());
    QTest::qWait(200);

    QCOMPARE(spy.count, 2);
}

void tst_QDBusAbstractAdaptor::sameObjectDifferentPaths()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj(2);

    con.registerObject("/p1",&obj);
    con.registerObject("/p2",&obj, 0); // don't export anything

    QDBusSignalSpy spy;
    con.connect(con.baseService(), "/p1", "local.Interface2", "signal", &spy, SLOT(slot(QDBusMessage)));
    con.connect(con.baseService(), "/p2", "local.Interface2", "signal", &spy, SLOT(slot(QDBusMessage)));
    obj.if2->emitSignal(QString(), QVariant());
    QTest::qWait(200);

    QCOMPARE(spy.count, 1);
    QCOMPARE(spy.interface, QString("local.Interface2"));
    QCOMPARE(spy.name, QString("signal"));
    QVERIFY(spy.signature.isEmpty());
}

void tst_QDBusAbstractAdaptor::scriptableSignalOrNot()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    {
        MyObject obj(0);

        con.registerObject("/p1",&obj, QDBusConnection::ExportScriptableSignals);
        con.registerObject("/p2",&obj, 0); // don't export anything

        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/p1", "local.MyObject", "scriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/p2", "local.MyObject", "scriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/p1", "local.MyObject", "nonScriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/p2", "local.MyObject", "nonScriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        obj.emitSignal("scriptableSignalVoid", QVariant());
        obj.emitSignal("nonScriptableSignalVoid", QVariant());
        QTest::qWait(200);

        QCOMPARE(spy.count, 1);     // only /p1 must have emitted
        QCOMPARE(spy.interface, QString("local.MyObject"));
        QCOMPARE(spy.name, QString("scriptableSignalVoid"));
        QCOMPARE(spy.path, QString("/p1"));
        QVERIFY(spy.signature.isEmpty());
    }

    {
        MyObject obj(0);

        con.registerObject("/p1",&obj, QDBusConnection::ExportScriptableSignals);
        con.registerObject("/p2",&obj, QDBusConnection::ExportScriptableSignals
                                       | QDBusConnection::ExportNonScriptableSignals);

        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/p1", "local.MyObject", "nonScriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/p2", "local.MyObject", "nonScriptableSignalVoid", &spy, SLOT(slot(QDBusMessage)));
        obj.emitSignal("nonScriptableSignalVoid", QVariant());
        QTest::qWait(200);

        QCOMPARE(spy.count, 1);     // only /p2 must have emitted now
        QCOMPARE(spy.interface, QString("local.MyObject"));
        QCOMPARE(spy.name, QString("nonScriptableSignalVoid"));
        QCOMPARE(spy.path, QString("/p2"));
        QVERIFY(spy.signature.isEmpty());
    }

    {
        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/p1", "local.MyObject", "destroyed", &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/p2", "local.MyObject", "destroyed", &spy, SLOT(slot(QDBusMessage)));

        {
            MyObject obj(0);

            con.registerObject("/p1",&obj, QDBusConnection::ExportScriptableSignals);
            con.registerObject("/p2",&obj, QDBusConnection::ExportScriptableSignals
                                           | QDBusConnection::ExportNonScriptableSignals);
        } // <--- QObject emits the destroyed(QObject*) signal at this point

        QTest::qWait(200);

        QCOMPARE(spy.count, 0);
    }
}

void tst_QDBusAbstractAdaptor::overloadedSignalEmission_data()
{
    QTest::addColumn<QString>("signature");
    QTest::addColumn<QVariant>("parameter");
    QTest::newRow("void") << QString("") << QVariant();
    QTest::newRow("int") << "i" << QVariant(1);
    QTest::newRow("string") << "s" << QVariant("foo");
}

void tst_QDBusAbstractAdaptor::overloadedSignalEmission()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QString interface = "local.Interface4";
    QString name = "signal";
    QFETCH(QVariant, parameter);
    //QDBusInterface *if4 = new QDBusInterface(con.baseService(), "/", interface, con);

    // connect all signals and emit only one
    {
        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/", "local.Interface4", "signal", "",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.Interface4", "signal", "i",
                    &spy, SLOT(slot(QDBusMessage)));
        con.connect(con.baseService(), "/", "local.Interface4", "signal", "s",
                    &spy, SLOT(slot(QDBusMessage)));

        emitSignal(&obj, interface, name, parameter);

        QCOMPARE(spy.count, 1);
        QCOMPARE(spy.interface, interface);
        QCOMPARE(spy.name, name);
        QTEST(spy.signature, "signature");
        QCOMPARE(spy.value, parameter);
    }

    QFETCH(QString, signature);
    // connect one signal and emit them all
    {
        QDBusSignalSpy spy;
        con.connect(con.baseService(), "/", interface, name, signature, &spy, SLOT(slot(QDBusMessage)));
        emitSignal(&obj, "local.Interface4", "signal", QVariant());
        emitSignal(&obj, "local.Interface4", "signal", QVariant(1));
        emitSignal(&obj, "local.Interface4", "signal", QVariant("foo"));

        QCOMPARE(spy.count, 1);
        QCOMPARE(spy.interface, interface);
        QCOMPARE(spy.name, name);
        QTEST(spy.signature, "signature");
        QCOMPARE(spy.value, parameter);
    }
}

void tst_QDBusAbstractAdaptor::readProperties()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);
    for (int i = 2; i <= 4; ++i) {
        QString name = QString("Interface%1").arg(i);

        for (int j = 1; j <= 2; ++j) {
            QString propname = QString("prop%1").arg(j);
            QDBusReply<QVariant> reply =
                properties.call(QDBus::BlockWithGui, "Get", "local." + name, propname);
            QVariant value = reply;

            QCOMPARE(value.userType(), int(QVariant::String));
            QCOMPARE(value.toString(), QString("QString %1::%2() const").arg(name, propname));
        }
    }
}

void tst_QDBusAbstractAdaptor::readPropertiesInvalidInterface()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);

    // test an invalid interface:
    QDBusReply<QVariant> reply = properties.call(QDBus::BlockWithGui, "Get", "local.DoesntExist", "prop1");
    QVERIFY(!reply.isValid());
}

void tst_QDBusAbstractAdaptor::readPropertiesEmptyInterface_data()
{
    QTest::addColumn<QVariantMap>("expectedProperties");
    QTest::addColumn<bool>("existing");

    QVariantMap expectedProperties;
    expectedProperties["prop1"] = QVariant();
    expectedProperties["prop2"] = QVariant();
    expectedProperties["interface3prop"] = "QString Interface3::interface3prop() const";
    expectedProperties["interface4prop"] = "QString Interface4::interface4prop() const";
    QTest::newRow("existing") << expectedProperties << true;

    expectedProperties.clear();
    expectedProperties["prop5"] = QVariant();
    expectedProperties["foobar"] = QVariant();
    QTest::newRow("non-existing") << expectedProperties << false;
}

void tst_QDBusAbstractAdaptor::readPropertiesEmptyInterface()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);

    QFETCH(QVariantMap, expectedProperties);
    QFETCH(bool, existing);

    QVariantMap::ConstIterator it = expectedProperties.constBegin();
    for ( ; it != expectedProperties.constEnd(); ++it) {
        QDBusReply<QVariant> reply = properties.call(QDBus::BlockWithGui, "Get", "", it.key());

        if (existing) {
            QVERIFY2(reply.isValid(), qPrintable(it.key()));
        } else {
            QVERIFY2(!reply.isValid(), qPrintable(it.key()));
            continue;
        }

        QCOMPARE(int(reply.value().type()), int(QVariant::String));
        if (it.value().isValid())
            QCOMPARE(reply.value().toString(), it.value().toString());
    }
}

void tst_QDBusAbstractAdaptor::readAllProperties()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);
    for (int i = 2; i <= 4; ++i) {
        QString name = QString("Interface%1").arg(i);
        QDBusReply<QVariantMap> reply =
            properties.call(QDBus::BlockWithGui, "GetAll", "local." + name);

        for (int j = 1; j <= 2; ++j) {
            QString propname = QString("prop%1").arg(j);
            QVERIFY2(reply.value().contains(propname),
                     qPrintable(propname + " on " + name));
            QVariant value = reply.value().value(propname);

            QCOMPARE(value.userType(), int(QVariant::String));
            QCOMPARE(value.toString(), QString("QString %1::%2() const").arg(name, propname));
        }
    }
}

void tst_QDBusAbstractAdaptor::readAllPropertiesInvalidInterface()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);

    // test an invalid interface:
    QDBusReply<QVariantMap> reply = properties.call(QDBus::BlockWithGui, "GetAll", "local.DoesntExist");
    QVERIFY(!reply.isValid());
}

void tst_QDBusAbstractAdaptor::readAllPropertiesEmptyInterface_data()
{
    readPropertiesEmptyInterface_data();
}

void tst_QDBusAbstractAdaptor::readAllPropertiesEmptyInterface()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);

    QDBusReply<QVariantMap> reply = properties.call(QDBus::BlockWithGui, "GetAll", "");
    QVERIFY(reply.isValid());

    QVariantMap allprops = reply;

    QFETCH(QVariantMap, expectedProperties);
    QFETCH(bool, existing);

    QVariantMap::ConstIterator it = expectedProperties.constBegin();
    if (existing) {
        for ( ; it != expectedProperties.constEnd(); ++it) {
            QVERIFY2(allprops.contains(it.key()), qPrintable(it.key()));

            QVariant propvalue = allprops.value(it.key());
            QVERIFY2(!propvalue.isNull(), qPrintable(it.key()));
            QVERIFY2(propvalue.isValid(), qPrintable(it.key()));

            QString stringvalue = propvalue.toString();
            QVERIFY2(!stringvalue.isEmpty(), qPrintable(it.key()));

            if (it.value().isValid())
                QCOMPARE(stringvalue, it.value().toString());

            // remove this property from the map
            allprops.remove(it.key());
        }

        QVERIFY2(allprops.isEmpty(),
                 qPrintable(QStringList(allprops.keys()).join(" ")));
    } else {
        for ( ; it != expectedProperties.constEnd(); ++it)
            QVERIFY2(!allprops.contains(it.key()), qPrintable(it.key()));
    }
}

void tst_QDBusAbstractAdaptor::writeProperties()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QDBusInterface properties(con.baseService(), "/", "org.freedesktop.DBus.Properties", con);
    for (int i = 2; i <= 4; ++i) {
        QString name = QString("Interface%1").arg(i);

        valueSpy.clear();
        properties.call(QDBus::BlockWithGui, "Set", "local." + name, QString("prop1"),
                        qVariantFromValue(QDBusVariant(name)));
        QVERIFY(valueSpy.isEmpty()); // call mustn't have succeeded

        properties.call(QDBus::BlockWithGui, "Set", "local." + name, QString("prop2"),
                        qVariantFromValue(QDBusVariant(name)));
        QCOMPARE(valueSpy, name);
        QCOMPARE(QString(slotSpy), QString("void %1::setProp2(const QString &)").arg(name));
    }
}

#if 0
void tst_QDBusAbstractAdaptor::adaptorIntrospection_data()
{
    methodCalls_data();
}

void tst_QDBusAbstractAdaptor::adaptorIntrospection()
{
    QDBusConnection con = QDBus::sessionBus();
    QVERIFY(con.isConnected());

    QObject obj;
    con.registerObject("/", &obj);

    QFETCH(int, nInterfaces);
    switch (nInterfaces)
    {
    case 4:
        new Interface4(&obj);
    case 3:
        new Interface3(&obj);
    case 2:
        new Interface2(&obj);
    case 1:
        new Interface1(&obj);
    }

    QDBusObject dobj = con.findObject(con.baseService(), "/");
    QVERIFY(dobj.isValid());

    QString xml = dobj.introspect();
    QVERIFY(!xml.isEmpty());

    QStringList interfaces = dobj.interfaces();
    QCOMPARE(interfaces.count(), nInterfaces + 2);
    switch (nInterfaces)
    {
    case 4: {
        QVERIFY(interfaces.contains("local.Interface4"));
        QDBusInterface iface(dobj, "local.Interface4");
        QCOMPARE(iface.methodData(), Interface4::methodData);
        QCOMPARE(iface.signalData(), Interface4::signalData);
        QCOMPARE(iface.propertyData(), Interface4::propertyData);
    }
    case 3: {
        QVERIFY(interfaces.contains("local.Interface3"));
        QDBusInterface iface(dobj, "local.Interface3");
        QCOMPARE(iface.methodData(), Interface3::methodData);
        QCOMPARE(iface.signalData(), Interface3::signalData);
        QCOMPARE(iface.propertyData(), Interface3::propertyData);
    }
    case 2: {
        QVERIFY(interfaces.contains("local.Interface2"));
        QDBusInterface iface(dobj, "local.Interface2");
        QCOMPARE(iface.methodData(), Interface2::methodData);
        QCOMPARE(iface.signalData(), Interface2::signalData);
        QCOMPARE(iface.propertyData(), Interface2::propertyData);
    }
    case 1: {
        QVERIFY(interfaces.contains("local.Interface1"));
        QDBusInterface iface(dobj, "local.Interface1");
        QCOMPARE(iface.methodData(), Interface1::methodData);
        QCOMPARE(iface.signalData(), Interface1::signalData);
        QCOMPARE(iface.propertyData(), Interface1::propertyData);
    }
    }
}

void tst_QDBusAbstractAdaptor::objectTreeIntrospection()
{
    QDBusConnection con = QDBus::sessionBus();
    QVERIFY(con.isConnected());

    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.isEmpty());
    }

    QObject root;
    con.registerObject("/", &root);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.isEmpty());
    }

    QObject p1;
    con.registerObject("/p1", &p1);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.contains("p1"));
    }

    con.unregisterObject("/");
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.contains("p1"));
    }

    con.registerObject("/p1/q/r", &root);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/p1");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.contains("q"));
    }
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/p1/q");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.contains("r"));
    }

    con.unregisterObject("/p1", QDBusConnection::UnregisterTree);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.isEmpty());
    }

    QObject p2;
    con.registerObject("/p2", &p2, QDBusConnection::ExportChildObjects);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(!tree.childObjects.contains("p1"));
        QVERIFY(tree.childObjects.contains("p2"));
    }

    QObject q;
    q.setParent(&p2);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/p2");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(!tree.childObjects.contains("q"));
    }

    q.setObjectName("q");
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/p2");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(tree.childObjects.contains("q"));
    }

    q.setParent(0);
    {
        QDBusObject dobj = con.findObject(con.baseService(), "/p2");
        QString xml = dobj.introspect();

        QDBusIntrospection::Object tree =
            QDBusIntrospection::parseObject(xml);
        QVERIFY(!tree.childObjects.contains("q"));
    }
}
#endif

void tst_QDBusAbstractAdaptor::typeMatching_data()
{
    QTest::addColumn<QString>("basename");
    QTest::addColumn<QString>("signature");
    QTest::addColumn<QVariant>("value");

    QTest::newRow("bool") << "Bool" << "b" << QVariant(true);
    QTest::newRow("byte") << "UChar" << "y" << qVariantFromValue(uchar(42));
    QTest::newRow("short") << "Short" << "n" << qVariantFromValue(short(-43));
    QTest::newRow("ushort") << "UShort" << "q" << qVariantFromValue(ushort(44));
    QTest::newRow("int") << "Int" << "i" << QVariant(42);
    QTest::newRow("uint") << "UInt" << "u" << QVariant(42U);
    QTest::newRow("qlonglong") << "LongLong" << "x" << QVariant(Q_INT64_C(42));
    QTest::newRow("qulonglong") << "ULongLong" << "t" << QVariant(Q_UINT64_C(42));
    QTest::newRow("double") << "Double" << "d" << QVariant(2.5);
    QTest::newRow("string") << "String" << "s" << QVariant("Hello, World!");

    QTest::newRow("variant") << "Variant" << "v" << qVariantFromValue(QDBusVariant("Hello again!"));
    QTest::newRow("list") << "List" << "av" << QVariant(QVariantList()
                                                        << 42
                                                        << QString("foo")
                                                        << QByteArray("bar")
                                                        << qVariantFromValue(QDBusVariant(QString("baz"))));
    QTest::newRow("stringlist") << "StringList" << "as" << QVariant(QStringList() << "Hello" << "world");
    QTest::newRow("bytearray") << "ByteArray" << "ay" << QVariant(QByteArray("foo"));

    QVariantMap map;
    map["one"] = 1;       // int
    map["The answer to life, the Universe and everything"] = 42u; // uint
    map["In the beginning..."] = QString("There was nothing"); // string
    map["but Unix came and said"] = QByteArray("\"Hello, World\""); // bytearray
    map["two"] = qVariantFromValue(short(2)); // short
    QTest::newRow("map") << "Map" << "a{sv}" << QVariant(map);

    StringStringMap ssmap;
    ssmap["a"] = "A";
    ssmap["A"] = "a";
    QTest::newRow("ssmap") << "SSMap" << "a{ss}" << qVariantFromValue(ssmap);

    LLDateTimeMap lldtmap;
    lldtmap[-1] = QDateTime();
    QDateTime now = QDateTime::currentDateTime();
    lldtmap[now.toTime_t()] = now; // array of struct of int64 and struct of 3 ints and struct of 4 ints and int
    QTest::newRow("lldtmap") << "LLDateTimeMap" << "a{x((iii)(iiii)i)}" << qVariantFromValue(lldtmap);

    MyStruct s;
    s.i = 42;
    s.s = "A value";
    QTest::newRow("struct") << "Struct" << "(is)" << qVariantFromValue(s);
}

void tst_QDBusAbstractAdaptor::typeMatching()
{
    QObject obj;
    new TypesInterface(&obj);

    QDBusConnection con = QDBusConnection::sessionBus();
    con.registerObject("/types", &obj);

    QFETCH(QString, basename);
    QFETCH(QString, signature);
    QFETCH(QVariant, value);

    QDBusMessage reply;
    QDBusInterface iface(con.baseService(), "/types", "local.TypesInterface", con);

    reply = iface.callWithArgumentList(QDBus::BlockWithGui, "method" + basename,
                                        QVariantList() << value);
    QCOMPARE(reply.type(), QDBusMessage::ReplyMessage);

    reply = iface.call(QDBus::BlockWithGui, "retrieve" + basename);
    QCOMPARE(reply.type(), QDBusMessage::ReplyMessage);
    QCOMPARE(reply.arguments().count(), 1);

    const QVariant &retval = reply.arguments().at(0);
    QVERIFY(compare(retval, value));
}

void tst_QDBusAbstractAdaptor::methodWithMoreThanOneReturnValue()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());

    MyObject obj;
    con.registerObject("/", &obj);

    QString testString = "This is a test string.";

    QDBusInterface remote(con.baseService(), "/", "local.Interface3", con);
    QDBusMessage reply = remote.call(QDBus::BlockWithGui, "methodStringString", testString);
    QVERIFY(reply.arguments().count() == 2);

    QDBusReply<int> intreply = reply;
    QVERIFY(intreply.isValid());
    QCOMPARE(intreply.value(), 42);

    QCOMPARE(reply.arguments().at(1).userType(), int(QVariant::String));
    QCOMPARE(qdbus_cast<QString>(reply.arguments().at(1)), testString);
}

QTEST_MAIN(tst_QDBusAbstractAdaptor)

#include "tst_qdbusabstractadaptor.moc"
