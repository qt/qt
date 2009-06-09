#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qfxview.h>
#include <QtDeclarative/qfxrect.h>

class tst_anchors : public QObject
{
    Q_OBJECT
public:
    tst_anchors() {}

    template<typename T>
    T *findItem(QFxItem *parent, const QString &id);

private slots:
    void basicAnchors();
    void loops();
    void illegalSets();
};

/*
   Find an item with the specified id.
*/
template<typename T>
T *tst_anchors::findItem(QFxItem *parent, const QString &id)
{
    const QMetaObject &mo = T::staticMetaObject;
    for (int i = 0; i < parent->QSimpleCanvasItem::children().count(); ++i) {
        QFxItem *item = qobject_cast<QFxItem*>(parent->QSimpleCanvasItem::children().at(i));
        if (mo.cast(item) && (id.isEmpty() || item->id() == id)) {
            return static_cast<T*>(item);
        }
        item = findItem<T>(item, id);
        if (item)
            return static_cast<T*>(item);
    }

    return 0;
}

void tst_anchors::basicAnchors()
{
    QFxView *view = new QFxView;
    view->setUrl(QUrl("file://" SRCDIR "/data/anchors.qml"));

    view->execute();
    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect1"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect2"))->x(), 122.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect3"))->x(), 74.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect4"))->x(), 16.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect5"))->x(), 112.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect7"))->x(), 0.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect8"))->x(), 240.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect9"))->x(), 120.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect10"))->x(), -10.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect11"))->x(), 230.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect13"))->y(), 20.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect15"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect15"))->width(), 96.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect16"))->x(), 26.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect16"))->width(), 192.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect17"))->x(), -70.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect17"))->width(), 192.0);

    //vertical stretch
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect18"))->y(), 20.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect18"))->height(), 40.0);

    //more parent horizontal
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect19"))->x(), 115.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect20"))->x(), 235.0);
    QCOMPARE(findItem<QFxRect>(view->root(), QLatin1String("Rect21"))->x(), -5.0);

    delete view;
}

// mostly testing that we don't crash
void tst_anchors::loops()
{
    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop1.qml"));

        QTest::ignoreMessage(QtWarningMsg, "QML QFxText (unknown location): Possible anchor loop detected on horizontal anchor. "); //x5
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop2.qml"));

        QTest::ignoreMessage(QtWarningMsg, "QML QFxImage (unknown location): Possible anchor loop detected on horizontal anchor. ");    //x3
        view->execute();
        qApp->processEvents();

        delete view;
    }
}

void tst_anchors::illegalSets()
{
    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal1.qml"));

        QTest::ignoreMessage(QtWarningMsg, "QML QFxRect (unknown location): Can't specify left, right, and hcenter anchors. ");
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal2.qml"));

        QTest::ignoreMessage(QtWarningMsg, "QML QFxText (unknown location): Baseline anchor can't be used in conjunction with top, bottom, or vcenter anchors. ");
        view->execute();
        //qApp->processEvents();

        delete view;
    }

    {
        QFxView *view = new QFxView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal3.qml"));

        QTest::ignoreMessage(QtWarningMsg, "QML QFxRect (unknown location): Can't anchor to an item that isn't a parent or sibling. ");
        view->execute();
        //qApp->processEvents();

        delete view;
    }
}

QTEST_MAIN(tst_anchors)

#include "tst_anchors.moc"
