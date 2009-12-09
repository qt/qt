/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlgraphicspositioners_p.h"
#include "qmlgraphicspositioners_p_p.h"

#include <qml.h>
#include <qmlstate_p.h>
#include <qmlstategroup_p.h>
#include <qmlstateoperations_p.h>
#include <qfxperf_p_p.h>
#include <QtCore/qmath.h>

#include <QDebug>
#include <QCoreApplication>

QT_BEGIN_NAMESPACE

int QmlGraphicsBasePositionerPrivate::prePosIdx = -1;
int QmlGraphicsBasePositionerPrivate::visibleIdx = -1;
int QmlGraphicsBasePositionerPrivate::opacityIdx = -1;


void QmlGraphicsBasePositionerPrivate::watchChanges(QmlGraphicsItem *other)
{
    Q_Q(QmlGraphicsBasePositioner);
    QMetaObject::connect(other, visibleIdx, q, prePosIdx);
    QMetaObject::connect(other, opacityIdx, q, prePosIdx);

    QmlGraphicsItemPrivate *otherPrivate = static_cast<QmlGraphicsItemPrivate*>(QGraphicsItemPrivate::get(other));

    if(type == QmlGraphicsBasePositioner::Vertical || type == QmlGraphicsBasePositioner::Both)
        otherPrivate->connectToHeightChanged(q, prePosIdx);
    if(type == QmlGraphicsBasePositioner::Horizontal || type == QmlGraphicsBasePositioner::Both)
        otherPrivate->connectToWidthChanged(q, prePosIdx);

    otherPrivate->registerSiblingOrderNotification(this);
    watched << other;
}

void QmlGraphicsBasePositionerPrivate::unwatchChanges(QmlGraphicsItem* other)
{
    Q_Q(QmlGraphicsBasePositioner);
    QmlGraphicsItemPrivate *otherPrivate = static_cast<QmlGraphicsItemPrivate*>(QGraphicsItemPrivate::get(other));
    bool stillAlive = false; //Use the returns from disconnect to see if it was deleted or just reparented
    stillAlive |= QMetaObject::disconnect(other, visibleIdx, q, prePosIdx);
    stillAlive |= QMetaObject::disconnect(other, opacityIdx, q, prePosIdx);
    //Is disconnect expensive enough to be worth this check?
    if(type == QmlGraphicsBasePositioner::Vertical || type == QmlGraphicsBasePositioner::Both)
        stillAlive |= otherPrivate->disconnectFromHeightChanged(q, prePosIdx);
    if(type == QmlGraphicsBasePositioner::Horizontal || type == QmlGraphicsBasePositioner::Both)
        stillAlive |= otherPrivate->disconnectFromWidthChanged(q, prePosIdx);

    if(stillAlive)
        otherPrivate->unregisterSiblingOrderNotification(this);
    watched.removeAll(other);
}

/*!
    \internal
    \class QmlGraphicsBasePositioner
    \ingroup group_layouts
    \brief The QmlGraphicsBasePositioner class provides a base for QmlGraphics layouts.

    To create a QmlGraphics Positioner, simply subclass QmlGraphicsBasePositioner and implement
    doLayout(), which is automatically called when the layout might need
    updating. In doLayout() use the setX and setY functions from QmlBasePositioner, and the
    base class will apply the positions along with the appropriate transitions. The items to
    position are provided in order as the protected member positionedItems.

    You also need to set a PositionerType, to declare whether you are positioning the x, y or both
    for the child items. Depending on the chosen type, only x or y changes will be applied.

    Note that the subclass is responsible for adding the
    spacing in between items.
*/
QmlGraphicsBasePositioner::QmlGraphicsBasePositioner(PositionerType at, QmlGraphicsItem *parent)
    : QmlGraphicsItem(*(new QmlGraphicsBasePositionerPrivate), parent)
{
    Q_D(QmlGraphicsBasePositioner);
    d->init(at);
}

