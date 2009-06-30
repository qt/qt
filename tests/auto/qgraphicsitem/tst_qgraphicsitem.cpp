/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <private/qtextcontrol_p.h>
#include <private/qgraphicsitem_p.h>
#include <QAbstractTextDocumentLayout>
#include <QBitmap>
#include <QCursor>
#include <QDial>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>

//TESTED_CLASS=
//TESTED_FILES=

Q_DECLARE_METATYPE(QList<int>)
Q_DECLARE_METATYPE(QList<QRectF>)
Q_DECLARE_METATYPE(QPainterPath)
Q_DECLARE_METATYPE(QPointF)
Q_DECLARE_METATYPE(QRectF)

#if defined(Q_OS_WIN) && !defined(Q_OS_WINCE)
#include <windows.h>
#define Q_CHECK_PAINTEVENTS \
    if (::SwitchDesktop(::GetThreadDesktop(::GetCurrentThreadId())) == 0) \
        QSKIP("The Graphics View doesn't get the paint events", SkipSingle);
#else
#define Q_CHECK_PAINTEVENTS
#endif

class EventTester : public QGraphicsItem
{
public:
    EventTester(QGraphicsItem *parent = 0) : QGraphicsItem(parent), repaints(0)
    { br = QRectF(-10, -10, 20, 20); }

    void setGeometry(const QRectF &rect)
    {
        prepareGeometryChange();
        br = rect;
        update();
    }

    QRectF boundingRect() const
    { return br; }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        hints = painter->renderHints();
        painter->drawRect(boundingRect());
        ++repaints;
    }

    bool sceneEvent(QEvent *event)
    {
        events << event->type();
        return QGraphicsItem::sceneEvent(event);
    }

    QList<QEvent::Type> events;
    QPainter::RenderHints hints;
    int repaints;
    QRectF br;
};

class tst_QGraphicsItem : public QObject
{
    Q_OBJECT

public slots:
    void init();

private slots:
    void construction();
    void constructionWithParent();
    void destruction();
    void scene();
    void parentItem();
    void setParentItem();
    void children();
    void flags();
    void toolTip();
    void visible();
    void explicitlyVisible();
    void enabled();
    void explicitlyEnabled();
    void selected();
    void selected2();
    void selected_group();
    void selected_textItem();
    void selected_multi();
    void acceptedMouseButtons();
    void acceptsHoverEvents();
    void childAcceptsHoverEvents();
    void hasFocus();
    void pos();
    void scenePos();
    void matrix();
    void sceneMatrix();
    void setMatrix();
    void zValue();
    void shape();
    void contains();
    void collidesWith_item();
    void collidesWith_path_data();
    void collidesWith_path();
    void collidesWithItemWithClip();
    void isObscuredBy();
    void isObscured();
    void mapFromToParent();
    void mapFromToScene();
    void mapFromToItem();
    void mapRectFromToParent_data();
    void mapRectFromToParent();
    void isAncestorOf();
    void commonAncestorItem();
    void data();
    void type();
    void graphicsitem_cast();
    void hoverEventsGenerateRepaints();
    void boundingRects_data();
    void boundingRects();
    void boundingRects2();
    void sceneBoundingRect();
    void childrenBoundingRect();
    void childrenBoundingRect2();
    void group();
    void setGroup();
    void nestedGroups();
    void warpChildrenIntoGroup();
    void removeFromGroup();
    void handlesChildEvents();
    void handlesChildEvents2();
    void handlesChildEvents3();
    void ensureVisible();
    void cursor();
    //void textControlGetterSetter();
    void defaultItemTest_QGraphicsLineItem();
    void defaultItemTest_QGraphicsPixmapItem();
    void defaultItemTest_QGraphicsTextItem();
    void defaultItemTest_QGraphicsEllipseItem();
    void itemChange();
    void sceneEventFilter();
    void prepareGeometryChange();
    void paint();
    void deleteItemInEventHandlers();
    void itemClipsToShape();
    void itemClipsChildrenToShape();
    void itemClipsChildrenToShape2();
    void itemClipsTextChildToShape();
    void itemClippingDiscovery();
    void ancestorFlags();
    void untransformable();
    void contextMenuEventPropagation();
    void itemIsMovable();
    void boundingRegion_data();
    void boundingRegion();
    void itemTransform_parentChild();
    void itemTransform_siblings();
    void itemTransform_unrelated();
    void opacity_data();
    void opacity();
    void opacity2();
    void opacityZeroUpdates();
    void itemStacksBehindParent();
    void nestedClipping();
    void nestedClippingTransforms();
    void sceneTransformCache();
    void tabChangesFocus();
    void tabChangesFocus_data();
    void cacheMode();
    void updateCachedItemAfterMove();
    void deviceTransform_data();
    void deviceTransform();

    // task specific tests below me
    void task141694_textItemEnsureVisible();
    void task128696_textItemEnsureMovable();
    void ensureUpdateOnTextItem();
    void task177918_lineItemUndetected();
    void task240400_clickOnTextItem_data();
    void task240400_clickOnTextItem();
    void task243707_addChildBeforeParent();
    void task197802_childrenVisibility();
};

void tst_QGraphicsItem::init()
{
#ifdef Q_OS_WINCE //disable magic for WindowsCE
    qApp->setAutoMaximizeThreshold(-1);
#endif
}

void tst_QGraphicsItem::construction()
{
    for (int i = 0; i < 7; ++i) {
        QGraphicsItem *item;
        switch (i) {
        case 0:
            item = new QGraphicsEllipseItem;
            QCOMPARE(int(item->type()), int(QGraphicsEllipseItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsEllipseItem *>(item), (QGraphicsEllipseItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        case 1:
            item = new QGraphicsLineItem;
            QCOMPARE(int(item->type()), int(QGraphicsLineItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsLineItem *>(item), (QGraphicsLineItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        case 2:
            item = new QGraphicsPathItem;
            QCOMPARE(int(item->type()), int(QGraphicsPathItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsPathItem *>(item), (QGraphicsPathItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        case 3:
            item = new QGraphicsPixmapItem;
            QCOMPARE(int(item->type()), int(QGraphicsPixmapItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsPixmapItem *>(item), (QGraphicsPixmapItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        case 4:
            item = new QGraphicsPolygonItem;
            QCOMPARE(int(item->type()), int(QGraphicsPolygonItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsPolygonItem *>(item), (QGraphicsPolygonItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        case 5:
            item = new QGraphicsRectItem;
            QCOMPARE(int(item->type()), int(QGraphicsRectItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsLineItem *>(item), (QGraphicsLineItem *)0);
            break;
        case 6:
        default:
            item = new QGraphicsTextItem;
            QCOMPARE(int(item->type()), int(QGraphicsTextItem::Type));
            QCOMPARE(qgraphicsitem_cast<QGraphicsTextItem *>(item), (QGraphicsTextItem *)item);
            QCOMPARE(qgraphicsitem_cast<QGraphicsRectItem *>(item), (QGraphicsRectItem *)0);
            break;
        }

        QCOMPARE(item->scene(), (QGraphicsScene *)0);
        QCOMPARE(item->parentItem(), (QGraphicsItem *)0);
        QVERIFY(item->children().isEmpty());
        QCOMPARE(item->flags(), 0);
        QVERIFY(item->isVisible());
        QVERIFY(item->isEnabled());
        QVERIFY(!item->isSelected());
        QCOMPARE(item->acceptedMouseButtons(), Qt::MouseButtons(0x1f));
        if (item->type() == QGraphicsTextItem::Type)
            QVERIFY(item->acceptsHoverEvents());
        else
            QVERIFY(!item->acceptsHoverEvents());
        QVERIFY(!item->hasFocus());
        QCOMPARE(item->pos(), QPointF());
        QCOMPARE(item->matrix(), QMatrix());
        QCOMPARE(item->sceneMatrix(), QMatrix());
        QCOMPARE(item->zValue(), qreal(0));
        QCOMPARE(item->sceneBoundingRect(), QRectF());
        QCOMPARE(item->shape(), QPainterPath());
        QVERIFY(!item->contains(QPointF(0, 0)));
        QVERIFY(!item->collidesWithItem(0));
        QVERIFY(item->collidesWithItem(item));
        QVERIFY(!item->collidesWithPath(QPainterPath()));
        QVERIFY(!item->isAncestorOf(0));
        QVERIFY(!item->isAncestorOf(item));
        QCOMPARE(item->data(0), QVariant());
        delete item;
    }
}

class BoundingRectItem : public QGraphicsRectItem
{
public:
    BoundingRectItem(QGraphicsItem *parent = 0)
        : QGraphicsRectItem(0, 0, parent ? 200 : 100, parent ? 200 : 100,
                            parent)
    {}

    QRectF boundingRect() const
    {
        QRectF tmp = QGraphicsRectItem::boundingRect();
        foreach (QGraphicsItem *child, children())
            tmp |= child->boundingRect(); // <- might be pure virtual
        return tmp;
    }
};

void tst_QGraphicsItem::constructionWithParent()
{
    // This test causes a crash if item1 calls item2's pure virtuals before the
    // object has been constructed.
    QGraphicsItem *item0 = new BoundingRectItem;
    QGraphicsItem *item1 = new BoundingRectItem;
    QGraphicsScene scene;
    scene.addItem(item0);
    scene.addItem(item1);
    QGraphicsItem *item2 = new BoundingRectItem(item1);
    QCOMPARE(item1->children(), QList<QGraphicsItem *>() << item2);
    QCOMPARE(item1->boundingRect(), QRectF(0, 0, 200, 200));

    item2->setParentItem(item0);
    QCOMPARE(item0->children(), QList<QGraphicsItem *>() << item2);
    QCOMPARE(item0->boundingRect(), QRectF(0, 0, 200, 200));
}

static int itemDeleted = 0;
class Item : public QGraphicsRectItem
{
public:
    ~Item()
    { ++itemDeleted; }
};

void tst_QGraphicsItem::destruction()
{
    QCOMPARE(itemDeleted, 0);
    {
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        QCOMPARE(child->parentItem(), parent);
        delete parent;
        QCOMPARE(itemDeleted, 1);
    }
    {
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        QCOMPARE(parent->children().size(), 1);
        delete child;
        QCOMPARE(parent->children().size(), 0);
        delete parent;
        QCOMPARE(itemDeleted, 2);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        QCOMPARE(child->parentItem(), (QGraphicsItem *)0);
        child->setParentItem(parent);
        QCOMPARE(child->parentItem(), parent);
        scene.addItem(parent);
        QCOMPARE(child->parentItem(), parent);
        delete parent;
        QCOMPARE(itemDeleted, 3);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        QCOMPARE(parent->children().size(), 1);
        delete child;
        QCOMPARE(parent->children().size(), 0);
        delete parent;
        QCOMPARE(itemDeleted, 4);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        scene.removeItem(parent);
        QCOMPARE(child->scene(), (QGraphicsScene *)0);
        delete parent;
        QCOMPARE(itemDeleted, 5);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        QCOMPARE(child->scene(), (QGraphicsScene *)0);
        QCOMPARE(parent->scene(), (QGraphicsScene *)0);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        scene.removeItem(child);
        QCOMPARE(child->scene(), (QGraphicsScene *)0);
        QCOMPARE(parent->scene(), &scene);
        QCOMPARE(child->parentItem(), (QGraphicsItem *)0);
        QVERIFY(parent->children().isEmpty());
        delete parent;
        QCOMPARE(itemDeleted, 5);
        delete child;
        QCOMPARE(itemDeleted, 6);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        scene.removeItem(child);
        scene.removeItem(parent);
        delete child;
        delete parent;
        QCOMPARE(itemDeleted, 7);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        QGraphicsScene scene2;
        scene2.addItem(parent);
        delete parent;
        QCOMPARE(itemDeleted, 8);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        QGraphicsScene scene2;
        scene2.addItem(parent);
        QCOMPARE(child->scene(), &scene2);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        scene2.addItem(parent);
        QCOMPARE(child->scene(), &scene2);
        delete parent;
        QCOMPARE(itemDeleted, 9);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *parent = new QGraphicsRectItem;
        Item *child = new Item;
        child->setParentItem(parent);
        scene.addItem(parent);
        QCOMPARE(child->scene(), &scene);
        QGraphicsScene scene2;
        scene2.addItem(child);
        QCOMPARE(child->scene(), &scene2);
        delete parent;
        QCOMPARE(itemDeleted, 9);
        delete child;
        QCOMPARE(itemDeleted, 10);
    }
    {
        QGraphicsScene scene;
        QGraphicsItem *root = new QGraphicsRectItem;
        QGraphicsItem *parent = root;
        QGraphicsItem *middleItem = 0;
        for (int i = 0; i < 99; ++i) {
            Item *child = new Item;
            child->setParentItem(parent);
            parent = child;
            if (i == 50)
                middleItem = parent;
        }
        scene.addItem(root);

        QCOMPARE(scene.items().size(), 100);

        QGraphicsScene scene2;
        scene2.addItem(middleItem);

        delete middleItem;
        QCOMPARE(itemDeleted, 59);
    }
    QCOMPARE(itemDeleted, 109);
}

void tst_QGraphicsItem::scene()
{
    QGraphicsRectItem *item = new QGraphicsRectItem;
    QCOMPARE(item->scene(), (QGraphicsScene *)0);

    QGraphicsScene scene;
    scene.addItem(item);
    QCOMPARE(item->scene(), (QGraphicsScene *)&scene);

    QGraphicsScene scene2;
    scene2.addItem(item);
    QCOMPARE(item->scene(), (QGraphicsScene *)&scene2);

    scene2.removeItem(item);
    QCOMPARE(item->scene(), (QGraphicsScene *)0);

    delete item;
}

void tst_QGraphicsItem::parentItem()
{
    QGraphicsRectItem item;
    QCOMPARE(item.parentItem(), (QGraphicsItem *)0);

    QGraphicsRectItem *item2 = new QGraphicsRectItem(QRectF(), &item);
    QCOMPARE(item2->parentItem(), (QGraphicsItem *)&item);
    item2->setParentItem(&item);
    QCOMPARE(item2->parentItem(), (QGraphicsItem *)&item);
    item2->setParentItem(0);
    QCOMPARE(item2->parentItem(), (QGraphicsItem *)0);

    delete item2;
}

void tst_QGraphicsItem::setParentItem()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 10, 10));
    QCOMPARE(item->scene(), &scene);

    QGraphicsRectItem *child = new QGraphicsRectItem;
    QCOMPARE(child->scene(), (QGraphicsScene *)0);

    // This implicitly adds the item to the parent's scene
    child->setParentItem(item);
    QCOMPARE(child->scene(), &scene);

    // This just makes it a toplevel
    child->setParentItem(0);
    QCOMPARE(child->scene(), &scene);

    // Add the child back to the parent, then remove the parent from the scene
    child->setParentItem(item);
    scene.removeItem(item);
    QCOMPARE(child->scene(), (QGraphicsScene *)0);
}

void tst_QGraphicsItem::children()
{
    QGraphicsRectItem item;
    QVERIFY(item.children().isEmpty());

    QGraphicsRectItem *item2 = new QGraphicsRectItem(QRectF(), &item);
    QCOMPARE(item.children().size(), 1);
    QCOMPARE(item.children().first(), (QGraphicsItem *)item2);
    QVERIFY(item2->children().isEmpty());

    delete item2;
    QVERIFY(item.children().isEmpty());
}

void tst_QGraphicsItem::flags()
{
    QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(-10, -10, 20, 20));
    QCOMPARE(item->flags(), 0);

    QGraphicsScene scene;
    scene.addItem(item);

    {
        // Focus
        item->setFlag(QGraphicsItem::ItemIsFocusable, false);
        QVERIFY(!item->hasFocus());
        item->setFocus();
        QVERIFY(!item->hasFocus());

        item->setFlag(QGraphicsItem::ItemIsFocusable, true);
        QVERIFY(!item->hasFocus());
        item->setFocus();
        QVERIFY(item->hasFocus());
        QVERIFY(scene.hasFocus());

        item->setFlag(QGraphicsItem::ItemIsFocusable, false);
        QVERIFY(!item->hasFocus());
        QVERIFY(scene.hasFocus());
    }
    {
        // Selectable
        item->setFlag(QGraphicsItem::ItemIsSelectable, false);
        QVERIFY(!item->isSelected());
        item->setSelected(true);
        QVERIFY(!item->isSelected());

        item->setFlag(QGraphicsItem::ItemIsSelectable, true);
        QVERIFY(!item->isSelected());
        item->setSelected(true);
        QVERIFY(item->isSelected());
        item->setFlag(QGraphicsItem::ItemIsSelectable, false);
        QVERIFY(!item->isSelected());
    }
    {
        // Movable
        item->setFlag(QGraphicsItem::ItemIsMovable, false);
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setScenePos(QPointF(0, 0));
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event);
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0); // mouse grabber is reset

        QGraphicsSceneMouseEvent event2(QEvent::GraphicsSceneMouseMove);
        event2.setScenePos(QPointF(10, 10));
        event2.setButton(Qt::LeftButton);
        event2.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event2);
        QCOMPARE(item->pos(), QPointF());

        QGraphicsSceneMouseEvent event3(QEvent::GraphicsSceneMouseRelease);
        event3.setScenePos(QPointF(10, 10));
        event3.setButtons(0);
        QApplication::sendEvent(&scene, &event3);
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);

        item->setFlag(QGraphicsItem::ItemIsMovable, true);
        QGraphicsSceneMouseEvent event4(QEvent::GraphicsSceneMousePress);
        event4.setScenePos(QPointF(0, 0));
        event4.setButton(Qt::LeftButton);
        event4.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event4);
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
        QGraphicsSceneMouseEvent event5(QEvent::GraphicsSceneMouseMove);
        event5.setScenePos(QPointF(10, 10));
        event5.setButton(Qt::LeftButton);
        event5.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event5);
        QCOMPARE(item->pos(), QPointF(10, 10));
    }
}

void tst_QGraphicsItem::toolTip()
{
    QString toolTip = "Qt rocks!";

    QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(0, 0, 100, 100));
    item->setPen(QPen(Qt::red, 1));
    item->setBrush(QBrush(Qt::blue));
    QVERIFY(item->toolTip().isEmpty());
    item->setToolTip(toolTip);
    QCOMPARE(item->toolTip(), toolTip);

    QGraphicsScene scene;
    scene.addItem(item);

    QGraphicsView view(&scene);
    view.setFixedSize(200, 200);
    view.show();
    QTest::qWait(250);
    {
        QHelpEvent helpEvent(QEvent::ToolTip, view.viewport()->rect().topLeft(),
                             view.viewport()->mapToGlobal(view.viewport()->rect().topLeft()));
        QApplication::sendEvent(view.viewport(), &helpEvent);
        QTest::qWait(250);

        bool foundView = false;
        bool foundTipLabel = false;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            if (widget == &view)
                foundView = true;
            if (widget->inherits("QTipLabel"))
                foundTipLabel = true;
        }
        QVERIFY(foundView);
        QVERIFY(!foundTipLabel);
    }

    {
        QHelpEvent helpEvent(QEvent::ToolTip, view.viewport()->rect().center(),
                             view.viewport()->mapToGlobal(view.viewport()->rect().center()));
        QApplication::sendEvent(view.viewport(), &helpEvent);
        QTest::qWait(250);

        bool foundView = false;
        bool foundTipLabel = false;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            if (widget == &view)
                foundView = true;
            if (widget->inherits("QTipLabel"))
                foundTipLabel = true;
        }
        QVERIFY(foundView);
        QVERIFY(foundTipLabel);
    }

    {
        QHelpEvent helpEvent(QEvent::ToolTip, view.viewport()->rect().topLeft(),
                             view.viewport()->mapToGlobal(view.viewport()->rect().topLeft()));
        QApplication::sendEvent(view.viewport(), &helpEvent);
        QTest::qWait(1000);

        bool foundView = false;
        bool foundTipLabel = false;
        foreach (QWidget *widget, QApplication::topLevelWidgets()) {
            if (widget == &view)
                foundView = true;
            if (widget->inherits("QTipLabel") && widget->isVisible())
                foundTipLabel = true;
        }
        QVERIFY(foundView);
        QVERIFY(!foundTipLabel);
    }
}

void tst_QGraphicsItem::visible()
{
    QGraphicsItem *item = new QGraphicsRectItem(QRectF(-10, -10, 20, 20));
    item->setFlag(QGraphicsItem::ItemIsMovable);
    QVERIFY(item->isVisible());
    item->setVisible(false);
    QVERIFY(!item->isVisible());
    item->setVisible(true);
    QVERIFY(item->isVisible());

    QGraphicsScene scene;
    scene.addItem(item);
    QVERIFY(item->isVisible());
    QCOMPARE(scene.itemAt(0, 0), item);
    item->setVisible(false);
    QCOMPARE(scene.itemAt(0, 0), (QGraphicsItem *)0);
    item->setVisible(true);
    QCOMPARE(scene.itemAt(0, 0), item);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setButton(Qt::LeftButton);
    event.setScenePos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(scene.mouseGrabberItem(), item);
    item->setVisible(false);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    item->setVisible(true);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);

    item->setFlag(QGraphicsItem::ItemIsFocusable);
    item->setFocus();
    QVERIFY(item->hasFocus());
    item->setVisible(false);
    QVERIFY(!item->hasFocus());
    item->setVisible(true);
    QVERIFY(!item->hasFocus());
}

void tst_QGraphicsItem::explicitlyVisible()
{
    QGraphicsScene scene;
    QGraphicsItem *parent = scene.addRect(QRectF(0, 0, 100, 100));
    QGraphicsItem *child = scene.addRect(QRectF(25, 25, 50, 50));
    child->setParentItem(parent);

    QVERIFY(parent->isVisible());
    QVERIFY(child->isVisible());

    parent->hide();

    QVERIFY(!parent->isVisible());
    QVERIFY(!child->isVisible());

    parent->show();
    child->hide();

    QVERIFY(parent->isVisible());
    QVERIFY(!child->isVisible());

    parent->hide();

    QVERIFY(!parent->isVisible());
    QVERIFY(!child->isVisible());

    parent->show();

    QVERIFY(parent->isVisible());
    QVERIFY(!child->isVisible()); // <- explicitly hidden

    child->show();

    QVERIFY(child->isVisible());

    parent->hide();

    QVERIFY(!parent->isVisible());
    QVERIFY(!child->isVisible()); // <- explicit show doesn't work

    parent->show();

    QVERIFY(parent->isVisible());
    QVERIFY(child->isVisible()); // <- no longer explicitly hidden

    // ------------------- Reparenting ------------------------------

    QGraphicsItem *parent2 = scene.addRect(-50, -50, 200, 200);
    QVERIFY(parent2->isVisible());

    // Reparent implicitly hidden item to a visible parent.
    parent->hide();
    QVERIFY(!parent->isVisible());
    QVERIFY(!child->isVisible());
    child->setParentItem(parent2);
    QVERIFY(parent2->isVisible());
    QVERIFY(child->isVisible());

    // Reparent implicitly hidden item to a hidden parent.
    child->setParentItem(parent);
    parent2->hide();
    child->setParentItem(parent2);
    QVERIFY(!parent2->isVisible());
    QVERIFY(!child->isVisible());

    // Reparent explicitly hidden item to a visible parent.
    child->hide();
    parent->show();
    child->setParentItem(parent);
    QVERIFY(parent->isVisible());
    QVERIFY(!child->isVisible());

    // Reparent explicitly hidden item to a hidden parent.
    child->setParentItem(parent2);
    QVERIFY(!parent2->isVisible());
    QVERIFY(!child->isVisible());

    // Reparent explicitly hidden item to a visible parent.
    parent->show();
    child->setParentItem(parent);
    QVERIFY(parent->isVisible());
    QVERIFY(!child->isVisible());

    // Reparent visible item to a hidden parent.
    child->show();
    parent2->hide();
    child->setParentItem(parent2);
    QVERIFY(!parent2->isVisible());
    QVERIFY(!child->isVisible());
    parent2->show();
    QVERIFY(parent2->isVisible());
    QVERIFY(child->isVisible());

    // Reparent implicitly hidden child to root.
    parent2->hide();
    QVERIFY(!child->isVisible());
    child->setParentItem(0);
    QVERIFY(child->isVisible());

    // Reparent an explicitly hidden child to root.
    child->hide();
    child->setParentItem(parent2);
    parent2->show();
    QVERIFY(!child->isVisible());
    child->setParentItem(0);
    QVERIFY(!child->isVisible());
}

void tst_QGraphicsItem::enabled()
{
    QGraphicsRectItem *item = new QGraphicsRectItem(QRectF(-10, -10, 20, 20));
    item->setFlag(QGraphicsItem::ItemIsMovable);
    QVERIFY(item->isEnabled());
    item->setEnabled(false);
    QVERIFY(!item->isEnabled());
    item->setEnabled(true);
    QVERIFY(item->isEnabled());
    item->setEnabled(false);
    item->setFlag(QGraphicsItem::ItemIsFocusable);
    QGraphicsScene scene;
    scene.addItem(item);
    item->setFocus();
    QVERIFY(!item->hasFocus());
    item->setEnabled(true);
    item->setFocus();
    QVERIFY(item->hasFocus());
    item->setEnabled(false);
    QVERIFY(!item->hasFocus());

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setButton(Qt::LeftButton);
    event.setScenePos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    item->setEnabled(true);
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
    item->setEnabled(false);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
}

void tst_QGraphicsItem::explicitlyEnabled()
{
    QGraphicsScene scene;
    QGraphicsItem *parent = scene.addRect(QRectF(0, 0, 100, 100));
    QGraphicsItem *child = scene.addRect(QRectF(25, 25, 50, 50));
    child->setParentItem(parent);

    QVERIFY(parent->isEnabled());
    QVERIFY(child->isEnabled());

    parent->setEnabled(false);

    QVERIFY(!parent->isEnabled());
    QVERIFY(!child->isEnabled());

    parent->setEnabled(true);
    child->setEnabled(false);

    QVERIFY(parent->isEnabled());
    QVERIFY(!child->isEnabled());

    parent->setEnabled(false);

    QVERIFY(!parent->isEnabled());
    QVERIFY(!child->isEnabled());

    parent->setEnabled(true);

    QVERIFY(parent->isEnabled());
    QVERIFY(!child->isEnabled()); // <- explicitly disabled

    child->setEnabled(true);

    QVERIFY(child->isEnabled());

    parent->setEnabled(false);

    QVERIFY(!parent->isEnabled());
    QVERIFY(!child->isEnabled()); // <- explicit enabled doesn't work

    parent->setEnabled(true);

    QVERIFY(parent->isEnabled());
    QVERIFY(child->isEnabled()); // <- no longer explicitly disabled

    // ------------------- Reparenting ------------------------------

    QGraphicsItem *parent2 = scene.addRect(-50, -50, 200, 200);
    QVERIFY(parent2->isEnabled());

    // Reparent implicitly hidden item to a enabled parent.
    parent->setEnabled(false);
    QVERIFY(!parent->isEnabled());
    QVERIFY(!child->isEnabled());
    child->setParentItem(parent2);
    QVERIFY(parent2->isEnabled());
    QVERIFY(child->isEnabled());

    // Reparent implicitly hidden item to a hidden parent.
    child->setParentItem(parent);
    parent2->setEnabled(false);
    child->setParentItem(parent2);
    QVERIFY(!parent2->isEnabled());
    QVERIFY(!child->isEnabled());

    // Reparent explicitly hidden item to a enabled parent.
    child->setEnabled(false);
    parent->setEnabled(true);
    child->setParentItem(parent);
    QVERIFY(parent->isEnabled());
    QVERIFY(!child->isEnabled());

    // Reparent explicitly hidden item to a hidden parent.
    child->setParentItem(parent2);
    QVERIFY(!parent2->isEnabled());
    QVERIFY(!child->isEnabled());

    // Reparent explicitly hidden item to a enabled parent.
    parent->setEnabled(true);
    child->setParentItem(parent);
    QVERIFY(parent->isEnabled());
    QVERIFY(!child->isEnabled());

    // Reparent enabled item to a hidden parent.
    child->setEnabled(true);
    parent2->setEnabled(false);
    child->setParentItem(parent2);
    QVERIFY(!parent2->isEnabled());
    QVERIFY(!child->isEnabled());
    parent2->setEnabled(true);
    QVERIFY(parent2->isEnabled());
    QVERIFY(child->isEnabled());

    // Reparent implicitly hidden child to root.
    parent2->setEnabled(false);
    QVERIFY(!child->isEnabled());
    child->setParentItem(0);
    QVERIFY(child->isEnabled());

    // Reparent an explicitly hidden child to root.
    child->setEnabled(false);
    child->setParentItem(parent2);
    parent2->setEnabled(true);
    QVERIFY(!child->isEnabled());
    child->setParentItem(0);
    QVERIFY(!child->isEnabled());
}

class SelectChangeItem : public QGraphicsRectItem
{
public:
    SelectChangeItem() : QGraphicsRectItem(-50, -50, 100, 100) { setBrush(Qt::blue); }
    QList<bool> values;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        if (change == ItemSelectedChange)
            values << value.toBool();
        return QGraphicsRectItem::itemChange(change, value);
    }
};

void tst_QGraphicsItem::selected()
{
    SelectChangeItem *item = new SelectChangeItem;
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    QVERIFY(!item->isSelected());
    QVERIFY(item->values.isEmpty());
    item->setSelected(true);
    QCOMPARE(item->values.size(), 1);
    QCOMPARE(item->values.last(), true);
    QVERIFY(item->isSelected());
    item->setSelected(false);
    QCOMPARE(item->values.size(), 2);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());
    item->setSelected(true);
    QCOMPARE(item->values.size(), 3);
    item->setEnabled(false);
    QCOMPARE(item->values.size(), 4);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());
    item->setEnabled(true);
    QCOMPARE(item->values.size(), 4);
    item->setSelected(true);
    QCOMPARE(item->values.size(), 5);
    QCOMPARE(item->values.last(), true);
    QVERIFY(item->isSelected());
    item->setVisible(false);
    QCOMPARE(item->values.size(), 6);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());
    item->setVisible(true);
    QCOMPARE(item->values.size(), 6);
    item->setSelected(true);
    QCOMPARE(item->values.size(), 7);
    QCOMPARE(item->values.last(), true);
    QVERIFY(item->isSelected());

    QGraphicsScene scene(-100, -100, 200, 200);
    scene.addItem(item);
    QCOMPARE(scene.selectedItems(), QList<QGraphicsItem *>() << item);
    item->setSelected(false);
    QVERIFY(scene.selectedItems().isEmpty());
    item->setSelected(true);
    QCOMPARE(scene.selectedItems(), QList<QGraphicsItem *>() << item);
    item->setSelected(false);
    QVERIFY(scene.selectedItems().isEmpty());

    // Interactive selection
    QGraphicsView view(&scene);
    view.setFixedSize(250, 250);
    view.show();

    qApp->processEvents();
    qApp->processEvents();

    scene.clearSelection();
    QCOMPARE(item->values.size(), 10);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());

    // Click inside and check that it's selected
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item->scenePos()));
    QCOMPARE(item->values.size(), 11);
    QCOMPARE(item->values.last(), true);
    QVERIFY(item->isSelected());

    // Click outside and check that it's not selected
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item->scenePos() + QPointF(item->boundingRect().width(), item->boundingRect().height())));

    QCOMPARE(item->values.size(), 12);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());

    SelectChangeItem *item2 = new SelectChangeItem;
    item2->setFlag(QGraphicsItem::ItemIsSelectable);
    item2->setPos(100, 0);
    scene.addItem(item2);

    // Click inside and check that it's selected
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item->scenePos()));
    QCOMPARE(item->values.size(), 13);
    QCOMPARE(item->values.last(), true);
    QVERIFY(item->isSelected());

    // Click inside item2 and check that it's selected, and item is not
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item2->scenePos()));
    QCOMPARE(item->values.size(), 14);
    QCOMPARE(item->values.last(), false);
    QVERIFY(!item->isSelected());
    QCOMPARE(item2->values.size(), 1);
    QCOMPARE(item2->values.last(), true);
    QVERIFY(item2->isSelected());
}

