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

#define ENSURE_INITIALIZED {if (!initialized) initialize();}

bool QMeeGoRuntime::initialized = false;

typedef int (*QMeeGoImageToEglSharedImageFunc) (const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataFromEglSharedImageFunc) (Qt::HANDLE handle, const QImage&);
typedef QPixmapData* (*QMeeGoPixmapDataFromEglImageFunc) (Qt::HANDLE handle);
typedef QPixmapData* (*QMeeGoPixmapDataWithGLTextureFunc) (int w, int h);
typedef bool (*QMeeGoDestroyEGLSharedImageFunc) (Qt::HANDLE handle);
typedef void (*QMeeGoUpdateEglSharedImagePixmapFunc) (QPixmap*);
typedef void (*QMeeGoSetSurfaceFixedSizeFunc) (int w, int h);
typedef void (*QMeeGoSetSurfaceScalingFunc) (int x, int y, int w, int h);
typedef void (*QMeeGoSetTranslucentFunc) (bool translucent);
typedef Qt::HANDLE (*QMeeGoLiveTextureCreateFunc) (int w, int h, QImage::Format format);
typedef bool (*QMeeGoLiveTextureLockFunc) (Qt::HANDLE h);
typedef bool (*QMeeGoLiveTextureUnlockFunc) (Qt::HANDLE h);
typedef void (*QMeeGoLiveTextureDestroyFunc) (Qt::HANDLE h);
typedef void (*QMeeGoLiveTextureQueryFunc) (Qt::HANDLE h, void **data, int *pitch);
typedef Qt::HANDLE (*QMeeGoLiveTextureToEglImageFunc) (Qt::HANDLE h);

static QMeeGoImageToEglSharedImageFunc qt_meego_image_to_egl_shared_image = NULL;
static QMeeGoPixmapDataFromEglSharedImageFunc qt_meego_pixmapdata_from_egl_shared_image = NULL;
static QMeeGoPixmapDataFromEglImageFunc qt_meego_pixmapdata_from_egl_image = NULL;
static QMeeGoPixmapDataWithGLTextureFunc qt_meego_pixmapdata_with_gl_texture = NULL;
static QMeeGoDestroyEGLSharedImageFunc qt_meego_destroy_egl_shared_image = NULL;
static QMeeGoUpdateEglSharedImagePixmapFunc qt_meego_update_egl_shared_image_pixmap = NULL;
static QMeeGoSetSurfaceFixedSizeFunc qt_meego_set_surface_fixed_size = NULL;
static QMeeGoSetSurfaceScalingFunc qt_meego_set_surface_scaling = NULL;
static QMeeGoSetTranslucentFunc qt_meego_set_translucent = NULL;
static QMeeGoLiveTextureCreateFunc qt_meego_live_texture_create = NULL;
static QMeeGoLiveTextureLockFunc qt_meego_live_texture_lock = NULL;
static QMeeGoLiveTextureUnlockFunc qt_meego_live_texture_unlock = NULL;
static QMeeGoLiveTextureDestroyFunc qt_meego_live_texture_destroy = NULL;
static QMeeGoLiveTextureQueryFunc qt_meego_live_texture_query = NULL;
static QMeeGoLiveTextureToEglImageFunc qt_meego_live_texture_to_egl_image = NULL;


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
        qt_meego_pixmapdata_from_egl_image = (QMeeGoPixmapDataFromEglImageFunc) library.resolve("qt_meego_pixmapdata_from_egl_image");
        qt_meego_pixmapdata_with_gl_texture = (QMeeGoPixmapDataWithGLTextureFunc) library.resolve("qt_meego_pixmapdata_with_gl_texture");
        qt_meego_destroy_egl_shared_image = (QMeeGoDestroyEGLSharedImageFunc) library.resolve("qt_meego_destroy_egl_shared_image");
        qt_meego_update_egl_shared_image_pixmap = (QMeeGoUpdateEglSharedImagePixmapFunc) library.resolve("qt_meego_update_egl_shared_image_pixmap");
        qt_meego_set_surface_fixed_size = (QMeeGoSetSurfaceFixedSizeFunc) library.resolve("qt_meego_set_surface_fixed_size");
        qt_meego_set_surface_scaling = (QMeeGoSetSurfaceScalingFunc) library.resolve("qt_meego_set_surface_scaling");
        qt_meego_set_translucent = (QMeeGoSetTranslucentFunc) library.resolve("qt_meego_set_translucent");
        qt_meego_live_texture_create = (QMeeGoLiveTextureCreateFunc) library.resolve("qt_meego_live_texture_create");
        qt_meego_live_texture_lock = (QMeeGoLiveTextureLockFunc) library.resolve("qt_meego_live_texture_lock");
        qt_meego_live_texture_unlock = (QMeeGoLiveTextureUnlockFunc) library.resolve("qt_meego_live_texture_unlock");
        qt_meego_live_texture_destroy = (QMeeGoLiveTextureDestroyFunc) library.resolve("qt_meego_live_texture_destroy");
        qt_meego_live_texture_query = (QMeeGoLiveTextureQueryFunc) library.resolve("qt_meego_live_texture_query");
        qt_meego_live_texture_to_egl_image = (QMeeGoLiveTextureToEglImageFunc) library.resolve("qt_meego_live_texture_to_egl_image");

        if (qt_meego_image_to_egl_shared_image && qt_meego_pixmapdata_from_egl_shared_image && qt_meego_pixmapdata_from_egl_image && 
            qt_meego_pixmapdata_with_gl_texture && qt_meego_destroy_egl_shared_image && qt_meego_update_egl_shared_image_pixmap && 
            qt_meego_set_surface_fixed_size && qt_meego_set_surface_scaling && qt_meego_set_translucent && 
            qt_meego_live_texture_create && qt_meego_live_texture_lock && qt_meego_live_texture_unlock && 
            qt_meego_live_texture_destroy && qt_meego_live_texture_query && qt_meego_live_texture_to_egl_image)
        {
            qDebug("Successfully resolved MeeGo graphics system: %s %s\n", qPrintable(libraryPrivate->fileName), qPrintable(libraryPrivate->fullVersion));
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

QPixmapData* QMeeGoRuntime::pixmapDataFromEGLImage(Qt::HANDLE handle)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_pixmapdata_from_egl_image);
    return qt_meego_pixmapdata_from_egl_image(handle);
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

Qt::HANDLE QMeeGoRuntime::createLiveTexture(int w, int h, QImage::Format format)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_create);
    return qt_meego_live_texture_create(w, h, format);
}

bool QMeeGoRuntime::lockLiveTexture(Qt::HANDLE h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_lock);
    return qt_meego_live_texture_lock(h);
}

bool QMeeGoRuntime::unlockLiveTexture(Qt::HANDLE h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_unlock);
    return qt_meego_live_texture_unlock(h);
}

void QMeeGoRuntime::destroyLiveTexture(Qt::HANDLE h)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_destroy);
    qt_meego_live_texture_destroy(h);
}

void QMeeGoRuntime::queryLiveTexture(Qt::HANDLE h, void **data, int *pitch)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_query);
    qt_meego_live_texture_query(h, data, pitch);
}

Qt::HANDLE QMeeGoRuntime::liveTextureToEGLImage(Qt::HANDLE handle)
{
    ENSURE_INITIALIZED;
    Q_ASSERT(qt_meego_live_texture_to_egl_image);
    return qt_meego_live_texture_to_egl_image(handle);
}