QmlGraphicsBasePositioner::QmlGraphicsBasePositioner(QmlGraphicsBasePositionerPrivate &dd, PositionerType at, QmlGraphicsItem *parent)
    : QmlGraphicsItem(dd, parent)
{
    Q_D(QmlGraphicsBasePositioner);
    d->init(at);
}

int QmlGraphicsBasePositioner::spacing() const
{
    Q_D(const QmlGraphicsBasePositioner);
    return d->spacing;
}

void QmlGraphicsBasePositioner::setSpacing(int s)
{
    Q_D(QmlGraphicsBasePositioner);
    if (s==d->spacing)
        return;
    d->spacing = s;
    prePositioning();
    emit spacingChanged();
}

QmlTransition *QmlGraphicsBasePositioner::move() const
{
    Q_D(const QmlGraphicsBasePositioner);
    return d->moveTransition;
}

void QmlGraphicsBasePositioner::setMove(QmlTransition *mt)
{
    Q_D(QmlGraphicsBasePositioner);
    d->moveTransition = mt;
}

QmlTransition *QmlGraphicsBasePositioner::add() const
{
    Q_D(const QmlGraphicsBasePositioner);
    return d->addTransition;
}

void QmlGraphicsBasePositioner::setAdd(QmlTransition *add)
{
    Q_D(QmlGraphicsBasePositioner);
    d->addTransition = add;
}

void QmlGraphicsBasePositioner::componentComplete()
{
    QmlGraphicsItem::componentComplete();
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::BasepositionerComponentComplete> cc;
#endif
    prePositioning();
}

QVariant QmlGraphicsBasePositioner::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    if (change == ItemChildAddedChange ||
               change == ItemChildRemovedChange) {
        prePositioning();
    }

    return QmlGraphicsItem::itemChange(change, value);
}

void QmlGraphicsBasePositioner::prePositioning()
{
    Q_D(QmlGraphicsBasePositioner);
    if (!isComponentComplete())
        return;

    d->queuedPositioning = false;
    //Need to order children by creation order modified by stacking order
    QList<QGraphicsItem *> children = childItems();
    qSort(children.begin(), children.end(), d->insertionOrder);
    positionedItems.clear();
    d->newItems.clear();

    for (int ii = 0; ii < children.count(); ++ii) {
        QmlGraphicsItem *child = qobject_cast<QmlGraphicsItem *>(children.at(ii));
        if (!child)
            continue;
        if(!d->watched.contains(child))
            d->watchChanges(child);
        if(child->opacity() <= 0.0 || !child->isVisible())
            continue;
        if (!d->items.contains(child)){
            d->items += child;
            d->newItems += child;
        }
        positionedItems << child;
    }
    if(d->items.count() > positionedItems.count()){
        //Assumed that (aside from init) every add/remove triggers this check
        //thus the above check will be triggered every time an item is removed
        QSet<QmlGraphicsItem *> deletedItems = d->items - positionedItems.toSet();
        foreach(QmlGraphicsItem *child, deletedItems){
            d->unwatchChanges(child);
            d->items -= child;
        }
    }
    doPositioning();
    if(d->addTransition || d->moveTransition)
        finishApplyTransitions();
    //Set implicit size to the size of its children
    qreal h = 0.0f;
    qreal w = 0.0f;
    foreach(QmlGraphicsItem *child, d->items){
        if(!child->isVisible() || child->opacity() <= 0)
            continue;
        h = qMax(h, child->y() + child->height());
        w = qMax(w, child->x() + child->width());
    }
    setImplicitHeight(h);
    setImplicitWidth(w);
}

void QmlGraphicsBasePositioner::positionX(int x, QmlGraphicsItem* target)
{
    Q_D(QmlGraphicsBasePositioner);
    if(d->type == Horizontal || d->type == Both){
        if(!d->addTransition && !d->moveTransition){
            target->setX(x);
        }else{
            if(d->newItems.contains(target))
                d->addActions << Action(target, QLatin1String("x"), QVariant(x));
            else
                d->moveActions << Action(target, QLatin1String("x"), QVariant(x));
        }
    }
}

