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

#include "hybridscreen.h"
#include "hybridsurface.h"

#include <QVector>
#include <QVarLengthArray>
#include <QApplication>
#include <QColor>
#include <QWidget>

#include <GLES/egl.h>

class HybridScreenPrivate
{
public:
    HybridScreenPrivate(HybridScreen *owner);

    bool verbose;
    EGLDisplay display;
    EGLint majorEGLVersion;
    EGLint minorEGLVersion;

    QScreen *screen;

private:
    HybridScreen *q_ptr;
};

HybridScreenPrivate::HybridScreenPrivate(HybridScreen *owner)
    : display(EGL_NO_DISPLAY), majorEGLVersion(0), minorEGLVersion(0),
      screen(0), q_ptr(owner)
{
}

HybridScreen::HybridScreen(int displayId)
    : QGLScreen(displayId)
{
    d_ptr = new HybridScreenPrivate(this);
}

HybridScreen::~HybridScreen()
{
    delete d_ptr;
}

static void error(const char *message)
{
    const EGLint error = eglGetError();
    qWarning("HybridScreen error: %s: 0x%x", message, error);
}

static int getDisplayId(const QString &spec)
{
    QRegExp regexp(QLatin1String(":(\\d+)\\b"));
    if (regexp.lastIndexIn(spec) != -1) {
        const QString capture = regexp.cap(1);
        return capture.toInt();
    }
    return 0;
}

bool HybridScreen::connect(const QString &displaySpec)
{
    QString dspec = displaySpec;
    if (dspec.startsWith(QLatin1String("hybrid:"), Qt::CaseInsensitive))
        dspec = dspec.mid(QString(QLatin1String("hybrid:")).size());
    else if (dspec.compare(QLatin1String("hybrid"), Qt::CaseInsensitive) == 0)
        dspec = QString();

    const QString displayIdSpec = QString(QLatin1String(" :%1")).arg(displayId);
    if (dspec.endsWith(displayIdSpec))
        dspec = dspec.left(dspec.size() - displayIdSpec.size());

    const QStringList args = dspec.split(QLatin1Char(':'),
                                         QString::SkipEmptyParts);
    const int id = getDisplayId(dspec);
    d_ptr->screen = qt_get_screen(id, dspec.toLatin1().constData());

    const QScreen *screen = d_ptr->screen;
    d = screen->depth();
    w = screen->width();
    h = screen->height();
    dw = screen->deviceWidth();
    dh = screen->deviceHeight();
    lstep = screen->linestep();
    data = screen->base();
    physWidth = screen->physicalWidth();
    physHeight = screen->physicalHeight();
    setPixelFormat(screen->pixelFormat());
    setOffset(screen->offset());

    d_ptr->verbose = args.contains(QLatin1String("verbose"));

    d_ptr->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (d_ptr->display == EGL_NO_DISPLAY) {
        error("getting display");
        return false;
    }

    EGLBoolean status;
    status = eglInitialize(d_ptr->display,
                           &d_ptr->majorEGLVersion, &d_ptr->minorEGLVersion);
    if (!status) {
        error("eglInitialize");
        return false;
    }
    if (d_ptr->verbose) {
        qDebug("Detected EGL version %d.%d",
               d_ptr->majorEGLVersion, d_ptr->minorEGLVersion);

        EGLint numConfigs = 0;
        eglGetConfigs(d_ptr->display, 0, 0, &numConfigs);
        qDebug("%d available configurations", numConfigs);
    }

    // XXX: hw: use eglQueryString to find supported APIs

    qt_screen = this; // XXX

    return true;
}

bool HybridScreen::initDevice()
{
    if (d_ptr->screen)
        return d_ptr->screen->initDevice();
    return false;
}

void HybridScreen::shutdownDevice()
{
    if (d_ptr->screen)
        d_ptr->screen->shutdownDevice();
}

void HybridScreen::disconnect()
{
    if (!eglTerminate(d_ptr->display))
        error("disconnecting");
    if (d_ptr->screen) {
        d_ptr->screen->disconnect();
        delete d_ptr->screen;
        d_ptr->screen = 0;
    }

}

bool HybridScreen::hasOpenGLOverlays() const
{
    return true;
}

