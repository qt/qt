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
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QGraphicsSceneMouseEvent>
#include <QtScript/qscriptengine.h>
#include <private/qfxperf_p.h>
#include <QtGui/qgraphicstransform.h>

#include <QtDeclarative/qmlengine.h>
#include "qmlstate.h"
#include "qlistmodelinterface.h"
#include "qfxanchors_p.h"

#include "qfxscalegrid.h"
#include "qfxview.h"
#include "qmlstategroup.h"

#include "qfxitem_p.h"
#include "qfxitem.h"
#include "qfxevents_p.h"
#include <qmlcomponent.h>

QT_BEGIN_NAMESPACE

#ifndef FLT_MAX
#define FLT_MAX 1E+37
#endif

QML_DEFINE_NOCREATE_TYPE(QFxContents)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Item,QFxItem)

QML_DEFINE_NOCREATE_TYPE(QGraphicsTransform);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Scale,QGraphicsScale)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Rotation,QGraphicsRotation)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Rotation3D,QGraphicsRotation3D)

/*!
    \group group_animation
    \title Animation
*/

/*!
    \group group_coreitems
    \title Basic Items
*/

/*!
    \group group_effects
    \title Effects
*/

/*!
    \group group_layouts
    \title Layouts
*/

/*!
    \group group_states
    \title States and Transitions
*/

/*!
    \group group_utility
    \title Utility
*/

/*!
    \group group_views
    \title Views
*/

/*!
    \group group_widgets
    \title Widgets
*/

/*!
    \internal
    \class QFxContents
    \ingroup group_utility
    \brief The QFxContents class gives access to the height and width of an item's contents.

*/

QFxContents::QFxContents() : m_height(0), m_width(0)
{
}

/*!
    \qmlproperty qreal Item::contents.width
    \qmlproperty qreal Item::contents.height

    The contents properties allow an item access to the size of its
    children. This property is useful if you have an item that needs to be
    sized to fit its children.    
*/

/*!
    \property QFxContents::height
    \brief The height of the contents.
*/
qreal QFxContents::height() const
{
    return m_height;
}

/*!
    \property QFxContents::width
    \brief The width of the contents.
*/
qreal QFxContents::width() const
{
    return m_width;
}

//TODO: optimization: only check sender(), if there is one
void QFxContents::calcHeight()
{
    qreal oldheight = m_height;

    qreal top = FLT_MAX;
    qreal bottom = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
        qreal y = child->y();
        if (y + child->height() > bottom)
            bottom = y + child->height();
        if (y < top)
            top = y;
    }
    m_height = qMax(bottom - top, qreal(0.0));

    if (m_height != oldheight)
        emit heightChanged();
}

//TODO: optimization: only check sender(), if there is one
void QFxContents::calcWidth()
{
    qreal oldwidth = m_width;

    qreal left = FLT_MAX;
    qreal right = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
        qreal x = child->x();
        if (x + child->width() > right)
            right = x + child->width();
        if (x < left)
            left = x;
    }
    m_width = qMax(right - left, qreal(0.0));

    if (m_width != oldwidth)
        emit widthChanged();
}

