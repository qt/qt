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

#include <QDebug>
#include <private/qpixmap_raster_p.h>
#include <private/qwindowsurface_gl_p.h>
#include <private/qegl_p.h>
#include <private/qglextensions_p.h>
#include <private/qgl_p.h>
#include <private/qimagepixmapcleanuphooks_p.h>
#include <private/qapplication_p.h>
#include <private/qgraphicssystem_runtime_p.h>
#include <private/qimage_p.h>
#include <private/qeglproperties_p.h>
#include <private/qeglcontext_p.h>
#include <private/qpixmap_x11_p.h>

#include "qmeegopixmapdata.h"
#include "qmeegographicssystem.h"
#include "qmeegoextensions.h"

bool QMeeGoGraphicsSystem::surfaceWasCreated = false;

QHash <Qt::HANDLE, QPixmap*> QMeeGoGraphicsSystem::liveTexturePixmaps;

QMeeGoGraphicsSystem::QMeeGoGraphicsSystem()
{
    qDebug("Using the meego graphics system");
}

QMeeGoGraphicsSystem::~QMeeGoGraphicsSystem()
{
    qDebug("Meego graphics system destroyed");
    qt_destroy_gl_share_widget();
}

QWindowSurface* QMeeGoGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    QMeeGoGraphicsSystem::surfaceWasCreated = true;
    QWindowSurface *surface = new QGLWindowSurface(widget);
    return surface;
}

QPixmapData *QMeeGoGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    // Long story short: without this it's possible to hit an 
    // unitialized paintDevice due to a Qt bug too complex to even 
    // explain here... not to mention fix without going crazy. 
    // MDK
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
 
    return new QRasterPixmapData(type);
}

QPixmapData *QMeeGoGraphicsSystem::createPixmapData(QPixmapData *origin)
{
    // If the pixmap is a raster type...
    // and if the pixmap pointer matches our mapping...
    // create a shared image instead with the given handle.

    if (origin->classId() == QPixmapData::RasterClass) {
        QRasterPixmapData *rasterClass = static_cast <QRasterPixmapData *> (origin);
        void *rawResource = static_cast <void *> (rasterClass->buffer()->data_ptr()->data);

        if (QMeeGoPixmapData::sharedImagesMap.contains(rawResource))
            return new QMeeGoPixmapData();
    } 
        
    return new QRasterPixmapData(origin->pixelType());
}

QPixmapData* QMeeGoGraphicsSystem::wrapPixmapData(QPixmapData *pmd)
{
    QString name = QApplicationPrivate::instance()->graphics_system_name;
    if (name == "runtime") {
        QRuntimeGraphicsSystem *rsystem = (QRuntimeGraphicsSystem *) QApplicationPrivate::instance()->graphics_system;
        QRuntimePixmapData *rt = new QRuntimePixmapData(rsystem, pmd->pixelType());;
        rt->m_data = pmd;
        rt->readBackInfo();
        rsystem->m_pixmapDatas << rt;
        return rt;
    } else
        return pmd;
}

void QMeeGoGraphicsSystem::setSurfaceFixedSize(int /*width*/, int /*height*/)
{
    if (QMeeGoGraphicsSystem::surfaceWasCreated)
        qWarning("Trying to set surface fixed size but surface already created!");

#ifdef QT_WAS_PATCHED
    QEglProperties *properties = new QEglProperties();
    properties->setValue(EGL_FIXED_WIDTH_NOK, width);
    properties->setValue(EGL_FIXED_HEIGHT_NOK, height);
    QGLContextPrivate::setExtraWindowSurfaceCreationProps(properties);
#endif
}

void QMeeGoGraphicsSystem::setSurfaceScaling(int x, int y, int width, int height)
{
    QMeeGoExtensions::ensureInitialized();
    QMeeGoExtensions::eglSetSurfaceScalingNOK(QEgl::display(), QEglContext::currentContext(QEgl::OpenGL)->currentSurface, x, y, width, height);
}

void QMeeGoGraphicsSystem::setTranslucent(bool translucent)
{
    QGLWindowSurface::surfaceFormat.setSampleBuffers(false);
    QGLWindowSurface::surfaceFormat.setSamples(0);
    QGLWindowSurface::surfaceFormat.setAlpha(translucent);
}

