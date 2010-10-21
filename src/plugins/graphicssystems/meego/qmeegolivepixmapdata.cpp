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

#include "qmeegolivepixmapdata.h"
#include "qmeegoextensions.h"
#include "qmeegorasterpixmapdata.h"
#include <private/qimage_p.h>
#include <private/qwindowsurface_gl_p.h>
#include <private/qeglcontext_p.h>
#include <private/qapplication_p.h>
#include <private/qgraphicssystem_runtime_p.h>
#include <private/qpixmap_x11_p.h>

static EGLint lock_attribs[] = {
    EGL_MAP_PRESERVE_PIXELS_KHR, EGL_TRUE,
    EGL_LOCK_USAGE_HINT_KHR, EGL_READ_SURFACE_BIT_KHR | EGL_WRITE_SURFACE_BIT_KHR,
    EGL_NONE
};

static EGLint preserved_attribs[] = {
    EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
    EGL_NONE
};

/* Public */

QMeeGoLivePixmapData::QMeeGoLivePixmapData(int w, int h, QImage::Format format) : QGLPixmapData(QPixmapData::PixmapType)
{
    QImage image(w, h, format);
    QX11PixmapData *pmd = new QX11PixmapData(QPixmapData::PixmapType);
    pmd->fromImage(image, Qt::NoOpaqueDetection);
    backingX11Pixmap = new QPixmap(pmd);

    initializeThroughEGLImage();
}

QMeeGoLivePixmapData::QMeeGoLivePixmapData(Qt::HANDLE h) : QGLPixmapData(QPixmapData::PixmapType)
{
    backingX11Pixmap = new QPixmap(QPixmap::fromX11Pixmap(h));
    initializeThroughEGLImage();
}

QMeeGoLivePixmapData::~QMeeGoLivePixmapData()
{
    delete backingX11Pixmap;
}

void QMeeGoLivePixmapData::initializeThroughEGLImage()
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    EGLImageKHR eglImage = EGL_NO_IMAGE_KHR;
    GLuint newTextureId = 0;

    eglImage = QEgl::eglCreateImageKHR(QEgl::display(), EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR,
                                       (EGLClientBuffer) backingX11Pixmap->handle(), preserved_attribs);

    if (eglImage == EGL_NO_IMAGE_KHR) {
        qWarning("eglCreateImageKHR failed (live texture)!");
        return;
    }

    glGenTextures(1, &newTextureId);
    glBindTexture(GL_TEXTURE_2D, newTextureId);

    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (EGLImageKHR) eglImage);
    if (glGetError() == GL_NO_ERROR) {
        resize(backingX11Pixmap->width(), backingX11Pixmap->height());
        texture()->id = newTextureId;
        texture()->options &= ~QGLContext::InvertedYBindOption;
        m_hasAlpha = backingX11Pixmap->hasAlphaChannel();
    } else {
        qWarning("Failed to create a texture from an egl image (live texture)!");
        glDeleteTextures(1, &newTextureId);
    }

    QEgl::eglDestroyImageKHR(QEgl::display(), eglImage);
}

QPixmapData *QMeeGoLivePixmapData::createCompatiblePixmapData() const
{
    return new QMeeGoRasterPixmapData(pixelType());
}

QImage* QMeeGoLivePixmapData::lock()
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    void *data = 0;
    int pitch = 0;
    EGLSurface surface = 0;
    QImage::Format format;

    surface = getSurfaceForBackingPixmap();
    if (! QMeeGoExtensions::eglLockSurfaceKHR(QEgl::display(), surface, lock_attribs)) {
        qWarning("Failed to lock surface (live texture)!");
        return new QImage();
    }

    eglQuerySurface(QEgl::display(), surface, EGL_BITMAP_POINTER_KHR, (EGLint*) &data);
    eglQuerySurface(QEgl::display(), surface, EGL_BITMAP_PITCH_KHR, (EGLint*) &pitch);

    // Ok, here we know we just support those two formats. Real solution would be:
    // uqery also the format.
    if (backingX11Pixmap->depth() > 16)
        format = QImage::Format_ARGB32_Premultiplied;
    else
        format = QImage::Format_RGB16;

    if (data == NULL || pitch == 0) {
        qWarning("Failed to query the live texture!");
        return new QImage();
    }

    return new QImage((uchar *) data, width(), height(), format);
}

bool QMeeGoLivePixmapData::release(QImage* /*img*/)
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    if (QMeeGoExtensions::eglUnlockSurfaceKHR(QEgl::display(), getSurfaceForBackingPixmap())) {
        glFinish();
        return true;
    } else {
        return false;
    }
}

Qt::HANDLE QMeeGoLivePixmapData::handle()
{
    return backingX11Pixmap->handle();
}

EGLSurface QMeeGoLivePixmapData::getSurfaceForBackingPixmap()
{
    // This code is a crative remix of the stuff that can be found in the
    // Qt's TFP implementation in /src/opengl/qgl_x11egl.cpp ::bindiTextureFromNativePixmap
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(backingX11Pixmap->data_ptr().data());
    Q_ASSERT(pixmapData->classId() == QPixmapData::X11Class);
    bool hasAlpha = pixmapData->hasAlphaChannel();

    if (pixmapData->gl_surface &&
        hasAlpha == (pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha))
        return pixmapData->gl_surface;

    // Check to see if the surface is still valid
    if (pixmapData->gl_surface &&
        hasAlpha != ((pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha) > 0)) {
        // Surface is invalid!
        destroySurfaceForPixmapData(pixmapData);
    }

    if (pixmapData->gl_surface == 0) {
        EGLConfig config = QEgl::defaultConfig(QInternal::Pixmap,
                                               QEgl::OpenGL,
                                               hasAlpha ? QEgl::Translucent : QEgl::NoOptions);

        pixmapData->gl_surface = (void*)QEgl::createSurface(backingX11Pixmap, config);

        if (hasAlpha)
            pixmapData->flags |= QX11PixmapData::GlSurfaceCreatedWithAlpha;
        else
            pixmapData->flags &= ~QX11PixmapData::GlSurfaceCreatedWithAlpha;

        if (pixmapData->gl_surface == (void*)EGL_NO_SURFACE)
            return NULL;
    }

    return pixmapData->gl_surface;
}

void QMeeGoLivePixmapData::destroySurfaceForPixmapData(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        eglDestroySurface(QEgl::display(), (EGLSurface)pixmapData->gl_surface);
        pixmapData->gl_surface = 0;
    }
}
