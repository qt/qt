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


class tst_qmlcomponent : public QObject
{
    Q_OBJECT

public:
    tst_qmlcomponent();
    virtual ~tst_qmlcomponent();

public slots:
    void init();
    void cleanup();

private slots:
    void creation_data();
    void creation();

private:
    QmlEngine engine;
};

tst_qmlcomponent::tst_qmlcomponent()
{
}

tst_qmlcomponent::~tst_qmlcomponent()
{
}

void tst_qmlcomponent::init()
{
}

void tst_qmlcomponent::cleanup()
{
}

void tst_qmlcomponent::creation_data()
{
    QTest::addColumn<QString>("file");

    QTest::newRow("Object") << "object.txt";
    QTest::newRow("MyQmlObject") << "myqmlobject.txt";
    QTest::newRow("MyQmlObject: basic binding") << "myqmlobject_binding.txt";
    QTest::newRow("Synthesized properties") << "synthesized_properties.txt";
    QTest::newRow("Synthesized properties.2") << "synthesized_properties.2.txt";
    QTest::newRow("SameGame - BoomBlock") << "samegame/BoomBlock.qml";
}

void tst_qmlcomponent::creation()
{
    QFETCH(QString, file);

    QmlComponent c(&engine, file);
    QVERIFY(c.isReady());

    QBENCHMARK {
        QObject *obj = c.create();
        delete obj;
    }
}

QTEST_MAIN(tst_qmlcomponent)
#include "tst_qmlcomponent.moc"
