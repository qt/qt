/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#include "hybridsurface.h"

#include <private/qwindowsurface_qws_p.h>
#include <private/qwslock_p.h>
#include <qscreen_qws.h>
#include <qvarlengtharray.h>

static void error(const char *message)
{
    const EGLint error = eglGetError();
    qWarning("HybridSurface error: %s: 0x%x", message, error);
}

static void imgToVanilla(const QImage *img, VanillaPixmap *pix)
{
    pix->width = img->width();
    pix->height = img->height();
    pix->stride = img->bytesPerLine();

    if (img->depth() == 32) {
        pix->rSize = pix->gSize = pix->bSize = pix->aSize = 8;
        pix->lSize = 0;
        pix->rOffset = 16;
        pix->gOffset = 8;
        pix->bOffset = 0;
        pix->aOffset = 24;
    } else if (img->format() == QImage::Format_RGB16) {
        pix->rSize = 5;
        pix->gSize = 6;
        pix->bSize = 5;
        pix->aSize = 0;
        pix->lSize = 0;
        pix->rOffset = 11;
        pix->gOffset = 5;
        pix->bOffset = 0;
        pix->aOffset = 0;
    }

    pix->padding = pix->padding2 = 0;
    pix->pixels = const_cast<uchar*>(img->bits());
}

HybridSurface::HybridSurface()
    : QWSGLWindowSurface(), memlock(0)
{
    setSurfaceFlags(Buffered | Opaque);
}

HybridSurface::HybridSurface(QWidget *w, EGLDisplay disp)
    :  QWSGLWindowSurface(w), memlock(0), display(disp), config(0),
       surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT),
       pdevice(new QWSGLPaintDevice(w))
{
    setSurfaceFlags(Buffered | Opaque);

    EGLint configAttribs[] = {
        EGL_RED_SIZE,        0,
        EGL_GREEN_SIZE,      0,
        EGL_BLUE_SIZE,       0,
        EGL_ALPHA_SIZE,      0,
        EGL_DEPTH_SIZE,      0,
        EGL_STENCIL_SIZE,   EGL_DONT_CARE,
        EGL_SURFACE_TYPE,   EGL_WINDOW_BIT,
        EGL_NONE,           EGL_NONE
    };


    EGLBoolean status;
    EGLint numConfigs;
    status = eglChooseConfig(display, configAttribs, 0, 0, &numConfigs);
    if (!status) {
        error("chooseConfig");
        return;
    }

    //If there isn't any configuration good enough
    if (numConfigs < 1) {
        error("chooseConfig, no matching configurations found");
        return;
    }

    QVarLengthArray<EGLConfig> configs(numConfigs);

    status = eglChooseConfig(display, configAttribs, configs.data(),
                             numConfigs, &numConfigs);
    if (!status) {
        error("chooseConfig");
        return;
    }

    // hw: if used on an image buffer we need to check whether the resulting
    // configuration matches our requirements exactly!
    config = configs[0];

    context = eglCreateContext(display, config, 0, 0);
                                      //(shareContext ? shareContext->d_func()->cx : 0),
                                      //configAttribs);
    if (context == EGL_NO_CONTEXT)
        error("eglCreateContext");

}

HybridSurface::~HybridSurface()
{
}

bool HybridSurface::isValid() const
{
    return true;
}

