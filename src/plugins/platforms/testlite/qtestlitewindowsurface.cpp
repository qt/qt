/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qtestlitewindowsurface.h"
#include "qtestliteintegration.h"

#include <QtCore/qdebug.h>
#include <QWindowSystemInterface>

#include "qtestlitewindow.h"

# include <sys/ipc.h>
# include <sys/shm.h>
# include <X11/extensions/XShm.h>

QT_BEGIN_NAMESPACE


struct MyShmImageInfo {
    MyShmImageInfo(Display *xdisplay) :  image(0), display(xdisplay) {}
    ~MyShmImageInfo() { destroy(); }

    void destroy();

    XShmSegmentInfo shminfo;
    XImage *image;
    Display *display;
};

//void QTestLiteWindowSurface::flush()


#ifndef DONT_USE_MIT_SHM
void MyShmImageInfo::destroy()
{
    XShmDetach (display, &shminfo);
    XDestroyImage (image);
    shmdt (shminfo.shmaddr);
    shmctl (shminfo.shmid, IPC_RMID, 0);
}
#endif

void QTestLiteWindowSurface::resizeShmImage(int width, int height)
{
    MyDisplay *xd = xw->xd;

#ifdef DONT_USE_MIT_SHM
    shm_img = QImage(width, height, QImage::Format_RGB32);
#else
    if (image_info)
        image_info->destroy();
    else
        image_info = new MyShmImageInfo(xd->display);

    Visual *visual = DefaultVisual(xd->display, xd->screen);


    XImage *image = XShmCreateImage (xd->display, visual, 24, ZPixmap, 0,
                                     &image_info->shminfo, width, height);


    image_info->shminfo.shmid = shmget (IPC_PRIVATE,
          image->bytes_per_line * image->height, IPC_CREAT|0777);

    image_info->shminfo.shmaddr = image->data = (char*)shmat (image_info->shminfo.shmid, 0, 0);
    image_info->shminfo.readOnly = False;

    image_info->image = image;

    Status shm_attach_status = XShmAttach(xd->display, &image_info->shminfo);

    Q_ASSERT(shm_attach_status == True);

    shm_img = QImage( (uchar*) image->data, image->width, image->height, image->bytes_per_line, QImage::Format_RGB32 );
#endif
    painted = false;
}


void QTestLiteWindowSurface::resizeBuffer(QSize s)
{
    if (shm_img.size() != s)
        resizeShmImage(s.width(), s.height());
}

QSize QTestLiteWindowSurface::bufferSize() const
{
    return shm_img.size();
}

QTestLiteWindowSurface::QTestLiteWindowSurface (QTestLiteScreen */*screen*/, QWidget *window)
    : QWindowSurface(window),
      painted(false), image_info(0)
{
    xw = static_cast<QTestLiteWindow*>(window->platformWindow());
    xw->windowSurface = this;
//    qDebug() << "QTestLiteWindowSurface::QTestLiteWindowSurface:" << xw->window;
}

QTestLiteWindowSurface::~QTestLiteWindowSurface()
{
    delete image_info;
}

QPaintDevice *QTestLiteWindowSurface::paintDevice()
{
    return &shm_img;
}


void QTestLiteWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    //   qDebug() << "QTestLiteWindowSurface::flush:" << (long)this;

    if (!painted)
        return;

    MyDisplay *xd = xw->xd;
    GC gc = xw->gc;
    Window window = xw->x_window;
#ifdef DONT_USE_MIT_SHM
    // just convert the image every time...
    if (!shm_img.isNull()) {
        Visual *visual = DefaultVisual(xd->display, xd->screen);

        QImage image = shm_img;
        //img.convertToFormat(
        XImage *xi = XCreateImage(xd->display, visual, 24, ZPixmap,
                                  0, (char *) image.scanLine(0), image.width(), image.height(),
                                  32, image.bytesPerLine());

        int x = 0;
        int y = 0;

        /*int r =*/  XPutImage(xd->display, window, gc, xi, 0, 0, x, y, image.width(), image.height());

        xi->data = 0; // QImage owns these bits
        XDestroyImage(xi);
    }
#else
    // Use MIT_SHM
    if (image_info->image) {
        //qDebug() << "Here we go" << image_info->image->width << image_info->image->height;
        int x = 0;
        int y = 0;

        // We could set send_event to true, and then use the ShmCompletion to synchronize,
        // but let's do like Qt/11 and just use XSync
        XShmPutImage (xd->display, window, gc, image_info->image, 0, 0,
                      x, y, image_info->image->width, image_info->image->height,
                      /*send_event*/ False);

        XSync(xd->display, False);
    }
#endif
}

//### scroll logic copied from QRasterWindowSurface, we should make better API for this

void copied_qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset)
{
    // make sure we don't detach
    uchar *mem = const_cast<uchar*>(const_cast<const QImage &>(img).bits());

    int lineskip = img.bytesPerLine();
    int depth = img.depth() >> 3;

    const QRect imageRect(0, 0, img.width(), img.height());
    const QRect r = rect & imageRect & imageRect.translated(-offset);
    const QPoint p = rect.topLeft() + offset;

    if (r.isEmpty())
        return;

    const uchar *src;
    uchar *dest;

    if (r.top() < p.y()) {
        src = mem + r.bottom() * lineskip + r.left() * depth;
        dest = mem + (p.y() + r.height() - 1) * lineskip + p.x() * depth;
        lineskip = -lineskip;
    } else {
        src = mem + r.top() * lineskip + r.left() * depth;
        dest = mem + p.y() * lineskip + p.x() * depth;
    }

    const int w = r.width();
    int h = r.height();
    const int bytes = w * depth;

    // overlapping segments?
    if (offset.y() == 0 && qAbs(offset.x()) < w) {
        do {
            ::memmove(dest, src, bytes);
            dest += lineskip;
            src += lineskip;
        } while (--h);
    } else {
        do {
            ::memcpy(dest, src, bytes);
            dest += lineskip;
            src += lineskip;
        } while (--h);
    }
}

bool QTestLiteWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    if (shm_img.isNull())
        return false;

    const QVector<QRect> rects = area.rects();
    for (int i = 0; i < rects.size(); ++i)
        copied_qt_scrollRectInImage(shm_img, rects.at(i), QPoint(dx, dy));

    return true;
}


void QTestLiteWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
    resizeBuffer(size());
}

void QTestLiteWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
    painted = true; //there is content in the buffer
}
QT_END_NAMESPACE
