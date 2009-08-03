/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QmlMetaProperty>
#include <QFile>
#include <QDebug>

//TESTED_FILES=


class tst_qmlmetaproperty : public QObject
{
    Q_OBJECT

public:
    tst_qmlmetaproperty();
    virtual ~tst_qmlmetaproperty();

public slots:
    void init();
    void cleanup();

private slots:
    void lookup_data();
    void lookup();

private:
    QmlEngine engine;
};

tst_qmlmetaproperty::tst_qmlmetaproperty()
{
}

tst_qmlmetaproperty::~tst_qmlmetaproperty()
{
}

void tst_qmlmetaproperty::init()
{
}

void tst_qmlmetaproperty::cleanup()
{
}

void tst_qmlmetaproperty::lookup_data()
{
    QTest::addColumn<QString>("file");

    QTest::newRow("Simple Object") << "object.txt";
    QTest::newRow("Synthesized Object") << "synthesized_object.txt";
}

void tst_qmlmetaproperty::lookup()
{
    QFETCH(QString, file);

    QmlComponent c(&engine, file);
    QVERIFY(c.isReady());

    QObject *obj = c.create();

    QBENCHMARK {
        QmlMetaProperty p(obj, "x");
    }

    delete obj;
}

QTEST_MAIN(tst_qmlmetaproperty)
#include "tst_qmlmetaproperty.moc"
