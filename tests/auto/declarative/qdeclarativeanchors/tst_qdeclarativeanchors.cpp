/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QSignalSpy>
#include <QtGui/QGraphicsWidget>
#include <private/qgraphicsitem_p.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativeanchors_p_p.h>
#include <QtDeclarative/private/qdeclarativeitem_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

Q_DECLARE_METATYPE(QDeclarativeAnchors::Anchor)
Q_DECLARE_METATYPE(QDeclarativeAnchorLine::AnchorLine)


class tst_qdeclarativeanchors : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeanchors() {}

    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id);
    QGraphicsObject *findObject(QGraphicsObject *parent, const QString &objectName);

private slots:
    void basicAnchors();
    void basicAnchorsQGraphicsWidget();
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
    void hvCenter();
    void fill();
    void margins();
};

/*
   Find an item with the specified id.
*/
template<typename T>
T *tst_qdeclarativeanchors::findItem(QGraphicsObject *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    QList<QGraphicsItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(children.at(i)->toGraphicsObject());
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

QGraphicsObject *tst_qdeclarativeanchors::findObject(QGraphicsObject *parent, const QString &objectName)
{
    QList<QGraphicsItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QGraphicsObject *item = children.at(i)->toGraphicsObject();
        if (item) {
            if (objectName.isEmpty() || item->objectName() == objectName) {
                return item;
            }
            item = findObject(item, objectName);
            if (item)
                return item;
        }
    }

    return 0;
}


void tst_qdeclarativeanchors::basicAnchors()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/anchors.qml"));

    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect1"))->x(), 26.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect2"))->x(), 122.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect3"))->x(), 74.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect4"))->x(), 16.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect5"))->x(), 112.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect7"))->x(), 0.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect8"))->x(), 240.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect9"))->x(), 120.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect10"))->x(), -10.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect11"))->x(), 230.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect13"))->y(), 20.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect15"))->x(), 26.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect15"))->width(), 96.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect16"))->x(), 26.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect16"))->width(), 192.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect17"))->x(), -70.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect17"))->width(), 192.0);

    //vertical stretch
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect18"))->y(), 20.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect18"))->height(), 40.0);

    //more parent horizontal
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect19"))->x(), 115.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect20"))->x(), 235.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect21"))->x(), -5.0);

    //centerIn
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect22"))->x(), 69.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect22"))->y(), 5.0);

     //margins
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect23"))->x(), 31.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect23"))->y(), 5.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect23"))->width(), 86.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect23"))->height(), 10.0);

    // offsets
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect24"))->x(), 26.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect25"))->y(), 60.0);
    QCOMPARE(findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("rect26"))->y(), 5.0);

    //baseline
    QDeclarativeText *text1 = findItem<QDeclarativeText>(view->rootObject(), QLatin1String("text1"));
    QDeclarativeText *text2 = findItem<QDeclarativeText>(view->rootObject(), QLatin1String("text2"));
    QCOMPARE(text1->y(), text2->y());

    delete view;
}

void tst_qdeclarativeanchors::basicAnchorsQGraphicsWidget()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/anchorsqgraphicswidget.qml"));

    qApp->processEvents();

    //sibling horizontal
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect1"))->x(), 26.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect2"))->x(), 122.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect3"))->x(), 74.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect4"))->x(), 16.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect5"))->x(), 112.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect6"))->x(), 64.0);

    //parent horizontal
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect7"))->x(), 0.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect8"))->x(), 240.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect9"))->x(), 120.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect10"))->x(), -10.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect11"))->x(), 230.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect12"))->x(), 110.0);

    //vertical
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect13"))->y(), 20.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect14"))->y(), 155.0);

    //stretch
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect15"))->x(), 26.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect15"))->property("width").toReal(), 96.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect16"))->x(), 26.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect16"))->property("width").toReal(), 192.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect17"))->x(), -70.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect17"))->property("width").toReal(), 192.0);

    //vertical stretch
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect18"))->y(), 20.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect18"))->property("height").toReal(), 40.0);

    //more parent horizontal
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect19"))->x(), 115.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect20"))->x(), 235.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect21"))->x(), -5.0);

    //centerIn
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect22"))->x(), 69.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect22"))->y(), 5.0);

     //margins
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect23"))->x(), 31.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect23"))->y(), 5.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect23"))->property("width").toReal(), 86.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect23"))->property("height").toReal(), 10.0);

    // offsets
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect24"))->x(), 26.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect25"))->y(), 60.0);
    QCOMPARE(findObject(view->rootObject(), QLatin1String("rect26"))->y(), 5.0);

    //baseline
    QDeclarativeText *text1 = findItem<QDeclarativeText>(view->rootObject(), QLatin1String("text1"));
    QDeclarativeText *text2 = findItem<QDeclarativeText>(view->rootObject(), QLatin1String("text2"));
    QCOMPARE(text1->y(), text2->y());

    delete view;
}

