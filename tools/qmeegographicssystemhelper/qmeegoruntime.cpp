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

#include "mruntime.h"

#define ENSURE_INITIALIZED {if (!initialized) initialize();}

QLibrary* MRuntime::library = NULL;
bool MRuntime::initialized = false;

typedef int (*MImageToEglSharedImageFunc) (const QImage&);
typedef QPixmapData* (*MPixmapDataFromEglSharedImageFunc) (Qt::HANDLE handle, const QImage&);
typedef QPixmapData* (*MPixmapDataWithGLTextureFunc) (int w, int h);
typedef bool (*MDestroyEGLSharedImageFunc) (Qt::HANDLE handle);
typedef void (*MUpdateEglSharedImagePixmapFunc) (QPixmap*);
typedef void (*MSetSurfaceFixedSizeFunc) (int w, int h);
typedef void (*MSetSurfaceScalingFunc) (int x, int y, int w, int h);
typedef void (*MSetTranslucentFunc) (bool translucent);

static MImageToEglSharedImageFunc m_image_to_egl_shared_image = NULL;
static MPixmapDataFromEglSharedImageFunc m_pixmapdata_from_egl_shared_image = NULL;
static MPixmapDataWithGLTextureFunc m_pixmapdata_with_gl_texture = NULL;
static MDestroyEGLSharedImageFunc m_destroy_egl_shared_image = NULL;
static MUpdateEglSharedImagePixmapFunc m_update_egl_shared_image_pixmap = NULL;
static MSetSurfaceFixedSizeFunc m_set_surface_fixed_size = NULL;
static MSetSurfaceScalingFunc m_set_surface_scaling = NULL;
static MSetTranslucentFunc m_set_translucent = NULL;

void MRuntime::initialize()
{
    library = new QLibrary("/usr/lib/qt4/plugins/graphicssystems/libmeegographicssystem.so");
    Q_ASSERT(library);
    
    m_image_to_egl_shared_image = (MImageToEglSharedImageFunc) library->resolve("m_image_to_egl_shared_image");
    m_pixmapdata_from_egl_shared_image = (MPixmapDataFromEglSharedImageFunc)  library->resolve("m_pixmapdata_from_egl_shared_image");
    m_pixmapdata_with_gl_texture = (MPixmapDataWithGLTextureFunc) library->resolve("m_pixmapdata_with_gl_texture");
    m_destroy_egl_shared_image = (MDestroyEGLSharedImageFunc) library->resolve("m_destroy_egl_shared_image");
    m_update_egl_shared_image_pixmap = (MUpdateEglSharedImagePixmapFunc) library->resolve("m_update_egl_shared_image_pixmap");
    m_set_surface_fixed_size = (MSetSurfaceFixedSizeFunc) library->resolve("m_set_surface_fixed_size");
    m_set_surface_scaling = (MSetSurfaceScalingFunc) library->resolve("m_set_surface_scaling");
    m_set_translucent = (MSetTranslucentFunc) library->resolve("m_set_translucent");
    
    
    initialized = true;
}

Qt::HANDLE MRuntime::imageToEGLSharedImage(const QImage &image)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_image_to_egl_shared_image);
    return m_image_to_egl_shared_image(image);
}

QPixmap MRuntime::pixmapFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_pixmapdata_from_egl_shared_image);
    return QPixmap(m_pixmapdata_from_egl_shared_image(handle, softImage));
}

QPixmap MRuntime::pixmapWithGLTexture(int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_pixmapdata_with_gl_texture);
    return QPixmap(m_pixmapdata_with_gl_texture(w, h));
}

bool MRuntime::destroyEGLSharedImage(Qt::HANDLE handle)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_destroy_egl_shared_image);
    return m_destroy_egl_shared_image(handle);
}

void MRuntime::updateEGLSharedImagePixmap(QPixmap *p)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_update_egl_shared_image_pixmap);
    return m_update_egl_shared_image_pixmap(p);
}

void MRuntime::setSurfaceFixedSize(int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_set_surface_fixed_size);
    m_set_surface_fixed_size(w, h);
}

void MRuntime::setSurfaceScaling(int x, int y, int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_set_surface_scaling);
    m_set_surface_scaling(x, y, w, h);
}

void MRuntime::setTranslucent(bool translucent)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(m_set_translucent);
    m_set_translucent(translucent);
}
