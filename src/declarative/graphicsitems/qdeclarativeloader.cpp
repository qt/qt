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
{
}

QDeclarativeLoaderPrivate::~QDeclarativeLoaderPrivate()
{
}

void QDeclarativeLoaderPrivate::itemGeometryChanged(QDeclarativeItem *resizeItem, const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (resizeItem == item)
        _q_updateSize(false);
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
            QDeclarativeItemPrivate *p =
                    static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
            p->removeItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
        }

        // We can't delete immediately because our item may have triggered
        // the Loader to load a different item.
        if (item->scene()) {
            item->scene()->removeItem(item);
        } else {
            item->setParentItem(0);
            item->setVisible(false);
        }
        item->deleteLater();
        item = 0;
    }
}

void QDeclarativeLoaderPrivate::initResize()
{
    Q_Q(QDeclarativeLoader);
    if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(item)) {
        QDeclarativeItemPrivate *p =
                static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
        p->addItemChangeListener(this, QDeclarativeItemPrivate::Geometry);
    } else if (item && item->isWidget()) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget*>(item);
        widget->installEventFilter(q);
    }
    _q_updateSize();
}

/*!
    \qmlclass Loader QDeclarativeLoader
    \since 4.7
    \inherits Item

    \brief The Loader item allows dynamically loading an Item-based
    subtree from a URL or Component.

    The Loader element instantiates an item from a component. The component to
    be instantiated may be specified directly by the \l sourceComponent
    property, or loaded from a URL via the \l source property.

    Loader can be used to delay the creation of a component until it
    is required.  For example, this loads "Page1.qml" as a component
    into the Loader element, when the \l MouseArea is clicked:

    \code
    import Qt 4.7

    Item {
        width: 200; height: 200

        MouseArea { 
            anchors.fill: parent
            onClicked: pageLoader.source = "Page1.qml"
        }

        Loader { id: pageLoader }
    }
    \endcode

    Note that Loader is like any other graphical Item and needs to be positioned 
    and sized accordingly to become visible. When a component is loaded, the 
    Loader is automatically resized to the size of the component.

    If the Loader source is changed, any previous items instantiated
    will be destroyed.  Setting \l source to an empty string, or setting
    sourceComponent to \e undefined
    will destroy the currently instantiated items, freeing resources
    and leaving the Loader empty.  For example:

    \code
    pageLoader.source = ""
    \endcode

      or

    \code
    pageLoader.sourceComponent = undefined
    \endcode

    unloads "Page1.qml" and frees resources consumed by it.

    Note that Loader is a focus scope. Its \c focus property must be set to \c true for any of its children
    to get the \e {active focus} (see \l{qmlfocus#Acquiring Focus and Focus Scopes}{the focus documentation page} for more details).

    \sa {dynamic-object-creation}{Dynamic Object Creation}
*/

/*!
    \internal
    \class QDeclarativeLoader
 */

/*!
    Create a new QDeclarativeLoader instance.
 */
QDeclarativeLoader::QDeclarativeLoader(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeLoaderPrivate), parent)
{
    Q_D(QDeclarativeLoader);
    d->flags |= QGraphicsItem::ItemIsFocusScope;
}

/*!
    Destroy the loader instance.
 */
QDeclarativeLoader::~QDeclarativeLoader()
{
    Q_D(QDeclarativeLoader);
    if (d->item) {
        if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(d->item)) {
            QDeclarativeItemPrivate *p =
                    static_cast<QDeclarativeItemPrivate *>(QGraphicsItemPrivate::get(qmlItem));
            p->removeItemChangeListener(d, QDeclarativeItemPrivate::Geometry);
        }
    }
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
    This property holds the \l{Component} to instantiate.

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

    Note this value must hold a \l Component object; it cannot be a \l Item.

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

void QDeclarativeLoader::resetSourceComponent()
{
    setSourceComponent(0);
}

void QDeclarativeLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QDeclarativeLoader);

    if (component) {
        if (!component->errors().isEmpty()) {
            QDeclarativeEnginePrivate::warning(qmlEngine(q), component->errors());
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
                QDeclarativeEnginePrivate::warning(qmlEngine(q), component->errors());
            delete obj;
            delete ctxt;
            source = QUrl();
        }
        emit q->sourceChanged();
        emit q->statusChanged();
        emit q->progressChanged();
        emit q->itemChanged();
        emit q->loaded();
    }
}

/*!
    \qmlproperty enumeration Loader::status

    This property holds the status of QML loading.  It can be one of:
    \list
    \o Loader.Null - no QML source has been set
    \o Loader.Ready - the QML source has been loaded
    \o Loader.Loading - the QML source is currently being loaded
    \o Loader.Error - an error occurred while loading the QML source
    \endlist

    Use this status to provide an update or respond to the status change in some way.
    For example, you could:

    \e {Trigger a state change:}
    \qml 
        State { name: 'loaded'; when: loader.status = Loader.Ready }
    \endqml

    \e {Implement an \c onStatusChanged signal handler:}
    \qml 
        Loader {
            id: loader
            onStatusChanged: if (loader.status == Loader.Ready) console.log('Loaded')
        }
    \endqml

    \e {Bind to the status value:}
    \qml
        Text { text: loader.status != Loader.Ready ? 'Not Loaded' : 'Loaded' }
    \endqml

    Note that if the source is a local file, the status will initially be Ready (or Error). While
    there will be no onStatusChanged signal in that case, the onLoaded will still be invoked.

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

void QDeclarativeLoader::componentComplete()
{
    QDeclarativeItem::componentComplete();
    if (status() == Ready)
        emit loaded();
}


/*!
    \qmlsignal Loader::onLoaded()

    This handler is called when the \l status becomes \c Loader.Ready, or on successful
    initial load.
*/


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

void QDeclarativeLoaderPrivate::_q_updateSize(bool loaderGeometryChanged)
{
    Q_Q(QDeclarativeLoader);
    if (!item)
        return;
    if (QDeclarativeItem *qmlItem = qobject_cast<QDeclarativeItem*>(item)) {
        q->setImplicitWidth(qmlItem->width());
        if (loaderGeometryChanged && q->widthValid())
            qmlItem->setWidth(q->width());
        q->setImplicitHeight(qmlItem->height());
        if (loaderGeometryChanged && q->heightValid())
            qmlItem->setHeight(q->height());
    } else if (item && item->isWidget()) {
        QGraphicsWidget *widget = static_cast<QGraphicsWidget*>(item);
        QSizeF widgetSize = widget->size();
        q->setImplicitWidth(widgetSize.width());
        if (loaderGeometryChanged && q->widthValid())
            widgetSize.setWidth(q->width());
        q->setImplicitHeight(widgetSize.height());
        if (loaderGeometryChanged && q->heightValid())
            widgetSize.setHeight(q->height());
        if (widget->size() != widgetSize)
            widget->resize(widgetSize);
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
        d->_q_updateSize();
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

QVariant QDeclarativeLoader::itemChange(GraphicsItemChange change, const QVariant &value)
{
    Q_D(QDeclarativeLoader);
    if (change == ItemSceneHasChanged) {
        if (d->item && d->item->isWidget()) {
            d->item->removeEventFilter(this);
            d->item->installEventFilter(this);
        }
    }
    return QDeclarativeItem::itemChange(change, value);
}

bool QDeclarativeLoader::eventFilter(QObject *watched, QEvent *e)
{
    Q_D(QDeclarativeLoader);
    if (watched == d->item && e->type() == QEvent::GraphicsSceneResize) {
        if (d->item && d->item->isWidget())
            d->_q_updateSize(false);
    }
    return QDeclarativeItem::eventFilter(watched, e);
}

#include <moc_qdeclarativeloader_p.cpp>

QT_END_NAMESPACE