void tst_QGraphicsItem::selected2()
{
    // Selecting an item, then moving another previously caused a crash.
    QGraphicsScene scene;
    QGraphicsItem *line1 = scene.addRect(QRectF(0, 0, 100, 100));
    line1->setPos(-105, 0);
    line1->setFlag(QGraphicsItem::ItemIsSelectable);

    QGraphicsItem *line2 = scene.addRect(QRectF(0, 0, 100, 100));
    line2->setFlag(QGraphicsItem::ItemIsMovable);

    line1->setSelected(true);

    {
        QGraphicsSceneMouseEvent mousePress(QEvent::GraphicsSceneMousePress);
        mousePress.setScenePos(QPointF(50, 50));
        mousePress.setButton(Qt::LeftButton);
        QApplication::sendEvent(&scene, &mousePress);
        QVERIFY(mousePress.isAccepted());
    }
    {
        QGraphicsSceneMouseEvent mouseMove(QEvent::GraphicsSceneMouseMove);
        mouseMove.setScenePos(QPointF(60, 60));
        mouseMove.setButton(Qt::LeftButton);
        mouseMove.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &mouseMove);
        QVERIFY(mouseMove.isAccepted());
    }
}

void tst_QGraphicsItem::selected_group()
{
    QGraphicsScene scene;
    QGraphicsItem *item1 = scene.addRect(QRectF());
    QGraphicsItem *item2 = scene.addRect(QRectF());
    item1->setFlag(QGraphicsItem::ItemIsSelectable);
    item2->setFlag(QGraphicsItem::ItemIsSelectable);
    scene.addRect(QRectF())->setParentItem(item1);
    QGraphicsItem *leaf = scene.addRect(QRectF());
    leaf->setFlag(QGraphicsItem::ItemIsSelectable);
    leaf->setParentItem(item2);

    QGraphicsItemGroup *group = scene.createItemGroup(QList<QGraphicsItem *>() << item1 << item2);
    QCOMPARE(group->scene(), &scene);
    group->setFlag(QGraphicsItem::ItemIsSelectable);
    foreach (QGraphicsItem *item, scene.items()) {
        if (item == group)
            QVERIFY(!item->group());
        else
            QCOMPARE(item->group(), group);
    }

    QVERIFY(group->handlesChildEvents());
    QVERIFY(!group->isSelected());
    group->setSelected(false);
    QVERIFY(!group->isSelected());
    group->setSelected(true);
    QVERIFY(group->isSelected());
    foreach (QGraphicsItem *item, scene.items())
        QVERIFY(item->isSelected());
    group->setSelected(false);
    QVERIFY(!group->isSelected());
    foreach (QGraphicsItem *item, scene.items())
        QVERIFY(!item->isSelected());
    leaf->setSelected(true);
    foreach (QGraphicsItem *item, scene.items())
        QVERIFY(item->isSelected());
    leaf->setSelected(false);
    foreach (QGraphicsItem *item, scene.items())
        QVERIFY(!item->isSelected());

    leaf->setSelected(true);
    QGraphicsScene scene2;
    scene2.addItem(item1);
    QVERIFY(!item1->isSelected());
    QVERIFY(item2->isSelected());
}

void tst_QGraphicsItem::selected_textItem()
{
    QGraphicsScene scene;
    QGraphicsTextItem *text = scene.addText(QLatin1String("Text"));
    text->setFlag(QGraphicsItem::ItemIsSelectable);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWait(1000);

    QVERIFY(!text->isSelected());
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0,
                      view.mapFromScene(text->mapToScene(0, 0)));
    QVERIFY(text->isSelected());

    text->setSelected(false);
    text->setTextInteractionFlags(Qt::TextEditorInteraction);

    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0,
                      view.mapFromScene(text->mapToScene(0, 0)));
    QVERIFY(text->isSelected());
}

void tst_QGraphicsItem::selected_multi()
{
    // Test multiselection behavior
    QGraphicsScene scene;

    // Create two disjoint items
    QGraphicsItem *item1 = scene.addRect(QRectF(-10, -10, 20, 20));
    QGraphicsItem *item2 = scene.addRect(QRectF(-10, -10, 20, 20));
    item1->setPos(-15, 0);
    item2->setPos(15, 20);

    // Make both items selectable
    item1->setFlag(QGraphicsItem::ItemIsSelectable);
    item2->setFlag(QGraphicsItem::ItemIsSelectable);

    // Create and show a view
    QGraphicsView view(&scene);
    view.show();
    view.fitInView(scene.sceneRect());
    qApp->processEvents();

    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Start clicking
    QTest::qWait(200);

    // Click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Click on item2
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item2->scenePos()));
    QTest::qWait(200);
    QVERIFY(item2->isSelected());
    QVERIFY(!item1->isSelected());

    // Ctrl-click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item2->isSelected());
    QVERIFY(item1->isSelected());

    // Ctrl-click on item1 again
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item2->isSelected());
    QVERIFY(!item1->isSelected());

    // Ctrl-click on item2
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item2->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item2->isSelected());
    QVERIFY(!item1->isSelected());

    // Click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Click on scene
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(0, 0));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Ctrl-click on scene
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(0, 0));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Press on item2
    QTest::mousePress(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item2->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(item2->isSelected());

    // Release on item2
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item2->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(item2->isSelected());

    // Click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Ctrl-click on item1
    QTest::mouseClick(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    // Ctrl-press on item1
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    {
        // Ctrl-move on item1
        QMouseEvent event(QEvent::MouseMove, view.mapFromScene(item1->scenePos()) + QPoint(1, 0), Qt::LeftButton, Qt::LeftButton, Qt::ControlModifier);
        QApplication::sendEvent(view.viewport(), &event);
        QTest::qWait(200);
        QVERIFY(!item1->isSelected());
        QVERIFY(!item2->isSelected());
    }

    // Release on item1
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());

    item1->setFlag(QGraphicsItem::ItemIsMovable);
    item1->setSelected(false);

    // Ctrl-press on item1
    QTest::mousePress(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(!item1->isSelected());
    QVERIFY(!item2->isSelected());

    {
        // Ctrl-move on item1
        QMouseEvent event(QEvent::MouseMove, view.mapFromScene(item1->scenePos()) + QPoint(1, 0), Qt::LeftButton, Qt::LeftButton, Qt::ControlModifier);
        QApplication::sendEvent(view.viewport(), &event);
        QTest::qWait(200);
        QVERIFY(item1->isSelected());
        QVERIFY(!item2->isSelected());
    }

    // Release on item1
    QTest::mouseRelease(view.viewport(), Qt::LeftButton, Qt::ControlModifier, view.mapFromScene(item1->scenePos()));
    QTest::qWait(200);
    QVERIFY(item1->isSelected());
    QVERIFY(!item2->isSelected());
}

void tst_QGraphicsItem::acceptedMouseButtons()
{
    QGraphicsScene scene;
    QGraphicsRectItem *item1 = scene.addRect(QRectF(-10, -10, 20, 20));
    QGraphicsRectItem *item2 = scene.addRect(QRectF(-10, -10, 20, 20));
    item2->setZValue(1);

    item1->setFlag(QGraphicsItem::ItemIsMovable);
    item2->setFlag(QGraphicsItem::ItemIsMovable);

    QCOMPARE(item1->acceptedMouseButtons(), Qt::MouseButtons(0x1f));
    QCOMPARE(item2->acceptedMouseButtons(), Qt::MouseButtons(0x1f));

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.setButton(Qt::LeftButton);
    event.setScenePos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item2);
    item2->setAcceptedMouseButtons(0);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item1);
}

class HoverItem : public QGraphicsRectItem
{
public:
    HoverItem(const QRectF &rect)
        : QGraphicsRectItem(rect), hoverInCount(0),
          hoverMoveCount(0), hoverOutCount(0)
    { }

    int hoverInCount;
    int hoverMoveCount;
    int hoverOutCount;
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *)
    { ++hoverInCount; }

    void hoverMoveEvent(QGraphicsSceneHoverEvent *)
    { ++hoverMoveCount; }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *)
    { ++hoverOutCount; }
};

void tst_QGraphicsItem::acceptsHoverEvents()
{
    QGraphicsScene scene;
    HoverItem *item1 = new HoverItem(QRectF(-10, -10, 20, 20));
    HoverItem *item2 = new HoverItem(QRectF(-5, -5, 10, 10));
    scene.addItem(item1);
    scene.addItem(item2);
    item2->setZValue(1);

    QVERIFY(!item1->acceptsHoverEvents());
    QVERIFY(!item2->acceptsHoverEvents());
    item1->setAcceptsHoverEvents(true);
    item2->setAcceptsHoverEvents(true);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
    event.setScenePos(QPointF(-100, -100));
    QApplication::sendEvent(&scene, &event);
    event.setScenePos(QPointF(-2.5, -2.5));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item2->hoverInCount, 1);

    item1->setAcceptsHoverEvents(false);
    item2->setAcceptsHoverEvents(false);

    event.setScenePos(QPointF(-100, -100));
    QApplication::sendEvent(&scene, &event);
    event.setScenePos(QPointF(-2.5, -2.5));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item2->hoverInCount, 1);

    item1->setAcceptsHoverEvents(true);
    item2->setAcceptsHoverEvents(false);

    event.setScenePos(QPointF(-100, -100));
    QApplication::sendEvent(&scene, &event);
    event.setScenePos(QPointF(-2.5, -2.5));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item1->hoverInCount, 1);
    QCOMPARE(item2->hoverInCount, 1);
}

void tst_QGraphicsItem::childAcceptsHoverEvents()
{
    QGraphicsScene scene;
    HoverItem *item1 = new HoverItem(QRectF(-10, -10, 20, 20));
    HoverItem *item2 = new HoverItem(QRectF(-5, -5, 10, 10));

    scene.addItem(item1);
    scene.addItem(item2);
    item2->setParentItem(item1);
    item2->setAcceptHoverEvents(true);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
    event.setScenePos(QPointF(-100, -100));
    QApplication::sendEvent(&scene, &event);
    QCOMPARE(item2->hoverInCount, 0);
    QCOMPARE(item2->hoverMoveCount, 0);
    QCOMPARE(item2->hoverOutCount, 0);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);

    event.setScenePos(QPointF(-2.5, -2.5));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item2->hoverInCount, 1);
    QCOMPARE(item2->hoverMoveCount, 1);
    QCOMPARE(item2->hoverOutCount, 0);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);

    event.setScenePos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item2->hoverInCount, 1);
    QCOMPARE(item2->hoverMoveCount, 2);
    QCOMPARE(item2->hoverOutCount, 0);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);

    event.setScenePos(QPointF(-7, -7));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item2->hoverInCount, 1);
    QCOMPARE(item2->hoverMoveCount, 2);
    QCOMPARE(item2->hoverOutCount, 1);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);

    event.setScenePos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item2->hoverInCount, 2);
    QCOMPARE(item2->hoverMoveCount, 3);
    QCOMPARE(item2->hoverOutCount, 1);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);

    HoverItem *item0 = new HoverItem(QRectF(-20, -20, 20, 20));
    scene.addItem(item0);
    item1->setParentItem(item0);
    item0->setAcceptHoverEvents(true);

    event.setScenePos(QPointF(-100, -100));
    QApplication::sendEvent(&scene, &event);

    event.setScenePos(QPointF(-15, -15));
    QApplication::sendEvent(&scene, &event);

    QCOMPARE(item2->hoverInCount, 2);
    QCOMPARE(item2->hoverMoveCount, 3);
    QCOMPARE(item2->hoverOutCount, 2);
    QCOMPARE(item1->hoverInCount, 0);
    QCOMPARE(item1->hoverMoveCount, 0);
    QCOMPARE(item1->hoverOutCount, 0);
    QCOMPARE(item0->hoverInCount, 1);
    QCOMPARE(item0->hoverMoveCount, 1);
    QCOMPARE(item0->hoverOutCount, 0);
}

void tst_QGraphicsItem::hasFocus()
{
    QGraphicsLineItem *line = new QGraphicsLineItem;
    QVERIFY(!line->hasFocus());
    line->setFocus();
    QVERIFY(!line->hasFocus());

    QGraphicsScene scene;
    scene.addItem(line);

    line->setFocus();
    QVERIFY(!line->hasFocus());
    line->setFlag(QGraphicsItem::ItemIsFocusable);
    line->setFocus();
    QVERIFY(line->hasFocus());

    QGraphicsScene scene2;
    scene2.addItem(line);
    QVERIFY(!line->hasFocus());

    QCOMPARE(scene.focusItem(), (QGraphicsItem *)0);
    QCOMPARE(scene2.focusItem(), (QGraphicsItem *)0);

    line->setFocus();
    QVERIFY(line->hasFocus());
    line->clearFocus();
    QVERIFY(!line->hasFocus());

    QGraphicsLineItem *line2 = new QGraphicsLineItem;
    line2->setFlag(QGraphicsItem::ItemIsFocusable);
    scene2.addItem(line2);

    line2->setFocus();
    QVERIFY(!line->hasFocus());
    QVERIFY(line2->hasFocus());
    line->setFocus();
    QVERIFY(line->hasFocus());
    QVERIFY(!line2->hasFocus());
}

void tst_QGraphicsItem::pos()
{
    QGraphicsItem *child = new QGraphicsLineItem;
    QGraphicsItem *parent = new QGraphicsLineItem;

    QCOMPARE(child->pos(), QPointF());
    QCOMPARE(parent->pos(), QPointF());

    child->setParentItem(parent);
    child->setPos(10, 10);

    QCOMPARE(child->pos(), QPointF(10, 10));

    parent->setPos(10, 10);

    QCOMPARE(parent->pos(), QPointF(10, 10));
    QCOMPARE(child->pos(), QPointF(10, 10));

    delete child;
    delete parent;
}

void tst_QGraphicsItem::scenePos()
{
    QGraphicsItem *child = new QGraphicsLineItem;
    QGraphicsItem *parent = new QGraphicsLineItem;

    QCOMPARE(child->scenePos(), QPointF());
    QCOMPARE(parent->scenePos(), QPointF());

    child->setParentItem(parent);
    child->setPos(10, 10);

    QCOMPARE(child->scenePos(), QPointF(10, 10));

    parent->setPos(10, 10);

    QCOMPARE(parent->scenePos(), QPointF(10, 10));
    QCOMPARE(child->scenePos(), QPointF(20, 20));

    parent->setPos(20, 20);

    QCOMPARE(parent->scenePos(), QPointF(20, 20));
    QCOMPARE(child->scenePos(), QPointF(30, 30));

    delete child;
    delete parent;
}

void tst_QGraphicsItem::matrix()
{
    QGraphicsLineItem line;
    QCOMPARE(line.matrix(), QMatrix());
    line.setMatrix(QMatrix().rotate(90));
    QCOMPARE(line.matrix(), QMatrix().rotate(90));
    line.setMatrix(QMatrix().rotate(90));
    QCOMPARE(line.matrix(), QMatrix().rotate(90));
    line.setMatrix(QMatrix().rotate(90), true);
    QCOMPARE(line.matrix(), QMatrix().rotate(180));
    line.setMatrix(QMatrix().rotate(-90), true);
    QCOMPARE(line.matrix(), QMatrix().rotate(90));
    line.resetMatrix();
    QCOMPARE(line.matrix(), QMatrix());

    line.rotate(90);
    QCOMPARE(line.matrix(), QMatrix().rotate(90));
    line.rotate(90);
    QCOMPARE(line.matrix(), QMatrix().rotate(90).rotate(90));
    line.resetMatrix();

    line.scale(2, 4);
    QCOMPARE(line.matrix(), QMatrix().scale(2, 4));
    line.scale(2, 4);
    QCOMPARE(line.matrix(), QMatrix().scale(2, 4).scale(2, 4));
    line.resetMatrix();

    line.shear(2, 4);
    QCOMPARE(line.matrix(), QMatrix().shear(2, 4));
    line.shear(2, 4);
    QCOMPARE(line.matrix(), QMatrix().shear(2, 4).shear(2, 4));
    line.resetMatrix();

    line.translate(10, 10);
    QCOMPARE(line.matrix(), QMatrix().translate(10, 10));
    line.translate(10, 10);
    QCOMPARE(line.matrix(), QMatrix().translate(10, 10).translate(10, 10));
    line.resetMatrix();
}

void tst_QGraphicsItem::sceneMatrix()
{
    QGraphicsLineItem *parent = new QGraphicsLineItem;
    QGraphicsLineItem *child = new QGraphicsLineItem(QLineF(), parent);

    QCOMPARE(parent->sceneMatrix(), QMatrix());
    QCOMPARE(child->sceneMatrix(), QMatrix());

    parent->translate(10, 10);
    QCOMPARE(parent->sceneMatrix(), QMatrix().translate(10, 10));
    QCOMPARE(child->sceneMatrix(), QMatrix().translate(10, 10));

    child->translate(10, 10);
    QCOMPARE(parent->sceneMatrix(), QMatrix().translate(10, 10));
    QCOMPARE(child->sceneMatrix(), QMatrix().translate(20, 20));

    parent->rotate(90);
    QCOMPARE(parent->sceneMatrix(), QMatrix().translate(10, 10).rotate(90));
    QCOMPARE(child->sceneMatrix(), QMatrix().translate(10, 10).rotate(90).translate(10, 10));

    delete child;
    delete parent;
}

void tst_QGraphicsItem::setMatrix()
{
    QGraphicsScene scene;
    qRegisterMetaType<QList<QRectF> >("QList<QRectF>");
    QSignalSpy spy(&scene, SIGNAL(changed(QList<QRectF>)));
    QRectF unrotatedRect(-12, -34, 56, 78);
    QGraphicsRectItem item(unrotatedRect, 0, &scene);
    scene.update(scene.sceneRect());
    QApplication::instance()->processEvents();

    QCOMPARE(spy.count(), 1);

    item.setMatrix(QMatrix().rotate(qreal(12.34)));
    QRectF rotatedRect = scene.sceneRect();
    QVERIFY(unrotatedRect != rotatedRect);
    scene.update(scene.sceneRect());
    QApplication::instance()->processEvents();

    QCOMPARE(spy.count(), 2);

    item.setMatrix(QMatrix());

    scene.update(scene.sceneRect());
    QApplication::instance()->processEvents();

    QCOMPARE(spy.count(), 3);
    QList<QRectF> rlist = qVariantValue<QList<QRectF> >(spy.last().at(0));

    QCOMPARE(rlist.size(), 3);
    QCOMPARE(rlist.at(0), rotatedRect);   // From item.setMatrix() (clearing rotated rect)
    QCOMPARE(rlist.at(1), rotatedRect);   // From scene.update()   (updating scene rect)
    QCOMPARE(rlist.at(2), unrotatedRect); // From post-update      (update current state)
}

static QList<QGraphicsItem *> paintedItems;
class PainterItem : public QGraphicsItem
{
protected:
    QRectF boundingRect() const
    { return QRectF(-10, -10, 20, 20); }

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
    { paintedItems << this; }
};

void tst_QGraphicsItem::zValue()
{
    Q_CHECK_PAINTEVENTS

    QGraphicsScene scene;

    QGraphicsItem *item1 = new PainterItem;
    QGraphicsItem *item2 = new PainterItem;
    QGraphicsItem *item3 = new PainterItem;
    QGraphicsItem *item4 = new PainterItem;
    scene.addItem(item1);
    scene.addItem(item2);
    scene.addItem(item3);
    scene.addItem(item4);
    item2->setZValue(-3);
    item4->setZValue(-2);
    item1->setZValue(-1);
    item3->setZValue(0);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QApplication::processEvents();
#ifdef Q_WS_QWS
    QApplication::sendPostedEvents(); //glib workaround
#endif

    QVERIFY(!paintedItems.isEmpty());
    QVERIFY((paintedItems.size() % 4) == 0);
    for (int i = 0; i < 3; ++i)
        QVERIFY(paintedItems.at(i)->zValue() < paintedItems.at(i + 1)->zValue());
}

