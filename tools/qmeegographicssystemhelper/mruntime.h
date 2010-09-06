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

#include <QLibrary>
#include <QPixmap>
#include <QImage>

class MRuntime
{
public:
    static void initialize();
    
    static Qt::HANDLE imageToEGLSharedImage(const QImage &image);
    static QPixmap pixmapFromEGLSharedImage(Qt::HANDLE handle, const QImage &softImage);
    static QPixmap pixmapWithGLTexture(int w, int h);
    static bool destroyEGLSharedImage(Qt::HANDLE handle);
    static void updateEGLSharedImagePixmap(QPixmap *p);
    static void setSurfaceFixedSize(int w, int h);
    static void setSurfaceScaling(int x, int y, int w, int h);
    static void setTranslucent(bool translucent);

private:
    static bool initialized;
    static QLibrary *library;  
};