// mostly testing that we don't crash
void tst_qdeclarativeanchors::loops()
{
    {
        QUrl source(QUrl::fromLocalFile(SRCDIR "/data/loop1.qml"));

        QString expect = source.toString() + ":6:5: QML Text: Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());

        QDeclarativeView *view = new QDeclarativeView;
        view->setSource(source);
        qApp->processEvents();

        delete view;
    }

    {
        QUrl source(QUrl::fromLocalFile(SRCDIR "/data/loop2.qml"));

        QString expect = source.toString() + ":8:3: QML Image: Possible anchor loop detected on horizontal anchor.";
        QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());

        QDeclarativeView *view = new QDeclarativeView;
        view->setSource(source);
        qApp->processEvents();

        delete view;
    }
}

void tst_qdeclarativeanchors::illegalSets()
{
    QFETCH(QString, qml);
    QFETCH(QString, warning);

    QTest::ignoreMessage(QtWarningMsg, warning.toLatin1());

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData(QByteArray("import Qt 4.7\n" + qml.toUtf8()), QUrl::fromLocalFile(""));
    if (!component.isReady())
        qWarning() << "Test errors:" << component.errors();
    QVERIFY(component.isReady());
    QObject *o = component.create();
    delete o;
}

void tst_qdeclarativeanchors::illegalSets_data()
{
    QTest::addColumn<QString>("qml");
    QTest::addColumn<QString>("warning");

    QTest::newRow("H - too many anchors")
        << "Rectangle { id: rect; Rectangle { anchors.left: rect.left; anchors.right: rect.right; anchors.horizontalCenter: rect.horizontalCenter } }"
        << "file::2:23: QML Rectangle: Cannot specify left, right, and hcenter anchors.";

    foreach (const QString &side, QStringList() << "left" << "right") {
        QTest::newRow("H - anchor to V")
            << QString("Rectangle { Rectangle { anchors.%1: parent.top } }").arg(side)
            << "file::2:13: QML Rectangle: Cannot anchor a horizontal edge to a vertical edge.";

        QTest::newRow("H - anchor to non parent/sibling")
            << QString("Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.%1: rect.%1 } }").arg(side)
            << "file::2:45: QML Rectangle: Cannot anchor to an item that isn't a parent or sibling.";

        QTest::newRow("H - anchor to self")
            << QString("Rectangle { id: rect; anchors.%1: rect.%1 }").arg(side)
            << "file::2:1: QML Rectangle: Cannot anchor item to self.";
    }


    QTest::newRow("V - too many anchors")
        << "Rectangle { id: rect; Rectangle { anchors.top: rect.top; anchors.bottom: rect.bottom; anchors.verticalCenter: rect.verticalCenter } }"
        << "file::2:23: QML Rectangle: Cannot specify top, bottom, and vcenter anchors.";

    QTest::newRow("V - too many anchors with baseline")
        << "Rectangle { Text { id: text1; text: \"Hello\" } Text { anchors.baseline: text1.baseline; anchors.top: text1.top; } }"
        << "file::2:47: QML Text: Baseline anchor cannot be used in conjunction with top, bottom, or vcenter anchors.";

    foreach (const QString &side, QStringList() << "top" << "bottom" << "baseline") {

        QTest::newRow("V - anchor to H")
            << QString("Rectangle { Rectangle { anchors.%1: parent.left } }").arg(side)
            << "file::2:13: QML Rectangle: Cannot anchor a vertical edge to a horizontal edge.";

        QTest::newRow("V - anchor to non parent/sibling")
            << QString("Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.%1: rect.%1 } }").arg(side)
            << "file::2:45: QML Rectangle: Cannot anchor to an item that isn't a parent or sibling.";

        QTest::newRow("V - anchor to self")
            << QString("Rectangle { id: rect; anchors.%1: rect.%1 }").arg(side)
            << "file::2:1: QML Rectangle: Cannot anchor item to self.";
    }


    QTest::newRow("centerIn - anchor to non parent/sibling")
        << "Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.centerIn: rect} }"
        << "file::2:45: QML Rectangle: Cannot anchor to an item that isn't a parent or sibling.";


    QTest::newRow("fill - anchor to non parent/sibling")
        << "Rectangle { Item { Rectangle { id: rect } } Rectangle { anchors.fill: rect} }"
        << "file::2:45: QML Rectangle: Cannot anchor to an item that isn't a parent or sibling.";
}

