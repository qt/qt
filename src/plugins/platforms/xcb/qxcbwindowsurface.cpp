/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qxcbwindowsurface.h"

#include "qxcbconnection.h"
#include "qxcbscreen.h"
#include "qxcbwindow.h"

#include <stdio.h>

QXcbWindowSurface::QXcbWindowSurface(QWidget *widget, bool setDefaultSurface)
    : QWindowSurface(widget, setDefaultSurface)
{
    setStaticContentsSupport(false);
    setPartialUpdateSupport(false);

    QXcbScreen *screen = static_cast<QXcbScreen *>(QPlatformScreen::platformScreenForWidget(widget));
    setConnection(screen->connection());
}

QXcbWindowSurface::~QXcbWindowSurface()
{
}

QPaintDevice *QXcbWindowSurface::paintDevice()
{
    return &m_image;
}

void QXcbWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(region);
    Q_UNUSED(offset);

    QXcbWindow *window = static_cast<QXcbWindow *>(widget->window()->platformWindow());

    m_gc = xcb_generate_id(xcb_connection());
    xcb_create_gc(xcb_connection(), m_gc, window->window(), 0, 0);

    xcb_put_image(xcb_connection(),
                  XCB_IMAGE_FORMAT_Z_PIXMAP,
                  window->window(),
                  m_gc,
                  m_image.width(),
                  m_image.height(),
                  0,
                  0,
                  0,
                  24,
                  m_image.numBytes(),
                  m_image.bits());

    xcb_free_gc(xcb_connection(), m_gc);
    xcb_flush(xcb_connection());
}

void QXcbWindowSurface::resize(const QSize &size)
{
    QWindowSurface::resize(size);
    m_image = QImage(size, QImage::Format_RGB32);
}

extern void qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset);

bool QXcbWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    if (m_image.isNull())
        return false;

    const QVector<QRect> rects = area.rects();
    for (int i = 0; i < rects.size(); ++i)
        qt_scrollRectInImage(m_image, rects.at(i), QPoint(dx, dy));

    return true;
}

