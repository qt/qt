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

#include "qwaylandintegration.h"
#include "qwaylandwindowsurface.h"
#include <QtCore/qdebug.h>
#include <QtGui/private/qapplication_p.h>

#include <wayland-client.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

QT_BEGIN_NAMESPACE

QWaylandWindowSurface::QWaylandWindowSurface(QWidget *window,
					     QWaylandDisplay *display)
    : QWindowSurface(window)
    , mImage(0)
    , mDisplay(display)
{
    //qDebug() << "QWaylandWindowSurface::QWaylandWindowSurface:" << (long)this;
}

QWaylandWindowSurface::~QWaylandWindowSurface()
{
}

QPaintDevice *QWaylandWindowSurface::paintDevice()
{
    //qDebug() << "QWaylandWindowSurface::paintDevice";
    return mImage;
}

void QWaylandWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QVector<QRect> rects = region.rects();
    const QRect *r;
    int i;

    for (i = 0; i < rects.size(); i++) {
	r = &rects.at(i);
	wl_surface_damage(ww->surface(),
			  r->x(), r->y(), r->width(), r->height());
    }
}

void QWaylandWindowSurface::resize(const QSize &size)
{
    //qDebug() << "QWaylandWindowSurface::setGeometry:" << (long)this << rect;
    QWaylandWindow *ww = (QWaylandWindow *) window()->platformWindow();
    QWindowSurface::resize(size);
    QImage::Format format = QApplicationPrivate::platformIntegration()->screens().first()->format();

    if (mImage != NULL && mImage->size() == size)
	return;

    if (mImage != NULL) {
	delete mImage;
	munmap(mData, mSize);
    }

    mStride = size.width() * 4;
    mSize = mStride * size.height();
    char filename[] = "/tmp/wayland-shm-XXXXXX";
    int fd = mkstemp(filename);
    if (fd < 0)
	qWarning("open %s failed: %s", filename, strerror(errno));
    if (ftruncate(fd, mSize) < 0)
	qWarning("ftruncate failed: %s", strerror(errno));
    mData = (uchar *)
	mmap(NULL, mSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    unlink(filename);

    if (mData == (uchar *) MAP_FAILED)
	qWarning("mmap /dev/zero failed: %s", strerror(errno));

    mImage = new QImage(mData, size.width(), size.height(), mStride, format);

    struct wl_buffer *buffer =
	mDisplay->createShmBuffer(fd, size.width(), size.height(),
				  mStride, mDisplay->argbVisual());
    wl_surface_attach(ww->surface(), buffer);
    wl_surface_map(ww->surface(), 0, 0, size.width(), size.height());
    wl_buffer_destroy(buffer);
    close(fd);
}

QT_END_NAMESPACE
