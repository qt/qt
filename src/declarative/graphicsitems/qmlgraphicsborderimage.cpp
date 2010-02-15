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

#include "qmlgraphicsborderimage_p.h"
#include "qmlgraphicsborderimage_p_p.h"

#include <qmlengine.h>

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass BorderImage QmlGraphicsBorderImage
    \brief The BorderImage element provides an image that can be used as a border.
    \inherits Item

    \snippet snippets/declarative/border-image.qml 0

    \image BorderImage.png
 */

/*!
    \internal
    \class QmlGraphicsBorderImage BorderImage
    \brief The QmlGraphicsBorderImage class provides an image item that you can add to a QmlView.
*/

QmlGraphicsBorderImage::QmlGraphicsBorderImage(QmlGraphicsItem *parent)
  : QmlGraphicsImageBase(*(new QmlGraphicsBorderImagePrivate), parent)
{
}

QmlGraphicsBorderImage::~QmlGraphicsBorderImage()
{
    Q_D(QmlGraphicsBorderImage);
    if (d->sciReply)
        d->sciReply->deleteLater();
    if (d->sciPendingPixmapCache)
        QmlPixmapCache::cancel(d->sciurl, this);
}
/*!
    \qmlproperty enum BorderImage::status

    This property holds the status of image loading.  It can be one of:
    \list
    \o Null - no image has been set
    \o Ready - the image has been loaded
    \o Loading - the image is currently being loaded
    \o Error - an error occurred while loading the image
    \endlist

    \sa progress
*/

/*!
    \qmlproperty real BorderImage::progress

    This property holds the progress of image loading, from 0.0 (nothing loaded)
    to 1.0 (finished).

    \sa status
*/

/*!
    \qmlproperty bool BorderImage::smooth

    Set this property if you want the image to be smoothly filtered when scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the image is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the image is stationary on
    the screen.  A common pattern when animating an image is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

/*!
    \qmlproperty url BorderImage::source

    BorderImage can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    It can also handle .sci files, which are a Qml-specific format. A .sci file uses a simple text-based format that specifies
    the borders, the image file and the tile rules.

    The following .sci file sets the borders to 10 on each side for the image \c picture.png:
    \qml
    border.left: 10
    border.top: 10
    border.bottom: 10
    border.right: 10
    source: picture.png
    \endqml

    The URL may be absolute, or relative to the URL of the component.
*/

static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}


void QmlGraphicsBorderImage::setSource(const QUrl &url)
{
    Q_D(QmlGraphicsBorderImage);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    if (d->sciReply) {
        d->sciReply->deleteLater();
        d->sciReply = 0;
    }

    if (d->pendingPixmapCache) {
        QmlPixmapCache::cancel(d->url, this);
        d->pendingPixmapCache = false;
    }
    if (d->sciPendingPixmapCache) {
        QmlPixmapCache::cancel(d->sciurl, this);
        d->sciPendingPixmapCache = false;
    }

    d->url = url;
    d->sciurl = QUrl();
    if (d->progress != 0.0) {
        d->progress = 0.0;
        emit progressChanged(d->progress);
    }

    if (url.isEmpty()) {
        d->pix = QPixmap();
        d->status = Null;
        setImplicitWidth(0);
        setImplicitHeight(0);
        emit statusChanged(d->status);
        emit sourceChanged(d->url);
        update();
    } else {
        d->status = Loading;
        if (d->url.path().endsWith(QLatin1String(".sci"))) {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
            QString lf = toLocalFileOrQrc(d->url);
            if (!lf.isEmpty()) {
                QFile file(lf);
                file.open(QIODevice::ReadOnly);
                setGridScaledImage(QmlGraphicsGridScaledImage(&file));
            } else
#endif
            {
                QNetworkRequest req(d->url);
                d->sciReply = qmlEngine(this)->networkAccessManager()->get(req);
                QObject::connect(d->sciReply, SIGNAL(finished()),
                                 this, SLOT(sciRequestFinished()));
            }
        } else {
            QmlPixmapReply::Status status = QmlPixmapCache::get(d->url, &d->pix);
            if (status != QmlPixmapReply::Ready && status != QmlPixmapReply::Error) {
                QmlPixmapReply *reply = QmlPixmapCache::request(qmlEngine(this), d->url);
                d->pendingPixmapCache = true;
                connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
                connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                        this, SLOT(requestProgress(qint64,qint64)));
            } else {
                //### should be unified with requestFinished
                setImplicitWidth(d->pix.width());
                setImplicitHeight(d->pix.height());

                if (d->pix.isNull())
                    d->status = Error;
                if (d->status == Loading)
                    d->status = Ready;
                d->progress = 1.0;
                emit statusChanged(d->status);
                emit sourceChanged(d->url);
                emit progressChanged(d->progress);
                update();
            }
        }
    }

    emit statusChanged(d->status);
}

/*!
    \qmlproperty int BorderImage::border.left
    \qmlproperty int BorderImage::border.right
    \qmlproperty int BorderImage::border.top
    \qmlproperty int BorderImage::border.bottom

    \target ImagexmlpropertiesscaleGrid

    The 4 border lines (2 horizontal and 2 vertical) break an image into 9 sections, as shown below:

    \image declarative-scalegrid.png

    When the image is scaled:
    \list
    \i the corners (sections 1, 3, 7, and 9) are not scaled at all
    \i the middle (section 5) is scaled according to BorderImage::horizontalTileMode and BorderImage::verticalTileMode
    \i sections 2 and 8 are scaled according to BorderImage::horizontalTileMode
    \i sections 4 and 6 are scaled according to BorderImage::verticalTileMode
    \endlist

    Each border line (left, right, top, and bottom) specifies an offset from the respective side. For example, \c{border.bottom: 10} sets the bottom line 10 pixels up from the bottom of the image.

    The border lines can also be specified using a
    \l {BorderImage::source}{.sci file}.
*/

