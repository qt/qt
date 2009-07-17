/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QFile>
#include <QDebug>
#include "testtypes.h"

//TESTED_FILES=

class tst_binding : public QObject
{
    Q_OBJECT

public:
    tst_binding();
    virtual ~tst_binding();

public slots:
    void init();
    void cleanup();

private slots:
    void objectproperty_data();
    void objectproperty();
    void basicproperty_data();
    void basicproperty();

private:
    QmlEngine engine;
};

tst_binding::tst_binding()
{
}

tst_binding::~tst_binding()
{
}

void tst_binding::init()
{
}

void tst_binding::cleanup()
{
}

#define COMPONENT(filename, binding) \
    QmlComponent c(&engine); \
    { \
        QFile f(filename); \
        QVERIFY(f.open(QIODevice::ReadOnly)); \
        QByteArray data = f.readAll(); \
        data.replace("###", binding.toUtf8()); \
        c.setData(data, QUrl()); \
        QVERIFY(c.isReady()); \
    }

void tst_binding::objectproperty_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("binding");

    QTest::newRow("object.value") << "objectproperty.txt" << "object.value";
    QTest::newRow("object.value + 10") << "objectproperty.txt" << "object.value + 10";
}

void tst_binding::objectproperty()
{
    QFETCH(QString, file);
    QFETCH(QString, binding);

    COMPONENT(file, binding);

    MyQmlObject object1;
    MyQmlObject object2;

    MyQmlObject *object = qobject_cast<MyQmlObject *>(c.create());
    QVERIFY(object != 0);
    object->setObject(&object2);

    QBENCHMARK {
        object->setObject(&object1);
        object->setObject(&object2);
    }
}

void tst_binding::basicproperty_data()
{
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("binding");

    QTest::newRow("value") << "localproperty.txt" << "value";
    QTest::newRow("value + 10") << "localproperty.txt" << "value + 10";
    QTest::newRow("value + value + 10") << "localproperty.txt" << "value + value + 10";

    QTest::newRow("MyObject.value") << "idproperty.txt" << "MyObject.value";
    QTest::newRow("MyObject.value + 10") << "idproperty.txt" << "MyObject.value + 10";
    QTest::newRow("MyObject.value + MyObject.value + 10") << "idproperty.txt" << "MyObject.value + MyObject.value + 10";
}

void tst_binding::basicproperty()
{
    QFETCH(QString, file);
    QFETCH(QString, binding);

    COMPONENT(file, binding);

    MyQmlObject *object = qobject_cast<MyQmlObject *>(c.create());
    QVERIFY(object != 0);
    object->setValue(10);

    QBENCHMARK {
        object->setValue(1);
    }
}

QTEST_MAIN(tst_binding)
#include "tst_binding.moc"