void QFxContents::setItem(QFxItem *item)
{
    m_item = item;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
        connect(child, SIGNAL(heightChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(yChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(widthChanged()), this, SLOT(calcWidth()));
        connect(child, SIGNAL(xChanged()), this, SLOT(calcWidth()));
    }

    calcHeight();
    calcWidth();
}

/*!
    \qmlclass Item QFxItem
    \brief The Item is the most basic of all visual items in QML.
 */

/*!
    \class QFxItem Item
    \brief The QFxItem class is a generic QFxView item. It is the base class for all other view items.

    \qmltext
    All visual items in Qt Declarative inherit from QFxItem.  Although QFxItem
    has no visual appearance, it defines all the properties that are
    common across visual items - like the x and y position, and the
    width and height.

    QFxItem is also useful for grouping items together.

    \qml
    Item {
        Image {
            file: "tile.png"
        }
        Image {
            x: 80
            width: 100
            height: 100
            file: "tile.png"
        }
        Image {
            x: 190
            width: 100
            height: 100
            tile: true
            file: "tile.png"
        }
    }
    \endqml
    \endqmltext
    
    \ingroup group_coreitems
*/

/*!
    \fn void QFxItem::activeFocusChanged()

    This signal is emitted when this item gains active focus.
*/

/*!
    \fn void QFxItem::baselineOffsetChanged()

    This signal is emitted when the baseline offset of the item
    is changed.
*/

/*!
    \fn void QFxItem::xChanged()

    This signal is emitted when the x coordinate of the item changes.
*/

/*!
    \fn void QFxItem::yChanged()

    This signal is emitted when the y coordinate of the item changes.
*/

/*!
    \fn void QFxItem::widthChanged()

    This signal is emitted when the width of the item changes.
*/

/*!
    \fn void QFxItem::heightChanged()

    This signal is emitted when the height of the item changes.
*/

/*!
    \fn void QFxItem::hcenterChanged()

    This signal is emitted when the horizontal center coordinate of the item changes.
*/

/*!
    \fn void QFxItem::vcenterChanged()

    This signal is emitted when the vertical center coordinate of the item changes.
*/

/*!
    \fn void QFxItem::stateChanged(const QString &state)

    This signal is emitted when the \a state of the item changes.

    \sa states-transitions
*/

/*!
    \qmlsignal Item::onKeyPress(event)

    This handler is called when a key is pressed.

    The key event is available via the KeyEvent \a event.

    \qml
    Item {
        onKeyPress: { if (event.key == Qt.Key_Enter) state='Enter' }
    }
    \endqml
*/

/*!
    \qmlsignal Item::onKeyRelease(event)

    This handler is called when a key is released.

    The key event is available in via the KeyEvent \a event.

    \qml
    Item {
        onKeyRelease: { if (event.key == Qt.Key_Enter) state='Enter' }
    }
    \endqml
*/

/*!
    \fn void QFxItem::parentChanged()

    This signal is emitted when the parent of the item changes.

    \sa setParentItem()
*/

/*!
    \fn void QFxItem::focusChanged()

    This signal is emitted when the item's focus state changes.

    \sa QFxItem::setFocus()
*/

/*!
    \fn QFxItem::QFxItem(QFxItem *parent)

    Constructs a QFxItem with the given \a parent.
*/
QFxItem::QFxItem(QFxItem* parent)
  : QGraphicsObject(*(new QFxItemPrivate), parent, 0)
{
    Q_D(QFxItem);
    d->init(parent);
}

/*! \internal
*/
QFxItem::QFxItem(QFxItemPrivate &dd, QFxItem *parent)
  : QGraphicsObject(dd, parent, 0)
{
    Q_D(QFxItem);
    d->init(parent);
}

/*! \internal
*/
void QFxItem::doUpdate()
{
    update();
}

/*!
    Destroys the QFxItem.
*/
QFxItem::~QFxItem()
{
    Q_D(QFxItem);
    for (int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->clearItem(this);
    }
    for (int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        if (anchor->d_func()->item && anchor->d_func()->item->parentItem() != this) //child will be deleted anyway
            anchor->d_func()->updateOnComplete();
    }
    delete d->_anchorLines; d->_anchorLines = 0;
}

/*!
    \qmlproperty enum Item::transformOrigin
    This property holds the origin point around which scale and rotation transform.

    Nine transform origins are available, as shown in the image below.

    \image declarative-transformorigin.png

    This example scales an image about its center.
    \qml
    Image {
        source: "myimage.png"
        transformOrigin: "Center"
        scale: 4
    }
    \endqml

    The default transform origin is \c TopLeft.
*/

/*!
    \qmlproperty Item Item::parent
    This property holds the parent of the item.
*/

/*!
    \property QFxItem::parent
    This property holds the parent of the item.
*/
void QFxItem::setParentItem(QFxItem *parent)
{
    QFxItem *oldParent = parentItem();
    if (parent == oldParent || !parent) return;

    QObject::setParent(parent);
    QGraphicsObject::setParentItem(parent);
}

/*!
    Returns the QFxItem parent of this item.
*/
QFxItem *QFxItem::parentItem() const
{
    return qobject_cast<QFxItem *>(QGraphicsObject::parentItem());
}

/*!
    \qmlproperty list<Item> Item::children
    \qmlproperty list<Object> Item::resources

    The children property contains the list of visual children of this item. 
    The resources property contains non-visual resources that you want to 
    reference by name.
    
    Generally you can rely on Item's default property to handle all this for
    you, but it can come in handy in some cases.

    \qml
    Item {
        children: [
            Text {},
            Rect {}
        ]
        resources: [
            Component {
                id: myComponent
                Text {}
            }
        ]
    }
    \endqml
*/

/*!
    \property QFxItem::children

    This property contains the list of visual children of this item. 
*/

/*!
    \property QFxItem::resources

    This property contains non-visual resources that you want to 
    reference by name.
*/

/*!
    Returns true if construction of the QML component is complete; otherwise
    returns false.
    
    It is often desireable to delay some processing until the component is
    completed.

    \sa componentComplete()
*/
bool QFxItem::isComponentComplete() const
{
    Q_D(const QFxItem);
    return d->_componentComplete;
}

/*!
    \property QFxItem::anchors
    \brief The anchors (alignments) used by the item.
*/
QFxAnchors *QFxItem::anchors()
{
    Q_D(QFxItem);
    return d->anchors();
}

void QFxItemPrivate::data_removeAt(int)
{
    // ###
}

int QFxItemPrivate::data_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::data_append(QObject *o)
{
    Q_Q(QFxItem);
    QFxItem *i = qobject_cast<QFxItem *>(o);
    if (i)
        q->children()->append(i);
    else
        resources_append(o);
}

void QFxItemPrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QFxItemPrivate::data_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::data_clear()
{
    // ###
}

void QFxItemPrivate::resources_removeAt(int)
{
    // ###
}

int QFxItemPrivate::resources_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::resources_append(QObject *o)
{
    Q_Q(QFxItem);
    o->setParent(q);
}

void QFxItemPrivate::resources_insert(int, QObject *)
{
    // ###
}

QObject *QFxItemPrivate::resources_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::resources_clear()
{
    // ###
}

void QFxItemPrivate::children_removeAt(int)
{
    // ###
}

int QFxItemPrivate::children_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::children_append(QFxItem *i)
{
    Q_Q(QFxItem);
    i->setParentItem(q);
}

void QFxItemPrivate::children_insert(int, QFxItem *)
{
    // ###
}

QFxItem *QFxItemPrivate::children_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::children_clear()
{
    // ###
}


void QFxItemPrivate::transform_removeAt(int i)
{
    if (!transformData)
        return;
    transformData->graphicsTransforms.removeAt(i);
    dirtySceneTransform = 1;
}

int QFxItemPrivate::transform_count() const
{
    return transformData ? transformData->graphicsTransforms.size() : 0;
}

void QFxItemPrivate::transform_append(QGraphicsTransform *item)
{
    appendGraphicsTransform(item);
}

void QFxItemPrivate::transform_insert(int, QGraphicsTransform *)
{
    // ###
}

QGraphicsTransform *QFxItemPrivate::transform_at(int idx) const
{
    if (!transformData)
        return 0;
    return transformData->graphicsTransforms.at(idx);
}

void QFxItemPrivate::transform_clear()
{
    if (!transformData)
        return;
    Q_Q(QFxItem);
    q->setTransformations(QList<QGraphicsTransform *>());
}

/*!
    \qmlproperty list<Object> Item::data
    \default

    The data property is allows you to freely mix visual children and resources
    of an item.  If you assign a visual item to the data list it becomes
    a child and if you assign any other object type, it is added as a resource.

    So you can write:
    \qml
    Item {
        Text {}
        Rect {}
        Script {}
    }
    \endqml

    instead of:
    \qml
    Item {
        children: [
            Text {},
            Rect {}
        ]
        resources: [
            Script {}
        ]
    }
    \endqml

    data is a behind-the-scenes property: you should never need to explicitly 
    specify it.
 */

/*!
    \property QFxItem::data

    The data property is allows you to freely mix the visual children
    and the non-visual resources of an item.  If you assign a visual
    item to the data list it becomes a child and if you assign any
    other object type, it is added as a resource.

    data is a behind-the-scenes property: you should never need to
    explicitly specify it.
 */
QmlList<QObject *> *QFxItem::data()
{
    Q_D(QFxItem);
    return &d->data;
}

/*!
    \property QFxItem::contents
    \brief An object that knows about the size of an item's children.

    contents provides an easy way to access the (collective) width and
    height of the item's children.
*/
QFxContents *QFxItem::contents()
{
    Q_D(QFxItem);
    if (!d->_contents) {
        d->_contents = new QFxContents;
        d->_contents->setParent(this);
        d->_contents->setItem(this);
    }
    return d->_contents;
}

bool QFxItem::clip() const
{
    return flags() & ItemClipsChildrenToShape;
}

void QFxItem::setClip(bool c)
{
    setFlag(ItemClipsChildrenToShape, c);
}

/*!
  \qmlproperty real Item::x
  \qmlproperty real Item::y
  \qmlproperty real Item::width
  \qmlproperty real Item::height

  Defines the item's position and size relative to its parent.

  \qml
  Item { x: 100; y: 100; width: 100; height: 100 }
  \endqml
 */

/*!
  \property QFxItem::width

  Defines the item's width relative to its parent.
 */

/*!
  \property QFxItem::height

  Defines the item's height relative to its parent.
 */

/*!
  \property QFxItem::x

  The x coordinate of the item relative to its parent.
*/

/*!
  \property QFxItem::y

  The y coordinate of the item relative to its parent.
*/

/*!
  \property QFxItem::z

  The z coordinate of the item relative to its parent.

  A negative z coordinate means the item will be painted below its parent.
*/


/*!
  \qmlproperty real Item::z

  Sets the stacking order of the item.  By default the stacking order is 0.
    
  Items with a higher stacking value are drawn on top of items with a
  lower stacking order.  Items with the same stacking value are drawn
  bottom up in the order they appear.  Items with a negative stacking
  value are drawn under their parent's content.
    
  The following example shows the various effects of stacking order.

  \table
  \row
  \o \image declarative-item_stacking1.png
  \o Same \c z - later children above earlier children:
  \qml
  Item {
      Rect {
          color: "red"
          width: 100; height: 100
      }
      Rect {
          color: "blue"
          x: 50; y: 50; width: 100; height: 100
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking2.png
  \o Higher \c z on top:
  \qml
  Item {
      Rect {
          z: 1
          color: "red"
          width: 100; height: 100
      }
      Rect {
          color: "blue"
          x: 50; y: 50; width: 100; height: 100
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking3.png
  \o Same \c z - children above parents:
  \qml
  Item {
      Rect {
          color: "red"
          width: 100; height: 100
          Rect {
              color: "blue"
              x: 50; y: 50; width: 100; height: 100
          }
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking4.png
  \o Lower \c z below:
  \qml
  Item {
      Rect {
          color: "red"
          width: 100; height: 100
          Rect {
              z: -1
              color: "blue"
              x: 50; y: 50; width: 100; height: 100
          }
      }
  }
  \endqml
  \endtable
 */

/*!
  This function is called to handle this item's changes in
  geometry from \a oldGeometry to \a newGeometry. If the two
  geometries are the same, it doesn't do anything.
 */
void QFxItem::geometryChanged(const QRectF &newGeometry, 
                              const QRectF &oldGeometry)
{
    Q_D(QFxItem);

    if (d->_anchors)
        d->_anchors->d_func()->updateMe();

    if (transformOrigin() != QFxItem::TopLeft)
        setTransformOriginPoint(d->computeTransformOrigin());

    if (newGeometry.x() != oldGeometry.x()) 
        emit xChanged();
    if (newGeometry.width() != oldGeometry.width())
        emit widthChanged();
    if (newGeometry.y() != oldGeometry.y()) 
        emit yChanged();
    if (newGeometry.height() != oldGeometry.height())
        emit heightChanged();

    for(int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->update(this, newGeometry, oldGeometry);
    }
}

/*! \fn void QFxItem::keyPress(QFxKeyEvent *event)
  This signal is emitted by keyPressEvent() for the \a event.
 */

/*! \fn void QFxItem::keyRelease(QFxKeyEvent *event)
  This signal is emitted by keyReleaseEvent() for the \a event.
 */

/*!
  \reimp
*/
void QFxItem::keyPressEvent(QKeyEvent *event)
{
    QFxKeyEvent ke(*event);
    emit keyPress(&ke);
    event->setAccepted(ke.isAccepted());
    if (parentItem() && !ke.isAccepted())
        parentItem()->keyPressEvent(event);
}

/*!
  \reimp
*/
void QFxItem::keyReleaseEvent(QKeyEvent *event)
{
    QFxKeyEvent ke(*event);
    emit keyRelease(&ke);
    event->setAccepted(ke.isAccepted());
    if (parentItem() && !ke.isAccepted())
        parentItem()->keyReleaseEvent(event);
}

/*!
  \qmlproperty string Item::id
  This property holds the identifier for the item.
  
  The identifier can be used in bindings and other expressions to
  refer to the item. For example:

  \qml
  Text { id: myText; ... }
  Text { text: myText.text }
  \endqml

  The identifier is available throughout to the \l {components}{component}
  where it is declared.  The identifier must be unique in thecomponent.
*/

/*!
    \internal
*/
QFxAnchorLine QFxItem::left() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->left;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::right() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->right;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::horizontalCenter() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->hCenter;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::top() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->top;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::bottom() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->bottom;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::verticalCenter() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->vCenter;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::baseline() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->baseline;
}

