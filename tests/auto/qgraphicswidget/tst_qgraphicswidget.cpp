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
#include <qgraphicswidget.h>
#include <qgraphicsscene.h>
#include <qgraphicssceneevent.h>
#include <qgraphicsview.h>
#include <qstyleoption.h>
#include <qgraphicslinearlayout.h>
#include <qcleanlooksstyle.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <qwidgetaction.h>
#include "../../shared/util.h"


class EventSpy : public QObject
{
    Q_OBJECT
public:
    EventSpy(QObject *watched, QEvent::Type type)
        : _count(0), spied(type)
    {
        watched->installEventFilter(this);
    }

    int count() const { return _count; }

protected:
    bool eventFilter(QObject *watched, QEvent *event)
    {
        Q_UNUSED(watched);
        if (event->type() == spied)
            ++_count;
        return false;
    }

    int _count;
    QEvent::Type spied;
};

#ifndef QT_NO_STYLE_CLEANLOOKS
class tst_QGraphicsWidget : public QObject {
Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgraphicswidget();

    void activation();
    void boundingRect_data();
    void boundingRect();
    void dumpFocusChain_data();
    void dumpFocusChain();
    void focusWidget_data();
    void focusWidget();
    void focusWidget2();
    void focusPolicy_data();
    void focusPolicy();
    void font_data();
    void font();
    void fontPropagation();
    void fontPropagationWidgetItemWidget();
    void fontPropagationSceneChange();
    void geometry_data();
    void geometry();
    void getContentsMargins_data();
    void getContentsMargins();
    void initStyleOption_data();
    void initStyleOption();
    void layout_data();
    void layout();
    void layoutDirection_data();
    void layoutDirection();
    void paint_data();
    void paint();
    void palettePropagation();
    void parentWidget_data();
    void parentWidget();
    void resize_data();
    void resize();
    void setAttribute_data();
    void setAttribute();
    void setStyle_data();
    void setStyle();
    void setTabOrder_data();
    void setTabOrder();
    void setTabOrderAndReparent();
    void topLevelWidget_data();
    void topLevelWidget();
    void unsetLayoutDirection_data();
    void unsetLayoutDirection();
    void focusNextPrevChild_data();
    void focusNextPrevChild();
    void verifyFocusChain();
    void updateFocusChainWhenChildDie();
    void sizeHint_data();
    void sizeHint();
    void consistentPosSizeGeometry_data();
    void consistentPosSizeGeometry();
    void setSizes_data();
    void setSizes();
    void closePopupOnOutsideClick();
    void defaultSize();
    void shortcutsDeletion();

    // Task fixes
    void task236127_bspTreeIndexFails();
    void task243004_setStyleCrash();
    void task250119_shortcutContext();
};


static void sendMouseMove(QWidget *widget, const QPoint &point, Qt::MouseButton button = Qt::NoButton, Qt::MouseButtons buttons = 0)
{
    QTest::mouseMove(widget, point);
    QMouseEvent event(QEvent::MouseMove, point, button, buttons, 0);
    QApplication::sendEvent(widget, &event);
}

// Subclass that exposes the protected functions.
class SubQGraphicsWidget : public QGraphicsWidget {
public:
    SubQGraphicsWidget(QGraphicsItem *parent = 0, Qt::WindowFlags windowFlags = 0)
        : QGraphicsWidget(parent, windowFlags), eventCount(0)
        { }

    void initStyleOption(QStyleOption *option)
        { QGraphicsWidget::initStyleOption(option); }

    void call_changeEvent(QEvent* event)
        { return QGraphicsWidget::changeEvent(event); }

    bool call_event(QEvent *e)
        { return event(e); }

    void call_focusInEvent(QFocusEvent* event)
        { return QGraphicsWidget::focusInEvent(event); }

    bool call_focusNextPrevChild(bool next)
        { return QGraphicsWidget::focusNextPrevChild(next); }

    void call_focusOutEvent(QFocusEvent* event)
        { return QGraphicsWidget::focusOutEvent(event); }

    void call_hideEvent(QHideEvent* event)
        { return QGraphicsWidget::hideEvent(event); }

    QVariant call_itemChange(QGraphicsItem::GraphicsItemChange change, QVariant const& value)
        { return QGraphicsWidget::itemChange(change, value); }

    void call_moveEvent(QGraphicsSceneMoveEvent* event)
        { return QGraphicsWidget::moveEvent(event); }

    void call_polishEvent()
        { return QGraphicsWidget::polishEvent(); }

    QVariant call_propertyChange(QString const& propertyName, QVariant const& value)
        { return QGraphicsWidget::propertyChange(propertyName, value); }

    void call_resizeEvent(QGraphicsSceneResizeEvent* event)
        { return QGraphicsWidget::resizeEvent(event); }

    bool call_sceneEvent(QEvent* event)
        { return QGraphicsWidget::sceneEvent(event); }

    void call_showEvent(QShowEvent* event)
        { return QGraphicsWidget::showEvent(event); }

    QSizeF call_sizeHint(Qt::SizeHint which, QSizeF const& constraint = QSizeF()) const
        { return QGraphicsWidget::sizeHint(which, constraint); }

    void call_updateGeometry()
        { return QGraphicsWidget::updateGeometry(); }

    int eventCount;
    Qt::LayoutDirection m_painterLayoutDirection;
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        m_painterLayoutDirection = painter->layoutDirection();
        QGraphicsWidget::paint(painter, option, widget);
        if (hasFocus()) {
            painter->setPen(Qt::DotLine);
            painter->drawRect(rect());
        }
        //painter->drawText(QPointF(0,15), data(0).toString());
    }

protected:
    bool event(QEvent *event)
    {
        eventCount++;
        return QGraphicsWidget::event(event);
    }
};

// This will be called before the first test function is executed.
// It is only called once.
void tst_QGraphicsWidget::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGraphicsWidget::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGraphicsWidget::init()
{
}

// This will be called after every test function.
void tst_QGraphicsWidget::cleanup()
{
}

class SizeHinter : public QGraphicsWidget
{
public:
    SizeHinter(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0,
                const QSizeF &min = QSizeF(5,5), 
                const QSizeF &pref = QSizeF(50, 50), 
                const QSizeF &max = QSizeF(500, 500)) 
        : QGraphicsWidget(parent, wFlags) 
    {
        m_sizes[Qt::MinimumSize] = min;
        m_sizes[Qt::PreferredSize] = pref;
        m_sizes[Qt::MaximumSize] = max;
        
    }
    void setSizeHint(Qt::SizeHint which, const QSizeF &newSizeHint)
    {
        m_sizes[which] = newSizeHint;
    }

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
    {
        Q_UNUSED(constraint);
        return m_sizes[which];
    }
private:
    QSizeF m_sizes[4];
};

void tst_QGraphicsWidget::qgraphicswidget()
{
    SubQGraphicsWidget widget;
    QVERIFY(widget.isVisible());

    QVERIFY(!widget.isWindow());
    QCOMPARE(widget.boundingRect(), QRectF(0, 0, 0, 0));
    QCOMPARE(widget.focusWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widget.focusPolicy(), Qt::NoFocus);
    QCOMPARE(widget.font(), QFont());
    QCOMPARE(widget.geometry(), QRectF(widget.pos(), widget.size()));
    QCOMPARE(widget.layout(), (QGraphicsLayout*)0);
    QCOMPARE(widget.layoutDirection(), Qt::LeftToRight);
    QCOMPARE(widget.palette(), QPalette());
    QCOMPARE(widget.parentWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widget.rect(), QRectF(QPointF(), widget.size()));
    QCOMPARE(widget.size(), QSizeF(0, 0));
    QVERIFY(widget.style() != (QStyle*)0);
    QCOMPARE(widget.testAttribute(Qt::WA_AcceptDrops), false);
    QCOMPARE(widget.topLevelWidget(), (QGraphicsWidget*)&widget);
    QCOMPARE(widget.type(), (int)QGraphicsWidget::Type);
    QCOMPARE(widget.call_propertyChange(QString(), QVariant()), QVariant());
    widget.call_sizeHint(Qt::PreferredSize, QSizeF());
    
    QGraphicsScene scene;
    QGraphicsWidget *parent = new QGraphicsWidget;
    SizeHinter *child = new SizeHinter(parent);
    
    QCOMPARE(child->minimumSize(), QSizeF(5, 5));
}

