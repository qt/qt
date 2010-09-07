/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativeimagebase_p.h"
#include "private/qdeclarativeimagebase_p_p.h"

#include <qdeclarativeengine.h>
#include <qdeclarativeinfo.h>
#include <qdeclarativepixmapcache_p.h>

#include <QFile>

QT_BEGIN_NAMESPACE


/*!
    \class QDeclarativeImageBase
    \internal
    \brief The base class for declarative images.
 */

QDeclarativeImageBase::QDeclarativeImageBase(QDeclarativeImageBasePrivate &dd, QDeclarativeItem *parent)
  : QDeclarativeItem(dd, parent)
{
}

QDeclarativeImageBase::~QDeclarativeImageBase()
{
}

QDeclarativeImageBase::Status QDeclarativeImageBase::status() const
{
    Q_D(const QDeclarativeImageBase);
    return d->status;
}


qreal QDeclarativeImageBase::progress() const
{
    Q_D(const QDeclarativeImageBase);
    return d->progress;
}


bool QDeclarativeImageBase::asynchronous() const
{
    Q_D(const QDeclarativeImageBase);
    return d->async;
}

void QDeclarativeImageBase::setAsynchronous(bool async)
{
    Q_D(QDeclarativeImageBase);
    if (d->async != async) {
        d->async = async;
        emit asynchronousChanged();
    }
}

QUrl QDeclarativeImageBase::source() const
{
    Q_D(const QDeclarativeImageBase);
    return d->url;
}

void QDeclarativeImageBase::setSource(const QUrl &url)
{
    Q_D(QDeclarativeImageBase);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    d->url = url;
    emit sourceChanged(d->url);

    if (isComponentComplete())
        load();
}

void QDeclarativeImageBase::setSourceSize(const QSize& size)
{
    Q_D(QDeclarativeImageBase);
    if (d->sourcesize == size)
        return;

    d->sourcesize = size;
    emit sourceSizeChanged();
    if (isComponentComplete())
        load();
}

QSize QDeclarativeImageBase::sourceSize() const
{
    Q_D(const QDeclarativeImageBase);
    return d->sourcesize.isValid() ? d->sourcesize : QSize(implicitWidth(),implicitHeight());
}

void QDeclarativeImageBase::load()
{
    Q_D(QDeclarativeImageBase);
    if (d->progress != 0.0) {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (d->url.isEmpty()) {
        d->pix.clear();
        d->status = Null;
        setImplicitWidth(0);
        setImplicitHeight(0);
        emit statusChanged(d->status);
        pixmapChange();
        update();
    } else {
        d->status = Loading;

        d->pix.load(qmlEngine(this), d->url, d->sourcesize, d->async);

        if (d->pix.isLoading()) {

            static int thisRequestProgress = -1;
            static int thisRequestFinished = -1;
            if (thisRequestProgress == -1) {
                thisRequestProgress =
                    QDeclarativeImageBase::staticMetaObject.indexOfSlot("requestProgress(qint64,qint64)");
                thisRequestFinished =
                    QDeclarativeImageBase::staticMetaObject.indexOfSlot("requestFinished()");
            }

            d->pix.connectFinished(this, thisRequestFinished);
            d->pix.connectDownloadProgress(this, thisRequestProgress);

        } else {
            QSize impsize = d->pix.implicitSize();
            setImplicitWidth(impsize.width());
            setImplicitHeight(impsize.height());

            if (d->pix.isReady()) {
                d->status = Ready;

                if (!d->sourcesize.isValid())
                    emit sourceSizeChanged();

            } else {
                d->status = Error;
                qmlInfo(this) << d->pix.error();
            }
            d->progress = 1.0;
            emit statusChanged(d->status);
            emit progressChanged(d->progress);
            pixmapChange();
            update();
        }

    }

    emit statusChanged(d->status);
}

void QDeclarativeImageBase::requestFinished()
{
    Q_D(QDeclarativeImageBase);

    QSize impsize = d->pix.implicitSize();

    if (d->pix.isError()) {
        d->status = Error;
        qmlInfo(this) << d->pix.error();
    }

    setImplicitWidth(impsize.width());
    setImplicitHeight(impsize.height());

    if (d->status == Loading)
        d->status = Ready;
    d->progress = 1.0;
    emit statusChanged(d->status);
    emit progressChanged(1.0);
    if (!d->sourcesize.isValid())
        emit sourceSizeChanged();
    pixmapChange();
    update();
}

void QDeclarativeImageBase::requestProgress(qint64 received, qint64 total)
{
    Q_D(QDeclarativeImageBase);
    if (d->status == Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}

void QDeclarativeImageBase::componentComplete()
{
    Q_D(QDeclarativeImageBase);
    QDeclarativeItem::componentComplete();
    if (d->url.isValid())
        load();
}

void QDeclarativeImageBase::pixmapChange()
{
}

QT_END_NAMESPACE
