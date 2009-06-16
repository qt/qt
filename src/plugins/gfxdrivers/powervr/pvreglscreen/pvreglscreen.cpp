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

#include "pvreglscreen.h"
#include "pvreglwindowsurface.h"
#include "pvrqwsdrawable_p.h"
#include <QRegExp>
#include <qwindowsystem_qws.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <fcntl.h>
#include <unistd.h>

PvrEglScreen::PvrEglScreen(int displayId)
    : QGLScreen(displayId)
{
    setOptions(NativeWindows);
    setSupportsBlitInClients(true);
    setSurfaceFunctions(new PvrEglScreenSurfaceFunctions(this, displayId));
    fd = -1;
    ttyfd = -1;
    doGraphicsMode = true;
    oldKdMode = KD_TEXT;
    if (QWSServer::instance())
        holder = new PvrEglSurfaceHolder();
    else
        holder = 0;
}

PvrEglScreen::~PvrEglScreen()
{
    if (fd >= 0)
        ::close(fd);
    delete holder;
}

bool PvrEglScreen::initDevice()
{
    openTty();
    return true;
}

bool PvrEglScreen::connect(const QString &displaySpec)
{
    if (!pvrQwsDisplayOpen())
        return false;

    // Initialize the QScreen properties.
    data = (uchar *)(pvrQwsDisplay.screens[0].mapped);
    w = pvrQwsDisplay.screens[0].screenRect.width;
    h = pvrQwsDisplay.screens[0].screenRect.height;
    lstep = pvrQwsDisplay.screens[0].screenStride;
    dw = w;
    dh = h;
    size = h * lstep;
    mapsize = size;
    switch (pvrQwsDisplay.screens[0].pixelFormat) {
	case PVR2D_RGB565:
            d = 16;
            setPixelFormat(QImage::Format_RGB16);
            break;
	case PVR2D_ARGB4444:
            d = 16;
            setPixelFormat(QImage::Format_ARGB4444_Premultiplied);
            break;
	case PVR2D_ARGB8888:
            d = 32;
            setPixelFormat(QImage::Format_ARGB32);
            break;
        default:
            pvrQwsDisplayClose();
            qWarning("PvrEglScreen::connect: unsupported pixel format %d", (int)(pvrQwsDisplay.screens[0].pixelFormat));
            return false;
    }

    // Handle display physical size spec.
    QStringList displayArgs = displaySpec.split(QLatin1Char(':'));
    QRegExp mmWidthRx(QLatin1String("mmWidth=?(\\d+)"));
    int dimIdxW = displayArgs.indexOf(mmWidthRx);
    QRegExp mmHeightRx(QLatin1String("mmHeight=?(\\d+)"));
    int dimIdxH = displayArgs.indexOf(mmHeightRx);
    if (dimIdxW >= 0) {
        mmWidthRx.exactMatch(displayArgs.at(dimIdxW));
        physWidth = mmWidthRx.cap(1).toInt();
        if (dimIdxH < 0)
            physHeight = dh*physWidth/dw;
    }
    if (dimIdxH >= 0) {
        mmHeightRx.exactMatch(displayArgs.at(dimIdxH));
        physHeight = mmHeightRx.cap(1).toInt();
        if (dimIdxW < 0)
            physWidth = dw*physHeight/dh;
    }
    if (dimIdxW < 0 && dimIdxH < 0) {
        const int dpi = 72;
        physWidth = qRound(dw * 25.4 / dpi);
        physHeight = qRound(dh * 25.4 / dpi);
    }

    // Find the name of the tty device to use.
    QRegExp ttyRegExp(QLatin1String("tty=(.*)"));
    if (displayArgs.indexOf(ttyRegExp) != -1)
        ttyDevice = ttyRegExp.cap(1);
    if (displayArgs.contains(QLatin1String("nographicsmodeswitch")))
        doGraphicsMode = false;

    // The screen is ready.
    return true;
}

void PvrEglScreen::disconnect()
{
    pvrQwsDisplayClose();
}