QPixmapData *QMeeGoGraphicsSystem::pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    if (softImage.format() != QImage::Format_ARGB32_Premultiplied &&
        softImage.format() != QImage::Format_ARGB32) {
        qFatal("For egl shared images, the soft image has to be ARGB32 or ARGB32_Premultiplied");
        return NULL;
    }
    
    if (QMeeGoGraphicsSystem::meeGoRunning()) {
        QMeeGoPixmapData *pmd = new QMeeGoPixmapData;
        pmd->fromEGLSharedImage(handle, softImage);
        return QMeeGoGraphicsSystem::wrapPixmapData(pmd);
    } else {
        QRasterPixmapData *pmd = new QRasterPixmapData(QPixmapData::PixmapType);
        pmd->fromImage(softImage, Qt::NoOpaqueDetection);

        // Make sure that the image was not converted in any way
        if (pmd->buffer()->data_ptr()->data !=
            const_cast<QImage &>(softImage).data_ptr()->data)
            qFatal("Iternal misalignment of raster data detected. Prolly a QImage copy fail.");

        QMeeGoPixmapData::registerSharedImage(handle, softImage);
        return QMeeGoGraphicsSystem::wrapPixmapData(pmd);
    }
}

QPixmapData *QMeeGoGraphicsSystem::pixmapDataFromEGLImage(Qt::HANDLE handle)
{
    if (QMeeGoGraphicsSystem::meeGoRunning()) {
        QMeeGoPixmapData *pmd = new QMeeGoPixmapData;
        pmd->fromEGLImage(handle);

        // FIXME Ok. This is a bit BAD BAD BAD. We're abusing here the fact that we KNOW
        // that this function is used for the live pixmap...
        pmd->texture()->options &= ~QGLContext::InvertedYBindOption;
        return QMeeGoGraphicsSystem::wrapPixmapData(pmd);
    } else {
        qFatal("Can't create from EGL image when not running meego graphics system!");
        return NULL;
    }
}

void QMeeGoGraphicsSystem::updateEGLSharedImagePixmap(QPixmap *pixmap)
{
    QMeeGoPixmapData *pmd = (QMeeGoPixmapData *) pixmap->pixmapData();
    
    // Basic sanity check to make sure this is really a QMeeGoPixmapData...
    if (pmd->classId() != QPixmapData::OpenGLClass) 
        qFatal("Trying to updated EGLSharedImage pixmap but it's not really a shared image pixmap!");

    pmd->updateFromSoftImage();
}

QPixmapData *QMeeGoGraphicsSystem::pixmapDataWithGLTexture(int w, int h)
{
    QGLPixmapData *pmd = new QGLPixmapData(QPixmapData::PixmapType);
    pmd->resize(w, h);
    return QMeeGoGraphicsSystem::wrapPixmapData(pmd);
}

Qt::HANDLE QMeeGoGraphicsSystem::createLiveTexture(int w, int h, QImage::Format format)
{
    // No need to wrap the QPixmapData here. This QPixmap(Data) is a 
    // internal implementation and we don't migrate it between 
    // graphics system switching.
    
    // We use a bit ugly way of enforcing a color format on the X pixmap -- we create 
    // a local QImage and fromImage from there. This is quite redundant (extra overhead of creating 
    // the useless image only to delete it) but shouldn't be too bad for now... you're not expected
    // to call createLiveTexture too often anyways. Would be great if QX11PixmapData had a way to 
    // force the X format upon creation or resize.
    
    QImage image(w, h, format);
    QX11PixmapData *pmd = new QX11PixmapData(QPixmapData::PixmapType);
    pmd->fromImage(image, Qt::NoOpaqueDetection);
    QPixmap *p = new QPixmap(pmd);
    
    liveTexturePixmaps.insert(p->handle(), p);
    return p->handle();
}

void QMeeGoGraphicsSystem::destroyLiveTexture(Qt::HANDLE h)
{
    if (liveTexturePixmaps.contains(h)) {
        QPixmap *p = liveTexturePixmaps.value(h);
        delete p;
        liveTexturePixmaps.remove(h);
    } else
        qWarning("Trying to destroy live texture %ld which was not found!", h);
}

