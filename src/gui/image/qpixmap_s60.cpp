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
