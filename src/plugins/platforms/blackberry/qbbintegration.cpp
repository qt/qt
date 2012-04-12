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
#include "qbbnavigatoreventhandler.h"
#include "qbbnavigatoreventnotifier.h"
#include "qbbrasterwindowsurface.h"
#include "qbbscreen.h"
#include "qbbscreeneventhandler.h"
#include "qbbwindow.h"
#include "qbbvirtualkeyboard.h"
#include "qgenericunixfontdatabase.h"
#include "qbbclipboard.h"
#include "qbbglcontext.h"
#include "qbblocalethread.h"
#include "qbbnativeinterface.h"

#include "qapplication.h"
#include <QtGui/private/qpixmap_raster_p.h>
#include <QtGui/QPlatformWindow>
#include <QtGui/QWindowSystemInterface>
#include <QtOpenGL/private/qpixmapdata_gl_p.h>
#include <QDebug>

#include <errno.h>

#if defined(Q_OS_BLACKBERRY)
#include <bps/navigator.h>
#endif

QT_BEGIN_NAMESPACE

Q_DECLARE_METATYPE(screen_window_t);

QBBIntegration::QBBIntegration() :
    mNavigatorEventHandler(new QBBNavigatorEventHandler()),
    mFontDb(new QGenericUnixFontDatabase()),
    mScreenEventHandler(new QBBScreenEventHandler()),
    mPaintUsingOpenGL(getenv("QBB_USE_OPENGL") != NULL),
    mVirtualKeyboard(0),
    mNativeInterface(new QBBNativeInterface(this))
{
    qRegisterMetaType<screen_window_t>();

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

    // Create/start navigator event notifier
    mNavigatorEventNotifier = new QBBNavigatorEventNotifier(mNavigatorEventHandler);

    // delay invocation of start() to the time the event loop is up and running
    // needed to have the QThread internals of the main thread properly initialized
    QMetaObject::invokeMethod(mNavigatorEventNotifier, "start", Qt::QueuedConnection);

    // Create displays for all possible screens (which may not be attached)
    createDisplays();

    // create/start event thread
    mEventThread = new QBBEventThread(mContext, mScreenEventHandler);
    mEventThread->start();

#ifdef QBBLOCALETHREAD_ENABLED
    // Start the locale change monitoring thread.
    mLocaleThread = new QBBLocaleThread();
    mLocaleThread->start();
#endif

#if defined(Q_OS_BLACKBERRY)
    bps_initialize();
#endif

    // create/start the keyboard class.
    mVirtualKeyboard = new QBBVirtualKeyboard();

    // delay invocation of start() to the time the event loop is up and running
    // needed to have the QThread internals of the main thread properly initialized
    QMetaObject::invokeMethod(mVirtualKeyboard, "start", Qt::QueuedConnection);

    // TODO check if we need to do this for all screens or only the primary one
    QObject::connect(mVirtualKeyboard, SIGNAL(heightChanged(int)),
                     primaryDisplay(), SLOT(keyboardHeightChanged(int)));

    // Set up the input context
    qApp->setInputContext(new QBBInputContext(*mVirtualKeyboard, qApp));
}

QBBIntegration::~QBBIntegration()
{
#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBB: platform plugin shutdown begin";
#endif


    delete mNativeInterface;

    // destroy the keyboard class.
    delete mVirtualKeyboard;

#ifdef QBBLOCALETHREAD_ENABLED
    // stop/destroy the locale thread.
    delete mLocaleThread;
#endif

    // stop/destroy event thread
    delete mEventThread;

    // stop/destroy navigator event handling classes
    delete mNavigatorEventNotifier;
    delete mNavigatorEventHandler;

    delete mScreenEventHandler;

    // destroy all displays
    destroyDisplays();

    // close connection to QNX composition manager
    screen_destroy_context(mContext);

    // cleanup global OpenGL resources
    QBBGLContext::shutdown();

#if defined(Q_OS_BLACKBERRY)
    bps_shutdown();
#endif

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

    return new QBBWindow(widget, mContext, primaryDisplay());
}

QWindowSurface *QBBIntegration::createWindowSurface(QWidget *widget, WId winId) const
{
    Q_UNUSED(winId);
    if (paintUsingOpenGL())
        return new QBBGLWindowSurface(widget);
    else
        return new QBBRasterWindowSurface(widget);
}

QPlatformNativeInterface *QBBIntegration::nativeInterface() const
{
    return mNativeInterface;
}

void QBBIntegration::moveToScreen(QWidget *window, int screen)
{
#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBBIntegration::moveToScreen - w=" << window << ", s=" << screen;
#endif

    // get platform window used by widget
    QBBWindow* platformWindow = static_cast<QBBWindow*>(window->platformWindow());

    // lookup platform screen by index
    QBBScreen* platformScreen = static_cast<QBBScreen*>(mScreens.at(screen));

    // move the platform window to the platform screen (this can fail when move to screen
    // is called before the window is actually created).
    if (platformWindow && platformScreen)
        platformWindow->setScreen(platformScreen);
}

QList<QPlatformScreen *> QBBIntegration::screens() const
{
    return mScreens;
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


QBBScreen *QBBIntegration::screenForWindow(screen_window_t window) const
{
    screen_display_t display = 0;
    if (screen_get_window_property_pv(window, SCREEN_PROPERTY_DISPLAY, (void**)&display) != 0) {
        qWarning("QBBIntegration: Failed to get screen for window, errno=%d", errno);
        return 0;
    }

    Q_FOREACH (QPlatformScreen *screen, mScreens) {
        QBBScreen * const bbScreen = static_cast<QBBScreen*>(screen);
        if (bbScreen->nativeDisplay() == display)
            return bbScreen;
    }

    return 0;
}

QBBScreen *QBBIntegration::primaryDisplay() const
{
    return static_cast<QBBScreen*>(mScreens.first());
}

void QBBIntegration::setCursorPos(int x, int y)
{
    mEventThread->injectPointerMoveEvent(x, y);
}

void QBBIntegration::createDisplays()
{
    // get number of displays
    errno = 0;
    int displayCount;
    int result = screen_get_context_property_iv(mContext, SCREEN_PROPERTY_DISPLAY_COUNT, &displayCount);
    if (result != 0)
        qFatal("QBBIntegration: failed to query display count, errno=%d", errno);

    // get all displays
    errno = 0;
    screen_display_t *displays = (screen_display_t *)alloca(sizeof(screen_display_t) * displayCount);
    result = screen_get_context_property_pv(mContext, SCREEN_PROPERTY_DISPLAYS, (void **)displays);
    if (result != 0)
        qFatal("QBBIntegration: failed to query displays, errno=%d", errno);

    for (int i=0; i<displayCount; i++) {
#if defined(QBBINTEGRATION_DEBUG)
        qDebug() << "QBBIntegration: Creating screen for display " << i;
#endif
        QBBScreen *screen = new QBBScreen(mContext, displays[i], i);
        mScreens.push_back(screen);

        QObject::connect(mScreenEventHandler, SIGNAL(newWindowCreated(screen_window_t)),
                         screen, SLOT(newWindowCreated(screen_window_t)));
        QObject::connect(mScreenEventHandler, SIGNAL(windowClosed(screen_window_t)),
                         screen, SLOT(windowClosed(screen_window_t)));

        QObject::connect(mNavigatorEventHandler, SIGNAL(rotationChanged(int)), screen, SLOT(setRotation(int)));
    }
}

void QBBIntegration::destroyDisplays()
{
    qDeleteAll(mScreens);
    mScreens.clear();
}

QT_END_NAMESPACE