void QmlGraphicsBasePositioner::positionY(int y, QmlGraphicsItem* target)
{
    Q_D(QmlGraphicsBasePositioner);
    if(d->type == Vertical || d->type == Both){
        if(!d->addTransition && !d->moveTransition){
            target->setY(y);
        }else{
            if(d->newItems.contains(target))
                d->addActions << Action(target, QLatin1String("y"), QVariant(y));
            else
                d->moveActions << Action(target, QLatin1String("y"), QVariant(y));
        }
    }
}

void QmlGraphicsBasePositioner::finishApplyTransitions()
{
    Q_D(QmlGraphicsBasePositioner);
    // Note that if a transition is not set the transition manager will
    // apply the changes directly, in the case add/move aren't set
    d->addTransitionManager.transition(d->addActions, d->addTransition);
    d->moveTransitionManager.transition(d->moveActions, d->moveTransition);
    d->addActions.clear();
    d->moveActions.clear();
}

QML_DEFINE_TYPE(Qt,4,6,Column,QmlGraphicsColumn)
/*!
  \qmlclass Column QmlGraphicsColumn
  \brief The Column item lines up its children vertically.
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
    remove: ...
    add: ...
    move: ...
    ...
}
  \endqml
  \endtable

  Note that the positioner assumes that the x and y positions of its children
  will not change. If you manually change the x or y properties in script, bind
  the x or y properties, or use anchors on a child of a positioner, then the
  positioner may exhibit strange behaviour.

*/
/*!
    \qmlproperty Transition Column::add
    This property holds the transition to be applied when adding an item to the positioner. The transition will only be applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    Added can mean that either the object has been created or reparented, and thus is now a child or the positioner, or that the object has had its opacity increased from zero, and thus is now visible.


*/
/*!
    \qmlproperty Transition Column::move
    This property holds the transition to apply when moving an item within the positioner.
    Positioner transitions will only affect the position (x,y) of items.

    This can happen when other items are added or removed from the positioner, or when items resize themselves.

    \table
    \row
    \o \image positioner-move.gif
    \o
    \qml
Column {
    move: Transition {
        NumberAnimation {
            matchProperties: "y"
            ease: "easeOutBounce"
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

  The below example places a Grid containing a red, a blue and a
  green rectangle on a gray background. The area the grid positioner
  occupies is colored white. The top positioner has the default of no spacing,
  and the bottom positioner has its spacing set to 2.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QmlGraphicsColumn
    \brief The QmlGraphicsColumn class lines up items vertically.
    \ingroup group_positioners
*/
QmlGraphicsColumn::QmlGraphicsColumn(QmlGraphicsItem *parent)
: QmlGraphicsBasePositioner(Vertical, parent)
{
}

static inline bool isInvisible(QmlGraphicsItem *child)
{
    return child->opacity() == 0.0 || !child->isVisible() || !child->width() || !child->height();
}

void QmlGraphicsColumn::doPositioning()
{
    int voffset = 0;

    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        QmlGraphicsItem *child = positionedItems.at(ii);
        if (!child || isInvisible(child))
            continue;

        if(child->y() != voffset)
            positionY(voffset, child);

        voffset += child->height();
        voffset += spacing();
    }
}

