/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativepropertymap.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativetext_p.h>
#include <QSignalSpy>

class tst_QDeclarativePropertyMap : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativePropertyMap() {}

private slots:
    void insert();
    void operatorInsert();
    void operatorValue();
    void clear();
    void changed();
    void count();

    void crashBug();
};

void tst_QDeclarativePropertyMap::insert()
{
    QDeclarativePropertyMap map;
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QVERIFY(map.keys().count() == 2);
    QVERIFY(map.contains(QLatin1String("key1")));

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));
    QCOMPARE(map.value(QLatin1String("key2")), QVariant(200));

    map.insert(QLatin1String("key1"),"Hello World");
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));

    //inserting property names same with existing method(signal, slot, method) names is not allowed
    //QDeclarativePropertyMap has an invokable keys() method
    QTest::ignoreMessage(QtWarningMsg, "Creating property with name \"keys\" is not permitted, conflicts with internal symbols. ");
    map.insert(QLatin1String("keys"), 1);
    QVERIFY(map.keys().count() == 2);
    QVERIFY(!map.contains(QLatin1String("keys")));
    QVERIFY(map.value(QLatin1String("keys")).isNull());

    //QDeclarativePropertyMap has a deleteLater() slot
    QTest::ignoreMessage(QtWarningMsg, "Creating property with name \"deleteLater\" is not permitted, conflicts with internal symbols. ");
    map.insert(QLatin1String("deleteLater"), 1);
    QVERIFY(map.keys().count() == 2);
    QVERIFY(!map.contains(QLatin1String("deleteLater")));
    QVERIFY(map.value(QLatin1String("deleteLater")).isNull());

    //QDeclarativePropertyMap has an valueChanged() signal
    QTest::ignoreMessage(QtWarningMsg, "Creating property with name \"valueChanged\" is not permitted, conflicts with internal symbols. ");
    map.insert(QLatin1String("valueChanged"), 1);
    QVERIFY(map.keys().count() == 2);
    QVERIFY(!map.contains(QLatin1String("valueChanged")));
    QVERIFY(map.value(QLatin1String("valueChanged")).isNull());

    //but 'valueChange' should be ok
    map.insert(QLatin1String("valueChange"), 1);
    QVERIFY(map.keys().count() == 3);
    QVERIFY(map.contains(QLatin1String("valueChange")));
    QCOMPARE(map.value(QLatin1String("valueChange")), QVariant(1));

    //'valueCHANGED' should be ok, too
    map.insert(QLatin1String("valueCHANGED"), 1);
    QVERIFY(map.keys().count() == 4);
    QVERIFY(map.contains(QLatin1String("valueCHANGED")));
    QCOMPARE(map.value(QLatin1String("valueCHANGED")), QVariant(1));
}

void tst_QDeclarativePropertyMap::operatorInsert()
{
    QDeclarativePropertyMap map;
    map[QLatin1String("key1")] = 100;
    map[QLatin1String("key2")] = 200;
    QVERIFY(map.keys().count() == 2);

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));
    QCOMPARE(map.value(QLatin1String("key2")), QVariant(200));

    map[QLatin1String("key1")] = "Hello World";
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));
}

void tst_QDeclarativePropertyMap::operatorValue()
{
    QDeclarativePropertyMap map;
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QVERIFY(map.count() == 2);
    QVERIFY(map.contains(QLatin1String("key1")));

    const QDeclarativePropertyMap &constMap = map;

    QCOMPARE(constMap.value(QLatin1String("key1")), QVariant(100));
    QCOMPARE(constMap.value(QLatin1String("key2")), QVariant(200));
    QCOMPARE(constMap[QLatin1String("key1")], constMap.value(QLatin1String("key1")));
    QCOMPARE(constMap[QLatin1String("key2")], constMap.value(QLatin1String("key2")));
}

void tst_QDeclarativePropertyMap::clear()
{
    QDeclarativePropertyMap map;
    map.insert(QLatin1String("key1"),100);
    QVERIFY(map.keys().count() == 1);

    QCOMPARE(map.value(QLatin1String("key1")), QVariant(100));

    map.clear(QLatin1String("key1"));
    QVERIFY(map.keys().count() == 1);
    QVERIFY(map.contains(QLatin1String("key1")));
    QCOMPARE(map.value(QLatin1String("key1")), QVariant());
}

void tst_QDeclarativePropertyMap::changed()
{
    QDeclarativePropertyMap map;
    QSignalSpy spy(&map, SIGNAL(valueChanged(const QString&, const QVariant&)));
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QCOMPARE(spy.count(), 0);

    map.clear(QLatin1String("key1"));
    QCOMPARE(spy.count(), 0);

    //make changes in QML
    QDeclarativeEngine engine;
    QDeclarativeContext *ctxt = engine.rootContext();
    ctxt->setContextProperty(QLatin1String("testdata"), &map);
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 1.0\nText { text: { testdata.key1 = 'Hello World'; 'X' } }",
            QUrl::fromLocalFile(""));
    QVERIFY(component.isReady());
    QDeclarativeText *txt = qobject_cast<QDeclarativeText*>(component.create());
    QVERIFY(txt);
    QCOMPARE(txt->text(), QString('X'));
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.count(), 2);
    QCOMPARE(arguments.at(0).toString(),QLatin1String("key1"));
    QCOMPARE(arguments.at(1).value<QVariant>(),QVariant("Hello World"));
    QCOMPARE(map.value(QLatin1String("key1")), QVariant("Hello World"));
}

void tst_QDeclarativePropertyMap::count()
{
    QDeclarativePropertyMap map;
    QCOMPARE(map.isEmpty(), true);
    map.insert(QLatin1String("key1"),100);
    map.insert(QLatin1String("key2"),200);
    QCOMPARE(map.count(), 2);
    QCOMPARE(map.isEmpty(), false);

    map.insert(QLatin1String("key3"),"Hello World");
    QCOMPARE(map.count(), 3);

    //clearing doesn't remove the key
    map.clear(QLatin1String("key3"));
    QCOMPARE(map.count(), 3);
    QCOMPARE(map.size(), map.count());
}

void tst_QDeclarativePropertyMap::crashBug()
{
    QDeclarativePropertyMap map;

    QDeclarativeEngine engine;
    QDeclarativeContext context(&engine);
    context.setContextProperty("map", &map);

    QDeclarativeComponent c(&engine);
    c.setData("import QtQuick 1.0\nBinding { target: map; property: \"myProp\"; value: 10 + 23 }",QUrl());
    QObject *obj = c.create(&context);
    delete obj;
}

QTEST_MAIN(tst_QDeclarativePropertyMap)

#include "tst_qdeclarativepropertymap.moc"
