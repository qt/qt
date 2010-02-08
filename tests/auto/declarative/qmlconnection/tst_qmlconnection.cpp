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
#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlconnection_p.h>
#include <private/qmlgraphicsitem_p.h>
#include "../../../shared/util.h"
#include <QtDeclarative/qmlscriptstring.h>

class tst_qmlconnection : public QObject

{
    Q_OBJECT
public:
    tst_qmlconnection();

private slots:
    void defaultValues();
    void properties();
    void connection();
    void trimming();

private:
    QmlEngine engine;
};

tst_qmlconnection::tst_qmlconnection()
{
}

void tst_qmlconnection::defaultValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection3.qml"));
    QmlConnection *item = qobject_cast<QmlConnection*>(c.create());

    QVERIFY(item != 0);
    QVERIFY(item->signalSender() == 0);
    QCOMPARE(item->script().script(), QString());
    QCOMPARE(item->signal(), QString());

    delete item;
}

void tst_qmlconnection::properties()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection2.qml"));
    QmlConnection *item = qobject_cast<QmlConnection*>(c.create());

    QVERIFY(item != 0);

    QVERIFY(item != 0);
    QVERIFY(item->signalSender() == item);
    QCOMPARE(item->script().script(), QString("1 == 1"));
    QCOMPARE(item->signal(), QString("widthChanged()"));

    delete item;
}

void tst_qmlconnection::connection()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-connection.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(c.create());

    QVERIFY(item != 0);

    QCOMPARE(item->property("tested").toBool(), false);
    QCOMPARE(item->width(), 50.);
    emit item->setWidth(100.);
    QCOMPARE(item->width(), 100.);
    QCOMPARE(item->property("tested").toBool(), true);

    delete item;
}

void tst_qmlconnection::trimming()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/trimming.qml"));
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem*>(c.create());

    QVERIFY(item != 0);

    QCOMPARE(item->property("tested").toString(), QString(""));
    int index = item->metaObject()->indexOfSignal("testMe(int,QString)");
    QMetaMethod method = item->metaObject()->method(index);
    method.invoke(item,
                  Qt::DirectConnection,
                  Q_ARG(int, 5),
                  Q_ARG(QString, "worked"));
    QCOMPARE(item->property("tested").toString(), QString("worked5"));

    delete item;
}

QTEST_MAIN(tst_qmlconnection)

#include "tst_qmlconnection.moc"
