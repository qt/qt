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

#include "qmeegoruntime.h"

#define ENSURE_INITIALIZED {if (!initialized) initialize();}

QLibrary* QMeeGoRuntime::library = NULL;
bool QMeeGoRuntime::initialized = false;

typedef int (*QMeeGoImageToEglSharedImageFunc) (const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataFromEglSharedImageFunc) (Qt::HANDLE handle, const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataWithGLTextureFunc) (int w, int h);
typedef bool (*QMeeGoDestroyEGLSharedImageFunc) (Qt::HANDLE handle);
typedef void (*QMeeGoUpdateEglSharedImagePixmapFunc) (QPixmap*);
typedef void (*QMeeGoSetSurfaceFixedSizeFunc) (int w, int h);
typedef void (*QMeeGoSetSurfaceScalingFunc) (int x, int y, int w, int h);
typedef void (*QMeeGoSetTranslucentFunc) (bool translucent);

static QMeeGoImageToEglSharedImageFunc qt_meego_image_to_egl_shared_image = NULL;
static QMeeGoPixmapDataFromEglSharedImageFunc qt_meego_pixmapdata_from_egl_shared_image = NULL;
static QMeeGoPixmapDataWithGLTextureFunc qt_meego_pixmapdata_with_gl_texture = NULL;
static QMeeGoDestroyEGLSharedImageFunc qt_meego_destroy_egl_shared_image = NULL;
static QMeeGoUpdateEglSharedImagePixmapFunc qt_meego_update_egl_shared_image_pixmap = NULL;
static QMeeGoSetSurfaceFixedSizeFunc qt_meego_set_surface_fixed_size = NULL;
static QMeeGoSetSurfaceScalingFunc qt_meego_set_surface_scaling = NULL;
static QMeeGoSetTranslucentFunc qt_meego_set_translucent = NULL;

void QMeeGoRuntime::initialize()
{
    library = new QLibrary("/usr/lib/qt4/plugins/graphicssystems/libmeegographicssystem.so");
    Q_ASSERT(library);
    
    qt_meego_image_to_egl_shared_image = (QMeeGoImageToEglSharedImageFunc) library->resolve("qt_meego_image_to_egl_shared_image");
    qt_meego_pixmapdata_from_egl_shared_image = (QMeeGoPixmapDataFromEglSharedImageFunc)  library->resolve("qt_meego_pixmapdata_from_egl_shared_image");
    qt_meego_pixmapdata_with_gl_texture = (QMeeGoPixmapDataWithGLTextureFunc) library->resolve("qt_meego_pixmapdata_with_gl_texture");
    qt_meego_destroy_egl_shared_image = (QMeeGoDestroyEGLSharedImageFunc) library->resolve("qt_meego_destroy_egl_shared_image");
    qt_meego_update_egl_shared_image_pixmap = (QMeeGoUpdateEglSharedImagePixmapFunc) library->resolve("qt_meego_update_egl_shared_image_pixmap");
    qt_meego_set_surface_fixed_size = (QMeeGoSetSurfaceFixedSizeFunc) library->resolve("qt_meego_set_surface_fixed_size");
    qt_meego_set_surface_scaling = (QMeeGoSetSurfaceScalingFunc) library->resolve("qt_meego_set_surface_scaling");
    qt_meego_set_translucent = (QMeeGoSetTranslucentFunc) library->resolve("qt_meego_set_translucent");
    
    
    initialized = true;
}

Qt::HANDLE QMeeGoRuntime::imageToEGLSharedImage(const QImage &image)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_image_to_egl_shared_image);
    return qt_meego_image_to_egl_shared_image(image);
}

QPixmap QMeeGoRuntime::pixmapFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_from_egl_shared_image);
    return QPixmap(qt_meego_pixmapdata_from_egl_shared_image(handle, softImage));
}

QPixmap QMeeGoRuntime::pixmapWithGLTexture(int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_with_gl_texture);
    return QPixmap(qt_meego_pixmapdata_with_gl_texture(w, h));
}

bool QMeeGoRuntime::destroyEGLSharedImage(Qt::HANDLE handle)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_destroy_egl_shared_image);
    return qt_meego_destroy_egl_shared_image(handle);
}

void QMeeGoRuntime::updateEGLSharedImagePixmap(QPixmap *p)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_update_egl_shared_image_pixmap);
    return qt_meego_update_egl_shared_image_pixmap(p);
}

void QMeeGoRuntime::setSurfaceFixedSize(int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_set_surface_fixed_size);
    qt_meego_set_surface_fixed_size(w, h);
}

void QMeeGoRuntime::setSurfaceScaling(int x, int y, int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_set_surface_scaling);
    qt_meego_set_surface_scaling(x, y, w, h);
}

void QMeeGoRuntime::setTranslucent(bool translucent)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_set_translucent);
    qt_meego_set_translucent(translucent);
}
