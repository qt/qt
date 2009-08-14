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

#include "qfximagebase.h"
#include "qfximagebase_p.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qfxpixmapcache.h>

QT_BEGIN_NAMESPACE

QFxImageBase::QFxImageBase(QFxItem *parent)
  : QFxItem(*(new QFxImageBasePrivate), parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
}

QFxImageBase::QFxImageBase(QFxImageBasePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
}

QFxImageBase::~QFxImageBase()
{
    Q_D(QFxImageBase);
    if (!d->url.isEmpty())
        QFxPixmapCache::cancelGet(d->url, this);
}

QFxImageBase::Status QFxImageBase::status() const
{
    Q_D(const QFxImageBase);
    return d->status;
}


qreal QFxImageBase::progress() const
{
    Q_D(const QFxImageBase);
    return d->progress;
}


/*!
    \property QFxImage::source
    \brief the url of the image to be displayed in this item.

    The content specified can be of any image type loadable by QImage. Alternatively,
    you can specify an sci format file, which specifies both an image and it's scale grid.
*/
QUrl QFxImageBase::source() const
{
    Q_D(const QFxImageBase);
    return d->url;
}

void QFxImageBase::setSource(const QUrl &url)
{
    Q_D(QFxImageBase);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    if (!d->url.isEmpty())
        QFxPixmapCache::cancelGet(d->url, this);

    d->url = url;
    if (d->progress != 0.0) {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (url.isEmpty()) {
        d->pix = QPixmap();
        d->status = Null;
        d->progress = 1.0;
        setImplicitWidth(0);
        setImplicitHeight(0);
        emit statusChanged(d->status);
        emit sourceChanged(d->url);
        emit progressChanged(1.0);
        update();
    } else {
        d->status = Loading;
            d->reply = QFxPixmapCache::get(qmlEngine(this), d->url, &d->pix);
            if (d->reply) {
                connect(d->reply, SIGNAL(finished()), this, SLOT(requestFinished()));
                connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                        this, SLOT(requestProgress(qint64,qint64)));
            } else {
                //### should be unified with requestFinished
                setImplicitWidth(d->pix.width());
                setImplicitHeight(d->pix.height());

                if (d->status == Loading)
                    d->status = Ready;
                d->progress = 1.0;
                emit statusChanged(d->status);
                emit sourceChanged(d->url);
                emit progressChanged(1.0);
                update();
            }
        }

    emit statusChanged(d->status);
}

void QFxImageBase::requestFinished()
{
    Q_D(QFxImageBase);
        if (d->reply) {
            //###disconnect really needed?
            disconnect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                       this, SLOT(requestProgress(qint64,qint64)));
            if (d->reply->error() != QNetworkReply::NoError)
                d->status = Error;
        }
        QFxPixmapCache::find(d->url, &d->pix);
    setImplicitWidth(d->pix.width());
    setImplicitHeight(d->pix.height());

    if (d->status == Loading)
        d->status = Ready;
    d->progress = 1.0;
    emit statusChanged(d->status);
    emit sourceChanged(d->url);
    emit progressChanged(1.0);
    update();
}

void QFxImageBase::requestProgress(qint64 received, qint64 total)
{
    Q_D(QFxImageBase);
    if (d->status == Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}


QT_END_NAMESPACE
