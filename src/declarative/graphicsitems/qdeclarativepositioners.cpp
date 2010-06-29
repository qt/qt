/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativepositioners_p.h"
#include "private/qdeclarativepositioners_p_p.h"

#include <qdeclarative.h>
#include <qdeclarativestate_p.h>
#include <qdeclarativestategroup_p.h>
#include <qdeclarativestateoperations_p.h>
#include <qdeclarativeinfo.h>
#include <QtCore/qmath.h>

#include <QDebug>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

static const QDeclarativeItemPrivate::ChangeTypes watchedChanges
    = QDeclarativeItemPrivate::Geometry
    | QDeclarativeItemPrivate::SiblingOrder
    | QDeclarativeItemPrivate::Visibility
    | QDeclarativeItemPrivate::Opacity
    | QDeclarativeItemPrivate::Destroyed;

void QDeclarativeBasePositionerPrivate::watchChanges(QDeclarativeItem *other)
{
    QDeclarativeItemPrivate *otherPrivate = static_cast<QDeclarativeItemPrivate*>(QGraphicsItemPrivate::get(other));
    otherPrivate->addItemChangeListener(this, watchedChanges);
}

void QDeclarativeBasePositionerPrivate::unwatchChanges(QDeclarativeItem* other)
{
    QDeclarativeItemPrivate *otherPrivate = static_cast<QDeclarativeItemPrivate*>(QGraphicsItemPrivate::get(other));
    otherPrivate->removeItemChangeListener(this, watchedChanges);
}

/*!
    \internal
    \class QDeclarativeBasePositioner
    \brief The QDeclarativeBasePositioner class provides a base for QDeclarativeGraphics layouts.

    To create a QDeclarativeGraphics Positioner, simply subclass QDeclarativeBasePositioner and implement
    doLayout(), which is automatically called when the layout might need
    updating. In doLayout() use the setX and setY functions from QDeclarativeBasePositioner, and the
    base class will apply the positions along with the appropriate transitions. The items to
    position are provided in order as the protected member positionedItems.

    You also need to set a PositionerType, to declare whether you are positioning the x, y or both
    for the child items. Depending on the chosen type, only x or y changes will be applied.

    Note that the subclass is responsible for adding the
    spacing in between items.
*/
QDeclarativeBasePositioner::QDeclarativeBasePositioner(PositionerType at, QDeclarativeItem *parent)
    : QDeclarativeItem(*(new QDeclarativeBasePositionerPrivate), parent)
{
    Q_D(QDeclarativeBasePositioner);
    d->init(at);
}

QDeclarativeBasePositioner::QDeclarativeBasePositioner(QDeclarativeBasePositionerPrivate &dd, PositionerType at, QDeclarativeItem *parent)
    : QDeclarativeItem(dd, parent)
{
    Q_D(QDeclarativeBasePositioner);
    d->init(at);
}

QDeclarativeBasePositioner::~QDeclarativeBasePositioner()
{
    Q_D(QDeclarativeBasePositioner);
    for (int i = 0; i < positionedItems.count(); ++i)
        d->unwatchChanges(positionedItems.at(i).item);
    positionedItems.clear();
}

int QDeclarativeBasePositioner::spacing() const
{
    Q_D(const QDeclarativeBasePositioner);
    return d->spacing;
}

void QDeclarativeBasePositioner::setSpacing(int s)
{
    Q_D(QDeclarativeBasePositioner);
    if (s==d->spacing)
        return;
    d->spacing = s;
    prePositioning();
    emit spacingChanged();
}

QDeclarativeTransition *QDeclarativeBasePositioner::move() const
{
    Q_D(const QDeclarativeBasePositioner);
    return d->moveTransition;
}

void QDeclarativeBasePositioner::setMove(QDeclarativeTransition *mt)
{
    Q_D(QDeclarativeBasePositioner);
    if (mt == d->moveTransition)
        return;
    d->moveTransition = mt;
    emit moveChanged();
}

