// Commit: 501180c6fbed0857126da2bb0ff1f17ee35472c6
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgloader_p_p.h"

#include <QtDeclarative/qdeclarativeinfo.h>

#include <private/qdeclarativeengine_p.h>
#include <private/qdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QSGLoaderPrivate::QSGLoaderPrivate()
    : item(0), component(0), ownComponent(false), updatingSize(false),
      itemWidthValid(false), itemHeightValid(false)
{
}

QSGLoaderPrivate::~QSGLoaderPrivate()
{
}

void QSGLoaderPrivate::itemGeometryChanged(QSGItem *resizeItem, const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (resizeItem == item) {
        if (!updatingSize && newGeometry.width() != oldGeometry.width())
            itemWidthValid = true;
        if (!updatingSize && newGeometry.height() != oldGeometry.height())
            itemHeightValid = true;
        _q_updateSize(false);
    }
    QSGItemChangeListener::itemGeometryChanged(resizeItem, newGeometry, oldGeometry);
}

void QSGLoaderPrivate::clear()
{
    if (ownComponent) {
        component->deleteLater();
        component = 0;
        ownComponent = false;
    }
    source = QUrl();

    if (item) {
        QSGItemPrivate *p = QSGItemPrivate::get(item);
        p->removeItemChangeListener(this, QSGItemPrivate::Geometry);

        // We can't delete immediately because our item may have triggered
        // the Loader to load a different item.
        item->setParentItem(0);
        item->setVisible(false);
        item->deleteLater();
        item = 0;
    }
}

void QSGLoaderPrivate::initResize()
{
    QSGItemPrivate *p = QSGItemPrivate::get(item);
    p->addItemChangeListener(this, QSGItemPrivate::Geometry);
    // We may override the item's size, so we need to remember
    // whether the item provided its own valid size.
    itemWidthValid = p->widthValid;
    itemHeightValid = p->heightValid;
    _q_updateSize();
}

QSGLoader::QSGLoader(QSGItem *parent)
  : QSGImplicitSizeItem(*(new QSGLoaderPrivate), parent)
{
    setFlag(ItemIsFocusScope);
}

QSGLoader::~QSGLoader()
{
    Q_D(QSGLoader);
    if (d->item) {
        QSGItemPrivate *p = QSGItemPrivate::get(d->item);
        p->removeItemChangeListener(d, QSGItemPrivate::Geometry);
    }
}

QUrl QSGLoader::source() const
{
    Q_D(const QSGLoader);
    return d->source;
}

void QSGLoader::setSource(const QUrl &url)
{
    Q_D(QSGLoader);
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

    if (isComponentComplete())
        d->load();
}

QDeclarativeComponent *QSGLoader::sourceComponent() const
{
    Q_D(const QSGLoader);
    return d->component;
}

void QSGLoader::setSourceComponent(QDeclarativeComponent *comp)
{
    Q_D(QSGLoader);
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

    if (isComponentComplete())
        d->load();
}

void QSGLoader::resetSourceComponent()
{
    setSourceComponent(0);
}

void QSGLoaderPrivate::load()
{
    Q_Q(QSGLoader);

    if (!q->isComponentComplete() || !component)
        return;

    if (!component->isLoading()) {
        _q_sourceLoaded();
    } else {
        QObject::connect(component, SIGNAL(statusChanged(QDeclarativeComponent::Status)),
                q, SLOT(_q_sourceLoaded()));
        QObject::connect(component, SIGNAL(progressChanged(qreal)),
                q, SIGNAL(progressChanged()));
        emit q->statusChanged();
        emit q->progressChanged();
        emit q->sourceChanged();
        emit q->itemChanged();
    }
}

void QSGLoaderPrivate::_q_sourceLoaded()
{
    Q_Q(QSGLoader);

    if (component) {
        if (!component->errors().isEmpty()) {
            QDeclarativeEnginePrivate::warning(qmlEngine(q), component->errors());
            emit q->sourceChanged();
            emit q->statusChanged();
            emit q->progressChanged();
            return;
        }

        QDeclarativeContext *creationContext = component->creationContext();
        if (!creationContext) creationContext = qmlContext(q);
        QDeclarativeContext *ctxt = new QDeclarativeContext(creationContext);
        ctxt->setContextObject(q);

        QDeclarativeGuard<QDeclarativeComponent> c = component;
        QObject *obj = component->beginCreate(ctxt);
        if (component != c) {
            // component->create could trigger a change in source that causes
            // component to be set to something else. In that case we just
            // need to cleanup.
            if (c)
                c->completeCreate();
            delete obj;
            delete ctxt;
            return;
        }
        if (obj) {
            item = qobject_cast<QSGItem *>(obj);
            if (item) {
                QDeclarative_setParent_noEvent(ctxt, obj);
                QDeclarative_setParent_noEvent(item, q);
                item->setParentItem(q);
//                item->setFocus(true);
                initResize();
            } else {
                qmlInfo(q) << QSGLoader::tr("Loader does not support loading non-visual elements.");
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
        component->completeCreate();
        emit q->sourceChanged();
        emit q->statusChanged();
        emit q->progressChanged();
        emit q->itemChanged();
        emit q->loaded();
    }
}

QSGLoader::Status QSGLoader::status() const
{
    Q_D(const QSGLoader);

    if (d->component)
        return static_cast<QSGLoader::Status>(d->component->status());

    if (d->item)
        return Ready;

    return d->source.isEmpty() ? Null : Error;
}

void QSGLoader::componentComplete()
{
    Q_D(QSGLoader);
    QSGItem::componentComplete();
    d->load();
}

qreal QSGLoader::progress() const
{
    Q_D(const QSGLoader);

    if (d->item)
        return 1.0;

    if (d->component)
        return d->component->progress();

    return 0.0;
}

void QSGLoaderPrivate::_q_updateSize(bool loaderGeometryChanged)
{
    Q_Q(QSGLoader);
    if (!item || updatingSize)
        return;

    updatingSize = true;

    if (!itemWidthValid)
        q->setImplicitWidth(item->implicitWidth());
    else
        q->setImplicitWidth(item->width());
    if (loaderGeometryChanged && q->widthValid())
        item->setWidth(q->width());

    if (!itemHeightValid)
        q->setImplicitHeight(item->implicitHeight());
    else
        q->setImplicitHeight(item->height());
    if (loaderGeometryChanged && q->heightValid())
        item->setHeight(q->height());

    updatingSize = false;
}

QSGItem *QSGLoader::item() const
{
    Q_D(const QSGLoader);
    return d->item;
}

void QSGLoader::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    Q_D(QSGLoader);
    if (newGeometry != oldGeometry) {
        d->_q_updateSize();
    }
    QSGItem::geometryChanged(newGeometry, oldGeometry);
}

#include <moc_qsgloader_p.cpp>

QT_END_NAMESPACE
