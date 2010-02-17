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

class tst_qmlanimations : public QObject
{
    Q_OBJECT
public:
    tst_qmlanimations() {}

private slots:
    void initTestCase() { QmlEngine engine; } // ensure types are registered

    void simpleProperty();
    void simpleNumber();
    void simpleColor();
    void alwaysRunToEnd();
    void complete();
    void resume();
    void dotProperty();
    void badTypes();
    void badProperties();
    void mixedTypes();
    void properties();
    void propertiesTransition();
    void easingStringConversion();
    void invalidDuration();
    void attached();
    void propertyValueSourceDefaultStart();
};

#define QTIMED_COMPARE(lhs, rhs) do { \
    for (int ii = 0; ii < 5; ++ii) { \
        if (lhs == rhs)  \
            break; \
        QTest::qWait(50); \
    } \
    QCOMPARE(lhs, rhs); \
} while (false)

void tst_qmlanimations::simpleProperty()
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
    QVERIFY(animation.currentTime() == 125);
    QCOMPARE(rect.pos(), QPointF(100,100));
}

void tst_qmlanimations::simpleNumber()
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
    QVERIFY(animation.currentTime() == 125);
    QCOMPARE(rect.x(), qreal(100));
}

void tst_qmlanimations::simpleColor()
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
    QVERIFY(animation.currentTime() == 125);
    QCOMPARE(rect.color(), QColor::fromRgbF(0.498039, 0, 0.498039, 1));

    rect.setColor(QColor("green"));
    animation.setFrom(QColor("blue"));
    QVERIFY(animation.from() == QColor("blue"));
    animation.restart();
    QCOMPARE(rect.color(), QColor("blue"));
    QVERIFY(animation.isRunning());
    animation.setCurrentTime(125);
    QCOMPARE(rect.color(), QColor::fromRgbF(0.498039, 0, 0.498039, 1));
}

void tst_qmlanimations::alwaysRunToEnd()
{
    QmlGraphicsRectangle rect;
    QmlPropertyAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setTo(200);
    animation.setDuration(1000);
    animation.setRepeat(true);
    animation.setAlwaysRunToEnd(true);
    QVERIFY(animation.repeat() == true);
    QVERIFY(animation.alwaysRunToEnd() == true);
    animation.start();
    QTest::qWait(1500);
    animation.stop();
    QVERIFY(rect.x() != qreal(200));
    QTest::qWait(500);
    QTIMED_COMPARE(rect.x(), qreal(200));
}

void tst_qmlanimations::complete()
{
    QmlGraphicsRectangle rect;
    QmlPropertyAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setFrom(1);
    animation.setTo(200);
    animation.setDuration(500);
    QVERIFY(animation.from() == 1);
    animation.start();
    QTest::qWait(50);
    animation.stop();
    QVERIFY(rect.x() != qreal(200));
    animation.start();
    QTest::qWait(50);
    QVERIFY(animation.isRunning());
    animation.complete();
    QCOMPARE(rect.x(), qreal(200));
}

void tst_qmlanimations::resume()
{
    QmlGraphicsRectangle rect;
    QmlPropertyAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setFrom(10);
    animation.setTo(200);
    animation.setDuration(500);
    QVERIFY(animation.from() == 10);

    animation.start();
    QTest::qWait(100);
    animation.pause();
    qreal x = rect.x();
    QVERIFY(x != qreal(200));
    QVERIFY(animation.isRunning());
    QVERIFY(animation.isPaused());

    animation.resume();
    QVERIFY(animation.isRunning());
    QVERIFY(!animation.isPaused());
    QTest::qWait(100);
    animation.stop();
    QVERIFY(rect.x() > x);
}

void tst_qmlanimations::dotProperty()
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
    QVERIFY(animation.currentTime() == 125);
    QCOMPARE(rect.border()->width(), 5);
}

void tst_qmlanimations::badTypes()
{
    //don't crash
    {
        QmlView *view = new QmlView;
        view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/badtype1.qml"));

        view->execute();
        qApp->processEvents();

        delete view;
    }

    //make sure we get a compiler error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/badtype2.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QmlComponent: Component is not ready");
        c.create();

        QVERIFY(c.errors().count() == 1);
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Invalid property assignment: double expected"));
    }

    //make sure we get a compiler error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/badtype3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QmlComponent: Component is not ready");
        c.create();

        QVERIFY(c.errors().count() == 1);
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Invalid property assignment: color expected"));
    }

    //don't crash
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/badtype4.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(1000 + 50);
        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("MyRect");
        QVERIFY(myRect);
        QCOMPARE(myRect->x(),qreal(200));
    }
}

void tst_qmlanimations::badProperties()
{
    //make sure we get a runtime error
    {
        QmlEngine engine;

        QmlComponent c1(&engine, QUrl::fromLocalFile(SRCDIR "/data/badproperty1.qml"));
        QByteArray message = "QML ColorAnimation (" + QUrl::fromLocalFile(SRCDIR "/data/badproperty1.qml").toString().toUtf8() + ":18:9) Cannot animate non-existent property \"border.colr\"";
        QTest::ignoreMessage(QtWarningMsg, message);
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c1.create());
        QVERIFY(rect);

        QmlComponent c2(&engine, QUrl::fromLocalFile(SRCDIR "/data/badproperty2.qml"));
        message = "QML ColorAnimation (" + QUrl::fromLocalFile(SRCDIR "/data/badproperty2.qml").toString().toUtf8() + ":18:9) Cannot animate read-only property \"border\"";
        QTest::ignoreMessage(QtWarningMsg, message);
        rect = qobject_cast<QmlGraphicsRectangle*>(c2.create());
        QVERIFY(rect);

        //### should we warn here are well?
        //rect->setState("state1");
    }
}

