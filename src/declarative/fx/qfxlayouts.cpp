/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QCoreApplication>
#include "qml.h"
#include "qmlstate.h"
#include "qmlstategroup.h"
#include "qmlstateoperations.h"
#include "private/qfxperf_p.h"
#include "qfxlayouts.h"
#include "qfxlayouts_p.h"


QT_BEGIN_NAMESPACE

/*!
    \internal
    \class QFxBaseLayout
    \brief The QFxBaseLayout class provides a base for QFx layouts.

    To create a QFx Layout, simple subclass QFxBaseLayout and implement
    doLayout(), which is automatically called when the layout might need
    updating.

    It is strongly recommended that in your implementation of doLayout()
    that you use the move, remove and add transitions when those conditions
    arise.

    \ingroup group_layouts
*/
QFxBaseLayout::QFxBaseLayout(AutoUpdateType at, QFxItem *parent)
    : QFxItem(*(new QFxBaseLayoutPrivate), parent)
{
    Q_D(QFxBaseLayout);
    d->init(at);
}

QFxBaseLayout::QFxBaseLayout(QFxBaseLayoutPrivate &dd, AutoUpdateType at, QFxItem *parent)
    : QFxItem(dd, parent)
{
    Q_D(QFxBaseLayout);
    d->init(at);
}

/*!
  \property QFxBaseLayout::spacing
  \brief the amount of spacing between items (in px)

  Note that the subclass is repsonsible for ensuring this.
*/
int QFxBaseLayout::spacing() const
{
    Q_D(const QFxBaseLayout);
    return d->_spacing;
}

void QFxBaseLayout::setSpacing(int s)
{
    Q_D(QFxBaseLayout);
    if (s==d->_spacing)
        return;
    d->_spacing = s;
    preLayout();
}

/*!
  \property QFxBaseLayout::margin
  \brief the size of the margin around all items (in px)

*/
int QFxBaseLayout::margin() const
{
    Q_D(const QFxBaseLayout);
    return d->_margin;
}

void QFxBaseLayout::setMargin(int s)
{
    Q_D(QFxBaseLayout);
    if (s==d->_margin)
        return;
    d->_margin = s;
    preLayout();
}


/*!
    \property QFxBaseLayout::move
    \brief the transition when moving an item.

    \qml
BaseLayout {
    id: layout
    y: 0
    move: Transition {
        NumberAnimation {
            properties: "y"
            ease: "easeOutBounce"
        }
    }
}
    \endqml
*/
QmlTransition *QFxBaseLayout::move() const
{
    Q_D(const QFxBaseLayout);
    return d->moveTransition;
}

void QFxBaseLayout::setMove(QmlTransition *mt)
{
    Q_D(QFxBaseLayout);
    d->moveTransition = mt;
}

/*!
    \property QFxBaseLayout::add
    \brief the transition when adding an item.

    \qml
BaseLayout {
    id: layout
    y: 0
    add: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 0
            to: 1
            duration: 500
        }
    }
}
    \endqml
*/
QmlTransition *QFxBaseLayout::add() const
{
    Q_D(const QFxBaseLayout);
    return d->addTransition;
}

void QFxBaseLayout::setAdd(QmlTransition *add)
{
    Q_D(QFxBaseLayout);
    d->addTransition = add;
}


/*!
    \property QFxBaseLayout::remove
    \brief the transition when removing an item.

    Note that the item may be 'removed' because its opacity is zero. This can make certain
    transitions difficult to see.
    \qml
BaseLayout {
    id: layout
    y: 0
    remove: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
    \endqml
*/
QmlTransition *QFxBaseLayout::remove() const
{
    Q_D(const QFxBaseLayout);
    return d->removeTransition;
}

void QFxBaseLayout::setRemove(QmlTransition *remove)
{
    Q_D(QFxBaseLayout);
    d->removeTransition = remove;
}

/*!
    \property QFxBaseLayout::item

    The item that is currently being laid out. Used to target transitions that apply
    only to the item being laid out, such as in the add transition.
*/

QFxItem *QFxBaseLayout::layoutItem() const
{
    Q_D(const QFxBaseLayout);
    return d->_layoutItem;
}

/*!
    \internal
*/
void QFxBaseLayout::setLayoutItem(QFxItem *li)
{
    Q_D(QFxBaseLayout);
    if (li == d->_layoutItem)
        return;
    d->_layoutItem = li;
    emit layoutItemChanged();
}