bool QMeeGoGraphicsSystem::lockLiveTexture(Qt::HANDLE h)
{
    if (! liveTexturePixmaps.contains(h)) {
        qWarning("Trying to lock live texture %ld which was not found!", h);
        return false;
    }

    EGLint attribs[] = {
        EGL_MAP_PRESERVE_PIXELS_KHR, EGL_TRUE,
        EGL_LOCK_USAGE_HINT_KHR, EGL_READ_SURFACE_BIT_KHR | EGL_WRITE_SURFACE_BIT_KHR, 
        EGL_NONE
    };

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    EGLSurface surface = getSurfaceForLiveTexturePixmap(liveTexturePixmaps.value(h));
    return QMeeGoExtensions::eglLockSurfaceKHR(QEgl::display(), surface, attribs);
}

bool QMeeGoGraphicsSystem::unlockLiveTexture(Qt::HANDLE h)
{
    if (! liveTexturePixmaps.contains(h)) {
        qWarning("Trying to lock live texture %ld which was not found!", h);
        return false;
    }

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    EGLSurface surface = getSurfaceForLiveTexturePixmap(liveTexturePixmaps.value(h));
    if (QMeeGoExtensions::eglUnlockSurfaceKHR(QEgl::display(), surface)) {
        glFinish();
        return true;
    } else {
        return false;
    }
}

void QMeeGoGraphicsSystem::queryLiveTexture(Qt::HANDLE h, void **data, int *pitch)
{
    // FIXME Only allow this on locked surfaces
    if (! liveTexturePixmaps.contains(h)) {
        qWarning("Trying to query live texture %ld which was not found!", h);
        return;
    }

    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    EGLSurface surface = getSurfaceForLiveTexturePixmap(liveTexturePixmaps.value(h));
    eglQuerySurface(QEgl::display(), surface, EGL_BITMAP_POINTER_KHR, (EGLint*) data);
    eglQuerySurface(QEgl::display(), surface, EGL_BITMAP_PITCH_KHR, (EGLint*) pitch);
}

Qt::HANDLE QMeeGoGraphicsSystem::liveTextureToEGLImage(Qt::HANDLE h)
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
    QMeeGoExtensions::ensureInitialized();

    EGLint attribs[] = {
        EGL_IMAGE_PRESERVED_KHR, EGL_TRUE,
        EGL_NONE
    };

    EGLImageKHR eglImage = QEgl::eglCreateImageKHR(QEgl::display(), EGL_NO_CONTEXT, EGL_NATIVE_PIXMAP_KHR,
                                                   (EGLClientBuffer) h, attribs);

    if (eglImage == EGL_NO_IMAGE_KHR) 
        qWarning("eglCreateImageKHR failed!");

    return (Qt::HANDLE) eglImage;
}

bool QMeeGoGraphicsSystem::meeGoRunning()
{
    if (! QApplicationPrivate::instance()) {
        qWarning("Application not running just yet... hard to know what system running!");
        return false;
    }

    QString name = QApplicationPrivate::instance()->graphics_system_name;
    if (name == "runtime") {
        QRuntimeGraphicsSystem *rsystem = (QRuntimeGraphicsSystem *) QApplicationPrivate::instance()->graphics_system;
        name = rsystem->graphicsSystemName();
    }

    return (name == "meego");
}

void QMeeGoGraphicsSystem::destroySurfaceForLiveTexturePixmap(QPixmapData* pmd)
{
    Q_ASSERT(pmd->classId() == QPixmapData::X11Class);
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pmd);
    if (pixmapData->gl_surface) {
        eglDestroySurface(QEgl::display(), (EGLSurface)pixmapData->gl_surface);
        pixmapData->gl_surface = 0;
    }
}