void PvrEglScreen::shutdownDevice()
{
    closeTty();
}

void PvrEglScreen::blit(const QImage &img, const QPoint &topLeft, const QRegion &region)
{
    QGLScreen::blit(img, topLeft, region);
    sync();
}

void PvrEglScreen::solidFill(const QColor &color, const QRegion &region)
{
    QGLScreen::solidFill(color, region);
    sync();
}

bool PvrEglScreen::chooseContext
    (QGLContext *context, const QGLContext *shareContext)
{
    // We use PvrEglScreenSurfaceFunctions instead.
    Q_UNUSED(context);
    Q_UNUSED(shareContext);
    return false;
}

bool PvrEglScreen::hasOpenGL()
{
    return true;
}

QWSWindowSurface* PvrEglScreen::createSurface(QWidget *widget) const
{
    if (qobject_cast<QGLWidget*>(widget))
        return new PvrEglWindowSurface(widget, (QScreen *)this, displayId);

    return QScreen::createSurface(widget);
}

QWSWindowSurface* PvrEglScreen::createSurface(const QString &key) const
{
    if (key == QLatin1String("PvrEgl"))
        return new PvrEglWindowSurface(holder);

    return QScreen::createSurface(key);
}

void PvrEglScreen::sync()
{
    // Put code here to synchronize 2D and 3D operations if necessary.
}

void PvrEglScreen::openTty()
{
    const char *const devs[] = {"/dev/tty0", "/dev/tty", "/dev/console", 0};

    if (ttyDevice.isEmpty()) {
        for (const char * const *dev = devs; *dev; ++dev) {
            ttyfd = ::open(*dev, O_RDWR);
            if (ttyfd != -1)
                break;
        }
    } else {
        ttyfd = ::open(ttyDevice.toAscii().constData(), O_RDWR);
    }

    if (ttyfd == -1)
        return;

    ::fcntl(ttyfd, F_SETFD, FD_CLOEXEC);

    if (doGraphicsMode) {
        ioctl(ttyfd, KDGETMODE, &oldKdMode);
        if (oldKdMode != KD_GRAPHICS) {
            int ret = ioctl(ttyfd, KDSETMODE, KD_GRAPHICS);
            if (ret == -1)
                doGraphicsMode = false;
        }
    }

    // No blankin' screen, no blinkin' cursor!, no cursor!
    const char termctl[] = "\033[9;0]\033[?33l\033[?25l\033[?1c";
    ::write(ttyfd, termctl, sizeof(termctl));
}

void PvrEglScreen::closeTty()
{
    if (ttyfd == -1)
        return;

    if (doGraphicsMode)
        ioctl(ttyfd, KDSETMODE, oldKdMode);

    // Blankin' screen, blinkin' cursor!
    const char termctl[] = "\033[9;15]\033[?33h\033[?25h\033[?0c";
    ::write(ttyfd, termctl, sizeof(termctl));

    ::close(ttyfd);
    ttyfd = -1;
}

bool PvrEglScreenSurfaceFunctions::createNativeWindow(QWidget *widget, EGLNativeWindowType *native)
{
    QWSWindowSurface *surface =
        static_cast<QWSWindowSurface *>(widget->windowSurface());
    if (!surface) {
        // The widget does not have a surface yet, so give it one.
        surface = new PvrEglWindowSurface(widget, screen, displayId);
        widget->setWindowSurface(surface);
    } else if (surface->key() != QLatin1String("PvrEgl")) {
        // The application has attached a QGLContext to an ordinary QWidget.
        // Replace the widget's window surface with a new one that can do GL.
        QRect geometry = widget->frameGeometry();
        geometry.moveTo(widget->mapToGlobal(QPoint(0, 0)));
        surface = new PvrEglWindowSurface(widget, screen, displayId);
        surface->setGeometry(geometry);
        widget->setWindowSurface(surface);
        widget->setAttribute(Qt::WA_NoSystemBackground, true);
    }
    PvrEglWindowSurface *nsurface = static_cast<PvrEglWindowSurface*>(surface);
    *native = (EGLNativeWindowType)(nsurface->nativeDrawable());
    return true;
}

