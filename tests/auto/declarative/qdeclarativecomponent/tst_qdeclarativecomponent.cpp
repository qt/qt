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
#include <qtest.h>
#include <QDebug>

#include <QtGui/qgraphicsitem.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativecomponent : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativecomponent() { }

private slots:
    void loadEmptyUrl();
    void qmlCreateObject();

private:
    QDeclarativeEngine engine;
};

void tst_qdeclarativecomponent::loadEmptyUrl()
{
    QDeclarativeComponent c(&engine);
    c.loadUrl(QUrl());

    QVERIFY(c.isError());
    QCOMPARE(c.errors().count(), 1);
    QDeclarativeError error = c.errors().first();
    QCOMPARE(error.url(), QUrl());
    QCOMPARE(error.line(), -1);
    QCOMPARE(error.column(), -1);
    QCOMPARE(error.description(), QLatin1String("Invalid empty URL"));
}

void tst_qdeclarativecomponent::qmlCreateObject()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/createObject.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QObject *testObject1 = object->property("qobject").value<QObject*>();
    QVERIFY(testObject1);
    QVERIFY(testObject1->parent() == object);

    QObject *testObject2 = object->property("declarativeitem").value<QObject*>();
    QVERIFY(testObject2);
    QVERIFY(testObject2->parent() == object);
    QCOMPARE(testObject2->metaObject()->className(), "QDeclarativeItem");

    //Note that QGraphicsObjects are not exposed to QML for instantiation, and so can't be used in a component directly
    //Also this is actually the extended type QDeclarativeGraphicsWidget, but it still doesn't inherit QDeclarativeItem
    QGraphicsObject *testObject3 = qobject_cast<QGraphicsObject*>(object->property("graphicswidget").value<QObject*>());
    QVERIFY(testObject3);
    QVERIFY(testObject3->parent() == object);
    QVERIFY(testObject3->parentItem() == qobject_cast<QGraphicsObject*>(object));
    QCOMPARE(testObject3->metaObject()->className(), "QDeclarativeGraphicsWidget");
}

QTEST_MAIN(tst_qdeclarativecomponent)

#include "tst_qdeclarativecomponent.moc"
