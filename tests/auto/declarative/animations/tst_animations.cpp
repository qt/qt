#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrect_p.h>
#include <QtDeclarative/QmlNumberAnimation>

class tst_animations : public QObject
{
    Q_OBJECT
public:
    tst_animations() {}

private slots:
    void simpleNumber();
    void simpleColor();
    void alwaysRunToEnd();
    void dotProperty();
    void badTypes();
    void badProperties();
    void mixedTypes();
};

#define QTIMED_COMPARE(lhs, rhs) do { \
    for (int ii = 0; ii < 5; ++ii) { \
        if (lhs == rhs)  \
            break; \
        QTest::qWait(50); \
    } \
    QCOMPARE(lhs, rhs); \
} while (false)
        
void tst_animations::simpleNumber()
{
    QmlGraphicsRect rect;
    QmlNumberAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("x");
    animation.setTo(200);
    animation.start();
    QTest::qWait(animation.duration());
    QTIMED_COMPARE(rect.x(), qreal(200));

    rect.setX(0);
    animation.start();
    animation.pause();
    animation.setCurrentTime(125);
    QCOMPARE(rect.x(), qreal(100));
}

void tst_animations::simpleColor()
{
    QmlGraphicsRect rect;
    QmlColorAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("color");
    animation.setTo(QColor("red"));
    animation.start();
    QTest::qWait(animation.duration());
    QTIMED_COMPARE(rect.color(), QColor("red"));

    rect.setColor(QColor("blue"));
    animation.start();
    animation.pause();
    animation.setCurrentTime(125);
    QCOMPARE(rect.color(), QColor::fromRgbF(0.498039, 0, 0.498039, 1));
}

void tst_animations::alwaysRunToEnd()
{
    QmlGraphicsRect rect;
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
    QmlGraphicsRect rect;
    QmlNumberAnimation animation;
    animation.setTarget(&rect);
    animation.setProperty("border.width");
    animation.setTo(10);
    animation.start();
    QTest::qWait(animation.duration());
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
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(1000 + 50);
        QmlGraphicsRect *myRect = qobject_cast<QmlGraphicsRect*>(rect->QGraphicsObject::children().at(3));    //### not robust
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
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        QTest::ignoreMessage(QtWarningMsg, "QML QmlColorAnimation (file://" SRCDIR "/data/badproperty1.qml:22:9) Cannot animate non-existant property \"pen.colr\"");
        rect->setState("state1");
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
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRect *myRect = qobject_cast<QmlGraphicsRect*>(rect->QGraphicsObject::children().at(3));    //### not robust
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->border()->width() > 1 && myRect->border()->width() < 10);
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/mixedtype2.qml"));
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        rect->setState("state1");
        QTest::qWait(500);
        QmlGraphicsRect *myRect = qobject_cast<QmlGraphicsRect*>(rect->QGraphicsObject::children().at(3));    //### not robust
        QVERIFY(myRect);

        //rather inexact -- is there a better way?
        QVERIFY(myRect->x() > 100 && myRect->x() < 200);
        QVERIFY(myRect->color() != QColor("red") && myRect->color() != QColor("blue"));
    }
}

QTEST_MAIN(tst_animations)

#include "tst_animations.moc"