void tst_QGraphicsItem::shape()
{
    QGraphicsLineItem line(QLineF(-10, -10, 20, 20));

    // We unfortunately need this hack as QPainterPathStroker will set a width of 1.0
    // if we pass a value of 0.0 to QPainterPathStroker::setWidth()
    const qreal penWidthZero = qreal(0.00000001);

    QPainterPathStroker ps;
    ps.setWidth(penWidthZero);

    QPainterPath path(line.line().p1());
    path.lineTo(line.line().p2());
    QPainterPath p = ps.createStroke(path);
    p.addPath(path);
    QCOMPARE(line.shape(), p);

    QPen linePen;
    linePen.setWidthF(5.0);
    linePen.setCapStyle(Qt::RoundCap);
    line.setPen(linePen);

    ps.setCapStyle(line.pen().capStyle());
    ps.setWidth(line.pen().widthF());
    p = ps.createStroke(path);
    p.addPath(path);
    QCOMPARE(line.shape(), p);

    linePen.setCapStyle(Qt::FlatCap);
    line.setPen(linePen);
    ps.setCapStyle(line.pen().capStyle());
    p = ps.createStroke(path);
    p.addPath(path);
    QCOMPARE(line.shape(), p);

    linePen.setCapStyle(Qt::SquareCap);
    line.setPen(linePen);
    ps.setCapStyle(line.pen().capStyle());
    p = ps.createStroke(path);
    p.addPath(path);
    QCOMPARE(line.shape(), p);

    QGraphicsRectItem rect(QRectF(-10, -10, 20, 20));
    QPainterPathStroker ps1;
    ps1.setWidth(penWidthZero);
    path = QPainterPath();
    path.addRect(rect.rect());
    p = ps1.createStroke(path);
    p.addPath(path);
    QCOMPARE(rect.shape(), p);

    QGraphicsEllipseItem ellipse(QRectF(-10, -10, 20, 20));
    QPainterPathStroker ps2;
    ps2.setWidth(ellipse.pen().widthF() <= 0.0 ? penWidthZero : ellipse.pen().widthF());
    path = QPainterPath();
    path.addEllipse(ellipse.rect());
    p = ps2.createStroke(path);
    p.addPath(path);
    QCOMPARE(ellipse.shape(), p);

    QPainterPathStroker ps3;
    ps3.setWidth(penWidthZero);
    p = ps3.createStroke(path);
    p.addPath(path);
    QGraphicsPathItem pathItem(path);
    QCOMPARE(pathItem.shape(), p);

    QRegion region(QRect(0, 0, 300, 200));
    region = region.subtracted(QRect(50, 50, 200, 100));

    QImage image(300, 200, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    painter.setClipRegion(region);
    painter.fillRect(0, 0, 300, 200, Qt::green);
    painter.end();
    QPixmap pixmap = QPixmap::fromImage(image);

    QGraphicsPixmapItem pixmapItem(pixmap);
    path = QPainterPath();
    path.addRegion(region);

    {
        QBitmap bitmap(300, 200);
        bitmap.clear();
        QPainter painter(&bitmap);
        painter.setClipRegion(region);
        painter.fillRect(0, 0, 300, 200, Qt::color1);
        painter.end();

        QBitmap bitmap2(300, 200);
        bitmap2.clear();
        painter.begin(&bitmap2);
        painter.setClipPath(pixmapItem.shape());
        painter.fillRect(0, 0, 300, 200, Qt::color1);
        painter.end();

        QCOMPARE(bitmap.toImage(), bitmap2.toImage());
    }

    QPolygonF poly;
    poly << QPointF(0, 0) << QPointF(10, 0) << QPointF(0, 10);
    QGraphicsPolygonItem polygon(poly);
    path = QPainterPath();
    path.addPolygon(poly);

    QPainterPathStroker ps4;
    ps4.setWidth(penWidthZero);
    p = ps4.createStroke(path);
    p.addPath(path);
    QCOMPARE(polygon.shape(), p);
}

void tst_QGraphicsItem::contains()
{
    if (sizeof(qreal) != sizeof(double)) {
        QSKIP("Skipped due to rounding errors", SkipAll);
    }

    // Rect
    QGraphicsRectItem rect(QRectF(-10, -10, 20, 20));
    QVERIFY(!rect.contains(QPointF(-11, -10)));
    QVERIFY(rect.contains(QPointF(-10, -10)));
    QVERIFY(!rect.contains(QPointF(-11, 0)));
    QVERIFY(rect.contains(QPointF(-10, 0)));
    QVERIFY(rect.contains(QPointF(0, -10)));
    QVERIFY(rect.contains(QPointF(0, 0)));
    QVERIFY(rect.contains(QPointF(9, 9)));

    // Ellipse
    QGraphicsEllipseItem ellipse(QRectF(-10, -10, 20, 20));
    QVERIFY(!ellipse.contains(QPointF(-10, -10)));
    QVERIFY(ellipse.contains(QPointF(-9, 0)));
    QVERIFY(ellipse.contains(QPointF(0, -9)));
    QVERIFY(ellipse.contains(QPointF(0, 0)));
    QVERIFY(!ellipse.contains(QPointF(9, 9)));

    // Line
    QGraphicsLineItem line(QLineF(-10, -10, 20, 20));
    QVERIFY(!line.contains(QPointF(-10, 0)));
    QVERIFY(!line.contains(QPointF(0, -10)));
    QVERIFY(!line.contains(QPointF(10, 0)));
    QVERIFY(!line.contains(QPointF(0, 10)));
    QVERIFY(line.contains(QPointF(0, 0)));
    QVERIFY(line.contains(QPointF(-9, -9)));
    QVERIFY(line.contains(QPointF(9, 9)));

    // Polygon
    QGraphicsPolygonItem polygon(QPolygonF()
                                 << QPointF(0, 0)
                                 << QPointF(10, 0)
                                 << QPointF(0, 10));
    QVERIFY(polygon.contains(QPointF(1, 1)));
    QVERIFY(polygon.contains(QPointF(4, 4)));
    QVERIFY(polygon.contains(QPointF(1, 4)));
    QVERIFY(polygon.contains(QPointF(4, 1)));
    QVERIFY(!polygon.contains(QPointF(8, 8)));
    QVERIFY(polygon.contains(QPointF(1, 8)));
    QVERIFY(polygon.contains(QPointF(8, 1)));
}

void tst_QGraphicsItem::collidesWith_item()
{
    // Rectangle
    QGraphicsRectItem rect(QRectF(-10, -10, 20, 20));
    QGraphicsRectItem rect2(QRectF(-10, -10, 20, 20));
    QVERIFY(rect.collidesWithItem(&rect2));
    QVERIFY(rect2.collidesWithItem(&rect));
    rect2.setPos(21, 21);
    QVERIFY(!rect.collidesWithItem(&rect2));
    QVERIFY(!rect2.collidesWithItem(&rect));
    rect2.setPos(-21, -21);
    QVERIFY(!rect.collidesWithItem(&rect2));
    QVERIFY(!rect2.collidesWithItem(&rect));
    rect2.setPos(-17, -17);
    QVERIFY(rect.collidesWithItem(&rect2));
    QVERIFY(rect2.collidesWithItem(&rect));

    QGraphicsEllipseItem ellipse(QRectF(-10, -10, 20, 20));
    QGraphicsEllipseItem ellipse2(QRectF(-10, -10, 20, 20));
    QVERIFY(ellipse.collidesWithItem(&ellipse2));
    QVERIFY(ellipse2.collidesWithItem(&ellipse));
    ellipse2.setPos(21, 21);
    QVERIFY(!ellipse.collidesWithItem(&ellipse2));
    QVERIFY(!ellipse2.collidesWithItem(&ellipse));
    ellipse2.setPos(-21, -21);
    QVERIFY(!ellipse.collidesWithItem(&ellipse2));
    QVERIFY(!ellipse2.collidesWithItem(&ellipse));

    ellipse2.setPos(-17, -17);
    QVERIFY(!ellipse.collidesWithItem(&ellipse2));
    QVERIFY(!ellipse2.collidesWithItem(&ellipse));

    {
        QGraphicsScene scene;
        QGraphicsRectItem rect(20, 20, 100, 100, 0, &scene);
        QGraphicsRectItem rect2(40, 40, 50, 50, 0, &scene);
        rect2.setZValue(1);
        QGraphicsLineItem line(0, 0, 200, 200, 0, &scene);
        line.setZValue(2);

        QCOMPARE(scene.items().size(), 3);

        QList<QGraphicsItem *> col1 = rect.collidingItems();
        QCOMPARE(col1.size(), 2);
        QCOMPARE(col1.first(), static_cast<QGraphicsItem *>(&line));
        QCOMPARE(col1.last(), static_cast<QGraphicsItem *>(&rect2));

        QList<QGraphicsItem *> col2 = rect2.collidingItems();
        QCOMPARE(col2.size(), 2);
        QCOMPARE(col2.first(), static_cast<QGraphicsItem *>(&line));
        QCOMPARE(col2.last(), static_cast<QGraphicsItem *>(&rect));

        QList<QGraphicsItem *> col3 = line.collidingItems();
        QCOMPARE(col3.size(), 2);
        QCOMPARE(col3.first(), static_cast<QGraphicsItem *>(&rect2));
        QCOMPARE(col3.last(), static_cast<QGraphicsItem *>(&rect));
    }
}

void tst_QGraphicsItem::collidesWith_path_data()
{
    QTest::addColumn<QPointF>("pos");
    QTest::addColumn<QMatrix>("matrix");
    QTest::addColumn<QPainterPath>("shape");
    QTest::addColumn<bool>("rectCollides");
    QTest::addColumn<bool>("ellipseCollides");

    QTest::newRow("nothing") << QPointF(0, 0) << QMatrix() << QPainterPath() << false << false;

    QPainterPath rect;
    rect.addRect(0, 0, 20, 20);

    QTest::newRow("rect1") << QPointF(0, 0) << QMatrix() << rect << true << true;
    QTest::newRow("rect2") << QPointF(0, 0) << QMatrix().translate(21, 21) << rect << false << false;
    QTest::newRow("rect3") << QPointF(21, 21) << QMatrix() << rect << false << false;
}

void tst_QGraphicsItem::collidesWith_path()
{
    QFETCH(QPointF, pos);
    QFETCH(QMatrix, matrix);
    QFETCH(QPainterPath, shape);
    QFETCH(bool, rectCollides);
    QFETCH(bool, ellipseCollides);

    QGraphicsRectItem rect(QRectF(0, 0, 20, 20));
    QGraphicsEllipseItem ellipse(QRectF(0, 0, 20, 20));

    rect.setPos(pos);
    rect.setMatrix(matrix);

    ellipse.setPos(pos);
    ellipse.setMatrix(matrix);

    QPainterPath mappedShape = rect.sceneMatrix().inverted().map(shape);

    if (rectCollides)
        QVERIFY(rect.collidesWithPath(mappedShape));
    else
        QVERIFY(!rect.collidesWithPath(mappedShape));

    if (ellipseCollides)
        QVERIFY(ellipse.collidesWithPath(mappedShape));
    else
        QVERIFY(!ellipse.collidesWithPath(mappedShape));
}

void tst_QGraphicsItem::collidesWithItemWithClip()
{
    QGraphicsScene scene;

    QGraphicsEllipseItem *ellipse = scene.addEllipse(0, 0, 100, 100);
    ellipse->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QGraphicsEllipseItem *ellipse2 = scene.addEllipse(0, 0, 10, 10);
    ellipse2->setParentItem(ellipse);
    QGraphicsEllipseItem *ellipse3 = scene.addEllipse(0, 0, 10, 10);
    ellipse3->setParentItem(ellipse);
    QGraphicsEllipseItem *ellipse5 = scene.addEllipse(50, 50, 10, 10);
    ellipse5->setParentItem(ellipse);
    QGraphicsEllipseItem *ellipse4 = scene.addEllipse(0, 0, 10, 10);

    QVERIFY(ellipse2->collidesWithItem(ellipse3));
    QVERIFY(ellipse3->collidesWithItem(ellipse2));
    QVERIFY(!ellipse2->collidesWithItem(ellipse));
    QVERIFY(!ellipse->collidesWithItem(ellipse2));
    QVERIFY(!ellipse4->collidesWithItem(ellipse));
    QVERIFY(!ellipse4->collidesWithItem(ellipse2));
    QVERIFY(!ellipse4->collidesWithItem(ellipse3));
    QVERIFY(!ellipse->collidesWithItem(ellipse4));
    QVERIFY(!ellipse2->collidesWithItem(ellipse4));
    QVERIFY(!ellipse3->collidesWithItem(ellipse4));
    QVERIFY(ellipse->collidesWithItem(ellipse5));
    QVERIFY(ellipse5->collidesWithItem(ellipse));
}

class MyItem : public QGraphicsEllipseItem
{
public:
    bool isObscuredBy(const QGraphicsItem *item) const
    {
        const MyItem *myItem = qgraphicsitem_cast<const MyItem *>(item);
        if (myItem) {
            if (item->zValue() > zValue()) {
                QRectF r = rect();
                QPointF topMid = (r.topRight()+r.topLeft())/2;
                QPointF botMid = (r.bottomRight()+r.bottomLeft())/2;
                QPointF leftMid = (r.topLeft()+r.bottomLeft())/2;
                QPointF rightMid = (r.topRight()+r.bottomRight())/2;

                QPainterPath mappedShape = item->mapToItem(this, item->opaqueArea());

                if (mappedShape.contains(topMid) &&
                    mappedShape.contains(botMid) &&
                    mappedShape.contains(leftMid) &&
                    mappedShape.contains(rightMid))
                    return true;
                else
                    return false;
            }
            else return false;
        }
        else
            return QGraphicsItem::isObscuredBy(item);
    }

    QPainterPath opaqueArea() const
    {
        return shape();
    }

    enum {
        Type = UserType+1
    };
    int type() const { return Type; }
};

void tst_QGraphicsItem::isObscuredBy()
{
    QGraphicsScene scene;

    MyItem myitem1, myitem2;

    myitem1.setRect(QRectF(50, 50, 40, 200));
    myitem1.rotate(67);

    myitem2.setRect(QRectF(25, 25, 20, 20));
    myitem2.setZValue(-1.0);
    scene.addItem(&myitem1);
    scene.addItem(&myitem2);

    QVERIFY(!myitem2.isObscuredBy(&myitem1));
    QVERIFY(!myitem1.isObscuredBy(&myitem2));

    myitem2.setRect(QRectF(-50, 85, 20, 20));
    QVERIFY(myitem2.isObscuredBy(&myitem1));
    QVERIFY(!myitem1.isObscuredBy(&myitem2));

    myitem2.setRect(QRectF(-30, 70, 20, 20));
    QVERIFY(!myitem2.isObscuredBy(&myitem1));
    QVERIFY(!myitem1.isObscuredBy(&myitem2));

    QGraphicsRectItem rect1, rect2;

    rect1.setRect(QRectF(-40, -40, 50, 50));
    rect1.setBrush(QBrush(Qt::red));
    rect2.setRect(QRectF(-30, -20, 20, 20));
    rect2.setZValue(-1.0);
    rect2.setBrush(QBrush(Qt::blue));

    QVERIFY(rect2.isObscuredBy(&rect1));
    QVERIFY(!rect1.isObscuredBy(&rect2));

    rect2.setPos(QPointF(-20, -25));

    QVERIFY(!rect2.isObscuredBy(&rect1));
    QVERIFY(!rect1.isObscuredBy(&rect2));

    rect2.setPos(QPointF(-100, -100));

    QVERIFY(!rect2.isObscuredBy(&rect1));
    QVERIFY(!rect1.isObscuredBy(&rect2));
}

class OpaqueItem : public QGraphicsRectItem
{
protected:
    QPainterPath opaqueArea() const
    {
        return shape();
    }
};

void tst_QGraphicsItem::isObscured()
{
    if (sizeof(qreal) != sizeof(double)) {
        QSKIP("Skipped due to rounding errors", SkipAll);
    }

    OpaqueItem *item1 = new OpaqueItem;
    item1->setRect(0, 0, 100, 100);
    item1->setZValue(0);

    OpaqueItem *item2 = new OpaqueItem;
    item2->setZValue(1);
    item2->setRect(0, 0, 100, 100);

    QGraphicsScene scene;
    scene.addItem(item1);
    scene.addItem(item2);

    QVERIFY(item1->isObscured());
    QVERIFY(item1->isObscuredBy(item2));
    QVERIFY(item1->isObscured(QRectF(0, 0, 50, 50)));
    QVERIFY(item1->isObscured(QRectF(50, 0, 50, 50)));
    QVERIFY(item1->isObscured(QRectF(50, 50, 50, 50)));
    QVERIFY(item1->isObscured(QRectF(0, 50, 50, 50)));
    QVERIFY(item1->isObscured(0, 0, 50, 50));
    QVERIFY(item1->isObscured(50, 0, 50, 50));
    QVERIFY(item1->isObscured(50, 50, 50, 50));
    QVERIFY(item1->isObscured(0, 50, 50, 50));
    QVERIFY(!item2->isObscured());
    QVERIFY(!item2->isObscuredBy(item1));
    QVERIFY(!item2->isObscured(QRectF(0, 0, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(50, 0, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(50, 50, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(0, 50, 50, 50)));
    QVERIFY(!item2->isObscured(0, 0, 50, 50));
    QVERIFY(!item2->isObscured(50, 0, 50, 50));
    QVERIFY(!item2->isObscured(50, 50, 50, 50));
    QVERIFY(!item2->isObscured(0, 50, 50, 50));

    item2->moveBy(50, 0);

    QVERIFY(!item1->isObscured());
    QVERIFY(!item1->isObscuredBy(item2));
    QVERIFY(!item1->isObscured(QRectF(0, 0, 50, 50)));
    QVERIFY(item1->isObscured(QRectF(50, 0, 50, 50)));
    QVERIFY(item1->isObscured(QRectF(50, 50, 50, 50)));
    QVERIFY(!item1->isObscured(QRectF(0, 50, 50, 50)));
    QVERIFY(!item1->isObscured(0, 0, 50, 50));
    QVERIFY(item1->isObscured(50, 0, 50, 50));
    QVERIFY(item1->isObscured(50, 50, 50, 50));
    QVERIFY(!item1->isObscured(0, 50, 50, 50));
    QVERIFY(!item2->isObscured());
    QVERIFY(!item2->isObscuredBy(item1));
    QVERIFY(!item2->isObscured(QRectF(0, 0, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(50, 0, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(50, 50, 50, 50)));
    QVERIFY(!item2->isObscured(QRectF(0, 50, 50, 50)));
    QVERIFY(!item2->isObscured(0, 0, 50, 50));
    QVERIFY(!item2->isObscured(50, 0, 50, 50));
    QVERIFY(!item2->isObscured(50, 50, 50, 50));
    QVERIFY(!item2->isObscured(0, 50, 50, 50));
}

void tst_QGraphicsItem::mapFromToParent()
{
    QPainterPath path1;
    path1.addRect(0, 0, 200, 200);

    QPainterPath path2;
    path2.addRect(0, 0, 100, 100);

    QPainterPath path3;
    path3.addRect(0, 0, 50, 50);

    QPainterPath path4;
    path4.addRect(0, 0, 25, 25);

    QGraphicsItem *item1 = new QGraphicsPathItem(path1);
    QGraphicsItem *item2 = new QGraphicsPathItem(path2, item1);
    QGraphicsItem *item3 = new QGraphicsPathItem(path3, item2);
    QGraphicsItem *item4 = new QGraphicsPathItem(path4, item3);

    item1->setPos(10, 10);
    item2->setPos(10, 10);
    item3->setPos(10, 10);
    item4->setPos(10, 10);

    for (int i = 0; i < 4; ++i) {
        QMatrix matrix;
        matrix.rotate(i * 90);
        matrix.translate(i * 100, -i * 100);
        matrix.scale(2, 4);
        item1->setMatrix(matrix);

        QCOMPARE(item1->mapToParent(QPointF(0, 0)), item1->pos() + matrix.map(QPointF(0, 0)));
        QCOMPARE(item2->mapToParent(QPointF(0, 0)), item2->pos());
        QCOMPARE(item3->mapToParent(QPointF(0, 0)), item3->pos());
        QCOMPARE(item4->mapToParent(QPointF(0, 0)), item4->pos());
        QCOMPARE(item1->mapToParent(QPointF(10, -10)), item1->pos() + matrix.map(QPointF(10, -10)));
        QCOMPARE(item2->mapToParent(QPointF(10, -10)), item2->pos() + QPointF(10, -10));
        QCOMPARE(item3->mapToParent(QPointF(10, -10)), item3->pos() + QPointF(10, -10));
        QCOMPARE(item4->mapToParent(QPointF(10, -10)), item4->pos() + QPointF(10, -10));
        QCOMPARE(item1->mapToParent(QPointF(-10, 10)), item1->pos() + matrix.map(QPointF(-10, 10)));
        QCOMPARE(item2->mapToParent(QPointF(-10, 10)), item2->pos() + QPointF(-10, 10));
        QCOMPARE(item3->mapToParent(QPointF(-10, 10)), item3->pos() + QPointF(-10, 10));
        QCOMPARE(item4->mapToParent(QPointF(-10, 10)), item4->pos() + QPointF(-10, 10));
        QCOMPARE(item1->mapFromParent(item1->pos()), matrix.inverted().map(QPointF(0, 0)));
        QCOMPARE(item2->mapFromParent(item2->pos()), QPointF(0, 0));
        QCOMPARE(item3->mapFromParent(item3->pos()), QPointF(0, 0));
        QCOMPARE(item4->mapFromParent(item4->pos()), QPointF(0, 0));
        QCOMPARE(item1->mapFromParent(item1->pos() + QPointF(10, -10)),
                 matrix.inverted().map(QPointF(10, -10)));
        QCOMPARE(item2->mapFromParent(item2->pos() + QPointF(10, -10)), QPointF(10, -10));
        QCOMPARE(item3->mapFromParent(item3->pos() + QPointF(10, -10)), QPointF(10, -10));
        QCOMPARE(item4->mapFromParent(item4->pos() + QPointF(10, -10)), QPointF(10, -10));
        QCOMPARE(item1->mapFromParent(item1->pos() + QPointF(-10, 10)),
                 matrix.inverted().map(QPointF(-10, 10)));
        QCOMPARE(item2->mapFromParent(item2->pos() + QPointF(-10, 10)), QPointF(-10, 10));
        QCOMPARE(item3->mapFromParent(item3->pos() + QPointF(-10, 10)), QPointF(-10, 10));
        QCOMPARE(item4->mapFromParent(item4->pos() + QPointF(-10, 10)), QPointF(-10, 10));
    }

    delete item1;
}

void tst_QGraphicsItem::mapFromToScene()
{
    QGraphicsItem *item1 = new QGraphicsPathItem(QPainterPath());
    QGraphicsItem *item2 = new QGraphicsPathItem(QPainterPath(), item1);
    QGraphicsItem *item3 = new QGraphicsPathItem(QPainterPath(), item2);
    QGraphicsItem *item4 = new QGraphicsPathItem(QPainterPath(), item3);

    item1->setPos(100, 100);
    item2->setPos(100, 100);
    item3->setPos(100, 100);
    item4->setPos(100, 100);
    QCOMPARE(item1->pos(), QPointF(100, 100));
    QCOMPARE(item2->pos(), QPointF(100, 100));
    QCOMPARE(item3->pos(), QPointF(100, 100));
    QCOMPARE(item4->pos(), QPointF(100, 100));
    QCOMPARE(item1->pos(), item1->mapToParent(0, 0));
    QCOMPARE(item2->pos(), item2->mapToParent(0, 0));
    QCOMPARE(item3->pos(), item3->mapToParent(0, 0));
    QCOMPARE(item4->pos(), item4->mapToParent(0, 0));
    QCOMPARE(item1->mapToParent(10, 10), QPointF(110, 110));
    QCOMPARE(item2->mapToParent(10, 10), QPointF(110, 110));
    QCOMPARE(item3->mapToParent(10, 10), QPointF(110, 110));
    QCOMPARE(item4->mapToParent(10, 10), QPointF(110, 110));
    QCOMPARE(item1->mapToScene(0, 0), QPointF(100, 100));
    QCOMPARE(item2->mapToScene(0, 0), QPointF(200, 200));
    QCOMPARE(item3->mapToScene(0, 0), QPointF(300, 300));
    QCOMPARE(item4->mapToScene(0, 0), QPointF(400, 400));
    QCOMPARE(item1->mapToScene(10, 0), QPointF(110, 100));
    QCOMPARE(item2->mapToScene(10, 0), QPointF(210, 200));
    QCOMPARE(item3->mapToScene(10, 0), QPointF(310, 300));
    QCOMPARE(item4->mapToScene(10, 0), QPointF(410, 400));
    QCOMPARE(item1->mapFromScene(100, 100), QPointF(0, 0));
    QCOMPARE(item2->mapFromScene(200, 200), QPointF(0, 0));
    QCOMPARE(item3->mapFromScene(300, 300), QPointF(0, 0));
    QCOMPARE(item4->mapFromScene(400, 400), QPointF(0, 0));
    QCOMPARE(item1->mapFromScene(110, 100), QPointF(10, 0));
    QCOMPARE(item2->mapFromScene(210, 200), QPointF(10, 0));
    QCOMPARE(item3->mapFromScene(310, 300), QPointF(10, 0));
    QCOMPARE(item4->mapFromScene(410, 400), QPointF(10, 0));

    // Rotate item1 90 degrees clockwise
    QMatrix matrix; matrix.rotate(90);
    item1->setMatrix(matrix);
    QCOMPARE(item1->pos(), item1->mapToParent(0, 0));
    QCOMPARE(item2->pos(), item2->mapToParent(0, 0));
    QCOMPARE(item3->pos(), item3->mapToParent(0, 0));
    QCOMPARE(item4->pos(), item4->mapToParent(0, 0));
    QCOMPARE(item1->mapToParent(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item3->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item4->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item1->mapToScene(0, 0), QPointF(100, 100));
    QCOMPARE(item2->mapToScene(0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapToScene(0, 0), QPointF(-100, 300));
    QCOMPARE(item4->mapToScene(0, 0), QPointF(-200, 400));
    QCOMPARE(item1->mapToScene(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToScene(10, 0), QPointF(0, 210));
    QCOMPARE(item3->mapToScene(10, 0), QPointF(-100, 310));
    QCOMPARE(item4->mapToScene(10, 0), QPointF(-200, 410));
    QCOMPARE(item1->mapFromScene(100, 100), QPointF(0, 0));
    QCOMPARE(item2->mapFromScene(0, 200), QPointF(0, 0));
    QCOMPARE(item3->mapFromScene(-100, 300), QPointF(0, 0));
    QCOMPARE(item4->mapFromScene(-200, 400), QPointF(0, 0));
    QCOMPARE(item1->mapFromScene(100, 110), QPointF(10, 0));
    QCOMPARE(item2->mapFromScene(0, 210), QPointF(10, 0));
    QCOMPARE(item3->mapFromScene(-100, 310), QPointF(10, 0));
    QCOMPARE(item4->mapFromScene(-200, 410), QPointF(10, 0));

    // Rotate item2 90 degrees clockwise
    item2->setMatrix(matrix);
    QCOMPARE(item1->pos(), item1->mapToParent(0, 0));
    QCOMPARE(item2->pos(), item2->mapToParent(0, 0));
    QCOMPARE(item3->pos(), item3->mapToParent(0, 0));
    QCOMPARE(item4->pos(), item4->mapToParent(0, 0));
    QCOMPARE(item1->mapToParent(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToParent(10, 0), QPointF(100, 110));
    QCOMPARE(item3->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item4->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item1->mapToScene(0, 0), QPointF(100, 100));
    QCOMPARE(item2->mapToScene(0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapToScene(0, 0), QPointF(-100, 100));
    QCOMPARE(item4->mapToScene(0, 0), QPointF(-200, 0));
    QCOMPARE(item1->mapToScene(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToScene(10, 0), QPointF(-10, 200));
    QCOMPARE(item3->mapToScene(10, 0), QPointF(-110, 100));
    QCOMPARE(item4->mapToScene(10, 0), QPointF(-210, 0));
    QCOMPARE(item1->mapFromScene(100, 100), QPointF(0, 0));
    QCOMPARE(item2->mapFromScene(0, 200), QPointF(0, 0));
    QCOMPARE(item3->mapFromScene(-100, 100), QPointF(0, 0));
    QCOMPARE(item4->mapFromScene(-200, 0), QPointF(0, 0));
    QCOMPARE(item1->mapFromScene(100, 110), QPointF(10, 0));
    QCOMPARE(item2->mapFromScene(-10, 200), QPointF(10, 0));
    QCOMPARE(item3->mapFromScene(-110, 100), QPointF(10, 0));
    QCOMPARE(item4->mapFromScene(-210, 0), QPointF(10, 0));

    // Translate item3 50 points, then rotate 90 degrees counterclockwise
    QMatrix matrix2;
    matrix2.translate(50, 0);
    matrix2.rotate(-90);
    item3->setMatrix(matrix2);
    QCOMPARE(item1->pos(), item1->mapToParent(0, 0));
    QCOMPARE(item2->pos(), item2->mapToParent(0, 0));
    QCOMPARE(item3->pos(), item3->mapToParent(0, 0) - QPointF(50, 0));
    QCOMPARE(item4->pos(), item4->mapToParent(0, 0));
    QCOMPARE(item1->mapToParent(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToParent(10, 0), QPointF(100, 110));
    QCOMPARE(item3->mapToParent(10, 0), QPointF(150, 90));
    QCOMPARE(item4->mapToParent(10, 0), QPointF(110, 100));
    QCOMPARE(item1->mapToScene(0, 0), QPointF(100, 100));
    QCOMPARE(item2->mapToScene(0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapToScene(0, 0), QPointF(-150, 100));
    QCOMPARE(item4->mapToScene(0, 0), QPointF(-250, 200));
    QCOMPARE(item1->mapToScene(10, 0), QPointF(100, 110));
    QCOMPARE(item2->mapToScene(10, 0), QPointF(-10, 200));
    QCOMPARE(item3->mapToScene(10, 0), QPointF(-150, 110));
    QCOMPARE(item4->mapToScene(10, 0), QPointF(-250, 210));
    QCOMPARE(item1->mapFromScene(100, 100), QPointF(0, 0));
    QCOMPARE(item2->mapFromScene(0, 200), QPointF(0, 0));
    QCOMPARE(item3->mapFromScene(-150, 100), QPointF(0, 0));
    QCOMPARE(item4->mapFromScene(-250, 200), QPointF(0, 0));
    QCOMPARE(item1->mapFromScene(100, 110), QPointF(10, 0));
    QCOMPARE(item2->mapFromScene(-10, 200), QPointF(10, 0));
    QCOMPARE(item3->mapFromScene(-150, 110), QPointF(10, 0));
    QCOMPARE(item4->mapFromScene(-250, 210), QPointF(10, 0));

    delete item1;
}

void tst_QGraphicsItem::mapFromToItem()
{
    QGraphicsItem *item1 = new QGraphicsPathItem;
    QGraphicsItem *item2 = new QGraphicsPathItem;
    QGraphicsItem *item3 = new QGraphicsPathItem;
    QGraphicsItem *item4 = new QGraphicsPathItem;

    item1->setPos(-100, -100);
    item2->setPos(100, -100);
    item3->setPos(100, 100);
    item4->setPos(-100, 100);

    QCOMPARE(item1->mapFromItem(item2, 0, 0), QPointF(200, 0));
    QCOMPARE(item2->mapFromItem(item3, 0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapFromItem(item4, 0, 0), QPointF(-200, 0));
    QCOMPARE(item4->mapFromItem(item1, 0, 0), QPointF(0, -200));
    QCOMPARE(item1->mapFromItem(item4, 0, 0), QPointF(0, 200));
    QCOMPARE(item2->mapFromItem(item1, 0, 0), QPointF(-200, 0));
    QCOMPARE(item3->mapFromItem(item2, 0, 0), QPointF(0, -200));
    QCOMPARE(item4->mapFromItem(item3, 0, 0), QPointF(200, 0));

    QMatrix matrix;
    matrix.translate(100, 100);
    item1->setMatrix(matrix);

    QCOMPARE(item1->mapFromItem(item2, 0, 0), QPointF(100, -100));
    QCOMPARE(item2->mapFromItem(item3, 0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapFromItem(item4, 0, 0), QPointF(-200, 0));
    QCOMPARE(item4->mapFromItem(item1, 0, 0), QPointF(100, -100));
    QCOMPARE(item1->mapFromItem(item4, 0, 0), QPointF(-100, 100));
    QCOMPARE(item2->mapFromItem(item1, 0, 0), QPointF(-100, 100));
    QCOMPARE(item3->mapFromItem(item2, 0, 0), QPointF(0, -200));
    QCOMPARE(item4->mapFromItem(item3, 0, 0), QPointF(200, 0));

    matrix.rotate(90);
    item1->setMatrix(matrix);
    item2->setMatrix(matrix);
    item3->setMatrix(matrix);
    item4->setMatrix(matrix);

    QCOMPARE(item1->mapFromItem(item2, 0, 0), QPointF(0, -200));
    QCOMPARE(item2->mapFromItem(item3, 0, 0), QPointF(200, 0));
    QCOMPARE(item3->mapFromItem(item4, 0, 0), QPointF(0, 200));
    QCOMPARE(item4->mapFromItem(item1, 0, 0), QPointF(-200, 0));
    QCOMPARE(item1->mapFromItem(item4, 0, 0), QPointF(200, 0));
    QCOMPARE(item2->mapFromItem(item1, 0, 0), QPointF(0, 200));
    QCOMPARE(item3->mapFromItem(item2, 0, 0), QPointF(-200, 0));
    QCOMPARE(item4->mapFromItem(item3, 0, 0), QPointF(0, -200));
    QCOMPARE(item1->mapFromItem(item2, 10, -5), QPointF(10, -205));
    QCOMPARE(item2->mapFromItem(item3, 10, -5), QPointF(210, -5));
    QCOMPARE(item3->mapFromItem(item4, 10, -5), QPointF(10, 195));
    QCOMPARE(item4->mapFromItem(item1, 10, -5), QPointF(-190, -5));
    QCOMPARE(item1->mapFromItem(item4, 10, -5), QPointF(210, -5));
    QCOMPARE(item2->mapFromItem(item1, 10, -5), QPointF(10, 195));
    QCOMPARE(item3->mapFromItem(item2, 10, -5), QPointF(-190, -5));
    QCOMPARE(item4->mapFromItem(item3, 10, -5), QPointF(10, -205));

    QCOMPARE(item1->mapFromItem(0, 10, -5), item1->mapFromScene(10, -5));
    QCOMPARE(item2->mapFromItem(0, 10, -5), item2->mapFromScene(10, -5));
    QCOMPARE(item3->mapFromItem(0, 10, -5), item3->mapFromScene(10, -5));
    QCOMPARE(item4->mapFromItem(0, 10, -5), item4->mapFromScene(10, -5));
    QCOMPARE(item1->mapToItem(0, 10, -5), item1->mapToScene(10, -5));
    QCOMPARE(item2->mapToItem(0, 10, -5), item2->mapToScene(10, -5));
    QCOMPARE(item3->mapToItem(0, 10, -5), item3->mapToScene(10, -5));
    QCOMPARE(item4->mapToItem(0, 10, -5), item4->mapToScene(10, -5));

    delete item1;
    delete item2;
    delete item3;
    delete item4;
}

void tst_QGraphicsItem::mapRectFromToParent_data()
{
    QTest::addColumn<bool>("parent");
    QTest::addColumn<QPointF>("parentPos");
    QTest::addColumn<QTransform>("parentTransform");
    QTest::addColumn<QPointF>("pos");
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QRectF>("inputRect");
    QTest::addColumn<QRectF>("outputRect");

    QTest::newRow("nil") << false << QPointF() << QTransform() << QPointF() << QTransform() << QRectF() << QRectF();
    QTest::newRow("simple") << false << QPointF() << QTransform() << QPointF() << QTransform()
                            << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10);
    QTest::newRow("simple w/parent") << true
                                     << QPointF() << QTransform()
                                     << QPointF() << QTransform()
                                     << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10);
    QTest::newRow("simple w/parent parentPos") << true
                                               << QPointF(50, 50) << QTransform()
                                               << QPointF() << QTransform()
                                               << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10);
    QTest::newRow("simple w/parent parentPos parentRotation") << true
                                                              << QPointF(50, 50) << QTransform().rotate(45)
                                                              << QPointF() << QTransform()
                                                              << QRectF(0, 0, 10, 10) << QRectF(0, 0, 10, 10);
    QTest::newRow("pos w/parent") << true
                                  << QPointF() << QTransform()
                                  << QPointF(50, 50) << QTransform()
                                  << QRectF(0, 0, 10, 10) << QRectF(50, 50, 10, 10);
    QTest::newRow("rotation w/parent") << true
                                       << QPointF() << QTransform()
                                       << QPointF() << QTransform().rotate(90)
                                       << QRectF(0, 0, 10, 10) << QRectF(-10, 0, 10, 10);
    QTest::newRow("pos rotation w/parent") << true
                                           << QPointF() << QTransform()
                                           << QPointF(50, 50) << QTransform().rotate(90)
                                           << QRectF(0, 0, 10, 10) << QRectF(40, 50, 10, 10);
    QTest::newRow("pos rotation w/parent parentPos parentRotation") << true
                                                                    << QPointF(-170, -190) << QTransform().rotate(90)
                                                                    << QPointF(50, 50) << QTransform().rotate(90)
                                                                    << QRectF(0, 0, 10, 10) << QRectF(40, 50, 10, 10);
}

void tst_QGraphicsItem::mapRectFromToParent()
{
    QFETCH(bool, parent);
    QFETCH(QPointF, parentPos);
    QFETCH(QTransform, parentTransform);
    QFETCH(QPointF, pos);
    QFETCH(QTransform, transform);
    QFETCH(QRectF, inputRect);
    QFETCH(QRectF, outputRect);

    QGraphicsRectItem *rect = new QGraphicsRectItem;
    rect->setPos(pos);
    rect->setTransform(transform);

    if (parent) {
        QGraphicsRectItem *rectParent = new QGraphicsRectItem;
        rect->setParentItem(rectParent);
        rectParent->setPos(parentPos);
        rectParent->setTransform(parentTransform);
    }

    // Make sure we use non-destructive transform operations (e.g., 90 degree
    // rotations).
    QCOMPARE(rect->mapRectToParent(inputRect), outputRect);
    QCOMPARE(rect->mapRectFromParent(outputRect), inputRect);
    QCOMPARE(rect->itemTransform(rect->parentItem()).mapRect(inputRect), outputRect);
    QCOMPARE(rect->mapToParent(inputRect).boundingRect(), outputRect);
    QCOMPARE(rect->mapToParent(QPolygonF(inputRect)).boundingRect(), outputRect);
    QCOMPARE(rect->mapFromParent(outputRect).boundingRect(), inputRect);
    QCOMPARE(rect->mapFromParent(QPolygonF(outputRect)).boundingRect(), inputRect);
    QPainterPath inputPath;
    inputPath.addRect(inputRect);
    QPainterPath outputPath;
    outputPath.addRect(outputRect);
    QCOMPARE(rect->mapToParent(inputPath).boundingRect(), outputPath.boundingRect());
    QCOMPARE(rect->mapFromParent(outputPath).boundingRect(), inputPath.boundingRect());
}

void tst_QGraphicsItem::isAncestorOf()
{
    QGraphicsItem *grandPa = new QGraphicsRectItem;
    QGraphicsItem *parent = new QGraphicsRectItem;
    QGraphicsItem *child = new QGraphicsRectItem;

    QVERIFY(!parent->isAncestorOf(0));
    QVERIFY(!child->isAncestorOf(0));
    QVERIFY(!parent->isAncestorOf(child));
    QVERIFY(!child->isAncestorOf(parent));
    QVERIFY(!parent->isAncestorOf(parent));

    child->setParentItem(parent);
    parent->setParentItem(grandPa);

    QVERIFY(parent->isAncestorOf(child));
    QVERIFY(grandPa->isAncestorOf(parent));
    QVERIFY(grandPa->isAncestorOf(child));
    QVERIFY(!child->isAncestorOf(parent));
    QVERIFY(!parent->isAncestorOf(grandPa));
    QVERIFY(!child->isAncestorOf(grandPa));
    QVERIFY(!child->isAncestorOf(child));
    QVERIFY(!parent->isAncestorOf(parent));
    QVERIFY(!grandPa->isAncestorOf(grandPa));

    parent->setParentItem(0);

    delete child;
    delete parent;
    delete grandPa;
}

void tst_QGraphicsItem::commonAncestorItem()
{
    QGraphicsItem *ancestor = new QGraphicsRectItem;
    QGraphicsItem *grandMa = new QGraphicsRectItem;
    QGraphicsItem *grandPa = new QGraphicsRectItem;
    QGraphicsItem *brotherInLaw = new QGraphicsRectItem;
    QGraphicsItem *cousin = new QGraphicsRectItem;
    QGraphicsItem *husband = new QGraphicsRectItem;
    QGraphicsItem *child = new QGraphicsRectItem;
    QGraphicsItem *wife = new QGraphicsRectItem;

    child->setParentItem(husband);
    husband->setParentItem(grandPa);
    brotherInLaw->setParentItem(grandPa);
    cousin->setParentItem(brotherInLaw);
    wife->setParentItem(grandMa);
    grandMa->setParentItem(ancestor);
    grandPa->setParentItem(ancestor);

    QCOMPARE(grandMa->commonAncestorItem(grandMa), grandMa);
    QCOMPARE(grandMa->commonAncestorItem(0), (QGraphicsItem *)0);
    QCOMPARE(grandMa->commonAncestorItem(grandPa), ancestor);
    QCOMPARE(grandPa->commonAncestorItem(grandMa), ancestor);
    QCOMPARE(grandPa->commonAncestorItem(husband), grandPa);
    QCOMPARE(grandPa->commonAncestorItem(wife), ancestor);
    QCOMPARE(grandMa->commonAncestorItem(husband), ancestor);
    QCOMPARE(grandMa->commonAncestorItem(wife), grandMa);
    QCOMPARE(wife->commonAncestorItem(grandMa), grandMa);
    QCOMPARE(child->commonAncestorItem(cousin), grandPa);
    QCOMPARE(cousin->commonAncestorItem(child), grandPa);
    QCOMPARE(wife->commonAncestorItem(child), ancestor);
    QCOMPARE(child->commonAncestorItem(wife), ancestor);
}

void tst_QGraphicsItem::data()
{
    QGraphicsTextItem text;

    QCOMPARE(text.data(0), QVariant());
    text.setData(0, "TextItem");
    QCOMPARE(text.data(0), QVariant(QString("TextItem")));
    text.setData(0, QVariant());
    QCOMPARE(text.data(0), QVariant());
}

void tst_QGraphicsItem::type()
{
    QCOMPARE(int(QGraphicsItem::Type), 1);
    QCOMPARE(int(QGraphicsPathItem::Type), 2);
    QCOMPARE(int(QGraphicsRectItem::Type), 3);
    QCOMPARE(int(QGraphicsEllipseItem::Type), 4);
    QCOMPARE(int(QGraphicsPolygonItem::Type), 5);
    QCOMPARE(int(QGraphicsLineItem::Type), 6);
    QCOMPARE(int(QGraphicsPixmapItem::Type), 7);
    QCOMPARE(int(QGraphicsTextItem::Type), 8);

    QCOMPARE(QGraphicsPathItem().type(), 2);
    QCOMPARE(QGraphicsRectItem().type(), 3);
    QCOMPARE(QGraphicsEllipseItem().type(), 4);
    QCOMPARE(QGraphicsPolygonItem().type(), 5);
    QCOMPARE(QGraphicsLineItem().type(), 6);
    QCOMPARE(QGraphicsPixmapItem().type(), 7);
    QCOMPARE(QGraphicsTextItem().type(), 8);
}

void tst_QGraphicsItem::graphicsitem_cast()
{
    QGraphicsPathItem pathItem;
    const QGraphicsPathItem *pPathItem = &pathItem;
    QGraphicsRectItem rectItem;
    const QGraphicsRectItem *pRectItem = &rectItem;
    QGraphicsEllipseItem ellipseItem;
    const QGraphicsEllipseItem *pEllipseItem = &ellipseItem;
    QGraphicsPolygonItem polygonItem;
    const QGraphicsPolygonItem *pPolygonItem = &polygonItem;
    QGraphicsLineItem lineItem;
    const QGraphicsLineItem *pLineItem = &lineItem;
    QGraphicsPixmapItem pixmapItem;
    const QGraphicsPixmapItem *pPixmapItem = &pixmapItem;
    QGraphicsTextItem textItem;
    const QGraphicsTextItem *pTextItem = &textItem;

    QVERIFY(qgraphicsitem_cast<QGraphicsPathItem *>(&pathItem));
    //QVERIFY(qgraphicsitem_cast<QAbstractGraphicsPathItem *>(&pathItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&pathItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pPathItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsPathItem *>(pPathItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsRectItem *>(&rectItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&rectItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pRectItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsRectItem *>(pRectItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsEllipseItem *>(&ellipseItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&ellipseItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pEllipseItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsEllipseItem *>(pEllipseItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsPolygonItem *>(&polygonItem));
    //QVERIFY(qgraphicsitem_cast<QAbstractGraphicsPathItem *>(&polygonItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&polygonItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pPolygonItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsPolygonItem *>(pPolygonItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsLineItem *>(&lineItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&lineItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pLineItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsLineItem *>(pLineItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsPixmapItem *>(&pixmapItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&pixmapItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pPixmapItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsPixmapItem *>(pPixmapItem));

    QVERIFY(qgraphicsitem_cast<QGraphicsTextItem *>(&textItem));
    QVERIFY(qgraphicsitem_cast<QGraphicsItem *>(&textItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsItem *>(pTextItem));
    QVERIFY(qgraphicsitem_cast<const QGraphicsTextItem *>(pTextItem));

    // and some casts that _should_ fail:
    QVERIFY(!qgraphicsitem_cast<QGraphicsEllipseItem *>(&pathItem));
    QVERIFY(!qgraphicsitem_cast<const QGraphicsTextItem *>(pPolygonItem));

    // and this shouldn't crash
    QGraphicsItem *ptr = 0;
    QVERIFY(!qgraphicsitem_cast<QGraphicsTextItem *>(ptr));
    QVERIFY(!qgraphicsitem_cast<QGraphicsItem *>(ptr));
}

void tst_QGraphicsItem::hoverEventsGenerateRepaints()
{
    Q_CHECK_PAINTEVENTS

    QGraphicsScene scene;
    EventTester *tester = new EventTester;
    scene.addItem(tester);
    tester->setAcceptsHoverEvents(true);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    qApp->processEvents();
    qApp->processEvents();


    // Send a hover enter event
    QGraphicsSceneHoverEvent hoverEnterEvent(QEvent::GraphicsSceneHoverEnter);
    hoverEnterEvent.setScenePos(QPointF(0, 0));
    hoverEnterEvent.setPos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &hoverEnterEvent);

    // Check that we get a repaint
    int npaints = tester->repaints;
    qApp->processEvents();
    qApp->processEvents();
    QCOMPARE(tester->events.size(), 2); // enter + move
    QCOMPARE(tester->repaints, npaints + 1);
    QCOMPARE(tester->events.last(), QEvent::GraphicsSceneHoverMove);

    // Send a hover move event
    QGraphicsSceneHoverEvent hoverMoveEvent(QEvent::GraphicsSceneHoverMove);
    hoverMoveEvent.setScenePos(QPointF(0, 0));
    hoverMoveEvent.setPos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &hoverMoveEvent);

    // Check that we don't get a repaint
    qApp->processEvents();
    qApp->processEvents();

    QCOMPARE(tester->events.size(), 3);
    QCOMPARE(tester->repaints, npaints + 1);
    QCOMPARE(tester->events.last(), QEvent::GraphicsSceneHoverMove);

    // Send a hover leave event
    QGraphicsSceneHoverEvent hoverLeaveEvent(QEvent::GraphicsSceneHoverLeave);
    hoverLeaveEvent.setScenePos(QPointF(-100, -100));
    hoverLeaveEvent.setPos(QPointF(0, 0));
    QApplication::sendEvent(&scene, &hoverLeaveEvent);

    // Check that we get a repaint
    qApp->processEvents();
    qApp->processEvents();

    QCOMPARE(tester->events.size(), 4);
    QCOMPARE(tester->repaints, npaints + 2);
    QCOMPARE(tester->events.last(), QEvent::GraphicsSceneHoverLeave);
}

void tst_QGraphicsItem::boundingRects_data()
{
    QTest::addColumn<QGraphicsItem *>("item");
    QTest::addColumn<QRectF>("boundingRect");

    QRectF rect(0, 0, 100, 100);
    QPainterPath path;
    path.addRect(rect);

    QRectF adjustedRect(-0.5, -0.5, 101, 101);

    QTest::newRow("path") << (QGraphicsItem *)new QGraphicsPathItem(path) << adjustedRect;
    QTest::newRow("rect") << (QGraphicsItem *)new QGraphicsRectItem(rect) << adjustedRect;
    QTest::newRow("ellipse") << (QGraphicsItem *)new QGraphicsEllipseItem(rect) << adjustedRect;
    QTest::newRow("polygon") << (QGraphicsItem *)new QGraphicsPolygonItem(rect) << adjustedRect;
}

void tst_QGraphicsItem::boundingRects()
{
    QFETCH(QGraphicsItem *, item);
    QFETCH(QRectF, boundingRect);

    ((QAbstractGraphicsShapeItem *)item)->setPen(QPen(Qt::black, 1));
    QCOMPARE(item->boundingRect(), boundingRect);
}

void tst_QGraphicsItem::boundingRects2()
{
    QGraphicsPixmapItem pixmap(QPixmap::fromImage(QImage(100, 100, QImage::Format_ARGB32_Premultiplied)));
    QCOMPARE(pixmap.boundingRect(), QRectF(-0.5, -0.5, 101, 101));

    QGraphicsLineItem line(0, 0, 100, 0);
    line.setPen(QPen(Qt::black, 1));
    QCOMPARE(line.boundingRect(), QRectF(-0.5, -0.5, 101, 1));
}

void tst_QGraphicsItem::sceneBoundingRect()
{
    QGraphicsScene scene;
    QGraphicsRectItem *item = scene.addRect(QRectF(0, 0, 100, 100), QPen(Qt::black, 0));
    item->setPos(100, 100);

    QCOMPARE(item->boundingRect(), QRectF(0, 0, 100, 100));
    QCOMPARE(item->sceneBoundingRect(), QRectF(100, 100, 100, 100));

    item->rotate(90);

    QCOMPARE(item->boundingRect(), QRectF(0, 0, 100, 100));
    QCOMPARE(item->sceneBoundingRect(), QRectF(0, 100, 100, 100));
}

void tst_QGraphicsItem::childrenBoundingRect()
{
    QGraphicsScene scene;
    QGraphicsRectItem *parent = scene.addRect(QRectF(0, 0, 100, 100), QPen(Qt::black, 0));
    QGraphicsRectItem *child = scene.addRect(QRectF(0, 0, 100, 100), QPen(Qt::black, 0));
    child->setParentItem(parent);
    parent->setPos(100, 100);
    child->setPos(100, 100);

    QCOMPARE(parent->boundingRect(), QRectF(0, 0, 100, 100));
    QCOMPARE(child->boundingRect(), QRectF(0, 0, 100, 100));
    QCOMPARE(child->childrenBoundingRect(), QRectF());
    QCOMPARE(parent->childrenBoundingRect(), QRectF(100, 100, 100, 100));

    QGraphicsRectItem *child2 = scene.addRect(QRectF(0, 0, 100, 100), QPen(Qt::black, 0));
    child2->setParentItem(parent);
    child2->setPos(-100, -100);
    QCOMPARE(parent->childrenBoundingRect(), QRectF(-100, -100, 300, 300));

    QGraphicsRectItem *childChild = scene.addRect(QRectF(0, 0, 100, 100), QPen(Qt::black, 0));
    childChild->setParentItem(child);
    childChild->setPos(500, 500);
    child->rotate(90);
    QCOMPARE(parent->childrenBoundingRect(), QRectF(-500, -100, 600, 800));
}

void tst_QGraphicsItem::childrenBoundingRect2()
{
    QGraphicsItemGroup box;
    QGraphicsLineItem l1(0, 0, 100, 0, &box);
    QGraphicsLineItem l2(100, 0, 100, 100, &box);
    QGraphicsLineItem l3(0, 0, 0, 100, &box);
    // Make sure lines (zero with/height) are included in the childrenBoundingRect.
    QCOMPARE(box.childrenBoundingRect(), QRectF(0, 0, 100, 100));
}

void tst_QGraphicsItem::group()
{
    QGraphicsScene scene;
    QGraphicsRectItem *parent = scene.addRect(QRectF(0, 0, 50, 50), QPen(Qt::black, 0), QBrush(Qt::green));
    QGraphicsRectItem *child = scene.addRect(QRectF(0, 0, 50, 50), QPen(Qt::black, 0), QBrush(Qt::blue));
    QGraphicsRectItem *parent2 = scene.addRect(QRectF(0, 0, 50, 50), QPen(Qt::black, 0), QBrush(Qt::red));
    parent2->setPos(-50, 50);
    child->rotate(45);
    child->setParentItem(parent);
    parent->setPos(25, 25);
    child->setPos(25, 25);

    QCOMPARE(parent->group(), (QGraphicsItemGroup *)0);
    QCOMPARE(parent2->group(), (QGraphicsItemGroup *)0);
    QCOMPARE(child->group(), (QGraphicsItemGroup *)0);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWait(250);

    QGraphicsItemGroup *group = new QGraphicsItemGroup;
    group->setSelected(true);
    scene.addItem(group);

    QRectF parentSceneBoundingRect = parent->sceneBoundingRect();
    group->addToGroup(parent);
    QCOMPARE(parent->group(), group);
    QCOMPARE(parent->sceneBoundingRect(), parentSceneBoundingRect);

    QCOMPARE(parent->parentItem(), (QGraphicsItem *)group);
    QCOMPARE(group->children().size(), 1);
    QCOMPARE(scene.items().size(), 4);
    QCOMPARE(scene.items(group->sceneBoundingRect()).size(), 3);

    QTest::qWait(250);

    QRectF parent2SceneBoundingRect = parent2->sceneBoundingRect();
    group->addToGroup(parent2);
    QCOMPARE(parent2->group(), group);
    QCOMPARE(parent2->sceneBoundingRect(), parent2SceneBoundingRect);

    QCOMPARE(parent2->parentItem(), (QGraphicsItem *)group);
    QCOMPARE(group->children().size(), 2);
    QCOMPARE(scene.items().size(), 4);
    QCOMPARE(scene.items(group->sceneBoundingRect()).size(), 4);

    QTest::qWait(250);

    QList<QGraphicsItem *> newItems;
    for (int i = 0; i < 100; ++i) {
        QGraphicsItem *item = scene.addRect(QRectF(-25, -25, 50, 50), QPen(Qt::black, 0),
                                            QBrush(QColor(rand() % 255, rand() % 255,
                                                          rand() % 255, rand() % 255)));
        newItems << item;
        item->setPos(-1000 + rand() % 2000,
                     -1000 + rand() % 2000);
        item->rotate(rand() % 90);
    }

    view.fitInView(scene.itemsBoundingRect());

    int n = 0;
    foreach (QGraphicsItem *item, newItems) {
        group->addToGroup(item);
        QCOMPARE(item->group(), group);
        if ((n++ % 100) == 0)
            QTest::qWait(10);
    }
}

void tst_QGraphicsItem::setGroup()
{
    QGraphicsItemGroup group1;
    QGraphicsItemGroup group2;

    QGraphicsRectItem *rect = new QGraphicsRectItem;
    QCOMPARE(rect->group(), (QGraphicsItemGroup *)0);
    QCOMPARE(rect->parentItem(), (QGraphicsItem *)0);
    rect->setGroup(&group1);
    QCOMPARE(rect->group(), &group1);
    QCOMPARE(rect->parentItem(), (QGraphicsItem *)&group1);
    rect->setGroup(&group2);
    QCOMPARE(rect->group(), &group2);
    QCOMPARE(rect->parentItem(), (QGraphicsItem *)&group2);
    rect->setGroup(0);
    QCOMPARE(rect->group(), (QGraphicsItemGroup *)0);
    QCOMPARE(rect->parentItem(), (QGraphicsItem *)0);
}

void tst_QGraphicsItem::nestedGroups()
{
    QGraphicsItemGroup *group1 = new QGraphicsItemGroup;
    QGraphicsItemGroup *group2 = new QGraphicsItemGroup;

    QGraphicsRectItem *rect = new QGraphicsRectItem;
    QGraphicsRectItem *rect2 = new QGraphicsRectItem;
    rect2->setParentItem(rect);

    group1->addToGroup(rect);
    QCOMPARE(rect->group(), group1);
    QCOMPARE(rect2->group(), group1);

    group2->addToGroup(group1);
    QCOMPARE(rect->group(), group1);
    QCOMPARE(rect2->group(), group1);
    QCOMPARE(group1->group(), group2);
    QCOMPARE(group2->group(), (QGraphicsItemGroup *)0);

    QGraphicsScene scene;
    scene.addItem(group1);

    QCOMPARE(rect->group(), group1);
    QCOMPARE(rect2->group(), group1);
    QCOMPARE(group1->group(), (QGraphicsItemGroup *)0);
    QVERIFY(group2->children().isEmpty());

    delete group2;
}

void tst_QGraphicsItem::warpChildrenIntoGroup()
{
    QGraphicsScene scene;
    QGraphicsRectItem *parentRectItem = scene.addRect(QRectF(0, 0, 100, 100));
    QGraphicsRectItem *childRectItem = scene.addRect(QRectF(0, 0, 100, 100));
    parentRectItem->rotate(90);
    childRectItem->setPos(-50, -25);
    childRectItem->setParentItem(parentRectItem);

    QCOMPARE(childRectItem->mapToScene(50, 0), QPointF(25, 0));
    QCOMPARE(childRectItem->scenePos(), QPointF(25, -50));

    QGraphicsRectItem *parentOfGroup = scene.addRect(QRectF(0, 0, 100, 100));
    parentOfGroup->setPos(-200, -200);
    parentOfGroup->scale(2, 2);

    QGraphicsItemGroup *group = new QGraphicsItemGroup;
    group->setPos(50, 50);
    group->setParentItem(parentOfGroup);

    QCOMPARE(group->scenePos(), QPointF(-100, -100));

    group->addToGroup(childRectItem);

    QCOMPARE(childRectItem->mapToScene(50, 0), QPointF(25, 0));
    QCOMPARE(childRectItem->scenePos(), QPointF(25, -50));
}

void tst_QGraphicsItem::removeFromGroup()
{
    QGraphicsScene scene;
    QGraphicsRectItem *rect1 = scene.addRect(QRectF(-100, -100, 200, 200));
    QGraphicsRectItem *rect2 = scene.addRect(QRectF(100, 100, 200, 200));
    rect1->setFlag(QGraphicsItem::ItemIsSelectable);
    rect2->setFlag(QGraphicsItem::ItemIsSelectable);
    rect1->setSelected(true);
    rect2->setSelected(true);

    QGraphicsView view(&scene);
    view.show();

    qApp->processEvents(); // index items
    qApp->processEvents(); // emit changed

    QGraphicsItemGroup *group = scene.createItemGroup(scene.selectedItems());
    QVERIFY(group);
    QCOMPARE(group->children().size(), 2);
    qApp->processEvents(); // index items
    qApp->processEvents(); // emit changed

    scene.destroyItemGroup(group); // calls removeFromGroup.

    qApp->processEvents(); // index items
    qApp->processEvents(); // emit changed

    QCOMPARE(scene.items().size(), 2);
    QVERIFY(!rect1->group());
    QVERIFY(!rect2->group());
}

class ChildEventTester : public QGraphicsRectItem
{
public:
    ChildEventTester(const QRectF &rect, QGraphicsItem *parent = 0)
        : QGraphicsRectItem(rect, parent), counter(0)
    { }

    int counter;

protected:
    void focusInEvent(QFocusEvent *event)
    { ++counter; QGraphicsRectItem::focusInEvent(event); }
    void mousePressEvent(QGraphicsSceneMouseEvent *)
    { ++counter; }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *)
    { ++counter; }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
    { ++counter; }
};
void tst_QGraphicsItem::handlesChildEvents()
{
    ChildEventTester *item2_level2 = new ChildEventTester(QRectF(0, 0, 25, 25));
    ChildEventTester *item1_level1 = new ChildEventTester(QRectF(0, 0, 50, 50));
    ChildEventTester *item_level0 = new ChildEventTester(QRectF(0, 0, 100, 100));
    ChildEventTester *item1_level2 = new ChildEventTester(QRectF(0, 0, 25, 25));
    ChildEventTester *item2_level1 = new ChildEventTester(QRectF(0, 0, 50, 50));

    item_level0->setBrush(QBrush(Qt::blue));
    item1_level1->setBrush(QBrush(Qt::red));
    item2_level1->setBrush(QBrush(Qt::yellow));
    item1_level2->setBrush(QBrush(Qt::green));
    item2_level2->setBrush(QBrush(Qt::gray));
    item1_level1->setPos(50, 0);
    item2_level1->setPos(50, 50);
    item1_level2->setPos(25, 0);
    item2_level2->setPos(25, 25);
    item1_level1->setParentItem(item_level0);
    item2_level1->setParentItem(item_level0);
    item1_level2->setParentItem(item1_level1);
    item2_level2->setParentItem(item1_level1);

    QGraphicsScene scene;
    scene.addItem(item_level0);

    // Pull out the items, closest item first
    QList<QGraphicsItem *> items = scene.items(scene.itemsBoundingRect());
    QCOMPARE(items.at(4), (QGraphicsItem *)item_level0);
    if (item1_level1 < item2_level1) {
        QCOMPARE(items.at(3), (QGraphicsItem *)item1_level1);
        if (item1_level2 < item2_level2) {
            QCOMPARE(items.at(2), (QGraphicsItem *)item1_level2);
            QCOMPARE(items.at(1), (QGraphicsItem *)item2_level2);
        } else {
            QCOMPARE(items.at(2), (QGraphicsItem *)item2_level2);
            QCOMPARE(items.at(1), (QGraphicsItem *)item1_level2);
        }
        QCOMPARE(items.at(0), (QGraphicsItem *)item2_level1);
    } else {
        QCOMPARE(items.at(3), (QGraphicsItem *)item2_level1);
        QCOMPARE(items.at(2), (QGraphicsItem *)item1_level1);
        if (item1_level2 < item2_level2) {
            QCOMPARE(items.at(1), (QGraphicsItem *)item1_level2);
            QCOMPARE(items.at(0), (QGraphicsItem *)item2_level2);
        } else {
            QCOMPARE(items.at(1), (QGraphicsItem *)item2_level2);
            QCOMPARE(items.at(0), (QGraphicsItem *)item1_level2);
        }
    }

    QGraphicsView view(&scene);
    view.show();

    QTest::qWait(1000);

    QCOMPARE(item_level0->counter, 0);

    // Send events to the toplevel item
    QGraphicsSceneMouseEvent pressEvent(QEvent::GraphicsSceneMousePress);
    QGraphicsSceneMouseEvent releaseEvent(QEvent::GraphicsSceneMouseRelease);

    pressEvent.setButton(Qt::LeftButton);
    pressEvent.setScenePos(item_level0->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setButton(Qt::LeftButton);
    releaseEvent.setScenePos(item_level0->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 2);

    // Send events to a level1 item
    pressEvent.setScenePos(item1_level1->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level1->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 2);
    QCOMPARE(item1_level1->counter, 2);

    // Send events to a level2 item
    pressEvent.setScenePos(item1_level2->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level2->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 2);
    QCOMPARE(item1_level1->counter, 2);
    QCOMPARE(item1_level2->counter, 2);

    item_level0->setHandlesChildEvents(true);

    // Send events to a level1 item
    pressEvent.setScenePos(item1_level1->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level1->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 4);
    QCOMPARE(item1_level1->counter, 2);

    // Send events to a level2 item
    pressEvent.setScenePos(item1_level2->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level2->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 6);
    QCOMPARE(item1_level1->counter, 2);
    QCOMPARE(item1_level2->counter, 2);

    item_level0->setHandlesChildEvents(false);

    // Send events to a level1 item
    pressEvent.setScenePos(item1_level1->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level1->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 6);
    QCOMPARE(item1_level1->counter, 4);

    // Send events to a level2 item
    pressEvent.setScenePos(item1_level2->mapToScene(5, 5));
    pressEvent.setScreenPos(view.mapFromScene(pressEvent.scenePos()));
    releaseEvent.setScenePos(item1_level2->mapToScene(5, 5));
    releaseEvent.setScreenPos(view.mapFromScene(releaseEvent.scenePos()));
    QApplication::sendEvent(&scene, &pressEvent);
    QApplication::sendEvent(&scene, &releaseEvent);

    QCOMPARE(item_level0->counter, 6);
    QCOMPARE(item1_level1->counter, 4);
    QCOMPARE(item1_level2->counter, 4);
}

void tst_QGraphicsItem::handlesChildEvents2()
{
    ChildEventTester *root = new ChildEventTester(QRectF(0, 0, 10, 10));
    root->setHandlesChildEvents(true);
    QVERIFY(root->handlesChildEvents());

    ChildEventTester *child = new ChildEventTester(QRectF(0, 0, 10, 10), root);
    QVERIFY(!child->handlesChildEvents());

    ChildEventTester *child2 = new ChildEventTester(QRectF(0, 0, 10, 10));
    ChildEventTester *child3 = new ChildEventTester(QRectF(0, 0, 10, 10), child2);
    ChildEventTester *child4 = new ChildEventTester(QRectF(0, 0, 10, 10), child3);
    child2->setParentItem(root);
    QVERIFY(!child2->handlesChildEvents());
    QVERIFY(!child3->handlesChildEvents());
    QVERIFY(!child4->handlesChildEvents());

    QGraphicsScene scene;
    scene.addItem(root);

    QGraphicsView view(&scene);
    view.show();

    QTestEventLoop::instance().enterLoop(1);

    QMouseEvent event(QEvent::MouseButtonPress, view.mapFromScene(5, 5),
                      view.viewport()->mapToGlobal(view.mapFromScene(5, 5)), Qt::LeftButton, 0, 0);
    QApplication::sendEvent(view.viewport(), &event);

    QCOMPARE(root->counter, 1);
}

void tst_QGraphicsItem::handlesChildEvents3()
{
    QGraphicsScene scene;
    ChildEventTester *group2 = new ChildEventTester(QRectF(), 0);
    ChildEventTester *group1 = new ChildEventTester(QRectF(), group2);
    ChildEventTester *leaf = new ChildEventTester(QRectF(), group1);
    scene.addItem(group2);

    leaf->setFlag(QGraphicsItem::ItemIsFocusable);
    group1->setFlag(QGraphicsItem::ItemIsFocusable);
    group1->setHandlesChildEvents(true);
    group2->setFlag(QGraphicsItem::ItemIsFocusable);
    group2->setHandlesChildEvents(true);

    leaf->setFocus();
    QVERIFY(leaf->hasFocus()); // group2 stole the event, but leaf still got focus
    QVERIFY(!group1->hasFocus());
    QVERIFY(!group2->hasFocus());
    QCOMPARE(leaf->counter, 0);
    QCOMPARE(group1->counter, 0);
    QCOMPARE(group2->counter, 1);

    group1->setFocus();
    QVERIFY(group1->hasFocus()); // group2 stole the event, but group1 still got focus
    QVERIFY(!leaf->hasFocus());
    QVERIFY(!group2->hasFocus());
    QCOMPARE(leaf->counter, 0);
    QCOMPARE(group1->counter, 0);
    QCOMPARE(group2->counter, 2);

    group2->setFocus();
    QVERIFY(group2->hasFocus()); // group2 stole the event, and now group2 also has focus
    QVERIFY(!leaf->hasFocus());
    QVERIFY(!group1->hasFocus());
    QCOMPARE(leaf->counter, 0);
    QCOMPARE(group1->counter, 0);
    QCOMPARE(group2->counter, 3);
}

class CustomItem : public QGraphicsItem
{
public:
    QRectF boundingRect() const
    { return QRectF(-110, -110, 220, 220); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        for (int x = -100; x <= 100; x += 25)
            painter->drawLine(x, -100, x, 100);
        for (int y = -100; y <= 100; y += 25)
            painter->drawLine(-100, y, 100, y);

        QFont font = painter->font();
        font.setPointSize(4);
        painter->setFont(font);
        for (int x = -100; x < 100; x += 25) {
            for (int y = -100; y < 100; y += 25) {
                painter->drawText(QRectF(x, y, 25, 25), Qt::AlignCenter, QString("%1x%2").arg(x).arg(y));
            }
        }
    }
};

void tst_QGraphicsItem::ensureVisible()
{
    QGraphicsScene scene;
    scene.setSceneRect(-200, -200, 400, 400);
    QGraphicsItem *item = new CustomItem;
    scene.addItem(item);

    QGraphicsView view(&scene);
    view.setFixedSize(300, 300);
    view.show();

    for (int i = 0; i < 25; ++i) {
        view.scale(qreal(1.06), qreal(1.06));
        QTest::qWait(25);
    }

    item->ensureVisible(-100, -100, 25, 25);
    QTest::qWait(250);

    for (int x = -100; x < 100; x += 25) {
        for (int y = -100; y < 100; y += 25) {
            int xmargin = rand() % 75;
            int ymargin = rand() % 75;
            item->ensureVisible(x, y, 25, 25, xmargin, ymargin);
            QTest::qWait(25);

            QPolygonF viewScenePoly;
            viewScenePoly << view.mapToScene(view.rect().topLeft())
                          << view.mapToScene(view.rect().topRight())
                          << view.mapToScene(view.rect().bottomRight())
                          << view.mapToScene(view.rect().bottomLeft());

            QVERIFY(scene.items(viewScenePoly).contains(item));

            QPainterPath path;
            path.addPolygon(viewScenePoly);
            QVERIFY(path.contains(item->mapToScene(x + 12, y + 12)));

            QPolygonF viewScenePolyMinusMargins;
            viewScenePolyMinusMargins << view.mapToScene(view.rect().topLeft() + QPoint(xmargin, ymargin))
                          << view.mapToScene(view.rect().topRight() + QPoint(-xmargin, ymargin))
                          << view.mapToScene(view.rect().bottomRight() + QPoint(-xmargin, -ymargin))
                          << view.mapToScene(view.rect().bottomLeft() + QPoint(xmargin, -ymargin));

            QPainterPath path2;
            path2.addPolygon(viewScenePolyMinusMargins);
            QVERIFY(path2.contains(item->mapToScene(x + 12, y + 12)));
        }
    }

    item->ensureVisible(100, 100, 25, 25);
    QTest::qWait(250);
}

void tst_QGraphicsItem::cursor()
{
#ifndef QT_NO_CURSOR
    QGraphicsScene scene;
    QGraphicsRectItem *item1 = scene.addRect(QRectF(0, 0, 50, 50));
    QGraphicsRectItem *item2 = scene.addRect(QRectF(0, 0, 50, 50));
    item1->setPos(-100, 0);
    item2->setPos(50, 0);

    QVERIFY(!item1->hasCursor());
    QVERIFY(!item2->hasCursor());

    item1->setCursor(Qt::IBeamCursor);
    item2->setCursor(Qt::PointingHandCursor);

    QVERIFY(item1->hasCursor());
    QVERIFY(item2->hasCursor());

    item1->setCursor(QCursor());
    item2->setCursor(QCursor());

    QVERIFY(item1->hasCursor());
    QVERIFY(item2->hasCursor());

    item1->unsetCursor();
    item2->unsetCursor();

    QVERIFY(!item1->hasCursor());
    QVERIFY(!item2->hasCursor());

    item1->setCursor(Qt::IBeamCursor);
    item2->setCursor(Qt::PointingHandCursor);

    QGraphicsView view(&scene);
    view.setFixedSize(200, 100);
    view.show();
    QTest::mouseMove(&view, view.rect().center());

    QTest::qWait(250);

    QCursor cursor = view.viewport()->cursor();

    {
        QMouseEvent event(QEvent::MouseMove, QPoint(100, 50), Qt::NoButton, 0, 0);
        QApplication::sendEvent(view.viewport(), &event);
    }

    QTest::qWait(250);

    QCOMPARE(view.viewport()->cursor().shape(), cursor.shape());

    {
        QTest::mouseMove(view.viewport(), view.mapFromScene(item1->sceneBoundingRect().center()));
        QMouseEvent event(QEvent::MouseMove, view.mapFromScene(item1->sceneBoundingRect().center()), Qt::NoButton, 0, 0);
        QApplication::sendEvent(view.viewport(), &event);
    }

#if !defined(Q_OS_WINCE)
    QTest::qWait(250);
#else
    // Test environment does not have any cursor, therefore no shape
    return;
#endif

    QCOMPARE(view.viewport()->cursor().shape(), item1->cursor().shape());

    {
        QTest::mouseMove(view.viewport(), view.mapFromScene(item2->sceneBoundingRect().center()));
        QMouseEvent event(QEvent::MouseMove, view.mapFromScene(item2->sceneBoundingRect().center()), Qt::NoButton, 0, 0);
        QApplication::sendEvent(view.viewport(), &event);
    }

    QTest::qWait(250);

    QCOMPARE(view.viewport()->cursor().shape(), item2->cursor().shape());

    {
        QTest::mouseMove(view.viewport(), view.rect().center());
        QMouseEvent event(QEvent::MouseMove, QPoint(100, 25), Qt::NoButton, 0, 0);
        QApplication::sendEvent(view.viewport(), &event);
    }

    QTest::qWait(250);

    QCOMPARE(view.viewport()->cursor().shape(), cursor.shape());
#endif
}
/*
void tst_QGraphicsItem::textControlGetterSetter()
{
    QGraphicsTextItem *item = new QGraphicsTextItem;
    QVERIFY(item->textControl()->parent() == item);
    QPointer<QTextControl> control = item->textControl();
    delete item;
    QVERIFY(!control);

    item = new QGraphicsTextItem;

    QPointer<QTextControl> oldControl = control;
    control = new QTextControl;

    item->setTextControl(control);
    QVERIFY(item->textControl() == control);
    QVERIFY(!control->parent());
    QVERIFY(!oldControl);

    // set some text to give it a size, to test that
    // setTextControl (re)connects signals
    const QRectF oldBoundingRect = item->boundingRect();
    QVERIFY(oldBoundingRect.isValid());
    item->setPlainText("Some text");
    item->adjustSize();
    QVERIFY(item->boundingRect().isValid());
    QVERIFY(item->boundingRect() != oldBoundingRect);

    // test that on setting a control the item size
    // is adjusted
    oldControl = control;
    control = new QTextControl;
    control->setPlainText("foo!");
    item->setTextControl(control);
    QCOMPARE(item->boundingRect().size(), control->document()->documentLayout()->documentSize());

    QVERIFY(oldControl);
    delete oldControl;

    delete item;
    QVERIFY(control);
    delete control;
}
*/

void tst_QGraphicsItem::defaultItemTest_QGraphicsLineItem()
{
    QGraphicsLineItem item;
    QCOMPARE(item.line(), QLineF());
    QCOMPARE(item.pen(), QPen());
    QCOMPARE(item.shape(), QPainterPath());

    item.setPen(QPen(Qt::black, 1));
    QCOMPARE(item.pen(), QPen(Qt::black, 1));
    item.setLine(QLineF(0, 0, 10, 0));
    QCOMPARE(item.line(), QLineF(0, 0, 10, 0));
    QCOMPARE(item.boundingRect(), QRectF(-0.5, -0.5, 11, 1));
    QCOMPARE(item.shape().elementCount(), 11);

    QPainterPath path;
    path.moveTo(0, -0.5);
    path.lineTo(10, -0.5);
    path.lineTo(10.5, -0.5);
    path.lineTo(10.5, 0.5);
    path.lineTo(10, 0.5);
    path.lineTo(0, 0.5);
    path.lineTo(-0.5, 0.5);
    path.lineTo(-0.5, -0.5);
    path.lineTo(0, -0.5);
    path.lineTo(0, 0);
    path.lineTo(10, 0);
    path.closeSubpath();

    for (int i = 0; i < 11; ++i)
        QCOMPARE(QPointF(item.shape().elementAt(i)), QPointF(path.elementAt(i)));
}

void tst_QGraphicsItem::defaultItemTest_QGraphicsPixmapItem()
{
    QGraphicsPixmapItem item;
    QVERIFY(item.pixmap().isNull());
    QCOMPARE(item.offset(), QPointF());
    QCOMPARE(item.transformationMode(), Qt::FastTransformation);

    QPixmap pixmap(300, 200);
    pixmap.fill(Qt::red);
    item.setPixmap(pixmap);
    QCOMPARE(item.pixmap(), pixmap);

    item.setTransformationMode(Qt::FastTransformation);
    QCOMPARE(item.transformationMode(), Qt::FastTransformation);
    item.setTransformationMode(Qt::SmoothTransformation);
    QCOMPARE(item.transformationMode(), Qt::SmoothTransformation);

    item.setOffset(-15, -15);
    QCOMPARE(item.offset(), QPointF(-15, -15));
    item.setOffset(QPointF(-10, -10));
    QCOMPARE(item.offset(), QPointF(-10, -10));

    QCOMPARE(item.boundingRect(), QRectF(-10.5, -10.5, 301, 201));
}

void tst_QGraphicsItem::defaultItemTest_QGraphicsTextItem()
{
    QGraphicsTextItem *text = new QGraphicsTextItem;
    QVERIFY(!text->openExternalLinks());
    QVERIFY(text->textCursor().isNull());
    QCOMPARE(text->defaultTextColor(), QPalette().color(QPalette::Text));
    QVERIFY(text->document() != 0);
    QCOMPARE(text->font(), QApplication::font());
    QCOMPARE(text->textInteractionFlags(), Qt::TextInteractionFlags(Qt::NoTextInteraction));
    QCOMPARE(text->textWidth(), -1.0);
    QCOMPARE(text->toPlainText(), QString(""));

    QGraphicsScene scene;
    scene.addItem(text);
    text->setPlainText("Hello world");
    text->setFlag(QGraphicsItem::ItemIsMovable);

    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setScenePos(QPointF(1, 1));
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event);
        QGraphicsSceneMouseEvent event2(QEvent::GraphicsSceneMouseMove);
        event2.setScenePos(QPointF(11, 11));
        event2.setButton(Qt::LeftButton);
        event2.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event2);
    }

    QCOMPARE(text->pos(), QPointF(10, 10));

    text->setTextInteractionFlags(Qt::TextEditorInteraction);
    QCOMPARE(text->textInteractionFlags(), Qt::TextInteractionFlags(Qt::TextEditorInteraction));

    {
        QGraphicsSceneMouseEvent event2(QEvent::GraphicsSceneMouseMove);
        event2.setScenePos(QPointF(21, 21));
        event2.setButton(Qt::LeftButton);
        event2.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event2);
    }

    QCOMPARE(text->pos(), QPointF(20, 20)); // clicked on edge, item moved
}

void tst_QGraphicsItem::defaultItemTest_QGraphicsEllipseItem()
{
    QGraphicsEllipseItem item;
    QVERIFY(item.rect().isNull());
    QVERIFY(item.boundingRect().isNull());
    QVERIFY(item.shape().isEmpty());
    QCOMPARE(item.spanAngle(), 360 * 16);
    QCOMPARE(item.startAngle(), 0);

    item.setRect(0, 0, 100, 100);
    QCOMPARE(item.boundingRect(), QRectF(0, 0, 100, 100));

    item.setSpanAngle(90 * 16);
    qFuzzyCompare(item.boundingRect().left(), qreal(50.0));
    qFuzzyCompare(item.boundingRect().top(), qreal(0.0));
    qFuzzyCompare(item.boundingRect().width(), qreal(50.0));
    qFuzzyCompare(item.boundingRect().height(), qreal(50.0));

    item.setPen(QPen(Qt::black, 1));
    QCOMPARE(item.boundingRect(), QRectF(49.5, -0.5, 51, 51));

    item.setSpanAngle(180 * 16);
    QCOMPARE(item.boundingRect(), QRectF(-0.5, -0.5, 101, 51));

    item.setSpanAngle(360 * 16);
    QCOMPARE(item.boundingRect(), QRectF(-0.5, -0.5, 101, 101));
}

class ItemChangeTester : public QGraphicsRectItem
{
public:
    ItemChangeTester(){}
    ItemChangeTester(QGraphicsItem *parent) : QGraphicsRectItem(parent) {}
    QVariant itemChangeReturnValue;
    QGraphicsScene *itemSceneChangeTargetScene;

    QList<GraphicsItemChange> changes;
    QList<QVariant> values;
    QList<QVariant> oldValues;
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        changes << change;
        values << value;
        switch (change) {
        case QGraphicsItem::ItemPositionChange:
            oldValues << pos();
            break;
        case QGraphicsItem::ItemPositionHasChanged:
            break;
        case QGraphicsItem::ItemMatrixChange: {
            QVariant variant;
            qVariantSetValue<QMatrix>(variant, matrix());
            oldValues << variant;
        }
            break;
        case QGraphicsItem::ItemTransformChange: {
            QVariant variant;
            qVariantSetValue<QTransform>(variant, transform());
            oldValues << variant;
        }
            break;
        case QGraphicsItem::ItemTransformHasChanged:
            break;
        case QGraphicsItem::ItemVisibleChange:
            oldValues << isVisible();
            break;
        case QGraphicsItem::ItemVisibleHasChanged:
            break;
        case QGraphicsItem::ItemEnabledChange:
            oldValues << isEnabled();
            break;
        case QGraphicsItem::ItemEnabledHasChanged:
            break;
        case QGraphicsItem::ItemSelectedChange:
            oldValues << isSelected();
            break;
        case QGraphicsItem::ItemSelectedHasChanged:
            break;
        case QGraphicsItem::ItemParentChange:
            oldValues << qVariantFromValue<void *>(parentItem());
            break;
        case QGraphicsItem::ItemParentHasChanged:
            break;
        case QGraphicsItem::ItemChildAddedChange:
            oldValues << children().size();
            break;
        case QGraphicsItem::ItemChildRemovedChange:
            oldValues << children().size();
            break;
        case QGraphicsItem::ItemSceneChange:
            oldValues << qVariantFromValue<QGraphicsScene *>(scene());
            if (itemSceneChangeTargetScene
                && qVariantValue<QGraphicsScene *>(value)
                && itemSceneChangeTargetScene != qVariantValue<QGraphicsScene *>(value)) {
                return qVariantFromValue<QGraphicsScene *>(itemSceneChangeTargetScene);
            }
            return value;
        case QGraphicsItem::ItemSceneHasChanged:
            break;
        case QGraphicsItem::ItemCursorChange:
#ifndef QT_NO_CURSOR
            oldValues << cursor();
#endif
            break;
        case QGraphicsItem::ItemCursorHasChanged:
            break;
        case QGraphicsItem::ItemToolTipChange:
            oldValues << toolTip();
            break;
        case QGraphicsItem::ItemToolTipHasChanged:
            break;
        case QGraphicsItem::ItemFlagsChange:
            oldValues << quint32(flags());
            break;
        case QGraphicsItem::ItemFlagsHaveChanged:
            break;
        case QGraphicsItem::ItemZValueChange:
            oldValues << zValue();
            break;
        case QGraphicsItem::ItemZValueHasChanged:
            break;
        case QGraphicsItem::ItemOpacityChange:
            oldValues << opacity();
            break;
        case QGraphicsItem::ItemOpacityHasChanged:
            break;
        }
        return itemChangeReturnValue.isValid() ? itemChangeReturnValue : value;
    }
};

void tst_QGraphicsItem::itemChange()
{
    ItemChangeTester tester;
    tester.itemSceneChangeTargetScene = 0;

    ItemChangeTester testerHelper;
    QVERIFY(tester.changes.isEmpty());
    QVERIFY(tester.values.isEmpty());

    int changeCount = 0;
    {
        // ItemEnabledChange
        tester.itemChangeReturnValue = true;
        tester.setEnabled(false);
        ++changeCount;
        ++changeCount; // HasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemEnabledChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemEnabledHasChanged);
        QCOMPARE(tester.values.at(tester.values.size() - 2), QVariant(false));
        QCOMPARE(tester.values.at(tester.values.size() - 1), QVariant(true));
        QCOMPARE(tester.oldValues.last(), QVariant(true));
        QCOMPARE(tester.isEnabled(), true);
    }
    {
        // ItemMatrixChange / ItemTransformHasChanged
        qVariantSetValue<QMatrix>(tester.itemChangeReturnValue, QMatrix().rotate(90));
        tester.setMatrix(QMatrix().translate(50, 0), true);
        ++changeCount; // notification sent too
        QCOMPARE(tester.changes.size(), ++changeCount);
        QCOMPARE(int(tester.changes.at(tester.changes.size() - 2)), int(QGraphicsItem::ItemMatrixChange));
        QCOMPARE(int(tester.changes.last()), int(QGraphicsItem::ItemTransformHasChanged));
        QCOMPARE(qVariantValue<QMatrix>(tester.values.at(tester.values.size() - 2)),
                 QMatrix().translate(50, 0));
        QCOMPARE(tester.values.last(), QVariant(QTransform(QMatrix().rotate(90))));
        QVariant variant;
        qVariantSetValue<QMatrix>(variant, QMatrix());
        QCOMPARE(tester.oldValues.last(), variant);
        QCOMPARE(tester.matrix(), QMatrix().rotate(90));
    }
    {
        tester.resetTransform();
        ++changeCount;
        ++changeCount; // notification sent too

        // ItemTransformChange / ItemTransformHasChanged
        qVariantSetValue<QTransform>(tester.itemChangeReturnValue, QTransform().rotate(90));
        tester.translate(50, 0);
        ++changeCount; // notification sent too
        ++changeCount;
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemTransformChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemTransformHasChanged);
        QCOMPARE(qVariantValue<QTransform>(tester.values.at(tester.values.size() - 2)),
                 QTransform().translate(50, 0));
        QCOMPARE(qVariantValue<QTransform>(tester.values.at(tester.values.size() - 1)),
                 QTransform().rotate(90));
        QVariant variant;
        qVariantSetValue<QTransform>(variant, QTransform());
        QCOMPARE(tester.oldValues.last(), variant);
        QCOMPARE(tester.transform(), QTransform().rotate(90));
    }
    {
        // ItemPositionChange / ItemPositionHasChanged
        tester.itemChangeReturnValue = QPointF(42, 0);
        tester.setPos(0, 42);
        ++changeCount; // notification sent too
        ++changeCount;
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemPositionChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemPositionHasChanged);
        QCOMPARE(tester.values.at(tester.changes.size() - 2), QVariant(QPointF(0, 42)));
        QCOMPARE(tester.values.at(tester.changes.size() - 1), QVariant(QPointF(42, 0)));
        QCOMPARE(tester.oldValues.last(), QVariant(QPointF()));
        QCOMPARE(tester.pos(), QPointF(42, 0));
    }
    {
        // ItemFlagsChange
        tester.itemChangeReturnValue = QGraphicsItem::ItemIsSelectable;
        tester.setFlag(QGraphicsItem::ItemIsSelectable, false);
        QCOMPARE(tester.changes.size(), changeCount);  // No change
        tester.setFlag(QGraphicsItem::ItemIsSelectable, true);
        ++changeCount;
        ++changeCount; // ItemFlagsHasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemFlagsChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemFlagsHaveChanged);
        QCOMPARE(tester.values.at(tester.values.size() - 2), qVariantFromValue<quint32>(QGraphicsItem::ItemIsSelectable));
        QCOMPARE(tester.values.at(tester.values.size() - 1), qVariantFromValue<quint32>(QGraphicsItem::ItemIsSelectable));
    }
    {
        // ItemSelectedChange
        tester.setSelected(false);
        QCOMPARE(tester.changes.size(), changeCount); // No change :-)
        tester.itemChangeReturnValue = true;
        tester.setSelected(true);
        ++changeCount;
        ++changeCount; // ItemSelectedHasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemSelectedChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemSelectedHasChanged);
        QCOMPARE(tester.values.at(tester.values.size() - 2), QVariant(true));
        QCOMPARE(tester.values.at(tester.values.size() - 1), QVariant(true));
        QCOMPARE(tester.oldValues.last(), QVariant(false));
        QCOMPARE(tester.isSelected(), true);

        tester.itemChangeReturnValue = false;
        tester.setSelected(true);

        // the value hasn't changed to the itemChange return value
        // bacause itemChange is never called (true -> true is a noop).
        QCOMPARE(tester.isSelected(), true);
    }
    {
        // ItemVisibleChange
        tester.itemChangeReturnValue = false;
        QVERIFY(tester.isVisible());
        tester.setVisible(false);
        ++changeCount; // ItemVisibleChange
        ++changeCount; // ItemSelectedChange
        ++changeCount; // ItemSelectedHasChanged
        ++changeCount; // ItemVisibleHasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 4), QGraphicsItem::ItemVisibleChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 3), QGraphicsItem::ItemSelectedChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemSelectedHasChanged);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemVisibleHasChanged);
        QCOMPARE(tester.values.at(tester.values.size() - 4), QVariant(false));
        QCOMPARE(tester.values.at(tester.values.size() - 3), QVariant(false));
        QCOMPARE(tester.values.at(tester.values.size() - 2), QVariant(false));
        QCOMPARE(tester.values.at(tester.values.size() - 1), QVariant(false));
        QCOMPARE(tester.isVisible(), false);
    }
    {
        // ItemParentChange
        qVariantSetValue<QGraphicsItem *>(tester.itemChangeReturnValue, 0);
        tester.setParentItem(&testerHelper);
        QCOMPARE(tester.changes.size(), ++changeCount);
        QCOMPARE(tester.changes.last(), QGraphicsItem::ItemParentChange);
        QCOMPARE(qVariantValue<QGraphicsItem *>(tester.values.last()), (QGraphicsItem *)&testerHelper);
        QCOMPARE(qVariantValue<QGraphicsItem *>(tester.oldValues.last()), (QGraphicsItem *)0);
        QCOMPARE(tester.parentItem(), (QGraphicsItem *)0);
    }
    {
        // ItemOpacityChange
        tester.itemChangeReturnValue = 1.0;
        tester.setOpacity(0.7);
        QCOMPARE(tester.changes.size(), ++changeCount);
        QCOMPARE(tester.changes.last(), QGraphicsItem::ItemOpacityChange);
        QVERIFY(qFuzzyCompare(qreal(tester.values.last().toDouble()), qreal(0.7)));
        QCOMPARE(tester.oldValues.last().toDouble(), double(1.0));
        QCOMPARE(tester.opacity(), qreal(1.0));
        tester.itemChangeReturnValue = 0.7;
        tester.setOpacity(0.7);
        ++changeCount; // ItemOpacityChange
        ++changeCount; // ItemOpacityHasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemOpacityChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemOpacityHasChanged);
        QCOMPARE(tester.opacity(), qreal(0.7));
    }
    {
        // ItemChildAddedChange
        tester.itemChangeReturnValue.clear();
        testerHelper.setParentItem(&tester);
        QCOMPARE(tester.changes.size(), ++changeCount);
        QCOMPARE(tester.changes.last(), QGraphicsItem::ItemChildAddedChange);
        QCOMPARE(qVariantValue<QGraphicsItem *>(tester.values.last()), (QGraphicsItem *)&testerHelper);
    }
    {
        // ItemChildRemovedChange 1
        testerHelper.setParentItem(0);
        QCOMPARE(tester.changes.size(), ++changeCount);
        QCOMPARE(tester.changes.last(), QGraphicsItem::ItemChildRemovedChange);
        QCOMPARE(qVariantValue<QGraphicsItem *>(tester.values.last()), (QGraphicsItem *)&testerHelper);

        // ItemChildRemovedChange 1
        ItemChangeTester *test = new ItemChangeTester;
        test->itemSceneChangeTargetScene = 0;
        int count = 0;
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addItem(test);
        count = test->changes.size();
        //We test here the fact that when a child is deleted the parent receive only one ItemChildRemovedChange
        QGraphicsRectItem *child = new QGraphicsRectItem(test);
        //We received ItemChildAddedChange
        QCOMPARE(test->changes.size(), ++count);
        QCOMPARE(test->changes.last(), QGraphicsItem::ItemChildAddedChange);
        delete child;
        child = 0;
        QCOMPARE(test->changes.size(), ++count);
        QCOMPARE(test->changes.last(), QGraphicsItem::ItemChildRemovedChange);

        ItemChangeTester *childTester = new ItemChangeTester(test);
        //Changes contains all sceneHasChanged and so on, we don't want to test that
        int childCount = childTester->changes.size();
        //We received ItemChildAddedChange
        QCOMPARE(test->changes.size(), ++count);
        child = new QGraphicsRectItem(childTester);
        //We received ItemChildAddedChange
        QCOMPARE(childTester->changes.size(), ++childCount);
        QCOMPARE(childTester->changes.last(), QGraphicsItem::ItemChildAddedChange);
        //Delete the child of the top level with all its children
        delete childTester;
        //Only one removal
        QCOMPARE(test->changes.size(), ++count);
        QCOMPARE(test->changes.last(), QGraphicsItem::ItemChildRemovedChange);
        delete scene;
    }
    {
        // ItemChildRemovedChange 2
        ItemChangeTester parent;
        ItemChangeTester *child = new ItemChangeTester;
        child->setParentItem(&parent);
        QCOMPARE(parent.changes.last(), QGraphicsItem::ItemChildAddedChange);
        QCOMPARE(qVariantValue<QGraphicsItem *>(parent.values.last()), (QGraphicsItem *)child);
        delete child;
        QCOMPARE(parent.changes.last(), QGraphicsItem::ItemChildRemovedChange);
        QCOMPARE(qVariantValue<QGraphicsItem *>(parent.values.last()), (QGraphicsItem *)child);
    }
    {
        // !!! Note: If this test crashes because of double-deletion, there's
        // a bug somewhere in QGraphicsScene or QGraphicsItem.

        // ItemSceneChange
        QGraphicsScene scene;
        QGraphicsScene scene2;
        scene.addItem(&tester);
        ++changeCount; // ItemSceneChange (scene)
        ++changeCount; // ItemSceneHasChanged (scene)
        QCOMPARE(tester.changes.size(), changeCount);

        QCOMPARE(tester.scene(), &scene);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemSceneChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemSceneHasChanged);
        // Item's old value was 0
        // Item's current value is scene
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.oldValues.last()), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.last()), (QGraphicsScene *)&scene);
        scene2.addItem(&tester);
        ++changeCount; // ItemSceneChange (0) was: (scene)
        ++changeCount; // ItemSceneHasChanged (0)
        ++changeCount; // ItemSceneChange (scene2) was: (0)
        ++changeCount; // ItemSceneHasChanged (scene2)
        QCOMPARE(tester.changes.size(), changeCount);

        QCOMPARE(tester.scene(), &scene2);
        QCOMPARE(tester.changes.at(tester.changes.size() - 4), QGraphicsItem::ItemSceneChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 3), QGraphicsItem::ItemSceneHasChanged);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemSceneChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemSceneHasChanged);
        // Item's last old value was scene
        // Item's last current value is 0

        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.oldValues.at(tester.oldValues.size() - 2)), (QGraphicsScene *)&scene);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.oldValues.at(tester.oldValues.size() - 1)), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 4)), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 3)), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 2)), (QGraphicsScene *)&scene2);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 1)), (QGraphicsScene *)&scene2);
        // Item's last old value was 0
        // Item's last current value is scene2
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.oldValues.last()), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.last()), (QGraphicsScene *)&scene2);

        scene2.removeItem(&tester);
        ++changeCount; // ItemSceneChange (0) was: (scene2)
        ++changeCount; // ItemSceneHasChanged (0)
        QCOMPARE(tester.changes.size(), changeCount);

        QCOMPARE(tester.scene(), (QGraphicsScene *)0);
        QCOMPARE(tester.changes.at(tester.changes.size() - 2), QGraphicsItem::ItemSceneChange);
        QCOMPARE(tester.changes.at(tester.changes.size() - 1), QGraphicsItem::ItemSceneHasChanged);
        // Item's last old value was scene2
        // Item's last current value is 0
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.oldValues.last()), (QGraphicsScene *)&scene2);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 2)), (QGraphicsScene *)0);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 1)), (QGraphicsScene *)0);

        tester.itemSceneChangeTargetScene = &scene;
        scene2.addItem(&tester);
        ++changeCount; // ItemSceneChange (scene2) was: (0)
        ++changeCount; // ItemSceneChange (scene) was: (0)
        ++changeCount; // ItemSceneHasChanged (scene)
        QCOMPARE(tester.values.size(), changeCount);

        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 3)), (QGraphicsScene *)&scene2);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 2)), (QGraphicsScene *)&scene);
        QCOMPARE(qVariantValue<QGraphicsScene *>(tester.values.at(tester.values.size() - 1)), (QGraphicsScene *)&scene);

        QCOMPARE(tester.scene(), &scene);
        tester.itemSceneChangeTargetScene = 0;
        tester.itemChangeReturnValue = QVariant();
        scene.removeItem(&tester);
        ++changeCount; // ItemSceneChange
        ++changeCount; // ItemSceneHasChanged
        QCOMPARE(tester.scene(), (QGraphicsScene *)0);
    }
    {
        // ItemToolTipChange/ItemToolTipHasChanged
        const QString toolTip(QLatin1String("I'm soo cool"));
        const QString overridenToolTip(QLatin1String("No, you are not soo cool"));
        tester.itemChangeReturnValue = overridenToolTip;
        tester.setToolTip(toolTip);
        ++changeCount; // ItemToolTipChange
        ++changeCount; // ItemToolTipHasChanged
        QCOMPARE(tester.changes.size(), changeCount);
        QCOMPARE(tester.changes.at(changeCount - 2), QGraphicsItem::ItemToolTipChange);
        QCOMPARE(tester.values.at(changeCount - 2).toString(), toolTip);
        QCOMPARE(tester.changes.at(changeCount - 1), QGraphicsItem::ItemToolTipHasChanged);
        QCOMPARE(tester.values.at(changeCount - 1).toString(), overridenToolTip);
        QCOMPARE(tester.toolTip(), overridenToolTip);
        tester.itemChangeReturnValue = QVariant();
    }
}

