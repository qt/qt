/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/
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
    if (sControl && sControl->widget()->windowType() & Qt::Desktop) {
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
    CFbsBitmap* temporary = new (ELeave) CFbsBitmap();
    TInt error = temporary->Create(srcRect.Size(), displayMode);
    if (error == KErrNone)
        error = screenDevice->CopyScreenToBitmap(temporary, srcRect);

    if (error != KErrNone) {
        CBase::Delete(temporary);
        return QPixmap();
    }

    QImage::Format format = qt_TDisplayMode2Format( displayMode );
    int bytesPerLine = CFbsBitmap::ScanLineLength(temporary->SizeInPixels().iWidth,displayMode);
    temporary->LockHeap();
    QImage image = QImage((uchar*)temporary->DataAddress(), srcRect.Width(), srcRect.Height(), bytesPerLine, format);
    QPixmap pixmap = QPixmap::fromImage(image.copy());
    temporary->UnlockHeap();
    CBase::Delete(temporary);
    return pixmap;

}
QT_END_NAMESPACE
