/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBINTEGRATION_DEBUG

#include "qbbintegration.h"
#include "qbbinputcontext.h"
#include "qbbeventthread.h"
#include "qbbglcontext.h"
#include "qbbglwindowsurface.h"
#include "qbbnavigatorthread.h"
#include "qbbrasterwindowsurface.h"
#include "qbbscreen.h"
#include "qbbwindow.h"
#include "qbbvirtualkeyboard.h"
#include "qgenericunixfontdatabase.h"
#include "qbbclipboard.h"
#include "qbbglcontext.h"
#include "qbblocalethread.h"

#include "qapplication.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>
#include <QtGui/QWindowSystemInterface>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#include <QDebug>

#include <errno.h>

QT_BEGIN_NAMESPACE

QBBIntegration::QBBIntegration() :
    mFontDb(new QGenericUnixFontDatabase()),
    mPaintUsingOpenGL(getenv("QBB_USE_OPENGL") != NULL)
{
    if (mPaintUsingOpenGL) {
        // Set default window API to OpenGL
        QPlatformWindowFormat format = QPlatformWindowFormat::defaultFormat();
        format.setWindowApi(QPlatformWindowFormat::OpenGL);
        QPlatformWindowFormat::setDefaultFormat(format);
    }

    // initialize global OpenGL resources
    QBBGLContext::initialize();

    // open connection to QNX composition manager
    errno = 0;
    int result = screen_create_context(&mContext, SCREEN_APPLICATION_CONTEXT);
    if (result != 0) {
        qFatal("QBB: failed to connect to composition manager, errno=%d", errno);
    }

    // Create displays for all possible screens (which may not be attached)
    QBBScreen::createDisplays(mContext);

    // create/start event thread
    mEventThread = new QBBEventThread(mContext, *QBBScreen::primaryDisplay());
    mEventThread->start();

    // create/start navigator thread
    mNavigatorThread = new QBBNavigatorThread(*QBBScreen::primaryDisplay());
    mNavigatorThread->start();

#ifdef QBBLOCALETHREAD_ENABLED
    // Start the locale change monitoring thread.
    mLocaleThread = new QBBLocaleThread();
    mLocaleThread->start();
#endif

    // create/start the keyboard class.
    QBBVirtualKeyboard::instance();

    // Set up the input context
    qApp->setInputContext(new QBBInputContext(qApp));
}

QBBIntegration::~QBBIntegration()
{
#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBB: platform plugin shutdown begin";
#endif
    // destroy the keyboard class.
    QBBVirtualKeyboard::destroy();

#ifdef QBBLOCALETHREAD_ENABLED
    // stop/destroy the locale thread.
    delete mLocaleThread;
#endif

    // stop/destroy event thread
    delete mEventThread;

    // stop/destroy navigator thread
    delete mNavigatorThread;

    // destroy all displays
    QBBScreen::destroyDisplays();

    // close connection to QNX composition manager
    screen_destroy_context(mContext);

    // cleanup global OpenGL resources
    QBBGLContext::shutdown();

#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBB: platform plugin shutdown end";
#endif
}

bool QBBIntegration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    case ThreadedPixmaps: return true;
#if defined(QT_OPENGL_ES)
    case OpenGL: return true;
#endif
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPixmapData *QBBIntegration::createPixmapData(QPixmapData::PixelType type) const
{
    if (paintUsingOpenGL())
        return new QGLPixmapData(type);
    else
        return new QRasterPixmapData(type);
}

QPlatformWindow *QBBIntegration::createPlatformWindow(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);

    // New windows are created on the primary display.
    return new QBBWindow(widget, mContext);
}

QWindowSurface *QBBIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    if (paintUsingOpenGL())
        return new QBBGLWindowSurface(widget);
    else
        return new QBBRasterWindowSurface(widget);
}

void QBBIntegration::moveToScreen(QWidget *window, int screen)
{
#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBBIntegration::moveToScreen - w=" << window << ", s=" << screen;
#endif

    // get platform window used by widget
    QBBWindow* platformWindow = static_cast<QBBWindow*>(window->platformWindow());

    // lookup platform screen by index
    QBBScreen* platformScreen = static_cast<QBBScreen*>(QBBScreen::screens().at(screen));

    // move the platform window to the platform screen (this can fail when move to screen
    // is called before the window is actually created).
    if (platformWindow && platformScreen)
        platformWindow->setScreen(platformScreen);
}

QList<QPlatformScreen *> QBBIntegration::screens() const
{
    return QBBScreen::screens();
}

#ifndef QT_NO_CLIPBOARD
QPlatformClipboard *QBBIntegration::clipboard() const
{
    static QPlatformClipboard *clipboard = 0;
    if (!clipboard) {
        clipboard = static_cast<QPlatformClipboard *>(new QBBClipboard);
    }
    return clipboard;
}
#endif

void QBBIntegration::setCursorPos(int x, int y)
{
    mEventThread->injectPointerMoveEvent(x, y);
}

QT_END_NAMESPACE