void tst_QGraphicsWidget::activation()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *window1 = new QGraphicsWidget(0, Qt::Window);
    QGraphicsWidget *window2 = new QGraphicsWidget(0, Qt::Window);
    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QGraphicsScene scene;
    scene.addItem(widget);
    scene.addItem(window1);
    scene.addItem(window2);

    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QEvent activateEvent(QEvent::WindowActivate);
    QApplication::sendEvent(&scene, &activateEvent);

    QVERIFY(widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    scene.setActiveWindow(window1);
    QVERIFY(widget->isActiveWindow());
    QVERIFY(window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());

    QEvent deactivateEvent(QEvent::WindowDeactivate);
    QApplication::sendEvent(&scene, &deactivateEvent);

    QVERIFY(!widget->isActiveWindow());
    QVERIFY(!window1->isActiveWindow());
    QVERIFY(!window2->isActiveWindow());
}

void tst_QGraphicsWidget::boundingRect_data()
{
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null") << QSizeF(0, 0);
    QTest::newRow("avg") << QSizeF(10, 10);
}

// QRectF boundingRect() const public
void tst_QGraphicsWidget::boundingRect()
{
    QFETCH(QSizeF, size);
    SubQGraphicsWidget widget;
    widget.resize(size);
    QCOMPARE(widget.rect(), QRectF(QPointF(), size));
    QCOMPARE(widget.boundingRect(), QRectF(QPointF(0, 0), size));
}

void tst_QGraphicsWidget::dumpFocusChain_data()
{
    QTest::addColumn<bool>("scene");
    QTest::addColumn<int>("children");
    QTest::addColumn<bool>("setFocus");
    QTest::newRow("empty world") << false << 0 << false;
    QTest::newRow("one world") << true << 2 << false;
    QTest::newRow("one world w/focus") << true << 2 << true;
}

// void dumpFocusChain(QGraphicsScene* scene) public (static)
void tst_QGraphicsWidget::dumpFocusChain()
{
    // ### this test is very strange...
    QFETCH(bool, scene);
    SubQGraphicsWidget *parent = new SubQGraphicsWidget;
    QGraphicsScene *theScene = 0;
    if (scene) {
        theScene = new QGraphicsScene(this);
        theScene->addItem(parent);
    }
    QFETCH(int, children);
    QFETCH(bool, setFocus);
    for (int i = 0; i < children; ++i) {
        SubQGraphicsWidget *widget = new SubQGraphicsWidget(parent);
        if (setFocus) {
            widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
            if (scene)
                theScene->setFocusItem(widget);
        }
    }

    if (!scene)
        delete parent;
}

void tst_QGraphicsWidget::focusWidget_data()
{
    QTest::addColumn<int>("childCount");
    QTest::addColumn<int>("childWithFocus");
    QTest::newRow("none") << 0 << 0;
    QTest::newRow("first") << 3 << 0;
    QTest::newRow("last") << 3 << 2;
}

// QGraphicsWidget* focusWidget() const public
void tst_QGraphicsWidget::focusWidget()
{
    SubQGraphicsWidget *parent = new SubQGraphicsWidget;
    QCOMPARE(parent->focusWidget(), (QGraphicsWidget *)0);
    QGraphicsScene scene;
    scene.addItem(parent);

    QFETCH(int, childCount);
    QList<SubQGraphicsWidget *> children;
    for (int i = 0; i < childCount; ++i) {
        SubQGraphicsWidget *widget = new SubQGraphicsWidget(parent);
        widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
        children.append(widget);
    }
    if (childCount > 0) {
        QFETCH(int, childWithFocus);
        SubQGraphicsWidget *widget = children[childWithFocus];
        widget->setFocus();
        QVERIFY(widget->hasFocus());
        QCOMPARE(parent->focusWidget(), static_cast<QGraphicsWidget*>(widget));
    }
}

void tst_QGraphicsWidget::focusWidget2()
{
    QGraphicsScene scene;
    
    QGraphicsWidget *widget = new QGraphicsWidget;
    EventSpy focusInSpy(widget, QEvent::FocusIn);
    EventSpy focusOutSpy(widget, QEvent::FocusOut);

    scene.addItem(widget);

    QVERIFY(!widget->hasFocus());
    widget->setFocusPolicy(Qt::StrongFocus);
    QVERIFY(!widget->hasFocus());

    QGraphicsWidget *subWidget = new QGraphicsWidget(widget);
    QVERIFY(!subWidget->hasFocus());

    scene.setFocus();

    QVERIFY(!widget->hasFocus());
    QVERIFY(!subWidget->hasFocus());

    widget->setFocus();    

    QVERIFY(widget->hasFocus());
    QCOMPARE(focusInSpy.count(), 1);
    QVERIFY(!subWidget->hasFocus());

    QGraphicsWidget *otherSubWidget = new QGraphicsWidget;
    EventSpy otherFocusInSpy(otherSubWidget, QEvent::FocusIn);
    EventSpy otherFocusOutSpy(otherSubWidget, QEvent::FocusOut);

    otherSubWidget->setFocusPolicy(Qt::StrongFocus);
    otherSubWidget->setParentItem(widget);

    QVERIFY(widget->hasFocus());
    QCOMPARE(scene.focusItem(), (QGraphicsItem *)widget);
    QVERIFY(!subWidget->hasFocus());
    QVERIFY(!otherSubWidget->hasFocus());

    widget->hide();
    QVERIFY(!widget->hasFocus()); // lose but still has subfocus
    QCOMPARE(focusInSpy.count(), 1);
    QCOMPARE(focusOutSpy.count(), 1);

    widget->show();
    QVERIFY(!widget->hasFocus()); // no regain
    QCOMPARE(focusInSpy.count(), 1);
    QCOMPARE(focusOutSpy.count(), 1);

    widget->hide();

    // try to setup subFocus on item that can't take focus
    subWidget->setFocus();
    QVERIFY(!subWidget->hasFocus());
    QVERIFY(!scene.focusItem()); // but isn't the scene's focus item

    // try to setup subFocus on item that can take focus
    otherSubWidget->setFocus();
    QVERIFY(!otherSubWidget->hasFocus());
    QCOMPARE(widget->focusWidget(), otherSubWidget);
    QVERIFY(!scene.focusItem()); // but isn't the scene's focus item

    widget->show();

    QCOMPARE(scene.focusItem(), (QGraphicsItem *)otherSubWidget); // but isn't the scene's focus item
    QCOMPARE(otherFocusInSpy.count(), 1);
    QCOMPARE(otherFocusOutSpy.count(), 0);

    delete otherSubWidget;

    QCOMPARE(otherFocusOutSpy.count(), 1);
    QVERIFY(!scene.focusItem());
    QVERIFY(!widget->focusWidget());
}

Q_DECLARE_METATYPE(Qt::FocusPolicy)
void tst_QGraphicsWidget::focusPolicy_data()
{
    QTest::addColumn<Qt::FocusPolicy>("focusPolicy1");
    QTest::addColumn<Qt::FocusPolicy>("focusPolicy2");

    for (int i = 0; i < 25; ++i) {
        QTestData &data = QTest::newRow(QString("%1").arg(i).toLatin1());
        switch(i % 5) {
        case 0: data << Qt::TabFocus; break;
        case 1: data << Qt::ClickFocus; break;
        case 2: data << Qt::StrongFocus; break;
        case 3: data << Qt::WheelFocus; break;
        case 4: data << Qt::NoFocus; break;
        }
        switch(i / 5) {
        case 0: data << Qt::TabFocus; break;
        case 1: data << Qt::ClickFocus; break;
        case 2: data << Qt::StrongFocus; break;
        case 3: data << Qt::WheelFocus; break;
        case 4: data << Qt::NoFocus; break;
        }
    }
}

