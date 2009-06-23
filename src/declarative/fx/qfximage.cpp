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

#include "qfximage.h"
#include "qfximage_p.h"
#include <qfxperf.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QtDeclarative/qmlengine.h>

QT_BEGIN_NAMESPACE


QML_DEFINE_TYPE(QFxImage,Image)

/*!
    \qmlclass Image QFxImage
    \brief The Image element allows you to add bitmaps to a scene.
    \inherits Item

    The Image element supports untransformed, stretched, grid-scaled and tiled images.
    For an explanation of grid-scaling see the scaleGrid property description
    or the QFxScaleGrid class description.

    Examples:
    \table
    \row
    \o \image declarative-qtlogo1.png
    \o Untransformed
    \qml
    Image { source: "pics/qtlogo.png" }
    \endqml
    \row
    \o \image declarative-qtlogo2.png
    \o Stretched
    \qml
    Image { width: 160; height: 160; source: "pics/qtlogo.png" }
    \endqml
    \row
    \o \image declarative-qtlogo4.png
    \o Grid-scaled
    \qml
    Image { scaleGrid.left: 20; scaleGrid.right: 10
            scaleGrid.top: 14; scaleGrid.bottom: 14
            width: 160; height: 160; source: "pics/qtlogo.png" }
    \endqml
    \row
    \o \image declarative-qtlogo3.png
    \o Tiled
    \qml
    Image { tile: true; width: 160; height: 160; source: "pics/qtlogo.png" }
    \endqml
    \endtable
 */

/*!
    \internal
    \class QFxImage Image
    \brief The QFxImage class provides an image item that you can add to a QFxView.

    \ingroup group_coreitems

    Example:
    \qml
    Image { source: "pics/star.png" }
    \endqml

    A QFxImage object can be instantiated in Qml using the tag \l Image.
*/

QFxImage::QFxImage(QFxItem *parent)
  : QFxItem(*(new QFxImagePrivate), parent)
{
    setOptions(SimpleItem | HasContents, true);
}

QFxImage::QFxImage(QFxImagePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    setOptions(SimpleItem | HasContents, true);
}

QFxImage::~QFxImage()
{
    Q_D(QFxImage);
    if (d->sciReply)
        d->sciReply->deleteLater();
    if (!d->url.isEmpty())
        QFxPixmap::cancelGet(d->url, this);
    if (!d->sciurl.isEmpty())
        QFxPixmap::cancelGet(d->sciurl, this);
}

/*!
    \property QFxImage::pixmap
    \brief the image displayed in this item.

    This property contains the image currently being displayed by this item,
    which may be an empty pixmap if nothing is currently displayed. If this
    property is set, the source property will be unset. This property is intended
    to be used only in C++, not in QML.
*/
QPixmap QFxImage::pixmap() const
{
    Q_D(const QFxImage);
    return d->pix;
}

void QFxImage::setPixmap(const QPixmap &pix)
{
    Q_D(QFxImage);
    d->url = QUrl();
    d->pix = pix;
    d->opaque=false;

    setImplicitWidth(d->pix.width());
    setImplicitHeight(d->pix.height());

    update();
}

/*!
    \qmlproperty int Image::scaleGrid.left
    \qmlproperty int Image::scaleGrid.right
    \qmlproperty int Image::scaleGrid.top
    \qmlproperty int Image::scaleGrid.bottom

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

    Each scale grid property (left, right, top, and bottom) specifies an offset from the respective side. For example, \c scaleGrid.bottom="10" sets the bottom scale grid line 10 pixels up from the bottom of the image.

    A scale grid can also be specified using a
    \l {Image::source}{.sci file}.
*/
QFxScaleGrid *QFxImage::scaleGrid()
{
    Q_D(QFxImage);
    return d->getScaleGrid();
}

/*!
    \qmlproperty bool Image::tile

    Set this property to enable image tiling.  Normally the Image element scales the
    bitmap file to its size.  If tiling is enabled, the bitmap is repeated as a set
    of unscaled tiles, clipped to the size of the Image.

    \qml
    Item {
        Image { source: "tile.png" }
        Image { x: 80; width: 100; height: 100; source: "tile.png" }
        Image { x: 190; width: 100; height: 100; tile: true; source: "tile.png" }
    }
    \endqml
    \image declarative-image_tile.png

    If both tiling and the scaleGrid are set, tiling takes precedence.
*/
bool QFxImage::isTiled() const
{
    Q_D(const QFxImage);
    return d->tiled;
}

void QFxImage::setTiled(bool tile)
{
    Q_D(QFxImage);
    d->tiled = tile;
}

/*!
    \qmlproperty bool Image::opaque

    Set this property if you know that the image is opaque to give your 
    application a significant performance boost.  

    \note
    This is a performance hint to Qt Declarative.  Unfortunately whether or not an image
    is opaque is not automatically detected.  Setting this property to true when
    the image is not opaque will lead to drawing artifacts.  However, leaving it as 
    false will always work correctly - although possibly not at maximum performance. 
 */