void HybridSurface::setGeometry(const QRect &rect, const QRegion &mask)
{
    const QSize size = rect.size();
    if (img.size() != size) {
//        QWidget *win = window();
        QImage::Format imageFormat = QImage::Format_ARGB32_Premultiplied;
        const int bytesPerPixel = 4;

        const int bpl = (size.width() * bytesPerPixel + 3) & ~3;
        const int imagesize = bpl * size.height();

        if (imagesize == 0) {
            eglDestroySurface(display, surface);
            mem.detach();
            img = QImage();
        } else {
            mem.detach();
            if (!mem.create(imagesize)) {
                perror("HybridSurface::setGeometry allocating shared memory");
                qFatal("Error creating shared memory of size %d", imagesize);
            }
            uchar *base = static_cast<uchar*>(mem.address());
            img = QImage(base, size.width(), size.height(), imageFormat);
//            setImageMetrics(img, win);

            imgToVanilla(&img, &vanillaPix);
            surface = eglCreatePixmapSurface(display, config, &vanillaPix, 0);
            if (surface == EGL_NO_SURFACE)
                error("setGeometry:eglCreatePixmapSurface");

        }
    }
    QWSWindowSurface::setGeometry(rect, mask);
}

QByteArray HybridSurface::permanentState() const
{
    QByteArray array;
    array.resize(4 * sizeof(int) + sizeof(QImage::Format) +
                 sizeof(SurfaceFlags));

    char *ptr = array.data();

    reinterpret_cast<int*>(ptr)[0] = mem.id();
    reinterpret_cast<int*>(ptr)[1] = img.width();
    reinterpret_cast<int*>(ptr)[2] = img.height();
    reinterpret_cast<int*>(ptr)[3] = (memlock ? memlock->id() : -1);
    ptr += 4 * sizeof(int);

    *reinterpret_cast<QImage::Format*>(ptr) = img.format();
    ptr += sizeof(QImage::Format);

    *reinterpret_cast<SurfaceFlags*>(ptr) = surfaceFlags();

    return array;
}

void HybridSurface::setPermanentState(const QByteArray &data)
{
    int memId;
    int width;
    int height;
    int lockId;
    QImage::Format format;
    SurfaceFlags flags;

    const char *ptr = data.constData();

    memId = reinterpret_cast<const int*>(ptr)[0];
    width = reinterpret_cast<const int*>(ptr)[1];
    height = reinterpret_cast<const int*>(ptr)[2];
    lockId = reinterpret_cast<const int*>(ptr)[3];
    ptr += 4 * sizeof(int);

    format = *reinterpret_cast<const QImage::Format*>(ptr);
    ptr += sizeof(QImage::Format);
    flags = *reinterpret_cast<const SurfaceFlags*>(ptr);

    setSurfaceFlags(flags);

//  setMemory(memId);
    if (mem.id() != memId) {
        mem.detach();
        if (!mem.attach(memId)) {
            perror("QWSSharedMemSurface: attaching to shared memory");
            qCritical("QWSSharedMemSurface: Error attaching to"
                      " shared memory 0x%x", memId);
        }
    }

//  setLock(lockId);
    if (!memlock || memlock->id() == lockId) {
        delete memlock;
        memlock = (lockId == -1 ? 0 : new QWSLock(lockId));
    }

    uchar *base = static_cast<uchar*>(mem.address());
    img = QImage(base, width, height, format);
}

QImage HybridSurface::image() const
{
    return img;
}

QPaintDevice* HybridSurface::paintDevice()
{
    return pdevice;
}

void HybridSurface::beginPaint(const QRegion &region)
{
    QWSGLWindowSurface::beginPaint(region);
    eglBindAPI(EGL_OPENGL_ES_API);

    EGLBoolean ok = eglMakeCurrent(display, surface, surface, context);
    if (!ok)
        error("qglMakeCurrent");
}

bool HybridSurface::lock(int timeout)
{
    Q_UNUSED(timeout);
    if (!memlock)
        return true;
    return memlock->lock(QWSLock::BackingStore);
}

void HybridSurface::unlock()
{
    if (memlock)
        memlock->unlock(QWSLock::BackingStore);
}

QPoint HybridSurface::painterOffset() const
{
    const QWidget *w = window();
    if (!w)
        return QPoint();

    if (w->mask().isEmpty())
        return QWSWindowSurface::painterOffset();

    const QRegion region = w->mask()
                           & w->frameGeometry().translated(-w->geometry().topLeft());
    return -region.boundingRect().topLeft();
}