class EventFilterTesterItem : public QGraphicsLineItem
{
public:
    QList<QEvent::Type> filteredEvents;
    QList<QGraphicsItem *> filteredEventReceivers;
    bool handlesSceneEvents;

    QList<QEvent::Type> receivedEvents;

    EventFilterTesterItem() : handlesSceneEvents(false) {}

protected:
    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event)
    {
        filteredEvents << event->type();
        filteredEventReceivers << watched;
        return handlesSceneEvents;
    }

    bool sceneEvent(QEvent *event)
    {
        return QGraphicsLineItem::sceneEvent(event);
    }
};

void tst_QGraphicsItem::sceneEventFilter()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();

    QGraphicsTextItem *text1 = scene.addText(QLatin1String("Text1"));
    QGraphicsTextItem *text2 = scene.addText(QLatin1String("Text2"));
    QGraphicsTextItem *text3 = scene.addText(QLatin1String("Text3"));
    text1->setFlag(QGraphicsItem::ItemIsFocusable);
    text2->setFlag(QGraphicsItem::ItemIsFocusable);
    text3->setFlag(QGraphicsItem::ItemIsFocusable);

    EventFilterTesterItem *tester = new EventFilterTesterItem;
    scene.addItem(tester);

    QVERIFY(!text1->hasFocus());
    text1->installSceneEventFilter(tester);
    text1->setFocus();
    QVERIFY(text1->hasFocus());

    QCOMPARE(tester->filteredEvents.size(), 1);
    QCOMPARE(tester->filteredEvents.at(0), QEvent::FocusIn);
    QCOMPARE(tester->filteredEventReceivers.at(0), static_cast<QGraphicsItem *>(text1));

    text2->installSceneEventFilter(tester);
    text3->installSceneEventFilter(tester);

    text2->setFocus();
    text3->setFocus();

    QCOMPARE(tester->filteredEvents.size(), 5);
    QCOMPARE(tester->filteredEvents.at(1), QEvent::FocusOut);
    QCOMPARE(tester->filteredEventReceivers.at(1), static_cast<QGraphicsItem *>(text1));
    QCOMPARE(tester->filteredEvents.at(2), QEvent::FocusIn);
    QCOMPARE(tester->filteredEventReceivers.at(2), static_cast<QGraphicsItem *>(text2));
    QCOMPARE(tester->filteredEvents.at(3), QEvent::FocusOut);
    QCOMPARE(tester->filteredEventReceivers.at(3), static_cast<QGraphicsItem *>(text2));
    QCOMPARE(tester->filteredEvents.at(4), QEvent::FocusIn);
    QCOMPARE(tester->filteredEventReceivers.at(4), static_cast<QGraphicsItem *>(text3));

    text1->removeSceneEventFilter(tester);
    text1->setFocus();

    QCOMPARE(tester->filteredEvents.size(), 6);
    QCOMPARE(tester->filteredEvents.at(5), QEvent::FocusOut);
    QCOMPARE(tester->filteredEventReceivers.at(5), static_cast<QGraphicsItem *>(text3));

    tester->handlesSceneEvents = true;
    text2->setFocus();

    QCOMPARE(tester->filteredEvents.size(), 7);
    QCOMPARE(tester->filteredEvents.at(6), QEvent::FocusIn);
    QCOMPARE(tester->filteredEventReceivers.at(6), static_cast<QGraphicsItem *>(text2));

    QVERIFY(text2->hasFocus());

    //Let check if the items are correctly removed from the sceneEventFilters array
    //to avoid stale pointers.
    QGraphicsView gv;
    QGraphicsScene *anotherScene = new QGraphicsScene;
    QGraphicsTextItem *ti = anotherScene->addText("This is a test #1");
    ti->moveBy(50, 50);
    QGraphicsTextItem *ti2 = anotherScene->addText("This is a test #2");
    QGraphicsTextItem *ti3 = anotherScene->addText("This is a test #3");
    gv.setScene(anotherScene);
    gv.show();
    QTest::qWait(250);
    ti->installSceneEventFilter(ti2);
    ti3->installSceneEventFilter(ti);
    delete ti2;
    //we souldn't crash
    QTest::mouseMove(gv.viewport(), gv.mapFromScene(ti->scenePos()));
    QTest::qWait(250);
    delete ti;
}

