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

#include "qmlgraphicsimage_p.h"
#include "qmlgraphicsimage_p_p.h"

#include <QKeyEvent>
#include <QPainter>

QT_BEGIN_NAMESPACE


QML_DEFINE_TYPE(Qt,4,6,Image,QmlGraphicsImage)

/*!
    \qmlclass Image QmlGraphicsImage
    \brief The Image element allows you to add bitmaps to a scene.
    \inherits Item

    The Image element supports untransformed, stretched and tiled.

    For an explanation of stretching and tiling, see the fillMode property description.

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
    \o fillMode: Stretch (default)
    \qml
    Image {
        width: 160
        height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo3.png
    \o fillMode: Tile
    \qml
    Image {
        fillMode: Image.Tile
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo6.png
    \o fillMode: TileVertically
    \qml
    Image {
        fillMode: Image.TileVertically
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo5.png
    \o fillMode: TileHorizontally
    \qml
    Image {
        fillMode: Image.TileHorizontally
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \endtable
 */

/*!
    \internal
    \class QmlGraphicsImage Image
    \brief The QmlGraphicsImage class provides an image item that you can add to a QmlView.

    \ingroup group_coreitems

    Example:
    \qml
    Image { source: "pics/star.png" }
    \endqml

    A QmlGraphicsImage object can be instantiated in Qml using the tag \l Image.
*/

QmlGraphicsImage::QmlGraphicsImage(QmlGraphicsItem *parent)
    : QmlGraphicsImageBase(*(new QmlGraphicsImagePrivate), parent)
{
    connect(this, SIGNAL(sourceChanged(QUrl)), this, SLOT(updatePaintedGeometry()));
}

QmlGraphicsImage::QmlGraphicsImage(QmlGraphicsImagePrivate &dd, QmlGraphicsItem *parent)
    : QmlGraphicsImageBase(dd, parent)
{
}

QmlGraphicsImage::~QmlGraphicsImage()
{
}

void QmlGraphicsImage::setSource(const QUrl &url)
{
    QmlGraphicsImageBase::setSource(url);
    updatePaintedGeometry();
}

/*!
    \qmlproperty QPixmap Image::pixmap

    This property holds the QPixmap image to display.

    This is useful for displaying images provided by a C++ implementation,
    for example, a model may provide a data role of type QPixmap.
*/

QPixmap QmlGraphicsImage::pixmap() const
{
    Q_D(const QmlGraphicsImage);
    return d->pix;
}

void QmlGraphicsImage::setPixmap(const QPixmap &pix)
{
    Q_D(QmlGraphicsImage);
    if (!d->url.isEmpty())
        return;
    d->setPixmap(pix);
}

void QmlGraphicsImagePrivate::setPixmap(const QPixmap &pixmap)
{
    Q_Q(QmlGraphicsImage);
    pix = pixmap;

    q->setImplicitWidth(pix.width());
    q->setImplicitHeight(pix.height());
    status = pix.isNull() ? QmlGraphicsImageBase::Null : QmlGraphicsImageBase::Ready;

    q->update();
    emit q->pixmapChanged();
}

/*!
    \qmlproperty enumeration Image::fillMode

    Set this property to define what happens when the image set for the item is smaller
    than the size of the item.

    \list
    \o Stretch - the image is scaled to fit
    \o PreserveAspectFit - the image is scaled uniformly to fit without cropping
    \o PreserveAspectCrop - the image is scaled uniformly to fill, cropping if necessary
    \o Tile - the image is duplicated horizontally and vertically
    \o TileVertically - the image is stretched horizontally and tiled vertically
    \o TileHorizontally - the image is stretched vertically and tiled horizontally
    \endlist

    \image declarative-image_fillMode.gif
*/
QmlGraphicsImage::FillMode QmlGraphicsImage::fillMode() const
{
    Q_D(const QmlGraphicsImage);
    return d->fillMode;
}

