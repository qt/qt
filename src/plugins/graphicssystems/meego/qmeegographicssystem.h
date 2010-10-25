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

#ifndef MGRAPHICSSYSTEM_H
#define MGRAPHICSSYSTEM_H

#include <private/qgraphicssystem_p.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

class QMeeGoGraphicsSystem : public QGraphicsSystem
{
public:
    QMeeGoGraphicsSystem();
    ~QMeeGoGraphicsSystem();

    virtual QWindowSurface *createWindowSurface(QWidget *widget) const;
    virtual QPixmapData *createPixmapData(QPixmapData::PixelType) const;
    virtual QPixmapData *createPixmapData(QPixmapData *origin);

    static QPixmapData *wrapPixmapData(QPixmapData *pmd);
    static void setSurfaceFixedSize(int width, int height);
    static void setSurfaceScaling(int x, int y, int width, int height);
    static void setTranslucent(bool translucent);

    static QPixmapData *pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage);
    static QPixmapData *pixmapDataFromEGLImage(Qt::HANDLE handle);
    static QPixmapData *pixmapDataWithGLTexture(int w, int h);
    static void updateEGLSharedImagePixmap(QPixmap *pixmap);

    static QPixmapData *pixmapDataWithNewLiveTexture(int w, int h, QImage::Format format);
    static QPixmapData *pixmapDataFromLiveTextureHandle(Qt::HANDLE handle);
    static QImage *lockLiveTexture(QPixmap* pixmap);
    static bool releaseLiveTexture(QPixmap *pixmap, QImage *image);
    static Qt::HANDLE getLiveTextureHandle(QPixmap *pixmap);

private:
    static bool meeGoRunning();
    static EGLSurface getSurfaceForLiveTexturePixmap(QPixmap *pixmap);
    static void destroySurfaceForLiveTexturePixmap(QPixmapData* pmd);

    static bool surfaceWasCreated;
    static QHash <Qt::HANDLE, QPixmap*> liveTexturePixmaps;
};

/* C api */

extern "C" {
    Q_DECL_EXPORT int qt_meego_image_to_egl_shared_image(const QImage &image);
    Q_DECL_EXPORT QPixmapData* qt_meego_pixmapdata_from_egl_shared_image(Qt::HANDLE handle, const QImage &softImage);
    Q_DECL_EXPORT QPixmapData* qt_meego_pixmapdata_with_gl_texture(int w, int h);
    Q_DECL_EXPORT void qt_meego_update_egl_shared_image_pixmap(QPixmap *pixmap);
    Q_DECL_EXPORT bool qt_meego_destroy_egl_shared_image(Qt::HANDLE handle);
    Q_DECL_EXPORT void qt_meego_set_surface_fixed_size(int width, int height);
    Q_DECL_EXPORT void qt_meego_set_surface_scaling(int x, int y, int width, int height);
    Q_DECL_EXPORT void qt_meego_set_translucent(bool translucent);    
    Q_DECL_EXPORT QPixmapData* qt_meego_pixmapdata_with_new_live_texture(int w, int h, QImage::Format format);
    Q_DECL_EXPORT QPixmapData* qt_meego_pixmapdata_from_live_texture_handle(Qt::HANDLE handle);
    Q_DECL_EXPORT QImage* qt_meego_live_texture_lock(QPixmap *pixmap);
    Q_DECL_EXPORT bool qt_meego_live_texture_release(QPixmap *pixmap, QImage *image);
    Q_DECL_EXPORT Qt::HANDLE qt_meego_live_texture_get_handle(QPixmap *pixmap);
}

#endif 