QmlGraphicsScaleGrid *QmlGraphicsBorderImage::border()
{
    Q_D(QmlGraphicsBorderImage);
    return d->getScaleGrid();
}

/*!
    \qmlproperty TileMode BorderImage::horizontalTileMode
    \qmlproperty TileMode BorderImage::verticalTileMode

    This property describes how to repeat or stretch the middle parts of the border image.

    \list
    \o Stretch - Scale the image to fit to the available area.
    \o Repeat - Tile the image until there is no more space. May crop the last image.
    \o Round - Like Repeat, but scales the images down to ensure that the last image is not cropped.
    \endlist
*/
QmlGraphicsBorderImage::TileMode QmlGraphicsBorderImage::horizontalTileMode() const
{
    Q_D(const QmlGraphicsBorderImage);
    return d->horizontalTileMode;
}

void QmlGraphicsBorderImage::setHorizontalTileMode(TileMode t)
{
    Q_D(QmlGraphicsBorderImage);
    if (t != d->horizontalTileMode) {
        d->horizontalTileMode = t;
        emit horizontalTileModeChanged();
        update();
    }
}

QmlGraphicsBorderImage::TileMode QmlGraphicsBorderImage::verticalTileMode() const
{
    Q_D(const QmlGraphicsBorderImage);
    return d->verticalTileMode;
}

void QmlGraphicsBorderImage::setVerticalTileMode(TileMode t)
{
    Q_D(QmlGraphicsBorderImage);
    if (t != d->verticalTileMode) {
        d->verticalTileMode = t;
        emit verticalTileModeChanged();
        update();
    }
}

void QmlGraphicsBorderImage::setGridScaledImage(const QmlGraphicsGridScaledImage& sci)
{
    Q_D(QmlGraphicsBorderImage);
    if (!sci.isValid()) {
        d->status = Error;
        emit statusChanged(d->status);
    } else {
        QmlGraphicsScaleGrid *sg = border();
        sg->setTop(sci.gridTop());
        sg->setBottom(sci.gridBottom());
        sg->setLeft(sci.gridLeft());
        sg->setRight(sci.gridRight());
        d->horizontalTileMode = sci.horizontalTileRule();
        d->verticalTileMode = sci.verticalTileRule();

        d->sciurl = d->url.resolved(QUrl(sci.pixmapUrl()));
        QmlPixmapReply::Status status = QmlPixmapCache::get(d->sciurl, &d->pix);
        if (status != QmlPixmapReply::Ready && status != QmlPixmapReply::Error) {
            QmlPixmapReply *reply = QmlPixmapCache::request(qmlEngine(this), d->sciurl);
            d->sciPendingPixmapCache = true;
            connect(reply, SIGNAL(finished()), this, SLOT(requestFinished()));
            connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
                    this, SLOT(requestProgress(qint64,qint64)));
        } else {
            //### should be unified with requestFinished
            setImplicitWidth(d->pix.width());
            setImplicitHeight(d->pix.height());

            if (d->pix.isNull())
                d->status = Error;
            if (d->status == Loading)
                d->status = Ready;
            d->progress = 1.0;
            emit statusChanged(d->status);
            emit sourceChanged(d->url);
            emit progressChanged(1.0);
            update();
        }
    }
}

void QmlGraphicsBorderImage::requestFinished()
{
    Q_D(QmlGraphicsBorderImage);

    if (d->url.path().endsWith(QLatin1String(".sci"))) {
        d->sciPendingPixmapCache = false;
        QmlPixmapCache::get(d->sciurl, &d->pix);
    } else {
        d->pendingPixmapCache = false;
        if (QmlPixmapCache::get(d->url, &d->pix) != QmlPixmapReply::Ready)
            d->status = Error;
    }
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

void QmlGraphicsBorderImage::sciRequestFinished()
{
    Q_D(QmlGraphicsBorderImage);
    if (d->sciReply->error() != QNetworkReply::NoError) {
        d->status = Error;
        d->sciReply->deleteLater();
        d->sciReply = 0;
        emit statusChanged(d->status);
    } else {
        QmlGraphicsGridScaledImage sci(d->sciReply);
        d->sciReply->deleteLater();
        d->sciReply = 0;
        setGridScaledImage(sci);
    }
}

void QmlGraphicsBorderImage::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QmlGraphicsBorderImage);
    if (d->pix.isNull())
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    const QmlGraphicsScaleGrid *border = d->getScaleGrid();
    QMargins margins(border->left(), border->top(), border->right(), border->bottom());
    QTileRules rules((Qt::TileRule)d->horizontalTileMode, (Qt::TileRule)d->verticalTileMode);
    qDrawBorderPixmap(p, QRect(0, 0, (int)d->width, (int)d->height), margins, d->pix, d->pix.rect(), margins, rules);
    if (d->smooth) {
        p->setRenderHint(QPainter::Antialiasing, oldAA);
        p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

QT_END_NAMESPACE