void QFxBaseLayout::componentComplete()
{
    QFxItem::componentComplete();
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BaseLayoutComponentComplete> cc;
#endif
    preLayout();
}

void QFxBaseLayout::childrenChanged()
{
    preLayout();
}

bool QFxBaseLayout::event(QEvent *e)
{
    Q_D(QFxBaseLayout);
    if (e->type() == QEvent::User) {
        d->_ep = false;
        d->_stableItems += d->_newItems;
        d->_leavingItems.clear();
        d->_newItems.clear();
        return true;
    }
    return QFxItem::event(e);
}

/*!
  Items that have just been added to the layout. This includes invisible items
  that have turned visible.
*/
QSet<QFxItem *>* QFxBaseLayout::newItems()
{
    Q_D(QFxBaseLayout);
    return &d->_newItems;
}

/*!
  Items that are visible in the layout, not including ones that have just been added.
*/
QSet<QFxItem *>* QFxBaseLayout::items()
{
    Q_D(QFxBaseLayout);
    return &d->_stableItems;
}

/*!
  Items that have just left the layout. This includes visible items
  that have turned invisible.
*/
QSet<QFxItem *>* QFxBaseLayout::leavingItems()
{
    Q_D(QFxBaseLayout);
    return &d->_leavingItems;
}

void QFxBaseLayout::preLayout()
{
    Q_D(QFxBaseLayout);
    if (!isComponentComplete() || d->_movingItem)
        return;

    if (!d->_ep) {
        d->_ep = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }
    QSet<QFxItem *> allItems;
    QList<QGraphicsItem *> children = childItems();
    for (int ii = 0; ii < children.count(); ++ii) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(ii));
        if (!child)
            continue;
        if (!d->_items.contains(child)){
            QObject::connect(child, SIGNAL(visibleChanged()),
                             this, SLOT(preLayout()));
            QObject::connect(child, SIGNAL(heightChanged()),
                             this, SLOT(preLayout()));
            QObject::connect(child, SIGNAL(widthChanged()),
                             this, SLOT(preLayout()));
            d->_items += child;
        }
        if (!child->isVisible()){
            if (d->_stableItems.contains(child)){
                d->_leavingItems += child;
                d->_stableItems -= child;
            }
        }else if (!d->_stableItems.contains(child)){
            d->_newItems+=child;
        }
        allItems += child;
    }
    QSet<QFxItem *> deletedItems = d->_items - allItems;
    foreach(QFxItem *child, d->_items){
        if (!allItems.contains(child)){
            if (!deletedItems.contains(child)) {
                QObject::disconnect(child, SIGNAL(visibleChanged()),
                                 this, SLOT(preLayout()));
                QObject::disconnect(child, SIGNAL(heightChanged()),
                                 this, SLOT(preLayout()));
                QObject::disconnect(child, SIGNAL(widthChanged()),
                                 this, SLOT(preLayout()));
            }
            d->_items -= child;
        }
    }
    d->_animated.clear();
    doLayout();
    //Set the layout's size to be the rect containing all children
    //Also set the margin
    qreal width=0;
    qreal height=0;
    foreach(QFxItem *item, d->_items){
        if (item->isVisible()){
            if (!d->_animated.contains(item)){
                setMovingItem(item);
                QPointF p(item->x(), item->y());
                if(d->aut & Horizontal)
                    p.setX(p.x() + d->_margin);
                if(d->aut & Vertical)
                    p.setY(p.y() + d->_margin);
                item->setPos(p);
                setMovingItem(0);
            }
            width = qMax(width, item->x() + item->width());
            height = qMax(height, item->y() + item->height());
        }
    }
    width += d->_margin;
    height+= d->_margin;

    if (d->aut & Horizontal)
        setWidth(int(width));
    else if (itemParent())
        setImplicitWidth(itemParent()->width());
    if (d->aut & Vertical)
        setHeight(int(height));
    else if (itemParent())
        setImplicitHeight(itemParent()->height());
    setLayoutItem(0);
}

//###This should be considered to move more centrally, as it seems useful
void QFxBaseLayout::applyTransition(const QList<QPair<QString, QVariant> >& changes, QFxItem* target, QmlTransition* trans)
{
    Q_D(QFxBaseLayout);
    if (!trans||!target)//TODO: if !trans, just apply changes
        return;
    setLayoutItem(target);

    QmlStateOperation::ActionList actions;

    for (int ii=0; ii<changes.size(); ++ii){

        QVariant val = changes[ii].second;
        if (d->_margin &&
            (changes[ii].first == QLatin1String("x") || 
             changes[ii].first == QLatin1String("y"))) {
                val = QVariant(val.toInt() + d->_margin);
        }

        actions << Action(target, changes[ii].first, val);

    }

    d->transitionManager.transition(actions, trans);
    d->_animated << target;
}

