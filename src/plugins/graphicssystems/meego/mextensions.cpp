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

#include "mextensions.h"
#include "../private/qeglcontext_p.h"
#include "../private/qpixmapdata_gl_p.h"

bool MExtensions::initialized = false;
bool MExtensions::hasImageShared = false;
bool MExtensions::hasSurfaceScaling = false;

/* Extension funcs */

typedef EGLBoolean (EGLAPIENTRY *eglQueryImageNOKFunc)(EGLDisplay, EGLImageKHR, EGLint, EGLint*);
typedef EGLNativeSharedImageTypeNOK (EGLAPIENTRY *eglCreateSharedImageNOKFunc)(EGLDisplay, EGLImageKHR, EGLint*);
typedef EGLBoolean (EGLAPIENTRY *eglDestroySharedImageNOKFunc)(EGLDisplay, EGLNativeSharedImageTypeNOK);
typedef EGLBoolean (EGLAPIENTRY *eglSetSurfaceScalingNOKFunc)(EGLDisplay, EGLSurface, EGLint, EGLint, EGLint, EGLint);

static eglQueryImageNOKFunc _eglQueryImageNOK = 0;
static eglCreateSharedImageNOKFunc _eglCreateSharedImageNOK = 0;
static eglDestroySharedImageNOKFunc _eglDestroySharedImageNOK = 0;
static eglSetSurfaceScalingNOKFunc _eglSetSurfaceScalingNOK = 0;

/* Public */

void MExtensions::ensureInitialized()
{
    if (!initialized)
        initialize();

    initialized = true;
}

EGLNativeSharedImageTypeNOK MExtensions::eglCreateSharedImageNOK(EGLDisplay dpy, EGLImageKHR image, EGLint *props)
{
    if (! hasImageShared)
        qFatal("EGL_NOK_image_shared not found but trying to use capability!");
        
    return _eglCreateSharedImageNOK(dpy, image, props);
}

bool MExtensions::eglQueryImageNOK(EGLDisplay dpy, EGLImageKHR image, EGLint prop, EGLint *v)
{
    if (! hasImageShared)
        qFatal("EGL_NOK_image_shared not found but trying to use capability!");

    return _eglQueryImageNOK(dpy, image, prop, v);
}

bool MExtensions::eglDestroySharedImageNOK(EGLDisplay dpy, EGLNativeSharedImageTypeNOK img)
{
    if (! hasImageShared)
        qFatal("EGL_NOK_image_shared not found but trying to use capability!");

    return _eglDestroySharedImageNOK(dpy, img);
}

bool MExtensions::eglSetSurfaceScalingNOK(EGLDisplay dpy, EGLSurface surface, int x, int y, int width, int height)
{
    if (! hasSurfaceScaling)
        qFatal("EGL_NOK_surface_scaling not found but trying to use capability!");

   return _eglSetSurfaceScalingNOK(dpy, surface, x, y, width, height);
}

/* Private */

void MExtensions::initialize()
{
    QGLContext *ctx = (QGLContext *) QGLContext::currentContext();
    qt_resolve_eglimage_gl_extensions(ctx);
    
    if (QEgl::hasExtension("EGL_NOK_image_shared")) {
        qDebug("MeegoGraphics: found EGL_NOK_image_shared");
        _eglQueryImageNOK = (eglQueryImageNOKFunc) eglGetProcAddress("eglQueryImageNOK");
        _eglCreateSharedImageNOK = (eglCreateSharedImageNOKFunc) eglGetProcAddress("eglCreateSharedImageNOK");
        _eglDestroySharedImageNOK = (eglDestroySharedImageNOKFunc) eglGetProcAddress("eglDestroySharedImageNOK");
        
        Q_ASSERT(_eglQueryImageNOK && _eglCreateSharedImageNOK && _eglDestroySharedImageNOK);
        hasImageShared = true;
    }
    
    if (QEgl::hasExtension("EGL_NOK_surface_scaling")) {
        qDebug("MeegoGraphics: found EGL_NOK_surface_scaling");
        _eglSetSurfaceScalingNOK = (eglSetSurfaceScalingNOKFunc) eglGetProcAddress("eglSetSurfaceScalingNOK");
        
        Q_ASSERT(_eglSetSurfaceScalingNOK);
        hasSurfaceScaling = true;
    }
}