QML_DEFINE_TYPE(Qt,4,6,Row,QmlGraphicsRow)
/*!
  \qmlclass Row QmlGraphicsRow
  \brief The Row item lines up its children horizontally.
  \inherits Item

  The Row item positions its child items so that they are
  horizontally aligned and not overlapping. Spacing can be added between the
  items, and a margin around all items can also be added. It also provides for
  transitions to be set when items are added, moved, or removed in the
  positioner. Adding and removing apply both to items which are deleted or have
  their position in the document changed so as to no longer be children of the
  positioner, as well as to items which have their opacity set to or from zero
  so as to appear or disappear.

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
  the x or y properties, or use anchors on a child of a positioner, then the
  positioner may exhibit strange behaviour.

*/
/*!
    \qmlproperty Transition Row::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition will only be applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    Added can mean that either the object has been created or reparented, and thus is now a child or the positioner, or that the object has had its opacity increased from zero, and thus is now visible.


*/
/*!
    \qmlproperty Transition Row::move
    This property holds the transition to apply when moving an item within the positioner.
    Positioner transitions will only affect the position (x,y) of items.

    This can happen when other items are added or removed from the positioner, or when items resize themselves.

    \qml
Row {
    id: positioner
    move: Transition {
        NumberAnimation {
            matchProperties: "x"
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

  The below example places a Grid containing a red, a blue and a
  green rectangle on a gray background. The area the grid positioner
  occupies is colored white. The top positioner has the default of no spacing,
  and the bottom positioner has its spacing set to 2.

  \image spacing_a.png
  \image spacing_b.png

*/
/*!
    \internal
    \class QmlGraphicsRow
    \brief The QmlGraphicsRow class lines up items horizontally.
    \ingroup group_positioners
*/
QmlGraphicsRow::QmlGraphicsRow(QmlGraphicsItem *parent)
: QmlGraphicsBasePositioner(Horizontal, parent)
{
}

void QmlGraphicsRow::doPositioning()
{
    int hoffset = 0;

    for (int ii = 0; ii < positionedItems.count(); ++ii) {
        QmlGraphicsItem *child = positionedItems.at(ii);
        if (!child || isInvisible(child))
            continue;

        if(child->x() != hoffset)
            positionX(hoffset, child);

        hoffset += child->width();
        hoffset += spacing();
    }
}

QML_DEFINE_TYPE(Qt,4,6,Grid,QmlGraphicsGrid)

/*!
  \qmlclass Grid QmlGraphicsGrid
  \brief The Grid item positions its children in a grid.
  \inherits Item

  The Grid item positions its child items so that they are
  aligned in a grid and are not overlapping. Spacing can be added
  between the items. It also provides for transitions to be set when items are
  added, moved, or removed in the positioner. Adding and removing apply
  both to items which are deleted or have their position in the
  document changed so as to no longer be children of the positioner, as
  well as to items which have their opacity set to or from zero so
  as to appear or disappear.

  The Grid defaults to using four columns, and as many rows as
  are necessary to fit all the child items. The number of rows
  and/or the number of columns can be constrained by setting the rows
  or columns properties. The grid positioner calculates a grid with
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
  the x or y properties, or use anchors on a child of a positioner, then the
  positioner may exhibit strange behaviour.
*/
/*!
    \qmlproperty Transition Grid::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition is only applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    Added can mean that either the object has been created or
    reparented, and thus is now a child or the positioner, or that the
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
            matchProperties: "x,y"
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
    \class QmlGraphicsGrid
    \brief The QmlGraphicsGrid class lays out items in a grid.
    \ingroup group_layouts

*/
QmlGraphicsGrid::QmlGraphicsGrid(QmlGraphicsItem *parent) :
    QmlGraphicsBasePositioner(Both, parent)
{
    _columns=-1;
    _rows=-1;
}

/*!
    \qmlproperty int Grid::columns
    This property holds the number of columns in the grid.

    When the columns property is set the Grid will always have
    that many columns. Note that if you do not have enough items to
    fill this many columns some columns will be of zero width.
*/

/*!
    \qmlproperty int Grid::rows
    This property holds the number of rows in the grid.

    When the rows property is set the Grid will always have that
    many rows. Note that if you do not have enough items to fill this
    many rows some rows will be of zero width.
*/