//test animating mixed types with property animation in a transition
//for example, int + real; color + real; etc
void tst_qmlanimations::mixedTypes()
{
    //assumes border.width stays a real -- not real robust
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/mixedtype1.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("MyRect");
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->border()->width() > 1 && myRect->border()->width() < 10);
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/mixedtype2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("MyRect");
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->color() != QColor("red") && myRect->color() != QColor("blue"));
    }
}

void tst_qmlanimations::properties()
{
    const int waitDuration = 300;
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/properties.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/properties2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/properties3.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(300));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/properties4.qml"));
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
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/properties5.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(100));
        QTIMED_COMPARE(myRect->y(),qreal(200));
    }
}

void tst_qmlanimations::propertiesTransition()
{
    const int waitDuration = 300;
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition.qml"));
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
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition2.qml"));
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
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition3.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(200));
        QCOMPARE(myRect->y(),qreal(100));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition4.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(100));
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition5.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(100));
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(200));
    }

    /*{
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/propertiesTransition6.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlGraphicsRectangle *myRect = rect->findChild<QmlGraphicsRectangle*>("TheRect");
        QVERIFY(myRect);
        rect->setState("moved");
        QCOMPARE(myRect->x(),qreal(100));
        QTest::qWait(waitDuration);
        QTIMED_COMPARE(myRect->x(),qreal(100));
    }*/
}

void tst_qmlanimations::easingStringConversion()
{
    QmlNumberAnimation *animation = new QmlNumberAnimation;
    animation->setEasing("easeInOutQuad");
    QCOMPARE(animation->easing(),QLatin1String("easeInOutQuad"));
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve(), QEasingCurve(QEasingCurve::InOutQuad));

    animation->setEasing("OutQuad");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve(), QEasingCurve(QEasingCurve::OutQuad));

    animation->setEasing("easeOutBounce(amplitude: 5)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::OutBounce);
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().amplitude(), qreal(5));

    animation->setEasing("easeOutElastic(amplitude: 5, period: 3)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::OutElastic);
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().amplitude(), qreal(5));
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().period(), qreal(3));

    animation->setEasing("easeInOutBack(overshoot: 2)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::InOutBack);
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().overshoot(), qreal(2));

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Unmatched parenthesis in easing function \"easeInOutBack(overshoot: 2\"");
    animation->setEasing("easeInOutBack(overshoot: 2");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::Linear);

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Easing function \"InOutBack(overshoot: 2)\" must start with \"ease\"");
    animation->setEasing("InOutBack(overshoot: 2)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::Linear);

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Unknown easing curve \"NonExistantEase\"");
    animation->setEasing("NonExistantEase");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::Linear);

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Improperly specified parameter in easing function \"easeInOutElastic(amplitude 5)\"");
    animation->setEasing("easeInOutElastic(amplitude 5)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::InOutElastic);

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Improperly specified parameter in easing function \"easeInOutElastic(amplitude: yes)\"");
    animation->setEasing("easeInOutElastic(amplitude: yes)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::InOutElastic);
    QVERIFY(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().amplitude() != qreal(5));

    QTest::ignoreMessage(QtWarningMsg, "QML NumberAnimation (unknown location) Unknown easing parameter \"nonexistentproperty\"");
    animation->setEasing("easeOutQuad(nonexistentproperty: 12)");
    QCOMPARE(static_cast<QVariantAnimation*>(((QmlAbstractAnimation*)animation)->qtAnimation())->easingCurve().type(), QEasingCurve::OutQuad);

    delete animation;
}

void tst_qmlanimations::invalidDuration()
{
    QmlPropertyAnimation *animation = new QmlPropertyAnimation;
    QTest::ignoreMessage(QtWarningMsg, "QML PropertyAnimation (unknown location) Cannot set a duration of < 0");
    animation->setDuration(-1);
    QCOMPARE(animation->duration(), 250);

    QmlPauseAnimation *pauseAnimation = new QmlPauseAnimation;
    QTest::ignoreMessage(QtWarningMsg, "QML PauseAnimation (unknown location) Cannot set a duration of < 0");
    pauseAnimation->setDuration(-1);
    QCOMPARE(pauseAnimation->duration(), 250);
}

void tst_qmlanimations::attached()
{
    QmlEngine engine;

    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/attached.qml"));
    QTest::ignoreMessage(QtDebugMsg, "off");
    QTest::ignoreMessage(QtDebugMsg, "on");
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);
}

void tst_qmlanimations::propertyValueSourceDefaultStart()
{
    {
        QmlEngine engine;

        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/valuesource.qml"));

        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlAbstractAnimation *myAnim = rect->findChild<QmlAbstractAnimation*>("MyAnim");
        QVERIFY(myAnim);
        QVERIFY(myAnim->isRunning());
    }

    {
        QmlEngine engine;

        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/valuesource2.qml"));

        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlAbstractAnimation *myAnim = rect->findChild<QmlAbstractAnimation*>("MyAnim");
        QVERIFY(myAnim);
        QVERIFY(myAnim->isRunning() == false);
    }

    {
        QmlEngine engine;

        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/dontAutoStart.qml"));

        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        QmlAbstractAnimation *myAnim = rect->findChild<QmlAbstractAnimation*>("MyAnim");
        QVERIFY(myAnim && myAnim->qtAnimation());
        QVERIFY(myAnim->qtAnimation()->state() == QAbstractAnimation::Stopped);
    }
}

QTEST_MAIN(tst_qmlanimations)

#include "tst_qmlanimations.moc"