/*!
    \property QFxImage::opaque
    \brief whether the image is opaque (non-transparent).

    This property is provided purely for the purpose of optimization. An opaque
    image can be optimized more than a non-opaque one.
*/
bool QFxImage::isOpaque() const
{
    Q_D(const QFxImage);
    return d->opaque;
}

void QFxImage::setOpaque(bool o)
{
    Q_D(QFxImage);
    if (o == d->opaque)
        return;
    d->opaque = o;

    setOptions(IsOpaque, o);

    update();
}

void QFxImage::componentComplete()
{
    QFxItem::componentComplete();
}

/*!
    \property QFxImage::scaleGrid
    \brief the 3x3 grid used to scale an image, excluding the corners.
*/

/*!
    \qmlproperty bool Image::smooth

    Set this property if you want the image to be smoothly filtered when scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If 
    the Image is displayed at its natural size, this property has no visual or 
    performance effect.

    \note Generally scaling artifacts are only visible if the image is stationary on 
    the screen.  A common pattern when animating an image is to disable smooth 
    filtering at the beginning of the animation and reenable it at the conclusion.
 */

/*!
    \property QFxImage::smooth
    \brief whether the image is smoothly transformed.

    This property is provided purely for the purpose of optimization. Turning
    smooth transforms off is faster, but looks worse; turning smooth 
    transformations on is slower, but looks better.

    By default smooth transformations are off.
*/
bool QFxImage::smoothTransform() const
{
    Q_D(const QFxImage);
    return d->smooth;
}

void QFxImage::setSmoothTransform(bool s)
{
    Q_D(QFxImage);
    if (d->smooth == s)
        return;
    d->smooth = s;
    update();
}

void QFxImage::paintContents(QPainter &p)
{
    Q_D(QFxImage);
    if (d->pix.isNull())
        return;

    QPainter::RenderHints oldHints = p.renderHints();
    if (d->smooth)
        p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    QPixmap pix = d->pix;

    if (d->tiled) {
        p.save();
        p.setClipRect(0, 0, width(), height(), Qt::IntersectClip);
        QRect me = QRect(0, 0, width(), height());

        int pw = pix.width();
        int ph = pix.height();
        int yy = 0;

        while(yy < height()) {
            int xx = 0;
            while(xx < width()) {
                p.drawPixmap(xx, yy, pix);
                xx += pw;
            }
            yy += ph;
        }

        p.restore();
    } else if (!d->scaleGrid || d->scaleGrid->isNull()) {
        if (width() != pix.width() || height() != pix.height()) {
            QTransform scale;
            scale.scale(width() / qreal(pix.width()), 
                        height() / qreal(pix.height()));
            QTransform old = p.transform();
            p.setWorldTransform(scale * old);
            p.drawPixmap(0, 0, pix);
            p.setWorldTransform(old);
        } else {
            p.drawPixmap(0, 0, pix);
        }
    } else {
        int sgl = d->scaleGrid->left();
        int sgr = d->scaleGrid->right();
        int sgt = d->scaleGrid->top();
        int sgb = d->scaleGrid->bottom();

        int w = width();
        int h = height();
        if (sgt + sgb > h)
            sgt = sgb = h/2;
        if (sgl + sgr > w)
            sgl = sgr = w/2;

        const int xSide = sgl + sgr;
        const int ySide = sgt + sgb;

        // Upper left
        if (sgt && sgl)
            p.drawPixmap(QRect(0, 0, sgl, sgt), pix, QRect(0, 0, sgl, sgt));
        // Upper middle
        if (pix.width() - xSide && sgt)
            p.drawPixmap(QRect(sgl, 0, w - xSide, sgt), pix,
                        QRect(sgl, 0, pix.width() - xSide, sgt));
        // Upper right
        if (sgt && pix.width() - sgr) 
            p.drawPixmap(QPoint(w-sgr, 0), pix,
                        QRect(pix.width()-sgr, 0, sgr, sgt));
        // Middle left
        if (sgl && pix.height() - ySide)
            p.drawPixmap(QRect(0, sgt, sgl, h - ySide), pix,
                        QRect(0, sgt, sgl, pix.height() - ySide));

        // Middle
        if (pix.width() - xSide && pix.height() - ySide)
            p.drawPixmap(QRect(sgl, sgt, w - xSide, h - ySide),
                        pix, 
                        QRect(sgl, sgt, pix.width() - xSide, pix.height() - ySide));
        // Middle right
        if (sgr && pix.height() - ySide)
            p.drawPixmap(QRect(w-sgr, sgt, sgr, h - ySide), pix,
                        QRect(pix.width()-sgr, sgt, sgr, pix.height() - ySide));
        // Lower left 
        if (sgl && sgr)
            p.drawPixmap(QPoint(0, h - sgb), pix,
                        QRect(0, pix.height() - sgb, sgl, sgb));
        // Lower Middle
        if (pix.width() - xSide && sgb)
            p.drawPixmap(QRect(sgl, h - sgb, w - xSide, sgb), pix,
                        QRect(sgl, pix.height() - sgb, pix.width() - xSide, sgb));
        // Lower Right
        if (sgr && sgb)
            p.drawPixmap(QPoint(w-sgr, h - sgb), pix,
                        QRect(pix.width()-sgr, pix.height() - sgb, sgr, sgb));
    }

    if (d->smooth)
        p.setRenderHints(oldHints);
}

