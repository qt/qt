/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
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

#include <qglobal.h> // for Q_WS_WIN define (non-PCH)

#include <QtGui/qpaintdevice.h>
#include <private/qwidget_p.h>
#include "qwindowsurface_s60_p.h"
#include "qt_s60_p.h"
#include "private/qdrawhelper_p.h"

QT_BEGIN_NAMESPACE

struct QS60WindowSurfacePrivate
{
    QImage device;
    CFbsBitmap *bitmap;
    uchar* bytes;

    // Since only one CFbsBitmap is allowed to be locked at a time, this is static.
    static QS60WindowSurface* lockedSurface;
};
QS60WindowSurface* QS60WindowSurfacePrivate::lockedSurface = NULL;

QS60WindowSurface::QS60WindowSurface(QWidget* widget)
    : QWindowSurface(widget), d_ptr(new QS60WindowSurfacePrivate)
{
    d_ptr->bytes = 0;
    d_ptr->bitmap = 0;

    TDisplayMode mode = S60->screenDevice()->DisplayMode();
    bool isOpaque = qt_widget_private(widget)->isOpaque;
    if (mode == EColor16MA && isOpaque)
        mode = EColor16MU; // Faster since 16MU -> 16MA is typically accelerated
    else if (mode == EColor16MU && !isOpaque)
        mode = EColor16MA; // Try for transparency anyway


    // We create empty CFbsBitmap here -> it will be resized in setGeometry
    d_ptr->bitmap = q_check_ptr(new CFbsBitmap);	// CBase derived object needs check on new
    qt_throwIfError( d_ptr->bitmap->Create(TSize(0, 0), mode ) );

    updatePaintDeviceOnBitmap();

    setStaticContentsSupport(true);
}

QS60WindowSurface::~QS60WindowSurface()
{
    // Ensure that locking and unlocking of this surface were symmetrical
    Q_ASSERT(QS60WindowSurfacePrivate::lockedSurface != this);

    delete d_ptr->bitmap;
    delete d_ptr;
}

void QS60WindowSurface::beginPaint(const QRegion &rgn)
{
    if(!d_ptr->bitmap)
        return;

    Q_ASSERT(!QS60WindowSurfacePrivate::lockedSurface);
    QS60WindowSurfacePrivate::lockedSurface = this;
    lockBitmapHeap();

    if (!qt_widget_private(window())->isOpaque) {
        QRgb *data = reinterpret_cast<QRgb *>(d_ptr->device.bits());
        const int row_stride = d_ptr->device.bytesPerLine() / 4;

        const QVector<QRect> rects = rgn.rects();
        for (QVector<QRect>::const_iterator it = rects.begin(); it != rects.end(); ++it) {
            const int x_start = it->x();
            const int width = it->width();

            const int y_start = it->y();
            const int height = it->height();

            QRgb *row = data + row_stride * y_start;
            for (int y = 0; y < height; ++y) {
                qt_memfill(row + x_start, 0U, width);
                row += row_stride;
            }
        }
    }
}

void QS60WindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &)
{
    const QVector<QRect> subRects = region.rects();
    for (int i = 0; i < subRects.count(); ++i) {
        TRect tr = qt_QRect2TRect(subRects[i]);
        widget->winId()->DrawNow(tr);
    }
}

bool QS60WindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    QRect rect = area.boundingRect();

    if (dx == 0 && dy == 0)
        return false;

    if (d_ptr->device.isNull())
        return false;

    CFbsBitmapDevice *bitmapDevice = 0;
    QT_TRAP_THROWING(bitmapDevice = CFbsBitmapDevice::NewL(d_ptr->bitmap));
    CBitmapContext *bitmapContext;
    TInt err = bitmapDevice->CreateBitmapContext(bitmapContext);
    if (err != KErrNone) {
        CBase::Delete(bitmapDevice);
        return false;
    }
    bitmapContext->CopyRect(TPoint(dx, dy), qt_QRect2TRect(rect));
    CBase::Delete(bitmapContext);
    CBase::Delete(bitmapDevice);
    return true;
}

void QS60WindowSurface::endPaint(const QRegion & /* rgn */)
{
    if(!d_ptr->bitmap)
        return;

    Q_ASSERT(QS60WindowSurfacePrivate::lockedSurface);
    unlockBitmapHeap();
    QS60WindowSurfacePrivate::lockedSurface = NULL;
}

QPaintDevice* QS60WindowSurface::paintDevice()
{
    return &d_ptr->device;
}

void QS60WindowSurface::setGeometry(const QRect& rect)
{
    if (rect == geometry())
        return;

    QWindowSurface::setGeometry(rect);

    TRect nativeRect(qt_QRect2TRect(rect));
    qt_throwIfError(d_ptr->bitmap->Resize(nativeRect.Size()));

    if (!rect.isNull())
        updatePaintDeviceOnBitmap();
}

void QS60WindowSurface::lockBitmapHeap()
{
    if (!QS60WindowSurfacePrivate::lockedSurface)
        return;

    // Get some local variables to make later code lines more clear to read
    CFbsBitmap*& bitmap = QS60WindowSurfacePrivate::lockedSurface->d_ptr->bitmap;
    QImage& device = QS60WindowSurfacePrivate::lockedSurface->d_ptr->device;
    uchar*& bytes = QS60WindowSurfacePrivate::lockedSurface->d_ptr->bytes;

    bitmap->LockHeap();
    uchar *newBytes = (uchar*)bitmap->DataAddress();
    if (newBytes != bytes) {
        bytes = newBytes;

        // Get some values for QImage creation
        TDisplayMode mode  = bitmap->DisplayMode();
        if (mode == EColor16MA
            && qt_widget_private(QS60WindowSurfacePrivate::lockedSurface->window())->isOpaque)
            mode = EColor16MU;
        QImage::Format format = qt_TDisplayMode2Format( mode );
        TSize bitmapSize = bitmap->SizeInPixels();
        int bytesPerLine = CFbsBitmap::ScanLineLength( bitmapSize.iWidth, mode);

        device = QImage( bytes, bitmapSize.iWidth, bitmapSize.iHeight, bytesPerLine, format );
    }
}

void QS60WindowSurface::unlockBitmapHeap()
{
    if (!QS60WindowSurfacePrivate::lockedSurface)
        return;

    QS60WindowSurfacePrivate::lockedSurface->d_ptr->bitmap->UnlockHeap();
}

void QS60WindowSurface::updatePaintDeviceOnBitmap()
{
    // This forces the actual device to be updated based on CFbsBitmap
    beginPaint(QRegion());
    endPaint(QRegion());
}

CFbsBitmap *QS60WindowSurface::symbianBitmap() const
{
    return d_ptr->bitmap;
}

QT_END_NAMESPACE
