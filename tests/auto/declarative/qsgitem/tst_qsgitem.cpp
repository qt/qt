/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgitem.h"
#include "qsgcanvas.h"
#include "private/qsgfocusscope_p.h"
#include <QDebug>

class TestItem : public QSGItem
{
Q_OBJECT
public:
    TestItem(QSGItem *parent = 0) : QSGItem(parent), focused(false), pressCount(0), releaseCount(0) {}

    bool focused;
    int pressCount;
    int releaseCount;
protected:
    virtual void focusInEvent(QFocusEvent *) { Q_ASSERT(!focused); focused = true; }
    virtual void focusOutEvent(QFocusEvent *) { Q_ASSERT(focused); focused = false; }
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) { event->accept(); ++pressCount; }
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { event->accept(); ++releaseCount; }
};

class TestFocusScope : public QSGFocusScope
{
Q_OBJECT
public:
    TestFocusScope(QSGItem *parent = 0) : QSGFocusScope(parent), focused(false) {}

    bool focused;
protected:
    virtual void focusInEvent(QFocusEvent *) { Q_ASSERT(!focused); focused = true; }
    virtual void focusOutEvent(QFocusEvent *) { Q_ASSERT(focused); focused = false; }
};

class tst_qsgitem : public QObject
{
    Q_OBJECT
public:
    tst_qsgitem();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void noCanvas();
    void simpleFocus();
    void scopedFocus();
    void addedToCanvas();
    void changeParent();

    void constructor();
    void setParentItem();

    void visible();
    void enabled();

    void mouseGrab();
};

tst_qsgitem::tst_qsgitem()
{
}

void tst_qsgitem::initTestCase()
{
}

void tst_qsgitem::cleanupTestCase()
{
}

// Focus has no effect when outside a canvas
void tst_qsgitem::noCanvas()
{
    QSGItem *root = new TestItem;
    QSGItem *child = new TestItem(root);
    QSGItem *scope = new TestItem(root);
    QSGFocusScope *scopedChild = new TestFocusScope(scope);
    QSGFocusScope *scopedChild2 = new TestFocusScope(scope);

    QCOMPARE(root->hasFocus(), false);
    QCOMPARE(child->hasFocus(), false);
    QCOMPARE(scope->hasFocus(), false);
    QCOMPARE(scopedChild->hasFocus(), false);
    QCOMPARE(scopedChild2->hasFocus(), false);

    root->setFocus(true);
    scope->setFocus(true);
    scopedChild2->setFocus(true);
    QCOMPARE(root->hasFocus(), true);
    QCOMPARE(child->hasFocus(), false);
    QCOMPARE(scope->hasFocus(), true);
    QCOMPARE(scopedChild->hasFocus(), false);
    QCOMPARE(scopedChild2->hasFocus(), true);

    root->setFocus(false);
    child->setFocus(true);
    scopedChild->setFocus(true);
    scope->setFocus(false);
    QCOMPARE(root->hasFocus(), false);
    QCOMPARE(child->hasFocus(), true);
    QCOMPARE(scope->hasFocus(), false);
    QCOMPARE(scopedChild->hasFocus(), true);
    QCOMPARE(scopedChild2->hasFocus(), true);

    delete root;
}

struct FocusData {
    FocusData() : focus(false), activeFocus(false) {}

    void set(bool f, bool af) { focus = f; activeFocus = af; }
    bool focus;
    bool activeFocus;
};
struct FocusState : public QHash<QSGItem *, FocusData>
{
    FocusState() : activeFocusItem(0) {}
    FocusState &operator<<(QSGItem *item) {
        insert(item, FocusData());
        return *this;
    }

    void active(QSGItem *i) {
        activeFocusItem = i;
    }
    QSGItem *activeFocusItem;
};

#define FVERIFY() \
    do { \
        if (focusState.activeFocusItem) { \
            QCOMPARE(canvas.activeFocusItem(), focusState.activeFocusItem); \
            if (qobject_cast<TestItem *>(canvas.activeFocusItem())) \
                QCOMPARE(qobject_cast<TestItem *>(canvas.activeFocusItem())->focused, true); \
            else if (qobject_cast<TestFocusScope *>(canvas.activeFocusItem())) \
                QCOMPARE(qobject_cast<TestFocusScope *>(canvas.activeFocusItem())->focused, true); \
        } else { \
            QCOMPARE(canvas.activeFocusItem(), canvas.rootItem()); \
        } \
        for(QHash<QSGItem *, FocusData>::Iterator iter = focusState.begin(); \
            iter != focusState.end(); \
            iter++) { \
            QCOMPARE(iter.key()->hasFocus(), iter.value().focus); \
            QCOMPARE(iter.key()->hasActiveFocus(), iter.value().activeFocus); \
        } \
    } while(false)

