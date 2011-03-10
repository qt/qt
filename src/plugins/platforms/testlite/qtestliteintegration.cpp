/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qtestliteintegration.h"
#include "qtestlitewindowsurface.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtCore/qdebug.h>

#include "qtestlitewindow.h"
#include "qgenericunixfontdatabase.h"
#include "qtestlitescreen.h"
#include "qtestliteclipboard.h"

#if !defined(QT_NO_OPENGL)
#if !defined(QT_OPENGL_ES_2)
#include <GL/glx.h>
#else
#include <EGL/egl.h>
#endif //!defined(QT_OPENGL_ES_2)
#include <private/qwindowsurface_gl_p.h>
#include <private/qpixmapdata_gl_p.h>
#endif //QT_NO_OPENGL

QT_BEGIN_NAMESPACE

QTestLiteIntegration::QTestLiteIntegration(bool useOpenGL)
    : mUseOpenGL(useOpenGL)
    , mFontDb(new QGenericUnixFontDatabase())
    , mClipboard(0)
{
    mPrimaryScreen = new QTestLiteScreen();
    mScreens.append(mPrimaryScreen);
}

QPixmapData *QTestLiteIntegration::createPixmapData(QPixmapData::PixelType type) const
{
#ifndef QT_NO_OPENGL
    if (mUseOpenGL)
        return new QGLPixmapData(type);
#endif
    return new QRasterPixmapData(type);
}

QWindowSurface *QTestLiteIntegration::createWindowSurface(QWidget *widget, WId) const
{
#ifndef QT_NO_OPENGL
    if (mUseOpenGL)
        return new QGLWindowSurface(widget);
#endif
    return new QTestLiteWindowSurface(widget);
}


QPlatformWindow *QTestLiteIntegration::createPlatformWindow(QWidget *widget, WId /*winId*/) const
{
    return new QTestLiteWindow(widget);
}



QPixmap QTestLiteIntegration::grabWindow(WId window, int x, int y, int width, int height) const
{
    QImage image;
    QWidget *widget = QWidget::find(window);
    if (widget) {
        QTestLiteScreen *screen = QTestLiteScreen::testLiteScreenForWidget(widget);
        image = screen->grabWindow(window,x,y,width,height);
    } else {
        for (int i = 0; i < mScreens.size(); i++) {
            QTestLiteScreen *screen = static_cast<QTestLiteScreen *>(mScreens[i]);
            if (screen->rootWindow() == window) {
                image = screen->grabWindow(window,x,y,width,height);
            }
        }
    }
    return QPixmap::fromImage(image);
}

QPlatformFontDatabase *QTestLiteIntegration::fontDatabase() const
{
    return mFontDb;
}

QPlatformClipboard * QTestLiteIntegration::clipboard() const
{
    //Use lazy init since clipboard needs QTestliteScreen
    if (!mClipboard) {
        QTestLiteIntegration *that = const_cast<QTestLiteIntegration *>(this);
        that->mClipboard = new QTestLiteClipboard(mPrimaryScreen);
    }
    return mClipboard;
}

bool QTestLiteIntegration::hasOpenGL() const
{
#if !defined(QT_NO_OPENGL)
#if !defined(QT_OPENGL_ES_2)
    QTestLiteScreen *screen = static_cast<const QTestLiteScreen *>(mScreens.at(0));
    return glXQueryExtension(screen->display(), 0, 0) != 0;
#else
    static bool eglHasbeenInitialized = false;
    static bool wasEglInitialized = false;
    if (!eglHasbeenInitialized) {
        eglHasbeenInitialized = true;
        QTestLiteScreen *screen = static_cast<const QTestLiteScreen *>(mScreens.at(0));
        EGLint major, minor;
        eglBindAPI(EGL_OPENGL_ES_API);
        EGLDisplay disp = eglGetDisplay(screen->display());
        wasEglInitialized = eglInitialize(disp,&major,&minor);
    }
    return wasEglInitialized;
#endif
#endif
    return false;
}


QT_END_NAMESPACE
