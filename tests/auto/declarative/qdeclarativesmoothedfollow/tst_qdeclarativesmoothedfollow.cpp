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
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeview.h>

#include <private/qdeclarativesmoothedfollow_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include "../../../shared/util.h"

class tst_qdeclarativesmoothedfollow : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativesmoothedfollow();

private slots:
    void defaultValues();
    void values();
    void disabled();
    void valueSource();
    void followTo();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativesmoothedfollow::tst_qdeclarativesmoothedfollow()
{
}

void tst_qdeclarativesmoothedfollow::defaultValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/smoothedfollow1.qml"));
    QDeclarativeSmoothedFollow *obj = qobject_cast<QDeclarativeSmoothedFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->to(), 0.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->duration(), -1);
    QCOMPARE(obj->maximumEasingTime(), -1);
    QCOMPARE(obj->reversingMode(), QDeclarativeSmoothedFollow::Eased);

    delete obj;
}

void tst_qdeclarativesmoothedfollow::values()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/smoothedfollow2.qml"));
    QDeclarativeSmoothedFollow *obj = qobject_cast<QDeclarativeSmoothedFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->to(), 10.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->duration(), 300);
    QCOMPARE(obj->maximumEasingTime(), -1);
    QCOMPARE(obj->reversingMode(), QDeclarativeSmoothedFollow::Immediate);

    delete obj;
}

void tst_qdeclarativesmoothedfollow::disabled()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/smoothedfollow3.qml"));
    QDeclarativeSmoothedFollow *obj = qobject_cast<QDeclarativeSmoothedFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->to(), 10.);
    QCOMPARE(obj->velocity(), 250.);
    QCOMPARE(obj->maximumEasingTime(), 150);
    QCOMPARE(obj->reversingMode(), QDeclarativeSmoothedFollow::Sync);

    delete obj;
}

void tst_qdeclarativesmoothedfollow::valueSource()
{
    QDeclarativeEngine engine;

    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/smoothedfollowValueSource.qml"));

    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QVERIFY(rect);

    QDeclarativeRectangle *theRect = rect->findChild<QDeclarativeRectangle*>("theRect");
    QVERIFY(theRect);

    QDeclarativeSmoothedFollow *easeX = rect->findChild<QDeclarativeSmoothedFollow*>("easeX");
    QVERIFY(easeX);
    QVERIFY(easeX->enabled());

    QDeclarativeSmoothedFollow *easeY = rect->findChild<QDeclarativeSmoothedFollow*>("easeY");
    QVERIFY(easeY);
    QVERIFY(easeY->enabled());

    // XXX get the proper duration
    QTest::qWait(200);

    QTRY_COMPARE(theRect->x(), easeX->to());
    QTRY_COMPARE(theRect->y(), easeY->to());

    QTRY_COMPARE(theRect->x(), qreal(200));
    QTRY_COMPARE(theRect->y(), qreal(200));
}

void tst_qdeclarativesmoothedfollow::followTo()
{
    QDeclarativeView canvas;
    canvas.setFixedSize(240,320);

    canvas.setSource(QUrl::fromLocalFile(SRCDIR "/data/smoothedfollowDisabled.qml"));
    canvas.show();
    canvas.setFocus();
    QVERIFY(canvas.rootObject() != 0);

    QDeclarativeRectangle *rect = canvas.rootObject()->findChild<QDeclarativeRectangle*>("theRect");
    QVERIFY(rect != 0);

    QDeclarativeSmoothedFollow *animX = canvas.rootObject()->findChild<QDeclarativeSmoothedFollow*>("animX");
    QVERIFY(animX != 0);
    QDeclarativeSmoothedFollow *animY = canvas.rootObject()->findChild<QDeclarativeSmoothedFollow*>("animY");
    QVERIFY(animY != 0);

    QVERIFY(animX->enabled());
    QVERIFY(!animY->enabled());

    // animX should track 'to'
    animX->setTo(50.0);
    // animY should not track this 'to' change
    animY->setTo(50.0);

    // XXX get the proper duration
    QTest::qWait(250);

    QTRY_COMPARE(rect->x(), animX->to());

    QCOMPARE(rect->x(), 50.0);
    QCOMPARE(rect->y(), 100.0);
}

QTEST_MAIN(tst_qdeclarativesmoothedfollow)

#include "tst_qdeclarativesmoothedfollow.moc"