QDeclarativeTransition *QDeclarativeBasePositioner::add() const
{
    Q_D(const QDeclarativeBasePositioner);
    return d->addTransition;
}

void QDeclarativeBasePositioner::setAdd(QDeclarativeTransition *add)
{
    Q_D(QDeclarativeBasePositioner);
    if (add == d->addTransition)
        return;

    d->addTransition = add;
    emit addChanged();
}

void QDeclarativeBasePositioner::componentComplete()
{
    Q_D(QDeclarativeBasePositioner);
    QDeclarativeItem::componentComplete();
    positionedItems.reserve(d->QGraphicsItemPrivate::children.count());
    prePositioning();
    reportConflictingAnchors();
}

QVariant QDeclarativeBasePositioner::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    Q_D(QDeclarativeBasePositioner);
    if (change == ItemChildAddedChange){
        QGraphicsItem* item = value.value<QGraphicsItem*>();
        QDeclarativeItem* child = 0;
        if(item)
            child = qobject_cast<QDeclarativeItem*>(item->toGraphicsObject());
        if (child)
            prePositioning();
    } else if (change == ItemChildRemovedChange) {
        QGraphicsItem* item = value.value<QGraphicsItem*>();
        QDeclarativeItem* child = 0;
        if(item)
            child = qobject_cast<QDeclarativeItem*>(item->toGraphicsObject());
        if (child) {
            QDeclarativeBasePositioner::PositionedItem posItem(child);
            int idx = positionedItems.find(posItem);
            if (idx >= 0) {
                d->unwatchChanges(child);
                positionedItems.remove(idx);
            }
            prePositioning();
        }
    }

    return QDeclarativeItem::itemChange(change, value);
}

void QDeclarativeBasePositioner::prePositioning()
{
    Q_D(QDeclarativeBasePositioner);
    if (!isComponentComplete())
        return;

    if (d->doingPositioning)
        return;

    d->queuedPositioning = false;
    d->doingPositioning = true;
    //Need to order children by creation order modified by stacking order
    QList<QGraphicsItem *> children = d->QGraphicsItemPrivate::children;
    qSort(children.begin(), children.end(), d->insertionOrder);

    QPODVector<PositionedItem,8> oldItems;
    positionedItems.copyAndClear(oldItems);
    for (int ii = 0; ii < children.count(); ++ii) {
        QDeclarativeItem *child = qobject_cast<QDeclarativeItem *>(children.at(ii));
        if (!child)
            continue;
        QDeclarativeItemPrivate *childPrivate = static_cast<QDeclarativeItemPrivate*>(QGraphicsItemPrivate::get(child));
        PositionedItem *item = 0;
        PositionedItem posItem(child);
        int wIdx = oldItems.find(posItem);
        if (wIdx < 0) {
            d->watchChanges(child);
            positionedItems.append(posItem);
            item = &positionedItems[positionedItems.count()-1];
            item->isNew = true;
            if (child->opacity() <= 0.0 || childPrivate->explicitlyHidden)
                item->isVisible = false;
        } else {
            item = &oldItems[wIdx];
            // Items are only omitted from positioning if they are explicitly hidden
            // i.e. their positioning is not affected if an ancestor is hidden.
            if (child->opacity() <= 0.0 || childPrivate->explicitlyHidden) {
                item->isVisible = false;
            } else if (!item->isVisible) {
                item->isVisible = true;
                item->isNew = true;
            } else {
                item->isNew = false;
            }
            positionedItems.append(*item);
        }
    }
    QSizeF contentSize;
    doPositioning(&contentSize);
    if(d->addTransition || d->moveTransition)
        finishApplyTransitions();
    d->doingPositioning = false;
    //Set implicit size to the size of its children
    setImplicitHeight(contentSize.height());
    setImplicitWidth(contentSize.width());
}