void QmlGraphicsImage::setFillMode(FillMode mode)
{
    Q_D(QmlGraphicsImage);
    if (d->fillMode == mode)
        return;
    d->fillMode = mode;
    update();
    updatePaintedGeometry();
    emit fillModeChanged();
}

qreal QmlGraphicsImage::paintedWidth() const
{
    Q_D(const QmlGraphicsImage);
    return d->paintedWidth;
}

qreal QmlGraphicsImage::paintedHeight() const
{
    Q_D(const QmlGraphicsImage);
    return d->paintedHeight;
}

/*!
    \qmlproperty enum Image::status

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
    \qmlproperty real Image::progress

    This property holds the progress of image loading, from 0.0 (nothing loaded)
    to 1.0 (finished).

    \sa status
*/

/*!
    \qmlproperty bool Image::smooth

    Set this property if you want the image to be smoothly filtered when scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the image is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the image is stationary on
    the screen.  A common pattern when animating an image is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QmlGraphicsImage::updatePaintedGeometry()
{
    Q_D(QmlGraphicsImage);

    if (d->fillMode == PreserveAspectFit) {
        qreal widthScale = width() / qreal(d->pix.width());
        qreal heightScale = height() / qreal(d->pix.height());
        if (!d->pix.width() || !d->pix.height())
            return;
        if (widthScale <= heightScale) {
            d->paintedWidth = width();
            d->paintedHeight = widthScale * qreal(d->pix.height());
        } else if(heightScale < widthScale) {
            d->paintedWidth = heightScale * qreal(d->pix.width());
            d->paintedHeight = height();
        }
    } else {
        d->paintedWidth = width();
        d->paintedHeight = height();
    }
    emit paintedGeometryChanged();
}

void QmlGraphicsImage::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QmlGraphicsImageBase::geometryChanged(newGeometry, oldGeometry);
    updatePaintedGeometry();
}

/*!
    \qmlproperty url Image::source

    Image can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    The URL may be absolute, or relative to the URL of the component.
*/

void QmlGraphicsImage::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QmlGraphicsImage);
    if (d->pix.isNull())
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    if (width() != d->pix.width() || height() != d->pix.height()) {
        if (d->fillMode >= Tile) {
            if (d->fillMode == Tile)
                p->drawTiledPixmap(QRectF(0,0,width(),height()), d->pix);
            else if (d->fillMode == TileVertically)
                p->drawTiledPixmap(QRectF(0,0,d->pix.width(),height()), d->pix);
            else
                p->drawTiledPixmap(QRectF(0,0,width(),d->pix.height()), d->pix);
        } else {
            qreal widthScale = width() / qreal(d->pix.width());
            qreal heightScale = height() / qreal(d->pix.height());

            QTransform scale;

            if (d->fillMode == PreserveAspectFit) {
                if (widthScale <= heightScale) {
                    heightScale = widthScale;
                    scale.translate(0, (height() - heightScale * d->pix.height()) / 2);
                } else if(heightScale < widthScale) {
                    widthScale = heightScale;
                    scale.translate((width() - widthScale * d->pix.width()) / 2, 0);
                }
            } else if (d->fillMode == PreserveAspectCrop) {
                if (widthScale < heightScale) {
                    widthScale = heightScale;
                    scale.translate((width() - widthScale * d->pix.width()) / 2, 0);
                } else if(heightScale < widthScale) {
                    heightScale = widthScale;
                    scale.translate(0, (height() - heightScale * d->pix.height()) / 2);
                }
            }
            if (clip()) {
                p->save();
                p->setClipRect(boundingRect(), Qt::IntersectClip);
            }
            scale.scale(widthScale, heightScale);
            QTransform old = p->transform();
            p->setWorldTransform(scale * old);
            p->drawPixmap(0, 0, d->pix);
            p->setWorldTransform(old);
            if (clip()) {
                p->restore();
            }
        }
    } else {
        p->drawPixmap(0, 0, d->pix);
    }

    if (d->smooth) {
        p->setRenderHint(QPainter::Antialiasing, oldAA);
        p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

QT_END_NAMESPACE
