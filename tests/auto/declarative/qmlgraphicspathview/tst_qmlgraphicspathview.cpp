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
#include <private/qmlgraphicspathview_p.h>
#include <qmlcontext.h>
#include <qmlexpression.h>
#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlvaluetype_p.h>
#include "../../../shared/util.h"

class tst_QmlGraphicsPathView : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsPathView();

private slots:
    void initValues();
    void pathview2();
    void pathview3();
};


tst_QmlGraphicsPathView::tst_QmlGraphicsPathView()
{
}

void tst_QmlGraphicsPathView::initValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/pathview1.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() == 0);
    QVERIFY(obj->delegate() == 0);
    QCOMPARE(obj->model(), QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->snapPosition(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 0);
    QCOMPARE(obj->pathItemCount(), -1);
}

void tst_QmlGraphicsPathView::pathview2()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/pathview2.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 0.);
    QCOMPARE(obj->snapPosition(), 0.);
    QCOMPARE(obj->dragMargin(), 0.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 10);
}

void tst_QmlGraphicsPathView::pathview3()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/pathview3.qml"));
    QmlGraphicsPathView *obj = qobject_cast<QmlGraphicsPathView*>(c.create());

    QVERIFY(obj != 0);
    QVERIFY(obj->path() != 0);
    QVERIFY(obj->delegate() != 0);
    QVERIFY(obj->model() != QVariant());
    QCOMPARE(obj->currentIndex(), 0);
    QCOMPARE(obj->offset(), 50.); // ???
    QCOMPARE(obj->snapPosition(), 0.5); // ???
    QCOMPARE(obj->dragMargin(), 24.);
    QCOMPARE(obj->count(), 8);
    QCOMPARE(obj->pathItemCount(), 4);
}

QTEST_MAIN(tst_QmlGraphicsPathView)

#include "tst_qmlgraphicspathview.moc"
