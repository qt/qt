/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QEGL_P_H
#define QEGL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience of
// the QtOpenGL and QtOpenVG modules.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_INCLUDE_NAMESPACE

#if defined(QT_GLES_EGL)
#include <GLES/egl.h>
#else
#include <EGL/egl.h>
#endif

#if defined(Q_WS_X11)
// If <EGL/egl.h> included <X11/Xlib.h>, then the global namespace
// may have been polluted with X #define's.  The following makes sure
// the X11 headers were included properly and then cleans things up.
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Bool
#undef Status
#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef Type
#undef FontChange
#undef CursorShape
#endif

// Internally we use the EGL-prefixed native types which are used in EGL >= 1.3.
// For older versions of EGL, we have to define these types ourselves here:
#if !defined(EGL_VERSION_1_3) && !defined(QEGL_NATIVE_TYPES_DEFINED)
#undef EGLNativeWindowType
#undef EGLNativePixmapType
#undef EGLNativeDisplayType
typedef NativeWindowType EGLNativeWindowType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeDisplayType EGLNativeDisplayType;
#define QEGL_NATIVE_TYPES_DEFINED 1
#endif

QT_END_INCLUDE_NAMESPACE

#include <QtGui/qpaintdevice.h>

QT_BEGIN_NAMESPACE

#define QEGL_NO_CONFIG ((EGLConfig)-1)

class QEglProperties;

namespace QEgl {
    enum API
    {
        OpenGL,
        OpenVG
    };

    enum PixelFormatMatch
    {
        ExactPixelFormat,
        BestPixelFormat
    };

    enum ConfigOptions
    {
        NoOptions   = 0,
        Translucent = 0x01,
        Renderable  = 0x02  // Config will be compatable with the paint engines (VG or GL)
    };

    // Most of the time we use the same config for things like widgets & pixmaps, so rather than
    // go through the eglChooseConfig loop every time, we use defaultConfig, which will return
    // the config for a particular device/api/option combo. This function assumes that once a
    // config is chosen for a particular combo, it's safe to always use that combo.
    Q_GUI_EXPORT EGLConfig  defaultConfig(int devType, API api, ConfigOptions options);

    Q_GUI_EXPORT EGLConfig  chooseConfig(const QEglProperties* configAttribs, QEgl::PixelFormatMatch match = QEgl::ExactPixelFormat);
    Q_GUI_EXPORT EGLSurface createSurface(QPaintDevice *device, EGLConfig cfg, const QEglProperties *surfaceAttribs = 0);

    Q_GUI_EXPORT void dumpAllConfigs();

    Q_GUI_EXPORT void clearError();
    Q_GUI_EXPORT EGLint error();
    Q_GUI_EXPORT QString errorString(EGLint code);
    Q_GUI_EXPORT QString errorString();

    Q_GUI_EXPORT QString extensions();
    Q_GUI_EXPORT bool hasExtension(const char* extensionName);

    Q_GUI_EXPORT EGLDisplay display();

    Q_GUI_EXPORT EGLNativeDisplayType nativeDisplay();
    Q_GUI_EXPORT EGLNativeWindowType  nativeWindow(QWidget*);
    Q_GUI_EXPORT EGLNativePixmapType  nativePixmap(QPixmap*);

#ifdef Q_WS_X11
    Q_GUI_EXPORT VisualID getCompatibleVisualId(EGLConfig config);
#endif
};


QT_END_NAMESPACE

#endif //QEGL_P_H
