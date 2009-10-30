#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrect_p.h>
#include <private/qmlanimation_p.h>

class tst_behaviors : public QObject
{
    Q_OBJECT
public:
    tst_behaviors() {}

private slots:
    void simpleBehavior();
    void scriptTriggered();
    void cppTriggered();
    void loop();
    void colorBehavior();
    void replaceBinding();
    //void transitionOverrides();
    void group();
};

void tst_behaviors::simpleBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/simple.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    rect->setState("moved");
    QTest::qWait(100);
    qreal x = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"))->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::scriptTriggered()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/scripttrigger.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    rect->setColor(QColor("red"));
    QTest::qWait(100);
    qreal x = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"))->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::cppTriggered()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/cpptrigger.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    QmlGraphicsRect *innerRect = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"));
    QVERIFY(innerRect);

    innerRect->setProperty("x", 200);
    QTest::qWait(100);
    qreal x = innerRect->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::loop()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/loop.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    //don't crash
    rect->setState("moved");
}

void tst_behaviors::colorBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/color.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    rect->setState("red");
    QTest::qWait(100);
    QColor color = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"))->color();
    QVERIFY(color != QColor("red") && color != QColor("green"));  //i.e. the behavior has been triggered
}

void tst_behaviors::replaceBinding()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/binding.qml"));
    QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
    QVERIFY(rect);

    rect->setState("moved");
    QTest::qWait(100);
    QmlGraphicsRect *innerRect = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"));
    QVERIFY(innerRect);
    qreal x = innerRect->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("basex", 10);
    QCOMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("movedx", 210);
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)210);

    rect->setState("");
    QTest::qWait(100);
    x = innerRect->x();
    QVERIFY(x > 10 && x < 210);  //i.e. the behavior has been triggered
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("movedx", 200);
    QCOMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("basex", 20);
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)20);
}

void tst_behaviors::group()
{
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/groupProperty.qml"));
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        QTest::qWait(100);
        qreal x = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"))->x();
        QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/groupProperty2.qml"));
        QmlGraphicsRect *rect = qobject_cast<QmlGraphicsRect*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        QTest::qWait(100);
        qreal x = qobject_cast<QmlGraphicsRect*>(rect->findChild<QmlGraphicsRect*>("MyRect"))->x();
        QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    }
}

QTEST_MAIN(tst_behaviors)

#include "tst_behaviors.moc"