/*!
  \property QFxItem::top

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::bottom

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::left

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::right

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::horizontalCenter

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::verticalCenter

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \qmlproperty AnchorLine Item::top
  \qmlproperty AnchorLine Item::bottom
  \qmlproperty AnchorLine Item::left
  \qmlproperty AnchorLine Item::right
  \qmlproperty AnchorLine Item::horizontalCenter
  \qmlproperty AnchorLine Item::verticalCenter
  \qmlproperty AnchorLine Item::baseline

  The anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \qmlproperty AnchorLine Item::anchors.top
  \qmlproperty AnchorLine Item::anchors.bottom
  \qmlproperty AnchorLine Item::anchors.left
  \qmlproperty AnchorLine Item::anchors.right
  \qmlproperty AnchorLine Item::anchors.horizontalCenter
  \qmlproperty AnchorLine Item::anchors.verticalCenter
  \qmlproperty AnchorLine Item::anchors.baseline

  \qmlproperty Item Item::anchors.fill
  \qmlproperty Item Item::anchors.centerIn

  \qmlproperty real Item::anchors.topMargin
  \qmlproperty real Item::anchors.bottomMargin
  \qmlproperty real Item::anchors.leftMargin
  \qmlproperty real Item::anchors.rightMargin
  \qmlproperty real Item::anchors.horizontalCenterOffset
  \qmlproperty real Item::anchors.verticalCenterOffset
  \qmlproperty real Item::anchors.baselineOffset
  
  Anchors provide a way to position an item by specifying its
  relationship with other items.

  Margins apply to top, bottom, left, right, and fill anchors.

  Offsets apply for horizontal center, vertical center, and baseline anchors.

  \table
  \row
  \o \image declarative-anchors_example.png
  \o Text anchored to Image, horizontally centered and vertically below, with a margin.
  \qml
  Image { id: pic; ... }
  Text {
      id: label
      anchors.horizontalCenter: pic.horizontalCenter
      anchors.top: pic.bottom
      anchors.topMargin: 5
      ...
  }
  \endqml
  \row
  \o \image declarative-anchors_example2.png
  \o
  Left of Text anchored to right of Image, with a margin. The y
  property of both defaults to 0.

  \qml
    Image { id: pic; ... }
    Text {
        id: label
        anchors.left: pic.right
        anchors.leftMargin: 5
        ...
    }
  \endqml
  \endtable

  anchors.fill provides a convenient way for one item to have the
  same geometry as another item, and is equivalent to connecting all
  four directional anchors.

  \note You can only anchor an item to siblings or a parent.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::baselineOffset
  \brief The position of the item's baseline in local coordinates.

  The baseline of a Text item is the imaginary line on which the text
  sits. Controls containing text usually set their baseline to the
  baseline of their text.

  For non-text items, a default baseline offset of 0 is used.
*/
qreal QFxItem::baselineOffset() const
{
    Q_D(const QFxItem);
    if (!d->_baselineOffset.isValid()) {
        return 0.0;
    } else
        return d->_baselineOffset;
}

