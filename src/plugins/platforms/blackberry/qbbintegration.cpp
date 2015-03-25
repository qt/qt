/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBINTEGRATION_DEBUG

#include "qbbintegration.h"
#include "qbbinputcontext.h"
#include "qbbscreeneventthread.h"
#include "qbbglcontext.h"
#include "qbbglwindowsurface.h"
#include "qbbnavigatoreventhandler.h"
#include "qbbnavigatoreventnotifier.h"
#include "qbbrasterwindowsurface.h"
#include "qbbscreen.h"
#include "qbbscreeneventhandler.h"
#include "qbbwindow.h"
#include "qbbvirtualkeyboardpps.h"
#include "qgenericunixfontdatabase.h"
#include "qbbclipboard.h"
#include "qbbglcontext.h"
#include "qbblocalethread.h"
#include "qbbnativeinterface.h"
#include "qbbbuttoneventnotifier.h"
#if defined(Q_OS_BLACKBERRY)
#include "qbbbpseventfilter.h"
#include "qbbvirtualkeyboardbps.h"
#endif

#include <QtCore/QAbstractEventDispatcher>
#include <QtGui/QApplication>
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
    mScreenEventThread(0),
    mNavigatorEventHandler(new QBBNavigatorEventHandler()),
    mButtonsNotifier(new QBBButtonEventNotifier()),
    mFontDb(new QGenericUnixFontDatabase()),
    mScreenEventHandler(new QBBScreenEventHandler(this)),
    mPaintUsingOpenGL(getenv("QBB_USE_OPENGL") != NULL),
    mVirtualKeyboard(0),
    mNativeInterface(new QBBNativeInterface(this)),
    mBpsEventFilter(0)
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
    // Not on BlackBerry, it has specialised event dispatcher which also handles navigator events
#if !defined(Q_OS_BLACKBERRY)
    mNavigatorEventNotifier = new QBBNavigatorEventNotifier(mNavigatorEventHandler);

    // delay invocation of start() to the time the event loop is up and running
    // needed to have the QThread internals of the main thread properly initialized
    QMetaObject::invokeMethod(mNavigatorEventNotifier, "start", Qt::QueuedConnection);
#endif

    // Create displays for all possible screens (which may not be attached)
    createDisplays();

    // create/start event thread
#if defined(QBB_SCREENEVENTTHREAD)
    mScreenEventThread = new QBBScreenEventThread(mContext, mScreenEventHandler);
    mScreenEventThread->start();
#endif

#ifdef QBBLOCALETHREAD_ENABLED
    // Start the locale change monitoring thread.
    mLocaleThread = new QBBLocaleThread();
    mLocaleThread->start();
#endif

#if defined(Q_OS_BLACKBERRY)
    bps_initialize();

    QBBVirtualKeyboardBps *virtualKeyboardBps = new QBBVirtualKeyboardBps;

    mBpsEventFilter = new QBBBpsEventFilter(mNavigatorEventHandler,
            (mScreenEventThread ? 0 : mScreenEventHandler), virtualKeyboardBps);

    if (!mScreenEventThread) {
        Q_FOREACH (QPlatformScreen *platformScreen, mScreens) {
            QBBScreen *screen = static_cast<QBBScreen*>(platformScreen);
            mBpsEventFilter->registerForScreenEvents(screen);
        }
    }

    mBpsEventFilter->installOnEventDispatcher(QAbstractEventDispatcher::instance());

    mVirtualKeyboard = virtualKeyboardBps;
#else
    // create/start the keyboard class.
    mVirtualKeyboard = new QBBVirtualKeyboardPps();

    // delay invocation of start() to the time the event loop is up and running
    // needed to have the QThread internals of the main thread properly initialized
    QMetaObject::invokeMethod(mVirtualKeyboard, "start", Qt::QueuedConnection);
