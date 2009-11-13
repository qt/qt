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
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlanimation_p.h>
#include <QVariantAnimation>

class tst_animations : public QObject
{
    Q_OBJECT
public:
    tst_animations() {}

private slots:
    void simpleProperty();
    void simpleNumber();
    void simpleColor();
    void alwaysRunToEnd();
    void dotProperty();
    void badTypes();
    void badProperties();
    void mixedTypes();
    void properties();
    void propertiesTransition();
    void easingStringConversion();
};

#define QTIMED_COMPARE(lhs, rhs) do { \
    for (int ii = 0; ii < 5; ++ii) { \
        if (lhs == rhs)  \
            break; \
        QTest::qWait(50); \
    } \
    QCOMPARE(lhs, rhs); \
} while (false)

void tst_animations::simpleProperty()
{
    QmlGraphicsRectangle rect;
    QmlPropertyAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("pos");
    animation.setTo(QPointF(200,200));
    QVERIFY(animation.target() == &rect);
    QVERIFY(animation.property() == "pos");
    QVERIFY(animation.to().toPointF() == QPointF(200,200));
    animation.start();
    QVERIFY(animation.isRunning());
    QTest::qWait(animation.duration());
    QTIMED_COMPARE(rect.pos(), QPointF(200,200));

    rect.setPos(0,0);
    animation.start();
    animation.pause();
    QVERIFY(animation.isRunning());
    QVERIFY(animation.isPaused());
    animation.setCurrentTime(125);
    QCOMPARE(rect.pos(), QPointF(100,100));
}

void tst_animations::simpleNumber()
{
    QmlGraphicsRectangle rect;
    QmlNumberAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setTo(200);
    QVERIFY(animation.target() == &rect);
    QVERIFY(animation.property() == "x");
    QVERIFY(animation.to() == 200);
    animation.start();
    QVERIFY(animation.isRunning());
    QTest::qWait(animation.duration());
    QTIMED_COMPARE(rect.x(), qreal(200));

    rect.setX(0);
    animation.start();
    animation.pause();
    QVERIFY(animation.isRunning());
    QVERIFY(animation.isPaused());
    animation.setCurrentTime(125);
    QCOMPARE(rect.x(), qreal(100));
}

void tst_animations::simpleColor()
{
    QmlGraphicsRectangle rect;
    QmlColorAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("color");
    animation.setTo(QColor("red"));
    QVERIFY(animation.target() == &rect);
    QVERIFY(animation.property() == "color");
    QVERIFY(animation.to() == QColor("red"));
    animation.start();
    QVERIFY(animation.isRunning());
    QTest::qWait(animation.duration());
    QTIMED_COMPARE(rect.color(), QColor("red"));

    rect.setColor(QColor("blue"));
    animation.start();
    animation.pause();
    QVERIFY(animation.isRunning());
    QVERIFY(animation.isPaused());
    animation.setCurrentTime(125);
    QCOMPARE(rect.color(), QColor::fromRgbF(0.498039, 0, 0.498039, 1));
}

void tst_animations::alwaysRunToEnd()
{
    QmlGraphicsRectangle rect;
    QmlPropertyAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setTo(200);
    animation.setDuration(1000);
    animation.setRepeat(true);
    animation.setAlwaysRunToEnd(true);
    animation.start();
    QTest::qWait(1500);
    animation.stop();
    QVERIFY(rect.x() != qreal(200));
    QTest::qWait(500);
    QTIMED_COMPARE(rect.x(), qreal(200));
}

void tst_animations::dotProperty()
{
    QmlGraphicsRectangle rect;
    QmlNumberAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("border.width");
    animation.setTo(10);
    animation.start();
    QTest::qWait(animation.duration()+50);
    QTIMED_COMPARE(rect.border()->width(), 10);

    rect.border()->setWidth(0);
    animation.start();
    animation.pause();
    animation.setCurrentTime(125);
    QCOMPARE(rect.border()->width(), 5);
}

void tst_animations::badTypes()
{
    //don't crash
    {
        QmlView *view = new QmlView;
        view->setUrl(QUrl("file://" SRCDIR "/data/badtype1.qml"));

        view->execute();
        qApp->processEvents();

        delete view;
    }

    //make sure we get a compiler error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/badtype2.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QmlComponent: Component is not ready");
        c.create();

        QVERIFY(c.errors().count() == 1);
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Invalid property assignment: double expected"));
    }

    //make sure we get a compiler error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/badtype3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QmlComponent: Component is not ready");
        c.create();

        QVERIFY(c.errors().count() == 1);
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Invalid property assignment: color expected"));
    }

    //don't crash
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/badtype4.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(1000 + 50);
        QmlGraphicsRectangle *myRect = qobject_cast<QmlGraphicsRectangle*>(rect->QGraphicsObject::children().at(3));    //### not robust
        QVERIFY(myRect);
        QCOMPARE(myRect->x(),qreal(200));
    }
}

void tst_animations::badProperties()
{
    //make sure we get a runtime error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/badproperty1.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QML QmlColorAnimation (file://" SRCDIR "/data/badproperty1.qml:22:9) Cannot animate non-existant property \"pen.colr\"");
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        //### should we warn here are well?
        //rect->setState("state1");
    }
}

//test animating mixed types with property animation in a transition
//for example, int + real; color + real; etc
void tst_animations::mixedTypes()
{
    //assumes border.width stats a real -- not real robust
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/mixedtype1.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRectangle *myRect = qobject_cast<QmlGraphicsRectangle*>(rect->QGraphicsObject::children().at(3));    //### not robust
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->border()->width() > 1 && myRect->border()->width() < 10);
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/mixedtype2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRectangle *myRect = qobject_cast<QmlGraphicsRectangle*>(rect->QGraphicsObject::children().at(3));    //### not robust
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->color() != QColor("red") && myRect->color() != QColor("blue"));
    }
}

void tst_animations::properties()
{
    const int waitDuration = 300;
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/properties.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/properties2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/properties3.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(300));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/properties4.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->y(),qreal(200));
        QTIMED_COMPARE(myRect->x(),qreal(100));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/properties5.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(100));
        QTIMED_COMPARE(myRect->y(),qreal(100));
    }
}

void tst_animations::propertiesTransition()
{
    const int waitDuration = 300;
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/propertiesTransition.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/propertiesTransition2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(200));
        QCOMPARE(myRect->y(),qreal(100));
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->y(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/propertiesTransition3.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::ignoreMessage(QtWarningMsg, "QML QmlNumberAnimation (file://" SRCDIR "/data/propertiesTransition4.qml:22:9) matchTargets/matchProperties/exclude and target/property are mutually exclusive.");
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/propertiesTransition4.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::ignoreMessage(QtWarningMsg, "QML QmlNumberAnimation (file://" SRCDIR "/data/propertiesTransition5.qml:22:9) matchTargets/matchProperties/exclude and target/property are mutually exclusive.");
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/propertiesTransition5.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        //### should output warning at some point -- theItem doesn't exist
        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(200));
    }
}

void tst_animations::easingStringConversion()
{
    QmlNumberAnimation *animation = new QmlNumberAnimation;
    animation->setEasing("easeInOutQuad");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve(), QEasingCurve(QEasingCurve::InOutQuad));

    animation->setEasing("OutQuad");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve(), QEasingCurve(QEasingCurve::OutQuad));

    animation->setEasing("easeOutBounce(amplitude: 5)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::OutBounce);
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().amplitude(), qreal(5));

    delete animation;
}

QTEST_MAIN(tst_animations)

#include "tst_animations.moc"