class GeometryChanger : public QGraphicsItem
{
public:
    void changeGeometry()
    { prepareGeometryChange(); }
};

void tst_QGraphicsItem::prepareGeometryChange()
{
    {
        QGraphicsScene scene;
        QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));
        QVERIFY(scene.items(QRectF(0, 0, 100, 100)).contains(item));
        ((GeometryChanger *)item)->changeGeometry();
        QVERIFY(scene.items(QRectF(0, 0, 100, 100)).contains(item));
    }
}


class PaintTester : public QGraphicsRectItem
{
public:
    PaintTester() : widget(NULL), painted(0) { setRect(QRectF(10, 10, 20, 20));}

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *w)
    {
        widget = w;
        painted++;
    }

    QWidget*  widget;
    int painted;
};

void tst_QGraphicsItem::paint()
{
    QGraphicsScene scene;

    PaintTester paintTester;
    scene.addItem(&paintTester);

    QGraphicsView view(&scene);

    view.show();
    QTest::qWait(250);

#ifdef Q_OS_WIN32
    //we try to switch the desktop: if it fails, we skip the test
    if (::SwitchDesktop( ::GetThreadDesktop( ::GetCurrentThreadId() ) ) == 0) {
        QSKIP("The Graphics View doesn't get the paint events", SkipSingle);
    }
#endif

    QVERIFY(paintTester.widget == view.viewport());


    view.hide();

    QGraphicsScene scene2;

    PaintTester tester2;
    scene2.addItem(&tester2);

    QGraphicsView view2(&scene2);
    view2.show();
    qApp->processEvents();
    QTest::qWait(250);

    //First show one paint
    QVERIFY(tester2.painted == 1);

    //nominal case, update call paint
    tester2.update();
    qApp->processEvents();
    QTest::qWait(250);
    QVERIFY(tester2.painted == 2);

    //we remove the item from the scene, number of updates is still the same
    tester2.update();
    scene2.removeItem(&tester2);
    qApp->processEvents();
    QTest::qWait(250);
    QVERIFY(tester2.painted == 2);

    //We re-add the item, the number of paint should increase
    scene2.addItem(&tester2);
    tester2.update();
    qApp->processEvents();
    QTest::qWait(250);
    QVERIFY(tester2.painted == 3);
}

