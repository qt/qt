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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <private/qsgpincharea_p.h>
#include <private/qsgrectangle_p.h>
#include <QtDeclarative/qsgview.h>
#include <QtDeclarative/qdeclarativecontext.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QSGPinchArea: public QObject
{
    Q_OBJECT
private slots:
    void pinchProperties();
    void scale();
    void pan();

private:
    QSGView *createView();
};

void tst_QSGPinchArea::pinchProperties()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pinchproperties.qml"));
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QSGPinchArea *pinchArea = canvas->rootObject()->findChild<QSGPinchArea*>("pincharea");
    QSGPinch *pinch = pinchArea->pinch();
    QVERIFY(pinchArea != 0);
    QVERIFY(pinch != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == pinch->target());
    QSGItem *rootItem = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(pinch, SIGNAL(targetChanged()));
    pinch->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);
    pinch->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);

    // axis
    QCOMPARE(pinch->axis(), QSGPinch::XandYAxis);
    QSignalSpy axisSpy(pinch, SIGNAL(dragAxisChanged()));
    pinch->setAxis(QSGPinch::XAxis);
    QCOMPARE(pinch->axis(), QSGPinch::XAxis);
    QCOMPARE(axisSpy.count(),1);
    pinch->setAxis(QSGPinch::XAxis);
    QCOMPARE(axisSpy.count(),1);

    // minimum and maximum drag properties
    QSignalSpy xminSpy(pinch, SIGNAL(minimumXChanged()));
    QSignalSpy xmaxSpy(pinch, SIGNAL(maximumXChanged()));
    QSignalSpy yminSpy(pinch, SIGNAL(minimumYChanged()));
    QSignalSpy ymaxSpy(pinch, SIGNAL(maximumYChanged()));

    QCOMPARE(pinch->xmin(), 0.0);
    QCOMPARE(pinch->xmax(), rootItem->width()-blackRect->width());
    QCOMPARE(pinch->ymin(), 0.0);
    QCOMPARE(pinch->ymax(), rootItem->height()-blackRect->height());

    pinch->setXmin(10);
    pinch->setXmax(10);
    pinch->setYmin(10);
    pinch->setYmax(10);

    QCOMPARE(pinch->xmin(), 10.0);
    QCOMPARE(pinch->xmax(), 10.0);
    QCOMPARE(pinch->ymin(), 10.0);
    QCOMPARE(pinch->ymax(), 10.0);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);

    pinch->setXmin(10);
    pinch->setXmax(10);
    pinch->setYmin(10);
    pinch->setYmax(10);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);

    // minimum and maximum scale properties
    QSignalSpy scaleMinSpy(pinch, SIGNAL(minimumScaleChanged()));
    QSignalSpy scaleMaxSpy(pinch, SIGNAL(maximumScaleChanged()));

    QCOMPARE(pinch->minimumScale(), 1.0);
    QCOMPARE(pinch->maximumScale(), 2.0);

    pinch->setMinimumScale(0.5);
    pinch->setMaximumScale(1.5);

    QCOMPARE(pinch->minimumScale(), 0.5);
    QCOMPARE(pinch->maximumScale(), 1.5);

    QCOMPARE(scaleMinSpy.count(),1);
    QCOMPARE(scaleMaxSpy.count(),1);

    pinch->setMinimumScale(0.5);
    pinch->setMaximumScale(1.5);

    QCOMPARE(scaleMinSpy.count(),1);
    QCOMPARE(scaleMaxSpy.count(),1);

    // minimum and maximum rotation properties
    QSignalSpy rotMinSpy(pinch, SIGNAL(minimumRotationChanged()));
    QSignalSpy rotMaxSpy(pinch, SIGNAL(maximumRotationChanged()));

    QCOMPARE(pinch->minimumRotation(), 0.0);
    QCOMPARE(pinch->maximumRotation(), 90.0);

    pinch->setMinimumRotation(-90.0);
    pinch->setMaximumRotation(45.0);

    QCOMPARE(pinch->minimumRotation(), -90.0);
    QCOMPARE(pinch->maximumRotation(), 45.0);

    QCOMPARE(rotMinSpy.count(),1);
    QCOMPARE(rotMaxSpy.count(),1);

    pinch->setMinimumRotation(-90.0);
    pinch->setMaximumRotation(45.0);

    QCOMPARE(rotMinSpy.count(),1);
    QCOMPARE(rotMaxSpy.count(),1);

    delete canvas;
}