#endif

    // TODO check if we need to do this for all screens or only the primary one
    QObject::connect(mVirtualKeyboard, SIGNAL(heightChanged(int)),
                     primaryDisplay(), SLOT(keyboardHeightChanged(int)));

    // Set up the input context
    qApp->setInputContext(new QBBInputContext(*mVirtualKeyboard, qApp));

    // delay invocation of start() to the time the event loop is up and running
    // needed to have the QThread internals of the main thread properly initialized
    QMetaObject::invokeMethod(mButtonsNotifier, "start", Qt::QueuedConnection);
}

QBBIntegration::~QBBIntegration()
{
#if defined(QBBINTEGRATION_DEBUG)
    qDebug() << "QBB: platform plugin shutdown begin";
#endif
    delete mNativeInterface;

    // Destroy the hardware button notifier
    delete mButtonsNotifier;

#ifdef QBBLOCALETHREAD_ENABLED
    // stop/destroy the locale thread.
    delete mLocaleThread;
#endif

#if defined(QBB_SCREENEVENTTHREAD)
    // stop/destroy event thread
    delete mScreenEventThread;
#elif defined(Q_OS_BLACKBERRY)
    Q_FOREACH (QPlatformScreen *platformScreen, mScreens) {
        QBBScreen *screen = static_cast<QBBScreen*>(platformScreen);
        mBpsEventFilter->unregisterForScreenEvents(screen);
    }
#endif

#if defined(Q_OS_BLACKBERRY)
    delete mBpsEventFilter;
#else
    // stop/destroy navigator event handling classes
    delete mNavigatorEventNotifier;
#endif

    // destroy the keyboard class.
    delete mVirtualKeyboard;

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
    mScreenEventThread->injectPointerMoveEvent(x, y);
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
        int isAttached = 0;
        result = screen_get_display_property_iv(displays[i], SCREEN_PROPERTY_ATTACHED, &isAttached);
        if (result != 0) {
            qWarning("QBBIntegration: failed to query display attachment, errno=%d", errno);
            isAttached = 1; // assume attached
        }

        if (!isAttached)
            continue;

        createDisplay(displays[i], i);
    } // of displays iteration
}

void QBBIntegration::createDisplay(screen_display_t display, int screenIndex)
{
    QBBScreen *screen = new QBBScreen(mContext, display, screenIndex);
    mScreens.append(screen);
    screen->adjustOrientation();

    QObject::connect(mScreenEventHandler, SIGNAL(newWindowCreated(screen_window_t)),
                  screen, SLOT(newWindowCreated(screen_window_t)));
    QObject::connect(mScreenEventHandler, SIGNAL(windowClosed(screen_window_t)),
                  screen, SLOT(windowClosed(screen_window_t)));

    QObject::connect(mNavigatorEventHandler, SIGNAL(rotationChanged(int)), screen, SLOT(setRotation(int)));
    QObject::connect(mNavigatorEventHandler, SIGNAL(windowGroupActivated(QByteArray)), screen, SLOT(activateWindowGroup(QByteArray)));
    QObject::connect(mNavigatorEventHandler, SIGNAL(windowGroupDeactivated(QByteArray)), screen, SLOT(deactivateWindowGroup(QByteArray)));
    QObject::connect(mNavigatorEventHandler, SIGNAL(windowGroupStateChanged(QByteArray,Qt::WindowState)),
            screen, SLOT(windowGroupStateChanged(QByteArray,Qt::WindowState)));
}

void QBBIntegration::removeDisplay(QBBScreen *screen)
{
    Q_CHECK_PTR(screen);
    Q_ASSERT(mScreens.contains(screen));
    mScreens.removeAll(screen);
    screen->deleteLater();
}

void QBBIntegration::destroyDisplays()
{
    qDeleteAll(mScreens);
    mScreens.clear();
}

QBBScreen *QBBIntegration::screenForNative(screen_display_t nativeScreen) const
{
    Q_FOREACH (QPlatformScreen *screen, mScreens) {
        QBBScreen *bbScreen = static_cast<QBBScreen*>(screen);
        if (bbScreen->nativeDisplay() == nativeScreen)
            return bbScreen;
    }

    return 0;
}

QT_END_NAMESPACE