void QDeclarativeBasePositioner::positionX(int x, const PositionedItem &target)
{
    Q_D(QDeclarativeBasePositioner);
    if(d->type == Horizontal || d->type == Both){
        if (target.isNew) {
            if (!d->addTransition)
                target.item->setX(x);
            else
                d->addActions << QDeclarativeAction(target.item, QLatin1String("x"), QVariant(x));
        } else if (x != target.item->x()) {
            if (!d->moveTransition)
                target.item->setX(x);
            else
                d->moveActions << QDeclarativeAction(target.item, QLatin1String("x"), QVariant(x));
        }
    }
}

void QDeclarativeBasePositioner::positionY(int y, const PositionedItem &target)
{
    Q_D(QDeclarativeBasePositioner);
    if(d->type == Vertical || d->type == Both){
        if (target.isNew) {
            if (!d->addTransition)
                target.item->setY(y);
            else
                d->addActions << QDeclarativeAction(target.item, QLatin1String("y"), QVariant(y));
        } else if (y != target.item->y()) {
            if (!d->moveTransition)
                target.item->setY(y);
            else
                d->moveActions << QDeclarativeAction(target.item, QLatin1String("y"), QVariant(y));
        }
    }
}

void QDeclarativeBasePositioner::finishApplyTransitions()
{
    Q_D(QDeclarativeBasePositioner);
    // Note that if a transition is not set the transition manager will
    // apply the changes directly, in the case add/move aren't set
    d->addTransitionManager.transition(d->addActions, d->addTransition);
    d->moveTransitionManager.transition(d->moveActions, d->moveTransition);
    d->addActions.clear();
    d->moveActions.clear();
}

static inline bool isInvisible(QDeclarativeItem *child)
{
    QDeclarativeItemPrivate *childPrivate = static_cast<QDeclarativeItemPrivate*>(QGraphicsItemPrivate::get(child));
    return child->opacity() == 0.0 || childPrivate->explicitlyHidden || !child->width() || !child->height();
}

/*!
  \qmlclass Column QDeclarativeColumn
    \since 4.7
  \brief The Column item arranges its children vertically.
  \inherits Item

  The Column item positions its child items so that they are vertically
    aligned and not overlapping. Spacing between items can be added.

  The below example positions differently shaped rectangles using a Column.
  \table
  \row
  \o \image verticalpositioner_example.png
  \o
  \qml
Column {
    spacing: 2
    Rectangle { color: "red"; width: 50; height: 50 }
    Rectangle { color: "green"; width: 20; height: 50 }
    Rectangle { color: "blue"; width: 50; height: 20 }
}
  \endqml
  \endtable

  Column also provides for transitions to be set when items are added, moved,
  or removed in the positioner. Adding and removing apply both to items which are deleted
  or have their position in the document changed so as to no longer be children of the positioner,
  as well as to items which have their opacity set to or from zero so as to appear or disappear.

  \table
  \row
  \o \image verticalpositioner_transition.gif
  \o
  \qml
Column {
    spacing: 2
    add: ...
    move: ...
    ...
}
  \endqml
  \endtable

  Note that the positioner assumes that the x and y positions of its children
  will not change. If you manually change the x or y properties in script, bind
  the x or y properties, use anchors on a child of a positioner, or have the
  height of a child depend on the position of a child, then the
  positioner may exhibit strange behaviour. If you need to perform any of these
  actions, consider positioning the items without the use of a Column.

  Items with a width or height of 0 will not be positioned.

  \sa Row, {declarative/positioners}{Positioners example}
*/
/*!
    \qmlproperty Transition Column::add

    This property holds the transition to be applied when adding an
    item to the positioner. The transition will only be applied to the
    added item(s).  Positioner transitions will only affect the
    position (x,y) of items.

    Added can mean that either the object has been created or
    reparented, and thus is now a child or the positioner, or that the
    object has had its opacity increased from zero, and thus is now
    visible.


*/
/*!
    \qmlproperty Transition Column::move

    This property holds the transition to apply when moving an item
    within the positioner.  Positioner transitions will only affect
    the position (x,y) of items.

    This can happen when other items are added or removed from the
    positioner, or when items resize themselves.

    \table
    \row
    \o \image positioner-move.gif
    \o
    \qml
Column {
    move: Transition {
        NumberAnimation {
            properties: "y"
            easing.type: Easing.OutBounce
        }
    }
}
    \endqml
    \endtable
*/
/*!
  \qmlproperty int Column::spacing

  spacing is the amount in pixels left empty between each adjacent
  item, and defaults to 0.

  The below example places a \l Grid containing a red, a blue and a
  green rectangle on a gray background. The area the grid positioner
  occupies is colored white. The top positioner has the default of no spacing,
  and the bottom positioner has its spacing set to 2.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QDeclarativeColumn
    \brief The QDeclarativeColumn class lines up items vertically.
*/
QDeclarativeColumn::QDeclarativeColumn(QDeclarativeItem *parent)
: QDeclarativeBasePositioner(Vertical, parent)
{
}

