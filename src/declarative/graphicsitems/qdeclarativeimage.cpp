/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativeimage_p.h"
#include "private/qdeclarativeimage_p_p.h"

#include <QKeyEvent>
#include <QPainter>

QT_BEGIN_NAMESPACE


/*!
    \qmlclass Image QDeclarativeImage
    \since 4.7
    \ingroup qml-basic-visual-elements
    \brief The Image element displays an image in a declarative user interface
    \inherits Item

    The Image element is used to display images in a declarative user interface.

    The source of the image is specified as a URL using the \l source property.
    Images can be supplied in any of the standard image formats supported by Qt,
    including bitmap formats such as PNG and JPEG, and vector graphics formats
    such as SVG. If you need to display animated images, use the \l AnimatedImage
    element.

    If the \l{Item::width}{width} and \l{Item::height}{height} properties are not
    specified, the Image element automatically uses the size of the loaded image.
    By default, specifying the width and height of the element causes the image
    to be scaled to that size. This behavior can be changed by setting the
    \l fillMode property, allowing the image to be stretched and tiled instead.

    \section1 Example Usage

    The following example shows the simplest usage of the Image element.

    \snippet doc/src/snippets/declarative/image.qml document

    \beginfloatleft
    \image declarative-qtlogo.png
    \endfloat

    \clearfloat

    \section1 Performance

    By default, locally available images are loaded immediately, and the user interface
    is blocked until loading is complete. If a large image is to be loaded, it may be
    preferable to load the image in a low priority thread, by enabling the \l asynchronous
    property.

    If the image is obtained from a network rather than a local resource, it is
    automatically loaded asynchronously, and the \l progress and \l status properties
    are updated as appropriate.

    Images are cached and shared internally, so if several Image elements have the same \l source,
    only one copy of the image will be loaded.

    \bold Note: Images are often the greatest user of memory in QML user interfaces.  It is recommended
    that images which do not form part of the user interface have their
    size bounded via the \l sourceSize property. This is especially important for content
    that is loaded from external sources or provided by the user.

    \sa {declarative/imageelements/image}{Image example}, QDeclarativeImageProvider
*/

QDeclarativeImage::QDeclarativeImage(QDeclarativeItem *parent)
    : QDeclarativeImageBase(*(new QDeclarativeImagePrivate), parent)
{
}

QDeclarativeImage::QDeclarativeImage(QDeclarativeImagePrivate &dd, QDeclarativeItem *parent)
    : QDeclarativeImageBase(dd, parent)
{
}

QDeclarativeImage::~QDeclarativeImage()
{
}

QPixmap QDeclarativeImage::pixmap() const
{
    Q_D(const QDeclarativeImage);
    return d->pix.pixmap();
}

void QDeclarativeImage::setPixmap(const QPixmap &pix)
{
    Q_D(QDeclarativeImage);
    if (!d->url.isEmpty())
        return;
    d->setPixmap(pix);
}

void QDeclarativeImagePrivate::setPixmap(const QPixmap &pixmap)
{
    Q_Q(QDeclarativeImage);
    pix.setPixmap(pixmap);

    q->setImplicitWidth(pix.width());
    q->setImplicitHeight(pix.height());
    status = pix.isNull() ? QDeclarativeImageBase::Null : QDeclarativeImageBase::Ready;

    q->update();
    q->pixmapChange();
}