// Qt::FocusPolicy focusPolicy() const public
void tst_QGraphicsWidget::focusPolicy()
{
    QGraphicsScene scene;
    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    scene.addItem(widget);
    QCOMPARE(Qt::NoFocus, widget->focusPolicy());

    QFETCH(Qt::FocusPolicy, focusPolicy1);
    widget->setFocusPolicy(focusPolicy1);
    QCOMPARE(widget->focusPolicy(), focusPolicy1);
    bool isFocusable = widget->flags() & QGraphicsItem::ItemIsFocusable;
    bool wasFocusable = isFocusable;
    QVERIFY(isFocusable == (focusPolicy1 != Qt::NoFocus));
    widget->setFocus();
    QCOMPARE(widget->hasFocus(), isFocusable);

    QFETCH(Qt::FocusPolicy, focusPolicy2);
    widget->setFocusPolicy(focusPolicy2);
    QCOMPARE(widget->focusPolicy(), focusPolicy2);
    isFocusable = widget->flags() & QGraphicsItem::ItemIsFocusable;
    QVERIFY(isFocusable == (focusPolicy2 != Qt::NoFocus));
    QCOMPARE(widget->hasFocus(), wasFocusable && isFocusable);
}

void tst_QGraphicsWidget::font_data()
{
    QTest::addColumn<QString>("fontName");
    QTest::newRow("Helvetica") << "Helvetica";
}

// QFont font() const public
void tst_QGraphicsWidget::font()
{
    QFETCH(QString, fontName);
    SubQGraphicsWidget widget;
    QCOMPARE(widget.font(), QFont());

    QFont font(fontName);
    widget.setFont(font);
    QCOMPARE(widget.font().family(), font.family());
}

void tst_QGraphicsWidget::fontPropagation()
{
    QGraphicsWidget *root = new QGraphicsWidget;
    QGraphicsWidget *child0 = new QGraphicsWidget(root);
    QGraphicsWidget *child1 = new QGraphicsWidget(child0);
    QGraphicsWidget *child2 = new QGraphicsWidget(child1);
    QGraphicsScene scene;
    scene.addItem(root);

    // Check that only the application fonts apply.
    QFont appFont = QApplication::font();
    QCOMPARE(scene.font(), appFont);
    QCOMPARE(root->font(), appFont);
    QCOMPARE(child0->font(), appFont);
    QCOMPARE(child1->font(), appFont);

    // Set child0's Text, and set ToolTipBase on child1.
    QFont boldFont;
    boldFont.setBold(true);
    child0->setFont(boldFont);
    QFont italicFont;
    italicFont.setItalic(true);
    child1->setFont(italicFont);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->font(), appFont);
    QCOMPARE(scene.font(), appFont);
    QVERIFY(child0->font().bold());
    QVERIFY(!child0->font().italic());
    QVERIFY(child1->font().bold());
    QVERIFY(child1->font().italic());
    QVERIFY(child2->font().bold());
    QVERIFY(child2->font().italic());

    QGraphicsWidget *child3 = new QGraphicsWidget(child2);
    QVERIFY(child3->font().bold());
    QVERIFY(child3->font().italic());

    QGraphicsWidget *child4 = new QGraphicsWidget;
    child4->setParentItem(child3);
    QVERIFY(child4->font().bold());
    QVERIFY(child4->font().italic());

    // Replace the app font for child2. Button should propagate but Text
    // should still be ignored. The previous ToolTipBase setting is gone.
    QFont sizeFont;
    sizeFont.setPointSize(43);
    child1->setFont(sizeFont);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->font(), appFont);
    QCOMPARE(scene.font(), appFont);
    QVERIFY(child0->font().bold());
    QVERIFY(!child0->font().italic());
    QVERIFY(child1->font().bold());
    QVERIFY(!child1->font().italic());
    QCOMPARE(child1->font().pointSize(), 43);
    QVERIFY(child2->font().bold());
    QVERIFY(!child2->font().italic());
    QCOMPARE(child2->font().pointSize(), 43);
}

void tst_QGraphicsWidget::fontPropagationWidgetItemWidget()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsRectItem *rect = new QGraphicsRectItem(widget);
    QGraphicsWidget *widget2 = new QGraphicsWidget(rect);

    QGraphicsScene scene;
    scene.addItem(widget);

    QFont font;
    font.setPointSize(43);
    widget->setFont(font);

    QCOMPARE(widget2->font().pointSize(), 43);
    QCOMPARE(widget2->font().resolve(), QFont().resolve());

    widget->setFont(QFont());

    QCOMPARE(widget2->font().pointSize(), qApp->font().pointSize());
}

void tst_QGraphicsWidget::fontPropagationSceneChange()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsRectItem *rect = new QGraphicsRectItem(widget);
    QGraphicsWidget *widget2 = new QGraphicsWidget(rect);

    QGraphicsScene scene;
    QGraphicsScene scene2;

    QFont font;
    font.setPointSize(47);
    scene.setFont(font);
    
    QFont font2;
    font2.setPointSize(74);
    scene2.setFont(font2);

    scene.addItem(widget);
    QCOMPARE(widget2->font().pointSize(), 47);
    scene2.addItem(widget);
    QCOMPARE(widget2->font().pointSize(), 74);
}

void tst_QGraphicsWidget::geometry_data()
{
    QTest::addColumn<QPointF>("pos");
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null, null") << QPointF() << QSizeF(0, 0);
    QTest::newRow("null, normal") << QPointF() << QSizeF(10, 10);
    QTest::newRow("neg, normal") << QPointF(-5, -5) << QSizeF(10, 10);
}

// QRectF geometry() const public
void tst_QGraphicsWidget::geometry()
{
    SubQGraphicsWidget widget;
    QCOMPARE(widget.geometry(), QRectF(widget.pos(), widget.size()));

    QFETCH(QPointF, pos);
    QFETCH(QSizeF, size);
    widget.setPos(pos);
    widget.resize(size);
    QCOMPARE(widget.geometry(), QRectF(pos, size));
}

void tst_QGraphicsWidget::getContentsMargins_data()
{
    QTest::addColumn<qreal>("left");
    QTest::addColumn<qreal>("top");
    QTest::addColumn<qreal>("right");
    QTest::addColumn<qreal>("bottom");
    QTest::newRow("null") << (qreal)0 << (qreal)0 << (qreal)0 << (qreal)0;
    QTest::newRow("something") << (qreal)10 << (qreal)5 << (qreal)3 << (qreal)7;
    QTest::newRow("real") << (qreal)1.7 << (qreal)5.9 << (qreal)3.2 << (qreal)9.7;
}

// void getContentsMargins(qreal* left, qreal* top, qreal* right, qreal* bottom) const public
void tst_QGraphicsWidget::getContentsMargins()
{
    qreal gleft;
    qreal gtop;
    qreal gright;
    qreal gbottom;

    SubQGraphicsWidget widget;
    widget.getContentsMargins(&gleft, &gtop, &gright, &gbottom);
    QCOMPARE(gleft, (qreal)0);
    QCOMPARE(gtop, (qreal)0);
    QCOMPARE(gright, (qreal)0);
    QCOMPARE(gbottom, (qreal)0);

    QFETCH(qreal, left);
    QFETCH(qreal, top);
    QFETCH(qreal, right);
    QFETCH(qreal, bottom);
    int oldEventCounts = widget.eventCount;
    widget.setContentsMargins(left, top, right, bottom);
    QVERIFY(left == 0 || oldEventCounts != widget.eventCount);
    widget.getContentsMargins(&gleft, &gtop, &gright, &gbottom);
    QCOMPARE(gleft, left);
    QCOMPARE(gtop, top);
    QCOMPARE(gright, right);
    QCOMPARE(gbottom, bottom);
}