void tst_qdeclarativeanchors::reset()
{
    QFETCH(QString, side);
    QFETCH(QDeclarativeAnchorLine::AnchorLine, anchorLine);
    QFETCH(QDeclarativeAnchors::Anchor, usedAnchor);

    QDeclarativeItem *baseItem = new QDeclarativeItem;

    QDeclarativeAnchorLine anchor;
    anchor.item = baseItem;
    anchor.anchorLine = anchorLine;

    QDeclarativeItem *item = new QDeclarativeItem;
    QDeclarativeItemPrivate *itemPrivate = QDeclarativeItemPrivate::get(item);

    const QMetaObject *meta = itemPrivate->anchors()->metaObject();
    QMetaProperty p = meta->property(meta->indexOfProperty(side.toUtf8().constData()));

    QVERIFY(p.write(itemPrivate->anchors(), qVariantFromValue(anchor)));
    QCOMPARE(itemPrivate->anchors()->usedAnchors().testFlag(usedAnchor), true);

    QVERIFY(p.reset(itemPrivate->anchors()));
    QCOMPARE(itemPrivate->anchors()->usedAnchors().testFlag(usedAnchor), false);

    delete item;
    delete baseItem;
}

void tst_qdeclarativeanchors::reset_data()
{
    QTest::addColumn<QString>("side");
    QTest::addColumn<QDeclarativeAnchorLine::AnchorLine>("anchorLine");
    QTest::addColumn<QDeclarativeAnchors::Anchor>("usedAnchor");

    QTest::newRow("left") << "left" << QDeclarativeAnchorLine::Left << QDeclarativeAnchors::LeftAnchor;
    QTest::newRow("top") << "top" << QDeclarativeAnchorLine::Top << QDeclarativeAnchors::TopAnchor;
    QTest::newRow("right") << "right" << QDeclarativeAnchorLine::Right << QDeclarativeAnchors::RightAnchor;
    QTest::newRow("bottom") << "bottom" << QDeclarativeAnchorLine::Bottom << QDeclarativeAnchors::BottomAnchor;

    QTest::newRow("hcenter") << "horizontalCenter" << QDeclarativeAnchorLine::HCenter << QDeclarativeAnchors::HCenterAnchor;
    QTest::newRow("vcenter") << "verticalCenter" << QDeclarativeAnchorLine::VCenter << QDeclarativeAnchors::VCenterAnchor;
    QTest::newRow("baseline") << "baseline" << QDeclarativeAnchorLine::Baseline << QDeclarativeAnchors::BaselineAnchor;
}

void tst_qdeclarativeanchors::resetConvenience()
{
    QDeclarativeItem *baseItem = new QDeclarativeItem;
    QDeclarativeItem *item = new QDeclarativeItem;
    QDeclarativeItemPrivate *itemPrivate = QDeclarativeItemPrivate::get(item);

    //fill
    itemPrivate->anchors()->setFill(baseItem);
    QVERIFY(itemPrivate->anchors()->fill() == baseItem);
    itemPrivate->anchors()->resetFill();
    QVERIFY(itemPrivate->anchors()->fill() == 0);

    //centerIn
    itemPrivate->anchors()->setCenterIn(baseItem);
    QVERIFY(itemPrivate->anchors()->centerIn() == baseItem);
    itemPrivate->anchors()->resetCenterIn();
    QVERIFY(itemPrivate->anchors()->centerIn() == 0);

    delete item;
    delete baseItem;
}

void tst_qdeclarativeanchors::nullItem()
{
    QFETCH(QString, side);

    QDeclarativeAnchorLine anchor;
    QDeclarativeItem *item = new QDeclarativeItem;
    QDeclarativeItemPrivate *itemPrivate = QDeclarativeItemPrivate::get(item);

    const QMetaObject *meta = itemPrivate->anchors()->metaObject();
    QMetaProperty p = meta->property(meta->indexOfProperty(side.toUtf8().constData()));

    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML Item: Cannot anchor to a null item.");
    QVERIFY(p.write(itemPrivate->anchors(), qVariantFromValue(anchor)));

    delete item;
}