void QFxBaseLayout::setMovingItem(QFxItem *i)
{
    Q_D(QFxBaseLayout);
    d->_movingItem = i;
}

/*!
  Applies the layout's add transition to the \a target item.\a changes is a list of property,value
  pairs which will be changed on the target using the add transition.
*/
void QFxBaseLayout::applyAdd(const QList<QPair<QString, QVariant> >& changes, QFxItem* target)
{
    applyTransition(changes,target, add());
}

/*!
  Applies the layout's move transition to the \a target.\a changes is a list of property,value pairs
  which will be changed on the target using the move transition.
*/
void QFxBaseLayout::applyMove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target)
{
    applyTransition(changes,target, move());
}

/*!
  Applies the layout's remove transition to the \a target item.\a changes is a list of
  property,value pairs which will be changed on the target using the remove transition.
*/
void QFxBaseLayout::applyRemove(const QList<QPair<QString, QVariant> >& changes, QFxItem* target)
{
    applyTransition(changes,target, remove());
}

QML_DEFINE_TYPE(Qt,4.6,VerticalLayout,QFxVerticalLayout)
/*!
  \qmlclass VerticalLayout
  \brief The VerticalLayout item arranges its children in a vertical layout.
  \inherits Item

  The VerticalLayout item arranges its child items so that they are vertically
    aligned and not overlapping. Spacing between items can be added, as can a margin around all the items.

  The below example lays out differently shaped rectangles using a VerticalLayout.
  \table
  \row
  \o \image verticalLayout_example.png
  \o
  \qml
VerticalLayout {
    spacing: 2
    Rect { color: "red"; width: 50; height: 50 }
    Rect { color: "green"; width: 20; height: 50 }
    Rect { color: "blue"; width: 50; height: 20 }
}
  \endqml
  \endtable

  VerticalLayout also provides for transitions to be set when items are added, moved,
  or removed in the layout. Adding and removing apply both to items which are deleted
  or have their position in the document changed so as to no longer be children of the layout,
  as well as to items which have their opacity set to or from zero so as to appear or disappear.

  \table
  \row
  \o \image verticalLayout_transition.gif
  \o
  \qml
VerticalLayout {
    spacing: 2
    remove: ...
    add: ...
    move: ...
    ...
}
  \endqml
  \endtable


*/
/*!
    \qmlproperty Transition VerticalLayout::remove
    This property holds the transition to apply when removing an item from the layout.

    Removed can mean that either the object has been deleted or reparented, and thus is now longer a child of the layout, or that the object has had its opacity set to zero, and thus is no longer visible.

    Note that if the item counts as removed because its opacity is zero it will not be visible during the transition unless you set the opacity in the transition, like in the below example.

    \table
    \row
    \o \image layout-remove.gif
    \o
    \qml
VerticalLayout {
    id: layout
    remove: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
    \endqml
    \endtable

*/
/*!
    \qmlproperty Transition VerticalLayout::add
    This property holds the transition to be applied when adding an item to the layout.

    Added can mean that either the object has been created or reparented, and thus is now a child or the layout, or that the object has had its opacity increased from zero, and thus is now visible.

    \table
    \row
    \o \image layout-add.gif
    \o
    \qml
VerticalLayout {
    id: layout
    add: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 0
            to: 1
            duration: 500
        }
    }
}
    \endqml
    \endtable

*/
/*!
    \qmlproperty Transition VerticalLayout::move
    This property holds the transition to apply when moving an item within the layout.

    This can happen when other items are added or removed from the layout, or when items resize themselves.

    \table
    \row
    \o \image layout-move.gif
    \o
    \qml
VerticalLayout {
    id: layout
    move: Transition {
        NumberAnimation {
            properties: "y"
            ease: "easeOutBounce"
        }
    }
}
    \endqml
    \endtable
*/
/*!
    \qmlproperty Item VerticalLayout::item

    The item that is currently being laid out. Used to target transitions that apply
    only to the item being laid out, such as in the add transition.

*/
/*!
  \qmlproperty int VerticalLayout::spacing
  \qmlproperty int VerticalLayout::margin

  spacing and margin allow you to control the empty space surrounding
  items in layouts.

  spacing is the amount in pixels left empty between each adjacent
  item.  margin is the amount in pixels which will be left empty
  around the inside edge of the layout.  Both default to 0.

  The below example places a GridLayout containing a red, a blue and a
  green rectangle on a gray background. The area the grid layout
  occupies is colored white. The top layout has a spacing of 2 and a
  margin of 5, the bottom layout has the defaults of no margin or
  spacing.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QFxVerticalLayout
    \brief The QFxVerticalLayout class lines up items vertically.
    \ingroup group_layouts
*/
QFxVerticalLayout::QFxVerticalLayout(QFxItem *parent)
: QFxBaseLayout(Vertical, parent)
{
}