void QDeclarativeColumn::doPositioning(QSizeF *contentSize)
{
    int voffset = 0;

    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (!child.item || isInvisible(child.item))
            continue;

        if(child.item->y() != voffset)
            positionY(voffset, child);

        contentSize->setWidth(qMax(contentSize->width(), child.item->width()));

        voffset += child.item->height();
        voffset += spacing();
    }

    contentSize->setHeight(voffset - spacing());
}

void QDeclarativeColumn::reportConflictingAnchors()
{
    QDeclarativeBasePositionerPrivate *d = static_cast<QDeclarativeBasePositionerPrivate*>(QDeclarativeBasePositionerPrivate::get(this));
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (child.item) {
            QDeclarativeAnchors *anchors = QDeclarativeItemPrivate::get(child.item)->_anchors;
            if (anchors) {
                QDeclarativeAnchors::Anchors usedAnchors = anchors->usedAnchors();
                if (usedAnchors & QDeclarativeAnchors::TopAnchor ||
                    usedAnchors & QDeclarativeAnchors::BottomAnchor ||
                    usedAnchors & QDeclarativeAnchors::VCenterAnchor ||
                    anchors->fill() || anchors->centerIn()) {
                    d->anchorConflict = true;
                    break;
                }
            }
        }
    }
    if (d->anchorConflict) {
        qmlInfo(this) << "Cannot specify top, bottom, verticalCenter, fill or centerIn anchors for items inside Column";
    }
}

/*!
  \qmlclass Row QDeclarativeRow
  \since 4.7
  \brief The Row item arranges its children horizontally.
  \inherits Item

  The Row item positions its child items so that they are
  horizontally aligned and not overlapping. 

  Use \l spacing to set the spacing between items in a Row, and use the
  \l add and \l move properties to set the transitions that should be applied
  when items are added to, removed from, or re-positioned within the Row.

  The below example lays out differently shaped rectangles using a Row.
  \qml
Row {
    spacing: 2
    Rectangle { color: "red"; width: 50; height: 50 }
    Rectangle { color: "green"; width: 20; height: 50 }
    Rectangle { color: "blue"; width: 50; height: 20 }
}
  \endqml
  \image horizontalpositioner_example.png

  Note that the positioner assumes that the x and y positions of its children
  will not change. If you manually change the x or y properties in script, bind
  the x or y properties, use anchors on a child of a positioner, or have the
  width of a child depend on the position of a child, then the
  positioner may exhibit strange behaviour. If you need to perform any of these
  actions, consider positioning the items without the use of a Row.

  Items with a width or height of 0 will not be positioned.

  \sa Column, {declarative/positioners}{Positioners example}
*/
/*!
    \qmlproperty Transition Row::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition will only be applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    An object is considered to be added to the positioner if it has been
    created or reparented and thus is now a child or the positioner, or if the
    object has had its opacity increased from zero, and thus is now
    visible.
*/
/*!
    \qmlproperty Transition Row::move

    This property holds the transition to apply when moving an item
    within the positioner.  Positioner transitions will only affect
    the position (x,y) of items.

    This can happen when other items are added or removed from the
    positioner, or when items resize themselves.

    \qml
Row {
    id: positioner
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
  \qmlproperty int Row::spacing

  spacing is the amount in pixels left empty between each adjacent
  item, and defaults to 0.

  The below example places a \l Grid containing a red, a blue and a
  green rectangle on a gray background. The area the grid positioner
  occupies is colored white. The top positioner has the default of no spacing,
  and the bottom positioner has its spacing set to 2.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QDeclarativeRow
    \brief The QDeclarativeRow class lines up items horizontally.
*/
QDeclarativeRow::QDeclarativeRow(QDeclarativeItem *parent)
: QDeclarativeBasePositioner(Horizontal, parent)
{
}