Q_DECLARE_METATYPE(Qt::LayoutDirection)
void tst_QGraphicsWidget::initStyleOption_data()
{
    QTest::addColumn<bool>("enabled");
    QTest::addColumn<bool>("focus");
    QTest::addColumn<bool>("underMouse");
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::addColumn<QSizeF>("size");
    QTest::addColumn<QPalette>("palette");
    QTest::addColumn<QString>("fontName");
    QTest::newRow("none") << false << false << false << Qt::LeftToRight << QSizeF(0, 0) << QPalette() << QString();
    QTest::newRow("all") << true << true << true << Qt::RightToLeft << QSizeF(300, 300) << QPalette(Qt::magenta) << "Helvetica";
    QTest::newRow("rand") << true << false << false << Qt::RightToLeft << QSizeF(1, 0) << QPalette(Qt::darkCyan) << "Times";
}

// void initStyleOption(QStyleOption* option) const public
void tst_QGraphicsWidget::initStyleOption()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.setAlignment(Qt::AlignTop | Qt::AlignLeft);
    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    widget->setAcceptsHoverEvents(true);
    QStyleOption option;
    scene.addItem(widget);

    QFETCH(QSizeF, size);
    widget->resize(size);

    QFETCH(bool, enabled);
    widget->setEnabled(enabled);
    QFETCH(bool, focus);
    if (focus) {
        widget->setFlag(QGraphicsItem::ItemIsFocusable, true);
        widget->setFocus();
        QVERIFY(widget->hasFocus());
    }
    QFETCH(bool, underMouse);
    if (underMouse) {
        view.resize(300, 300);
        view.show();
        QTest::qWait(125);
        sendMouseMove(view.viewport(), view.mapFromScene(widget->mapToScene(widget->boundingRect().center())));
    }

    QFETCH(QPalette, palette);
    widget->setPalette(palette);

    QFETCH(QString, fontName);
    widget->setFont(QFont(fontName));

    // The test
    widget->initStyleOption(&option);

    bool isEnabled = option.state & QStyle::State_Enabled;
    QCOMPARE(isEnabled, enabled);
    bool hasFocus = option.state & QStyle::State_HasFocus;
    QCOMPARE(hasFocus, focus);
    bool isUnderMouse = option.state & QStyle::State_MouseOver;
#ifndef Q_OS_WINCE
    QCOMPARE(isUnderMouse, underMouse);
#endif
    // if (layoutDirection != Qt::LeftToRight)
    //QEXPECT_FAIL("", "QApplicaiton::layoutDirection doesn't propagate to QGraphicsWidget", Continue);
    //QCOMPARE(option.direction, layoutDirection);
    QCOMPARE(option.rect, QRectF(QPointF(), size).toRect());
    QCOMPARE(option.palette, palette.resolve(QApplication::palette()));
    QCOMPARE(option.fontMetrics, QFontMetrics(widget->font()));
}

void tst_QGraphicsWidget::layout_data()
{
    QTest::addColumn<int>("childCount");
    QTest::newRow("empty") << 0;
    QTest::newRow("10") << 10;
}

// QGraphicsLayout* layout() const public
void tst_QGraphicsWidget::layout()
{
    SubQGraphicsWidget widget;
    widget.setContentsMargins(10, 5, 50, 100);
    QCOMPARE(widget.layout(), (QGraphicsLayout *)0);
    QFETCH(int, childCount);

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < childCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget;
        layout->addItem(item);
        children.append(item);
    }
    widget.setLayout(layout);

    QTest::qWait(250);

    QCOMPARE(widget.layout(), static_cast<QGraphicsLayout*>(layout));
    for (int i = 0; i < children.count(); ++i) {
        SubQGraphicsWidget *item = children[i];
        QCOMPARE(item->parentWidget(), (QGraphicsWidget *)&widget);
        QVERIFY(item->geometry() != QRectF(0, 0, -1, -1));
    }

    // don't crash
    widget.setLayout(0);
}

void tst_QGraphicsWidget::layoutDirection_data()
{
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::newRow("rtl") << Qt::RightToLeft;
    QTest::newRow("ltr") << Qt::LeftToRight;
}

// Qt::LayoutDirection layoutDirection() const public
void tst_QGraphicsWidget::layoutDirection()
{
    QFETCH(Qt::LayoutDirection, layoutDirection);
    QGraphicsScene scene;
    QGraphicsView *view = new QGraphicsView(&scene);
    SubQGraphicsWidget widget;
    scene.addItem(&widget);
    QCOMPARE(widget.layoutDirection(), Qt::LeftToRight);
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), false);

    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < 10; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&widget);
        children.append(item);
        QCOMPARE(item->testAttribute(Qt::WA_SetLayoutDirection), false);
    }
    widget.setLayoutDirection(layoutDirection);
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), true);
    view->show();
    QTest::qWait(100);
    for (int i = 0; i < children.count(); ++i) {
        QCOMPARE(children[i]->layoutDirection(), layoutDirection);
        QCOMPARE(children[i]->testAttribute(Qt::WA_SetLayoutDirection), false);
        view->repaint();
        QTest::qWait(200);
        QCOMPARE(children[i]->m_painterLayoutDirection, layoutDirection);
    }
    delete view;
}

void tst_QGraphicsWidget::paint_data()
{
    // currently QGraphicsWidget doesn't paint or do anything ...
}

// void paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget* widget) public
void tst_QGraphicsWidget::paint()
{
    SubQGraphicsWidget widget;
    QPainter painter;
    QStyleOptionGraphicsItem option;
    widget.paint(&painter, &option, 0); // check that widget = 0 works.
}

