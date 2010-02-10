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

#include "qmlgraphicsloader_p_p.h"

#include <qmlengine_p.h>

QT_BEGIN_NAMESPACE

QmlGraphicsLoaderPrivate::QmlGraphicsLoaderPrivate()
    : item(0), component(0), ownComponent(false)
    , resizeMode(QmlGraphicsLoader::SizeLoaderToItem)
{
}

QmlGraphicsLoaderPrivate::~QmlGraphicsLoaderPrivate()
{
}

void QmlGraphicsLoaderPrivate::clear()
{
    if (ownComponent) {
        delete component;
        component = 0;
        ownComponent = false;
    }
    source = QUrl();

    if (item) {
        // We can't delete immediately because our item may have triggered
        // the Loader to load a different item.
        item->setVisible(false);
        static_cast<QGraphicsItem*>(item)->setParentItem(0);
        item->deleteLater();
        item = 0;
    }
}

void QmlGraphicsLoaderPrivate::initResize()
{
    Q_Q(QmlGraphicsLoader);

    QmlGraphicsItem *resizeItem = 0;
    if (resizeMode == QmlGraphicsLoader::SizeLoaderToItem)
        resizeItem = item;
    else if (resizeMode == QmlGraphicsLoader::SizeItemToLoader)
        resizeItem = q;
    if (resizeItem) {
        QObject::connect(resizeItem, SIGNAL(widthChanged()), q, SLOT(_q_updateSize()));
        QObject::connect(resizeItem, SIGNAL(heightChanged()), q, SLOT(_q_updateSize()));
    }
    _q_updateSize();
}


QML_DEFINE_TYPE(Qt,4,6,Loader,QmlGraphicsLoader)

/*!
    \qmlclass Loader QmlGraphicsLoader
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
        MouseRegion { anchors.fill: parent; onClicked: pageLoader.source = "Page1.qml" }
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
    \class QmlGraphicsLoader
    \qmlclass Loader
 */

/*!
    Create a new QmlGraphicsLoader instance.
 */
QmlGraphicsLoader::QmlGraphicsLoader(QmlGraphicsItem *parent)
  : QmlGraphicsItem(*(new QmlGraphicsLoaderPrivate), parent)
{
}

/*!
    Destroy the loader instance.
 */
QmlGraphicsLoader::~QmlGraphicsLoader()
{
}

/*!
    \qmlproperty url Loader::source
    This property holds the URL of the QML component to
    instantiate.

    \sa sourceComponent, status, progress
*/
QUrl QmlGraphicsLoader::source() const
{
    Q_D(const QmlGraphicsLoader);
    return d->source;
}

void QmlGraphicsLoader::setSource(const QUrl &url)
{
    Q_D(QmlGraphicsLoader);
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

    d->component = new QmlComponent(qmlEngine(this), d->source, this);
    d->ownComponent = true;
    if (!d->component->isLoading()) {
        d->_q_sourceLoaded();
    } else {
        connect(d->component, SIGNAL(statusChanged(QmlComponent::Status)),
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

QmlComponent *QmlGraphicsLoader::sourceComponent() const
{
    Q_D(const QmlGraphicsLoader);
    return d->component;
}

void QmlGraphicsLoader::setSourceComponent(QmlComponent *comp)
{
    Q_D(QmlGraphicsLoader);
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
        connect(d->component, SIGNAL(statusChanged(QmlComponent::Status)),
                this, SLOT(_q_sourceLoaded()));
        connect(d->component, SIGNAL(progressChanged(qreal)),
                this, SIGNAL(progressChanged()));
        emit progressChanged();
        emit sourceChanged();
        emit statusChanged();
        emit itemChanged();
    }
}

void QmlGraphicsLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QmlGraphicsLoader);

    if (component) {
        QmlContext *ctxt = new QmlContext(qmlContext(q));
        ctxt->addDefaultObject(q);

        if (!component->errors().isEmpty()) {
            qWarning() << component->errors();
            emit q->sourceChanged();
            emit q->statusChanged();
            emit q->progressChanged();
            return;
        }

        QObject *obj = component->create(ctxt);
        if (obj) {
            ctxt->setParent(obj);
            item = qobject_cast<QmlGraphicsItem *>(obj);
            if (item) {
                item->setParentItem(q);
//                item->setFocus(true);
                initResize();
            }
        } else {
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

    \sa progress
*/

QmlGraphicsLoader::Status QmlGraphicsLoader::status() const
{
    Q_D(const QmlGraphicsLoader);

    if (d->component)
        return static_cast<QmlGraphicsLoader::Status>(d->component->status());

    if (d->item)
        return Ready;

    return d->source.isEmpty() ? Null : Error;
}

/*!
    \qmlproperty real Loader::progress

    This property holds the progress of QML data loading, from 0.0 (nothing loaded)
    to 1.0 (finished).

    \sa status
*/
qreal QmlGraphicsLoader::progress() const
{
    Q_D(const QmlGraphicsLoader);

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
QmlGraphicsLoader::ResizeMode QmlGraphicsLoader::resizeMode() const
{
    Q_D(const QmlGraphicsLoader);
    return d->resizeMode;
}

void QmlGraphicsLoader::setResizeMode(ResizeMode mode)
{
    Q_D(QmlGraphicsLoader);
    if (mode == d->resizeMode)
        return;

    if (d->item) {
        QmlGraphicsItem *resizeItem = 0;
        if (d->resizeMode == SizeLoaderToItem)
            resizeItem = d->item;
        else if (d->resizeMode == SizeItemToLoader)
            resizeItem = this;
        if (resizeItem) {
            disconnect(resizeItem, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            disconnect(resizeItem, SIGNAL(heightChanged()), this, SLOT(_q_updateSize()));
        }
    }

    d->resizeMode = mode;
    d->initResize();
}

void QmlGraphicsLoaderPrivate::_q_updateSize()
{
    Q_Q(QmlGraphicsLoader);
    if (!item)
        return;
    switch (resizeMode) {
    case QmlGraphicsLoader::SizeLoaderToItem:
        q->setImplicitWidth(item->width());
        q->setImplicitHeight(item->height());
        break;
    case QmlGraphicsLoader::SizeItemToLoader:
        item->setWidth(q->width());
        item->setHeight(q->height());
        break;
    default:
        break;
    }
}

/*!
    \qmlproperty Item Loader::item
    This property holds the top-level item created from source.
*/
QmlGraphicsItem *QmlGraphicsLoader::item() const
{
    Q_D(const QmlGraphicsLoader);
    return d->item;
}

#include <moc_qmlgraphicsloader_p.cpp>

QT_END_NAMESPACE
