/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativeflickable_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include <math.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativeflickable : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeflickable();

private slots:
    void create();
    void horizontalViewportSize();
    void verticalViewportSize();
    void properties();
    void boundsBehavior();
    void maximumFlickVelocity();
    void flickDeceleration();
    void pressDelay();
    void flickableDirection();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativeflickable::tst_qdeclarativeflickable()
{
}

void tst_qdeclarativeflickable::create()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable01.qml"));
    QDeclarativeFlickable *obj = qobject_cast<QDeclarativeFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);
    QCOMPARE(obj->contentX(), 0.);
    QCOMPARE(obj->contentY(), 0.);

    QCOMPARE(obj->horizontalVelocity(), 0.);
    QCOMPARE(obj->verticalVelocity(), 0.);

    QCOMPARE(obj->isInteractive(), true);
    QCOMPARE(obj->boundsBehavior(), QDeclarativeFlickable::DragAndOvershootBounds);
    QCOMPARE(obj->pressDelay(), 0);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    delete obj;
}

void tst_qdeclarativeflickable::horizontalViewportSize()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable02.qml"));
    QDeclarativeFlickable *obj = qobject_cast<QDeclarativeFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentWidth(), 800.);
    QCOMPARE(obj->contentHeight(), 300.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qdeclarativeflickable::verticalViewportSize()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable03.qml"));
    QDeclarativeFlickable *obj = qobject_cast<QDeclarativeFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentWidth(), 200.);
    QCOMPARE(obj->contentHeight(), 1200.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qdeclarativeflickable::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable04.qml"));
    QDeclarativeFlickable *obj = qobject_cast<QDeclarativeFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->isInteractive(), false);
    QCOMPARE(obj->boundsBehavior(), QDeclarativeFlickable::StopAtBounds);
    QCOMPARE(obj->pressDelay(), 200);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    QVERIFY(obj->property("ok").toBool() == false);
    QMetaObject::invokeMethod(obj, "check");
    QVERIFY(obj->property("ok").toBool() == true);

    delete obj;
}

void tst_qdeclarativeflickable::boundsBehavior()
{
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.7; Flickable { boundsBehavior: Flickable.StopAtBounds }", QUrl::fromLocalFile(""));
    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(boundsBehaviorChanged()));

    QVERIFY(flickable);
    QVERIFY(flickable->boundsBehavior() == QDeclarativeFlickable::StopAtBounds);

    flickable->setBoundsBehavior(QDeclarativeFlickable::DragAndOvershootBounds);
    QVERIFY(flickable->boundsBehavior() == QDeclarativeFlickable::DragAndOvershootBounds);
    QCOMPARE(spy.count(),1);
    flickable->setBoundsBehavior(QDeclarativeFlickable::DragAndOvershootBounds);
    QCOMPARE(spy.count(),1);

    flickable->setBoundsBehavior(QDeclarativeFlickable::DragOverBounds);
    QVERIFY(flickable->boundsBehavior() == QDeclarativeFlickable::DragOverBounds);
    QCOMPARE(spy.count(),2);
    flickable->setBoundsBehavior(QDeclarativeFlickable::DragOverBounds);
    QCOMPARE(spy.count(),2);

    flickable->setBoundsBehavior(QDeclarativeFlickable::StopAtBounds);
    QVERIFY(flickable->boundsBehavior() == QDeclarativeFlickable::StopAtBounds);
    QCOMPARE(spy.count(),3);
    flickable->setBoundsBehavior(QDeclarativeFlickable::StopAtBounds);
    QCOMPARE(spy.count(),3);
}

void tst_qdeclarativeflickable::maximumFlickVelocity()
{
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.7; Flickable { maximumFlickVelocity: 1.0; }", QUrl::fromLocalFile(""));
    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(maximumFlickVelocityChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->maximumFlickVelocity(), 1.0);

    flickable->setMaximumFlickVelocity(2.0);
    QCOMPARE(flickable->maximumFlickVelocity(), 2.0);
    QCOMPARE(spy.count(),1);
    flickable->setMaximumFlickVelocity(2.0);
    QCOMPARE(spy.count(),1);
}

void tst_qdeclarativeflickable::flickDeceleration()
{
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.7; Flickable { flickDeceleration: 1.0; }", QUrl::fromLocalFile(""));
    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(flickDecelerationChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->flickDeceleration(), 1.0);

    flickable->setFlickDeceleration(2.0);
    QCOMPARE(flickable->flickDeceleration(), 2.0);
    QCOMPARE(spy.count(),1);
    flickable->setFlickDeceleration(2.0);
    QCOMPARE(spy.count(),1);
}

void tst_qdeclarativeflickable::pressDelay()
{
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.7; Flickable { pressDelay: 100; }", QUrl::fromLocalFile(""));
    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(pressDelayChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->pressDelay(), 100);

    flickable->setPressDelay(200);
    QCOMPARE(flickable->pressDelay(), 200);
    QCOMPARE(spy.count(),1);
    flickable->setPressDelay(200);
    QCOMPARE(spy.count(),1);
}

void tst_qdeclarativeflickable::flickableDirection()
{
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.7; Flickable { flickableDirection: Flickable.VerticalFlick; }", QUrl::fromLocalFile(""));
    QDeclarativeFlickable *flickable = qobject_cast<QDeclarativeFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(flickableDirectionChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->flickableDirection(), QDeclarativeFlickable::VerticalFlick);

    flickable->setFlickableDirection(QDeclarativeFlickable::HorizontalAndVerticalFlick);
    QCOMPARE(flickable->flickableDirection(), QDeclarativeFlickable::HorizontalAndVerticalFlick);
    QCOMPARE(spy.count(),1);

    flickable->setFlickableDirection(QDeclarativeFlickable::AutoFlickDirection);
    QCOMPARE(flickable->flickableDirection(), QDeclarativeFlickable::AutoFlickDirection);
    QCOMPARE(spy.count(),2);

    flickable->setFlickableDirection(QDeclarativeFlickable::HorizontalFlick);
    QCOMPARE(flickable->flickableDirection(), QDeclarativeFlickable::HorizontalFlick);
    QCOMPARE(spy.count(),3);

    flickable->setFlickableDirection(QDeclarativeFlickable::HorizontalFlick);
    QCOMPARE(flickable->flickableDirection(), QDeclarativeFlickable::HorizontalFlick);
    QCOMPARE(spy.count(),3);
}

QTEST_MAIN(tst_qdeclarativeflickable)

#include "tst_qdeclarativeflickable.moc"
