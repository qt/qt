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

#include "qfxborderimage.h"
#include "qfxborderimage_p.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QtDeclarative/qmlengine.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,BorderImage,QFxBorderImage)

/*!
    \qmlclass BorderImage QFxBorderImage
    \brief The BorderImage element allows you to add an image used as a border to a scene.
    \inherits Item

    Example:
    \qml
    BorderImage { border.left: 20; border.right: 10
            border.top: 14; border.bottom: 14
            width: 160; height: 160
            source: "pics/qtlogo.png"
    }
    \endqml
 */

/*!
    \internal
    \class QFxBorderImage BorderImage
    \brief The QFxBorderImage class provides an image item that you can add to a QFxView.

    \ingroup group_coreitems

    Example:
    \qml
    BorderImage { source: "pics/star.png" }
    \endqml

    A QFxBorderImage object can be instantiated in Qml using the tag \l BorderImage.
*/

QFxBorderImage::QFxBorderImage(QFxItem *parent)
  : QFxImageBase(*(new QFxBorderImagePrivate), parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxBorderImage::~QFxBorderImage()
{
    Q_D(QFxBorderImage);
    if (d->sciReply)
        d->sciReply->deleteLater();
    if (!d->sciurl.isEmpty())
        QFxPixmap::cancelGet(d->sciurl, this);
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
    \qmlproperty url BorderImage::source

    BorderImage can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    It can also handle .sci files, which are a Qml-specific format. A .sci file uses a simple text-based format that specifies
    \list
    \i the grid lines describing a \l {BorderImage::border.left}{scale grid}.
    \i an image file.
    \endlist

    The following .sci file sets grid line offsets of 10 on each side for the image \c picture.png:
    \code
    gridLeft: 10
    gridTop: 10
    gridBottom: 10
    gridRight: 10
    imageFile: picture.png
    \endcode

    The URL may be absolute, or relative to the URL of the component.
*/

void QFxBorderImage::setSource(const QUrl &url)
{
    Q_D(QFxBorderImage);
    //equality is fairly expensive, so we bypass for simple, common case
    if ((d->url.isEmpty() == url.isEmpty()) && url == d->url)
        return;

    if (d->sciReply) {
        d->sciReply->deleteLater();
        d->sciReply = 0;
    }

    if (!d->url.isEmpty())
        QFxPixmap::cancelGet(d->url, this);
    if (!d->sciurl.isEmpty())
        QFxPixmap::cancelGet(d->sciurl, this);

    d->url = url;
    d->sciurl = QUrl();
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
        if (d->url.path().endsWith(QLatin1String(".sci"))) {
#ifndef QT_NO_LOCALFILE_OPTIMIZED_QML
            if (d->url.scheme() == QLatin1String("file")) {
                QFile file(d->url.toLocalFile());
                file.open(QIODevice::ReadOnly);
                setGridScaledImage(QFxGridScaledImage(&file));
            } else
#endif
            {
                QNetworkRequest req(d->url);
                req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
                d->sciReply = qmlEngine(this)->networkAccessManager()->get(req);
                QObject::connect(d->sciReply, SIGNAL(finished()),
                                 this, SLOT(sciRequestFinished()));
            }
        } else {
            d->reply = QFxPixmap::get(qmlEngine(this), d->url, &d->pix);
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
    }

    emit statusChanged(d->status);
}

/*!
    \qmlproperty int BorderImage::border.left
    \qmlproperty int BorderImage::border.right
    \qmlproperty int BorderImage::border.top
    \qmlproperty int BorderImage::border.bottom

    \target ImagexmlpropertiesscaleGrid

    A scale grid uses 4 grid lines (2 horizontal and 2 vertical) to break an image into 9 sections, as shown below:

    \image declarative-scalegrid.png

    When the image is scaled:
    \list
    \i the corners (sections 1, 3, 7, and 9) are not scaled at all
    \i the middle (section 5) is scaled both horizontally and vertically
    \i sections 2 and 8 are scaled horizontally
    \i sections 4 and 6 are scaled vertically
    \endlist

    Each scale grid property (left, right, top, and bottom) specifies an offset from the respective side. For example, \c{border.bottom: 10} sets the bottom scale grid line 10 pixels up from the bottom of the image.

    A scale grid can also be specified using a
    \l {BorderImage::source}{.sci file}.
*/

QFxScaleGrid *QFxBorderImage::border()
{
    Q_D(QFxBorderImage);
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

    \sa examples/declarative/border-image
*/
QFxBorderImage::TileMode QFxBorderImage::horizontalTileMode() const
{
    Q_D(const QFxBorderImage);
    return d->horizontalTileMode;
}

void QFxBorderImage::setHorizontalTileMode(TileMode t)
{
    Q_D(QFxBorderImage);
    d->horizontalTileMode = t;
}

QFxBorderImage::TileMode QFxBorderImage::verticalTileMode() const
{
    Q_D(const QFxBorderImage);
    return d->verticalTileMode;
}

void QFxBorderImage::setVerticalTileMode(TileMode t)
{
    Q_D(QFxBorderImage);
    d->verticalTileMode = t;
}

void QFxBorderImage::setGridScaledImage(const QFxGridScaledImage& sci)
{
    Q_D(QFxBorderImage);
    if (!sci.isValid()) {
        d->status = Error;
        emit statusChanged(d->status);
    } else {
        QFxScaleGrid *sg = border();
        sg->setTop(sci.gridTop());
        sg->setBottom(sci.gridBottom());
        sg->setLeft(sci.gridLeft());
        sg->setRight(sci.gridRight());
        d->horizontalTileMode = sci.horizontalTileRule();
        d->verticalTileMode = sci.verticalTileRule();

        d->sciurl = d->url.resolved(QUrl(sci.pixmapUrl()));
        d->reply = QFxPixmap::get(qmlEngine(this), d->sciurl, &d->pix);
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
}

void QFxBorderImage::requestFinished()
{
    Q_D(QFxBorderImage);
    if (d->url.path().endsWith(QLatin1String(".sci"))) {
        QFxPixmap::find(d->sciurl, &d->pix);
    } else {
        if (d->reply) {
            //###disconnect really needed?
            disconnect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                       this, SLOT(requestProgress(qint64,qint64)));
            if (d->reply->error() != QNetworkReply::NoError)
                d->status = Error;
        }
        QFxPixmap::find(d->url, &d->pix);
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

void QFxBorderImage::sciRequestFinished()
{
    Q_D(QFxBorderImage);
    if (d->sciReply->error() != QNetworkReply::NoError) {
        d->status = Error;
        d->sciReply->deleteLater();
        d->sciReply = 0;
        emit statusChanged(d->status);
    } else {
        QFxGridScaledImage sci(d->sciReply);
        d->sciReply->deleteLater();
        d->sciReply = 0;
        setGridScaledImage(sci);
    }
}

void QFxBorderImage::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QFxBorderImage);
    if (d->pix.isNull())
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    QPixmap pix = d->pix;

    QMargins margins(border()->top(), border()->left(), border()->bottom(), border()->right());
    QTileRules rules((Qt::TileRule)d->horizontalTileMode, (Qt::TileRule)d->verticalTileMode);
    qDrawBorderPixmap(p, QRect(0, 0, (int)d->width, (int)d->height), margins, pix, pix.rect(), margins, rules);
    if (d->smooth) {
        p->setRenderHint(QPainter::Antialiasing, oldAA);
        p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

QFxBorderImage::QFxBorderImage(QFxBorderImagePrivate &dd, QFxItem *parent)
  : QFxImageBase(dd, parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QT_END_NAMESPACE