class HarakiriItem : public QGraphicsRectItem
{
public:
    HarakiriItem(int harakiriPoint)
        : QGraphicsRectItem(QRectF(0, 0, 100, 100)), harakiri(harakiriPoint)
    { dead = 0; }

    static int dead;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QGraphicsRectItem::paint(painter, option, widget);
        if (harakiri == 0) {
	    // delete unsupported since 4.5
	    /*
            dead = 1;
	    delete this;
	    */
        }
    }

    void advance(int n)
    {
        if (harakiri == 1 && n == 0) {
            // delete unsupported
            /*
            dead = 1;
            delete this;
            */
        }
        if (harakiri == 2 && n == 1) {
            dead = 1;
            delete this;
        }
    }

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *)
    {
        if (harakiri == 3) {
            dead = 1;
            delete this;
        }
    }

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event)
    {
        // ??
        QGraphicsRectItem::dragEnterEvent(event);
    }

    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
    {
        // ??
        QGraphicsRectItem::dragLeaveEvent(event);
    }

    void dragMoveEvent(QGraphicsSceneDragDropEvent *event)
    {
        // ??
        QGraphicsRectItem::dragMoveEvent(event);
    }

    void dropEvent(QGraphicsSceneDragDropEvent *event)
    {
        // ??
        QGraphicsRectItem::dropEvent(event);
    }

    void focusInEvent(QFocusEvent *)
    {
        if (harakiri == 4) {
            dead = 1;
            delete this;
        }
    }

    void focusOutEvent(QFocusEvent *)
    {
        if (harakiri == 5) {
            dead = 1;
            delete this;
        }
    }

    void hoverEnterEvent(QGraphicsSceneHoverEvent *)
    {
        if (harakiri == 6) {
            dead = 1;
            delete this;
        }
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *)
    {
        if (harakiri == 7) {
            dead = 1;
            delete this;
        }
    }

    void hoverMoveEvent(QGraphicsSceneHoverEvent *)
    {
        if (harakiri == 8) {
            dead = 1;
            delete this;
        }
    }

    void inputMethodEvent(QInputMethodEvent *event)
    {
        // ??
        QGraphicsRectItem::inputMethodEvent(event);
    }

    QVariant inputMethodQuery(Qt::InputMethodQuery query) const
    {
        // ??
        return QGraphicsRectItem::inputMethodQuery(query);
    }

    QVariant itemChange(GraphicsItemChange change, const QVariant &value)
    {
        // deletion not supported
        return QGraphicsRectItem::itemChange(change, value);
    }

    void keyPressEvent(QKeyEvent *)
    {
        if (harakiri == 9) {
            dead = 1;
            delete this;
        }
    }

    void keyReleaseEvent(QKeyEvent *)
    {
        if (harakiri == 10) {
            dead = 1;
            delete this;
        }
    }

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *)
    {
        if (harakiri == 11) {
            dead = 1;
            delete this;
        }
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *)
    {
        if (harakiri == 12) {
            dead = 1;
            delete this;
        }
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *)
    {
        if (harakiri == 13) {
            dead = 1;
            delete this;
        }
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
    {
        if (harakiri == 14) {
            dead = 1;
            delete this;
        }
    }

    bool sceneEvent(QEvent *event)
    {
        // deletion not supported
        return QGraphicsRectItem::sceneEvent(event);
    }

    bool sceneEventFilter(QGraphicsItem *watched, QEvent *event)
    {
        // deletion not supported
        return QGraphicsRectItem::sceneEventFilter(watched, event);
    }

    void wheelEvent(QGraphicsSceneWheelEvent *)
    {
        if (harakiri == 16) {
            dead = 1;
            delete this;
        }
    }

private:
    int harakiri;
};

int HarakiriItem::dead;

void tst_QGraphicsItem::deleteItemInEventHandlers()
{
    for (int i = 0; i < 17; ++i) {
        QGraphicsScene scene;
        HarakiriItem *item = new HarakiriItem(i);
        item->setAcceptsHoverEvents(true);
        item->setFlag(QGraphicsItem::ItemIsFocusable);

        scene.addItem(item);

        item->installSceneEventFilter(item); // <- ehey!

        QGraphicsView view(&scene);
        view.show();

        qApp->processEvents();
        qApp->processEvents();

        if (!item->dead)
            scene.advance();

        if (!item->dead) {
            QContextMenuEvent event(QContextMenuEvent::Other,
                                    view.mapFromScene(item->scenePos()));
            QCoreApplication::sendEvent(view.viewport(), &event);
        }
        if (!item->dead)
            QTest::mouseMove(view.viewport(), view.mapFromScene(item->scenePos()));
        if (!item->dead)
            QTest::mouseClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item->scenePos()));
        if (!item->dead)
            QTest::mouseDClick(view.viewport(), Qt::LeftButton, 0, view.mapFromScene(item->scenePos()));
        if (!item->dead)
            QTest::mouseClick(view.viewport(), Qt::RightButton, 0, view.mapFromScene(item->scenePos()));
        if (!item->dead)
            QTest::mouseMove(view.viewport(), view.mapFromScene(item->scenePos() + QPointF(20, -20)));
        if (!item->dead)
            item->setFocus();
        if (!item->dead)
            item->clearFocus();
        if (!item->dead)
            item->setFocus();
        if (!item->dead)
            QTest::keyPress(view.viewport(), Qt::Key_A);
        if (!item->dead)
            QTest::keyRelease(view.viewport(), Qt::Key_A);
        if (!item->dead)
            QTest::keyPress(view.viewport(), Qt::Key_A);
        if (!item->dead)
            QTest::keyRelease(view.viewport(), Qt::Key_A);
    }
}

class ItemPaintsOutsideShape : public QGraphicsItem
{
public:
    QRectF boundingRect() const
    {
        return QRectF(0, 0, 100, 100);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        painter->fillRect(-50, -50, 200, 200, Qt::red);
        painter->fillRect(0, 0, 100, 100, Qt::blue);
    }
};

void tst_QGraphicsItem::itemClipsToShape()
{
    QGraphicsItem *clippedItem = new ItemPaintsOutsideShape;
    clippedItem->setFlag(QGraphicsItem::ItemClipsToShape);

    QGraphicsItem *unclippedItem = new ItemPaintsOutsideShape;
    unclippedItem->setPos(200, 0);

    QGraphicsScene scene(-50, -50, 400, 200);
    scene.addItem(clippedItem);
    scene.addItem(unclippedItem);

    QImage image(400, 200, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(45, 100), QRgb(0));
    QCOMPARE(image.pixel(100, 45), QRgb(0));
    QCOMPARE(image.pixel(155, 100), QRgb(0));
    QCOMPARE(image.pixel(45, 155), QRgb(0));
    QCOMPARE(image.pixel(55, 100), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(100, 55), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(145, 100), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(55, 145), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(245, 100), QColor(Qt::red).rgba());
    QCOMPARE(image.pixel(300, 45), QColor(Qt::red).rgba());
    QCOMPARE(image.pixel(355, 100), QColor(Qt::red).rgba());
    QCOMPARE(image.pixel(245, 155), QColor(Qt::red).rgba());
    QCOMPARE(image.pixel(255, 100), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(300, 55), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(345, 100), QColor(Qt::blue).rgba());
    QCOMPARE(image.pixel(255, 145), QColor(Qt::blue).rgba());
}

void tst_QGraphicsItem::itemClipsChildrenToShape()
{
    QGraphicsScene scene;
    QGraphicsItem *rect = scene.addRect(0, 0, 50, 50, QPen(Qt::NoPen), QBrush(Qt::yellow));

    QGraphicsItem *ellipse = scene.addEllipse(0, 0, 100, 100, QPen(Qt::NoPen), QBrush(Qt::green));
    ellipse->setParentItem(rect);

    QGraphicsItem *clippedEllipse = scene.addEllipse(0, 0, 50, 50, QPen(Qt::NoPen), QBrush(Qt::blue));
    clippedEllipse->setParentItem(ellipse);

    QGraphicsItem *clippedEllipse2 = scene.addEllipse(0, 0, 25, 25, QPen(Qt::NoPen), QBrush(Qt::red));
    clippedEllipse2->setParentItem(clippedEllipse);

    QGraphicsItem *clippedEllipse3 = scene.addEllipse(50, 50, 25, 25, QPen(Qt::NoPen), QBrush(Qt::red));
    clippedEllipse3->setParentItem(clippedEllipse);

    QVERIFY(!(ellipse->flags() & QGraphicsItem::ItemClipsChildrenToShape));
    ellipse->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QVERIFY((ellipse->flags() & QGraphicsItem::ItemClipsChildrenToShape));

    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    scene.render(&painter);
    painter.end();

    QCOMPARE(image.pixel(16, 16), QColor(255, 0, 0).rgba());
    QCOMPARE(image.pixel(32, 32), QColor(0, 0, 255).rgba());
    QCOMPARE(image.pixel(50, 50), QColor(0, 255, 0).rgba());
    QCOMPARE(image.pixel(12, 12), QColor(255, 255, 0).rgba());
    QCOMPARE(image.pixel(60, 60), QColor(255, 0, 0).rgba());
}

void tst_QGraphicsItem::itemClipsChildrenToShape2()
{
    QGraphicsRectItem *parent = new QGraphicsRectItem(QRectF(0, 0, 10, 10));
    QGraphicsEllipseItem *child1 = new QGraphicsEllipseItem(QRectF(50, 50, 100, 100));
    QGraphicsRectItem *child2 = new QGraphicsRectItem(QRectF(15, 15, 80, 80));

    child1->setParentItem(parent);
    child1->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    child2->setParentItem(child1);

    parent->setBrush(Qt::blue);
    child1->setBrush(Qt::green);
    child2->setBrush(Qt::red);

    QGraphicsScene scene;
    scene.addItem(parent);

    QCOMPARE(scene.itemAt(5, 5), (QGraphicsItem *)parent);
    QCOMPARE(scene.itemAt(15, 5), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(5, 15), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(60, 60), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(140, 60), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(60, 140), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(140, 140), (QGraphicsItem *)0);
    QCOMPARE(scene.itemAt(75, 75), (QGraphicsItem *)child2);
    QCOMPARE(scene.itemAt(75, 100), (QGraphicsItem *)child1);
    QCOMPARE(scene.itemAt(100, 75), (QGraphicsItem *)child1);

#if 1
    QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    scene.render(&painter);
    painter.end();
    
    QCOMPARE(image.pixel(5, 5), QColor(0, 0, 255).rgba());
    QCOMPARE(image.pixel(5, 10), QRgb(0));
    QCOMPARE(image.pixel(10, 5), QRgb(0));
    QCOMPARE(image.pixel(40, 40), QRgb(0));
    QCOMPARE(image.pixel(90, 40), QRgb(0));
    QCOMPARE(image.pixel(40, 90), QRgb(0));
    QCOMPARE(image.pixel(95, 95), QRgb(0));
    QCOMPARE(image.pixel(50, 70), QColor(0, 255, 0).rgba());
    QCOMPARE(image.pixel(70, 50), QColor(0, 255, 0).rgba());
    QCOMPARE(image.pixel(50, 60), QColor(255, 0, 0).rgba());
    QCOMPARE(image.pixel(60, 50), QColor(255, 0, 0).rgba());
#else
    QGraphicsView view(&scene);
    view.show();
    QTest::qWait(5000);
#endif
}

void tst_QGraphicsItem::itemClipsTextChildToShape()
{
    // Construct a scene with a rect that clips its children, with one text
    // child that has text that exceeds the size of the rect.
    QGraphicsScene scene;
    QGraphicsItem *rect = scene.addRect(0, 0, 50, 50, QPen(Qt::black), Qt::black);
    rect->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QGraphicsTextItem *text = new QGraphicsTextItem("This is a long sentence that's wider than 50 pixels.");
    text->setParentItem(rect);

    // Render this scene to a transparent image.
    QRectF sr = scene.itemsBoundingRect();
    QImage image(sr.size().toSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter painter(&image);
    scene.render(&painter);

    // Erase the area immediately underneath the rect.
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect->sceneBoundingRect().translated(-sr.topLeft()).adjusted(-0.5, -0.5, 0.5, 0.5),
                     Qt::transparent);
    painter.end();

    // Check that you get a truly transparent image back (i.e., the text was
    // clipped away, so there should be no trails left after erasing only the
    // rect's area).
    QImage emptyImage(scene.itemsBoundingRect().size().toSize(), QImage::Format_ARGB32_Premultiplied);
    emptyImage.fill(0);
    QCOMPARE(image, emptyImage);
}

void tst_QGraphicsItem::itemClippingDiscovery()
{
    // A simple scene with an ellipse parent and two rect children, one a
    // child of the other.
    QGraphicsScene scene;
    QGraphicsEllipseItem *clipItem = scene.addEllipse(0, 0, 100, 100);
    QGraphicsRectItem *leftRectItem = scene.addRect(0, 0, 50, 100);
    QGraphicsRectItem *rightRectItem = scene.addRect(50, 0, 50, 100);
    leftRectItem->setParentItem(clipItem);
    rightRectItem->setParentItem(clipItem);

    // The rects item are both visible at these points.
    QCOMPARE(scene.itemAt(10, 10), (QGraphicsItem *)leftRectItem);
    QCOMPARE(scene.itemAt(90, 90), (QGraphicsItem *)rightRectItem);

    // The ellipse clips the rects now.
    clipItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape);

    // The rect items are no longer visible at these points.
    QCOMPARE(scene.itemAt(10, 10), (QGraphicsItem *)0);
    if (sizeof(qreal) != sizeof(double))
        QSKIP("This fails due to internal rounding errors", SkipSingle);
    QCOMPARE(scene.itemAt(90, 90), (QGraphicsItem *)0);
}

