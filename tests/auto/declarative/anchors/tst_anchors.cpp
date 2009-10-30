#include <qtest.h>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrect_p.h>
#include <QtDeclarative/private/qmlgraphicsanchors_p_p.h>


class tst_anchors : public QObject
{
    Q_OBJECT
public:
    tst_anchors() {}

    template<typename T>
    T *findItem(QmlGraphicsItem *parent, const QString &id);

private slots:
    void basicAnchors();
    void loops();
    void illegalSets();
    void reset();
    void nullItem();
};

/*
   Find an item with the specified id.
*/
template<typename T>
T *tst_anchors::findItem(QmlGraphicsItem *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    QList<QGraphicsItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(children.at(i)->toGraphicsObject());
        if (item) {
            if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
                return static_cast<T*>(item);
            }
            item = findItem<T>(item, objectName);
            if (item)
                return static_cast<T*>(item);
        }
    }

    return 0;
}

void tst_anchors::basicAnchors()
{
    QmlView *view = new QmlView;
    view->setUrl(QUrl("file://" SRCDIR "/data/anchors.qml"));

    view->execute();
    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect1"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect2"))->x(), 122.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect3"))->x(), 74.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect4"))->x(), 16.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect5"))->x(), 112.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect7"))->x(), 0.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect8"))->x(), 240.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect9"))->x(), 120.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect10"))->x(), -10.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect11"))->x(), 230.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect13"))->y(), 20.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect15"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect15"))->width(), 96.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect16"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect16"))->width(), 192.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect17"))->x(), -70.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect17"))->width(), 192.0);

    //vertical stretch
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect18"))->y(), 20.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect18"))->height(), 40.0);

    //more parent horizontal
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect19"))->x(), 115.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect20"))->x(), 235.0);
    QCOMPARE(findItem<QmlGraphicsRect>(view->root(), QLatin1String("Rect21"))->x(), -5.0);

    delete view;
}

// mostly testing that we don't crash
void tst_anchors::loops()
{
    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop1.qml"));

        QString expect = "QML QmlGraphicsText (" + view->url().toString() + ":7:5" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/loop2.qml"));

        QString expect = "QML QmlGraphicsImage (" + view->url().toString() + ":14:3" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }
}

void tst_anchors::illegalSets()
{
    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal1.qml"));

        QString expect = "QML QmlGraphicsRect (" + view->url().toString() + ":7:5" + ") Can't specify left, right, and hcenter anchors.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal2.qml"));

        QString expect = "QML QmlGraphicsText (" + view->url().toString() + ":7:5" + ") Baseline anchor can't be used in conjunction with top, bottom, or vcenter anchors.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        //qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl("file://" SRCDIR "/data/illegal3.qml"));

        QString expect = "QML QmlGraphicsRect (" + view->url().toString() + ":9:5" + ") Can't anchor to an item that isn't a parent or sibling.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        //qApp->processEvents();

        delete view;
    }
}

void tst_anchors::reset()
{
    QmlGraphicsItem *aItem = new QmlGraphicsItem;
    QmlGraphicsAnchorLine anchor;
    anchor.item = aItem;
    anchor.anchorLine = QmlGraphicsAnchorLine::Top;

    QmlGraphicsItem *item = new QmlGraphicsItem;
    item->anchors()->setBottom(anchor);
    QCOMPARE(item->anchors()->usedAnchors().testFlag(QmlGraphicsAnchors::HasBottomAnchor), true);

    item->anchors()->resetBottom();
    QCOMPARE(item->anchors()->usedAnchors().testFlag(QmlGraphicsAnchors::HasBottomAnchor), false);
}

void tst_anchors::nullItem()
{
    QmlGraphicsAnchorLine anchor;

    QTest::ignoreMessage(QtWarningMsg, "QML QmlGraphicsItem (unknown location) Can't anchor to a null item.");
    QmlGraphicsItem *item = new QmlGraphicsItem;
    item->anchors()->setBottom(anchor);
}

QTEST_MAIN(tst_anchors)

#include "tst_anchors.moc"
