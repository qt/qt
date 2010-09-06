/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QDebug>
#include "../private/qpixmap_raster_p.h"
#include "../private/qwindowsurface_gl_p.h"
#include "../private/qegl_p.h"
#include "../private/qglextensions_p.h"
#include "../private/qgl_p.h"
#include "../private/qimagepixmapcleanuphooks_p.h"
#include "../private/qapplication_p.h"
#include "../private/qgraphicssystem_runtime_p.h"
#include "../private/qimage_p.h"
#include "../private/qeglproperties_p.h"
#include "../private/qeglcontext_p.h"

#include "mpixmapdata.h"
#include "mgraphicssystem.h"
#include "mextensions.h"

bool MGraphicsSystem::surfaceWasCreated = false;

MGraphicsSystem::MGraphicsSystem()
{
    qDebug("Using the meego graphics system");
}

MGraphicsSystem::~MGraphicsSystem()
{
    qDebug("Meego graphics system destroyed");
    qt_destroy_gl_share_widget();
}

QWindowSurface* MGraphicsSystem::createWindowSurface(QWidget *widget) const
{
    MGraphicsSystem::surfaceWasCreated = true;
    QWindowSurface *surface = new QGLWindowSurface(widget);
    surface->window()->setAttribute(Qt::WA_NoSystemBackground);
    return surface;
}

QPixmapData *MGraphicsSystem::createPixmapData(QPixmapData::PixelType type) const
{
    // Long story short: without this it's possible to hit an 
    // unitialized paintDevice due to a Qt bug too complex to even 
    // explain here... not to mention fix without going crazy. 
    // MDK
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
 
    return new QRasterPixmapData(type);
}

QPixmapData *MGraphicsSystem::createPixmapData(QPixmapData *origin)
{
    // If the pixmap is a raster type...
    // and if the pixmap pointer matches our mapping...
    // create a shared image instead with the given handle.

    if (origin->classId() == QPixmapData::RasterClass) {
        QRasterPixmapData *rasterClass = static_cast <QRasterPixmapData *> (origin);
        void *rawResource = static_cast <void *> (rasterClass->buffer()->data_ptr()->data);

        if (MPixmapData::sharedImagesMap.contains(rawResource))
            return new MPixmapData();
    } 
        
    return new QRasterPixmapData(origin->pixelType());
}

QPixmapData* MGraphicsSystem::wrapPixmapData(QPixmapData *pmd)
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

void MGraphicsSystem::setSurfaceFixedSize(int /*width*/, int /*height*/)
{
    if (MGraphicsSystem::surfaceWasCreated)
        qWarning("Trying to set surface fixed size but surface already created!");

#ifdef QT_WAS_PATCHED
    QEglProperties *properties = new QEglProperties();
    properties->setValue(EGL_FIXED_WIDTH_NOK, width);
    properties->setValue(EGL_FIXED_HEIGHT_NOK, height);
    QGLContextPrivate::setExtraWindowSurfaceCreationProps(properties);
#endif
}

void MGraphicsSystem::setSurfaceScaling(int x, int y, int width, int height)
{
    MExtensions::ensureInitialized();
    MExtensions::eglSetSurfaceScalingNOK(QEgl::display(), QEglContext::currentContext(QEgl::OpenGL)->currentSurface, x, y, width, height);
}

void MGraphicsSystem::setTranslucent(bool translucent)
{
    QGLWindowSurface::surfaceFormat.setSampleBuffers(false);
    QGLWindowSurface::surfaceFormat.setSamples(0);
    QGLWindowSurface::surfaceFormat.setAlpha(translucent);
}

QPixmapData *MGraphicsSystem::pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    if (softImage.format() != QImage::Format_ARGB32_Premultiplied &&
        softImage.format() != QImage::Format_ARGB32) {
        qFatal("For egl shared images, the soft image has to be ARGB32 or ARGB32_Premultiplied");
        return NULL;
    }
    
    if (MGraphicsSystem::meegoRunning()) {
        MPixmapData *pmd = new MPixmapData;
        pmd->fromEGLSharedImage(handle, softImage);
        return MGraphicsSystem::wrapPixmapData(pmd);
    } else {
        QRasterPixmapData *pmd = new QRasterPixmapData(QPixmapData::PixmapType);
        pmd->fromImage(softImage, Qt::NoOpaqueDetection);

        // Make sure that the image was not converted in any way
        if (pmd->buffer()->data_ptr()->data !=
            const_cast<QImage &>(softImage).data_ptr()->data)
            qFatal("Iternal misalignment of raster data detected. Prolly a QImage copy fail.");

        MPixmapData::registerSharedImage(handle, softImage);
        return MGraphicsSystem::wrapPixmapData(pmd);
    }
}

void MGraphicsSystem::updateEGLSharedImagePixmap(QPixmap *pixmap)
{
    MPixmapData *pmd = (MPixmapData *) pixmap->pixmapData();
    
    // Basic sanity check to make sure this is really a MPixmapData...
    if (pmd->classId() != QPixmapData::OpenGLClass) 
        qFatal("Trying to updated EGLSharedImage pixmap but it's not really a shared image pixmap!");

    pmd->updateFromSoftImage();
}

QPixmapData *MGraphicsSystem::pixmapDataWithGLTexture(int w, int h)
{
    QGLPixmapData *pmd = new QGLPixmapData(QPixmapData::PixmapType);
    pmd->resize(w, h);
    return MGraphicsSystem::wrapPixmapData(pmd);
}

bool MGraphicsSystem::meegoRunning()
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

/* C API */

int m_image_to_egl_shared_image(const QImage &image)
{
    return MPixmapData::imageToEGLSharedImage(image);
}

QPixmapData* m_pixmapdata_from_egl_shared_image(Qt::HANDLE handle, const QImage &softImage)
{
    return MGraphicsSystem::pixmapDataFromEGLSharedImage(handle, softImage);
}

QPixmapData* m_pixmapdata_with_gl_texture(int w, int h)
{
    return MGraphicsSystem::pixmapDataWithGLTexture(w, h);
}

bool m_destroy_egl_shared_image(Qt::HANDLE handle)
{
    return MPixmapData::destroyEGLSharedImage(handle);
}

void m_set_surface_fixed_size(int width, int height)
{
    MGraphicsSystem::setSurfaceFixedSize(width, height);
}

void m_set_surface_scaling(int x, int y, int width, int height)
{
    MGraphicsSystem::setSurfaceScaling(x, y, width, height);
}

void m_set_translucent(bool translucent)
{
    MGraphicsSystem::setTranslucent(translucent);
}

void m_update_egl_shared_image_pixmap(QPixmap *pixmap)
{
    MGraphicsSystem::updateEGLSharedImagePixmap(pixmap);
}
