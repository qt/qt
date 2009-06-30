#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qfxrect.h>

class tst_animations : public QObject
{
    Q_OBJECT
public:
    tst_animations() {}

private slots:
    void badTypes();
    //void mixedTypes();
};

void tst_animations::badTypes()
{
    //don't crash
    {
        QFxView *view = new QFxView;
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
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Cannot convert value \"blue\" to double number"));
    }

    //make sure we get a compiler error
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/badtype3.qml"));
        QTest::ignoreMessage(QtWarningMsg, "QmlComponent: Component is not ready");
        c.create();

        QVERIFY(c.errors().count() == 1);
        QCOMPARE(c.errors().at(0).description(), QLatin1String("Cannot convert value \"10\" to color"));
    }
}

/*//test animating mixed types with property animation
  //for example, int + real; color + real; etc
void tst_animations::mixedTypes()
{
    //### this one isn't real robust because width will likely change to real as well
    {
        QFxView *view = new QFxView;
        view->setUrl(QUrl("file://" SRCDIR "/data/mixedtype1.qml"));

        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QFxView *view = new QFxView;
        view->setUrl(QUrl("file://" SRCDIR "/data/mixedtype2.qml"));

        view->execute();
        qApp->processEvents();

        delete view;
    }
}*/

QTEST_MAIN(tst_animations)

#include "tst_animations.moc"