void QFxVerticalLayout::doLayout()
{
    int voffset = 0;

    foreach(QFxItem* item, *leavingItems()){
        if (remove()){
            QList<QPair<QString,QVariant> > changes;
            applyRemove(changes, item);
        }
    }

    QList<QGraphicsItem *> children = childItems();
    for (int ii = 0; ii < children.count(); ++ii) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(ii));
        if (!child || !child->isVisible())
            continue;

        bool needMove = (child->y() != voffset || child->x());

        QList<QPair<QString, QVariant> > changes;
        changes << qMakePair(QString(QLatin1String("y")),QVariant(voffset));
        changes << qMakePair(QString(QLatin1String("x")),QVariant(0));
        if (needMove && items()->contains(child) && move()) {
            applyMove(changes,child);
        } else if (!items()->contains(child) && add()) {
            applyAdd(changes,child);
        } else if (needMove) {
            setMovingItem(child);
            child->setY(voffset);
            setMovingItem(0);
        }
        voffset += child->height();
        voffset += spacing();
    }
    setMovingItem(this);
    setHeight(voffset);
    setMovingItem(0);
}

QML_DEFINE_TYPE(Qt,4.6,HorizontalLayout,QFxHorizontalLayout)
/*!
  \qmlclass HorizontalLayout
  \brief The HorizontalLayout item arranges its children in a horizontal layout.
  \inherits Item

  The HorizontalLayout item arranges its child items so that they are horizontally aligned and not overlapping. Spacing can be added between the items, and a margin around all items can also be added. It also provides for transitions to be set when items are added, moved, or removed in the layout. Adding and removing apply both to items which are deleted or have their position in the document changed so as to no longer be children of the layout, as well as to items which have their opacity set to or from zero so as to appear or disappear.

  The below example lays out differently shaped rectangles using a HorizontalLayout.
  \qml
HorizontalLayout {
    spacing: 2
    Rect { color: "red"; width: 50; height: 50 }
    Rect { color: "green"; width: 20; height: 50 }
    Rect { color: "blue"; width: 50; height: 20 }
}
  \endqml
  \image horizontalLayout_example.png

*/
/*!
    \qmlproperty Transition HorizontalLayout::remove
    This property holds the transition to apply when removing an item from the layout.

    Removed can mean that either the object has been deleted or reparented, and thus is now longer a child of the layout, or that the object has had its opacity set to zero, and thus is no longer visible.

    Note that if the item counts as removed because its opacity is zero it will not be visible during the transition unless you set the opacity in the transition, like in the below example.

    \qml
HorizontalLayout {
    id: layout
    remove: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Transition HorizontalLayout::add
    This property holds the transition to apply when adding an item to the layout.

    Added can mean that either the object has been created or reparented, and thus is now a child or the layout, or that the object has had its opacity increased from zero, and thus is now visible.

    \qml
HorizontalLayout {
    id: layout
    add: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 0
            to: 1
            duration: 500
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Transition HorizontalLayout::move
    This property holds the transition to apply when moving an item within the layout.

    This can happen when other items are added or removed from the layout, or when items resize themselves.

    \qml
HorizontalLayout {
    id: layout
    move: Transition {
        NumberAnimation {
            properties: "x"
            ease: "easeOutBounce"
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Item HorizontalLayout::item

    The item that is currently being laid out. Used to target transitions that apply
    only to the item being laid out, such as in the add transition.

*/
/*!
  \qmlproperty int HorizontalLayout::spacing

  The spacing, in pixels, left empty between each adjacent item.
*/
/*!
  \qmlproperty int HorizontalLayout::margin

  The margin size, in pixels, which will be left empty around the inside edge of the layout.
*/
/*!
  \qmlproperty int HorizontalLayout::spacing
  \qmlproperty int HorizontalLayout::margin

  spacing and margin allow you to control the empty space surrounding items in layouts.

  spacing is the amount in pixels left empty between each adjacent item.
  margin is the amount in pixels which will be left empty around the inside edge of the layout.
  Both default to 0.

  The below example places a GridLayout containing a red, a blue and a green rectangle on a gray background. The area the grid layout occupies is colored white. The top layout has a spacing of 2 and a margin of 5, the bottom layout has the defaults of no margin or spacing.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QFxHorizontalLayout
    \brief The QFxHorizontalLayout class lines up items horizontally.
    \ingroup group_layouts
*/
QFxHorizontalLayout::QFxHorizontalLayout(QFxItem *parent)
: QFxBaseLayout(Horizontal, parent)
{
}