bool HybridScreen::chooseContext(QGLContext *context,
                                 const QGLContext *shareContext)
{
#if 0
    // hw: update the glFormat variable. Probably needs a setter in the
    // QGLWindowSurface class which can be a friend of whatever it wants.

    GLint res;
    eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_LEVEL, &res);
    d_ptr->glFormat.setPlane(res);
    QT_EGL_ERR("eglGetConfigAttrib");

    /*
    if(deviceIsPixmap())
        res = 0;
    else
    eglDescribePixelFormat(fmt, EGL_DOUBLEBUFFER, &res);
    d_ptr->glFormat.setDoubleBuffer(res);
    */

    eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_DEPTH_SIZE, &res);
    d_ptr->glFormat.setDepth(res);
    if (d_ptr->glFormat.depth())
        d_ptr->glFormat.setDepthBufferSize(res);

    //eglGetConfigAttrib(d_ptr->display,d_ptr->config, EGL_RGBA, &res);
    //d_ptr->glFormat.setRgba(res);

    eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_ALPHA_SIZE, &res);
    d_ptr->glFormat.setAlpha(res);
    if (d_ptr->glFormat.alpha())
        d_ptr->glFormat.setAlphaBufferSize(res);

    //eglGetConfigAttrib(d_ptr->display,d_ptr->config, EGL_ACCUM_RED_SIZE, &res);
    //d_ptr->glFormat.setAccum(res);
    //if (d_ptr->glFormat.accum())
    //    d_ptr->glFormat.setAccumBufferSize(res);

    eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_STENCIL_SIZE, &res);
    d_ptr->glFormat.setStencil(res);
    if (d_ptr->glFormat.stencil())
        d_ptr->glFormat.setStencilBufferSize(res);

    //eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_STEREO, &res);
    //d_ptr->glFormat.setStereo(res);

    eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_SAMPLE_BUFFERS, &res);
    d_ptr->glFormat.setSampleBuffers(res);

    if (d_ptr->glFormat.sampleBuffers()) {
        eglGetConfigAttrib(d_ptr->display, d_ptr->config, EGL_SAMPLES, &res);
        d_ptr->glFormat.setSamples(res);
    }
#endif

    // hw: TODO: implement sharing of contexts

#if 0
    if(shareContext &&
       (!shareContext->isValid() || !shareContext->d_func()->cx)) {
        qWarning("QGLContext::chooseContext(): Cannot share with invalid context");
        shareContext = 0;
    }
#endif

#if 0
    d_ptr->cx = ctx;
    if (shareContext && shareContext->d_func()->cx) {
        QGLContext *share = const_cast<QGLContext *>(shareContext);
        d_ptr->sharing = true;
        share->d_func()->sharing = true;
    }
#endif

#if 0
    // vblank syncing
    GLint interval = d_ptr->reqFormat.swapInterval();
    if (interval != -1) {
        if (interval != 0)
            eglSwapInterval(d_ptr->display, interval);
    }
#endif

    return QGLScreen::chooseContext(context, shareContext);
}

void HybridScreen::setDirty(const QRect& rect)
{
    d_ptr->screen->setDirty(rect);
}

void HybridScreen::setMode(int w, int h, int d)
{
    d_ptr->screen->setMode(w, h, d);
    setDirty(region().boundingRect());
}

bool HybridScreen::supportsDepth(int depth) const
{
    return d_ptr->screen->supportsDepth(depth);
}

void HybridScreen::save()
{
    d_ptr->screen->save();
}

void HybridScreen::restore()
{
    d_ptr->screen->restore();
}

void HybridScreen::blank(bool on)
{
    d_ptr->screen->blank(on);
}

bool HybridScreen::onCard(const unsigned char *ptr) const
{
    return d_ptr->screen->onCard(ptr);
}

bool HybridScreen::onCard(const unsigned char *ptr, ulong &offset) const
{
    return d_ptr->screen->onCard(ptr, offset);
}

bool HybridScreen::isInterlaced() const
{
    return d_ptr->screen->isInterlaced();
}

int HybridScreen::memoryNeeded(const QString &str)
{
    return d_ptr->screen->memoryNeeded(str);
}

int HybridScreen::sharedRamSize(void *ptr)
{
    return d_ptr->screen->sharedRamSize(ptr);
}

void HybridScreen::haltUpdates()
{
    d_ptr->screen->haltUpdates();
}

void HybridScreen::resumeUpdates()
{
    d_ptr->screen->resumeUpdates();
}

void HybridScreen::exposeRegion(QRegion r, int changing)
{
    d_ptr->screen->exposeRegion(r, changing);
}

void HybridScreen::blit(const QImage &img, const QPoint &topLeft, const QRegion &region)
{
    d_ptr->screen->blit(img, topLeft, region);
}

void HybridScreen::solidFill(const QColor &color, const QRegion &region)
{
    d_ptr->screen->solidFill(color, region);
}

QWSWindowSurface* HybridScreen::createSurface(QWidget *widget) const
{
    if (qobject_cast<QGLWidget*>(widget))
        return new HybridSurface(widget, d_ptr->display);
    return d_ptr->screen->createSurface(widget);
}

QWSWindowSurface* HybridScreen::createSurface(const QString &key) const
{
    if (key == QLatin1String("hybrid"))
        return new HybridSurface;
    return d_ptr->screen->createSurface(key);
}

QList<QScreen*> HybridScreen::subScreens() const
{
    return d_ptr->screen->subScreens();
}

QRegion HybridScreen::region() const
{
    return d_ptr->screen->region();
}
