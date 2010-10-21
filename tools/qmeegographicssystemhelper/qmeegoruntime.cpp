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

#include "qmeegoruntime.h"

#include <private/qlibrary_p.h>
#include <private/qfactoryloader_p.h>
#include <private/qgraphicssystemplugin_p.h>
#include <stdio.h>

#define ENSURE_INITIALIZED {if (!initialized) initialize();}

bool QMeeGoRuntime::initialized = false;

typedef int (*QMeeGoImageToEglSharedImageFunc) (const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataFromEglSharedImageFunc) (Qt::HANDLE handle, const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataWithGLTextureFunc) (int w, int h);
typedef bool (*QMeeGoDestroyEGLSharedImageFunc) (Qt::HANDLE handle);
typedef void (*QMeeGoUpdateEglSharedImagePixmapFunc) (QPixmap*);
typedef void (*QMeeGoSetSurfaceFixedSizeFunc) (int w, int h);
typedef void (*QMeeGoSetSurfaceScalingFunc) (int x, int y, int w, int h);
typedef void (*QMeeGoSetTranslucentFunc) (bool translucent);
typedef QPixmapData* (*QMeeGoPixmapDataWithNewLiveTextureFunc) (int w, int h, QImage::Format format);
typedef QPixmapData* (*QMeeGoPixmapDataFromLiveTextureHandleFunc) (Qt::HANDLE h);
typedef QImage* (*QMeeGoLiveTextureLockFunc) (QPixmap*);
typedef bool (*QMeeGoLiveTextureReleaseFunc) (QPixmap*, QImage *i);
typedef Qt::HANDLE (*QMeeGoLiveTextureGetHandleFunc) (QPixmap*);

static QMeeGoImageToEglSharedImageFunc qt_meego_image_to_egl_shared_image = NULL;
static QMeeGoPixmapDataFromEglSharedImageFunc qt_meego_pixmapdata_from_egl_shared_image = NULL;
static QMeeGoPixmapDataWithGLTextureFunc qt_meego_pixmapdata_with_gl_texture = NULL;
static QMeeGoDestroyEGLSharedImageFunc qt_meego_destroy_egl_shared_image = NULL;
static QMeeGoUpdateEglSharedImagePixmapFunc qt_meego_update_egl_shared_image_pixmap = NULL;
static QMeeGoSetSurfaceFixedSizeFunc qt_meego_set_surface_fixed_size = NULL;
static QMeeGoSetSurfaceScalingFunc qt_meego_set_surface_scaling = NULL;
static QMeeGoSetTranslucentFunc qt_meego_set_translucent = NULL;
static QMeeGoPixmapDataWithNewLiveTextureFunc qt_meego_pixmapdata_with_new_live_texture = NULL;
static QMeeGoPixmapDataFromLiveTextureHandleFunc qt_meego_pixmapdata_from_live_texture_handle = NULL;
static QMeeGoLiveTextureLockFunc qt_meego_live_texture_lock = NULL;
static QMeeGoLiveTextureReleaseFunc qt_meego_live_texture_release = NULL;
static QMeeGoLiveTextureGetHandleFunc qt_meego_live_texture_get_handle = NULL;

