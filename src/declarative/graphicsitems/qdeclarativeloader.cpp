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

#include "private/qdeclarativeloader_p_p.h"

#include <qdeclarativeinfo.h>
#include <qdeclarativeengine_p.h>
#include <qdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QDeclarativeLoaderPrivate::QDeclarativeLoaderPrivate()
    : item(0), component(0), ownComponent(false)
    , resizeMode(QDeclarativeLoader::SizeLoaderToItem)
{
}

QDeclarativeLoaderPrivate::~QDeclarativeLoaderPrivate()
{
}

void QDeclarativeLoaderPrivate::itemGeometryChanged(QDeclarativeItem *resizeItem, const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (resizeItem == item && resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
        _q_updateSize();
    }
    QDeclarativeItemChangeListener::itemGeometryChanged(resizeItem, newGeometry, oldGeometry);
}

void QDeclarativeLoaderPrivate::clear()
{
    if (ownComponent) {
        component->deleteLater();
        component = 0;
        ownComponent = false;
    }
    source = QUrl();

    if (item) {
        if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(item)) {
            if (resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
                QDeclarativeItemPrivate *p =
                    static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
                p->removeItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
            }
        }

        // We can't delete immediately because our item may have triggered
        // the Loader to load a different item.
        item->setVisible(false);
        item->setParentItem(0);
        item->deleteLater();
        item = 0;
    }
}

void QDeclarativeLoaderPrivate::initResize()
{
    Q_Q(QDeclarativeLoader);
    if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(item)) {
        if (resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
            QDeclarativeItemPrivate *p =
                static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
            p->addItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
        }
    } else if (item && item->isWidget()) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget*>(item);
        if (resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
            widget->installEventFilter(q);
        }
    }
    _q_updateSize();
}

/*!
    \qmlclass Loader QDeclarativeLoader
    \since 4.7
    \inherits Item

    \brief The Loader item allows dynamically loading an Item-based
    subtree from a QML URL or Component.

    Loader instantiates an item from a component. The component to
    instantiate may be specified directly by the \c sourceComponent
    property, or loaded from a URL via the \c source property.

    It is also an effective means of delaying the creation of a component
    until it is required:
    \code
    Loader { id: pageLoader }
    Rectangle {
        MouseArea { anchors.fill: parent; onClicked: pageLoader.source = "Page1.qml" }
    }
    \endcode

    If the Loader source is changed, any previous items instantiated
    will be destroyed.  Setting \c source to an empty string
    will destroy the currently instantiated items, freeing resources
    and leaving the Loader empty.  For example:

    \code
    pageLoader.source = ""
    \endcode

    unloads "Page1.qml" and frees resources consumed by it.

    \sa {dynamic-object-creation}{Dynamic Object Creation}
*/

/*!
    \internal
    \class QDeclarativeLoader
    \qmlclass Loader
 */

/*!
    Create a new QDeclarativeLoader instance.
 */
QDeclarativeLoader::QDeclarativeLoader(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeLoaderPrivate), parent)
{
    Q_D(QDeclarativeItem);
    d->flags |= QGraphicsItem::ItemIsFocusScope;
}

/*!
    Destroy the loader instance.
 */
QDeclarativeLoader::~QDeclarativeLoader()
{
}

/*!
    \qmlproperty url Loader::source
    This property holds the URL of the QML component to
    instantiate.

    \sa sourceComponent, status, progress
*/
QUrl QDeclarativeLoader::source() const
{
    Q_D(const QDeclarativeLoader);
    return d->source;
}

void QDeclarativeLoader::setSource(const QUrl &url)
{
    Q_D(QDeclarativeLoader);
    if (d->source == url)
        return;

    d->clear();

    d->source = url;
    if (d->source.isEmpty()) {
        emit sourceChanged();
        emit statusChanged();
        emit progressChanged();
        emit itemChanged();
        return;
    }

    d->component = new QDeclarativeComponent(qmlEngine(this), d->source, this);
    d->ownComponent = true;
    if (!d->component->isLoading()) {
        d->_q_sourceLoaded();
    } else {
        connect(d->component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                this, SLOT(_q_sourceLoaded()));
        connect(d->component, SIGNAL(progressChanged(qreal)),
                this, SIGNAL(progressChanged()));
        emit statusChanged();
        emit progressChanged();
        emit sourceChanged();
        emit itemChanged();
    }
}

