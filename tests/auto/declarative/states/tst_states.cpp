#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxrect.h>

class tst_states : public QObject
{
    Q_OBJECT
public:
    tst_states() {}

private slots:
    void basicChanges();
    void basicExtension();
    void basicBinding();
};

void tst_states::basicChanges()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges2.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges3.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);
        //### we should be checking that this is an implicit rather than explicit 1 (which currently fails)

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

    }
}

void tst_states::basicExtension()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicExtension.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/fakeExtension.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
    }
}

void tst_states::basicBinding()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding2.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("green"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding3.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("red"));
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor2", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor2", QColor("green"));
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding4.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
        rect->setProperty("sourceColor", QColor("purple"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("purple"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
    }
}

QTEST_MAIN(tst_states)

#include "tst_states.moc"
