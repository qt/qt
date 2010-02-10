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
#include <QSignalSpy>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlgraphicstext_p.h>
#include <QtDeclarative/private/qmlgraphicsanchors_p_p.h>

Q_DECLARE_METATYPE(QmlGraphicsAnchors::UsedAnchor)
Q_DECLARE_METATYPE(QmlGraphicsAnchorLine::AnchorLine)


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
    void illegalSets_data();
    void reset();
    void reset_data();
    void resetConvenience();
    void nullItem();
    void nullItem_data();
    void crash1();
    void centerIn();
    void fill();
    void margins();
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
    view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/anchors.qml"));

    view->execute();
    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect1"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect2"))->x(), 122.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect3"))->x(), 74.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect4"))->x(), 16.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect5"))->x(), 112.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect7"))->x(), 0.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect8"))->x(), 240.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect9"))->x(), 120.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect10"))->x(), -10.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect11"))->x(), 230.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect13"))->y(), 20.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect15"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect15"))->width(), 96.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect16"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect16"))->width(), 192.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect17"))->x(), -70.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect17"))->width(), 192.0);

    //vertical stretch
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect18"))->y(), 20.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect18"))->height(), 40.0);

    //more parent horizontal
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect19"))->x(), 115.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect20"))->x(), 235.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect21"))->x(), -5.0);

    //centerIn
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect22"))->x(), 69.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect22"))->y(), 5.0);

     //margins
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect23"))->x(), 31.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect23"))->y(), 5.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect23"))->width(), 86.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect23"))->height(), 10.0);

    // offsets
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect24"))->x(), 26.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect25"))->y(), 60.0);
    QCOMPARE(findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("rect26"))->y(), 5.0);

    //baseline
    QmlGraphicsText *text1 = findItem<QmlGraphicsText>(view->root(), QLatin1String("text1"));
    QmlGraphicsText *text2 = findItem<QmlGraphicsText>(view->root(), QLatin1String("text2"));
    QCOMPARE(text1->y(), text2->y());

    delete view;
}

// mostly testing that we don't crash
void tst_anchors::loops()
{
    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/loop1.qml"));

        QString expect = "QML Text (" + view->url().toString() + ":6:5" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }

    {
        QmlView *view = new QmlView;

        view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/loop2.qml"));

        QString expect = "QML Image (" + view->url().toString() + ":8:3" + ") Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        view->execute();
        qApp->processEvents();

        delete view;
    }
}

void tst_anchors::illegalSets()
{
    QFETCH(QString, qml);
    QFETCH(QString, warning);

    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1());

    QmlEngine engine;
    QmlComponent component(&engine);
    component.setData(QByteArray("import Qt 4.6\n" + qml.toUtf8()), QUrl::fromLocalFile(""));
    if (!component.isReady())
        qWarning() << "Test errors:" << component.errors();
    QVERIFY(component.isReady());
    QObject *o = component.create();
    delete o;
}