EGLSurface QMeeGoGraphicsSystem::getSurfaceForLiveTexturePixmap(QPixmap *pixmap)
{
    // This code is a crative remix of the stuff that can be found in the 
    // Qt's TFP implementation in /src/opengl/qgl_x11egl.cpp ::bindiTextureFromNativePixmap
    QX11PixmapData *pixmapData = static_cast<QX11PixmapData*>(pixmap->data_ptr().data());
    Q_ASSERT(pixmapData->classId() == QPixmapData::X11Class);
    bool hasAlpha = pixmapData->hasAlphaChannel();
   
    if (pixmapData->gl_surface &&
        hasAlpha == (pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha))
        return pixmapData->gl_surface;

    // Check to see if the surface is still valid
    if (pixmapData->gl_surface &&
        hasAlpha != ((pixmapData->flags & QX11PixmapData::GlSurfaceCreatedWithAlpha) > 0)) {
        // Surface is invalid!
        QMeeGoGraphicsSystem::destroySurfaceForLiveTexturePixmap(pixmapData);
    }

    if (pixmapData->gl_surface == 0) {
        EGLConfig config = QEgl::defaultConfig(QInternal::Pixmap,
                                               QEgl::OpenGL,
                                               hasAlpha ? QEgl::Translucent : QEgl::NoOptions);

        pixmapData->gl_surface = (void*)QEgl::createSurface(pixmap, config);
        
        if (hasAlpha)
            pixmapData->flags = pixmapData->flags | QX11PixmapData::GlSurfaceCreatedWithAlpha;
            
        if (pixmapData->gl_surface == (void*)EGL_NO_SURFACE)
            return NULL;
    }

    return pixmapData->gl_surface;
}

/* C API */

int qt_meego_image_to_egl_shared_image(const QImage &image)
{
    return QMeeGoPixmapData::imageToEGLSharedImage(image);
}

QPixmapData* qt_meego_pixmapdata_from_egl_shared_image(Qt::HANDLE handle, const QImage &softImage)
{
    return QMeeGoGraphicsSystem::pixmapDataFromEGLSharedImage(handle, softImage);
}

QPixmapData* qt_meego_pixmapdata_from_egl_image(Qt::HANDLE handle)
{
    return QMeeGoGraphicsSystem::pixmapDataFromEGLImage(handle);
}

QPixmapData* qt_meego_pixmapdata_with_gl_texture(int w, int h)
{
    return QMeeGoGraphicsSystem::pixmapDataWithGLTexture(w, h);
}

bool qt_meego_destroy_egl_shared_image(Qt::HANDLE handle)
{
    return QMeeGoPixmapData::destroyEGLSharedImage(handle);
}

void qt_meego_set_surface_fixed_size(int width, int height)
{
    QMeeGoGraphicsSystem::setSurfaceFixedSize(width, height);
}

void qt_meego_set_surface_scaling(int x, int y, int width, int height)
{
    QMeeGoGraphicsSystem::setSurfaceScaling(x, y, width, height);
}

void qt_meego_set_translucent(bool translucent)
{
    QMeeGoGraphicsSystem::setTranslucent(translucent);
}

void qt_meego_update_egl_shared_image_pixmap(QPixmap *pixmap)
{
    QMeeGoGraphicsSystem::updateEGLSharedImagePixmap(pixmap);
}

Qt::HANDLE qt_meego_live_texture_create(int w, int h, QImage::Format format)
{
    return QMeeGoGraphicsSystem::createLiveTexture(w, h, format);
}

void qt_meego_live_texture_destroy(Qt::HANDLE h)
{
    QMeeGoGraphicsSystem::destroyLiveTexture(h);
}

bool qt_meego_live_texture_lock(Qt::HANDLE h)
{
    return QMeeGoGraphicsSystem::lockLiveTexture(h);
}

bool qt_meego_live_texture_unlock(Qt::HANDLE h)
{
    return QMeeGoGraphicsSystem::unlockLiveTexture(h);
}

void qt_meego_live_texture_query(Qt::HANDLE h, void **data, int *pitch)
{
    return QMeeGoGraphicsSystem::queryLiveTexture(h, data, pitch);
}

Qt::HANDLE qt_meego_live_texture_to_egl_image(Qt::HANDLE h)
{
    return QMeeGoGraphicsSystem::liveTextureToEGLImage(h); 
}