void tst_QGraphicsWidget::palettePropagation()
{
    QGraphicsWidget *root = new QGraphicsWidget;
    QGraphicsWidget *child0 = new QGraphicsWidget(root);
    QGraphicsWidget *child1 = new QGraphicsWidget(child0);
    QGraphicsWidget *child2 = new QGraphicsWidget(child1);
    QGraphicsScene scene;
    scene.addItem(root);

    // These colors are unlikely to be imposed on the default palette of
    // QWidget ;-).
    QColor sysPalText(21, 22, 23);
    QColor sysPalToolTipBase(12, 13, 14);
    QColor overridePalText(42, 43, 44);
    QColor overridePalToolTipBase(45, 46, 47);
    QColor sysPalButton(99, 98, 97);

    // Check that only the application fonts apply.
    QPalette appPal = QApplication::palette();
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(root->palette(), appPal);
    QCOMPARE(child0->palette(), appPal);
    QCOMPARE(child1->palette(), appPal);

    // Set child0's Text, and set ToolTipBase on child1.
    QPalette textPalette;
    textPalette.setColor(QPalette::Text, overridePalText);
    child0->setPalette(textPalette);
    QPalette toolTipPalette;
    toolTipPalette.setColor(QPalette::ToolTipBase, overridePalToolTipBase);
    child1->setPalette(toolTipPalette);

    // Check that the above settings propagate correctly.
    QCOMPARE(root->palette(), appPal);
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(child0->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child0->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child1->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);
    QCOMPARE(child2->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child2->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    QGraphicsWidget *child3 = new QGraphicsWidget(child2);
    QCOMPARE(child3->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child3->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    QGraphicsWidget *child4 = new QGraphicsWidget;
    child4->setParentItem(child3);
    QCOMPARE(child4->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child4->palette().color(QPalette::ToolTipBase), overridePalToolTipBase);

    // Replace the app palette for child2. Button should propagate but Text
    // should still be ignored. The previous ToolTipBase setting is gone.
    QPalette buttonPalette;
    buttonPalette.setColor(QPalette::Button, sysPalButton);
    child1->setPalette(buttonPalette);

    QCOMPARE(root->palette(), appPal);
    QCOMPARE(scene.palette(), appPal);
    QCOMPARE(child0->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child0->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child1->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child1->palette().color(QPalette::Button), sysPalButton);
    QCOMPARE(child2->palette().color(QPalette::Text), overridePalText);
    QCOMPARE(child2->palette().color(QPalette::ToolTipBase), appPal.color(QPalette::ToolTipBase));
    QCOMPARE(child2->palette().color(QPalette::Button), sysPalButton);
}

void tst_QGraphicsWidget::parentWidget_data()
{
    QTest::addColumn<int>("childrenCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// QGraphicsWidget* parentWidget() const public
void tst_QGraphicsWidget::parentWidget()
{
    QFETCH(int, childrenCount);
    SubQGraphicsWidget standAlongWidget;
    QGraphicsLineItem standAlongItem;

    SubQGraphicsWidget widgetChild(&standAlongWidget);
    SubQGraphicsWidget itemChild(&standAlongItem);

    QCOMPARE(standAlongWidget.parentWidget(), (QGraphicsWidget*)0);
    QCOMPARE(widgetChild.parentWidget(), static_cast<QGraphicsWidget*>(&standAlongWidget));
    QCOMPARE(itemChild.parentWidget(), (QGraphicsWidget*)0);

    for (int i = 0; i < childrenCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&standAlongWidget);
        QCOMPARE(item->parentWidget(), static_cast<QGraphicsWidget*>(&standAlongWidget));
    }
}

void tst_QGraphicsWidget::resize_data()
{
    QTest::addColumn<QSizeF>("size");
    QTest::newRow("null") << QSizeF();
    QTest::newRow("10x10") << QSizeF(10, 10);
    QTest::newRow("10x-1") << QSizeF(10, -1);
}

// void resize(qreal w, qreal h) public
void tst_QGraphicsWidget::resize()
{
    QFETCH(QSizeF, size);
    SubQGraphicsWidget widget;

    int oldEventCounts = widget.eventCount;
    QSizeF oldSize = widget.size();
    widget.resize(size);

    QSizeF boundedSize = size.expandedTo(widget.minimumSize()).boundedTo(widget.maximumSize());
    QCOMPARE(widget.eventCount, oldEventCounts + ((oldSize == boundedSize) ? 0 : 1));
    QCOMPARE(widget.size(), boundedSize);
}

Q_DECLARE_METATYPE(Qt::WidgetAttribute)
void tst_QGraphicsWidget::setAttribute_data()
{
    QTest::addColumn<Qt::WidgetAttribute>("attribute");
    QTest::addColumn<bool>("supported");
    QTest::newRow("WA_SetLayoutDirection") << Qt::WA_SetLayoutDirection << true;
    QTest::newRow("WA_RightToLeft") << Qt::WA_RightToLeft << true;
    QTest::newRow("WA_SetStyle") << Qt::WA_SetStyle << true;
    QTest::newRow("WA_Resized") << Qt::WA_Resized << true;
    QTest::newRow("unsupported") << Qt::WA_PaintOutsidePaintEvent << false;
}

// void setAttribute(Qt::WidgetAttribute attribute, bool on = true) public
void tst_QGraphicsWidget::setAttribute()
{
    QFETCH(Qt::WidgetAttribute, attribute);
    QFETCH(bool, supported);
    SubQGraphicsWidget widget;
    if (attribute == Qt::WA_PaintOutsidePaintEvent)
        QTest::ignoreMessage(QtWarningMsg, "QGraphicsWidget::setAttribute: unsupported attribute 13");
    widget.setAttribute(attribute);
    QCOMPARE(widget.testAttribute(attribute), supported);
}

void tst_QGraphicsWidget::setStyle_data()
{
    QTest::addColumn<QString>("style");
    QTest::newRow("null") << "";
    QTest::newRow("cleanlooks") << "QCleanlooksStyle";
}

// void setStyle(QStyle* style) public
void tst_QGraphicsWidget::setStyle()
{
    SubQGraphicsWidget widget;
    QCleanlooksStyle cleanlooksStyle;

    int oldEventCounts = widget.eventCount;

    QFETCH(QString, style);
    if (style == "QCleanlooksStyle") {
        widget.setStyle(&cleanlooksStyle);
        QCOMPARE(widget.style(), static_cast<QStyle*>(&cleanlooksStyle));
    } else {
        widget.setStyle(0);
        QVERIFY(widget.style() != (QStyle *)0);
    }
    QCOMPARE(widget.eventCount, oldEventCounts + 1);
    QCOMPARE(widget.testAttribute(Qt::WA_SetStyle), !style.isEmpty());

    // cleanup
    widget.setStyle(0);
}

void tst_QGraphicsWidget::setTabOrder_data()
{
    QTest::addColumn<int>("childrenCount");
    QTest::newRow("0") << 0;
    QTest::newRow("1") << 1;
    QTest::newRow("10") << 10;
}

// void setTabOrder(QGraphicsWidget* first, QGraphicsWidget* second) public
void tst_QGraphicsWidget::setTabOrder()
{
    QFETCH(int, childrenCount);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif

    QGraphicsWidget *lastItem = 0;
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsWidget::setTabOrder(0, 0) is undefined");
    QGraphicsWidget::setTabOrder(0, 0);

    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < childrenCount; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget();
        item->setFocusPolicy(Qt::TabFocus);
        children.append(item);
        scene.addItem(item);
        if (lastItem)
            QGraphicsWidget::setTabOrder(lastItem, item);
        lastItem = item;
    }

    if (!children.isEmpty()) {
        QGraphicsWidget *first = children.first();
        view.viewport()->setFocus();
        QTest::qWait(250);
        QVERIFY(view.viewport()->hasFocus());
        first->setFocus();
        QVERIFY(first->hasFocus());
        QVERIFY(scene.hasFocus());
        QVERIFY(view.viewport()->hasFocus());

        int currentItem = 0;
        while (currentItem < children.count() - 1) {
            QTest::keyPress(view.viewport(), Qt::Key_Tab);
            ++currentItem;
            QVERIFY(children[currentItem % children.size()]->hasFocus());
        }
    }
}

static bool compareFocusChain(QGraphicsView *view, const QList<QGraphicsItem*> &order)
{
    QGraphicsScene *scene = view->scene();
    QStringList actual;
    QGraphicsItem *oldFocusItem = scene->focusItem();
    for (int i = 0; i < order.count(); ++i) {
        QGraphicsItem *focusItem = scene->focusItem();
        actual << focusItem->data(0).toString();
        //qDebug() << "i:" << i << "expected:" << QString::number(uint(order.at(i)), 16) << QString::number(uint(focusItem), 16);
        if (focusItem != order.at(i)) {
            qDebug() << "actual:"  << actual;
            scene->setFocusItem(oldFocusItem);
            return false;
        }
        if (i < order.count() - 1)
            QTest::keyPress(view, Qt::Key_Tab);
    }
    scene->setFocusItem(oldFocusItem);
    return true;
}

void tst_QGraphicsWidget::setTabOrderAndReparent()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    int i;
    QGraphicsWidget *w1, *w2, *w3, *w4;
    for (i = 1; i < 4; ++i) {
        QGraphicsWidget *wid = new QGraphicsWidget;
        wid->setFocusPolicy(Qt::StrongFocus);
        wid->setData(0, QString::fromAscii("w%1").arg(i));
        scene.addItem(wid);
        if (i == 1)
            w1 = wid;
        else if (i == 2)
            w2 = wid;
        else if (i == 3)
            w3 = wid;
    }

    w1->setFocus();
    QVERIFY(w1->hasFocus());
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w2 << w3));

    QGraphicsWidget *p = new QGraphicsWidget;
    p->setData(0, QLatin1String("parent"));
    p->setFocusPolicy(Qt::StrongFocus);
    
    w1->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w2 << w3));
    
    w1->setParentItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() <<  w2 << w3));
    
    w2->setParentItem(p);
    w3->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w3));
    w3->setParentItem(p);
    QCOMPARE(scene.focusItem(), static_cast<QGraphicsItem*>(0));
    
    scene.addItem(p);
    p->setFocus();
    
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << p << w1 << w2 << w3));
    delete p;

    for (i = 1; i < 5; ++i) {
        QGraphicsWidget *wid = new QGraphicsWidget;
        wid->setFocusPolicy(Qt::StrongFocus);
        wid->setData(0, QString::fromAscii("w%1").arg(i));
        scene.addItem(wid);
        if (i == 1)
            w1 = wid;
        else if (i == 2)
            w2 = wid;
        else if (i == 3)
            w3 = wid;
        else if (i == 4)
            w4 = wid;
    }
    w4->setParentItem(w1);
    QGraphicsWidget::setTabOrder(w1, w4);
    w1->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w1 << w4 << w2 << w3));
    
    p = new QGraphicsWidget;
    p->setData(0, QLatin1String("parent"));
    p->setFocusPolicy(Qt::StrongFocus);

    w1->setParentItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w2 << w3));
    
    scene.addItem(p);
    w2->setFocus();
    QVERIFY(compareFocusChain(&view, QList<QGraphicsItem*>() << w2 << w3 << p << w1 << w4));
}

