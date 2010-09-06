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

#include "mpixmapdata.h"
#include "mextensions.h"
#include "../private/qimage_p.h"
#include "../private/qwindowsurface_gl_p.h"
#include "../private/qeglcontext_p.h"
#include "../private/qapplication_p.h"
#include "../private/qgraphicssystem_runtime_p.h"

static EGLint preserved_image_attribs[] = { EGL_IMAGE_PRESERVED_KHR, EGL_TRUE, EGL_NONE };

QHash <void*, MImageInfo*> MPixmapData::sharedImagesMap;

/* Public */

MPixmapData::MPixmapData() : QGLPixmapData(QPixmapData::PixmapType)
{
}

void MPixmapData::fromTexture(GLuint textureId, int w, int h, bool alpha)
{
    resize(w, h); 
    texture()->id = textureId;
    m_hasAlpha = alpha;
    softImage = QImage();
}

QImage MPixmapData::toImage() const
{
    return softImage;
}

void MPixmapData::fromImage(const QImage &image,
                            Qt::ImageConversionFlags flags)
{
    void *rawResource = static_cast <void *> (((QImage &) image).data_ptr()->data);

    if (sharedImagesMap.contains(rawResource)) {
        MImageInfo *info = sharedImagesMap.value(rawResource);
        fromEGLSharedImage(info->handle, image);
    } else {
        // This should *never* happen since the graphics system should never
        // create a MPixmapData for an origin that doesn't contain a raster
        // image we know about. But...
        qWarning("MPixmapData::fromImage called on non-know resource. Falling back...");
        QGLPixmapData::fromImage(image, flags);
    }
}

void MPixmapData::fromEGLSharedImage(Qt::HANDLE handle, const QImage &si)
{
    if (si.isNull())
        qFatal("Trying to build pixmap with an empty/null softimage!");
        
    QGLShareContextScope ctx(qt_gl_share_widget()->context());
   
    MExtensions::ensureInitialized();
 
    bool textureIsBound = false;
    GLuint newTextureId;
    GLint newWidth, newHeight;

    glGenTextures(1, &newTextureId);
    glBindTexture(GL_TEXTURE_2D, newTextureId);
    
    glFinish();
    EGLImageKHR image = QEgl::eglCreateImageKHR(QEgl::display(), EGL_NO_CONTEXT, EGL_SHARED_IMAGE_NOK,
                                                (EGLClientBuffer)handle, preserved_image_attribs);

    if (image != EGL_NO_IMAGE_KHR) {
        glFinish();
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image);
        GLint err = glGetError();
        if (err == GL_NO_ERROR)
            textureIsBound = true;
        
        MExtensions::eglQueryImageNOK(QEgl::display(), image, EGL_WIDTH, &newWidth);
        MExtensions::eglQueryImageNOK(QEgl::display(), image, EGL_HEIGHT, &newHeight);
          
        QEgl::eglDestroyImageKHR(QEgl::display(), image);
        glFinish();
    }
        
    if (textureIsBound) {
        // FIXME Remove this ugly hasAlphaChannel check when Qt lands the NoOpaqueCheck flag fix
        // for QGLPixmapData.
        fromTexture(newTextureId, newWidth, newHeight, 
                    (si.hasAlphaChannel() && const_cast<QImage &>(si).data_ptr()->checkForAlphaPixels()));
        softImage = si;
        MPixmapData::registerSharedImage(handle, softImage);
    } else {
        qWarning("Failed to create a texture from a shared image!");
        glDeleteTextures(1, &newTextureId);
    }
}

Qt::HANDLE MPixmapData::imageToEGLSharedImage(const QImage &image)
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());

    MExtensions::ensureInitialized();

    glFinish();
    QGLPixmapData pixmapData(QPixmapData::PixmapType);
    pixmapData.fromImage(image, 0);
    GLuint textureId = pixmapData.bind();

    glFinish();
    EGLImageKHR eglimage = QEgl::eglCreateImageKHR(QEgl::display(), QEglContext::currentContext(QEgl::OpenGL)->context(),
                                                                                                EGL_GL_TEXTURE_2D_KHR,
                                                                                                (EGLClientBuffer) textureId,
                                                                                                preserved_image_attribs);
    glFinish();

    if (eglimage) {
        EGLNativeSharedImageTypeNOK handle = MExtensions::eglCreateSharedImageNOK(QEgl::display(), eglimage, NULL);
        QEgl::eglDestroyImageKHR(QEgl::display(), eglimage);
        glFinish();
        return (Qt::HANDLE) handle;
    } else {
        qWarning("Failed to create shared image from pixmap/texture!");
        return 0;
    }
}

void MPixmapData::updateFromSoftImage()
{
    m_dirty = true;
    m_source = softImage;
    ensureCreated();
    
    if (softImage.width() != w || softImage.height() != h)
        qWarning("Ooops, looks like softImage changed dimensions since last updated! Corruption ahead?!");
}

bool MPixmapData::destroyEGLSharedImage(Qt::HANDLE h)
{
    QGLShareContextScope ctx(qt_gl_share_widget()->context());   
    MExtensions::ensureInitialized();

    QMutableHashIterator <void*, MImageInfo*> i(sharedImagesMap);
    while (i.hasNext()) {
        i.next();
        if (i.value()->handle == h)
            i.remove();
    }

    return MExtensions::eglDestroySharedImageNOK(QEgl::display(), (EGLNativeSharedImageTypeNOK) h);
}

void MPixmapData::registerSharedImage(Qt::HANDLE handle, const QImage &si)
{
    void *raw = static_cast <void *> (((QImage) si).data_ptr()->data);
    MImageInfo *info;
    
    if (! sharedImagesMap.contains(raw)) {
        info = new MImageInfo;
        info->handle = handle;
        info->rawFormat = si.format();
        sharedImagesMap.insert(raw, info);
    } else {
        info = sharedImagesMap.value(raw);
        if (info->handle != handle || info->rawFormat != si.format())
            qWarning("Inconsistency detected: overwriting entry in sharedImagesMap but handle/format different");
    }
}