/*!
    \qmlproperty enumeration Image::fillMode

    Set this property to define what happens when the image set for the item is smaller
    than the size of the item.

    \list
    \o Image.Stretch - the image is scaled to fit
    \o Image.PreserveAspectFit - the image is scaled uniformly to fit without cropping
    \o Image.PreserveAspectCrop - the image is scaled uniformly to fill, cropping if necessary
    \o Image.Tile - the image is duplicated horizontally and vertically
    \o Image.TileVertically - the image is stretched horizontally and tiled vertically
    \o Image.TileHorizontally - the image is stretched vertically and tiled horizontally
    \endlist

    \table
    
    \row
    \o \image declarative-qtlogo-stretch.png
    \o Stretch (default)
    \qml
    Image {
        width: 130; height: 100
        smooth: true
        source: "qtlogo.png"
    }
    \endqml

    \row
    \o \image declarative-qtlogo-preserveaspectfit.png
    \o PreserveAspectFit
    \qml
    Image {
        width: 130; height: 100
        fillMode: Image.PreserveAspectFit
        smooth: true
        source: "qtlogo.png"
    }
    \endqml

    \row
    \o \image declarative-qtlogo-preserveaspectcrop.png
    \o PreserveAspectCrop
    \qml
    Image {
        width: 130; height: 100
        fillMode: Image.PreserveAspectCrop
        smooth: true
        source: "qtlogo.png"
        clip: true
    }
    \endqml

    \row
    \o \image declarative-qtlogo-tile.png
    \o Tile
    \qml
    Image {
        width: 120; height: 120
        fillMode: Image.Tile
        source: "qtlogo.png"
    }
    \endqml

    \row
    \o \image declarative-qtlogo-tilevertically.png
    \o TileVertically
    \qml
    Image {
        width: 120; height: 120
        fillMode: Image.TileVertically
        smooth: true
        source: "qtlogo.png"
    }
    \endqml

    \row
    \o \image declarative-qtlogo-tilehorizontally.png
    \o TileHorizontally
    \qml
    Image {
        width: 120; height: 120
        fillMode: Image.TileHorizontally
        smooth: true
        source: "qtlogo.png"
    }
    \endqml

    \endtable

    \sa {declarative/imageelements/image}{Image example}
*/
QDeclarativeImage::FillMode QDeclarativeImage::fillMode() const
{
    Q_D(const QDeclarativeImage);
    return d->fillMode;
}

void QDeclarativeImage::setFillMode(FillMode mode)
{
    Q_D(QDeclarativeImage);
    if (d->fillMode == mode)
        return;
    d->fillMode = mode;
    update();
    updatePaintedGeometry();
    emit fillModeChanged();
}

/*!

    \qmlproperty real Image::paintedWidth
    \qmlproperty real Image::paintedHeight

    These properties hold the size of the image that is actually painted.
    In most cases it is the same as \c width and \c height, but when using a 
    \c fillMode \c PreserveAspectFit or \c fillMode \c PreserveAspectCrop 
    \c paintedWidth or \c paintedHeight can be smaller or larger than 
    \c width and \c height of the Image element.
*/
qreal QDeclarativeImage::paintedWidth() const
{
    Q_D(const QDeclarativeImage);
    return d->paintedWidth;
}

qreal QDeclarativeImage::paintedHeight() const
{
    Q_D(const QDeclarativeImage);
    return d->paintedHeight;
}

/*!
    \qmlproperty enumeration Image::status

    This property holds the status of image loading.  It can be one of:
    \list
    \o Image.Null - no image has been set
    \o Image.Ready - the image has been loaded
    \o Image.Loading - the image is currently being loaded
    \o Image.Error - an error occurred while loading the image
    \endlist

    Use this status to provide an update or respond to the status change in some way.
    For example, you could:

    \list
    \o Trigger a state change:
    \qml
        State { name: 'loaded'; when: image.status == Image.Ready }
    \endqml

    \o Implement an \c onStatusChanged signal handler:
    \qml
        Image {
            id: image
            onStatusChanged: if (image.status == Image.Ready) console.log('Loaded')
        }
    \endqml

    \o Bind to the status value:
    \qml
        Text { text: image.status == Image.Ready ? 'Loaded' : 'Not loaded' }
    \endqml
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

/*!
    \qmlproperty QSize Image::sourceSize

    This property holds the actual width and height of the loaded image.

    Unlike the \l {Item::}{width} and \l {Item::}{height} properties, which scale
    the painting of the image, this property sets the actual number of pixels
    stored for the loaded image so that large images do not use more
    memory than necessary. For example, this ensures the image in memory is no
    larger than 1024x1024 pixels, regardless of the Image's \l {Item::}{width} and 
    \l {Item::}{height} values:

    \code
    Rectangle {
        width: ...
        height: ...

        Image {
           anchors.fill: parent
           source: "reallyBigImage.jpg"
           sourceSize.width: 1024
           sourceSize.height: 1024
        }
    }
    \endcode

    If the image's actual size is larger than the sourceSize, the image is scaled down.
    If only one dimension of the size is set to greater than 0, the
    other dimension is set in proportion to preserve the source image's aspect ratio.
    (The \l fillMode is independent of this.)

    If the source is an instrinsically scalable image (eg. SVG), this property
    determines the size of the loaded image regardless of intrinsic size.
    Avoid changing this property dynamically; rendering an SVG is \e slow compared
    to an image.

    If the source is a non-scalable image (eg. JPEG), the loaded image will
    be no greater than this property specifies. For some formats (currently only JPEG),
    the whole image will never actually be loaded into memory.
 
    \note \e {Changing this property dynamically causes the image source to be reloaded,
    potentially even from the network, if it is not in the disk cache.}
*/