/*!
    \internal
*/
void QFxItem::setBaselineOffset(qreal offset)
{
    Q_D(QFxItem);
    if (offset == d->_baselineOffset)
        return;

    d->_baselineOffset = offset;
    emit baselineOffsetChanged();

    for(int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->updateVerticalAnchors();
    }
}

/*!
  \qmlproperty real Item::rotation
  This property holds the rotation of the item in degrees.

  This specifies how many degrees to rotate the item around its transformOrigin.
  The default rotation is 0 degrees (i.e. not rotated at all).

  \table
  \row
  \o \image declarative-rotation.png
  \o
  \qml
  Rect {
      color: "blue"
      width: 100; height: 100
      Rect {
          color: "green"
          width: 25; height: 25
      }
      Rect {
          color: "red"
          x: 25; y: 25; width: 50; height: 50
          rotation: 30
      }
  }
  \endqml
  \endtable
*/

/*!
  \qmlproperty real Item::scale
  This property holds the scale of the item.

  A scale of less than 1 means the item will be displayed smaller than
  normal, and a scale of greater than 1 means the item will be
  displayed larger than normal.  A negative scale means the item will
  be mirrored.

  By default, items are displayed at a scale of 1 (i.e. at their
  normal size).

  Scaling is from the item's transformOrigin.

  \table
  \row
  \o \image declarative-scale.png
  \o
  \qml
  Rect {
      color: "blue"
      width: 100; height: 100
      Rect {
          color: "green"
          width: 25; height: 25
      }
      Rect {
          color: "red"
          x: 25; y: 25; width: 50; height: 50
          scale: 1.4
      }
  }
  \endqml
  \endtable
*/