void QFxHorizontalLayout::doLayout()
{
    int hoffset = 0;

    foreach(QFxItem* item, *leavingItems()){
        if (remove()){
            QList<QPair<QString,QVariant> > changes;
            applyRemove(changes, item);
        }
    }
    QList<QGraphicsItem *> children = childItems();
    for (int ii = 0; ii < children.count(); ++ii) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(ii));
        if (!child || !child->isVisible())
            continue;

        bool needMove = (child->x() != hoffset || child->y());

        QList<QPair<QString, QVariant> > changes;
        changes << qMakePair(QString(QLatin1String("x")),QVariant(hoffset));
        changes << qMakePair(QString(QLatin1String("y")),QVariant(0));
        if (needMove && items()->contains(child) && move()) {
            applyMove(changes,child);
        } else if (!items()->contains(child) && add()) {
            applyAdd(changes,child);
        } else if (needMove) {
            setMovingItem(child);
            child->setX(hoffset);
            setMovingItem(0);
        }
        hoffset += child->width();
        hoffset += spacing();
    }
    setWidth(hoffset);
}

QML_DEFINE_TYPE(Qt,4.6,GridLayout,QFxGridLayout)

/*!
  \qmlclass GridLayout QFxGridLayout
  \brief The GridLayout item arranges its children in a grid layout.
  \inherits Item

  The GridLayout item arranges its child items so that they are
  aligned in a grid and are not overlapping. Spacing can be added
  between the items, and a margin around all the items can also be
  defined. It also provides for transitions to be set when items are
  added, moved, or removed in the layout. Adding and removing apply
  both to items which are deleted or have their position in the
  document changed so as to no longer be children of the layout, as
  well as to items which have their opacity set to or from zero so
  as to appear or disappear.

  The GridLayout defaults to using four columns, and as many rows as
  are necessary to fit all the child items. The number of rows
  and/or the number of columns can be constrained by setting the rows
  or columns properties. The grid layout calculates a grid with
  rectangular cells of sufficient size to hold all items, and then
  places the items in the cells, going across then down, and
  positioning each item at the (0,0) corner of the cell. The below
  example demonstrates this.

  \table
  \row 
  \o \image gridLayout_example.png
  \o
  \qml
GridLayout {
    columns: 3
    spacing: 2
    Rect { color: "red"; width: 50; height: 50 }
    Rect { color: "green"; width: 20; height: 50 }
    Rect { color: "blue"; width: 50; height: 20 }
    Rect { color: "cyan"; width: 50; height: 50 }
    Rect { color: "magenta"; width: 10; height: 10 }
}
  \endqml
  \endtable
*/
/*!
    \qmlproperty Transition GridLayout::remove
    This property holds the transition to apply when removing an item from the layout.

    Removed can mean that either the object has been deleted or
    reparented, and thus is now longer a child of the layout, or that
    the object has had its opacity set to zero, and thus is no longer
    visible.

    Note that if the item counts as removed because its opacity is
    zero it will not be visible during the transition unless you set
    the opacity in the transition, like in the below example.

    \qml
GridLayout {
    id: layout
    remove: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 1
            to: 0
            duration: 500
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Transition GridLayout::add
    This property holds the transition to apply when adding an item to the layout.

    Added can mean that either the object has been created or
    reparented, and thus is now a child or the layout, or that the
    object has had its opacity increased from zero, and thus is now
    visible.

    \qml
GridLayout {
    id: layout
    add: Transition {
        NumberAnimation {
            target: layout.item
            properties: "opacity"
            from: 0
            to: 1
            duration: 500
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Transition GridLayout::move
    This property holds the transition to apply when moving an item within the layout.

    This can happen when other items are added or removed from the layout, or
    when items resize themselves.

    \qml
GridLayout {
    id: layout
    move: Transition {
        NumberAnimation {
            properties: "x,y"
            ease: "easeOutBounce"
        }
    }
}
    \endqml

*/
/*!
    \qmlproperty Item GridLayout::item

    The item that is currently being laid out. Used to target
    transitions that apply only to the item being laid out, such as in
    the add transition.

*/
/*!
  \qmlproperty int GridLayout::spacing
  \qmlproperty int GridLayout::margin

  spacing and margin allow you to control the empty space surrounding
  items in layouts.

  spacing is the amount in pixels left empty between each adjacent
  item.  margin is the amount in pixels which will be left empty
  around the inside edge of the layout.  Both default to 0.

  The below example places a GridLayout containing a red, a blue and a
  green rectangle on a gray background. The area the grid layout
  occupies is colored white. The top layout has a spacing of 2 and a
  margin of 5, the bottom layout has the defaults of no margin or
  spacing.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QFxGridLayout
    \brief The QFxGridLayout class lays out items in a grid.
    \ingroup group_layouts

*/
QFxGridLayout::QFxGridLayout(QFxItem *parent) :
    QFxBaseLayout(Both, parent)
{
    _columns=-1;
    _rows=-1;
}