void QDeclarativeImage::updatePaintedGeometry()
{
    Q_D(QDeclarativeImage);

    if (d->fillMode == PreserveAspectFit) {
        if (!d->pix.width() || !d->pix.height())
            return;
        qreal widthScale = width() / qreal(d->pix.width());
        qreal heightScale = height() / qreal(d->pix.height());
        if (widthScale <= heightScale) {
            d->paintedWidth = width();
            d->paintedHeight = widthScale * qreal(d->pix.height());
        } else if(heightScale < widthScale) {
            d->paintedWidth = heightScale * qreal(d->pix.width());
            d->paintedHeight = height();
        }
        if (widthValid() && !heightValid()) {
            setImplicitHeight(d->paintedHeight);
        }
        if (heightValid() && !widthValid()) {
            setImplicitWidth(d->paintedWidth);
        }
    } else if (d->fillMode == PreserveAspectCrop) {
        if (!d->pix.width() || !d->pix.height())
            return;
        qreal widthScale = width() / qreal(d->pix.width());
        qreal heightScale = height() / qreal(d->pix.height());
        if (widthScale < heightScale) {
            widthScale = heightScale;
        } else if(heightScale < widthScale) {
            heightScale = widthScale;
        }

        d->paintedHeight = heightScale * qreal(d->pix.height());
        d->paintedWidth = widthScale * qreal(d->pix.width());
    } else {
        d->paintedWidth = width();
        d->paintedHeight = height();
    }
    emit paintedGeometryChanged();
}

void QDeclarativeImage::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QDeclarativeImageBase::geometryChanged(newGeometry, oldGeometry);
    updatePaintedGeometry();
}

QRectF QDeclarativeImage::boundingRect() const
{
    Q_D(const QDeclarativeImage);
    return QRectF(0, 0, qMax(d->mWidth, d->paintedWidth), qMax(d->mHeight, d->paintedHeight));
}

/*!
    \qmlproperty url Image::source

    Image can handle any image format supported by Qt, loaded from any URL scheme supported by Qt.

    The URL may be absolute, or relative to the URL of the component.

    \sa QDeclarativeImageProvider
*/

/*!
    \qmlproperty bool Image::asynchronous

    Specifies that images on the local filesystem should be loaded
    asynchronously in a separate thread.  The default value is
    false, causing the user interface thread to block while the
    image is loaded.  Setting \a asynchronous to true is useful where
    maintaining a responsive user interface is more desirable
    than having images immediately visible.

    Note that this property is only valid for images read from the
    local filesystem.  Images loaded via a network resource (e.g. HTTP)
    are always loaded asynchonously.
*/

void QDeclarativeImage::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QDeclarativeImage);
    if (d->pix.pixmap().isNull() )
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    if (width() != d->pix.width() || height() != d->pix.height()) {
        if (d->fillMode >= Tile) {
            if (d->fillMode == Tile) {
                p->drawTiledPixmap(QRectF(0,0,width(),height()), d->pix);
            } else {
                qreal widthScale = width() / qreal(d->pix.width());
                qreal heightScale = height() / qreal(d->pix.height());

                QTransform scale;
                if (d->fillMode == TileVertically) {
                    scale.scale(widthScale, 1.0);
                    QTransform old = p->transform();
                    p->setWorldTransform(scale * old);
                    p->drawTiledPixmap(QRectF(0,0,d->pix.width(),height()), d->pix);
                    p->setWorldTransform(old);
                } else {
                    scale.scale(1.0, heightScale);
                    QTransform old = p->transform();
                    p->setWorldTransform(scale * old);
                    p->drawTiledPixmap(QRectF(0,0,width(),d->pix.height()), d->pix);
                    p->setWorldTransform(old);
                }
            }
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
                p->setClipRect(QRectF(0, 0, d->mWidth, d->mHeight), Qt::IntersectClip);
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

void QDeclarativeImage::pixmapChange()
{
    updatePaintedGeometry();
}

QT_END_NAMESPACE
