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

#include <QKeyEvent>
#include <QPainter>

QT_BEGIN_NAMESPACE


QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Image,QFxImage)

/*!
    \qmlclass Image QFxImage
    \brief The Image element allows you to add bitmaps to a scene.
    \inherits Item

    The Image element supports untransformed, stretched, tiled, and grid-scaled images.

    For an explanation of stretching and tiling, see the fillMode property description.

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
    \o fillMode: Stretch (default)
    \qml
    Image {
        width: 160
        height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo4.png
    \o Grid-scaled (only with fillMode: Stretch)
    \qml
    Image { scaleGrid.left: 20; scaleGrid.right: 10
            scaleGrid.top: 14; scaleGrid.bottom: 14
            width: 160; height: 160
            source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo3.png
    \o fillMode: Tile
    \qml
    Image {
        fillMode: "Tile"
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo6.png
    \o fillMode: TileVertically
    \qml
    Image {
        fillMode: "TileVertically"
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
    \endqml
    \row
    \o \image declarative-qtlogo5.png
    \o fillMode: TileHorizontally
    \qml
    Image {
        fillMode: "TileHorizontally"
        width: 160; height: 160
        source: "pics/qtlogo.png"
    }
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
  : QFxImageBase(*(new QFxImagePrivate), parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxImage::QFxImage(QFxImagePrivate &dd, QFxItem *parent)
  : QFxImageBase(dd, parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxImage::~QFxImage()
{
}

/*!
    \property QFxImage::image
    \brief the image displayed in this item.

    This property contains the image currently being displayed by this item,
    which may be empty if nothing is currently displayed. Setting the source
    property overrides any setting of this property.
*/
QPixmap QFxImage::pixmap() const
{
    Q_D(const QFxImage);
    return d->pix;
}

void QFxImage::setPixmap(const QPixmap &pix)
{
    Q_D(QFxImage);
    if (!d->url.isEmpty())
        return;
    d->pix = pix;

    setImplicitWidth(d->pix.width());
    setImplicitHeight(d->pix.height());

    update();
}

/*!
    \qmlproperty FillMode Image::fillMode

    Set this property to define what happens when the image set for the item is smaller
    than the size of the item.

    \list
    \o Stretch - the image is scaled to fit
    \o PreserveAspect - the image is scaled uniformly to fit
    \o Tile - the image is duplicated horizontally and vertically
    \o TileVertically - the image is stretched horizontally and tiled vertically
    \o TileHorizontally - the image is stretched vertically and tiled horizontally
    \endlist

    \image declarative-image_fillMode.gif

    Only fillMode: Stretch can be used with scaleGrid. Other settings
    will cause a run-time warning.

    \sa examples/declarative/aspectratio
*/
QFxImage::FillMode QFxImage::fillMode() const
{
    Q_D(const QFxImage);
    return d->fillMode;
}

void QFxImage::setFillMode(FillMode mode)
{
    Q_D(QFxImage);
    if (d->fillMode == mode)
        return;
    d->fillMode = mode;
    update();
    emit fillModeChanged();
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
    \qmlproperty url Image::source

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

void QFxImage::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_D(QFxImage);
    if (d->pix.isNull())
        return;

    bool oldAA = p->testRenderHint(QPainter::Antialiasing);
    bool oldSmooth = p->testRenderHint(QPainter::SmoothPixmapTransform);
    if (d->smooth)
        p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, d->smooth);

    QPixmap pix = d->pix;

    if (!d->scaleGrid || d->scaleGrid->isNull()) {
        if (width() != pix.width() || height() != pix.height()) {
            if (d->fillMode >= Tile) {
                p->save();
                p->setClipRect(0, 0, width(), height(), Qt::IntersectClip);

                if (d->fillMode == Tile) {
                    const int pw = pix.width();
                    const int ph = pix.height();
                    int yy = 0;

                    while(yy < height()) {
                        int xx = 0;
                        while(xx < width()) {
                            p->drawPixmap(xx, yy, pix);
                            xx += pw;
                        }
                        yy += ph;
                    }
                } else if (d->fillMode == TileVertically) {
                    const int ph = pix.height();
                    int yy = 0;

                    while(yy < height()) {
                        p->drawPixmap(QRect(0, yy, width(), ph), pix);
                        yy += ph;
                    }
                } else {
                    const int pw = pix.width();
                    int xx = 0;

                    while(xx < width()) {
                        p->drawPixmap(QRect(xx, 0, pw, height()), pix);
                        xx += pw;
                    }
                }

                p->restore();
            } else {
                qreal widthScale = width() / qreal(pix.width());
                qreal heightScale = height() / qreal(pix.height());

                QTransform scale;

                if (d->fillMode == PreserveAspect) {
                    if (widthScale < heightScale) {
                        heightScale = widthScale;
                        scale.translate(0, (height() - heightScale * pix.height()) / 2);
                    } else if(heightScale < widthScale) {
                        widthScale = heightScale;
                        scale.translate((width() - widthScale * pix.width()) / 2, 0);
                    }
                }

                scale.scale(widthScale, heightScale);
                QTransform old = p->transform();
                p->setWorldTransform(scale * old);
                p->drawPixmap(0, 0, pix);
                p->setWorldTransform(old);
            }
        } else {
            p->drawPixmap(0, 0, pix);
        }
    } else {
        QMargins margins(d->scaleGrid->top(), d->scaleGrid->left(), d->scaleGrid->bottom(), d->scaleGrid->right());
        QTileRules rules((Qt::TileRule)d->scaleGrid->horizontalTileRule(),
                         (Qt::TileRule)d->scaleGrid->verticalTileRule());
        qDrawBorderPixmap(p, QRect(0, 0, (int)d->width, (int)d->height), margins, pix, pix.rect(), margins, rules);
    }

    if (d->smooth) {
        p->setRenderHint(QPainter::Antialiasing, oldAA);
        p->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    }
}

QT_END_NAMESPACE