QTouchEvent::TouchPoint makeTouchPoint(int id, QPoint p, QSGView *v, QSGItem *i)
{
    QTouchEvent::TouchPoint touchPoint(id);
    touchPoint.setPos(i->mapFromScene(p));
    touchPoint.setScreenPos(v->mapToGlobal(p));
    touchPoint.setScenePos(p);
    return touchPoint;
}

void tst_QSGPinchArea::scale()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pinchproperties.qml"));
    canvas->show();
    canvas->setFocus();
    QTest::qWaitForWindowShown(canvas);
    QVERIFY(canvas->rootObject() != 0);
    qApp->processEvents();

    QSGPinchArea *pinchArea = canvas->rootObject()->findChild<QSGPinchArea*>("pincharea");
    QSGPinch *pinch = pinchArea->pinch();
    QVERIFY(pinchArea != 0);
    QVERIFY(pinch != 0);

    QSGItem *root = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(root != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);

    QPoint p1(80, 80);
    QPoint p2(100, 100);

    QTest::touchEvent(canvas).press(0, p1);
    QTest::touchEvent(canvas).stationary(0).press(1, p2);
    p1 -= QPoint(10,10);
    p2 += QPoint(10,10);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(root->property("scale").toReal(), 1.0);

    p1 -= QPoint(10,10);
    p2 += QPoint(10,10);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(root->property("scale").toReal(), 1.5);
    QCOMPARE(root->property("center").toPointF(), QPointF(40, 40)); // blackrect is at 50,50
    QCOMPARE(blackRect->scale(), 1.5);

    // scale beyond bound
    p1 -= QPoint(50,50);
    p2 += QPoint(50,50);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(blackRect->scale(), 2.0);

    QTest::touchEvent(canvas).release(0, p1).release(1, p2);

    delete canvas;
}

void tst_QSGPinchArea::pan()
{
    QSGView *canvas = createView();
    canvas->setSource(QUrl::fromLocalFile(SRCDIR "/data/pinchproperties.qml"));
    canvas->show();
    canvas->setFocus();
    QTest::qWaitForWindowShown(canvas);
    QVERIFY(canvas->rootObject() != 0);
    qApp->processEvents();

    QSGPinchArea *pinchArea = canvas->rootObject()->findChild<QSGPinchArea*>("pincharea");
    QSGPinch *pinch = pinchArea->pinch();
    QVERIFY(pinchArea != 0);
    QVERIFY(pinch != 0);

    QSGItem *root = qobject_cast<QSGItem*>(canvas->rootObject());
    QVERIFY(root != 0);

    // target
    QSGItem *blackRect = canvas->rootObject()->findChild<QSGItem*>("blackrect");
    QVERIFY(blackRect != 0);

    QPoint p1(80, 80);
    QPoint p2(100, 100);

    QTest::touchEvent(canvas).press(0, p1);
    QTest::touchEvent(canvas).stationary(0).press(1, p2);
    p1 += QPoint(10,10);
    p2 += QPoint(10,10);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(root->property("scale").toReal(), 1.0);

    p1 += QPoint(10,10);
    p2 += QPoint(10,10);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(root->property("center").toPointF(), QPointF(60, 60)); // blackrect is at 50,50

    QCOMPARE(blackRect->x(), 60.0);
    QCOMPARE(blackRect->y(), 60.0);

    // pan x beyond bound
    p1 += QPoint(100,100);
    p2 += QPoint(100,100);
    QTest::touchEvent(canvas).move(0, p1).move(1, p2);

    QCOMPARE(blackRect->x(), 140.0);
    QCOMPARE(blackRect->y(), 160.0);

    QTest::touchEvent(canvas).release(0, p1).release(1, p2);

    delete canvas;
}

QSGView *tst_QSGPinchArea::createView()
{
    QSGView *canvas = new QSGView(0);
    canvas->setAttribute(Qt::WA_AcceptTouchEvents);
    canvas->setFixedSize(240,320);

    return canvas;
}

QTEST_MAIN(tst_QSGPinchArea)

#include "tst_qsgpincharea.moc"