void tst_QGraphicsWidget::topLevelWidget_data()
{
    QTest::addColumn<QString>("str");
    QTest::newRow("test one") << "foo";
}

// QGraphicsWidget* topLevelWidget() const public
void tst_QGraphicsWidget::topLevelWidget()
{
    QFETCH(QString, str);
    SubQGraphicsWidget widget;
    QCOMPARE(widget.topLevelWidget(), (QGraphicsWidget *)&widget);
}

void tst_QGraphicsWidget::unsetLayoutDirection_data()
{
    QTest::addColumn<Qt::LayoutDirection>("layoutDirection");
    QTest::newRow("rtl") << Qt::RightToLeft;
    QTest::newRow("ltr") << Qt::LeftToRight;
}

// void unsetLayoutDirection() public
void tst_QGraphicsWidget::unsetLayoutDirection()
{
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QFETCH(Qt::LayoutDirection, layoutDirection);
    SubQGraphicsWidget widget;
    QCOMPARE(Qt::LeftToRight, widget.layoutDirection());

    QList<SubQGraphicsWidget*> children;
    for (int i = 0; i < 10; ++i) {
        SubQGraphicsWidget *item = new SubQGraphicsWidget(&widget);
        children.append(item);
    }
    widget.setLayoutDirection(layoutDirection);
    widget.unsetLayoutDirection();
    QCOMPARE(widget.testAttribute(Qt::WA_SetLayoutDirection), false);
    for (int i = 0; i < children.count(); ++i) {
        QCOMPARE(children[i]->layoutDirection(), Qt::LeftToRight);
    }
}

void tst_QGraphicsWidget::focusNextPrevChild_data()
{
    QTest::addColumn<QString>("str");
    QTest::newRow("test one") << "foo";
}

// bool focusNextPrevChild(bool next) protected
void tst_QGraphicsWidget::focusNextPrevChild()
{
    QFETCH(QString, str);
    SubQGraphicsWidget widget;
    // ### write test after just calling it stops crashing :)
    widget.call_focusNextPrevChild(true);
}

void tst_QGraphicsWidget::verifyFocusChain()
{
    QGraphicsScene scene;
    {
        // parent/child focus
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        w->setFocusPolicy(Qt::StrongFocus);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w->setFocus();
        QVERIFY(w->hasFocus());
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_1->hasFocus());
        delete w;
    }

    {
        // delete item in focus chain and verify chain
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        SubQGraphicsWidget *w1_2 = new SubQGraphicsWidget(w);
        SubQGraphicsWidget *w1_3 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        w1_2->setFocusPolicy(Qt::StrongFocus);
        w1_3->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w1_1->setFocus();
        QVERIFY(w1_1->hasFocus());
        QCOMPARE(w->call_focusNextPrevChild(true), true);
        QVERIFY(w1_2->hasFocus());
        QCOMPARE(w->call_focusNextPrevChild(true), true);
        QVERIFY(w1_3->hasFocus());
        w1_1->setFocus();
        delete w1_2;
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_3->hasFocus());
        delete w;
    }
    {
        // parent/child focus
        SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
        w->setFocusPolicy(Qt::StrongFocus);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w);
        w->setFocus();
        QVERIFY(w->hasFocus());
        w->call_focusNextPrevChild(true);
        QVERIFY(w1_1->hasFocus());
        delete w;
    }
    {
        // remove the tabFocusFirst widget from the scene.
        QWidget *window = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout;
        window->setLayout(layout);
        QLineEdit *lineEdit = new QLineEdit;
        layout->addWidget(lineEdit);
        QGraphicsView *view = new QGraphicsView(&scene);
        scene.setSceneRect(-20, -20, 200, 50);
        layout->addWidget(view);
        view->setMinimumSize(150, 50);
        SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget;
        w1_1->setData(0, "w1_1");
        w1_1->setGeometry(0,0,25, 25);
        w1_1->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w1_1);
        SubQGraphicsWidget *w1_2 = new SubQGraphicsWidget;
        w1_2->setData(0, "w1_2");
        w1_2->setGeometry(25,0,25, 25);
        w1_2->setFocusPolicy(Qt::StrongFocus);
        scene.addItem(w1_2);
        window->show();
#ifdef Q_WS_X11
        qt_x11_wait_for_window_manager(window);
#endif
        
        lineEdit->setFocus();
        QTest::qWait(250);
        QVERIFY(lineEdit->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTest::qWait(250);
        QVERIFY(w1_1->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTest::qWait(250);
        QVERIFY(w1_2->hasFocus());

        // remove the tabFocusFirst and insert new item
        delete w1_1;            // calls _q_removeItemLater
        QTest::qWait(250);
        SubQGraphicsWidget *w1_3 = new SubQGraphicsWidget;
        w1_3->setFocusPolicy(Qt::StrongFocus);
        w1_3->setData(0, "w1_3");
        w1_3->setGeometry(50,0,25, 25);
        scene.addItem(w1_3);
        QVERIFY(w1_2->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Backtab);
        QTest::qWait(250);
        QVERIFY(lineEdit->hasFocus());
        // tabFocusFirst should now point to w1_2
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTest::qWait(250);
        QVERIFY(w1_2->hasFocus());
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTest::qWait(250);
        QVERIFY(w1_3->hasFocus());
        scene.removeItem(w1_2);   // does not call _q_removeItemLater
        delete w1_2;            // calls _q_removeItemLater

        SubQGraphicsWidget *w1_4 = new SubQGraphicsWidget;
        w1_4->setFocusPolicy(Qt::StrongFocus);
        w1_4->setData(0, "w1_4");
        w1_4->setGeometry(75,0,25, 25);
        scene.addItem(w1_4);        
        QVERIFY(w1_3->hasFocus());
        QTest::qWait(250);
        QVERIFY(compareFocusChain(view, QList<QGraphicsItem*>() << w1_3 << w1_4));
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Backtab);
        QTest::qWait(250);
        QVERIFY(lineEdit->hasFocus());
        // tabFocusFirst should now point to w1_3
        QTest::keyPress(QApplication::focusWidget(), Qt::Key_Tab);
        QTest::qWait(250);
        QVERIFY(w1_3->hasFocus());        
        QTest::qWait(250);
        QVERIFY(compareFocusChain(view, QList<QGraphicsItem*>() << w1_3 << w1_4));
        delete window;
    }
}