// Tests a simple set of top-level scoped items
void tst_qsgitem::simpleFocus()
{
    QSGCanvas canvas;

    QSGItem *l1c1 = new TestItem(canvas.rootItem());
    QSGItem *l1c2 = new TestItem(canvas.rootItem());
    QSGItem *l1c3 = new TestItem(canvas.rootItem());

    QSGItem *l2c1 = new TestItem(l1c1);
    QSGItem *l2c2 = new TestItem(l1c1);
    QSGItem *l2c3 = new TestItem(l1c3);

    FocusState focusState;
    focusState << l1c1 << l1c2 << l1c3
               << l2c1 << l2c2 << l2c3;
    FVERIFY();

    l1c1->setFocus(true);
    focusState[l1c1].set(true, true);
    focusState.active(l1c1);
    FVERIFY();

    l2c3->setFocus(true);
    focusState[l1c1].set(false, false);
    focusState[l2c3].set(true, true);
    focusState.active(l2c3);
    FVERIFY();

    l1c3->setFocus(true);
    focusState[l2c3].set(false, false);
    focusState[l1c3].set(true, true);
    focusState.active(l1c3);
    FVERIFY();

    l1c2->setFocus(false);
    FVERIFY();

    l1c3->setFocus(false);
    focusState[l1c3].set(false, false);
    focusState.active(0);
    FVERIFY();

    l2c1->setFocus(true);
    focusState[l2c1].set(true, true);
    focusState.active(l2c1);
    FVERIFY();
}

// Items with a focus scope
void tst_qsgitem::scopedFocus()
{
    QSGCanvas canvas;

    QSGItem *l1c1 = new TestItem(canvas.rootItem());
    QSGItem *l1c2 = new TestItem(canvas.rootItem());
    QSGItem *l1c3 = new TestItem(canvas.rootItem());

    QSGItem *l2c1 = new TestItem(l1c1);
    QSGItem *l2c2 = new TestItem(l1c1);
    QSGItem *l2c3 = new TestFocusScope(l1c3);

    QSGItem *l3c1 = new TestItem(l2c3);
    QSGItem *l3c2 = new TestFocusScope(l2c3);

    QSGItem *l4c1 = new TestItem(l3c2);
    QSGItem *l4c2 = new TestItem(l3c2);

    FocusState focusState;
    focusState << l1c1 << l1c2 << l1c3
               << l2c1 << l2c2 << l2c3
               << l3c1 << l3c2
               << l4c1 << l4c2;
    FVERIFY();

    l4c2->setFocus(true);
    focusState[l4c2].set(true, false);
    FVERIFY();

    l4c1->setFocus(true);
    focusState[l4c2].set(false, false);
    focusState[l4c1].set(true, false);
    FVERIFY();

    l1c1->setFocus(true);
    focusState[l1c1].set(true, true);
    focusState.active(l1c1);
    FVERIFY();

    l3c2->setFocus(true);
    focusState[l3c2].set(true, false);
    FVERIFY();

    l2c3->setFocus(true);
    focusState[l1c1].set(false, false);
    focusState[l2c3].set(true, true);
    focusState[l3c2].set(true, true);
    focusState[l4c1].set(true, true);
    focusState.active(l4c1);
    FVERIFY();

    l3c2->setFocus(false);
    focusState[l3c2].set(false, false);
    focusState[l4c1].set(true, false);
    focusState.active(l2c3);
    FVERIFY();

    l3c2->setFocus(true);
    focusState[l3c2].set(true, true);
    focusState[l4c1].set(true, true);
    focusState.active(l4c1);
    FVERIFY();

    l4c1->setFocus(false);
    focusState[l4c1].set(false, false);
    focusState.active(l3c2);
    FVERIFY();

    l1c3->setFocus(true);
    focusState[l1c3].set(true, true);
    focusState[l2c3].set(false, false);
    focusState[l3c2].set(true, false);
    focusState.active(l1c3);
    FVERIFY();
}

