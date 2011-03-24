/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsgflickable_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include <math.h>
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qsgflickable : public QObject
{
    Q_OBJECT
public:
    tst_qsgflickable();

private slots:
    void create();
    void horizontalViewportSize();
    void verticalViewportSize();
    void properties();
    void boundsBehavior();
    void maximumFlickVelocity();
    void flickDeceleration();
    void pressDelay();
    void disabledContent();
    void nestedPressDelay();
    void flickableDirection();
    void resizeContent();
    void returnToBounds();
    void wheel();

private:
    QDeclarativeEngine engine;

    template<typename T>
    T *findItem(QSGItem *parent, const QString &objectName);
};

tst_qsgflickable::tst_qsgflickable()
{
}

void tst_qsgflickable::create()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable01.qml"));
    QSGFlickable *obj = qobject_cast<QSGFlickable*>(c.create());

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
    QCOMPARE(obj->boundsBehavior(), QSGFlickable::DragAndOvershootBounds);
    QCOMPARE(obj->pressDelay(), 0);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    delete obj;
}

void tst_qsgflickable::horizontalViewportSize()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable02.qml"));
    QSGFlickable *obj = qobject_cast<QSGFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentWidth(), 800.);
    QCOMPARE(obj->contentHeight(), 300.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qsgflickable::verticalViewportSize()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable03.qml"));
    QSGFlickable *obj = qobject_cast<QSGFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentWidth(), 200.);
    QCOMPARE(obj->contentHeight(), 1200.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qsgflickable::properties()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable04.qml"));
    QSGFlickable *obj = qobject_cast<QSGFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->isInteractive(), false);
    QCOMPARE(obj->boundsBehavior(), QSGFlickable::StopAtBounds);
    QCOMPARE(obj->pressDelay(), 200);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    QVERIFY(obj->property("ok").toBool() == false);
    QMetaObject::invokeMethod(obj, "check");
    QVERIFY(obj->property("ok").toBool() == true);

    delete obj;
}

void tst_qsgflickable::boundsBehavior()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Flickable { boundsBehavior: Flickable.StopAtBounds }", QUrl::fromLocalFile(""));
    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(boundsBehaviorChanged()));

    QVERIFY(flickable);
    QVERIFY(flickable->boundsBehavior() == QSGFlickable::StopAtBounds);

    flickable->setBoundsBehavior(QSGFlickable::DragAndOvershootBounds);
    QVERIFY(flickable->boundsBehavior() == QSGFlickable::DragAndOvershootBounds);
    QCOMPARE(spy.count(),1);
    flickable->setBoundsBehavior(QSGFlickable::DragAndOvershootBounds);
    QCOMPARE(spy.count(),1);

    flickable->setBoundsBehavior(QSGFlickable::DragOverBounds);
    QVERIFY(flickable->boundsBehavior() == QSGFlickable::DragOverBounds);
    QCOMPARE(spy.count(),2);
    flickable->setBoundsBehavior(QSGFlickable::DragOverBounds);
    QCOMPARE(spy.count(),2);

    flickable->setBoundsBehavior(QSGFlickable::StopAtBounds);
    QVERIFY(flickable->boundsBehavior() == QSGFlickable::StopAtBounds);
    QCOMPARE(spy.count(),3);
    flickable->setBoundsBehavior(QSGFlickable::StopAtBounds);
    QCOMPARE(spy.count(),3);
}

void tst_qsgflickable::maximumFlickVelocity()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Flickable { maximumFlickVelocity: 1.0; }", QUrl::fromLocalFile(""));
    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(maximumFlickVelocityChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->maximumFlickVelocity(), 1.0);

    flickable->setMaximumFlickVelocity(2.0);
    QCOMPARE(flickable->maximumFlickVelocity(), 2.0);
    QCOMPARE(spy.count(),1);
    flickable->setMaximumFlickVelocity(2.0);
    QCOMPARE(spy.count(),1);
}

void tst_qsgflickable::flickDeceleration()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Flickable { flickDeceleration: 1.0; }", QUrl::fromLocalFile(""));
    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(flickDecelerationChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->flickDeceleration(), 1.0);

    flickable->setFlickDeceleration(2.0);
    QCOMPARE(flickable->flickDeceleration(), 2.0);
    QCOMPARE(spy.count(),1);
    flickable->setFlickDeceleration(2.0);
    QCOMPARE(spy.count(),1);
}

void tst_qsgflickable::pressDelay()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Flickable { pressDelay: 100; }", QUrl::fromLocalFile(""));
    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(pressDelayChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->pressDelay(), 100);

    flickable->setPressDelay(200);
    QCOMPARE(flickable->pressDelay(), 200);
    QCOMPARE(spy.count(),1);
    flickable->setPressDelay(200);
    QCOMPARE(spy.count(),1);
}

// QT-4677
void tst_qsgflickable::disabledContent()
{
    QSGView *canvas = new QSGView;
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/disabledcontent.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(canvas->rootObject());
    QVERIFY(flickable != 0);

    QVERIFY(flickable->contentX() == 0);
    QVERIFY(flickable->contentY() == 0);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50, 50));
    {
        QMouseEvent mv(QEvent::MouseMove, QPoint(70,70), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(canvas, &mv);
    }
    {
        QMouseEvent mv(QEvent::MouseMove, QPoint(90,90), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(canvas, &mv);
    }
    {
        QMouseEvent mv(QEvent::MouseMove, QPoint(100,100), Qt::LeftButton, Qt::LeftButton,Qt::NoModifier);
        QApplication::sendEvent(canvas, &mv);
    }

    QVERIFY(flickable->contentX() < 0);
    QVERIFY(flickable->contentY() < 0);

    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(90, 90));

    delete canvas;
}


