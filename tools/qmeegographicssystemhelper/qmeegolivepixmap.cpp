/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qmeegolivepixmap.h"
#include <private/qimage_p.h>
#include <private/qpixmap_raster_p.h>
#include "qmeegolivepixmap_p.h"
#include "qmeegoruntime.h"
#include <QSharedMemory>

/* QMeeGoLivePixmapPrivate */

QMeeGoLivePixmapPrivate::QMeeGoLivePixmapPrivate(Qt::HANDLE h) : handle(h)
{
}

QMeeGoLivePixmapPrivate::~QMeeGoLivePixmapPrivate()
{
}

/* QMeeGoLivePixmap */

QMeeGoLivePixmap* QMeeGoLivePixmap::livePixmapWithSize(int w, int h, Format format)
{
    QImage::Format qtFormat; 
    if (format == Format_RGB16)
        qtFormat = QImage::Format_RGB16;
    else if (format == Format_ARGB32_Premultiplied)
        qtFormat = QImage::Format_ARGB32_Premultiplied;
    else {
        qWarning("Unsupported live pixmap format!");
            return 0;
    }
    
    Qt::HANDLE liveTextureHandle = QMeeGoRuntime::createLiveTexture(w, h, qtFormat);
    if (! liveTextureHandle) {
        qWarning("Failed to create a live texture with given size!");
        return NULL;
    }

    return QMeeGoLivePixmap::fromHandle(liveTextureHandle);
}

QMeeGoLivePixmap::QMeeGoLivePixmap(QPixmapData *p, Qt::HANDLE h) : QPixmap(p), d_ptr(new QMeeGoLivePixmapPrivate(h))
{
    Q_D(QMeeGoLivePixmap);
    d->q_ptr = this;
}

QMeeGoLivePixmap* QMeeGoLivePixmap::fromHandle(Qt::HANDLE liveTextureHandle)
{
    Qt::HANDLE eglImage = QMeeGoRuntime::liveTextureToEGLImage(liveTextureHandle);
    if (! eglImage) {
        qWarning("Failed to bind the live texture as an egl image!");
        return NULL;
    }

    QPixmapData *pmd = QMeeGoRuntime::pixmapDataFromEGLImage(eglImage);
    if (! pmd) {
        qWarning("Failed to allocate a pixmap data from a given live texture egl image!");
        return NULL;
    }
    
    return new QMeeGoLivePixmap(pmd, liveTextureHandle);
}

Qt::HANDLE QMeeGoLivePixmap::handle()
{
    Q_D(QMeeGoLivePixmap);
    return d->handle;
}

QMeeGoLivePixmap::~QMeeGoLivePixmap()
{
}

QImage* QMeeGoLivePixmap::lock()
{
    Q_D(QMeeGoLivePixmap);

    void *data = NULL; 
    int pitch = 0;
    QImage::Format format;

    if (! QMeeGoRuntime::lockLiveTexture(d->handle)) {
        qWarning("Failed to lock a live texture!");
        return new QImage();    
    }

    QMeeGoRuntime::queryLiveTexture(d->handle, &data, &pitch, &format);
    if (data == NULL || pitch == 0) {
        qWarning("Failed to query the live texture!");
        return new QImage();
    }

    return new QImage((uchar *) data, width(), height(), format);
}
    
void QMeeGoLivePixmap::release(QImage *img)
{
    Q_D(QMeeGoLivePixmap);
    // FIXME Make sure we're locked!
    QMeeGoRuntime::unlockLiveTexture(d->handle);
    delete img;
}