/*!
    \qmlproperty Component Loader::sourceComponent
    The sourceComponent property holds the \l{Component} to instantiate.

    \qml
    Item {
        Component {
            id: redSquare
            Rectangle { color: "red"; width: 10; height: 10 }
        }

        Loader { sourceComponent: redSquare }
        Loader { sourceComponent: redSquare; x: 10 }
    }
    \endqml

    \sa source, progress
*/

QDeclarativeComponent *QDeclarativeLoader::sourceComponent() const
{
    Q_D(const QDeclarativeLoader);
    return d->component;
}

void QDeclarativeLoader::setSourceComponent(QDeclarativeComponent *comp)
{
    Q_D(QDeclarativeLoader);
    if (comp == d->component)
        return;

    d->clear();

    d->component = comp;
    d->ownComponent = false;
    if (!d->component) {
        emit sourceChanged();
        emit statusChanged();
        emit progressChanged();
        emit itemChanged();
        return;
    }

    if (!d->component->isLoading()) {
        d->_q_sourceLoaded();
    } else {
        connect(d->component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                this, SLOT(_q_sourceLoaded()));
        connect(d->component, SIGNAL(progressChanged(qreal)),
                this, SIGNAL(progressChanged()));
        emit progressChanged();
        emit sourceChanged();
        emit statusChanged();
        emit itemChanged();
    }
}

void QDeclarativeLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QDeclarativeLoader);

    if (component) {
        if (!component->errors().isEmpty()) {
            qWarning() << component->errors();
            emit q->sourceChanged();
            emit q->statusChanged();
            emit q->progressChanged();
            return;
        }

        QDeclarativeContext *ctxt = new QDeclarativeContext(qmlContext(q));
        ctxt->setContextObject(q);

        QDeclarativeComponent *c = component;
        QObject *obj = component->create(ctxt);
        if (component != c) {
            // component->create could trigger a change in source that causes
            // component to be set to something else. In that case we just
            // need to cleanup.
            delete obj;
            delete ctxt;
            return;
        }
        if (obj) {
            item = qobject_cast<QGraphicsObject *>(obj);
            if (item) {
                QDeclarative_setParent_noEvent(ctxt, obj);
                QDeclarative_setParent_noEvent(item, q);
                item->setParentItem(q);
//                item->setFocus(true);
                initResize();
            } else {
                qmlInfo(q) << QDeclarativeLoader::tr("Loader does not support loading non-visual elements.");
                delete obj;
                delete ctxt;
            }
        } else {
            if (!component->errors().isEmpty())
                qWarning() << component->errors();
            delete obj;
            delete ctxt;
            source = QUrl();
        }
        emit q->sourceChanged();
        emit q->statusChanged();
        emit q->progressChanged();
        emit q->itemChanged();
    }
}

/*!
    \qmlproperty enum Loader::status

    This property holds the status of QML loading.  It can be one of:
    \list
    \o Null - no QML source has been set
    \o Ready - the QML source has been loaded
    \o Loading - the QML source is currently being loaded
    \o Error - an error occurred while loading the QML source
    \endlist

    Note that a change in the status property does not cause anything to happen
    (although it reflects what has happened to the loader internally). If you wish
    to react to the change in status you need to do it yourself, for example in one
    of the following ways:
    \list
    \o Create a state, so that a state change occurs, e.g. State{name: 'loaded'; when: loader.status = Loader.Ready;}
    \o Do something inside the onStatusChanged signal handler, e.g. Loader{id: loader; onStatusChanged: if(loader.status == Loader.Ready) console.log('Loaded');}
    \o Bind to the status variable somewhere, e.g. Text{text: if(loader.status!=Loader.Ready){'Not Loaded';}else{'Loaded';}}
    \endlist
    \sa progress
*/

QDeclarativeLoader::Status QDeclarativeLoader::status() const
{
    Q_D(const QDeclarativeLoader);

    if (d->component)
        return static_cast<QDeclarativeLoader::Status>(d->component->status());

    if (d->item)
        return Ready;

    return d->source.isEmpty() ? Null : Error;
}