void QmlGraphicsGrid::doPositioning()
{
    int c=_columns,r=_rows;//Actual number of rows/columns
    int numVisible = positionedItems.count();
    if (_columns==-1 && _rows==-1){
        c = 4;
        r = (numVisible+3)/4;
    }else if (_rows==-1){
        r = (numVisible+(_columns-1))/_columns;
    }else if (_columns==-1){
        c = (numVisible+(_rows-1))/_rows;
    }

    QList<int> maxColWidth;
    QList<int> maxRowHeight;
    int childIndex =0;
    for (int i=0; i<r; i++){
        for (int j=0; j<c; j++){
            if (j==0)
                maxRowHeight << 0;
            if (i==0)
                maxColWidth << 0;

            if (childIndex == positionedItems.count())
                continue;
            QmlGraphicsItem *child = positionedItems.at(childIndex++);
            if (!child || isInvisible(child))
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
    foreach(QmlGraphicsItem* child, positionedItems){
        if (!child || isInvisible(child))
            continue;
        if((child->x()!=xoffset)||(child->y()!=yoffset)){
            positionX(xoffset, child);
            positionY(yoffset, child);
        }
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
    }
}


QML_DEFINE_TYPE(Qt,4,6,Flow,QmlGraphicsFlow)
/*!
  \qmlclass Flow QmlGraphicsFlow
  \brief The Flow item lines up its children side by side, wrapping as necessary.
  \inherits Item


*/
/*!
    \qmlproperty Transition Flow::add
    This property holds the transition to apply when adding an item to the positioner.
    The transition will only be applied to the added item(s).
    Positioner transitions will only affect the position (x,y) of items.

    Added can mean that either the object has been created or reparented, and thus is now a child or the positioner, or that the object has had its opacity increased from zero, and thus is now visible.


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
            matchProperties: "x,y"
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

class QmlGraphicsFlowPrivate : public QmlGraphicsBasePositionerPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsFlow)

public:
    QmlGraphicsFlowPrivate()
        : QmlGraphicsBasePositionerPrivate(), flow(QmlGraphicsFlow::LeftToRight)
    {}

    QmlGraphicsFlow::Flow flow;
};

QmlGraphicsFlow::QmlGraphicsFlow(QmlGraphicsItem *parent)
: QmlGraphicsBasePositioner(*(new QmlGraphicsFlowPrivate), Both, parent)
{
}

/*!
    \qmlproperty enumeration Flow::flow
    This property holds the flow of the layout.

    Possible values are \c LeftToRight (default) and \c TopToBottom.

    If \a flow is \c LeftToRight, the items are positioned next to
    to each other from left to right until the width of the Flow
    is exceeded, then wrapped to the next line.
    If \a flow is \c TopToBottom, the items are positioned next to each
    other from top to bottom until the height of the Flow is exceeded,
    then wrapped to the next column.
*/
QmlGraphicsFlow::Flow QmlGraphicsFlow::flow() const
{
    Q_D(const QmlGraphicsFlow);
    return d->flow;
}

void QmlGraphicsFlow::setFlow(Flow flow)
{
    Q_D(QmlGraphicsFlow);
    if (d->flow != flow) {
        d->flow = flow;
        prePositioning();
        emit flowChanged();
    }
}

void QmlGraphicsFlow::doPositioning()
{
    Q_D(QmlGraphicsFlow);

    int hoffset = 0;
    int voffset = 0;
    int linemax = 0;

    foreach(QmlGraphicsItem* child, positionedItems){
        if (!child || isInvisible(child))
            continue;

        if (d->flow == LeftToRight)  {
            if (hoffset && hoffset + child->width() > width()) {
                hoffset = 0;
                voffset += linemax + spacing();
                linemax = 0;
            }
        } else {
            if (voffset && voffset + child->height() > height()) {
                voffset = 0;
                hoffset += linemax + spacing();
                linemax = 0;
            }
        }

        if(child->x() != hoffset || child->y() != voffset){
            positionX(hoffset, child);
            positionY(voffset, child);
        }

        if (d->flow == LeftToRight)  {
            hoffset += child->width();
            hoffset += spacing();
            linemax = qMax(linemax, qCeil(child->height()));
        } else {
            voffset += child->height();
            voffset += spacing();
            linemax = qMax(linemax, qCeil(child->width()));
        }
    }
}


QT_END_NAMESPACE