/*!
  \qmlproperty real Item::opacity

  The opacity of the item.  Opacity is specified as a number between 0
  (fully transparent) and 1 (fully opaque).  The default is 1.

  Opacity is an \e inherited attribute.  That is, the opacity is
  also applied individually to child items.  In almost all cases this
  is what you want.  If you can spot the issue in the following
  example, you might need to use an opacity filter (not yet available) instead.

  \table
  \row
  \o \image declarative-item_opacity1.png
  \o
  \qml
    Item {
        Rect {
            color: "red"
            width: 100; height: 100
            Rect {
                color: "blue"
                x: 50; y: 50; width: 100; height: 100
            }
        }
    }
  \endqml
  \row
  \o \image declarative-item_opacity2.png
  \o
  \qml
    Item {
        Rect {
            opacity: 0.5
            color: "red"
            width: 100; height: 100
            Rect {
                color: "blue"
                x: 50; y: 50; width: 100; height: 100
            }
        }
    }
  \endqml
  \endtable
*/

/*!
  Returns a value indicating whether the mouse should
  remain with this item.
 */
bool QFxItem::keepMouseGrab() const
{
    Q_D(const QFxItem);
    return d->_keepMouse;
}

/*!
  The flag indicating whether the mouse should remain
  with this item is set to \a keep.
 */