// Tests focus corrects itself when a tree is added to a canvas for the first time
void tst_qsgitem::addedToCanvas()
{
    {
    QSGCanvas canvas;

    QSGItem *item = new TestItem;

    FocusState focusState;
    focusState << item;

    item->setFocus(true);
    focusState[item].set(true, false);
    FVERIFY();

    item->setParentItem(canvas.rootItem());
    focusState[item].set(true, true);
    focusState.active(item);
    FVERIFY();
    }

    {
    QSGCanvas canvas;

    QSGItem *item = new TestItem(canvas.rootItem());

    QSGItem *tree = new TestItem;
    QSGItem *c1 = new TestItem(tree);
    QSGItem *c2 = new TestItem(tree);

    FocusState focusState;
    focusState << item << tree << c1 << c2;

    item->setFocus(true);
    c1->setFocus(true);
    c2->setFocus(true);
    focusState[item].set(true, true);
    focusState[c1].set(true, false);
    focusState[c2].set(true, false);
    focusState.active(item);
    FVERIFY();

    tree->setParentItem(item);
    focusState[c1].set(false, false);
    focusState[c2].set(false, false);
    FVERIFY();
    }

    {
    QSGCanvas canvas;

    QSGItem *tree = new TestItem;
    QSGItem *c1 = new TestItem(tree);
    QSGItem *c2 = new TestItem(tree);

    FocusState focusState;
    focusState << tree << c1 << c2;
    c1->setFocus(true);
    c2->setFocus(true);
    focusState[c1].set(true, false);
    focusState[c2].set(true, false);
    FVERIFY();

    tree->setParentItem(canvas.rootItem());
    focusState[c1].set(true, true);
    focusState[c2].set(false, false);
    focusState.active(c1);
    FVERIFY();
    }

    {
    QSGCanvas canvas;
    QSGItem *tree = new TestFocusScope;
    QSGItem *c1 = new TestItem(tree);
    QSGItem *c2 = new TestItem(tree);

    FocusState focusState;
    focusState << tree << c1 << c2;
    c1->setFocus(true);
    c2->setFocus(true);
    focusState[c1].set(true, false);
    focusState[c2].set(true, false);
    FVERIFY();

    tree->setParentItem(canvas.rootItem());
    focusState[c1].set(true, false);
    focusState[c2].set(false, false);
    FVERIFY();

    tree->setFocus(true);
    focusState[tree].set(true, true);
    focusState[c1].set(true, true);
    focusState.active(c1);
    FVERIFY();
    }

    {
    QSGCanvas canvas;
    QSGItem *tree = new TestFocusScope;
    QSGItem *c1 = new TestItem(tree);
    QSGItem *c2 = new TestItem(tree);

    FocusState focusState;
    focusState << tree << c1 << c2;
    tree->setFocus(true);
    c1->setFocus(true);
    c2->setFocus(true);
    focusState[tree].set(true, false);
    focusState[c1].set(true, false);
    focusState[c2].set(true, false);
    FVERIFY();

    tree->setParentItem(canvas.rootItem());
    focusState[tree].set(true, true);
    focusState[c1].set(true, true);
    focusState[c2].set(false, false);
    focusState.active(c1);
    FVERIFY();
    }

    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());
    QSGItem *tree = new TestFocusScope;
    QSGItem *c1 = new TestItem(tree);
    QSGItem *c2 = new TestItem(tree);

    FocusState focusState;
    focusState << child << tree << c1 << c2;
    child->setFocus(true);
    tree->setFocus(true);
    c1->setFocus(true);
    c2->setFocus(true);
    focusState[child].set(true, true);
    focusState[tree].set(true, false);
    focusState[c1].set(true, false);
    focusState[c2].set(true, false);
    focusState.active(child);
    FVERIFY();

    tree->setParentItem(canvas.rootItem());
    focusState[tree].set(false, false);
    focusState[c1].set(true, false);
    focusState[c2].set(false, false);
    FVERIFY();

    tree->setFocus(true);
    focusState[child].set(false, false);
    focusState[tree].set(true, true);
    focusState[c1].set(true, true);
    focusState.active(c1);
    FVERIFY();
    }
}