void tst_anchors::illegalSets_data()
{
    QTest::addColumn<QString>("qml");
    QTest::addColumn<QString>("warning");

    QTest::newRow("H - too many anchors")
        << "Rectangle { id: rect; Rectangle { anchors.left: rect.left; anchors.right: rect.right; anchors.horizontalCenter: rect.horizontalCenter } }"
        << "QML Rectangle (file::2:23) Can't specify left, right, and hcenter anchors.";

    foreach (const QString &side, QStringList() << "left" << "right") {
        QTest::newRow("H - anchor to V")
            << QString("Rectangle { Rectangle { anchors.%1: parent.top } }").arg(side)
            << "QML Rectangle (file::2:13) Can't anchor a horizontal edge to a vertical edge.";

        QTest::newRow("H - anchor to non parent/sibling")
            << QString("Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.%1: rect.%1 } }").arg(side)
            << "QML Rectangle (file::2:45) Can't anchor to an item that isn't a parent or sibling.";

        QTest::newRow("H - anchor to self")
            << QString("Rectangle { id: rect; anchors.%1: rect.%1 }").arg(side)
            << "QML Rectangle (file::2:1) Can't anchor item to self.";
    }


    QTest::newRow("V - too many anchors")
        << "Rectangle { id: rect; Rectangle { anchors.top: rect.top; anchors.bottom: rect.bottom; anchors.verticalCenter: rect.verticalCenter } }"
        << "QML Rectangle (file::2:23) Can't specify top, bottom, and vcenter anchors.";

    QTest::newRow("V - too many anchors with baseline")
        << "Rectangle { Text { id: text1; text: \"Hello\" } Text { anchors.baseline: text1.baseline; anchors.top: text1.top; } }"
        << "QML Text (file::2:47) Baseline anchor can't be used in conjunction with top, bottom, or vcenter anchors.";

    foreach (const QString &side, QStringList() << "top" << "bottom" << "baseline") {

        QTest::newRow("V - anchor to H")
            << QString("Rectangle { Rectangle { anchors.%1: parent.left } }").arg(side)
            << "QML Rectangle (file::2:13) Can't anchor a vertical edge to a horizontal edge.";

        QTest::newRow("V - anchor to non parent/sibling")
            << QString("Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.%1: rect.%1 } }").arg(side)
            << "QML Rectangle (file::2:45) Can't anchor to an item that isn't a parent or sibling.";

        QTest::newRow("V - anchor to self")
            << QString("Rectangle { id: rect; anchors.%1: rect.%1 }").arg(side)
            << "QML Rectangle (file::2:1) Can't anchor item to self.";
    }


    QTest::newRow("centerIn - anchor to non parent/sibling")
        << "Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.centerIn: rect} }"
        << "QML Rectangle (file::2:45) Can't anchor to an item that isn't a parent or sibling.";


    QTest::newRow("fill - anchor to non parent/sibling")
        << "Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.fill: rect} }"
        << "QML Rectangle (file::2:45) Can't anchor to an item that isn't a parent or sibling.";
}

void tst_anchors::reset()
{
    QFETCH(QString, side);
    QFETCH(QmlGraphicsAnchorLine::AnchorLine, anchorLine);
    QFETCH(QmlGraphicsAnchors::UsedAnchor, usedAnchor);

    QmlGraphicsItem *baseItem = new QmlGraphicsItem;

    QmlGraphicsAnchorLine anchor;
    anchor.item = baseItem;
    anchor.anchorLine = anchorLine;

    QmlGraphicsItem *item = new QmlGraphicsItem;

    const QMetaObject *meta = item->anchors()->metaObject();
    QMetaProperty p = meta->property(meta->indexOfProperty(side.toUtf8().constData()));

    QVERIFY(p.write(item->anchors(), qVariantFromValue(anchor)));
    QCOMPARE(item->anchors()->usedAnchors().testFlag(usedAnchor), true);

    QVERIFY(p.reset(item->anchors()));
    QCOMPARE(item->anchors()->usedAnchors().testFlag(usedAnchor), false);

    delete item;
    delete baseItem;
}

void tst_anchors::reset_data()
{
    QTest::addColumn<QString>("side");
    QTest::addColumn<QmlGraphicsAnchorLine::AnchorLine>("anchorLine");
    QTest::addColumn<QmlGraphicsAnchors::UsedAnchor>("usedAnchor");

    QTest::newRow("left") << "left" << QmlGraphicsAnchorLine::Left << QmlGraphicsAnchors::HasLeftAnchor;
    QTest::newRow("top") << "top" << QmlGraphicsAnchorLine::Top << QmlGraphicsAnchors::HasTopAnchor;
    QTest::newRow("right") << "right" << QmlGraphicsAnchorLine::Right << QmlGraphicsAnchors::HasRightAnchor;
    QTest::newRow("bottom") << "bottom" << QmlGraphicsAnchorLine::Bottom << QmlGraphicsAnchors::HasBottomAnchor;

    QTest::newRow("hcenter") << "horizontalCenter" << QmlGraphicsAnchorLine::HCenter << QmlGraphicsAnchors::HasHCenterAnchor;
    QTest::newRow("vcenter") << "verticalCenter" << QmlGraphicsAnchorLine::VCenter << QmlGraphicsAnchors::HasVCenterAnchor;
    QTest::newRow("baseline") << "baseline" << QmlGraphicsAnchorLine::Baseline << QmlGraphicsAnchors::HasBaselineAnchor;
}