void QDeclarativeRow::doPositioning(QSizeF *contentSize)
{
    int hoffset = 0;

    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (!child.item || isInvisible(child.item))
            continue;

        if(child.item->x() != hoffset)
            positionX(hoffset, child);

        contentSize->setHeight(qMax(contentSize->height(), child.item->height()));

        hoffset += child.item->width();
        hoffset += spacing();
    }

    contentSize->setWidth(hoffset - spacing());
}

void QDeclarativeRow::reportConflictingAnchors()
{
    QDeclarativeBasePositionerPrivate *d = static_cast<QDeclarativeBasePositionerPrivate*>(QDeclarativeBasePositionerPrivate::get(this));
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (child.item) {
            QDeclarativeAnchors *anchors = QDeclarativeItemPrivate::get(child.item)->_anchors;
            if (anchors) {
                QDeclarativeAnchors::Anchors usedAnchors = anchors->usedAnchors();
                if (usedAnchors & QDeclarativeAnchors::LeftAnchor ||
                    usedAnchors & QDeclarativeAnchors::RightAnchor ||
                    usedAnchors & QDeclarativeAnchors::HCenterAnchor ||
                    anchors->fill() || anchors->centerIn()) {
                    d->anchorConflict = true;
                    break;
                }
            }
        }
    }
    if (d->anchorConflict)
        qmlInfo(this) << "Cannot specify left, right, horizontalCenter, fill or centerIn anchors for items inside Row";
}

/*!
  \qmlclass Grid QDeclarativeGrid
  \since 4.7
  \brief The Grid item positions its children in a grid.
  \inherits Item

  The Grid item positions its child items so that they are
  aligned in a grid and are not overlapping. 
  
  Spacing can be added
  between child items. It also provides for transitions to be set when items are
  added, moved, or removed in the positioner. Adding and removing apply
  both to items which are deleted or have their position in the
  document changed so as to no longer be children of the positioner, as
  well as to items which have their opacity set to or from zero so
  as to appear or disappear.

  A Grid defaults to four columns, and as many rows as
  are necessary to fit all child items. The number of rows
  and/or the number of columns can be constrained by setting the \l rows
  or \l columns properties. The grid positioner calculates a grid with
  rectangular cells of sufficient size to hold all items, and then
  places the items in the cells, going across then down, and
  positioning each item at the (0,0) corner of the cell. The below
  example demonstrates this.

  \table
  \row
  \o \image gridLayout_example.png
  \o
  \qml
Grid {
    columns: 3
    spacing: 2
    Rectangle { color: "red"; width: 50; height: 50 }
    Rectangle { color: "green"; width: 20; height: 50 }
    Rectangle { color: "blue"; width: 50; height: 20 }
    Rectangle { color: "cyan"; width: 50; height: 50 }
    Rectangle { color: "magenta"; width: 10; height: 10 }
}
  \endqml
  \endtable

  Note that the positioner assumes that the x and y positions of its children
  will not change. If you manually change the x or y properties in script, bind
  the x or y properties, use anchors on a child of a positioner, or have the
  width or height of a child depend on the position of a child, then the
  positioner may exhibit strange behaviour. If you need to perform any of these
  actions, consider positioning the items without the use of a Grid.

  Items with a width or height of 0 will not be positioned.

  \sa Flow, {declarative/positioners}{Positioners example}
*/
/*!
    \qmlproperty Transition Grid::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition is only applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items,
    as that is all the positioners affect. To animate other property change
    you will have to do so based on how you have changed those properties.

    An object is considered to be added to the positioner if it has been
    created or reparented and thus is now a child or the positioner, or if the
    object has had its opacity increased from zero, and thus is now
    visible.
*/
/*!
    \qmlproperty Transition Grid::move
    This property holds the transition to apply when moving an item within the positioner.
    Positioner transitions will only affect the position (x,y) of items.

    This can happen when other items are added or removed from the positioner, or
    when items resize themselves.

    \qml
Grid {
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
  \qmlproperty int Grid::spacing

  spacing is the amount in pixels left empty between each adjacent
  item, and defaults to 0.

  The below example places a Grid containing a red, a blue and a
  green rectangle on a gray background. The area the grid positioner
  occupies is colored white. The top positioner has the default of no spacing,
  and the bottom positioner has its spacing set to 2.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QDeclarativeGrid
    \brief The QDeclarativeGrid class lays out items in a grid.
*/
QDeclarativeGrid::QDeclarativeGrid(QDeclarativeItem *parent) :
    QDeclarativeBasePositioner(Both, parent), m_rows(-1), m_columns(-1), m_flow(LeftToRight)
{
}

