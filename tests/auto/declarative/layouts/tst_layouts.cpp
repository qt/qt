#include <QtTest/QtTest>
#include <qlistmodelinterface.h>
#include <qmlview.h>
#include <qfxrect.h>
#include <qmlexpression.h>

class tst_QFxLayouts : public QObject
{
    Q_OBJECT
public:
    tst_QFxLayouts();

private slots:
    void test_horizontal();
    void test_horizontal_spacing();
    void test_vertical();
    void test_vertical_spacing();
    void test_grid();
    void test_grid_spacing();

private:
    QmlView *createView(const QString &filename);
};

tst_QFxLayouts::tst_QFxLayouts()
{
}

void tst_QFxLayouts::test_horizontal()
{
    QmlView *canvas = createView(SRCDIR "/data/horizontal.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);

    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);

    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
}

void tst_QFxLayouts::test_horizontal_spacing()
{
    QmlView *canvas = createView(SRCDIR "/data/horizontal-spacing.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);

    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);

    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 60.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 90.0);
    QCOMPARE(three->y(), 0.0);
}

void tst_QFxLayouts::test_vertical()
{
    QmlView *canvas = createView(SRCDIR "/data/vertical.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);

    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);

    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 50.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 60.0);
}

void tst_QFxLayouts::test_vertical_spacing()
{
    QmlView *canvas = createView(SRCDIR "/data/vertical-spacing.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);

    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);

    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 0.0);
    QCOMPARE(two->y(), 60.0);
    QCOMPARE(three->x(), 0.0);
    QCOMPARE(three->y(), 80.0);
}

void tst_QFxLayouts::test_grid()
{
    QmlView *canvas = createView("data/grid.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);
    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);
    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);
    QFxRect *four = canvas->root()->findChild<QFxRect*>("four");
    QVERIFY(four != 0);
    QFxRect *five = canvas->root()->findChild<QFxRect*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 50.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 70.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 50.0);
    QCOMPARE(five->x(), 50.0);
    QCOMPARE(five->y(), 50.0);
}

void tst_QFxLayouts::test_grid_spacing()
{
    QmlView *canvas = createView("data/grid-spacing.qml");

    canvas->execute();
    qApp->processEvents();

    QFxRect *one = canvas->root()->findChild<QFxRect*>("one");
    QVERIFY(one != 0);
    QFxRect *two = canvas->root()->findChild<QFxRect*>("two");
    QVERIFY(two != 0);
    QFxRect *three = canvas->root()->findChild<QFxRect*>("three");
    QVERIFY(three != 0);
    QFxRect *four = canvas->root()->findChild<QFxRect*>("four");
    QVERIFY(four != 0);
    QFxRect *five = canvas->root()->findChild<QFxRect*>("five");
    QVERIFY(five != 0);

    QCOMPARE(one->x(), 0.0);
    QCOMPARE(one->y(), 0.0);
    QCOMPARE(two->x(), 54.0);
    QCOMPARE(two->y(), 0.0);
    QCOMPARE(three->x(), 78.0);
    QCOMPARE(three->y(), 0.0);
    QCOMPARE(four->x(), 0.0);
    QCOMPARE(four->y(), 54.0);
    QCOMPARE(five->x(), 54.0);
    QCOMPARE(five->y(), 54.0);
}

QmlView *tst_QFxLayouts::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString xml = file.readAll();
    canvas->setQml(xml, filename);

    return canvas;
}


QTEST_MAIN(tst_QFxLayouts)

#include "tst_layouts.moc"
