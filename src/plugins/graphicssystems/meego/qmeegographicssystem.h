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

#ifndef MGRAPHICSSYSTEM_H
#define MGRAPHICSSYSTEM_H

#include "../private/qgraphicssystem_p.h"

class MGraphicsSystem : public QGraphicsSystem
{
public:
    MGraphicsSystem();
    ~MGraphicsSystem();
    
    virtual QWindowSurface *createWindowSurface(QWidget *widget) const;
    virtual QPixmapData *createPixmapData(QPixmapData::PixelType) const;
    virtual QPixmapData *createPixmapData(QPixmapData *origin);

    static QPixmapData *wrapPixmapData(QPixmapData *pmd);
    static void setSurfaceFixedSize(int width, int height);
    static void setSurfaceScaling(int x, int y, int width, int height);
    static void setTranslucent(bool translucent);

    static QPixmapData *pixmapDataFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage);
    static QPixmapData *pixmapDataWithGLTexture(int w, int h);
    static void updateEGLSharedImagePixmap(QPixmap *pixmap);

private:
    static bool meegoRunning();
    
    static bool surfaceWasCreated;
};

/* C api */

extern "C" {
    int m_image_to_egl_shared_image(const QImage &image);
    QPixmapData* m_pixmapdata_from_egl_shared_image(Qt::HANDLE handle, const QImage &softImage);
    QPixmapData* m_pixmapdata_with_gl_texture(int w, int h);
    void m_update_egl_shared_image_pixmap(QPixmap *pixmap);
    bool m_destroy_egl_shared_image(Qt::HANDLE handle);
    void m_set_surface_fixed_size(int width, int height);
    void m_set_surface_scaling(int x, int y, int width, int height);
    void m_set_translucent(bool translucent);
}

#endif 