void tst_qsgitem::changeParent()
{
    // Parent to no parent
    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());

    FocusState focusState;
    focusState << child;
    FVERIFY();

    child->setFocus(true);
    focusState[child].set(true, true);
    focusState.active(child);
    FVERIFY();

    child->setParentItem(0);
    focusState[child].set(true, false);
    focusState.active(0);
    FVERIFY();
    }

    // Different parent, same focus scope
    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());
    QSGItem *child2 = new TestItem(canvas.rootItem());

    FocusState focusState;
    focusState << child << child2;
    FVERIFY();

    child->setFocus(true);
    focusState[child].set(true, true);
    focusState.active(child);
    FVERIFY();

    child->setParentItem(child2);
    FVERIFY();
    }

    // Different parent, different focus scope
    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());
    QSGItem *child2 = new TestFocusScope(canvas.rootItem());
    QSGItem *item = new TestItem(child);

    FocusState focusState;
    focusState << child << child2 << item;
    FVERIFY();

    item->setFocus(true);
    focusState[item].set(true, true);
    focusState.active(item);
    FVERIFY();

    item->setParentItem(child2);
    focusState[item].set(true, false);
    focusState.active(0);
    FVERIFY();
    }
    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());
    QSGItem *child2 = new TestFocusScope(canvas.rootItem());
    QSGItem *item = new TestItem(child2);

    FocusState focusState;
    focusState << child << child2 << item;
    FVERIFY();

    item->setFocus(true);
    focusState[item].set(true, false);
    focusState.active(0);
    FVERIFY();

    item->setParentItem(child);
    focusState[item].set(true, true);
    focusState.active(item);
    FVERIFY();
    }
    {
    QSGCanvas canvas;
    QSGItem *child = new TestItem(canvas.rootItem());
    QSGItem *child2 = new TestFocusScope(canvas.rootItem());
    QSGItem *item = new TestItem(child2);

    FocusState focusState;
    focusState << child << child2 << item;
    FVERIFY();

    child->setFocus(true);
    item->setFocus(true);
    focusState[child].set(true, true);
    focusState[item].set(true, false);
    focusState.active(child);
    FVERIFY();

    item->setParentItem(child);
    focusState[item].set(false, false);
    FVERIFY();
    }
    
}

void tst_qsgitem::constructor()
{
    QSGItem *root = new QSGItem;
    QVERIFY(root->parent() == 0);
    QVERIFY(root->parentItem() == 0);

    QSGItem *child1 = new QSGItem(root);
    QVERIFY(child1->parent() == root);
    QVERIFY(child1->parentItem() == root);
    QCOMPARE(root->childItems().count(), 1);
    QCOMPARE(root->childItems().at(0), child1);

    QSGItem *child2 = new QSGItem(root);
    QVERIFY(child2->parent() == root);
    QVERIFY(child2->parentItem() == root);
    QCOMPARE(root->childItems().count(), 2);
    QCOMPARE(root->childItems().at(0), child1);
    QCOMPARE(root->childItems().at(1), child2);

    delete root;
}

void tst_qsgitem::setParentItem()
{
    QSGItem *root = new QSGItem;
    QVERIFY(root->parent() == 0);
    QVERIFY(root->parentItem() == 0);

    QSGItem *child1 = new QSGItem;
    QVERIFY(child1->parent() == 0);
    QVERIFY(child1->parentItem() == 0);

    child1->setParentItem(root);
    QVERIFY(child1->parent() == 0);
    QVERIFY(child1->parentItem() == root);
    QCOMPARE(root->childItems().count(), 1);
    QCOMPARE(root->childItems().at(0), child1);

    QSGItem *child2 = new QSGItem;
    QVERIFY(child2->parent() == 0);
    QVERIFY(child2->parentItem() == 0);
    child2->setParentItem(root);
    QVERIFY(child2->parent() == 0);
    QVERIFY(child2->parentItem() == root);
    QCOMPARE(root->childItems().count(), 2);
    QCOMPARE(root->childItems().at(0), child1);
    QCOMPARE(root->childItems().at(1), child2);

    child1->setParentItem(0);
    QVERIFY(child1->parent() == 0);
    QVERIFY(child1->parentItem() == 0);
    QCOMPARE(root->childItems().count(), 1);
    QCOMPARE(root->childItems().at(0), child2);

    delete root;

    QVERIFY(child1->parent() == 0);
    QVERIFY(child1->parentItem() == 0);
    QVERIFY(child2->parent() == 0);
    QVERIFY(child2->parentItem() == 0);

    delete child1;
    delete child2;
}

