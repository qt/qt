/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qpixmapfilter_vg_p.h"
#include <QtCore/qvarlengtharray.h>
#include <QtGui/qpainter.h>

QT_BEGIN_NAMESPACE

#if !defined(QT_SHIVAVG)

QVGPixmapConvolutionFilter::QVGPixmapConvolutionFilter()
    : QPixmapConvolutionFilter()
{
}

QVGPixmapConvolutionFilter::~QVGPixmapConvolutionFilter()
{
}

extern void qt_vg_drawVGImage
    (QPainter *painter, const QPointF& pos, VGImage vgImg);

void QVGPixmapConvolutionFilter::draw
        (QPainter *painter, const QPointF &dest,
         const QPixmap &src, const QRectF &srcRect) const
{
    if (src.pixmapData()->classId() != QPixmapData::OpenVGClass) {
        // The pixmap data is not an instance of QVGPixmapData, so fall
        // back to the default convolution filter implementation.
        QPixmapConvolutionFilter::draw(painter, dest, src, srcRect);
        return;
    }

    QVGPixmapData *pd = static_cast<QVGPixmapData *>(src.pixmapData());

    VGImage srcImage = pd->toVGImage();
    if (srcImage == VG_INVALID_HANDLE)
        return;

    QSize size = pd->size();
    VGImage dstImage = vgCreateImage
        (VG_sARGB_8888_PRE, size.width(), size.height(),
         VG_IMAGE_QUALITY_FASTER);
    if (dstImage == VG_INVALID_HANDLE)
        return;

    int kernelWidth = rows();
    int kernelHeight = columns();
    const qreal *kern = convolutionKernel();
    QVarLengthArray<VGshort> kernel;
    for (int i = 0; i < kernelWidth; ++i) {
        for (int j = 0; j < kernelHeight; ++j) {
            kernel.append((VGshort)(kern[j * kernelWidth + i] * 1024.0f));
        }
    }

    VGfloat values[4];
    values[0] = 0.0f;
    values[1] = 0.0f;
    values[2] = 0.0f;
    values[3] = 0.0f;
    vgSetfv(VG_TILE_FILL_COLOR, 4, values);

    vgConvolve(dstImage, srcImage,
               kernelWidth, kernelHeight, 0, 0,
               kernel.constData(), 1.0f / 1024.0f, 0.0f,
               VG_TILE_FILL);

    VGImage child = VG_INVALID_HANDLE;

    if (srcRect.isNull() ||
        (srcRect.topLeft().isNull() && srcRect.size() == size)) {
        child = dstImage;
    } else {
        QRect src = srcRect.toRect();
        child = vgChildImage(dstImage, src.x(), src.y(), src.width(), src.height());
    }

    qt_vg_drawVGImage(painter, dest, child);

    if(child != dstImage)
        vgDestroyImage(child);
    vgDestroyImage(dstImage);
}

QVGPixmapColorizeFilter::QVGPixmapColorizeFilter()
    : QPixmapColorizeFilter(),
      firstTime(true)
{
}

QVGPixmapColorizeFilter::~QVGPixmapColorizeFilter()
{
}

void QVGPixmapColorizeFilter::draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect) const
{
    if (src.pixmapData()->classId() != QPixmapData::OpenVGClass) {
        // The pixmap data is not an instance of QVGPixmapData, so fall
        // back to the default convolution filter implementation.
        QPixmapColorizeFilter::draw(painter, dest, src, srcRect);
        return;
    }

    QVGPixmapData *pd = static_cast<QVGPixmapData *>(src.pixmapData());

    VGImage srcImage = pd->toVGImage();
    if (srcImage == VG_INVALID_HANDLE)
        return;

    QSize size = pd->size();
    VGImage dstImage = vgCreateImage
        (VG_sARGB_8888_PRE, size.width(), size.height(),
         VG_IMAGE_QUALITY_FASTER);
    if (dstImage == VG_INVALID_HANDLE)
        return;

    // Recompute the color matrix if the color has changed.
    QColor c = color();
    if (c != prevColor || firstTime) {
        prevColor = c;

        // Determine the weights for the matrix from the color.
        VGfloat weights[3];
        VGfloat invweights[3];
        VGfloat alpha = c.alphaF();
        weights[0] = c.redF() * alpha;
        weights[1] = c.greenF() * alpha;
        weights[2] = c.blueF() * alpha;
        invweights[0] = 1.0f - weights[0];
        invweights[1] = 1.0f - weights[1];
        invweights[2] = 1.0f - weights[2];

        // Grayscale weights.
        static const VGfloat redGray = 11.0f / 32.0f;
        static const VGfloat greenGray = 16.0f / 32.0f;
        static const VGfloat blueGray = 1.0f - (redGray + greenGray);

        matrix[0][0] = redGray * invweights[0];
        matrix[0][1] = redGray * invweights[1];
        matrix[0][2] = redGray * invweights[2];
        matrix[0][3] = 0.0f;
        matrix[1][0] = greenGray * invweights[0];
        matrix[1][1] = greenGray * invweights[1];
        matrix[1][2] = greenGray * invweights[2];
        matrix[1][3] = 0.0f;
        matrix[2][0] = blueGray * invweights[0];
        matrix[2][1] = blueGray * invweights[1];
        matrix[2][2] = blueGray * invweights[2];
        matrix[2][3] = 0.0f;
        matrix[3][0] = 0.0f;
        matrix[3][1] = 0.0f;
        matrix[3][2] = 0.0f;
        matrix[3][3] = 1.0f;
        matrix[4][0] = weights[0];
        matrix[4][1] = weights[1];
        matrix[4][2] = weights[2];
        matrix[4][3] = 0.0f;
    }

    firstTime = false;

    vgColorMatrix(dstImage, srcImage, matrix[0]);

    VGImage child = VG_INVALID_HANDLE;

    if (srcRect.isNull() ||
        (srcRect.topLeft().isNull() && srcRect.size() == size)) {
        child = dstImage;
    } else {
        QRect src = srcRect.toRect();
        child = vgChildImage(dstImage, src.x(), src.y(), src.width(), src.height());
    }

    qt_vg_drawVGImage(painter, dest, child);

    if(child != dstImage)
        vgDestroyImage(child);
    vgDestroyImage(dstImage);
}