void tst_anchors::resetConvenience()
{
    QmlGraphicsItem *baseItem = new QmlGraphicsItem;
    QmlGraphicsItem *item = new QmlGraphicsItem;

    //fill
    item->anchors()->setFill(baseItem);
    QVERIFY(item->anchors()->fill() == baseItem);
    item->anchors()->resetFill();
    QVERIFY(item->anchors()->fill() == 0);

    //centerIn
    item->anchors()->setCenterIn(baseItem);
    QVERIFY(item->anchors()->centerIn() == baseItem);
    item->anchors()->resetCenterIn();
    QVERIFY(item->anchors()->centerIn() == 0);

    delete item;
    delete baseItem;
}

void tst_anchors::nullItem()
{
    QFETCH(QString, side);

    QmlGraphicsAnchorLine anchor;
    QmlGraphicsItem *item = new QmlGraphicsItem;

    const QMetaObject *meta = item->anchors()->metaObject();
    QMetaProperty p = meta->property(meta->indexOfProperty(side.toUtf8().constData()));

    QTest::ignoreMessage(QtWarningMsg, "QML Item (unknown location) Can't anchor to a null item.");
    QVERIFY(p.write(item->anchors(), qVariantFromValue(anchor)));

    delete item;
}

void tst_anchors::nullItem_data()
{
    QTest::addColumn<QString>("side");

    QTest::newRow("left") << "left";
    QTest::newRow("top") << "top";
    QTest::newRow("right") << "right";
    QTest::newRow("bottom") << "bottom";

    QTest::newRow("hcenter") << "horizontalCenter";
    QTest::newRow("vcenter") << "verticalCenter";
    QTest::newRow("baseline") << "baseline";
}

void tst_anchors::crash1()
{
    QmlView *view = new QmlView;

    view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/crash1.qml"));

    QString expect = "QML Text (" + view->url().toString() + ":4:5" + ") Possible anchor loop detected on fill.";
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1()); // XXX ideally, should be one message
    view->execute();
    qApp->processEvents();

    delete view;
}

void tst_anchors::fill()
{
    QmlView *view = new QmlView;

    view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/fill.qml"));

    view->execute();
    qApp->processEvents();
    QmlGraphicsRectangle* rect = findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("filler"));
    QCOMPARE(rect->x(), 0.0 + 10.0);
    QCOMPARE(rect->y(), 0.0 + 30.0);
    QCOMPARE(rect->width(), 200.0 - 10.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 30.0 - 40.0);
    //Alter Offsets (QTBUG-6631)
    rect->anchors()->setLeftMargin(20.0);
    rect->anchors()->setRightMargin(0.0);
    rect->anchors()->setBottomMargin(0.0);
    rect->anchors()->setTopMargin(10.0);
    QCOMPARE(rect->x(), 0.0 + 20.0);
    QCOMPARE(rect->y(), 0.0 + 10.0);
    QCOMPARE(rect->width(), 200.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 10.0);

    delete view;
}

void tst_anchors::centerIn()
{
    QmlView *view = new QmlView;

    view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/centerin.qml"));

    view->execute();
    qApp->processEvents();
    QmlGraphicsRectangle* rect = findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("centered"));
    QCOMPARE(rect->x(), 75.0 + 10);
    QCOMPARE(rect->y(), 75.0 + 30);
    //Alter Offsets (QTBUG-6631)
    rect->anchors()->setHorizontalCenterOffset(-20.0);
    rect->anchors()->setVerticalCenterOffset(-10.0);
    QCOMPARE(rect->x(), 75.0 - 20.0);
    QCOMPARE(rect->y(), 75.0 - 10.0);

    delete view;
}

void tst_anchors::margins()
{
    QmlView *view = new QmlView;

    view->setUrl(QUrl::fromLocalFile(SRCDIR "/data/margins.qml"));

    view->execute();
    qApp->processEvents();
    QmlGraphicsRectangle* rect = findItem<QmlGraphicsRectangle>(view->root(), QLatin1String("filler"));
    QCOMPARE(rect->x(), 5.0);
    QCOMPARE(rect->y(), 6.0);
    QCOMPARE(rect->width(), 200.0 - 5.0 - 10.0);
    QCOMPARE(rect->height(), 200.0 - 6.0 - 10.0);

    rect->anchors()->setTopMargin(0.0);
    rect->anchors()->setMargins(20.0);

    QCOMPARE(rect->x(), 5.0);
    QCOMPARE(rect->y(), 20.0);
    QCOMPARE(rect->width(), 200.0 - 5.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 20.0 - 20.0);

    delete view;
}

QTEST_MAIN(tst_anchors)

#include "tst_anchors.moc"