void tst_qsgitem::visible()
{
    QSGItem *root = new QSGItem;

    QSGItem *child1 = new QSGItem;
    child1->setParentItem(root);

    QSGItem *child2 = new QSGItem;
    child2->setParentItem(root);

    QVERIFY(child1->isVisible());
    QVERIFY(child2->isVisible());

    root->setVisible(false);
    QVERIFY(!child1->isVisible());
    QVERIFY(!child2->isVisible());

    root->setVisible(true);
    QVERIFY(child1->isVisible());
    QVERIFY(child2->isVisible());

    child1->setVisible(false);
    QVERIFY(!child1->isVisible());
    QVERIFY(child2->isVisible());

    child2->setParentItem(child1);
    QVERIFY(!child1->isVisible());
    QVERIFY(!child2->isVisible());

    child2->setParentItem(root);
    QVERIFY(!child1->isVisible());
    QVERIFY(child2->isVisible());

    delete root;
    delete child1;
    delete child2;
}

void tst_qsgitem::enabled()
{
    QSGItem *root = new QSGItem;

    QSGItem *child1 = new QSGItem;
    child1->setParentItem(root);

    QSGItem *child2 = new QSGItem;
    child2->setParentItem(root);

    QVERIFY(child1->isEnabled());
    QVERIFY(child2->isEnabled());

    root->setEnabled(false);
    QVERIFY(!child1->isEnabled());
    QVERIFY(!child2->isEnabled());

    root->setEnabled(true);
    QVERIFY(child1->isEnabled());
    QVERIFY(child2->isEnabled());

    child1->setEnabled(false);
    QVERIFY(!child1->isEnabled());
    QVERIFY(child2->isEnabled());

    child2->setParentItem(child1);
    QVERIFY(!child1->isEnabled());
    QVERIFY(!child2->isEnabled());

    child2->setParentItem(root);
    QVERIFY(!child1->isEnabled());
    QVERIFY(child2->isEnabled());

    delete root;
    delete child1;
    delete child2;
}

void tst_qsgitem::mouseGrab()
{
    QSGCanvas *canvas = new QSGCanvas;
    canvas->resize(200, 200);
    canvas->show();

    TestItem *child1 = new TestItem;
    child1->setAcceptedMouseButtons(Qt::LeftButton);
    child1->setSize(QSizeF(200, 100));
    child1->setParentItem(canvas->rootItem());

    TestItem *child2 = new TestItem;
    child2->setAcceptedMouseButtons(Qt::LeftButton);
    child2->setY(100);
    child2->setSize(QSizeF(200, 100));
    child2->setParentItem(canvas->rootItem());

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QVERIFY(canvas->mouseGrabberItem() == child1);
    QCOMPARE(child1->pressCount, 1);
    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QVERIFY(canvas->mouseGrabberItem() == 0);
    QCOMPARE(child1->releaseCount, 1);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QVERIFY(canvas->mouseGrabberItem() == child1);
    QCOMPARE(child1->pressCount, 2);
    child1->setEnabled(false);
    QVERIFY(canvas->mouseGrabberItem() == 0);
    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QCOMPARE(child1->releaseCount, 1);
    child1->setEnabled(true);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QVERIFY(canvas->mouseGrabberItem() == child1);
    QCOMPARE(child1->pressCount, 3);
    child1->setVisible(false);
    QVERIFY(canvas->mouseGrabberItem() == 0);
    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QCOMPARE(child1->releaseCount, 1);
    child1->setVisible(true);

    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QVERIFY(canvas->mouseGrabberItem() == child1);
    QCOMPARE(child1->pressCount, 4);
    child2->grabMouse();
    QVERIFY(canvas->mouseGrabberItem() == child2);
    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QCOMPARE(child1->releaseCount, 1);
    QCOMPARE(child2->releaseCount, 1);

    child2->grabMouse();
    QVERIFY(canvas->mouseGrabberItem() == child2);
    QTest::mousePress(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QCOMPARE(child1->pressCount, 4);
    QCOMPARE(child2->pressCount, 1);
    QTest::mouseRelease(canvas, Qt::LeftButton, 0, QPoint(50,50));
    QCOMPARE(child1->releaseCount, 1);
    QCOMPARE(child2->releaseCount, 2);

    delete child1;
    delete child2;
    delete canvas;
}


QTEST_MAIN(tst_qsgitem)

#include "tst_qsgitem.moc"