/*!
    \qmlproperty int Grid::columns
    This property holds the number of columns in the grid.

    If the grid does not have enough items to fill the specified
    number of columns, some columns will be of zero width.
*/

/*!
    \qmlproperty int Grid::rows
    This property holds the number of rows in the grid.

    If the grid does not have enough items to fill the specified
    number of rows, some rows will be of zero width.
*/

void QDeclarativeGrid::setColumns(const int columns)
{
    if (columns == m_columns)
        return;
    m_columns = columns;
    prePositioning();
    emit columnsChanged();
}

void QDeclarativeGrid::setRows(const int rows)
{
    if (rows == m_rows)
        return;
    m_rows = rows;
    prePositioning();
    emit rowsChanged();
}

/*!
    \qmlproperty enumeration Grid::flow
    This property holds the flow of the layout.

    Possible values are:

    \list
    \o Grid.LeftToRight (default) - Items are positioned next to
       to each other from left to right, then wrapped to the next line.
    \o Grid.TopToBottom - Items are positioned next to each
       other from top to bottom, then wrapped to the next column.
    \endlist
*/
QDeclarativeGrid::Flow QDeclarativeGrid::flow() const
{
    return m_flow;
}

void QDeclarativeGrid::setFlow(Flow flow)
{
    if (m_flow != flow) {
        m_flow = flow;
        prePositioning();
        emit flowChanged();
    }
}