void tst_QGraphicsWidget::updateFocusChainWhenChildDie()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    view.show();
    // delete item in focus chain with no focus and verify chain
    SubQGraphicsWidget *parent = new SubQGraphicsWidget(0, Qt::Window);
    SubQGraphicsWidget *w = new SubQGraphicsWidget(0, Qt::Window);
    w->resize(50,50);
    w->resize(100,100);
    SubQGraphicsWidget *w1_1 = new SubQGraphicsWidget(w);
    w1_1->setFocusPolicy(Qt::StrongFocus);
    w->setFocusPolicy(Qt::StrongFocus);
    scene.addItem(w);
    scene.addItem(parent);
    w1_1->setFocus();

    QVERIFY(w1_1->hasFocus());
    QWidget myWidget(0);
    QLineEdit edit(&myWidget);
    myWidget.show();
    edit.setFocus();
    QTRY_VERIFY(edit.hasFocus());
    delete w1_1;
    myWidget.hide();
    w->setParentItem(parent);
    //We don't crash perfect
    QVERIFY(w);
    QTest::mouseClick(view.viewport(), Qt::LeftButton, 0);
    QTRY_COMPARE(qApp->activeWindow(), static_cast<QWidget *>(&view));
    QTRY_COMPARE(scene.focusItem(), static_cast<QGraphicsItem *>(w));
}

void tst_QGraphicsWidget::sizeHint_data()
{
    QTest::addColumn<bool>("layout");
    QTest::newRow("no layout") << false;
    QTest::newRow("layout") << true;
}

// QSizeF sizeHint(Qt::SizeHint which, QSizeF const& constraint = QSizeF()) const protected
void tst_QGraphicsWidget::sizeHint()
{
    QFETCH(bool, layout);
    SubQGraphicsWidget widget;

    if (layout) {
        QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;
        widget.setLayout(layout);
    }
    widget.call_sizeHint(Qt::MinimumSize, QSizeF());
}

void tst_QGraphicsWidget::consistentPosSizeGeometry_data()
{
    QTest::addColumn<QSizeF>("minSize");
    QTest::addColumn<QSizeF>("maxSize");
    QTest::addColumn<QRectF>("geometry");
    QTest::addColumn<QRectF>("expectedGeometry");

    QTest::newRow("size is valid") << QSizeF(0, 0) << QSizeF(200, 200) << QRectF(0, 0, 100, 100) << QRectF(0, 0, 100, 100);
    QTest::newRow("size is larger than max") << QSizeF(0, 0) << QSizeF(50, 50) << QRectF(0, 0, 100, 100) << QRectF(0, 0, 50, 50);
    QTest::newRow("size is smaller than min") << QSizeF(50, 50) << QSizeF(150, 150) << QRectF(0, 0, 10, 10) << QRectF(0, 0, 50, 50);
}

void tst_QGraphicsWidget::consistentPosSizeGeometry()
{
    QFETCH(QSizeF, minSize);
    QFETCH(QSizeF, maxSize);
    QFETCH(QRectF, geometry);
    QFETCH(QRectF, expectedGeometry);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *w = new QGraphicsWidget;
    scene.addItem(w);
    w->setMinimumSize(minSize);
    w->setMaximumSize(maxSize);
    w->setGeometry(geometry);
    QCOMPARE(w->geometry(), expectedGeometry);
    QCOMPARE(w->pos(), expectedGeometry.topLeft());
    QCOMPARE(w->size(), expectedGeometry.size());

    QRectF otherGeom = QRectF(QPointF(12.34,12.34), minSize);
    w->setGeometry(otherGeom);
    QCOMPARE(w->geometry(), otherGeom);
    QCOMPARE(w->pos(), otherGeom.topLeft());
    QCOMPARE(w->size(), otherGeom.size());

    w->setPos(geometry.topLeft());
    QCOMPARE(w->geometry().topLeft(), expectedGeometry.topLeft());
    QCOMPARE(w->pos(), expectedGeometry.topLeft());

    w->resize(geometry.size());
    QCOMPARE(w->geometry().size(), expectedGeometry.size());
    QCOMPARE(w->geometry(), expectedGeometry);

    view.show();

}


enum WhichSize {
    MinimumWidth,
    PreferredWidth,
    MaximumWidth,
    MinimumHeight,
    PreferredHeight,
    MaximumHeight,
    MinimumSize,
    PreferredSize,
    MaximumSize,
    MinimumSizeHint,
    PreferredSizeHint,
    MaximumSizeHint,
    Size,
    None,
};

typedef QPair<int, QVariant> Inst;

Q_DECLARE_METATYPE(Inst)
Q_DECLARE_METATYPE(QVector<Inst>)

void tst_QGraphicsWidget::setSizes_data()
{

    QTest::addColumn<QVector<Inst> >("inputInstructions");
    QTest::addColumn<QVector<Inst> >("compareInstructions");

    QTest::newRow("minSize1") << (QVector<Inst>() << Inst(Size, QSize(25, 25)) << Inst(MinimumSize, QSize(10, 10)))
                                << (QVector<Inst>() << Inst(Size, QSize(25,25)));
    QTest::newRow("minSize2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumSize, QSizeF(25, 25)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(25, 25)));
    QTest::newRow("minWidth1") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumWidth, 5.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)));
    QTest::newRow("minWidth2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumWidth, 25.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(25, 20)));
    QTest::newRow("minHeight1") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumHeight, 5.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)));
    QTest::newRow("minHeight2") << (QVector<Inst>() << Inst(Size, QSizeF(20, 20)) << Inst(MinimumHeight, 25.0))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(20, 25)));
    QTest::newRow("maxSize1") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(30, 30)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 30)));
    QTest::newRow("maxSize2") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(30, -1)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 40)));
    QTest::newRow("maxSize3") << (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(-1, 30)))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(40, 30)));
    QTest::newRow("maxWidth1")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumWidth, 30))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(30, 40)));
    QTest::newRow("maxHeight")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumHeight, 20))
                                 << (QVector<Inst>() << Inst(Size, QSizeF(40, 20)));
    QTest::newRow("unsetMinSize")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MinimumSize, QSizeF(-1, -1)))
                                 << (QVector<Inst>() << Inst(MinimumSize, QSizeF(5, 5)));
    QTest::newRow("unsetMaxSize")<< (QVector<Inst>() << Inst(Size, QSizeF(40, 40)) << Inst(MaximumSize, QSizeF(-1, -1)))
                                 << (QVector<Inst>() << Inst(MaximumSize, QSizeF(500, 500)));
    QTest::newRow("unsetMinSize, expand size to minimumSizeHint") << (QVector<Inst>() 
                                        << Inst(MinimumSize, QSize(0, 0))
                                        << Inst(Size, QSize(1,1))
                                        << Inst(MinimumSize, QSize(-1.0, -1.0))
                                        )
                                    << (QVector<Inst>()
                                        << Inst(Size, QSize(5,5))
                                        << Inst(MinimumSize, QSize(5,5))
                                        );

}

