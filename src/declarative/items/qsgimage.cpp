// Commit: 695a39410c8ce186a2ce78cef51093c55fc32643
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

#include "qsgimage_p.h"
#include "qsgimage_p_p.h"

#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>

#include <QtGui/qpainter.h>

QT_BEGIN_NAMESPACE

QSGImagePrivate::QSGImagePrivate()
    : fillMode(QSGImage::Stretch)
    , paintedWidth(0)
    , paintedHeight(0)
    , pixmapChanged(false)
{
}

QSGImage::QSGImage(QSGItem *parent)
    : QSGImageBase(*(new QSGImagePrivate), parent)
{
}

QSGImage::QSGImage(QSGImagePrivate &dd, QSGItem *parent)
    : QSGImageBase(dd, parent)
{
}

QSGImage::~QSGImage()
{
}

void QSGImagePrivate::setPixmap(const QPixmap &pixmap)
{
    Q_Q(QSGImage);
    pix.setPixmap(pixmap);

    q->setImplicitWidth(pix.width());
    q->setImplicitHeight(pix.height());
    status = pix.isNull() ? QSGImageBase::Null : QSGImageBase::Ready;

    q->update();
    q->pixmapChange();
}

QSGImage::FillMode QSGImage::fillMode() const
{
    Q_D(const QSGImage);
    return d->fillMode;
}

void QSGImage::setFillMode(FillMode mode)
{
    Q_D(QSGImage);
    if (d->fillMode == mode)
        return;
    d->fillMode = mode;
    update();
    updatePaintedGeometry();
    emit fillModeChanged();
}

qreal QSGImage::paintedWidth() const
{
    Q_D(const QSGImage);
    return d->paintedWidth;
}

qreal QSGImage::paintedHeight() const
{
    Q_D(const QSGImage);
    return d->paintedHeight;
}

void QSGImage::updatePaintedGeometry()
{
    Q_D(QSGImage);

    if (d->fillMode == PreserveAspectFit) {
        if (!d->pix.width() || !d->pix.height())
            return;
        qreal w = widthValid() ? width() : d->pix.width();
        qreal widthScale = w / qreal(d->pix.width());
        qreal h = heightValid() ? height() : d->pix.height();
        qreal heightScale = h / qreal(d->pix.height());
        if (widthScale <= heightScale) {
            d->paintedWidth = w;
            d->paintedHeight = widthScale * qreal(d->pix.height());
        } else if(heightScale < widthScale) {
            d->paintedWidth = heightScale * qreal(d->pix.width());
            d->paintedHeight = h;
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

void QSGImage::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QSGImageBase::geometryChanged(newGeometry, oldGeometry);
    updatePaintedGeometry();
}

QRectF QSGImage::boundingRect() const
{
    Q_D(const QSGImage);
    return QRectF(0, 0, qMax(width(), d->paintedWidth), qMax(height(), d->paintedHeight));
}

QSGTexture *QSGImage::texture() const
{
    Q_D(const QSGImage);
    QSGTexture *t = d->pix.texture();
    t->setFiltering(QSGItemPrivate::get(this)->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
    t->setMipmapFiltering(QSGTexture::None);
    t->setHorizontalWrapMode(QSGTexture::ClampToEdge);
    t->setVerticalWrapMode(QSGTexture::ClampToEdge);
    return t;
}

QSGNode *QSGImage::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    Q_D(QSGImage);
    //XXX Support mirror property

    if (!d->pix.texture() || width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGImageNode *node = static_cast<QSGImageNode *>(oldNode);
    if (!node) { 
        d->pixmapChanged = true;
        node = d->sceneGraphContext()->createImageNode();
        node->setTexture(d->pix.texture());
    }

    if (d->pixmapChanged) {
        // force update the texture in the node to trigger reconstruction of
        // geometry and the likes when a atlas segment has changed.
        QSGTexture *t = d->pix.texture();
        node->setTexture(0);
        node->setTexture(t);
        d->pixmapChanged = false;
    }

    QRectF targetRect;
    QRectF sourceRect;
    QSGTexture::WrapMode hWrap = QSGTexture::ClampToEdge;
    QSGTexture::WrapMode vWrap = QSGTexture::ClampToEdge;

    switch (d->fillMode) {
    default:
    case Stretch:
        targetRect = QRectF(0, 0, width(), height());
        sourceRect = d->pix.rect();
        break;

    case PreserveAspectFit:
        targetRect = QRectF((width() - d->paintedWidth) / 2., (height() - d->paintedHeight) / 2.,
                            d->paintedWidth, d->paintedHeight);
        sourceRect = d->pix.rect();
        break;

    case PreserveAspectCrop: {
        targetRect = QRect(0, 0, width(), height());
        qreal wscale = width() / qreal(d->pix.width());
        qreal hscale = height() / qreal(d->pix.height());

        if (wscale > hscale) {
            int src = (hscale / wscale) * qreal(d->pix.height());
            sourceRect = QRectF(0, (d->pix.height() - src) / 2, d->pix.width(), src);
        } else {
            int src = (wscale / hscale) * qreal(d->pix.width());
            sourceRect = QRectF((d->pix.width() - src) / 2, 0, src, d->pix.height());
        }
    }
        break;

    case Tile:
        targetRect = QRectF(0, 0, width(), height());
        sourceRect = QRectF(0, 0, width(), height());
        hWrap = QSGTexture::Repeat;
        vWrap = QSGTexture::Repeat;
        break;

    case TileHorizontally:
        targetRect = QRectF(0, 0, width(), height());
        sourceRect = QRectF(0, 0, width(), d->pix.height());
        hWrap = QSGTexture::Repeat;
        break;

    case TileVertically:
        targetRect = QRectF(0, 0, width(), height());
        sourceRect = QRectF(0, 0, d->pix.width(), height());
        vWrap = QSGTexture::Repeat;
        break;

    };

    QRectF nsrect(sourceRect.x() / d->pix.width(),
                  1 - sourceRect.y() / d->pix.height(),
                  sourceRect.width() / d->pix.width(),
                  -sourceRect.height() / d->pix.height());

    node->setHorizontalWrapMode(hWrap);
    node->setVerticalWrapMode(vWrap);
    node->setFiltering(d->smooth ? QSGTexture::Linear : QSGTexture::Nearest);

    node->setTargetRect(targetRect);
    node->setSourceRect(nsrect);
    node->update();

    return node;
}

void QSGImage::pixmapChange()
{
    Q_D(QSGImage);

    updatePaintedGeometry();
    d->pixmapChanged = true;
}

QT_END_NAMESPACE