void QMeeGoRuntime::initialize()
{
    QFactoryLoader loader(QGraphicsSystemFactoryInterface_iid, QLatin1String("/graphicssystems"), Qt::CaseInsensitive);

    QLibraryPrivate *libraryPrivate = loader.library(QLatin1String("meego"));
    Q_ASSERT(libraryPrivate);

    QLibrary library(libraryPrivate->fileName, libraryPrivate->fullVersion);

    bool success = library.load();

    if (success) {
        qt_meego_image_to_egl_shared_image = (QMeeGoImageToEglSharedImageFunc) library.resolve("qt_meego_image_to_egl_shared_image");
        qt_meego_pixmapdata_from_egl_shared_image = (QMeeGoPixmapDataFromEglSharedImageFunc) library.resolve("qt_meego_pixmapdata_from_egl_shared_image");
        qt_meego_pixmapdata_with_gl_texture = (QMeeGoPixmapDataWithGLTextureFunc) library.resolve("qt_meego_pixmapdata_with_gl_texture");
        qt_meego_destroy_egl_shared_image = (QMeeGoDestroyEGLSharedImageFunc) library.resolve("qt_meego_destroy_egl_shared_image");
        qt_meego_update_egl_shared_image_pixmap = (QMeeGoUpdateEglSharedImagePixmapFunc) library.resolve("qt_meego_update_egl_shared_image_pixmap");
        qt_meego_set_surface_fixed_size = (QMeeGoSetSurfaceFixedSizeFunc) library.resolve("qt_meego_set_surface_fixed_size");
        qt_meego_set_surface_scaling = (QMeeGoSetSurfaceScalingFunc) library.resolve("qt_meego_set_surface_scaling");
        qt_meego_set_translucent = (QMeeGoSetTranslucentFunc) library.resolve("qt_meego_set_translucent");
        qt_meego_pixmapdata_with_new_live_texture = (QMeeGoPixmapDataWithNewLiveTextureFunc) library.resolve("qt_meego_pixmapdata_with_new_live_texture");
        qt_meego_pixmapdata_from_live_texture_handle = (QMeeGoPixmapDataFromLiveTextureHandleFunc) library.resolve("qt_meego_pixmapdata_from_live_texture_handle");
        qt_meego_live_texture_lock = (QMeeGoLiveTextureLockFunc) library.resolve("qt_meego_live_texture_lock");
        qt_meego_live_texture_release = (QMeeGoLiveTextureReleaseFunc) library.resolve("qt_meego_live_texture_release");
        qt_meego_live_texture_get_handle = (QMeeGoLiveTextureGetHandleFunc) library.resolve("qt_meego_live_texture_get_handle");

        if (qt_meego_image_to_egl_shared_image && qt_meego_pixmapdata_from_egl_shared_image && 
            qt_meego_pixmapdata_with_gl_texture && qt_meego_destroy_egl_shared_image && qt_meego_update_egl_shared_image_pixmap && 
            qt_meego_set_surface_fixed_size && qt_meego_set_surface_scaling && qt_meego_set_translucent && 
            qt_meego_pixmapdata_with_new_live_texture && qt_meego_pixmapdata_from_live_texture_handle &&
            qt_meego_live_texture_lock && qt_meego_live_texture_release && qt_meego_live_texture_get_handle)
        {
            qDebug("Successfully resolved MeeGo graphics system: %s %s\n", qPrintable(libraryPrivate->fileName), qPrintable(libraryPrivate->fullVersion));
        } else {
            Q_ASSERT(false);
        }
    } else {
        Q_ASSERT(false);
    }

    initialized = true;
}

Qt::HANDLE QMeeGoRuntime::imageToEGLSharedImage(const QImage &image)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_image_to_egl_shared_image);
    return qt_meego_image_to_egl_shared_image(image);
}

QPixmapData* QMeeGoRuntime::pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_from_egl_shared_image);
    return qt_meego_pixmapdata_from_egl_shared_image(handle, softImage);
}

QPixmapData* QMeeGoRuntime::pixmapDataWithGLTexture(int w, int h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_with_gl_texture);
    return qt_meego_pixmapdata_with_gl_texture(w, h);
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

QPixmapData* QMeeGoRuntime::pixmapDataWithNewLiveTexture(int w, int h, QImage::Format format)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_with_new_live_texture);
    return qt_meego_pixmapdata_with_new_live_texture(w, h, format);
}

QPixmapData* QMeeGoRuntime::pixmapDataFromLiveTextureHandle(Qt::HANDLE h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_from_live_texture_handle);
    return qt_meego_pixmapdata_from_live_texture_handle(h);
}

QImage* QMeeGoRuntime::lockLiveTexture(QPixmap *p)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_lock);
    return qt_meego_live_texture_lock(p);
}

bool QMeeGoRuntime::releaseLiveTexture(QPixmap *p, QImage *i)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_release);
    return qt_meego_live_texture_release(p, i);
}

Qt::HANDLE QMeeGoRuntime::getLiveTextureHandle(QPixmap *pixmap)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_get_handle);
    return qt_meego_live_texture_get_handle(pixmap);
}
