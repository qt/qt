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

#include "qmlengine.h"
#include "qmlstate.h"
#include "qlistmodelinterface.h"
#include "qfxanchors_p.h"

#include "qfxtransform.h"
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
QML_DEFINE_TYPE(QFxItem,Item)

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
    \fn void QFxItem::scaleChanged()

    This signal is emitted when the scale of the item changes.
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
    \fn void QFxItem::visibleChanged()

    This signal is emitted when the visibility of the item changes.

    \sa setVisible()
*/

/*!
    \fn void QFxItem::opacityChanged()

    This signal is emitted when the opacity of the item changes.

    \sa opacity(), setOpacity()
*/

/*!
    \fn void QFxItem::parentChanged()

    This signal is emitted when the parent of the item changes.

    \sa setItemParent()
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
void QFxItem::setItemParent(QFxItem *parent)
{
    setParent(parent);
}

/*!
  \internal
  \property QFxItem::moveToParent
  Playing around with view2view transitions.
 */

/*!
  \internal
 */
void QFxItem::moveToParent(QFxItem *parent)
{
    if (parent && itemParent()) {
        QPointF me = itemParent()->mapToScene(QPointF(0,0));
        QPointF them = parent->mapToScene(QPointF(0,0));

        QPointF themx = parent->mapToScene(QPointF(1,0));
        QPointF themy = parent->mapToScene(QPointF(0,1));

        themx -= them;
        themy -= them;

        setItemParent(parent);

        // XXX - this is silly and will only work in a few cases

        /*
            xDiff = rx * themx_x + ry * themy_x
            yDiff = rx * themx_y + ry * themy_y
         */

        qreal rx = 0;
        qreal ry = 0;
        qreal xDiff = them.x() - me.x();
        qreal yDiff = them.y() - me.y();


        if (themx.x() == 0.) {
            ry = xDiff / themy.x();
            rx = (yDiff - ry * themy.y()) / themx.y();
        } else if (themy.x() == 0.) {
            rx = xDiff / themx.x();
            ry = (yDiff - rx * themx.y()) / themy.y();
        } else if (themx.y() == 0.) {
            ry = yDiff / themy.y();
            rx = (xDiff - ry * themy.x()) / themx.x();
        } else if (themy.y() == 0.) {
            rx = yDiff / themx.y();
            ry = (xDiff - rx * themx.x()) / themy.x();
        } else {
            qreal div = (themy.x() * themx.y() - themy.y() * themx.x());

            if (div != 0.) 
                rx = (themx.y() * xDiff - themx.x() * yDiff) / div;

           if (themy.y() != 0.) ry = (yDiff - rx * themx.y()) / themy.y();
        }

        setX(x() - rx);
        setY(y() - ry);
    } else {
        setItemParent(parent);
    }
}

/*!
    Returns the QFxItem parent of this item.
*/
QFxItem *QFxItem::itemParent() const
{
    return qobject_cast<QFxItem *>(QGraphicsItem::parentItem());
}

