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
#include <qfxperf.h>

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
#include <qsimplecanvasfilter.h>
#include <qmlcomponent.h>


QT_BEGIN_NAMESPACE
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

QML_DEFINE_NOCREATE_TYPE(QFxContents)
QML_DEFINE_TYPE(QFxItem,Item)
QML_DEFINE_NOCREATE_TYPE(QSimpleCanvasFilter)

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

QFxContents::QFxContents() : _height(0), _width(0)
{
}

/*!
    \property QFxContents::height
    \brief The height of the contents.
*/
int QFxContents::height() const
{
    return _height;
}

/*!
    \property QFxContents::width
    \brief The width of the contents.
*/
int QFxContents::width() const
{
    return _width;
}

//TODO: optimization: only check sender(), if there is one
void QFxContents::calcHeight()
{
    int oldheight = _height;

    int top = INT_MAX;
    int bottom = 0;

    const QList<QSimpleCanvasItem *> &children = _item->QSimpleCanvasItem::children();
    for (int i = 0; i < children.count(); ++i) {
        const QSimpleCanvasItem *child = children.at(i);
        int y = int(child->y());
        if (y + child->height() > bottom)
            bottom = y + child->height();
        if (y < top)
            top = y;
    }
    _height = bottom - top;

    if (_height != oldheight)
        emit heightChanged();
}

//TODO: optimization: only check sender(), if there is one
void QFxContents::calcWidth()
{
    int oldwidth = _width;

    int left = INT_MAX;
    int right = 0;
    const QList<QSimpleCanvasItem *> &children = _item->QSimpleCanvasItem::children();
    for (int i = 0; i < children.count(); ++i) {
        const QSimpleCanvasItem *child = children.at(i);
        int x = int(child->x());
        if (x + child->width() > right)
            right = x + child->width();
        if (x < left)
            left = x;
    }
    _width = right - left;

    if (_width != oldwidth)
        emit widthChanged();
}

