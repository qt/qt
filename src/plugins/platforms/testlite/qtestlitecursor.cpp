/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#include "qtestlitecursor.h"

#include "qtestliteintegration.h"
#include "qtestlitescreen.h"
#include "qtestlitewindow.h"

#include <QtGui/QBitmap>

#include <X11/cursorfont.h>

QT_BEGIN_NAMESPACE

QTestLiteCursor::QTestLiteCursor(QTestLiteScreen *screen)
    : QPlatformCursor(screen)
{
}

void QTestLiteCursor::changeCursor(QCursor *cursor, QWidget *widget)
{
    QTestLiteWindow *w = 0;
    if (widget) {
        QWidget *window = widget->window();
        w = static_cast<QTestLiteWindow*>(window->platformWindow());
    } else {
        // No X11 cursor control when there is no widget under the cursor
        return;
    }

    if (!w)
        return;

    int id = cursor->handle();

    Cursor c;
    if (!cursorMap.contains(id)) {
        if (cursor->shape() == Qt::BitmapCursor)
            c = createCursorBitmap(cursor);
        else
            c = createCursorShape(cursor->shape());
        if (!c) {
            return;
        }
        cursorMap.insert(id, c);
    } else {
        c = cursorMap.value(id);
    }

    w->setCursor(c);
}

Cursor QTestLiteCursor::createCursorBitmap(QCursor * cursor)
{
    XColor bg, fg;
    bg.red   = 255 << 8;
    bg.green = 255 << 8;
    bg.blue  = 255 << 8;
    fg.red   = 0;
    fg.green = 0;
    fg.blue  = 0;
    QPoint spot = cursor->hotSpot();
    Window rootwin = testLiteScreen()->rootWindow();

    QImage mapImage = cursor->bitmap()->toImage().convertToFormat(QImage::Format_MonoLSB);
    QImage maskImage = cursor->mask()->toImage().convertToFormat(QImage::Format_MonoLSB);

    int width = cursor->bitmap()->width();
    int height = cursor->bitmap()->height();
    int bytesPerLine = mapImage.bytesPerLine();
    int destLineSize = width / 8;
    if (width % 8)
        destLineSize++;

    const uchar * map = mapImage.bits();
    const uchar * mask = maskImage.bits();

    char * mapBits = new char[height * destLineSize];
    char * maskBits = new char[height * destLineSize];
    for (int i = 0; i < height; i++) {
        memcpy(mapBits + (destLineSize * i),map + (bytesPerLine * i), destLineSize);
        memcpy(maskBits + (destLineSize * i),mask + (bytesPerLine * i), destLineSize);
    }

    Pixmap cp = XCreateBitmapFromData(testLiteScreen()->display(), rootwin, mapBits, width, height);
    Pixmap mp = XCreateBitmapFromData(testLiteScreen()->display(), rootwin, maskBits, width, height);
    Cursor c = XCreatePixmapCursor(testLiteScreen()->display(), cp, mp, &fg, &bg, spot.x(), spot.y());
    XFreePixmap(testLiteScreen()->display(), cp);
    XFreePixmap(testLiteScreen()->display(), mp);
    delete[] mapBits;
    delete[] maskBits;

    return c;
}

Cursor QTestLiteCursor::createCursorShape(int cshape)
{
    Cursor cursor = 0;

    if (cshape < 0 || cshape > Qt::LastCursor)
        return 0;

    switch (cshape) {
    case Qt::ArrowCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_left_ptr);
        break;
    case Qt::UpArrowCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_center_ptr);
        break;
    case Qt::CrossCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_crosshair);
        break;
    case Qt::WaitCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_watch);
        break;
    case Qt::IBeamCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_xterm);
        break;
    case Qt::SizeAllCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_fleur);
        break;
    case Qt::PointingHandCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_hand2);
        break;
    case Qt::SizeBDiagCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_top_right_corner);
        break;
    case Qt::SizeFDiagCursor:
        cursor =  XCreateFontCursor(testLiteScreen()->display(), XC_bottom_right_corner);
        break;
    case Qt::SizeVerCursor:
    case Qt::SplitVCursor:
        cursor = XCreateFontCursor(testLiteScreen()->display(), XC_sb_v_double_arrow);
        break;
    case Qt::SizeHorCursor:
    case Qt::SplitHCursor:
        cursor = XCreateFontCursor(testLiteScreen()->display(), XC_sb_h_double_arrow);
        break;
    case Qt::WhatsThisCursor:
        cursor = XCreateFontCursor(testLiteScreen()->display(), XC_question_arrow);
        break;
    case Qt::ForbiddenCursor:
        cursor = XCreateFontCursor(testLiteScreen()->display(), XC_circle);
        break;
    case Qt::BusyCursor:
        cursor = XCreateFontCursor(testLiteScreen()->display(), XC_watch);
        break;

    default: //default cursor for all the rest
        break;
    }
    return cursor;
}

QTestLiteScreen * QTestLiteCursor::testLiteScreen() const
{
    return static_cast<QTestLiteScreen *>(screen);
}

QT_END_NAMESPACE