void tst_QGraphicsItem::ancestorFlags()
{
    QGraphicsItem *level1 = new QGraphicsRectItem;
    QGraphicsItem *level21 = new QGraphicsRectItem;
    level21->setParentItem(level1);
    QGraphicsItem *level22 = new QGraphicsRectItem;
    level22->setParentItem(level1);
    QGraphicsItem *level31 = new QGraphicsRectItem;
    level31->setParentItem(level21);
    QGraphicsItem *level32 = new QGraphicsRectItem;
    level32->setParentItem(level21);

    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 0);

    // HandlesChildEvents: 1) Root level sets a flag
    level1->setHandlesChildEvents(true);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // HandlesChildEvents: 2) Root level set it again
    level1->setHandlesChildEvents(true);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // HandlesChildEvents: 3) Root level unsets a flag
    level1->setHandlesChildEvents(false);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 0);

    // HandlesChildEvents: 4) Child item sets a flag
    level21->setHandlesChildEvents(true);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // HandlesChildEvents: 5) Parent item sets a flag
    level1->setHandlesChildEvents(true);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // HandlesChildEvents: 6) Child item unsets a flag
    level21->setHandlesChildEvents(false);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // HandlesChildEvents: 7) Parent item unsets a flag
    level21->setHandlesChildEvents(true);
    level1->setHandlesChildEvents(false);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Reparent the child to root
    level21->setParentItem(0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Reparent the child to level1 again.
    level1->setHandlesChildEvents(true);
    level21->setParentItem(level1);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Reparenting level31 back to level1.
    level31->setParentItem(level1);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Reparenting level31 back to level21.
    level31->setParentItem(0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
    level31->setParentItem(level21);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Level1 doesn't handle child events
    level1->setHandlesChildEvents(false);
    QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
    QCOMPARE(int(level31->d_ptr->ancestorFlags), 1);
    QCOMPARE(int(level32->d_ptr->ancestorFlags), 1);

    // Nobody handles child events
    level21->setHandlesChildEvents(false);

    for (int i = 0; i < 2; ++i) {
        QGraphicsItem::GraphicsItemFlag flag = !i ? QGraphicsItem::ItemClipsChildrenToShape
                                               : QGraphicsItem::ItemIgnoresTransformations;
        int ancestorFlag = !i ? QGraphicsItemPrivate::AncestorClipsChildren
                           : QGraphicsItemPrivate::AncestorIgnoresTransformations;

        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), 0);

        // HandlesChildEvents: 1) Root level sets a flag
        level1->setFlag(flag, true);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // HandlesChildEvents: 2) Root level set it again
        level1->setFlag(flag, true);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // HandlesChildEvents: 3) Root level unsets a flag
        level1->setFlag(flag, false);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), 0);

        // HandlesChildEvents: 4) Child item sets a flag
        level21->setFlag(flag, true);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // HandlesChildEvents: 5) Parent item sets a flag
        level1->setFlag(flag, true);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // HandlesChildEvents: 6) Child item unsets a flag
        level21->setFlag(flag, false);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // HandlesChildEvents: 7) Parent item unsets a flag
        level21->setFlag(flag, true);
        level1->setFlag(flag, false);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Reparent the child to root
        level21->setParentItem(0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Reparent the child to level1 again.
        level1->setFlag(flag, true);
        level21->setParentItem(level1);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Reparenting level31 back to level1.
        level31->setParentItem(level1);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Reparenting level31 back to level21.
        level31->setParentItem(0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
        level31->setParentItem(level21);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Level1 doesn't handle child events
        level1->setFlag(flag, false);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), ancestorFlag);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), ancestorFlag);

        // Nobody handles child events
        level21->setFlag(flag, false);
        QCOMPARE(int(level1->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level21->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level22->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level31->d_ptr->ancestorFlags), 0);
        QCOMPARE(int(level32->d_ptr->ancestorFlags), 0);
    }

    delete level1;
}

void tst_QGraphicsItem::untransformable()
{
    QGraphicsItem *item1 = new QGraphicsEllipseItem(QRectF(-50, -50, 100, 100));
    item1->setZValue(1);
    item1->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    item1->rotate(45);
    ((QGraphicsEllipseItem *)item1)->setBrush(Qt::red);

    QGraphicsItem *item2 = new QGraphicsEllipseItem(QRectF(-50, -50, 100, 100));
    item2->setParentItem(item1);
    item2->rotate(45);
    item2->setPos(100, 0);
    ((QGraphicsEllipseItem *)item2)->setBrush(Qt::green);

    QGraphicsItem *item3 = new QGraphicsEllipseItem(QRectF(-50, -50, 100, 100));
    item3->setParentItem(item2);
    item3->setPos(100, 0);
    ((QGraphicsEllipseItem *)item3)->setBrush(Qt::blue);

    QGraphicsScene scene(-500, -500, 1000, 1000);
    scene.addItem(item1);

    QGraphicsView view(&scene);
    view.resize(300, 300);
    view.show();
    view.scale(8, 8);
    view.centerOn(0, 0);

// Painting with the DiagCrossPattern is really slow on Mac
// when zoomed out. (The test times out). Task to fix is 155567.
#if !defined(Q_WS_MAC) || 1
    view.setBackgroundBrush(QBrush(Qt::black, Qt::DiagCrossPattern));
#endif

    for (int i = 0; i < 10; ++i) {
        QPoint center = view.viewport()->rect().center();
        QCOMPARE(view.itemAt(center), item1);
        QCOMPARE(view.itemAt(center - QPoint(40, 0)), item1);
        QCOMPARE(view.itemAt(center - QPoint(-40, 0)), item1);
        QCOMPARE(view.itemAt(center - QPoint(0, 40)), item1);
        QCOMPARE(view.itemAt(center - QPoint(0, -40)), item1);

        center += QPoint(70, 70);
        QCOMPARE(view.itemAt(center - QPoint(40, 0)), item2);
        QCOMPARE(view.itemAt(center - QPoint(-40, 0)), item2);
        QCOMPARE(view.itemAt(center - QPoint(0, 40)), item2);
        QCOMPARE(view.itemAt(center - QPoint(0, -40)), item2);

        center += QPoint(0, 100);
        QCOMPARE(view.itemAt(center - QPoint(40, 0)), item3);
        QCOMPARE(view.itemAt(center - QPoint(-40, 0)), item3);
        QCOMPARE(view.itemAt(center - QPoint(0, 40)), item3);
        QCOMPARE(view.itemAt(center - QPoint(0, -40)), item3);

        view.scale(0.5, 0.5);
        view.rotate(13);
        view.shear(qreal(0.01), qreal(0.01));
        view.translate(10, 10);
        QTest::qWait(250);
    }
}

class ContextMenuItem : public QGraphicsRectItem
{
public:
    ContextMenuItem()
        : ignoreEvent(true), gotEvent(false), eventWasAccepted(false)
    { }
    bool ignoreEvent;
    bool gotEvent;
    bool eventWasAccepted;
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
    {
        gotEvent = true;
        eventWasAccepted = event->isAccepted();
        if (ignoreEvent)
            event->ignore();
    }
};

void tst_QGraphicsItem::contextMenuEventPropagation()
{
    ContextMenuItem *bottomItem = new ContextMenuItem;
    bottomItem->setRect(0, 0, 100, 100);
    ContextMenuItem *topItem = new ContextMenuItem;
    topItem->setParentItem(bottomItem);
    topItem->setRect(0, 0, 100, 100);

    QGraphicsScene scene;

    QGraphicsView view(&scene);
    view.setAlignment(Qt::AlignLeft | Qt::AlignTop);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    view.resize(200, 200);

    QContextMenuEvent event(QContextMenuEvent::Mouse, QPoint(10, 10),
                            view.viewport()->mapToGlobal(QPoint(10, 10)));
    event.ignore();
    QApplication::sendEvent(view.viewport(), &event);
    QVERIFY(!event.isAccepted());

    scene.addItem(bottomItem);
    topItem->ignoreEvent = true;
    bottomItem->ignoreEvent = true;

    QApplication::sendEvent(view.viewport(), &event);
    QVERIFY(!event.isAccepted());
    QCOMPARE(topItem->gotEvent, true);
    QCOMPARE(topItem->eventWasAccepted, true);
    QCOMPARE(bottomItem->gotEvent, true);
    QCOMPARE(bottomItem->eventWasAccepted, true);

    topItem->ignoreEvent = false;
    topItem->gotEvent = false;
    bottomItem->gotEvent = false;

    QApplication::sendEvent(view.viewport(), &event);
    QVERIFY(event.isAccepted());
    QCOMPARE(topItem->gotEvent, true);
    QCOMPARE(bottomItem->gotEvent, false);
    QCOMPARE(topItem->eventWasAccepted, true);
}

void tst_QGraphicsItem::itemIsMovable()
{
    QGraphicsRectItem *rect = new QGraphicsRectItem(-50, -50, 100, 100);
    rect->setFlag(QGraphicsItem::ItemIsMovable);

    QGraphicsScene scene;
    scene.addItem(rect);

    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        qApp->sendEvent(&scene, &event);
    }
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(rect->pos(), QPointF(0, 0));
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
        event.setButtons(Qt::LeftButton);
        event.setScenePos(QPointF(10, 10));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(rect->pos(), QPointF(10, 10));
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
        event.setButtons(Qt::RightButton);
        event.setScenePos(QPointF(20, 20));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(rect->pos(), QPointF(10, 10));
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseMove);
        event.setButtons(Qt::LeftButton);
        event.setScenePos(QPointF(30, 30));
        qApp->sendEvent(&scene, &event);
    }
    QCOMPARE(rect->pos(), QPointF(30, 30));
}

class ItemAddScene : public QGraphicsScene
{
    Q_OBJECT
public:
    ItemAddScene()
    {
        QTimer::singleShot(500, this, SLOT(newTextItem()));
    }

public slots:
    void newTextItem()
    {
        // Add a text item
        QGraphicsItem *item = new QGraphicsTextItem("This item will not ensure that it's visible", 0, this);
        item->setPos(.0, .0);
        item->show();
    }
};

void tst_QGraphicsItem::task141694_textItemEnsureVisible()
{
    ItemAddScene scene;
    scene.setSceneRect(-1000, -1000, 2000, 2000);

    QGraphicsView view(&scene);
    view.setFixedSize(200, 200);
    view.show();

    view.ensureVisible(-1000, -1000, 5, 5);
    int hscroll = view.horizontalScrollBar()->value();
    int vscroll = view.verticalScrollBar()->value();

    QTestEventLoop::instance().enterLoop(1);

    // This should not cause the view to scroll
    QCOMPARE(view.horizontalScrollBar()->value(), hscroll);
    QCOMPARE(view.verticalScrollBar()->value(), vscroll);
}

void tst_QGraphicsItem::task128696_textItemEnsureMovable()
{
    QGraphicsTextItem *item = new QGraphicsTextItem;
    item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
    item->setPlainText("abc de\nf ghi\n   j k l");

    QGraphicsScene scene;
    scene.setSceneRect(-100, -100, 200, 200);
    scene.addItem(item);

    QGraphicsView view(&scene);
    view.setFixedSize(200, 200);
    view.show();

    QGraphicsSceneMouseEvent event1(QEvent::GraphicsSceneMousePress);
    event1.setScenePos(QPointF(0, 0));
    event1.setButton(Qt::LeftButton);
    event1.setButtons(Qt::LeftButton);
    QApplication::sendEvent(&scene, &event1);
    QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);

    QGraphicsSceneMouseEvent event2(QEvent::GraphicsSceneMouseMove);
    event2.setScenePos(QPointF(10, 10));
    event2.setButton(Qt::LeftButton);
    event2.setButtons(Qt::LeftButton);
    QApplication::sendEvent(&scene, &event2);
    QCOMPARE(item->pos(), QPointF(10, 10));
}

void tst_QGraphicsItem::task177918_lineItemUndetected()
{
    QGraphicsScene scene;
    QGraphicsLineItem *line = scene.addLine(10, 10, 10, 10);
    QCOMPARE(line->boundingRect(), QRectF(10, 10, 0, 0));

    QVERIFY(!scene.items(9, 9, 2, 2, Qt::IntersectsItemShape).isEmpty());
    QVERIFY(!scene.items(9, 9, 2, 2, Qt::ContainsItemShape).isEmpty());
    QVERIFY(!scene.items(9, 9, 2, 2, Qt::IntersectsItemBoundingRect).isEmpty());
    QVERIFY(!scene.items(9, 9, 2, 2, Qt::ContainsItemBoundingRect).isEmpty());
}

void tst_QGraphicsItem::task240400_clickOnTextItem_data()
{
    QTest::addColumn<int>("flags");
    QTest::addColumn<int>("textFlags");
    QTest::newRow("editor, noflags") << 0 << int(Qt::TextEditorInteraction);
    QTest::newRow("editor, movable") << int(QGraphicsItem::ItemIsMovable) << int(Qt::TextEditorInteraction);
    QTest::newRow("editor, selectable") << int(QGraphicsItem::ItemIsSelectable) << int(Qt::TextEditorInteraction);
    QTest::newRow("editor, movable | selectable") << int(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable)
                                                  << int(Qt::TextEditorInteraction);
    QTest::newRow("noninteractive, noflags") << 0 << int(Qt::NoTextInteraction);
    QTest::newRow("noninteractive, movable") << int(QGraphicsItem::ItemIsMovable) << int(Qt::NoTextInteraction);
    QTest::newRow("noninteractive, selectable") << int(QGraphicsItem::ItemIsSelectable) << int(Qt::NoTextInteraction);
    QTest::newRow("noninteractive, movable | selectable") << int(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable)
                                                          << int(Qt::NoTextInteraction);
}

void tst_QGraphicsItem::task240400_clickOnTextItem()
{
    QFETCH(int, flags);
    QFETCH(int, textFlags);

    QGraphicsScene scene;
    QGraphicsTextItem *item = scene.addText("Hello");
    item->setFlags(QGraphicsItem::GraphicsItemFlags(flags));
    item->setTextInteractionFlags(Qt::TextInteractionFlags(textFlags));
    bool focusable = (item->flags() & QGraphicsItem::ItemIsFocusable);
    QVERIFY(textFlags ? focusable : !focusable);

    int column = item->textCursor().columnNumber();
    QCOMPARE(column, 0);

    QVERIFY(!item->hasFocus());

    // Click in the top-left corner of the item
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setScenePos(item->sceneBoundingRect().topLeft() + QPointF(0.1, 0.1));
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event);
    }
    if (flags || textFlags)
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
    else
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.setScenePos(item->sceneBoundingRect().topLeft() + QPointF(0.1, 0.1));
        event.setButton(Qt::LeftButton);
        event.setButtons(0);
        QApplication::sendEvent(&scene, &event);
    }
    if (textFlags)
        QVERIFY(item->hasFocus());
    else
        QVERIFY(!item->hasFocus());
    QVERIFY(!scene.mouseGrabberItem());
    bool selectable = (flags & QGraphicsItem::ItemIsSelectable);
    QVERIFY(selectable ? item->isSelected() : !item->isSelected());

    // Now click in the middle and check that the cursor moved.
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
        event.setScenePos(item->sceneBoundingRect().center());
        event.setButton(Qt::LeftButton);
        event.setButtons(Qt::LeftButton);
        QApplication::sendEvent(&scene, &event);
    }
    if (flags || textFlags)
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)item);
    else
        QCOMPARE(scene.mouseGrabberItem(), (QGraphicsItem *)0);
    {
        QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMouseRelease);
        event.setScenePos(item->sceneBoundingRect().center());
        event.setButton(Qt::LeftButton);
        event.setButtons(0);
        QApplication::sendEvent(&scene, &event);
    }
    if (textFlags)
        QVERIFY(item->hasFocus());
    else
        QVERIFY(!item->hasFocus());
    QVERIFY(!scene.mouseGrabberItem());

    QVERIFY(selectable ? item->isSelected() : !item->isSelected());

    // 
    if (textFlags & Qt::TextEditorInteraction)
        QVERIFY(item->textCursor().columnNumber() > column);
    else
        QCOMPARE(item->textCursor().columnNumber(), 0);
}

class TextItem : public QGraphicsSimpleTextItem
{
public:
    TextItem(const QString& text) : QGraphicsSimpleTextItem(text)
    {
        updates = 0;
    }

    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
    {
        updates++;
        QGraphicsSimpleTextItem::paint(painter, option, widget);
    }

    int updates;
};

void tst_QGraphicsItem::ensureUpdateOnTextItem()
{
    QGraphicsScene scene;
    TextItem *text1 = new TextItem(QLatin1String("123"));
    scene.addItem(text1);
    QGraphicsView view(&scene);
    view.show();
    QTest::qWait(250);
    QCOMPARE(text1->updates,1);

    //same bouding rect but we have to update
    text1->setText(QLatin1String("321"));
    QTest::qWait(250);
    QCOMPARE(text1->updates,2);
}

void tst_QGraphicsItem::task243707_addChildBeforeParent()
{
    // Task reports that adding the child before the parent leads to an
    // inconsistent internal state that can cause a crash.  This test shows
    // one such crash.
    QGraphicsScene scene;
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *widget2 = new QGraphicsWidget(widget);
    scene.addItem(widget2);
    QVERIFY(!widget2->parentItem());
    scene.addItem(widget);
    QVERIFY(!widget->commonAncestorItem(widget2));
    QVERIFY(!widget2->commonAncestorItem(widget));
}

void tst_QGraphicsItem::task197802_childrenVisibility()
{
    QGraphicsScene scene;
    QGraphicsRectItem item(QRectF(0,0,20,20));

    QGraphicsRectItem *item2 = new QGraphicsRectItem(QRectF(0,0,10,10), &item);
    scene.addItem(&item);

    //freshly created: both visible
    QVERIFY(item.isVisible());
    QVERIFY(item2->isVisible());

    //hide child: parent visible, child not
    item2->hide();
    QVERIFY(item.isVisible());
    QVERIFY(!item2->isVisible());

    //hide parent: parent and child invisible
    item.hide();
    QVERIFY(!item.isVisible());
    QVERIFY(!item2->isVisible());

    //ask to show the child: parent and child invisible anyways
    item2->show();
    QVERIFY(!item.isVisible());
    QVERIFY(!item2->isVisible());

    //show the parent: both parent and child visible
    item.show();
    QVERIFY(item.isVisible());
    QVERIFY(item2->isVisible());

    delete item2;
}

void tst_QGraphicsItem::boundingRegion_data()
{
    QTest::addColumn<QLineF>("line");
    QTest::addColumn<qreal>("granularity");
    QTest::addColumn<QTransform>("transform");
    QTest::addColumn<QRegion>("expectedRegion");

    QTest::newRow("(0, 0, 10, 10) | 0.0 | identity | {(0, 0, 10, 10)}") << QLineF(0, 0, 10, 10) << qreal(0.0) << QTransform()
                                                                        << QRegion(QRect(0, 0, 10, 10));
    {
        QRegion r;
        r += QRect(0, 0, 6, 2);
        r += QRect(0, 2, 8, 2);
        r += QRect(0, 4, 10, 2);
        r += QRect(2, 6, 8, 2);
        r += QRect(4, 8, 6, 2);
        QTest::newRow("(0, 0, 10, 10) | 0.5 | identity | {(0, 0, 10, 10)}") << QLineF(0, 0, 10, 10) << qreal(0.5) << QTransform() << r;
    }
    {
        QRegion r;
        r += QRect(0, 0, 4, 1); r += QRect(0, 1, 5, 1); r += QRect(0, 2, 6, 1);
        r += QRect(0, 3, 7, 1); r += QRect(1, 4, 7, 1); r += QRect(2, 5, 7, 1);
        r += QRect(3, 6, 7, 1); r += QRect(4, 7, 6, 1); r += QRect(5, 8, 5, 1);
        r += QRect(6, 9, 4, 1);
        QTest::newRow("(0, 0, 10, 10) | 1.0 | identity | {(0, 0, 10, 10)}") << QLineF(0, 0, 10, 10) << qreal(1.0) << QTransform() << r;
    }
    QTest::newRow("(0, 0, 10, 0) | 0.0 | identity | {(0, 0, 10, 10)}") << QLineF(0, 0, 10, 0) << qreal(0.0) << QTransform()
                                                                       << QRegion(QRect(0, 0, 10, 1));
    QTest::newRow("(0, 0, 10, 0) | 0.5 | identity | {(0, 0, 10, 1)}") << QLineF(0, 0, 10, 0) << qreal(0.5) << QTransform()
                                                                      << QRegion(QRect(0, 0, 10, 1));
    QTest::newRow("(0, 0, 10, 0) | 1.0 | identity | {(0, 0, 10, 1)}") << QLineF(0, 0, 10, 0) << qreal(1.0) << QTransform()
                                                                      << QRegion(QRect(0, 0, 10, 1));
    QTest::newRow("(0, 0, 0, 10) | 0.0 | identity | {(0, 0, 10, 10)}") << QLineF(0, 0, 0, 10) << qreal(0.0) << QTransform()
                                                                       << QRegion(QRect(0, 0, 1, 10));
    QTest::newRow("(0, 0, 0, 10) | 0.5 | identity | {(0, 0, 1, 10)}") << QLineF(0, 0, 0, 10) << qreal(0.5) << QTransform()
                                                                      << QRegion(QRect(0, 0, 1, 10));
    QTest::newRow("(0, 0, 0, 10) | 1.0 | identity | {(0, 0, 1, 10)}") << QLineF(0, 0, 0, 10) << qreal(1.0) << QTransform()
                                                                      << QRegion(QRect(0, 0, 1, 10));
}

void tst_QGraphicsItem::boundingRegion()
{
    QFETCH(QLineF, line);
    QFETCH(qreal, granularity);
    QFETCH(QTransform, transform);
    QFETCH(QRegion, expectedRegion);

    QGraphicsLineItem item(line);
    QCOMPARE(item.boundingRegionGranularity(), qreal(0.0));
    item.setBoundingRegionGranularity(granularity);
    QCOMPARE(item.boundingRegionGranularity(), granularity);
    QCOMPARE(item.boundingRegion(transform), expectedRegion);
}

void tst_QGraphicsItem::itemTransform_parentChild()
{
    QGraphicsScene scene;
    QGraphicsItem *parent = scene.addRect(0, 0, 100, 100);
    QGraphicsItem *child = scene.addRect(0, 0, 100, 100);
    child->setParentItem(parent);
    child->setPos(10, 10);
    child->scale(2, 2);
    child->rotate(90);

    QCOMPARE(child->itemTransform(parent).map(QPointF(10, 10)), QPointF(-10, 30));
    QCOMPARE(parent->itemTransform(child).map(QPointF(-10, 30)), QPointF(10, 10));
}

void tst_QGraphicsItem::itemTransform_siblings()
{
    QGraphicsScene scene;
    QGraphicsItem *parent = scene.addRect(0, 0, 100, 100);
    QGraphicsItem *brother = scene.addRect(0, 0, 100, 100);
    QGraphicsItem *sister = scene.addRect(0, 0, 100, 100);
    parent->scale(10, 5);
    parent->rotate(-180);
    parent->shear(2, 3);

    brother->setParentItem(parent);
    sister->setParentItem(parent);

    brother->setPos(10, 10);
    brother->scale(2, 2);
    brother->rotate(90);
    sister->setPos(10, 10);
    sister->scale(2, 2);
    sister->rotate(90);

    QCOMPARE(brother->itemTransform(sister).map(QPointF(10, 10)), QPointF(10, 10));
    QCOMPARE(sister->itemTransform(brother).map(QPointF(10, 10)), QPointF(10, 10));
}

void tst_QGraphicsItem::itemTransform_unrelated()
{
    QGraphicsScene scene;
    QGraphicsItem *stranger1 = scene.addRect(0, 0, 100, 100);
    QGraphicsItem *stranger2 = scene.addRect(0, 0, 100, 100);
    stranger1->setPos(10, 10);
    stranger1->scale(2, 2);
    stranger1->rotate(90);
    stranger2->setPos(10, 10);
    stranger2->scale(2, 2);
    stranger2->rotate(90);

    QCOMPARE(stranger1->itemTransform(stranger2).map(QPointF(10, 10)), QPointF(10, 10));
    QCOMPARE(stranger2->itemTransform(stranger1).map(QPointF(10, 10)), QPointF(10, 10));
}