void tst_QGraphicsWidget::setSizes()
{
    QFETCH(QVector<Inst>, inputInstructions);
    QFETCH(QVector<Inst>, compareInstructions);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    SizeHinter *widget = new SizeHinter(0, Qt::Window);
    QSizeF min = QSizeF(10, 10);
    QSizeF pref = QSizeF(25, 25);
    QSizeF max = QSizeF(50, 50);

    int i;
    for (i = 0; i < inputInstructions.count(); ++i) {
        Inst input = inputInstructions.at(i);

        // defaults
        switch (input.first) {
            case MinimumSize:
                min = input.second.toSizeF();
                break;
            case PreferredSize:
                pref = input.second.toSizeF();
                break;
            case MaximumSize:
                max = input.second.toSizeF();
                break;
            case Size:
                widget->resize(input.second.toSizeF());
                break;
            case MinimumWidth:
                widget->setMinimumWidth(qreal(input.second.toDouble()));
                break;
            case PreferredWidth:
                widget->setPreferredWidth(qreal(input.second.toDouble()));
                break;
            case MaximumWidth:
                widget->setMaximumWidth(qreal(input.second.toDouble()));
                break;
            case MinimumHeight:
                widget->setMinimumHeight(qreal(input.second.toDouble()));
                break;
            case PreferredHeight:
                widget->setPreferredHeight(qreal(input.second.toDouble()));
                break;
            case MaximumHeight:
                widget->setMaximumHeight(qreal(input.second.toDouble()));
                break;
            case MinimumSizeHint:
                widget->setSizeHint(Qt::MinimumSize, input.second.toSizeF());
                break;
            case PreferredSizeHint:
                widget->setSizeHint(Qt::PreferredSize, input.second.toSizeF());
                break;
            case MaximumSizeHint:
                widget->setSizeHint(Qt::MaximumSize, input.second.toSizeF());
                break;
            default:
                qWarning("instruction not implemented");
                break;
        }
    }

    widget->setMinimumSize(min);
    widget->setPreferredSize(pref);
    widget->setMaximumSize(max);

    QApplication::processEvents();
    
    for (i = 0; i < compareInstructions.count(); ++i) {
        Inst input = compareInstructions.at(i);
        switch (input.first) {
            case MinimumSize:
                QCOMPARE(widget->minimumSize(), input.second.toSizeF());
                break;
            case PreferredSize:
                QCOMPARE(widget->preferredSize(), input.second.toSizeF());
                break;
            case MaximumSize:
                QCOMPARE(widget->maximumSize(), input.second.toSizeF());
                break;
            case Size:
                QCOMPARE(widget->size(), input.second.toSizeF());
                break;
            case MinimumWidth:
                QCOMPARE(widget->minimumWidth(), qreal(input.second.toDouble()));
                break;
            case PreferredWidth:
                QCOMPARE(widget->preferredWidth(), qreal(input.second.toDouble()));
                break;
            case MaximumWidth:
                QCOMPARE(widget->maximumWidth(), qreal(input.second.toDouble()));
                break;
            default:
                qWarning("instruction not implemented");
                break;
        }
    }
    delete widget;
}

void tst_QGraphicsWidget::closePopupOnOutsideClick()
{
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Popup);
    widget->resize(100, 100);

    QGraphicsScene scene;
    scene.addItem(widget);

    QGraphicsSceneMouseEvent event(QEvent::GraphicsSceneMousePress);
    event.ignore();
    event.setScenePos(QPointF(50, 50));
    qApp->sendEvent(&scene, &event);

    QVERIFY(widget->isVisible());
    QVERIFY(event.isAccepted());

    event.ignore();
    event.setScenePos(QPointF(150, 150));
    qApp->sendEvent(&scene, &event);

    QVERIFY(!widget->isVisible());
    QVERIFY(event.isAccepted());
}

void tst_QGraphicsWidget::task236127_bspTreeIndexFails()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    widget->resize(10, 10);
    widget2->resize(10, 10);
    widget2->setZValue(1);

    QGraphicsScene scene;
    scene.addItem(widget);
    scene.addItem(widget2);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(50);

    QVERIFY(!scene.itemAt(25, 25));
    widget->setGeometry(0, 112, 360, 528);
    QCOMPARE(scene.itemAt(15, 120), (QGraphicsItem *)widget);
    widget2->setGeometry(0, 573, 360, 67);
    QCOMPARE(scene.itemAt(15, 120), (QGraphicsItem *)widget);
    QCOMPARE(scene.itemAt(50, 585), (QGraphicsItem *)widget2);
}

void tst_QGraphicsWidget::defaultSize()
{
    SubQGraphicsWidget *widget = new SubQGraphicsWidget;
    widget->setMinimumSize(40, 40);
    QGraphicsScene scene;
    scene.addItem(widget);

    QGraphicsView view(&scene);
    view.show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(&view);
#endif
    QTest::qWait(50);
    QSizeF initialSize = widget->size();

    widget->resize(initialSize);
    QCOMPARE(widget->geometry().size(), initialSize);
    widget->setVisible(false);
    widget->setMinimumSize(10, 10);
    widget->setPreferredSize(60, 60);
    widget->setMaximumSize(110, 110);
    widget->setVisible(true);
    QTest::qWait(50);
    // should still have its size set to initialsize
    QCOMPARE(widget->geometry().size(), initialSize);

}

void tst_QGraphicsWidget::shortcutsDeletion()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsWidget *widget2 = new QGraphicsWidget;
    widget->setMinimumSize(40, 40);
    QWidgetAction *del = new QWidgetAction(widget);
    del->setIcon(QIcon("edit-delete"));
    del->setShortcut(Qt::Key_Delete);
    del->setShortcutContext(Qt::WidgetShortcut);
    widget2->addAction(del);
    widget2->addAction(del);
    delete widget;
}

class ProxyStyle : public QCommonStyle
{
public:
    ProxyStyle(QStyle *proxyStyle) : QCommonStyle()
    {
        m_proxyStyle = proxyStyle;
    }

    int pixelMetric(QStyle::PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const
    {
        return m_proxyStyle->pixelMetric(metric, option, widget);
    }

private:
    QStyle *m_proxyStyle;
};

class StyledGraphicsWidget : public QGraphicsWidget
{
public:
    StyledGraphicsWidget(bool useOwnStyle) : QGraphicsWidget(), m_style(0) {
        if (useOwnStyle) {
            QStyle *oldStyle = style();
            m_style = new ProxyStyle(oldStyle);
            setStyle(m_style);
        }

        style()->pixelMetric(QStyle::PM_SmallIconSize); // crash when style() is still in widgetStyles
    }

    ~StyledGraphicsWidget() {
        delete m_style;
    }

private:
    QStyle *m_style;
};

void tst_QGraphicsWidget::task243004_setStyleCrash()
{
    QGraphicsItem *item1 = new StyledGraphicsWidget(true);
    delete item1; // item1 not removed from widgetStyles

    QGraphicsItem *item2 = new StyledGraphicsWidget(false);
    delete item2;
}

class GraphicsWidget_task250119 : public QGraphicsWidget
{
public:
    GraphicsWidget_task250119()
        : shortcutEvents(0)
    {
        setFocusPolicy(Qt::StrongFocus);
        resize(100, 100);
    }

    int shortcutEvents;

private:
    bool event(QEvent *event)
    {
        if (event->type() == QEvent::Shortcut)
            shortcutEvents++;
        return QGraphicsWidget::event(event);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
    {
        if (hasFocus()) {
            painter->setPen(QPen(Qt::black, 0, Qt::DashLine));
            painter->drawRect(rect());
        }
        painter->setPen(QPen(Qt::black, 0, Qt::SolidLine));
        painter->fillRect(rect().adjusted(2, 2, -2, -2), Qt::yellow);
        painter->drawRect(rect().adjusted(2, 2, -2, -2));
    }
};

void tst_QGraphicsWidget::task250119_shortcutContext()
{
    QGraphicsScene scene;
    QGraphicsView view;
    view.setScene(&scene);
    view.show();
    QTest::qWait(100);


    // *** Event: ***

    GraphicsWidget_task250119 w_event;
    scene.addItem(&w_event);

    const int id = w_event.grabShortcut(Qt::Key_A, Qt::WidgetWithChildrenShortcut);
    w_event.setShortcutEnabled(id, true);

    w_event.setFocus();
    QTest::keyPress(&view, Qt::Key_A);
    QCOMPARE(w_event.shortcutEvents, 1);

    w_event.clearFocus();
    QTest::keyPress(&view, Qt::Key_A);
    QCOMPARE(w_event.shortcutEvents, 1);

    scene.removeItem(&w_event);


    // *** Signal: ***

    GraphicsWidget_task250119 w_signal;
    scene.addItem(&w_signal);

    QAction action(0);
    action.setShortcut(Qt::Key_B);
    action.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    QSignalSpy spy(&action, SIGNAL(triggered()));

    w_signal.addAction(&action);

    w_signal.setFocus();
    QTest::keyPress(&view, Qt::Key_B);
    QCOMPARE(spy.count(), 1);

    w_signal.clearFocus();
    QTest::keyPress(&view, Qt::Key_B);
    QCOMPARE(spy.count(), 1);

    scene.removeItem(&w_signal);
}

QTEST_MAIN(tst_QGraphicsWidget)
#include "tst_qgraphicswidget.moc"

#else // QT_NO_STYLE_CLEANLOOKS
QTEST_NOOP_MAIN
#endif