void QFxItem::setKeepMouseGrab(bool keep)
{
    Q_D(QFxItem);
    d->_keepMouse = keep;
}

/*!
  This function emits the \e activeFocusChanged signal.
  \a flag is not used.
 */
void QFxItem::activeFocusChanged(bool flag)
{
    Q_UNUSED(flag);
    emit activeFocusChanged();
}

/*!
  This function emits the \e focusChanged signal.
  \a flag is not used.
 */
void QFxItem::focusChanged(bool flag)
{
    Q_UNUSED(flag);
    emit focusChanged();
}

QmlList<QFxItem *> *QFxItem::children()
{
    Q_D(QFxItem);
    return &(d->children);
}

QmlList<QObject *> *QFxItem::resources()
{
    Q_D(QFxItem);
    return &(d->resources);
}

/*!
  \qmlproperty list<State> Item::states
  This property holds a list of states defined by the item.

  \qml
  Item {
    states: [
      State { ... },
      State { ... }
      ...
    ]
  }
  \endqml

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::states
  This property holds a list of states defined by the item.

  \sa {states-transitions}{States and Transitions}
*/
QmlList<QmlState *>* QFxItem::states()
{
    Q_D(QFxItem);
    return d->states()->statesProperty();
}

/*!
  \qmlproperty list<Transition> Item::transitions
  This property holds a list of transitions defined by the item.

  \qml
  Item {
    transitions: [
      Transition { ... },
      Transition { ... }
      ...
    ]
  }
  \endqml

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::transitions
  This property holds a list of transitions defined by the item.

  \sa {states-transitions}{States and Transitions}
*/
QmlList<QmlTransition *>* QFxItem::transitions()
{
    Q_D(QFxItem);
    return d->states()->transitionsProperty();
}

/*!
  \internal
  \property QFxItem::filter
*/

/*!
  \qmlproperty list<Filter> Item::filter
  This property holds a list of graphical filters to be applied to the item.

  \l {Filter}{Filters} include things like \l {Blur}{blurring}
  the item, or giving it a \l Reflection.  Some
  filters may not be available on all canvases; if a filter is not
  available on a certain canvas, it will simply not be applied for
  that canvas (but the QML will still be considered valid).

  \qml
  Item {
    filter: [
      Blur { ... },
      Relection { ... }
      ...
    ]
  }
  \endqml
*/

/*!
  \qmlproperty bool Item::clip
  This property holds whether clipping is enabled.

  if clipping is enabled, an item will clip its own painting, as well
  as the painting of its children, to its bounding rectangle.

  Non-rectangular clipping regions are not supported for performance reasons.
*/

/*!
  \property QFxItem::clip
  This property holds whether clipping is enabled.

  if clipping is enabled, an item will clip its own painting, as well
  as the painting of its children, to its bounding rectangle.

  Non-rectangular clipping regions are not supported for performance reasons.
*/

/*!
  \qmlproperty string Item::state

  This property holds the name of the current state of the item.

  This property is often used in scripts to change between states. For
  example:

  \qml
    Script {
        function toggle() {
            if (button.state == 'On')
                button.state = 'Off';
            else
                button.state = 'On';
        }
    }
  \endqml

  If the item is in its base state (i.e. no explicit state has been
  set), \c state will be a blank string. Likewise, you can return an
  item to its base state by setting its current state to \c ''.

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::state

  This property holds the name of the current state of the item.

  This property is often used in scripts to change between states. For
  example:

  \qml
    Script {
        function toggle() {
            if (button.state == 'On')
                button.state = 'Off';
            else
                button.state = 'On';
        }
    }
  \endqml

  If the item is in its base state (i.e. no explicit state has been
  set), \c state will be a blank string. Likewise, you can return an
  item to its base state by setting its current state to \c ''.

  \sa {states-transitions}{States and Transitions}
*/
QString QFxItem::state() const
{
    Q_D(const QFxItem);
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

void QFxItem::setState(const QString &state)
{
    Q_D(QFxItem);
    d->states()->setState(state);
}

/*!
  \qmlproperty list<Transform> Item::transform
  This property holds the list of transformations to apply.

  For more information see \l Transform.
*/

/*!
  \property QFxItem::transform
  This property holds a list of transformations set on the item.
*/
QmlList<QGraphicsTransform *>* QFxItem::transform()
{
    Q_D(QFxItem);
    return &(d->transform);
}

/*!
  classBegin() is called when the item is constructed, but its
  properties have not yet been set.

  \sa componentComplete(), isComponentComplete()
*/
void QFxItem::classBegin()
{
    Q_D(QFxItem);
    d->_componentComplete = false;
    if (d->_stateGroup)
        d->_stateGroup->classBegin();
    if (d->_anchors)
        d->_anchors->classBegin();
}

/*!
  componentComplete() is called when all items in the component
  have been constructed.  It is often desireable to delay some
  processing until the component is complete an all bindings in the
  component have been resolved.
*/
void QFxItem::componentComplete()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ItemComponentComplete> cc;
#endif

    Q_D(QFxItem);
    d->_componentComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->componentComplete();
    if (d->_anchors) {
        d->_anchors->componentComplete(); 
        d->_anchors->d_func()->updateOnComplete();
    }
}