void tst_QGraphicsItem::opacity_data()
{
    QTest::addColumn<qreal>("p_opacity");
    QTest::addColumn<int>("p_opacityFlags");
    QTest::addColumn<qreal>("c1_opacity");
    QTest::addColumn<int>("c1_opacityFlags");
    QTest::addColumn<qreal>("c2_opacity");
    QTest::addColumn<int>("c2_opacityFlags");
    QTest::addColumn<qreal>("p_effectiveOpacity");
    QTest::addColumn<qreal>("c1_effectiveOpacity");
    QTest::addColumn<qreal>("c2_effectiveOpacity");
    QTest::addColumn<qreal>("c3_effectiveOpacity");

    // Modify the opacity and see how it propagates
    QTest::newRow("A: 1.0 0 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(1.0) << 0 << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(1.0) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("B: 0.5 0 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(0.5) << qreal(0.5) << qreal(0.5) << qreal(0.5);
    QTest::newRow("C: 0.5 0 0.1 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 << qreal(0.1) << 0 << qreal(1.0) << 0
                                                        << qreal(0.5) << qreal(0.05) << qreal(0.05) << qreal(0.05);
    QTest::newRow("D: 0.0 0 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.0) << 0 << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(0.0) << qreal(0.0) << qreal(0.0) << qreal(0.0);

    // Parent doesn't propagate to children - now modify the opacity and see how it propagates
    int flags = QGraphicsItem::ItemDoesntPropagateOpacityToChildren;
    QTest::newRow("E: 1.0 2 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(1.0) << flags << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(1.0) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("F: 0.5 2 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << flags << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(0.5) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("G: 0.5 2 0.1 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << flags << qreal(0.1) << 0 << qreal(1.0) << 0
                                                        << qreal(0.5) << qreal(0.1) << qreal(0.1) << qreal(0.1);
    QTest::newRow("H: 0.0 2 1.0 0 1.0 1.0 1.0 1.0 1.0") << qreal(0.0) << flags << qreal(1.0) << 0 << qreal(1.0) << 0
                                                        << qreal(0.0) << qreal(1.0) << qreal(1.0) << qreal(1.0);

    // Child ignores parent - now modify the opacity and see how it propagates
    flags = QGraphicsItem::ItemIgnoresParentOpacity;
    QTest::newRow("I: 1.0 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(1.0) << 0 << qreal(1.0) << flags << qreal(1.0) << 0
                                                        << qreal(1.0) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("J: 1.0 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 << qreal(0.5) << flags << qreal(0.5) << 0
                                                        << qreal(0.5) << qreal(0.5) << qreal(0.25) << qreal(0.25);
    QTest::newRow("K: 1.0 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(0.2) << 0 << qreal(0.2) << flags << qreal(0.2) << 0
                                                        << qreal(0.2) << qreal(0.2) << qreal(0.04) << qreal(0.04);
    QTest::newRow("L: 1.0 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(0.0) << 0 << qreal(0.0) << flags << qreal(0.0) << 0
                                                        << qreal(0.0) << qreal(0.0) << qreal(0.0) << qreal(0.0);

    // Child ignores parent and doesn't propagate - now modify the opacity and see how it propagates
    flags = QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemDoesntPropagateOpacityToChildren;
    QTest::newRow("M: 1.0 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(1.0) << 0 // p
                                                        << qreal(1.0) << flags // c1 (no prop)
                                                        << qreal(1.0) << 0 // c2
                                                        << qreal(1.0) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("M: 0.5 0 1.0 1 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 // p
                                                        << qreal(1.0) << flags // c1 (no prop)
                                                        << qreal(1.0) << 0 // c2
                                                        << qreal(0.5) << qreal(1.0) << qreal(1.0) << qreal(1.0);
    QTest::newRow("M: 0.5 0 0.5 1 1.0 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 // p
                                                        << qreal(0.5) << flags // c1 (no prop)
                                                        << qreal(1.0) << 0 // c2
                                                        << qreal(0.5) << qreal(0.5) << qreal(1.0) << qreal(1.0);
    QTest::newRow("M: 0.5 0 0.5 1 0.5 1.0 1.0 1.0 1.0") << qreal(0.5) << 0 // p
                                                        << qreal(0.5) << flags // c1 (no prop)
                                                        << qreal(0.5) << 0 // c2
                                                        << qreal(0.5) << qreal(0.5) << qreal(0.5) << qreal(0.5);
    QTest::newRow("M: 1.0 0 0.5 1 0.5 1.0 1.0 1.0 1.0") << qreal(1.0) << 0 // p
                                                        << qreal(0.5) << flags // c1 (no prop)
                                                        << qreal(0.5) << 0 // c2
                                                        << qreal(1.0) << qreal(0.5) << qreal(0.5) << qreal(0.5);
}

void tst_QGraphicsItem::opacity()
{
    QFETCH(qreal, p_opacity);
    QFETCH(int, p_opacityFlags);
    QFETCH(qreal, p_effectiveOpacity);
    QFETCH(qreal, c1_opacity);
    QFETCH(int, c1_opacityFlags);
    QFETCH(qreal, c1_effectiveOpacity);
    QFETCH(qreal, c2_opacity);
    QFETCH(int, c2_opacityFlags);
    QFETCH(qreal, c2_effectiveOpacity);
    QFETCH(qreal, c3_effectiveOpacity);

    QGraphicsRectItem *p = new QGraphicsRectItem;
    QGraphicsRectItem *c1 = new QGraphicsRectItem(p);
    QGraphicsRectItem *c2 = new QGraphicsRectItem(c1);
    QGraphicsRectItem *c3 = new QGraphicsRectItem(c2);

    QCOMPARE(p->opacity(), qreal(1.0));
    QCOMPARE(p->effectiveOpacity(), qreal(1.0));
    int opacityMask = QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemDoesntPropagateOpacityToChildren;
    QVERIFY(!(p->flags() & opacityMask));

    p->setOpacity(p_opacity);
    c1->setOpacity(c1_opacity);
    c2->setOpacity(c2_opacity);
    p->setFlags(QGraphicsItem::GraphicsItemFlags(p->flags() | p_opacityFlags));
    c1->setFlags(QGraphicsItem::GraphicsItemFlags(c1->flags() | c1_opacityFlags));
    c2->setFlags(QGraphicsItem::GraphicsItemFlags(c2->flags() | c2_opacityFlags));

    QCOMPARE(int(p->flags() & opacityMask), p_opacityFlags);
    QCOMPARE(int(c1->flags() & opacityMask), c1_opacityFlags);
    QCOMPARE(int(c2->flags() & opacityMask), c2_opacityFlags);
    QCOMPARE(p->opacity(), p_opacity);
    QCOMPARE(p->effectiveOpacity(), p_effectiveOpacity);
    QCOMPARE(c1->effectiveOpacity(), c1_effectiveOpacity);
    QCOMPARE(c2->effectiveOpacity(), c2_effectiveOpacity);
    QCOMPARE(c3->effectiveOpacity(), c3_effectiveOpacity);
}

void tst_QGraphicsItem::opacity2()
{
    EventTester *parent = new EventTester;
    EventTester *child = new EventTester(parent);
    EventTester *grandChild = new EventTester(child);

    QGraphicsScene scene;
    scene.addItem(parent);

    class MyGraphicsView : public QGraphicsView
    { public:
        int repaints;
        MyGraphicsView(QGraphicsScene *scene) : QGraphicsView(scene), repaints(0) {}
        void paintEvent(QPaintEvent *e) { ++repaints; QGraphicsView::paintEvent(e); }
    };

    MyGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(250);

#define RESET_REPAINT_COUNTERS \
    parent->repaints = 0; \
    child->repaints = 0; \
    grandChild->repaints = 0; \
    view.repaints = 0;

    RESET_REPAINT_COUNTERS

    child->setOpacity(0.0);
    QTest::qWait(100);
    QCOMPARE(view.repaints, 1);
    QCOMPARE(parent->repaints, 1);
    QCOMPARE(child->repaints, 0);
    QCOMPARE(grandChild->repaints, 0);

    RESET_REPAINT_COUNTERS

    child->setOpacity(1.0);
    QTest::qWait(100);
    QCOMPARE(view.repaints, 1);
    QCOMPARE(parent->repaints, 1);
    QCOMPARE(child->repaints, 1);
    QCOMPARE(grandChild->repaints, 1);

    RESET_REPAINT_COUNTERS

    parent->setOpacity(0.0);
    QTest::qWait(100);
    QCOMPARE(view.repaints, 1);
    QCOMPARE(parent->repaints, 0);
    QCOMPARE(child->repaints, 0);
    QCOMPARE(grandChild->repaints, 0);

    RESET_REPAINT_COUNTERS

    parent->setOpacity(1.0);
    QTest::qWait(100);
    QCOMPARE(view.repaints, 1);
    QCOMPARE(parent->repaints, 1);
    QCOMPARE(child->repaints, 1);
    QCOMPARE(grandChild->repaints, 1);

    grandChild->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
    RESET_REPAINT_COUNTERS

    child->setOpacity(0.0);
    QTest::qWait(100);
    QCOMPARE(view.repaints, 1);
    QCOMPARE(parent->repaints, 1);
    QCOMPARE(child->repaints, 0);
    QCOMPARE(grandChild->repaints, 1);

    RESET_REPAINT_COUNTERS

    child->setOpacity(0.0); // Already 0.0; no change.
    QTest::qWait(100);
    QCOMPARE(view.repaints, 0);
    QCOMPARE(parent->repaints, 0);
    QCOMPARE(child->repaints, 0);
    QCOMPARE(grandChild->repaints, 0);
}

void tst_QGraphicsItem::opacityZeroUpdates()
{
    EventTester *parent = new EventTester;
    EventTester *child = new EventTester(parent);

    child->setPos(10, 10);

    QGraphicsScene scene;
    scene.addItem(parent);

    class MyGraphicsView : public QGraphicsView
    { public:
        int repaints;
        QRegion paintedRegion;
        MyGraphicsView(QGraphicsScene *scene) : QGraphicsView(scene), repaints(0) {}
        void paintEvent(QPaintEvent *e)
        {
            ++repaints;
            paintedRegion += e->region();
            QGraphicsView::paintEvent(e);
        }
        void reset() { repaints = 0; paintedRegion = QRegion(); }
    };

    MyGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(250);

    view.reset();
    parent->setOpacity(0.0);

    QTest::qWait(200);

    // transforming items bounding rect to view coordinates
    const QRect childDeviceBoundingRect = child->deviceTransform(view.viewportTransform())
                                           .mapRect(child->boundingRect()).toRect();
    const QRect parentDeviceBoundingRect = parent->deviceTransform(view.viewportTransform())
                                           .mapRect(parent->boundingRect()).toRect();

    QRegion expectedRegion = parentDeviceBoundingRect.adjusted(-2, -2, 2, 2);
    expectedRegion += childDeviceBoundingRect.adjusted(-2, -2, 2, 2);

    QCOMPARE(view.paintedRegion, expectedRegion);
}

void tst_QGraphicsItem::itemStacksBehindParent()
{
    QGraphicsRectItem *parent1 = new QGraphicsRectItem(QRectF(0, 0, 100, 50));
    QGraphicsRectItem *child11 = new QGraphicsRectItem(QRectF(-10, 10, 50, 50), parent1);
    QGraphicsRectItem *grandChild111 = new QGraphicsRectItem(QRectF(-20, 20, 50, 50), child11);
    QGraphicsRectItem *child12 = new QGraphicsRectItem(QRectF(60, 10, 50, 50), parent1);
    QGraphicsRectItem *grandChild121 = new QGraphicsRectItem(QRectF(70, 20, 50, 50), child12);

    QGraphicsRectItem *parent2 = new QGraphicsRectItem(QRectF(0, 0, 100, 50));
    QGraphicsRectItem *child21 = new QGraphicsRectItem(QRectF(-10, 10, 50, 50), parent2);
    QGraphicsRectItem *grandChild211 = new QGraphicsRectItem(QRectF(-20, 20, 50, 50), child21);
    QGraphicsRectItem *child22 = new QGraphicsRectItem(QRectF(60, 10, 50, 50), parent2);
    QGraphicsRectItem *grandChild221 = new QGraphicsRectItem(QRectF(70, 20, 50, 50), child22);

    parent1->setData(0, "parent1");
    child11->setData(0, "child11");
    grandChild111->setData(0, "grandChild111");
    child12->setData(0, "child12");
    grandChild121->setData(0, "grandChild121");
    parent2->setData(0, "parent2");
    child21->setData(0, "child21");
    grandChild211->setData(0, "grandChild211");
    child22->setData(0, "child22");
    grandChild221->setData(0, "grandChild221");

    // Disambiguate siblings
    parent1->setZValue(1);
    child11->setZValue(1);
    child21->setZValue(1);

    QGraphicsScene scene;
    scene.addItem(parent1);
    scene.addItem(parent2);

    QCOMPARE(scene.items(0, 0, 100, 100), (QList<QGraphicsItem *>()
                                           << grandChild111 << child11
                                           << grandChild121 << child12 << parent1
                                           << grandChild211 << child21
                                           << grandChild221 << child22 << parent2));

    child11->setFlag(QGraphicsItem::ItemStacksBehindParent);
    QCOMPARE(scene.items(0, 0, 100, 100), (QList<QGraphicsItem *>()
                                           << grandChild121 << child12 << parent1
                                           << grandChild111 << child11
                                           << grandChild211 << child21
                                           << grandChild221 << child22 << parent2));

    child12->setFlag(QGraphicsItem::ItemStacksBehindParent);
    QCOMPARE(scene.items(0, 0, 100, 100), (QList<QGraphicsItem *>()
                                           << parent1 << grandChild111 << child11
                                           << grandChild121 << child12
                                           << grandChild211 << child21
                                           << grandChild221 << child22 << parent2));
}

class ClippingAndTransformsScene : public QGraphicsScene
{
public:
    QList<QGraphicsItem *> drawnItems;
protected:
    void drawItems(QPainter *painter, int numItems, QGraphicsItem *items[],
                   const QStyleOptionGraphicsItem options[], QWidget *widget = 0)
    {
        drawnItems.clear();
        for (int i = 0; i < numItems; ++i)
            drawnItems << items[i];
        QGraphicsScene::drawItems(painter, numItems, items, options, widget);
    }
};

void tst_QGraphicsItem::nestedClipping()
{
    ClippingAndTransformsScene scene;
    scene.setSceneRect(-50, -50, 200, 200);

    QGraphicsRectItem *root = new QGraphicsRectItem(QRectF(0, 0, 100, 100));
    root->setBrush(QColor(0, 0, 255));
    root->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QGraphicsRectItem *l1 = new QGraphicsRectItem(QRectF(0, 0, 100, 100));
    l1->setParentItem(root);
    l1->setPos(-50, 0);
    l1->setBrush(QColor(255, 0, 0));
    l1->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    QGraphicsEllipseItem *l2 = new QGraphicsEllipseItem(QRectF(0, 0, 100, 100));
    l2->setParentItem(l1);
    l2->setPos(50, 50);
    l2->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    l2->setBrush(QColor(255, 255, 0));
    QGraphicsRectItem *l3 = new QGraphicsRectItem(QRectF(0, 0, 25, 25));
    l3->setParentItem(l2);
    l3->setBrush(QColor(0, 255, 0));
    l3->setPos(50 - 12, -12);

    scene.addItem(root);

    root->setData(0, "root");
    l1->setData(0, "l1");
    l2->setData(0, "l2");
    l3->setData(0, "l3");
    
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(250);

    QList<QGraphicsItem *> expected;
    expected << root << l1 << l2 << l3;
    QCOMPARE(scene.drawnItems, expected);

    QImage image(200, 200, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);

    QPainter painter(&image);
    scene.render(&painter);
    painter.end();

    // Check transparent areas
    QCOMPARE(image.pixel(100, 25), qRgba(0, 0, 0, 0));
    QCOMPARE(image.pixel(100, 175), qRgba(0, 0, 0, 0));
    QCOMPARE(image.pixel(25, 100), qRgba(0, 0, 0, 0));
    QCOMPARE(image.pixel(175, 100), qRgba(0, 0, 0, 0));
    QCOMPARE(image.pixel(70, 80), qRgba(255, 0, 0, 255));
    QCOMPARE(image.pixel(80, 130), qRgba(255, 255, 0, 255));
    QCOMPARE(image.pixel(92, 105), qRgba(0, 255, 0, 255));
    QCOMPARE(image.pixel(105, 105), qRgba(0, 0, 255, 255));
#if 0 
    // Enable this to compare if the test starts failing.
    image.save("nestedClipping_reference.png");
#endif
}

class TransformDebugItem : public QGraphicsRectItem
{
public:
    TransformDebugItem()
        : QGraphicsRectItem(QRectF(-10, -10, 20, 20))
    {
        setBrush(QColor(qrand() % 256, qrand() % 256, qrand() % 256));
    }

    QTransform x;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0)
    {
        x = painter->worldTransform();
        QGraphicsRectItem::paint(painter, option, widget);
    }
};

void tst_QGraphicsItem::nestedClippingTransforms()
{
    TransformDebugItem *rootClipper = new TransformDebugItem;
    rootClipper->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    TransformDebugItem *child = new TransformDebugItem;
    child->setParentItem(rootClipper);
    child->setPos(2, 2);
    TransformDebugItem *grandChildClipper = new TransformDebugItem;
    grandChildClipper->setParentItem(child);
    grandChildClipper->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    grandChildClipper->setPos(4, 4);
    TransformDebugItem *greatGrandChild = new TransformDebugItem;
    greatGrandChild->setPos(2, 2);
    greatGrandChild->setParentItem(grandChildClipper);
    TransformDebugItem *grandChildClipper2 = new TransformDebugItem;
    grandChildClipper2->setParentItem(child);
    grandChildClipper2->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    grandChildClipper2->setPos(8, 8);
    TransformDebugItem *greatGrandChild2 = new TransformDebugItem;
    greatGrandChild2->setPos(2, 2);
    greatGrandChild2->setParentItem(grandChildClipper2);
    TransformDebugItem *grandChildClipper3 = new TransformDebugItem;
    grandChildClipper3->setParentItem(child);
    grandChildClipper3->setFlag(QGraphicsItem::ItemClipsChildrenToShape);
    grandChildClipper3->setPos(12, 12);
    TransformDebugItem *greatGrandChild3 = new TransformDebugItem;
    greatGrandChild3->setPos(2, 2);
    greatGrandChild3->setParentItem(grandChildClipper3);

    QGraphicsScene scene;
    scene.addItem(rootClipper);

    QImage image(scene.itemsBoundingRect().size().toSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    QPainter p(&image);
    scene.render(&p);
    p.end();

    QCOMPARE(rootClipper->x, QTransform(1, 0, 0, 0, 1, 0, 10, 10, 1));
    QCOMPARE(child->x, QTransform(1, 0, 0, 0, 1, 0, 12, 12, 1));
    QCOMPARE(grandChildClipper->x, QTransform(1, 0, 0, 0, 1, 0, 16, 16, 1));
    QCOMPARE(greatGrandChild->x, QTransform(1, 0, 0, 0, 1, 0, 18, 18, 1));
    QCOMPARE(grandChildClipper2->x, QTransform(1, 0, 0, 0, 1, 0, 20, 20, 1));
    QCOMPARE(greatGrandChild2->x, QTransform(1, 0, 0, 0, 1, 0, 22, 22, 1));
    QCOMPARE(grandChildClipper3->x, QTransform(1, 0, 0, 0, 1, 0, 24, 24, 1));
    QCOMPARE(greatGrandChild3->x, QTransform(1, 0, 0, 0, 1, 0, 26, 26, 1));
}

void tst_QGraphicsItem::sceneTransformCache()
{
    // Test that an item's scene transform is updated correctly when the
    // parent is transformed.
    QGraphicsScene scene;
    QGraphicsRectItem *rect = scene.addRect(0, 0, 100, 100);
    QGraphicsRectItem *rect2 = scene.addRect(0, 0, 100, 100);
    rect2->setParentItem(rect);
    rect2->rotate(90);
    rect->translate(0, 50);
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    rect->translate(0, 100);
    QTransform x;
    x.translate(0, 150);
    x.rotate(90);
    QCOMPARE(rect2->sceneTransform(), x);

    scene.removeItem(rect);

    //Crazy use case : rect4 child of rect3 so the transformation of rect4 will be cached.Good!
    //We remove rect4 from the scene, then the validTransform bit flag is set to 0 and the index of the cache
    //add to the freeTransformSlots. The problem was that sceneTransformIndex was not set to -1 so if a new item arrive
    //with a child (rect6) that will be cached then it will take the freeSlot (ex rect4) and put it his transform. But if rect4 is
    //added back to the scene then it will set the transform to his old sceneTransformIndex value that will erase the new
    //value of rect6 so rect6 transform will be wrong.
    QGraphicsRectItem *rect3 = scene.addRect(0, 0, 100, 100);
    QGraphicsRectItem *rect4 = scene.addRect(0, 0, 100, 100);
    rect3->setPos(QPointF(10,10));

    rect4->setParentItem(rect3);
    rect4->setPos(QPointF(10,10));

    QCOMPARE(rect4->mapToScene(rect4->boundingRect().topLeft()), QPointF(20,20));

    scene.removeItem(rect4);
    //rect4 transform is local only
    QCOMPARE(rect4->mapToScene(rect4->boundingRect().topLeft()), QPointF(10,10));

    QGraphicsRectItem *rect5 = scene.addRect(0, 0, 100, 100);
    QGraphicsRectItem *rect6 = scene.addRect(0, 0, 100, 100);
    rect5->setPos(QPointF(20,20));

    rect6->setParentItem(rect5);
    rect6->setPos(QPointF(10,10));
    //test if rect6 transform is ok
    QCOMPARE(rect6->mapToScene(rect6->boundingRect().topLeft()), QPointF(30,30));

    scene.addItem(rect4);

    QCOMPARE(rect4->mapToScene(rect4->boundingRect().topLeft()), QPointF(10,10));
    //test if rect6 transform is still correct
    QCOMPARE(rect6->mapToScene(rect6->boundingRect().topLeft()), QPointF(30,30));
}

void tst_QGraphicsItem::tabChangesFocus_data()
{
    QTest::addColumn<bool>("tabChangesFocus");
    QTest::newRow("tab changes focus") << true;
    QTest::newRow("tab doesn't change focus") << false;
}

void tst_QGraphicsItem::tabChangesFocus()
{
    QFETCH(bool, tabChangesFocus);
    
    QGraphicsScene scene;
    QGraphicsTextItem *item = scene.addText("Hello");
    item->setTabChangesFocus(tabChangesFocus);
    item->setTextInteractionFlags(Qt::TextEditorInteraction);
    item->setFocus();

    QDial *dial1 = new QDial;
    QGraphicsView *view = new QGraphicsView(&scene);
    QDial *dial2 = new QDial;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(dial1);
    layout->addWidget(view);
    layout->addWidget(dial2);

    QWidget widget;
    widget.setLayout(layout);
    widget.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&widget);
#endif

    dial1->setFocus();
    QTest::qWait(125);
    QVERIFY(dial1->hasFocus());

    QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
    QTest::qWait(125);
    QVERIFY(view->hasFocus());
    QVERIFY(item->hasFocus());

    QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
    QTest::qWait(125);

    if (tabChangesFocus) {
        QVERIFY(!view->hasFocus());
        QVERIFY(!item->hasFocus());
        QVERIFY(dial2->hasFocus());
    } else {
        QVERIFY(view->hasFocus());
        QVERIFY(item->hasFocus());
        QCOMPARE(item->toPlainText(), QString("\tHello"));
    }
}

void tst_QGraphicsItem::cacheMode()
{
    QGraphicsScene scene(0, 0, 100, 100);
    QGraphicsView view(&scene);
    view.resize(150, 150);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    EventTester *tester = new EventTester;
    EventTester *testerChild = new EventTester;
    testerChild->setParentItem(tester);
    EventTester *testerChild2 = new EventTester;
    testerChild2->setParentItem(testerChild);
    testerChild2->setFlag(QGraphicsItem::ItemIgnoresTransformations);

    scene.addItem(tester);
    QTest::qWait(250);

    for (int i = 0; i < 2; ++i) {
        // No visual change.
        QCOMPARE(tester->repaints, 1);
        QCOMPARE(testerChild->repaints, 1);
        QCOMPARE(testerChild2->repaints, 1);
        tester->setCacheMode(QGraphicsItem::NoCache);
        testerChild->setCacheMode(QGraphicsItem::NoCache);
        testerChild2->setCacheMode(QGraphicsItem::NoCache);
        QTest::qWait(250);
        QCOMPARE(tester->repaints, 1);
        QCOMPARE(testerChild->repaints, 1);
        QCOMPARE(testerChild2->repaints, 1);
        tester->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        testerChild->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        testerChild2->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
        QTest::qWait(250);
    }

    // The first move causes a repaint as the item is painted into its pixmap.
    // (Only occurs if the item has previously been painted without cache).
    tester->setPos(10, 10);
    testerChild->setPos(10, 10);
    testerChild2->setPos(10, 10);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 2);
    QCOMPARE(testerChild->repaints, 2);
    QCOMPARE(testerChild2->repaints, 2);

    // Consecutive moves should not repaint.
    tester->setPos(20, 20);
    testerChild->setPos(20, 20);
    testerChild2->setPos(20, 20);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 2);
    QCOMPARE(testerChild->repaints, 2);
    QCOMPARE(testerChild2->repaints, 2);

    // Translating does not result in a repaint.
    tester->translate(10, 10);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 2);
    QCOMPARE(testerChild->repaints, 2);
    QCOMPARE(testerChild2->repaints, 2);

    // Rotating results in a repaint.
    tester->rotate(45);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 3);
    QCOMPARE(testerChild->repaints, 3);
    QCOMPARE(testerChild2->repaints, 2);

    // Change to ItemCoordinateCache (triggers repaint).
    tester->setCacheMode(QGraphicsItem::ItemCoordinateCache); // autosize
    testerChild->setCacheMode(QGraphicsItem::ItemCoordinateCache); // autosize
    testerChild2->setCacheMode(QGraphicsItem::ItemCoordinateCache); // autosize
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 4);
    QCOMPARE(testerChild->repaints, 4);
    QCOMPARE(testerChild2->repaints, 3);

    // Rotating items with ItemCoordinateCache doesn't cause a repaint.
    tester->rotate(22);
    testerChild->rotate(22);
    testerChild2->rotate(22);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 4);
    QCOMPARE(testerChild->repaints, 4);
    QCOMPARE(testerChild2->repaints, 3);

    // Explicit update causes a repaint.
    tester->update(0, 0, 5, 5);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 5);
    QCOMPARE(testerChild->repaints, 4);
    QCOMPARE(testerChild2->repaints, 3);

    // Updating outside the item's bounds does not cause a repaint.
    tester->update(10, 10, 5, 5);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 5);
    QCOMPARE(testerChild->repaints, 4);
    QCOMPARE(testerChild2->repaints, 3);

    // Resizing an item should cause a repaint of that item. (because of
    // autosize).
    tester->setGeometry(QRectF(-15, -15, 30, 30));
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 6);
    QCOMPARE(testerChild->repaints, 4);
    QCOMPARE(testerChild2->repaints, 3);

    // Set fixed size.
    tester->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(30, 30));
    testerChild->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(30, 30));
    testerChild2->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(30, 30));
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 7);
    QCOMPARE(testerChild->repaints, 5);
    QCOMPARE(testerChild2->repaints, 4);

    // Resizing the item should cause a repaint.
    testerChild->setGeometry(QRectF(-15, -15, 30, 30));
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 7);
    QCOMPARE(testerChild->repaints, 6);
    QCOMPARE(testerChild2->repaints, 4);

    // Scaling the view does not cause a repaint.
    view.scale(0.7, 0.7);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 7);
    QCOMPARE(testerChild->repaints, 6);
    QCOMPARE(testerChild2->repaints, 4);

    // Switch to device coordinate cache.
    tester->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    testerChild->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    testerChild2->setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 8);
    QCOMPARE(testerChild->repaints, 7);
    QCOMPARE(testerChild2->repaints, 5);

    // Scaling the view back should cause repaints for two of the items.
    view.setTransform(QTransform());
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 9);
    QCOMPARE(testerChild->repaints, 8);
    QCOMPARE(testerChild2->repaints, 5);

    // Rotating the base item (perspective) should repaint two items.
    tester->setTransform(QTransform().rotate(10, Qt::XAxis));
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 10);
    QCOMPARE(testerChild->repaints, 9);
    QCOMPARE(testerChild2->repaints, 5);

    // Moving the middle item should case a repaint even if it's a move,
    // because the parent is rotated with a perspective.
    testerChild->setPos(1, 1);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 10);
    QCOMPARE(testerChild->repaints, 10);
    QCOMPARE(testerChild2->repaints, 5);

    // Make a huge item
    tester->setGeometry(QRectF(-4000, -4000, 8000, 8000));
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 11);
    QCOMPARE(testerChild->repaints, 10);
    QCOMPARE(testerChild2->repaints, 5);

    // Move the large item - will cause a repaint as the
    // cache is clipped.
    tester->setPos(5, 0);
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 12);
    QCOMPARE(testerChild->repaints, 10);
    QCOMPARE(testerChild2->repaints, 5);

    // Hiding and showing should invalidate the cache
    tester->hide();
    QTest::qWait(250);
    tester->show();
    QTest::qWait(250);
    QCOMPARE(tester->repaints, 13);
    QCOMPARE(testerChild->repaints, 11);
    QCOMPARE(testerChild2->repaints, 6);
}

void tst_QGraphicsItem::updateCachedItemAfterMove()
{
    // A simple item that uses ItemCoordinateCache
    EventTester *tester = new EventTester;
    tester->setCacheMode(QGraphicsItem::ItemCoordinateCache);

    // Add to a scene, show in a view, ensure it's painted and reset its
    // repaint counter.
    QGraphicsScene scene;
    scene.addItem(tester);
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(125);
    tester->repaints = 0;

    // Move the item, should not cause repaints
    tester->setPos(10, 0);
    QTest::qWait(125);
    QCOMPARE(tester->repaints, 0);

    // Move then update, should cause one repaint
    tester->setPos(20, 0);
    tester->update();
    QTest::qWait(125);
    QCOMPARE(tester->repaints, 1);

    // Hiding the item doesn't cause a repaint
    tester->hide();
    QTest::qWait(125);
    QCOMPARE(tester->repaints, 1);

    // Moving a hidden item doesn't cause a repaint
    tester->setPos(30, 0);
    tester->update();
    QTest::qWait(125);
    QCOMPARE(tester->repaints, 1);
}

class Track : public QGraphicsRectItem
{
public:
    Track(const QRectF &rect)
        : QGraphicsRectItem(rect)
    {
        setAcceptHoverEvents(true);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        QGraphicsRectItem::paint(painter, option, widget);
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("%1x%2\n%3x%4").arg(p.x()).arg(p.y()).arg(sp.x()).arg(sp.y()));
    }
    
protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event)
    {
        p = event->pos();
        sp = event->widget()->mapFromGlobal(event->screenPos());
        update();
    }
private:
    QPointF p;
    QPoint sp;
};

void tst_QGraphicsItem::deviceTransform_data()
{
    QTest::addColumn<bool>("untransformable1");
    QTest::addColumn<bool>("untransformable2");
    QTest::addColumn<bool>("untransformable3");
    QTest::addColumn<qreal>("rotation1");
    QTest::addColumn<qreal>("rotation2");
    QTest::addColumn<qreal>("rotation3");
    QTest::addColumn<QTransform>("deviceX");
    QTest::addColumn<QPointF>("mapResult1");
    QTest::addColumn<QPointF>("mapResult2");
    QTest::addColumn<QPointF>("mapResult3");

    QTest::newRow("nil") << false << false << false
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform()
                         << QPointF(150, 150) << QPointF(250, 250) << QPointF(350, 350);
    QTest::newRow("deviceX rot 90") << false << false << false
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-150, 150) << QPointF(-250, 250) << QPointF(-350, 350);
    QTest::newRow("deviceX rot 90 100") << true << false << false
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-50, 150) << QPointF(50, 250) << QPointF(150, 350);
    QTest::newRow("deviceX rot 90 010") << false << true << false
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-150, 150) << QPointF(-150, 250) << QPointF(-50, 350);
    QTest::newRow("deviceX rot 90 001") << false << false << true
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-150, 150) << QPointF(-250, 250) << QPointF(-250, 350);
    QTest::newRow("deviceX rot 90 111") << true << true << true
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-50, 150) << QPointF(50, 250) << QPointF(150, 350);
    QTest::newRow("deviceX rot 90 101") << true << false << true
                         << qreal(0.0) << qreal(0.0) << qreal(0.0)
                         << QTransform().rotate(90)
                         << QPointF(-50, 150) << QPointF(50, 250) << QPointF(150, 350);
}

void tst_QGraphicsItem::deviceTransform()
{
    QFETCH(bool, untransformable1);
    QFETCH(bool, untransformable2);
    QFETCH(bool, untransformable3);
    QFETCH(qreal, rotation1);
    QFETCH(qreal, rotation2);
    QFETCH(qreal, rotation3);
    QFETCH(QTransform, deviceX);
    QFETCH(QPointF, mapResult1);
    QFETCH(QPointF, mapResult2);
    QFETCH(QPointF, mapResult3);

    QGraphicsScene scene;
    Track *rect1 = new Track(QRectF(0, 0, 100, 100));
    Track *rect2 = new Track(QRectF(0, 0, 100, 100));
    Track *rect3 = new Track(QRectF(0, 0, 100, 100));
    rect2->setParentItem(rect1);
    rect3->setParentItem(rect2);
    rect1->setPos(100, 100);
    rect2->setPos(100, 100);
    rect3->setPos(100, 100);
    rect1->rotate(rotation1);
    rect2->rotate(rotation2);
    rect3->rotate(rotation3);
    rect1->setFlag(QGraphicsItem::ItemIgnoresTransformations, untransformable1);
    rect2->setFlag(QGraphicsItem::ItemIgnoresTransformations, untransformable2);
    rect3->setFlag(QGraphicsItem::ItemIgnoresTransformations, untransformable3);
    rect1->setBrush(Qt::red);
    rect2->setBrush(Qt::green);
    rect3->setBrush(Qt::blue);
    scene.addItem(rect1);

    QCOMPARE(rect1->deviceTransform(deviceX).map(QPointF(50, 50)), mapResult1);
    QCOMPARE(rect2->deviceTransform(deviceX).map(QPointF(50, 50)), mapResult2);
    QCOMPARE(rect3->deviceTransform(deviceX).map(QPointF(50, 50)), mapResult3);
}

QTEST_MAIN(tst_QGraphicsItem)
#include "tst_qgraphicsitem.moc"