/*!
\qmlproperty real Loader::progress

This property holds the progress of loading QML data from the network, from
0.0 (nothing loaded) to 1.0 (finished).  Most QML files are quite small, so
this value will rapidly change from 0 to 1.

\sa status
*/
qreal QDeclarativeLoader::progress() const
{
    Q_D(const QDeclarativeLoader);

    if (d->item)
        return 1.0;

    if (d->component)
        return d->component->progress();

    return 0.0;
}

/*!
    \qmlproperty enum Loader::resizeMode

    This property determines how the Loader or item are resized:
    \list
    \o NoResize - no item will be resized
    \o SizeLoaderToItem - the Loader will be sized to the size of the item, unless the size of the Loader has been otherwise specified.
    \o SizeItemToLoader - the item will be sized to the size of the Loader.
    \endlist

    Note that changing from SizeItemToLoader to SizeLoaderToItem
    after the component is loaded will not return the item or Loader
    to it's original size.  This is due to the item size being adjusted
    to the Loader size, thereby losing the original size of the item.
    Future changes to the item's size will affect the loader, however.

    The default resizeMode is SizeLoaderToItem.
*/
QDeclarativeLoader::ResizeMode QDeclarativeLoader::resizeMode() const
{
    Q_D(const QDeclarativeLoader);
    return d->resizeMode;
}

void QDeclarativeLoader::setResizeMode(ResizeMode mode)
{
    Q_D(QDeclarativeLoader);
    if (mode == d->resizeMode)
        return;

    if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(d->item)) {
        if (d->resizeMode == SizeLoaderToItem) {
            QDeclarativeItemPrivate *p =
                static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
            p->removeItemChangeListener(d, QDeclarativeItemPrivate::Geometry);
        }
    } else if (d->item && d->item->isWidget()) {
        if (d->resizeMode == SizeLoaderToItem)
            d->item->removeEventFilter(this);
    }

    d->resizeMode = mode;
    emit resizeModeChanged();
    d->initResize();
}

void QDeclarativeLoaderPrivate::_q_updateSize()
{
    Q_Q(QDeclarativeLoader);
    if (!item)
        return;
    if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(item)) {
        if (resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
            q->setWidth(qmlItem->width());
            q->setHeight(qmlItem->height());
        } else if (resizeMode == QDeclarativeLoader::SizeItemToLoader) {
            qmlItem->setWidth(q->width());
            qmlItem->setHeight(q->height());
        }
    } else if (item && item->isWidget()) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget*>(item);
        if (resizeMode == QDeclarativeLoader::SizeLoaderToItem) {
            QSizeF newSize = widget->size();
            if (newSize.isValid()) {
                q->setWidth(newSize.width());
                q->setHeight(newSize.height());
            }
        } else if (resizeMode == QDeclarativeLoader::SizeItemToLoader) {
            QSizeF oldSize = widget->size();
            QSizeF newSize = oldSize;
            if (q->heightValid())
                newSize.setHeight(q->height());
            if (q->widthValid())
                newSize.setWidth(q->width());
            if (oldSize != newSize)
                widget->resize(newSize);
        }
    }
}

/*!
    \qmlproperty Item Loader::item
    This property holds the top-level item created from source.
*/
QGraphicsObject *QDeclarativeLoader::item() const
{
    Q_D(const QDeclarativeLoader);
    return d->item;
}

void QDeclarativeLoader::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QDeclarativeLoader);
    if (newGeometry != oldGeometry) {
        if (d->resizeMode == SizeItemToLoader) {
            d->_q_updateSize();
        }
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

QVariant QDeclarativeLoader::itemChange(GraphicsItemChange change, const QVariant &value)
{
    Q_D(QDeclarativeLoader);
    if (change == ItemSceneHasChanged) {
        if (d->item && d->item->isWidget()) {
            if (d->resizeMode == SizeLoaderToItem) {
                d->item->removeEventFilter(this);
                d->item->installEventFilter(this);
            }
        }
    }
    return QDeclarativeItem::itemChange(change, value);
}

bool QDeclarativeLoader::eventFilter(QObject *watched, QEvent *e)
{
    Q_D(QDeclarativeLoader);
    if (watched == d->item && e->type() == QEvent::GraphicsSceneResize) {
        if (d->item && d->item->isWidget() && d->resizeMode == SizeLoaderToItem) {
            d->_q_updateSize();
       }
    }
    return QDeclarativeItem::eventFilter(watched, e);
}

#include <moc_qdeclarativeloader_p.cpp>

QT_END_NAMESPACE