void QDeclarativeGrid::doPositioning(QSizeF *contentSize)
{
    int c = m_columns;
    int r = m_rows;
    int numVisible = positionedItems.count();
    if (m_columns <= 0 && m_rows <= 0){
        c = 4;
        r = (numVisible+3)/4;
    } else if (m_rows <= 0){
        r = (numVisible+(m_columns-1))/m_columns;
    } else if (m_columns <= 0){
        c = (numVisible+(m_rows-1))/m_rows;
    }

    QList<int> maxColWidth;
    QList<int> maxRowHeight;
    int childIndex =0;
    if (m_flow == LeftToRight) {
        for (int i=0; i < r; i++){
            for (int j=0; j < c; j++){
                if (j==0)
                    maxRowHeight << 0;
                if (i==0)
                    maxColWidth << 0;

                if (childIndex == positionedItems.count())
                    continue;
                const PositionedItem &child = positionedItems.at(childIndex++);
                if (!child.item || isInvisible(child.item))
                    continue;
                if (child.item->width() > maxColWidth[j])
                    maxColWidth[j] = child.item->width();
                if (child.item->height() > maxRowHeight[i])
                    maxRowHeight[i] = child.item->height();
            }
        }
    } else {
        for (int j=0; j < c; j++){
            for (int i=0; i < r; i++){
                if (j==0)
                    maxRowHeight << 0;
                if (i==0)
                    maxColWidth << 0;

                if (childIndex == positionedItems.count())
                    continue;
                const PositionedItem &child = positionedItems.at(childIndex++);
                if (!child.item || isInvisible(child.item))
                    continue;
                if (child.item->width() > maxColWidth[j])
                    maxColWidth[j] = child.item->width();
                if (child.item->height() > maxRowHeight[i])
                    maxRowHeight[i] = child.item->height();
            }
        }
    }

    int xoffset=0;
    int yoffset=0;
    int curRow =0;
    int curCol =0;
    for (int i = 0; i < positionedItems.count(); ++i) {
        const PositionedItem &child = positionedItems.at(i);
        if (!child.item || isInvisible(child.item))
            continue;
        if((child.item->x()!=xoffset)||(child.item->y()!=yoffset)){
            positionX(xoffset, child);
            positionY(yoffset, child);
        }

        if (m_flow == LeftToRight) {
            contentSize->setWidth(qMax(contentSize->width(), xoffset + child.item->width()));
            contentSize->setHeight(yoffset + maxRowHeight[curRow]);

            xoffset+=maxColWidth[curCol]+spacing();
            curCol++;
            curCol%=c;
            if (!curCol){
                yoffset+=maxRowHeight[curRow]+spacing();
                xoffset=0;
                curRow++;
                if (curRow>=r)
                    break;
            }
        } else {
            contentSize->setHeight(qMax(contentSize->height(), yoffset + child.item->height()));
            contentSize->setWidth(xoffset + maxColWidth[curCol]);

            yoffset+=maxRowHeight[curRow]+spacing();
            curRow++;
            curRow%=r;
            if (!curRow){
                xoffset+=maxColWidth[curCol]+spacing();
                yoffset=0;
                curCol++;
                if (curCol>=c)
                    break;
            }
        }
    }
}

void QDeclarativeGrid::reportConflictingAnchors()
{
    QDeclarativeBasePositionerPrivate *d = static_cast<QDeclarativeBasePositionerPrivate*>(QDeclarativeBasePositionerPrivate::get(this));
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (child.item) {
            QDeclarativeAnchors *anchors = QDeclarativeItemPrivate::get(child.item)->_anchors;
            if (anchors && (anchors->usedAnchors() || anchors->fill() || anchors->centerIn())) {
                d->anchorConflict = true;
                break;
            }
        }
    }
    if (d->anchorConflict)
        qmlInfo(this) << "Cannot specify anchors for items inside Grid";
}

/*!
  \qmlclass Flow QDeclarativeFlow
  \since 4.7
  \brief The Flow item arranges its children side by side, wrapping as necessary.
  \inherits Item

  The Flow item positions its child items so that they are side by side and are
  not overlapping.

  Note that the positioner assumes that the x and y positions of its children
  will not change. If you manually change the x or y properties in script, bind
  the x or y properties, use anchors on a child of a positioner, or have the
  width or height of a child depend on the position of a child, then the
  positioner may exhibit strange behaviour.  If you need to perform any of these
  actions, consider positioning the items without the use of a Flow.

  Items with a width or height of 0 will not be positioned.

    \sa Grid, {declarative/positioners}{Positioners example}
*/
/*!
    \qmlproperty Transition Flow::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition will only be applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    An object is considered to be added to the positioner if it has been
    created or reparented and thus is now a child or the positioner, or if the
    object has had its opacity increased from zero, and thus is now
    visible.
*/
/*!
    \qmlproperty Transition Flow::move
    This property holds the transition to apply when moving an item within the positioner.
    Positioner transitions will only affect the position (x,y) of items.

    This can happen when other items are added or removed from the positioner, or when items resize themselves.

    \qml
Flow {
    id: positioner
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
  \qmlproperty int Flow::spacing

  spacing is the amount in pixels left empty between each adjacent
  item, and defaults to 0.

*/

