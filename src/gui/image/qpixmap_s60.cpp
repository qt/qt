/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui of the Qt Toolkit.
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
#include <exception>
#include <w32std.h>
#include <fbs.h>

#include <private/qt_s60_p.h>
#include "qpixmap.h"
#include "qpixmap_raster_p.h"
#include <qwidget.h>

QT_BEGIN_NAMESPACE

QPixmap QPixmap::grabWindow(WId winId, int x, int y, int w, int h )
{
    CWsScreenDevice* screenDevice = S60->screenDevice();
    TSize screenSize = screenDevice->SizeInPixels();

    TSize srcSize;
    // Find out if this is one of our windows.
    QSymbianControl *sControl;
    sControl = winId->MopGetObject(sControl);
    if (sControl && sControl->widget()->windowType() == Qt::Desktop) {
        // Grabbing desktop widget
        srcSize = screenSize;
    } else {
        TPoint relativePos = winId->PositionRelativeToScreen();
        x += relativePos.iX;
        y += relativePos.iY;
        srcSize = winId->Size();
    }

    TRect srcRect(TPoint(x, y), srcSize);
    // Clip to the screen
    srcRect.Intersection(TRect(screenSize));

    if (w > 0 && h > 0) {
        TRect subRect(TPoint(x, y), TSize(w, h));
        // Clip to the subRect
        srcRect.Intersection(subRect);
    }

    if (srcRect.IsEmpty())
        return QPixmap();

    TDisplayMode displayMode = screenDevice->DisplayMode();
    CFbsBitmap* temporary = q_check_ptr(new CFbsBitmap());		// CBase derived object needs check on new
    TInt error = temporary->Create(srcRect.Size(), displayMode);
    if (error == KErrNone)
        error = screenDevice->CopyScreenToBitmap(temporary, srcRect);

    if (error != KErrNone) {
        CBase::Delete(temporary);
        return QPixmap();
    }

    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(temporary);
    CBase::Delete(temporary);
    return pixmap;
}

/*!
\since 4.6

Returns a \c CFbsBitmap that is equivalent to the QPixmap by copying the data.

It is the caller's responsibility to delete the \c CFbsBitmap after use.

\warning This function is only available on Symbian OS.

\sa fromSymbianCFbsBitmap()
*/

CFbsBitmap *QPixmap::toSymbianCFbsBitmap() const
{
    if (isNull())
        return 0;

    TDisplayMode mode;
    const QImage img = toImage();
    QImage::Format destFormat = img.format();
    switch (img.format()) {
    case QImage::Format_Mono:
        destFormat = QImage::Format_MonoLSB;
        // Fall through intended
    case QImage::Format_MonoLSB:
        mode = EGray2;
        break;
    case QImage::Format_Indexed8:
        if (img.isGrayscale())
            mode = EGray256;
        else
            mode = EColor256;
        break;
    case QImage::Format_RGB32:
        mode = EColor16MU;
        break;
    case QImage::Format_ARGB6666_Premultiplied:
    case QImage::Format_ARGB8565_Premultiplied:
    case QImage::Format_ARGB8555_Premultiplied:
        destFormat = QImage::Format_ARGB32_Premultiplied;
        // Fall through intended
    case QImage::Format_ARGB32_Premultiplied:
#if !defined(__SERIES60_31__) && !defined(__S60_32__)
        // ### TODO: Add runtime detection as well?
        mode = EColor16MAP;
            break;
#endif
        destFormat = QImage::Format_ARGB32;
        // Fall through intended
    case QImage::Format_ARGB32:
        mode = EColor16MA;
        break;
    case QImage::Format_RGB555:
        destFormat = QImage::Format_RGB16;
        // Fall through intended
    case QImage::Format_RGB16:
        mode = EColor64K;
        break;
    case QImage::Format_RGB666:
        destFormat = QImage::Format_RGB888;
        // Fall through intended
    case QImage::Format_RGB888:
        mode = EColor16M;
        break;
    case QImage::Format_RGB444:
        mode = EColor4K;
        break;
    case QImage::Format_Invalid:
        return 0;
    default:
        qWarning("Image format not supported: %d", img.format());
        return 0;
    }

    CFbsBitmap* bitmap = q_check_ptr(new CFbsBitmap());		// CBase derived object needs check on new
    TSize size(width(), height());
    if (bitmap->Create(size, mode) != KErrNone) {
        CBase::Delete(bitmap);
        return 0;
    }

    const QImage converted = img.convertToFormat(destFormat);

    bitmap->LockHeap();
    const uchar *sptr = converted.bits();
    uchar *dptr = (uchar*)bitmap->DataAddress();
    Mem::Copy(dptr, sptr, converted.numBytes());
    bitmap->UnlockHeap();
    return bitmap;
}

/*!
\since 4.6

Returns a QPixmap that is equivalent to the \c CFbsBitmap by copying the data.
If the CFbsBitmap is not valid or is compressed in memory, this function will
return a null QPixmap.

\warning This function is only available on Symbian OS.

\sa toSymbianCFbsBitmap(), {QPixmap#Pixmap Conversion}{Pixmap Conversion}
*/

QPixmap QPixmap::fromSymbianCFbsBitmap(CFbsBitmap *bitmap)
{
    int width = bitmap->SizeInPixels().iWidth;
    int height = bitmap->SizeInPixels().iHeight;

    if (!bitmap || width <= 0 || height <= 0 || bitmap->IsCompressedInRAM())
        return QPixmap();

    TDisplayMode displayMode = bitmap->DisplayMode();
    QImage::Format format = qt_TDisplayMode2Format(displayMode);
    int bytesPerLine = CFbsBitmap::ScanLineLength(width, displayMode);
    bitmap->LockHeap();
    QImage image = QImage((const uchar*)bitmap->DataAddress(), width, height, bytesPerLine, format);
    if (displayMode == EGray2) {
        image.setNumColors(2);
        image.setColor(0, QColor(Qt::color0).rgba());
        image.setColor(1, QColor(Qt::color1).rgba());
    } else if (displayMode == EGray256) {
        for (int i=0; i < 256; ++i)
            image.setColor(i, qRgb(i, i, i));
    }else if (displayMode == EColor256) {
        const TColor256Util *palette = TColor256Util::Default();
        for (int i=0; i < 256; ++i)
            image.setColor(i, (QRgb)(palette->Color256(i).Value()));
    }
    QPixmap pixmap = QPixmap::fromImage(image.copy());
    bitmap->UnlockHeap();
    return pixmap;
}

QT_END_NAMESPACE
