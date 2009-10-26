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

#include "qfxloader_p.h"
#include <private/qmlengine_p.h>

QT_BEGIN_NAMESPACE

QFxLoaderPrivate::QFxLoaderPrivate()
: item(0), component(0), ownComponent(false), resizeMode(QFxLoader::SizeLoaderToItem)
{
}

QFxLoaderPrivate::~QFxLoaderPrivate()
{
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Loader,QFxLoader)

/*!
    \qmlclass Loader
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
*/

/*!
    \internal
    \class QFxLoader
    \qmlclass Loader
 */

/*!
    Create a new QFxLoader instance.
 */
QFxLoader::QFxLoader(QFxItem *parent)
  : QFxItem(*(new QFxLoaderPrivate), parent)
{
}

/*!
    Destroy the loader instance.
 */
QFxLoader::~QFxLoader()
{
}

/*!
    \qmlproperty url Loader::source
    This property holds the URL of the QML component to
    instantiate.

    \sa status, progress
*/
QUrl QFxLoader::source() const
{
    Q_D(const QFxLoader);
    return d->source;
}

void QFxLoader::setSource(const QUrl &url)
{
    Q_D(QFxLoader);
    if (d->source == url)
        return;

    if (d->ownComponent) {
        delete d->component;
        d->component = 0;
    }
    delete d->item;
    d->item = 0;

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

    \sa source
*/

QmlComponent *QFxLoader::sourceComponent() const
{
    Q_D(const QFxLoader);
    return d->component;
}

void QFxLoader::setSourceComponent(QmlComponent *comp)
{
    Q_D(QFxLoader);
    if (comp == d->component)
        return;

    d->source = QUrl();
    if (d->ownComponent) {
        delete d->component;
        d->component = 0;
    }
    delete d->item;
    d->item = 0;

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

void QFxLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QFxLoader);

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
            item = qobject_cast<QFxItem *>(obj);
            if (item) {
                item->setParentItem(q);
//                item->setFocus(true);
                QFxItem *resizeItem = 0;
                if (resizeMode == QFxLoader::SizeLoaderToItem)
                    resizeItem = item;
                else if (resizeMode == QFxLoader::SizeItemToLoader)
                    resizeItem = q;
                if (resizeItem) {
                    QObject::connect(resizeItem, SIGNAL(widthChanged()), q, SLOT(_q_updateSize()));
                    QObject::connect(resizeItem, SIGNAL(heightChanged()), q, SLOT(_q_updateSize()));
                }
                _q_updateSize();
            }
        } else {
            delete obj;
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

QFxLoader::Status QFxLoader::status() const
{
    Q_D(const QFxLoader);

    if (d->component)
        return static_cast<QFxLoader::Status>(d->component->status());

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
qreal QFxLoader::progress() const
{
    Q_D(const QFxLoader);

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

    The default resizeMode is SizeLoaderToItem.
*/
QFxLoader::ResizeMode QFxLoader::resizeMode() const
{
    Q_D(const QFxLoader);
    return d->resizeMode;
}

void QFxLoader::setResizeMode(ResizeMode mode)
{
    Q_D(QFxLoader);
    if (mode == d->resizeMode)
        return;

    if (d->item) {
        QFxItem *resizeItem = 0;
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

    if (d->item) {
        QFxItem *resizeItem = 0;
        if (d->resizeMode == SizeLoaderToItem)
            resizeItem = d->item;
        else if (d->resizeMode == SizeItemToLoader)
            resizeItem = this;
        if (resizeItem) {
            connect(resizeItem, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
            connect(resizeItem, SIGNAL(heightChanged()), this, SLOT(_q_updateSize()));
        }
    }
}

void QFxLoaderPrivate::_q_updateSize()
{
    Q_Q(QFxLoader);
    if (!item)
        return;
    switch (resizeMode) {
    case QFxLoader::SizeLoaderToItem:
        q->setImplicitWidth(item->width());
        q->setImplicitHeight(item->height());
        break;
    case QFxLoader::SizeItemToLoader:
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
QFxItem *QFxLoader::item() const
{
    Q_D(const QFxLoader);
    return d->item;
}

QT_END_NAMESPACE

#include "moc_qfxloader.cpp"