/*!
    \qmlproperty int GridLayout::columns
    This property holds the number of columns in the grid.

    When the columns property is set the GridLayout will always have
    that many columns. Note that if you do not have enough items to
    fill this many columns some columns will be of zero width.
*/

/*!
    \qmlproperty int GridLayout::rows
    This property holds the number of rows in the grid.

    When the rows property is set the GridLayout will always have that
    many rows. Note that if you do not have enough items to fill this
    many rows some rows will be of zero width.
*/

/*!
    \property QFxGridLayout::columns
    \brief the number of columns in the grid.
*/

/*!
    \property QFxGridLayout::rows
    \brief the number of rows in the grid.
*/

void QFxGridLayout::doLayout()
{
    int c=_columns,r=_rows;//Actual number of rows/columns
    int numVisible = items()->size() + newItems()->size();
    if (_columns==-1 && _rows==-1){
        c = 4;
        r = (numVisible+2)/3;
    }else if (_rows==-1){
        r = (numVisible+(_columns-1))/_columns;
    }else if (_columns==-1){
        c = (numVisible+(_rows-1))/_rows;
    }

    QList<int> maxColWidth;
    QList<int> maxRowHeight;
    int childIndex =0;
    QList<QGraphicsItem *> children = childItems();
    for (int i=0; i<r; i++){
        for (int j=0; j<c; j++){
            if (j==0)
                maxRowHeight << 0;
            if (i==0)
                maxColWidth << 0;

            if (childIndex == children.count())
                continue;
            QFxItem *child = qobject_cast<QFxItem *>(children.at(childIndex++));
            if (!child || !child->isVisible())
                continue;
            if (child->width() > maxColWidth[j])
                maxColWidth[j] = child->width();
            if (child->height() > maxRowHeight[i])
                maxRowHeight[i] = child->height();
        }
    }

    int xoffset=0;
    int yoffset=0;
    int curRow =0;
    int curCol =0;
    foreach(QFxItem* item, *leavingItems()){
        if (remove()){
            QList<QPair<QString,QVariant> > changes;
            applyRemove(changes, item);
        }
    }
    foreach(QGraphicsItem* schild, children){
        QFxItem *child = qobject_cast<QFxItem *>(schild);
        if (!child || !child->isVisible())
            continue;
        bool needMove = (child->x()!=xoffset)||(child->y()!=yoffset);
        QList<QPair<QString, QVariant> > changes;
        changes << qMakePair(QString(QLatin1String("x")),QVariant(xoffset));
        changes << qMakePair(QString(QLatin1String("y")),QVariant(yoffset));
        if (newItems()->contains(child) && add()) {
            applyAdd(changes,child);
        } else if (needMove) {
            if (move()){
                applyMove(changes,child);
            }else{
                setMovingItem(child);
                child->setPos(QPointF(xoffset, yoffset));
                setMovingItem(0);
            }
        }
        xoffset+=maxColWidth[curCol]+spacing();
        curCol++;
        curCol%=c;
        if (!curCol){
            yoffset+=maxRowHeight[curRow]+spacing();
            xoffset=0;
            curRow++;
            if (curRow>=r)
                return;
        }
    }
}

QT_END_NAMESPACE
