// Commit: 462429f5692f810bdd4e04b916db5f9af428d9e4
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

#include "qsgborderimage_p.h"
#include "qsgborderimage_p_p.h"
#include "qsgninepatchnode_p.h"

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtCore/qfile.h>

#include <private/qdeclarativeengine_p.h>

QT_BEGIN_NAMESPACE

QSGBorderImage::QSGBorderImage(QSGItem *parent)
: QSGImageBase(*(new QSGBorderImagePrivate), parent)
{
}

QSGBorderImage::~QSGBorderImage()
{
    Q_D(QSGBorderImage);
    if (d->sciReply)
        d->sciReply->deleteLater();
}

void QSGBorderImage::setSource(const QUrl &url)
{
    Q_D(QSGBorderImage);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    if (d->sciReply) {
        d->sciReply->deleteLater();
        d->sciReply = 0;
    }

    d->url = url;
    d->sciurl = QUrl();
    emit sourceChanged(d->url);

    if (isComponentComplete())
        load();
}

void QSGBorderImage::load()
{
    Q_D(QSGBorderImage);
    if (d->progress != 0.0) {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (d->url.isEmpty()) {
        d->pix.clear(this);
        d->status = Null;
        setImplicitWidth(0);
        setImplicitHeight(0);
        emit statusChanged(d->status);
        update();
    } else {
        d->status = Loading;
        if (d->url.path().endsWith(QLatin1String("sci"))) {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
            QString lf = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(d->url);
            if (!lf.isEmpty()) {
                QFile file(lf);
                file.open(QIODevice::ReadOnly);
                setGridScaledImage(QSGGridScaledImage(&file));
            } else
#endif
            {
                QNetworkRequest req(d->url);
                d->sciReply = qmlEngine(this)->networkAccessManager()->get(req);

                static int sciReplyFinished = -1;
                static int thisSciRequestFinished = -1;
                if (sciReplyFinished == -1) {
                    sciReplyFinished =
                        QNetworkReply::staticMetaObject.indexOfSignal("finished()");
                    thisSciRequestFinished =
                        QSGBorderImage::staticMetaObject.indexOfSlot("sciRequestFinished()");
                }

                QMetaObject::connect(d->sciReply, sciReplyFinished, this,
                                     thisSciRequestFinished, Qt::DirectConnection);
            }
        } else {

            QDeclarativePixmap::Options options;
            if (d->async)
                options |= QDeclarativePixmap::Asynchronous;
            if (d->cache)
                options |= QDeclarativePixmap::Cache;
            d->pix.clear(this);
            d->pix.load(qmlEngine(this), d->url, options);

            if (d->pix.isLoading()) {
                d->pix.connectFinished(this, SLOT(requestFinished()));
                d->pix.connectDownloadProgress(this, SLOT(requestProgress(qint64,qint64)));
            } else {
                QSize impsize = d->pix.implicitSize();
                setImplicitWidth(impsize.width());
                setImplicitHeight(impsize.height());

                if (d->pix.isReady()) {
                    d->status = Ready;
                } else {
                    d->status = Error;
                    qmlInfo(this) << d->pix.error();
                }

                d->progress = 1.0;
                emit statusChanged(d->status);
                emit progressChanged(d->progress);
                update();
            }
        }
    }

    emit statusChanged(d->status);
}

QSGScaleGrid *QSGBorderImage::border()
{
    Q_D(QSGBorderImage);
    return d->getScaleGrid();
}

QSGBorderImage::TileMode QSGBorderImage::horizontalTileMode() const
{
    Q_D(const QSGBorderImage);
    return d->horizontalTileMode;
}

void QSGBorderImage::setHorizontalTileMode(TileMode t)
{
    Q_D(QSGBorderImage);
    if (t != d->horizontalTileMode) {
        d->horizontalTileMode = t;
        emit horizontalTileModeChanged();
        update();
    }
}

QSGBorderImage::TileMode QSGBorderImage::verticalTileMode() const
{
    Q_D(const QSGBorderImage);
    return d->verticalTileMode;
}

void QSGBorderImage::setVerticalTileMode(TileMode t)
{
    Q_D(QSGBorderImage);
    if (t != d->verticalTileMode) {
        d->verticalTileMode = t;
        emit verticalTileModeChanged();
        update();
    }
}

void QSGBorderImage::setGridScaledImage(const QSGGridScaledImage& sci)
{
    Q_D(QSGBorderImage);
    if (!sci.isValid()) {
        d->status = Error;
        emit statusChanged(d->status);
    } else {
        QSGScaleGrid *sg = border();
        sg->setTop(sci.gridTop());
        sg->setBottom(sci.gridBottom());
        sg->setLeft(sci.gridLeft());
        sg->setRight(sci.gridRight());
        d->horizontalTileMode = sci.horizontalTileRule();
        d->verticalTileMode = sci.verticalTileRule();

        d->sciurl = d->url.resolved(QUrl(sci.pixmapUrl()));

        QDeclarativePixmap::Options options;
        if (d->async)
            options |= QDeclarativePixmap::Asynchronous;
        if (d->cache)
            options |= QDeclarativePixmap::Cache;
        d->pix.clear(this);
        d->pix.load(qmlEngine(this), d->sciurl, options);

        if (d->pix.isLoading()) {
            static int thisRequestProgress = -1;
            static int thisRequestFinished = -1;
            if (thisRequestProgress == -1) {
                thisRequestProgress =
                    QSGBorderImage::staticMetaObject.indexOfSlot("requestProgress(qint64,qint64)");
                thisRequestFinished =
                    QSGBorderImage::staticMetaObject.indexOfSlot("requestFinished()");
            }

            d->pix.connectFinished(this, thisRequestFinished);
            d->pix.connectDownloadProgress(this, thisRequestProgress);

        } else {

            QSize impsize = d->pix.implicitSize();
            setImplicitWidth(impsize.width());
            setImplicitHeight(impsize.height());

            if (d->pix.isReady()) {
                d->status = Ready;
            } else {
                d->status = Error;
                qmlInfo(this) << d->pix.error();
            }

            d->progress = 1.0;
            emit statusChanged(d->status);
            emit progressChanged(1.0);
            update();

        }
    }
}

void QSGBorderImage::requestFinished()
{
    Q_D(QSGBorderImage);

    QSize impsize = d->pix.implicitSize();
    if (d->pix.isError()) {
        d->status = Error;
        qmlInfo(this) << d->pix.error();
    } else {
        d->status = Ready;
    }

    setImplicitWidth(impsize.width());
    setImplicitHeight(impsize.height());

    if (d->sourcesize.width() != d->pix.width() || d->sourcesize.height() != d->pix.height())
        emit sourceSizeChanged();

    d->progress = 1.0;
    emit statusChanged(d->status);
    emit progressChanged(1.0);
    update();
}

#define BORDERIMAGE_MAX_REDIRECT 16

void QSGBorderImage::sciRequestFinished()
{
    Q_D(QSGBorderImage);

    d->redirectCount++;
    if (d->redirectCount < BORDERIMAGE_MAX_REDIRECT) {
        QVariant redirect = d->sciReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (redirect.isValid()) {
            QUrl url = d->sciReply->url().resolved(redirect.toUrl());
            setSource(url);
            return;
        }
    }
    d->redirectCount=0;

    if (d->sciReply->error() != QNetworkReply::NoError) {
        d->status = Error;
        d->sciReply->deleteLater();
        d->sciReply = 0;
        emit statusChanged(d->status);
    } else {
        QSGGridScaledImage sci(d->sciReply);
        d->sciReply->deleteLater();
        d->sciReply = 0;
        setGridScaledImage(sci);
    }
}

void QSGBorderImage::doUpdate()
{
    update();
}

QSGNode *QSGBorderImage::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    Q_D(QSGBorderImage);

    if (!d->pix.texture() || width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGNinePatchNode *node = static_cast<QSGNinePatchNode *>(oldNode);

    if (!node) {
        node = new QSGNinePatchNode();
    }

    node->setTexture(d->pix.texture());

    const QSGScaleGrid *border = d->getScaleGrid();
    node->setInnerRect(QRectF(border->left(),
                              border->top(),
                              d->pix.width() - border->right() - border->left(),
                              d->pix.height() - border->bottom() - border->top()));
    node->setRect(QRectF(0, 0, width(), height()));
    node->setFiltering(d->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
    node->setHorzontalTileMode(d->horizontalTileMode);
    node->setVerticalTileMode(d->verticalTileMode);
    node->update();

    return node;
}

void QSGBorderImage::pixmapChange()
{
    Q_D(QSGBorderImage);

    d->pixmapChanged = true;
}

QT_END_NAMESPACE