class QDeclarativeFlowPrivate : public QDeclarativeBasePositionerPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeFlow)

public:
    QDeclarativeFlowPrivate()
        : QDeclarativeBasePositionerPrivate(), flow(QDeclarativeFlow::LeftToRight)
    {}

    QDeclarativeFlow::Flow flow;
};

QDeclarativeFlow::QDeclarativeFlow(QDeclarativeItem *parent)
: QDeclarativeBasePositioner(*(new QDeclarativeFlowPrivate), Both, parent)
{
    Q_D(QDeclarativeFlow);
    // Flow layout requires relayout if its own size changes too.
    d->addItemChangeListener(d, QDeclarativeItemPrivate::Geometry);
}

/*!
    \qmlproperty enumeration Flow::flow
    This property holds the flow of the layout.

    Possible values are:

    \list
    \o Flow.LeftToRight (default) - Items are positioned next to
    to each other from left to right until the width of the Flow
    is exceeded, then wrapped to the next line.
    \o Flow.TopToBottom - Items are positioned next to each
    other from top to bottom until the height of the Flow is exceeded,
    then wrapped to the next column.
    \endlist
*/
QDeclarativeFlow::Flow QDeclarativeFlow::flow() const
{
    Q_D(const QDeclarativeFlow);
    return d->flow;
}

void QDeclarativeFlow::setFlow(Flow flow)
{
    Q_D(QDeclarativeFlow);
    if (d->flow != flow) {
        d->flow = flow;
        prePositioning();
        emit flowChanged();
    }
}

void QDeclarativeFlow::doPositioning(QSizeF *contentSize)
{
    Q_D(QDeclarativeFlow);

    int hoffset = 0;
    int voffset = 0;
    int linemax = 0;

    for (int i = 0; i < positionedItems.count(); ++i) {
        const PositionedItem &child = positionedItems.at(i);
        if (!child.item || isInvisible(child.item))
            continue;

        if (d->flow == LeftToRight)  {
            if (widthValid() && hoffset && hoffset + child.item->width() > width()) {
                hoffset = 0;
                voffset += linemax + spacing();
                linemax = 0;
            }
        } else {
            if (heightValid() && voffset && voffset + child.item->height() > height()) {
                voffset = 0;
                hoffset += linemax + spacing();
                linemax = 0;
            }
        }

        if(child.item->x() != hoffset || child.item->y() != voffset){
            positionX(hoffset, child);
            positionY(voffset, child);
        }

        contentSize->setWidth(qMax(contentSize->width(), hoffset + child.item->width()));
        contentSize->setHeight(qMax(contentSize->height(), voffset + child.item->height()));

        if (d->flow == LeftToRight)  {
            hoffset += child.item->width();
            hoffset += spacing();
            linemax = qMax(linemax, qCeil(child.item->height()));
        } else {
            voffset += child.item->height();
            voffset += spacing();
            linemax = qMax(linemax, qCeil(child.item->width()));
        }
    }
}

void QDeclarativeFlow::reportConflictingAnchors()
{
    Q_D(QDeclarativeFlow);
    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        const PositionedItem &child = positionedItems.at(ii);
        if (child.item) {
            QDeclarativeAnchors *anchors = QDeclarativeItemPrivate::get(child.item)->_anchors;
            if (anchors && (anchors->usedAnchors() || anchors->fill() || anchors->centerIn())) {
                d->anchorConflict = true;
                break;
            }
        }
    }
    if (d->anchorConflict)
        qmlInfo(this) << "Cannot specify anchors for items inside Flow";
}

QT_END_NAMESPACE