// QTBUG-17361
void tst_qsgflickable::nestedPressDelay()
{
    QSGView *canvas = new QSGView;
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/nestedPressDelay.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGFlickable *outer = qobject_cast<QSGFlickable*>(canvas->rootObject());
    QVERIFY(outer != 0);

    QSGFlickable *inner = canvas->rootObject()->findChild<QSGFlickable*>("innerFlickable");
    QVERIFY(inner != 0);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(150, 150));
    // the MouseArea is not pressed immediately
    QVERIFY(outer->property("pressed").toBool() == false);

    // The outer pressDelay will prevail (50ms, vs. 10sec)
    // QTRY_VERIFY() has 5sec timeout, so will timeout well within 10sec.
    QTRY_VERIFY(outer->property("pressed").toBool() == true);

    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(150, 150));

    delete canvas;
}

void tst_qsgflickable::flickableDirection()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 2.0; Flickable { flickableDirection: Flickable.VerticalFlick; }", QUrl::fromLocalFile(""));
    QSGFlickable *flickable = qobject_cast<QSGFlickable*>(component.create());
    QSignalSpy spy(flickable, SIGNAL(flickableDirectionChanged()));

    QVERIFY(flickable);
    QCOMPARE(flickable->flickableDirection(), QSGFlickable::VerticalFlick);

    flickable->setFlickableDirection(QSGFlickable::HorizontalAndVerticalFlick);
    QCOMPARE(flickable->flickableDirection(), QSGFlickable::HorizontalAndVerticalFlick);
    QCOMPARE(spy.count(),1);

    flickable->setFlickableDirection(QSGFlickable::AutoFlickDirection);
    QCOMPARE(flickable->flickableDirection(), QSGFlickable::AutoFlickDirection);
    QCOMPARE(spy.count(),2);

    flickable->setFlickableDirection(QSGFlickable::HorizontalFlick);
    QCOMPARE(flickable->flickableDirection(), QSGFlickable::HorizontalFlick);
    QCOMPARE(spy.count(),3);

    flickable->setFlickableDirection(QSGFlickable::HorizontalFlick);
    QCOMPARE(flickable->flickableDirection(), QSGFlickable::HorizontalFlick);
    QCOMPARE(spy.count(),3);
}

// QtQuick 1.1
void tst_qsgflickable::resizeContent()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/resize.qml"));
    QSGItem *root = qobject_cast<QSGItem*>(c.create());
    QSGFlickable *obj = findItem<QSGFlickable>(root, "flick");

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentX(), 0.);
    QCOMPARE(obj->contentY(), 0.);
    QCOMPARE(obj->contentWidth(), 300.);
    QCOMPARE(obj->contentHeight(), 300.);

    QMetaObject::invokeMethod(root, "resizeContent");

    QCOMPARE(obj->contentX(), 100.);
    QCOMPARE(obj->contentY(), 100.);
    QCOMPARE(obj->contentWidth(), 600.);
    QCOMPARE(obj->contentHeight(), 600.);

    delete root;
}

// QtQuick 1.1
void tst_qsgflickable::returnToBounds()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/resize.qml"));
    QSGItem *root = qobject_cast<QSGItem*>(c.create());
    QSGFlickable *obj = findItem<QSGFlickable>(root, "flick");

    QVERIFY(obj != 0);
    QCOMPARE(obj->contentX(), 0.);
    QCOMPARE(obj->contentY(), 0.);
    QCOMPARE(obj->contentWidth(), 300.);
    QCOMPARE(obj->contentHeight(), 300.);

    obj->setContentX(100);
    obj->setContentY(400);
    QTRY_COMPARE(obj->contentX(), 100.);
    QTRY_COMPARE(obj->contentY(), 400.);

    QMetaObject::invokeMethod(root, "returnToBounds");

    QTRY_COMPARE(obj->contentX(), 0.);
    QTRY_COMPARE(obj->contentY(), 0.);

    delete root;
}

void tst_qsgflickable::wheel()
{
    QSGView *canvas = new QSGView;
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/wheel.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGFlickable *flick = canvas->rootObject()->findChild<QSGFlickable*>("flick");
    QVERIFY(flick != 0);

    {
        QWheelEvent event(QPoint(200, 200), -120, Qt::NoButton, Qt::NoModifier, Qt::Vertical);
        event.setAccepted(false);
        QApplication::sendEvent(canvas, &event);
    }

    QTRY_VERIFY(flick->contentY() > 0);
    QVERIFY(flick->contentX() == 0);

    flick->setContentY(0);
    QVERIFY(flick->contentY() == 0);

    {
        QWheelEvent event(QPoint(200, 200), -120, Qt::NoButton, Qt::NoModifier, Qt::Horizontal);
        event.setAccepted(false);
        QApplication::sendEvent(canvas, &event);
    }

    QTRY_VERIFY(flick->contentX() > 0);
    QVERIFY(flick->contentY() == 0);

    delete canvas;
}


template<typename T>
T *tst_qsgflickable::findItem(QSGItem *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    //qDebug() << parent->childItems().count() << "children";
    for (int i = 0; i < parent->childItems().count(); ++i) {
        QSGItem *item = qobject_cast<QSGItem*>(parent->childItems().at(i));
        if(!item)
            continue;
        //qDebug() << "try" << item;
        if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
            return static_cast<T*>(item);
        }
        item = findItem<T>(item, objectName);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

QTEST_MAIN(tst_qsgflickable)

#include "tst_qsgflickable.moc"