void QFxContents::setItem(QFxItem *item)
{
    _item = item;

    const QList<QSimpleCanvasItem *> &children = _item->QSimpleCanvasItem::children();
    for (int i = 0; i < children.count(); ++i) {
        const QSimpleCanvasItem *child = children.at(i);
        connect(child, SIGNAL(heightChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(topChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(widthChanged()), this, SLOT(calcWidth()));
        connect(child, SIGNAL(leftChanged()), this, SLOT(calcWidth()));
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
  \property QFxItem::activeFocus
  This property indicates whether the item has the active focus.
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
    \fn void QFxItem::leftChanged()

    This signal is emitted when the left coordinate of the item changes.
*/

/*!
    \fn void QFxItem::topChanged()

    This signal is emitted when the top coordinate of the item changes.
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

    \sa QSimpleCanvasItem::setFocus()
*/

/*!
    \fn QFxItem::QFxItem(QFxItem *parent)

    Constructs a QFxItem with the given \a parent.
*/
QFxItem::QFxItem(QFxItem* parent)
  : QSimpleCanvasItem(*(new QFxItemPrivate), parent)
{
    Q_D(QFxItem);
    d->init(parent);
}

/*! \internal
*/
QFxItem::QFxItem(QFxItemPrivate &dd, QFxItem *parent)
  : QSimpleCanvasItem(dd, parent)
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
    return qobject_cast<QFxItem *>(QObject::parent());
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
    i->setParent(q);
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
  \qmlproperty int Item::width
  \qmlproperty int Item::height

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
        emit leftChanged();
    if (newGeometry.width() != oldGeometry.width())
        emit widthChanged();
    if (newGeometry.y() != oldGeometry.y()) 
        emit topChanged();
    if (newGeometry.height() != oldGeometry.height())
        emit heightChanged();

    for(int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->update(this, newGeometry, oldGeometry);
    }
}

/*!
  \qmlproperty bool Item::flipVertically
  \qmlproperty bool Item::flipHorizontally

  When set, the item will be displayed flipped horizontally or vertically
  about its center.
 */

/*!
  \property QFxItem::flipVertically

  When set, the item will be displayed flipped horizontally or vertically
  about its center.
 */
bool QFxItem::flipVertically() const
{
    return flip() & VerticalFlip;
}

void QFxItem::setFlipVertically(bool v)
{
    if (v)
        setFlip((QSimpleCanvasItem::Flip)(flip() | VerticalFlip));
    else
        setFlip((QSimpleCanvasItem::Flip)(flip() & ~VerticalFlip));
}

/*!
  \property QFxItem::flipHorizontally

  When set, the item will be displayed flipped horizontally or vertically
  about its center.
 */
bool QFxItem::flipHorizontally() const
{
    return flip() & HorizontalFlip;
}

void QFxItem::setFlipHorizontally(bool v)
{
    if (v)
        setFlip((QSimpleCanvasItem::Flip)(flip() | HorizontalFlip));
    else
        setFlip((QSimpleCanvasItem::Flip)(flip() & ~HorizontalFlip));
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

  \qmlproperty int Item::anchors.topMargin
  \qmlproperty int Item::anchors.bottomMargin
  \qmlproperty int Item::anchors.leftMargin
  \qmlproperty int Item::anchors.rightMargin
  \qmlproperty int Item::anchors.horizontalCenterOffset
  \qmlproperty int Item::anchors.verticalCenterOffset
  
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
int QFxItem::baselineOffset() const
{
    Q_D(const QFxItem);
    if (!d->_baselineOffset.isValid()) {
        return 0;
    } else
        return d->_baselineOffset;
}

/*!
    \internal
*/
void QFxItem::setBaselineOffset(int offset)
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
#if defined(QFX_RENDER_OPENGL)
    QMatrix4x4 trans;
    QPointF to = transformOriginPoint();
    trans.translate(to.x(), to.y());
    trans.rotate(d->_rotation, 0, 0, 1);
    trans.translate(-to.x(), -to.y());
#else
    QPointF to = d->transformOrigin();
    QTransform trans = QTransform::fromTranslate(to.x(), to.y());
    trans.rotate(d->_rotation);
    trans.translate(-to.x(), -to.y());
#endif
    setTransform(trans);
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
qreal QFxItem::scale() const
{
    return QSimpleCanvasItem::scale();
}

void QFxItem::setScale(qreal s)
{
    if (QSimpleCanvasItem::scale() == s) return;
    QSimpleCanvasItem::setScale(s);
    emit scaleChanged();
    update();
}

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

qreal QFxItem::opacity() const
{
    return QSimpleCanvasItem::visible();
}

void QFxItem::setOpacity(qreal v)
{
    if (v == QSimpleCanvasItem::visible())
        return;

    if (v < 0) v = 0;
    else if (v > 1) v = 1;
    QSimpleCanvasItem::setVisible(v);

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
  \property QFxItem::focus
  This property holds the item's focus state.
*/

/*!
  \property QFxItem::focusable
  This property holds whether the item has focus state.
*/

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
void QFxItem::dump(int depth)
{
    QByteArray ba(depth * 4, ' ');
    qWarning() << ba.constData() << metaObject()->className() << "(" << (void *)static_cast<QFxItem*>(this) << ", " << (void *)static_cast<QSimpleCanvasItem*>(this) << "):" << x() << y() << width() << height() << (void *) itemParent();
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

/*! \internal
*/
void QFxItem::parentChanged(QSimpleCanvasItem *, QSimpleCanvasItem *)
{
    emit parentChanged();
}

/*! \internal
*/
void QFxItem::reparentItems()
{
    qFatal("EEK");
}

/*!
    \internal
*/
void QFxItem::updateTransform()
{
    Q_D(QFxItem);
    QSimpleCanvas::Matrix trans;
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
void QFxItem::transformChanged(const QSimpleCanvas::Matrix &)
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

QT_END_NAMESPACE