QmlStateGroup *QFxItemPrivate::states()
{
    Q_Q(QFxItem);
    if (!_stateGroup) {
        _stateGroup = new QmlStateGroup(q);
        if (!_componentComplete)
            _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         q, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}

QFxItemPrivate::AnchorLines::AnchorLines(QFxItem *q)
{
    left.item = q;
    left.anchorLine = QFxAnchorLine::Left;
    right.item = q;
    right.anchorLine = QFxAnchorLine::Right;
    hCenter.item = q;
    hCenter.anchorLine = QFxAnchorLine::HCenter;
    top.item = q;
    top.anchorLine = QFxAnchorLine::Top;
    bottom.item = q;
    bottom.anchorLine = QFxAnchorLine::Bottom;
    vCenter.item = q;
    vCenter.anchorLine = QFxAnchorLine::VCenter;
    baseline.item = q;
    baseline.anchorLine = QFxAnchorLine::Baseline;
}

QPointF QFxItemPrivate::computeTransformOrigin() const
{
    Q_Q(const QFxItem);

    QRectF br = q->boundingRect();

    switch(origin) {
    default:
    case QFxItem::TopLeft:
        return QPointF(0, 0);
    case QFxItem::Top:
        return QPointF(br.width() / 2., 0);
    case QFxItem::TopRight:
        return QPointF(br.width(), 0);
    case QFxItem::Left:
        return QPointF(0, br.height() / 2.);
    case QFxItem::Center:
        return QPointF(br.width() / 2., br.height() / 2.);
    case QFxItem::Right:
        return QPointF(br.width(), br.height() / 2.);
    case QFxItem::BottomLeft:
        return QPointF(0, br.height());
    case QFxItem::Bottom:
        return QPointF(br.width() / 2., br.height());
    case QFxItem::BottomRight:
        return QPointF(br.width(), br.height());
    }
}

QFxItem::Options QFxItem::options() const
{
    Q_D(const QFxItem);
    return (QFxItem::Options)d->options;
}

bool QFxItem::mouseFilter(QGraphicsSceneMouseEvent *)
{
    return false;
}

bool QFxItem::sceneEvent(QEvent *event)
{
    bool rv = QGraphicsItem::sceneEvent(event);
    if (event->type() == QEvent::UngrabMouse)
        mouseUngrabEvent();
    return rv;
}

QVariant QFxItem::itemChange(GraphicsItemChange change, 
                                       const QVariant &value)
{
    Q_D(QFxItem);
    if (change == ItemSceneHasChanged) {
        d->canvas = qvariant_cast<QGraphicsScene *>(value);
    } else if (change == ItemChildAddedChange ||
               change == ItemChildRemovedChange) {
        childrenChanged();
    } else if (change == ItemParentHasChanged) {
        emit parentChanged();
    }

    return QGraphicsItem::itemChange(change, value);
}


void QFxItem::mouseUngrabEvent()
{
}

void QFxItem::childrenChanged()
{
}

QRectF QFxItem::boundingRect() const
{
    Q_D(const QFxItem);
    return QRectF(0, 0, d->width, d->height);
}

void QFxItem::paintContents(QPainter &)
{
}

/*!
    \enum QFxItem::TransformOrigin

    Controls the point about which simple transforms like scale apply.

    \value TopLeft The top-left corner of the item.
    \value Top The center point of the top of the item.
    \value TopRight The top-right corner of the item.
    \value Left The left most point of the vertical middle.
    \value Center The center of the item.
    \value Right The right most point of the vertical middle.
    \value BottomLeft The bottom-left corner of the item.
    \value Bottom The center point of the bottom of the item.
    \value BottomRight The bottom-right corner of the item.
*/

/*!
    Returns the current transform origin.
*/
QFxItem::TransformOrigin QFxItem::transformOrigin() const
{
    Q_D(const QFxItem);
    return d->origin;
}

/*!
    Set the transform \a origin.
*/
void QFxItem::setTransformOrigin(TransformOrigin origin)
{
    Q_D(QFxItem);
    if (origin != d->origin) {
        d->origin = origin;
        QGraphicsItem::setTransformOriginPoint(d->computeTransformOrigin());
    }
}

qreal QFxItem::width() const
{
    Q_D(const QFxItem);
    return d->width;
}

void QFxItem::setWidth(qreal w)
{
    Q_D(QFxItem);
    d->widthValid = true;
    if (d->width == w)
        return;

    qreal oldWidth = d->width;

    prepareGeometryChange();
    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), oldWidth, height()));
}

