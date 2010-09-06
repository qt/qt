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

#ifndef MEXTENSIONS_H
#define MEXTENSIONS_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "../private/qgl_p.h"
#include "../private/qeglcontext_p.h"
#include "../private/qpixmapdata_gl_p.h"

/* Extensions decls */

#ifndef EGL_SHARED_IMAGE_NOK
#define EGL_SHARED_IMAGE_NOK 0x30DA
typedef void* EGLNativeSharedImageTypeNOK;
#endif

#ifndef EGL_GL_TEXTURE_2D_KHR
#define EGL_GL_TEXTURE_2D_KHR 0x30B1
#endif

#ifndef EGL_FIXED_WIDTH_NOK
#define EGL_FIXED_WIDTH_NOK 0x30DB
#define EGL_FIXED_HEIGHT_NOK 0x30DC
#endif

/* Class */

class MExtensions 
{
public:
    static void ensureInitialized();

    static EGLNativeSharedImageTypeNOK eglCreateSharedImageNOK(EGLDisplay dpy, EGLImageKHR image, EGLint *props);
    static bool eglQueryImageNOK(EGLDisplay dpy, EGLImageKHR image, EGLint prop, EGLint *v);
    static bool eglDestroySharedImageNOK(EGLDisplay dpy, EGLNativeSharedImageTypeNOK img);
    static bool eglSetSurfaceScalingNOK(EGLDisplay dpy, EGLSurface surface, int x, int y, int width, int height);

private:
    static void initialize();

    static bool initialized;
    static bool hasImageShared;
    static bool hasSurfaceScaling;
};

#endif
