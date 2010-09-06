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

#ifndef MPIXMAPDATA_H
#define MPIXMAPDATA_H

#include "../private/qpixmapdata_gl_p.h"

struct MImageInfo
{
    Qt::HANDLE handle;
    QImage::Format rawFormat;
};

class MPixmapData : public QGLPixmapData
{
public:
    MPixmapData();
    void fromTexture(GLuint textureId, int w, int h, bool alpha);

    virtual void fromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage);
    virtual void fromImage (const QImage &image, Qt::ImageConversionFlags flags);
    virtual QImage toImage() const;
    virtual void updateFromSoftImage();

    QImage softImage;
    
    static QHash <void*, MImageInfo*> sharedImagesMap;
    
    static Qt::HANDLE imageToEGLSharedImage(const QImage &image);
    static bool destroyEGLSharedImage(Qt::HANDLE h);
    static void registerSharedImage(Qt::HANDLE handle, const QImage &si);
};

#endif