QVGPixmapDropShadowFilter::QVGPixmapDropShadowFilter()
    : QPixmapDropShadowFilter(),
      prevRadius(0.0f),
      kernelSize(0),
      firstTime(true)
{
}

QVGPixmapDropShadowFilter::~QVGPixmapDropShadowFilter()
{
}

void QVGPixmapDropShadowFilter::draw(QPainter *painter, const QPointF &dest, const QPixmap &src, const QRectF &srcRect) const
{
    if (src.pixmapData()->classId() != QPixmapData::OpenVGClass) {
        // The pixmap data is not an instance of QVGPixmapData, so fall
        // back to the default drop shadow filter implementation.
        QPixmapDropShadowFilter::draw(painter, dest, src, srcRect);
        return;
    }

    QVGPixmapData *pd = static_cast<QVGPixmapData *>(src.pixmapData());

    VGImage srcImage = pd->toVGImage();
    if (srcImage == VG_INVALID_HANDLE)
        return;

    QSize size = pd->size();
    VGImage tmpImage = vgCreateImage
        (VG_sARGB_8888_PRE, size.width(), size.height(),
         VG_IMAGE_QUALITY_FASTER);
    if (tmpImage == VG_INVALID_HANDLE)
        return;

    VGImage dstImage = vgCreateImage
        (VG_sARGB_8888_PRE, size.width(), size.height(),
         VG_IMAGE_QUALITY_FASTER);
    if (dstImage == VG_INVALID_HANDLE) {
        vgDestroyImage(tmpImage);
        return;
    }

    // Recompute the color matrix if the color has changed.
    QColor c = color();
    if (c != prevColor || firstTime) {
        prevColor = c;

        matrix[0][0] = 0.0f;
        matrix[0][1] = 0.0f;
        matrix[0][2] = 0.0f;
        matrix[0][3] = 0.0f;
        matrix[1][0] = 0.0f;
        matrix[1][1] = 0.0f;
        matrix[1][2] = 0.0f;
        matrix[1][3] = 0.0f;
        matrix[2][0] = 0.0f;
        matrix[2][1] = 0.0f;
        matrix[2][2] = 0.0f;
        matrix[2][3] = 0.0f;
        matrix[3][0] = c.redF();
        matrix[3][1] = c.greenF();
        matrix[3][2] = c.blueF();
        matrix[3][3] = c.alphaF();
        matrix[4][0] = 0.0f;
        matrix[4][1] = 0.0f;
        matrix[4][2] = 0.0f;
        matrix[4][3] = 0.0f;
    }

    // Blacken the source image.
    vgColorMatrix(tmpImage, srcImage, matrix[0]);

    // Recompute the convolution kernel if the blur radius has changed.
    qreal radius = blurRadius();
    if (radius != prevRadius || firstTime) {
        prevRadius = radius;
        int dim = 2 * qRound(radius) + 1;
        int size = dim * dim;
        VGshort f = VGshort(1024.0f / size);
        kernel.resize(size);
        for (int i = 0; i < size; ++i)
            kernel[i] = f;
        kernelSize = dim;
    }

    // Apply the convolution filter using the kernel.
    VGfloat values[4];
    values[0] = 0.0f;
    values[1] = 0.0f;
    values[2] = 0.0f;
    values[3] = 0.0f;
    vgSetfv(VG_TILE_FILL_COLOR, 4, values);
    vgConvolve(dstImage, tmpImage,
               kernelSize, kernelSize, 0, 0,
               kernel.constData(), 1.0f / 1024.0f, 0.0f,
               VG_TILE_FILL);

    firstTime = false;

    VGImage child = VG_INVALID_HANDLE;

    if (srcRect.isNull() ||
        (srcRect.topLeft().isNull() && srcRect.size() == size)) {
        child = dstImage;
    } else {
        QRect src = srcRect.toRect();
        child = vgChildImage(dstImage, src.x(), src.y(), src.width(), src.height());
    }

    qt_vg_drawVGImage(painter, dest + offset(), child);

    if(child != dstImage)
        vgDestroyImage(child);
    vgDestroyImage(tmpImage);
    vgDestroyImage(dstImage);
}

#endif

QT_END_NAMESPACE