void QFxItem::setImplicitWidth(qreal w)
{
    Q_D(QFxItem);
    if (d->width == w || widthValid())
        return;

    qreal oldWidth = d->width;

    prepareGeometryChange();
    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), oldWidth, height()));
}

bool QFxItem::widthValid() const
{
    Q_D(const QFxItem);
    return d->widthValid;
}

qreal QFxItem::height() const
{
    Q_D(const QFxItem);
    return d->height;
}

void QFxItem::setHeight(qreal h)
{
    Q_D(QFxItem);
    d->heightValid = true;
    if (d->height == h)
        return;

    qreal oldHeight = d->height;

    prepareGeometryChange();
    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), width(), oldHeight));
}

void QFxItem::setImplicitHeight(qreal h)
{
    Q_D(QFxItem);
    if (d->height == h || heightValid())
        return;

    qreal oldHeight = d->height;

    prepareGeometryChange();
    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), width(), oldHeight));
}

bool QFxItem::heightValid() const
{
    Q_D(const QFxItem);
    return d->heightValid;
}

/*!
  \qmlproperty bool Item::focus
  This property indicates whether the item has has an active focus request. Set this
  property to true to request active focus.
*/

bool QFxItem::hasFocus() const
{
    const QGraphicsItem *current = this->parentItem();
    while (current && !(current->flags() & ItemAutoDetectsFocusProxy))
        current = current->parentItem();

    if (current)
        return current->focusProxy() == this;
    else
        return QGraphicsItem::hasFocus();
}

void QFxItem::setFocus(bool focus)
{
    QGraphicsScene *s = scene();
    if (!s) {
        if (focus) QGraphicsItem::setFocus(Qt::OtherFocusReason);
        else QGraphicsItem::clearFocus();
        focusChanged(focus);
        return;
    }

    QGraphicsItem *current = this->parentItem();
    while (current && !(current->flags() & ItemAutoDetectsFocusProxy))
        current = current->parentItem();

    if (!current) {
        if (focus) QGraphicsItem::setFocus(Qt::OtherFocusReason);
        else QGraphicsItem::clearFocus();
        focusChanged(focus);
        return;
    }

    if (current->focusProxy() && current->focusProxy() != this) {
        QFxItem *currentItem = qobject_cast<QFxItem *>(current->focusProxy());
        if (currentItem) 
            currentItem->setFocus(false);
    }

    if (current->focusProxy() == this && !focus)
        current->setFocusProxy(0);
    else if (focus)
        current->setFocusProxy(this);

    focusChanged(focus);
}

/*!
  \qmlproperty bool Item::activeFocus
  This property indicates whether the item has the active focus.
*/

bool QFxItem::hasActiveFocus() const
{
    return QGraphicsItem::hasFocus();
}

bool QFxItem::sceneEventFilter(QGraphicsItem *w, QEvent *e)
{
    switch(e->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
        if (mouseFilter(static_cast<QGraphicsSceneMouseEvent *>(e))) 
            return true;
        break;
    default:
        break;
    }

    return QGraphicsItem::sceneEventFilter(w, e);
}

void QFxItem::setOptions(Options options, bool set)
{
    Q_D(QFxItem);
    Options old = (Options)d->options;

    if (options & IsFocusRealm) {
        if (!set) {
            qWarning("QFxItem::setOptions: Cannot unset IsFocusRealm");
            return;
        }
    }

    if (set)
        d->options |= options;
    else
        d->options &= ~options;

    setFiltersChildEvents(d->options & ChildMouseFilter);
    setFlag(QGraphicsItem::ItemAutoDetectsFocusProxy, d->options & IsFocusRealm);
}

void QFxItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    paintContents(*p);
}

QT_END_NAMESPACE