QFxItem *QFxItem::parentItem() const
{
    return itemParent();
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
    Returns true if all of the attributes set via QML have been set;
    otherwise returns false.

    \sa classComplete()
*/
bool QFxItem::isClassComplete() const
{
    Q_D(const QFxItem);
    return d->_classComplete;
}

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
  \internal
  \property QFxItem::qmlItem
*/

/*! \fn QFxItem *QFxItem::qmlItem() const
  \internal
 */
QFxItem *QFxItem::qmlItem() const
{
    Q_D(const QFxItem);
    return d->qmlItem;
}

/*!
    \qmlproperty url Item::qml
    This property holds the dynamic URL of the QML for the item.

    This property is used for dynamically loading QML into the
    item. Querying for the QML only has meaning if the QML has been
    dynamically set; otherwise an empty URL is returned.
*/

/*! \fn void QFxItem::qmlChanged()
  This signal is emitted whenever the item's dynamic QML
  string changes.

  \sa setQml()
 */

/*!
    \property QFxItem::qml
    This property holds the dynamic URL of the QML for the item.

    This property is used for dynamically loading QML into the
    item. Querying for the QML only has meaning if the QML has been
    dynamically set; otherwise an empty URL is returned.
*/
QUrl QFxItem::qml() const
{
    Q_D(const QFxItem);
    return d->_qml;
}

void QFxItem::setQml(const QUrl &qml)
{
    Q_D(QFxItem);
    if (d->_qml == qml)
        return;

    if (!d->_qml.isEmpty()) {
        QmlChildren::Iterator iter = d->_qmlChildren.find(d->_qml.toString());
        if (iter != d->_qmlChildren.end())
            (*iter)->setOpacity(0.);
    }

    d->_qml = qml;
    d->qmlItem = 0;

    if (d->_qml.isEmpty()) {
        emit qmlChanged();
        return;
    }

    QmlChildren::Iterator iter = d->_qmlChildren.find(d->_qml.toString());
    if (iter != d->_qmlChildren.end()) {
        (*iter)->setOpacity(1.);
        d->qmlItem = (*iter);
        emit qmlChanged();
    } else {
        d->_qmlcomp = 
            new QmlComponent(qmlEngine(this), d->_qml, this);
        if (!d->_qmlcomp->isLoading())
            qmlLoaded();
        else
            QObject::connect(d->_qmlcomp, SIGNAL(statusChanged(QmlComponent::Status)),
                             this, SLOT(qmlLoaded()));
    }
}

/*! \fn void QFxItem::newChildCreated(const QString &url, QScriptValue v)
  This signal is emitted with the \a url and the script value \a v,
  when a new child is created.
 */

/*!
  \internal
 */
void QFxItem::qmlLoaded()
{
    Q_D(QFxItem);

    { // newChild...
        // ###
        for (int i=0; i<d->_qmlnewloading.length(); ++i) {
            QmlComponent *c = d->_qmlnewcomp.at(i);
            if (c->isLoading())
                continue;

            QmlContext *ctxt = new QmlContext(qmlContext(this));
            QObject* o = c ? c->create(ctxt):0;
            QFxItem* ret = qobject_cast<QFxItem*>(o);
            if (ret) {
                ret->setItemParent(this);
                QScriptValue v = qmlEngine(this)->scriptEngine()->newQObject(ret);
                emit newChildCreated(d->_qmlnewloading.at(i).toString(),v);
            }

            delete c;
            d->_qmlnewloading.removeAt(i);
            d->_qmlnewcomp.removeAt(i);
            --i;
        }
    }

    // setQml...
    if (d->_qmlcomp) {
        QmlContext *ctxt = new QmlContext(qmlContext(this));
        ctxt->addDefaultObject(this);

        if (!d->_qmlcomp->errors().isEmpty()) {
            qWarning() << d->_qmlcomp->errors();
            delete d->_qmlcomp;
            d->_qmlcomp = 0;
            emit qmlChanged();
            return;
        }
        QObject *obj = d->_qmlcomp->create(ctxt);
        if (!d->_qmlcomp->errors().isEmpty())
            qWarning() << d->_qmlcomp->errors();
        QFxItem *qmlChild = qobject_cast<QFxItem *>(obj);
        if (qmlChild) {
            qmlChild->setItemParent(this);
            d->_qmlChildren.insert(d->_qml.toString(), qmlChild);
            d->qmlItem = qmlChild;
        } else {
            delete qmlChild;
            d->_qml = QUrl();
        }
        delete d->_qmlcomp;
        d->_qmlcomp = 0;
        emit qmlChanged();
    }
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

    if (newGeometry.size() != oldGeometry.size()) {
        if (rotation() && transformOrigin() != QFxItem::TopLeft)
            setRotation(rotation());
        if (scale() && transformOrigin() != QFxItem::TopLeft)
            setScale(scale());
    }

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
    if (itemParent() && !ke.isAccepted())
        itemParent()->keyPressEvent(event);
}

/*!
  \reimp
*/
void QFxItem::keyReleaseEvent(QKeyEvent *event)
{
    QFxKeyEvent ke(*event);
    emit keyRelease(&ke);
    event->setAccepted(ke.isAccepted());
    if (itemParent() && !ke.isAccepted())
        itemParent()->keyReleaseEvent(event);
}

/*!
  Returns the bounding rectangle of the item in scene coordinates.
*/
QRectF QFxItem::sceneBoundingRect() const
{
    return QRectF(mapToScene(QPointF(0,0)), QSize(width(), height()));
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
  \property QFxItem::id
  This property holds the identifier for the item.
  
  The identifier can be used in bindings and other expressions to
  refer to the item. For example:

  \qml
  Text { id: myText; ... }
  Text { text: myText.text }
  \endqml

  The identifier is available throughout the \l {components}{component}
  where it is declared.  The identifier must be unique in thecomponent.
*/
QString QFxItem::id() const
{
    Q_D(const QFxItem);
    return d->_id;
}

void QFxItem::setId(const QString &id)
{
    Q_D(QFxItem);
    setObjectName(id);
    d->_id = id;
}

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
  \qmlproperty Item Item::anchors.centeredIn

  \qmlproperty real Item::anchors.topMargin
  \qmlproperty real Item::anchors.bottomMargin
  \qmlproperty real Item::anchors.leftMargin
  \qmlproperty real Item::anchors.rightMargin
  \qmlproperty real Item::anchors.horizontalCenterOffset
  \qmlproperty real Item::anchors.verticalCenterOffset
  
  Anchors provide a way to position an item by specifying its
  relationship with other items.

  Margins apply to top, bottom, left, right, and fill anchors.

  Offsets apply for horizontal and vertical center anchors.

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

/*! \fn void QFxItem::rotationChanged()
  This signal is emitted when the rotation property is changed.

  \sa setRotation()
 */

/*!
  \property QFxItem::rotation
  This property holds the rotation of the item in degrees.

  This specifies how many degrees to rotate the item around its transformOrigin.
  The default rotation is 0 degrees (i.e. not rotated at all).
*/
qreal QFxItem::rotation() const
{
    Q_D(const QFxItem);
    return d->_rotation;
}

void QFxItem::setRotation(qreal rotation)
{
    Q_D(QFxItem);
    if (d->_rotation == rotation)
        return;
    d->_rotation = rotation;
    setTransform(d->transform);
    emit rotationChanged();
}

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
  \property QFxItem::scale
  This property holds the scale of the item.

  A scale of less than 1 means the item will be displayed smaller than
  normal, and a scale of greater than 1 means the item will be
  displayed larger than normal.  A negative scale means the item will
  be mirrored.

  By default, items are displayed at a scale of 1 (i.e. at their
  normal size).

  Scaling is from the item's transformOrigin.
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
  \property QFxItem::opacity

  The opacity of the item.  Opacity is specified as a number between 0
  (fully transparent) and 1 (fully opaque).  The default is 1.

  Opacity is an \e inherited attribute.  That is, the opacity is
  also applied individually to child items. 
*/

void QFxItem::setOpacity(qreal v)
{
    if (v == opacity())
        return;

    QGraphicsItem::setOpacity(v);

    emit opacityChanged();
}

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
  Returns the state with \a name.  Returns 0 if no matching state is found.
*/
QmlState *QFxItem::findState(const QString &name) const
{
    Q_D(const QFxItem);
    if (!d->_stateGroup)
        return 0;
    else
        return d->_stateGroup->findState(name);
}

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
  This property holds the list of transformations to apply.

  For more information see \l Transform.
*/
QList<QFxTransform *> *QFxItem::transform()
{
    Q_D(QFxItem);
    return &(d->_transform);
}

/*!
  Returns true if the item is visible; otherwise returns false.

  An item is considered visible if its opacity is not 0.
*/
bool QFxItem::isVisible() const
{
    Q_D(const QFxItem);
    return d->visible;
}

/*! 
  \property QFxItem::visible

  This property specifies whether the item is visible or invisible.

  Setting visibility to false sets opacity to 0.  Setting the
  visibility to true restores the opacity to its previous value.

  \sa isVisible()
*/
void QFxItem::setVisible(bool visible)
{
    Q_D(QFxItem);
    if (visible == d->visible)
        return;

    d->visible = visible;
    if (visible)
        setOpacity(d->visibleOp);
    else {
        d->visibleOp = opacity();
        setOpacity(0);
    }

    emit visibleChanged();
}

/*! \internal
*/
QString QFxItem::propertyInfo() const
{
    return QString();
}

/*!
  Creates a new child of the given component \a type.  The
  newChildCreated() signal will be emitted when and if the child is
  successfully created.

  \preliminary
*/
void QFxItem::newChild(const QString &type)
{
    Q_D(QFxItem);

    QUrl url = qmlContext(this)->resolvedUri(QUrl(type));
    if (url.isEmpty())
        return;

    d->_qmlnewloading.append(url);
    d->_qmlnewcomp.append(new QmlComponent(qmlEngine(this), url, this));

    if (!d->_qmlnewcomp.last()->isLoading())
        qmlLoaded();
    else
        connect(d->_qmlnewcomp.last(), SIGNAL(statusChanged(QmlComponent::Status)), 
                this, SLOT(qmlLoaded()));
}

/*!
  classBegin() is called when the item is constructed, but its
  properties have not yet been set.

  \sa classComplete(), componentComplete(), isClassComplete(), isComponentComplete()
*/
void QFxItem::classBegin()
{
    Q_D(QFxItem);
    d->_classComplete = false;
    d->_componentComplete = false;
    if (d->_stateGroup)
        d->_stateGroup->classBegin();
}

/*!
  classComplete() is called when all properties specified in QML
  have been assigned.  It is sometimes desireable to delay some
  processing until all property assignments are complete.
*/
void QFxItem::classComplete()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ItemComponentComplete> cc;
#endif
    Q_D(QFxItem);
    d->_classComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->classComplete();
}

/*!
  componentComplete() is called when all items in the component
  have been constructed.  It is often desireable to delay some
  processing until the component is complete an all bindings in the
  component have been resolved.
*/
void QFxItem::componentComplete()
{
    Q_D(QFxItem);
    d->_componentComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->componentComplete();
    if (d->_anchors) 
        d->anchors()->d_func()->updateOnComplete();
    if (!d->_transform.isEmpty())
        updateTransform();
}

/*!
    \internal
*/
void QFxItem::updateTransform()
{
    Q_D(QFxItem);
    QTransform trans;
    for (int ii = d->_transform.count() - 1; ii >= 0; --ii) {
        QFxTransform *a = d->_transform.at(ii);
        if (!a->isIdentity()) 
            trans = a->transform() * trans;
    }

    setTransform(trans);
    transformChanged(trans);
}

/*!
    \internal
*/
void QFxItem::transformChanged(const QTransform &)
{
}

QmlStateGroup *QFxItemPrivate::states()
{
    Q_Q(QFxItem);
    if (!_stateGroup) {
        _stateGroup = new QmlStateGroup(q);
        if (!_classComplete)
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

QPointF QFxItemPrivate::transformOrigin() const
{
    Q_Q(const QFxItem);

    QRectF br = q->boundingRect();

    switch(origin) {
    default:
    case QFxItem::TopLeft:
        return QPointF(0, 0);
    case QFxItem::TopCenter:
        return QPointF(br.width() / 2., 0);
    case QFxItem::TopRight:
        return QPointF(br.width(), 0);
    case QFxItem::MiddleLeft:
        return QPointF(0, br.height() / 2.);
    case QFxItem::Center:
        return QPointF(br.width() / 2., br.height() / 2.);
    case QFxItem::MiddleRight:
        return QPointF(br.width(), br.height() / 2.);
    case QFxItem::BottomLeft:
        return QPointF(0, br.height());
    case QFxItem::BottomCenter:
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
        if (options() & QFxItem::MouseFilter)
            d->gvRemoveMouseFilter();

        d->canvas = qvariant_cast<QGraphicsScene *>(value);
            
        if (options() & QFxItem::MouseFilter)
            d->gvAddMouseFilter();

        if (d->canvas && d->isFocusItemForArea)
            d->canvas->setFocusItem(this);
    } else if (change == ItemChildAddedChange ||
               change == ItemChildRemovedChange) {
        childrenChanged();
    }

    return QGraphicsItem::itemChange(change, value);
}


void QFxItem::mouseUngrabEvent()
{
}

void QFxItem::childrenChanged()
{
}

void QFxItem::setPaintMargin(qreal margin)
{
    Q_D(QFxItem);
    if (margin < d->paintmargin)
        update(); // schedule repaint of old boundingRect
    d->paintmargin = margin;
}

QRectF QFxItem::boundingRect() const
{
    Q_D(const QFxItem);
    return QRectF(-d->paintmargin, -d->paintmargin, d->width+d->paintmargin*2, d->height+d->paintmargin*2);
}

void QFxItem::paintContents(QPainter &)
{
}

void QFxItem::parentChanged(QFxItem *, QFxItem *)
{
    emit parentChanged();
}

/*!
    Returns the item's (0, 0) point relative to its parent.
 */
QPointF QFxItem::pos() const
{
    return QPointF(x(),y());
}

/*!
    Returns the item's (0, 0) point mapped to scene coordinates.
 */
QPointF QFxItem::scenePos() const
{
    return mapToScene(QPointF(0, 0));
}

/*!
    \enum QFxItem::TransformOrigin

    Controls the point about which simple transforms like scale apply.

    \value TopLeft The top-left corner of the item.
    \value TopCenter The center point of the top of the item.
    \value TopRight The top-right corner of the item.
    \value MiddleLeft The left most point of the vertical middle.
    \value Center The center of the item.
    \value MiddleRight The right most point of the vertical middle.
    \value BottomLeft The bottom-left corner of the item.
    \value BottomCenter The center point of the bottom of the item. 
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
        update();
    }
}

QPointF QFxItem::transformOriginPoint() const
{
    Q_D(const QFxItem);
    return d->transformOrigin();
}

qreal QFxItem::z() const
{
    Q_D(const QFxItem);
    return zValue();
}

void QFxItem::setX(qreal x)
{
    Q_D(QFxItem);
    if (x == this->x())
        return;

    qreal oldX = this->x();

    QGraphicsItem::setPos(x, y());

    geometryChanged(QRectF(this->x(), y(), width(), height()), 
                    QRectF(oldX, y(), width(), height()));
}

void QFxItem::setY(qreal y)
{
    if (y == this->y())
        return;

    qreal oldY = this->y();

    QGraphicsItem::setPos(x(), y);

    geometryChanged(QRectF(x(), this->y(), width(), height()), 
                    QRectF(x(), oldY, width(), height()));
}

void QFxItem::setZ(qreal z)
{
    if (z == this->z())
        return;

    if (z < 0)
        setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    else
        setFlag(QGraphicsItem::ItemStacksBehindParent, false);

    setZValue(z);
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

void QFxItem::setPos(const QPointF &point)
{
    qreal oldX = x();
    qreal oldY = y();

    QGraphicsItem::setPos(point);

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(oldX, oldY, width(), height()));
}

qreal QFxItem::scale() const
{
    Q_D(const QFxItem);
    return d->scale;
}

void QFxItem::setScale(qreal s)
{
    Q_D(QFxItem);
    if (d->scale == s)
        return;

    d->scale = s;
    setTransform(d->transform);

    emit scaleChanged();
}

QRect QFxItem::itemBoundingRect()
{
    return boundingRect().toAlignedRect();
}

QPointF QFxItem::mapFromScene(const QPointF &p) const
{
    return QGraphicsItem::mapFromScene(p);
}

QRectF QFxItem::mapFromScene(const QRectF &r) const
{
    return QGraphicsItem::mapFromScene(r).boundingRect();
}

QPointF QFxItem::mapToScene(const QPointF &p) const
{
    return QGraphicsItem::mapToScene(p);
}

QRectF QFxItem::mapToScene(const QRectF &r) const
{
    return QGraphicsItem::mapToScene(r).boundingRect();
}

QTransform QFxItem::transform() const
{
    Q_D(const QFxItem);
    return d->transform;
}

//### optimize (perhaps cache scale and rot transforms, and have dirty flags)
//### we rely on there not being an "if (d->transform == m) return;" check
void QFxItem::setTransform(const QTransform &m)
{
    Q_D(QFxItem);
    d->transform = m;
    QTransform scaleTransform, rotTransform;
    if (d->scale != 1) {
        QPointF to = transformOriginPoint();
        if (to.x() != 0. || to.y() != 0.)
            scaleTransform.translate(to.x(), to.y());
        scaleTransform.scale(d->scale, d->scale);
        if (to.x() != 0. || to.y() != 0.)
            scaleTransform.translate(-to.x(), -to.y());
    }
    if (d->_rotation != 0) {
        QPointF to = d->transformOrigin();
        if (to.x() != 0. || to.y() != 0.)
            rotTransform.translate(to.x(), to.y());
        rotTransform.rotate(d->_rotation);
        if (to.x() != 0. || to.y() != 0.)
            rotTransform.translate(-to.x(), -to.y());
    }
    QGraphicsItem::setTransform(scaleTransform * rotTransform * d->transform);
}

QFxItem *QFxItem::mouseGrabberItem() const
{
    QGraphicsScene *s = scene();
    if (s) {
        QGraphicsItem *item = s->mouseGrabberItem();
        return static_cast<QFxItem*>(item); // ###
    }
    return 0;
}

/*!
  \qmlproperty bool Item::focus
  This property indicates whether the item has has an active focus request. Set this
  property to true to request active focus.
*/

bool QFxItem::hasFocus() const
{
    Q_D(const QFxItem);
    return d->isFocusItemForArea;
}

void QFxItem::setFocus(bool focus)
{
    Q_D(QFxItem);
    QGraphicsScene *s = scene();
    if (s) {
        if (d->hasActiveFocus)
            s->setFocusItem(focus ? this : 0);
        else if (focus)
            s->setFocusItem(this);
        else {
            d->isFocusItemForArea = false;
            focusChanged(false);
        }

    } else {
        d->isFocusItemForArea = focus;
        focusChanged(focus);
    }
}

/*!
  \qmlproperty bool Item::activeFocus
  This property indicates whether the item has the active focus.
*/

bool QFxItem::hasActiveFocus() const
{
    Q_D(const QFxItem);
    return d->hasActiveFocus;
}

bool QFxItem::activeFocusPanel() const
{
    return false;
}

void QFxItem::setActiveFocusPanel(bool b)
{
    Q_UNUSED(b)
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

    if ((d->options & IsFocusPanel) && (d->options & IsFocusRealm)) {
        qWarning("QFxItem::setOptions: Cannot set both IsFocusPanel and IsFocusRealm.  IsFocusRealm will be unset.");
        d->options &= ~IsFocusRealm;
    }

    setFlag(QGraphicsItem::ItemHasNoContents, !(d->options & HasContents));
    setFiltersChildEvents(d->options & ChildMouseFilter);
    setFlag(QGraphicsItem::ItemAcceptsInputMethod, (d->options & AcceptsInputMethods));
    setAcceptHoverEvents(d->options & HoverEvents);
    d->isFocusRealm = static_cast<bool>(d->options & IsFocusRealm);

    if ((old & MouseFilter) != (d->options & MouseFilter)) {
        if (d->options & MouseFilter)
            d->gvAddMouseFilter();
        else
            d->gvRemoveMouseFilter();
    }
}

/*!
    \fn void QFxItem::setParent(QFxItem *parent)

    Sets the parent of the item to \a parent.
 */
void QFxItem::setParent(QFxItem *p)
{
    Q_D(QFxItem);
    if (p == parent() || !p) return;

    QObject::setParent(p);

    QFxItem *oldParent = itemParent();
    setParentItem(p);
    parentChanged(p, oldParent);
}

void QFxItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    paintContents(*p);
}

void QFxItemPrivate::gvRemoveMouseFilter()
{
    Q_Q(QFxItem);
    if (q->scene())
        q->removeSceneEventFilter(q);
}

void QFxItemPrivate::gvAddMouseFilter()
{
    Q_Q(QFxItem);
    if (q->scene())
        q->installSceneEventFilter(q);
}

QPixmap QFxItem::string(const QString &str, const QColor &c, const QFont &f)
{
    QFontMetrics fm(f);
    QSize size(fm.width(str), fm.height()*(str.count(QLatin1Char('\n'))+1)); //fm.boundingRect(str).size();
    QPixmap img(size);
    img.fill(Qt::transparent);
    QPainter p(&img);
    p.setPen(c);
    p.setFont(f);
    p.drawText(img.rect(), Qt::AlignVCenter, str);
    return img;
}

QT_END_NAMESPACE