// The PowerVR engine on the device needs to allocate about 2Mb of
// contiguous physical memory to manage drawing into a surface.
//
// The problem is that once Qtopia begins its startup sequence,
// it allocates enough memory to severely fragment the physical
// address space on the device.  This leaves the PowerVR engine
// unable to allocate the necessary contiguous physical memory
// when an EGL surface is created.
//
// A solution to this is to pre-allocate a dummy surface early
// in the startup sequence before memory becomes fragmented,
// reserving it for any future EGL applications to use.
//
// However, the PowerVR engine has problems managing multiple
// surfaces concurrently, and so real EGL applications end up
// with unacceptably slow frame rates unless the dummy surface
// is destroyed while the real EGL applications are running.
//
// In summary, we need to try to ensure that there is always at
// least one EGL surface active at any given time to reserve the
// memory but destroy the temporary surface when a real surface
// is using the device.  That is the purpose of PvrEglSurfaceHolder.

PvrEglSurfaceHolder::PvrEglSurfaceHolder(QObject *parent)
    : QObject(parent)
{
    numRealSurfaces = 0;

    PvrQwsRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = 16;
    rect.height = 16;
    tempSurface = pvrQwsCreateWindow(0, -1, &rect);

    dpy = EGL_NO_DISPLAY;
    config = 0;
    surface = EGL_NO_SURFACE;

    dpy = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);
    if (!eglInitialize(dpy, 0, 0)) {
        qWarning("Could not initialize EGL display - are the drivers loaded?");
        dpy = EGL_NO_DISPLAY;
        return;
    }

    EGLint attribList[16];
    int temp = 0;
    attribList[temp++] = EGL_LEVEL;     // Framebuffer level 0
    attribList[temp++] = 0;
    attribList[temp++] = EGL_SURFACE_TYPE;
    attribList[temp++] = EGL_WINDOW_BIT;
    attribList[temp++] = EGL_NONE;

    EGLint numConfigs = 0;
    if (!eglChooseConfig(dpy, attribList, &config, 1, &numConfigs) || numConfigs != 1) {
        qWarning("Could not find a matching a EGL configuration");
        eglTerminate(dpy);
        dpy = EGL_NO_DISPLAY;
        return;
    }

    surface = eglCreateWindowSurface
        (dpy, config, (EGLNativeWindowType)(-1), NULL);
    if (surface == EGL_NO_SURFACE)
        qWarning("Could not create the temporary EGL surface");
}

PvrEglSurfaceHolder::~PvrEglSurfaceHolder()
{
    if (surface != EGL_NO_SURFACE)
        eglDestroySurface(dpy, surface);
    if (dpy != EGL_NO_DISPLAY)
        eglTerminate(dpy);
    if (tempSurface)
        pvrQwsDestroyDrawable(tempSurface);
}

// Add a real EGL surface to the system.
void PvrEglSurfaceHolder::addSurface()
{
    ++numRealSurfaces;
    if (numRealSurfaces == 1) {
        // Destroy the temporary surface while some other application
        // is making use of the EGL sub-system for 3D rendering.
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(dpy, surface);
            surface = EGL_NO_SURFACE;
        }
    }
}

// Remove an actual EGL surface from the system.
void PvrEglSurfaceHolder::removeSurface()
{
    if (numRealSurfaces > 0) {
        --numRealSurfaces;
        if (numRealSurfaces == 0) {
            // The last real EGL surface has been destroyed, so re-create
            // the temporary surface.  There is a race condition here in
            // that Qtopia could allocate a lot of memory just after
            // the real EGL surface is destroyed but before we could
            // create the temporary surface again.
            if (surface == EGL_NO_SURFACE && dpy != EGL_NO_DISPLAY) {
                surface = eglCreateWindowSurface
                    (dpy, config, (EGLNativeWindowType)(-1), NULL);
                if (surface == EGL_NO_SURFACE)
                    qWarning("Could not re-create the temporary EGL surface");
            }
        }
    }
}