QString QFxImage::propertyInfo() const
{
    Q_D(const QFxImage);
    return d->url.toString();
}

/*!
    \qmlproperty enum Image::status

    This property holds the status of image loading.  It can be one of:
    \list
    \o Idle - no image has been set, or the image has been loaded
    \o Loading - the images is currently being loaded
    \o Error - an error occurred while loading the image
    \endlist

    \sa progress
*/

QFxImage::Status QFxImage::status() const
{
    Q_D(const QFxImage);
    return d->status;
}

/*!
    \qmlproperty real Image::progress

    This property holds the progress of image loading, from 0.0 (nothing loaded)
    to 1.0 (finished).

    \sa status
*/

qreal QFxImage::progress() const
{
    Q_D(const QFxImage);
    return d->progress;
}

/*!
    \qmlproperty string Image::source

    Image can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    It can also handle .sci files, which are a Qml-specific format. A .sci file uses a simple text-based format that specifies
    \list
    \i the grid lines describing a \l {Image::scaleGrid.left}{scale grid}.
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

/*!
    \property QFxImage::source
    \brief the url of the image to be displayed in this item.

    The content specified can be of any image type loadable by QImage. Alternatively,
    you can specify an sci format file, which specifies both an image and it's scale grid.
*/
QUrl QFxImage::source() const
{
    Q_D(const QFxImage);
    return d->url;
}

void QFxImage::setSource(const QUrl &url)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::PixmapLoad> perf;
#endif
    Q_D(QFxImage);
    if (url == d->url)
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
        setPixmap(QPixmap());
        d->status = Idle;
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
            d->reply = QFxPixmap::get(qmlEngine(this), d->url, this, SLOT(requestFinished()));
            if (d->reply) {
                connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                        this, SLOT(requestProgress(qint64,qint64)));
            } else {
                d->progress = 1.0;
                emit progressChanged(d->progress);
            }
        }
    }

    emit statusChanged(d->status);
}

void QFxImage::requestFinished()
{
    Q_D(QFxImage);
    if (d->url.path().endsWith(QLatin1String(".sci"))) {
        d->pix = QFxPixmap(d->sciurl);
    } else {
        if (d->reply) {
            disconnect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                       this, SLOT(requestProgress(qint64,qint64)));
            if (d->reply->error() != QNetworkReply::NoError)
                d->status = Error;
        }
        d->pix = QFxPixmap(d->url);
        setOptions(QFxImage::SimpleItem, true);
    }
    setImplicitWidth(d->pix.width());
    setImplicitHeight(d->pix.height());

    if (d->status == Loading)
        d->status = Idle;
    d->progress = 1.0;
    emit statusChanged(d->status);
    emit sourceChanged(d->url);
    emit progressChanged(1.0);
    update();
}

void QFxImage::sciRequestFinished()
{
    Q_D(QFxImage);
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

void QFxImage::requestProgress(qint64 received, qint64 total)
{
    Q_D(QFxImage);
    if (d->status == Loading && total > 0) {
        d->progress = qreal(received)/total;
        emit progressChanged(d->progress);
    }
}

void QFxImage::setGridScaledImage(const QFxGridScaledImage& sci)
{
    Q_D(QFxImage);
    if (!sci.isValid()) {
        d->status = Error;
        emit statusChanged(d->status);
    } else {
        QFxScaleGrid *sg = scaleGrid();
        sg->setTop(sci.gridTop());
        sg->setBottom(sci.gridBottom());
        sg->setLeft(sci.gridLeft());
        sg->setRight(sci.gridRight());
        setOptions(QFxImage::SimpleItem, false);

        d->sciurl = d->url.resolved(QUrl(sci.pixmapUrl()));
        d->reply = QFxPixmap::get(qmlEngine(this), d->sciurl, this, SLOT(requestFinished()));
        if (d->reply) {
            connect(d->reply, SIGNAL(downloadProgress(qint64,qint64)),
                    this, SLOT(requestProgress(qint64,qint64)));
        } else {
            d->progress = 1.0;
            emit progressChanged(d->progress);
        }
    }
}


QT_END_NAMESPACE