void tst_qdeclarativeanchors::nullItem_data()
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

void tst_qdeclarativeanchors::crash1()
{
    QUrl source(QUrl::fromLocalFile(SRCDIR "/data/crash1.qml"));

    QString expect = source.toString() + ":4:5: QML Text: Possible anchor loop detected on fill.";

    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());

    // QT-3245 ... anchor loop detection needs improving.
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());

    QDeclarativeView *view = new QDeclarativeView(source);
    qApp->processEvents();

    delete view;
}

void tst_qdeclarativeanchors::fill()
{
    QDeclarativeView *view = new QDeclarativeView(QUrl::fromLocalFile(SRCDIR "/data/fill.qml"));

    qApp->processEvents();
    QDeclarativeRectangle* rect = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("filler"));
    QDeclarativeItemPrivate *rectPrivate = QDeclarativeItemPrivate::get(rect);
    QCOMPARE(rect->x(), 0.0 + 10.0);
    QCOMPARE(rect->y(), 0.0 + 30.0);
    QCOMPARE(rect->width(), 200.0 - 10.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 30.0 - 40.0);
    //Alter Offsets (tests QTBUG-6631)
    rectPrivate->anchors()->setLeftMargin(20.0);
    rectPrivate->anchors()->setRightMargin(0.0);
    rectPrivate->anchors()->setBottomMargin(0.0);
    rectPrivate->anchors()->setTopMargin(10.0);
    QCOMPARE(rect->x(), 0.0 + 20.0);
    QCOMPARE(rect->y(), 0.0 + 10.0);
    QCOMPARE(rect->width(), 200.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 10.0);

    delete view;
}

void tst_qdeclarativeanchors::centerIn()
{
    QDeclarativeView *view = new QDeclarativeView(QUrl::fromLocalFile(SRCDIR "/data/centerin.qml"));

    qApp->processEvents();
    QDeclarativeRectangle* rect = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("centered"));
    QDeclarativeItemPrivate *rectPrivate = QDeclarativeItemPrivate::get(rect);
    QCOMPARE(rect->x(), 75.0 + 10);
    QCOMPARE(rect->y(), 75.0 + 30);
    //Alter Offsets (tests QTBUG-6631)
    rectPrivate->anchors()->setHorizontalCenterOffset(-20.0);
    rectPrivate->anchors()->setVerticalCenterOffset(-10.0);
    QCOMPARE(rect->x(), 75.0 - 20.0);
    QCOMPARE(rect->y(), 75.0 - 10.0);

    delete view;
}

void tst_qdeclarativeanchors::hvCenter()
{
    QDeclarativeView *view = new QDeclarativeView(QUrl::fromLocalFile(SRCDIR "/data/hvCenter.qml"));

    qApp->processEvents();
    QDeclarativeRectangle* rect = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("centered"));
    QDeclarativeItemPrivate *rectPrivate = QDeclarativeItemPrivate::get(rect);
    // test QTBUG-10999
    QCOMPARE(rect->x(), 10.0);
    QCOMPARE(rect->y(), 19.0);
    delete view;
}

void tst_qdeclarativeanchors::margins()
{
    QDeclarativeView *view = new QDeclarativeView(QUrl::fromLocalFile(SRCDIR "/data/margins.qml"));

    qApp->processEvents();
    QDeclarativeRectangle* rect = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("filler"));
    QDeclarativeItemPrivate *rectPrivate = QDeclarativeItemPrivate::get(rect);
    QCOMPARE(rect->x(), 5.0);
    QCOMPARE(rect->y(), 6.0);
    QCOMPARE(rect->width(), 200.0 - 5.0 - 10.0);
    QCOMPARE(rect->height(), 200.0 - 6.0 - 10.0);

    rectPrivate->anchors()->setTopMargin(0.0);
    rectPrivate->anchors()->setMargins(20.0);

    QCOMPARE(rect->x(), 5.0);
    QCOMPARE(rect->y(), 20.0);
    QCOMPARE(rect->width(), 200.0 - 5.0 - 20.0);
    QCOMPARE(rect->height(), 200.0 - 20.0 - 20.0);

    delete view;
}

QTEST_MAIN(tst_qdeclarativeanchors)

#include "tst_qdeclarativeanchors.moc"
