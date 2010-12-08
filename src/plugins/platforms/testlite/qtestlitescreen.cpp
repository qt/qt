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

#include "qtestlitescreen.h"

#include "qtestlitecursor.h"
#include "qtestlitewindow.h"

#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>

QT_BEGIN_NAMESPACE

static int (*original_x_errhandler)(Display *dpy, XErrorEvent *);
static bool seen_badwindow;

static int qt_x_errhandler(Display *dpy, XErrorEvent *err)
{

qDebug() << "qt_x_errhandler" << err->error_code;

    switch (err->error_code) {
    case BadAtom:
#if 0
        if (err->request_code == 20 /* X_GetProperty */
            && (err->resourceid == XA_RESOURCE_MANAGER
                || err->resourceid == XA_RGB_DEFAULT_MAP
                || err->resourceid == ATOM(_NET_SUPPORTED)
                || err->resourceid == ATOM(_NET_SUPPORTING_WM_CHECK)
                || err->resourceid == ATOM(KDE_FULL_SESSION)
                || err->resourceid == ATOM(KWIN_RUNNING)
                || err->resourceid == ATOM(XdndProxy)
                || err->resourceid == ATOM(XdndAware))


            ) {
            // Perhaps we're running under SECURITY reduction? :/
            return 0;
        }
#endif
        qDebug() << "BadAtom";
        break;

    case BadWindow:
        if (err->request_code == 2 /* X_ChangeWindowAttributes */
            || err->request_code == 38 /* X_QueryPointer */) {
            for (int i = 0; i < ScreenCount(dpy); ++i) {
                if (err->resourceid == RootWindow(dpy, i)) {
                    // Perhaps we're running under SECURITY reduction? :/
                    return 0;
                }
            }
        }
        seen_badwindow = true;
        if (err->request_code == 25 /* X_SendEvent */) {
            for (int i = 0; i < ScreenCount(dpy); ++i) {
                if (err->resourceid == RootWindow(dpy, i)) {
                    // Perhaps we're running under SECURITY reduction? :/
                    return 0;
                }
            }
#if 0
            if (X11->xdndHandleBadwindow()) {
                qDebug("xdndHandleBadwindow returned true");
                return 0;
            }
#endif
        }
#if 0
        if (X11->ignore_badwindow)
            return 0;
#endif
        break;

    case BadMatch:
        if (err->request_code == 42 /* X_SetInputFocus */)
            return 0;
        break;

    default:
#if 0 //!defined(QT_NO_XINPUT)
        if (err->request_code == X11->xinput_major
            && err->error_code == (X11->xinput_errorbase + XI_BadDevice)
            && err->minor_code == 3 /* X_OpenDevice */) {
            return 0;
        }
#endif
        break;
    }

    char errstr[256];
    XGetErrorText( dpy, err->error_code, errstr, 256 );
    char buffer[256];
    char request_str[256];
    qsnprintf(buffer, 256, "%d", err->request_code);
    XGetErrorDatabaseText(dpy, "XRequest", buffer, "", request_str, 256);
    if (err->request_code < 128) {
        // X error for a normal protocol request
        qWarning( "X Error: %s %d\n"
                  "  Major opcode: %d (%s)\n"
                  "  Resource id:  0x%lx",
                  errstr, err->error_code,
                  err->request_code,
                  request_str,
                  err->resourceid );
    } else {
        // X error for an extension request
        const char *extensionName = 0;
#if 0
        if (err->request_code == X11->xrender_major)
            extensionName = "RENDER";
        else if (err->request_code == X11->xrandr_major)
            extensionName = "RANDR";
        else if (err->request_code == X11->xinput_major)
            extensionName = "XInputExtension";
        else if (err->request_code == X11->mitshm_major)
            extensionName = "MIT-SHM";
#endif
        char minor_str[256];
        if (extensionName) {
            qsnprintf(buffer, 256, "%s.%d", extensionName, err->minor_code);
            XGetErrorDatabaseText(dpy, "XRequest", buffer, "", minor_str, 256);
        } else {
            extensionName = "Uknown extension";
            qsnprintf(minor_str, 256, "Unknown request");
        }
        qWarning( "X Error: %s %d\n"
                  "  Extension:    %d (%s)\n"
                  "  Minor opcode: %d (%s)\n"
                  "  Resource id:  0x%lx",
                  errstr, err->error_code,
                  err->request_code,
                  extensionName,
                  err->minor_code,
                  minor_str,
                  err->resourceid );
    }

    // ### we really should distinguish between severe, non-severe and
    // ### application specific errors

    return 0;
}

QTestLiteScreen::QTestLiteScreen()
        : mFormat(QImage::Format_RGB32)
        , mWmProtocolsAtom(0)
        , mWmDeleteWindowAtom(0)
{
    char *display_name = getenv("DISPLAY");
    mDisplay = XOpenDisplay(display_name);
    if (!mDisplay) {
        fprintf(stderr, "Cannot connect to X server: %s\n",
                display_name);
        exit(1);
    }

#ifndef DONT_USE_MIT_SHM
    Status MIT_SHM_extension_supported = XShmQueryExtension (mDisplay);
    Q_ASSERT(MIT_SHM_extension_supported == True);
#endif
    original_x_errhandler = XSetErrorHandler(qt_x_errhandler);

    if (qgetenv("DO_X_SYNCHRONIZE").toInt())
        XSynchronize(mDisplay, true);

    mScreen = DefaultScreen(mDisplay);
    int width = DisplayWidth(mDisplay, mScreen);
    int height = DisplayHeight(mDisplay, mScreen);
    mGeometry = QRect(0,0,width,height);

    int physicalWidth = DisplayWidthMM(mDisplay, mScreen);
    int physicalHeight = DisplayHeightMM(mDisplay, mScreen);
    mPhysicalSize = QSize(physicalWidth,physicalHeight);

    int xSocketNumber = XConnectionNumber(mDisplay);

    mDepth = DefaultDepth(mDisplay,mScreen);
#ifdef MYX11_DEBUG
    qDebug() << "X socket:"<< xSocketNumber;
#endif
    QSocketNotifier *sock = new QSocketNotifier(xSocketNumber, QSocketNotifier::Read, this);
    connect(sock, SIGNAL(activated(int)), this, SLOT(eventDispatcher()));

    mWmProtocolsAtom = XInternAtom (mDisplay, "WM_PROTOCOLS", False);
    mWmDeleteWindowAtom = XInternAtom (mDisplay, "WM_DELETE_WINDOW", False);

    mWmMotifHintAtom = XInternAtom(mDisplay, "_MOTIF_WM_HINTS\0", False);

    mCursor = new QTestLiteCursor(this);
}

QTestLiteScreen::~QTestLiteScreen()
{
    delete mCursor;
    XCloseDisplay(mDisplay);
}

#ifdef KeyPress
#undef KeyPress
#endif
#ifdef KeyRelease
#undef KeyRelease
#endif

//Q_GUI_EXPORT extern Atom wmProtocolsAtom;
//Q_GUI_EXPORT extern Atom wmDeleteWindowAtom;

bool QTestLiteScreen::handleEvent(XEvent *xe)
{
    int quit = false;
    QTestLiteWindow *xw = 0;
    QWidget *widget = QWidget::find(xe->xany.window);
    if (widget) {
        xw = static_cast<QTestLiteWindow *>(widget->platformWindow());
    }
    if (!xw) {
#ifdef MYX11_DEBUG
        qWarning() << "Unknown window" << hex << xe->xany.window << "received event" <<  xe->type;
#endif
        return quit;
    }

    switch (xe->type) {

    case ClientMessage:
        if (xe->xclient.format == 32 && xe->xclient.message_type == wmProtocolsAtom()) {
            Atom a = xe->xclient.data.l[0];
            if (a == wmDeleteWindowAtom())
                xw->handleCloseEvent();
#ifdef MYX11_DEBUG
            qDebug() << "ClientMessage WM_PROTOCOLS" << a;
#endif
        }
#ifdef MYX11_DEBUG
        else
            qDebug() << "ClientMessage" << xe->xclient.format << xe->xclient.message_type;
#endif
        break;

    case Expose:
        if (xw)
            if (xe->xexpose.count == 0)
                xw->paintEvent();
        break;
    case ConfigureNotify:
        if (xw)
            xw->resizeEvent(&xe->xconfigure);
        break;

    case ButtonPress:
        xw->mousePressEvent(&xe->xbutton);
        break;

    case ButtonRelease:
        xw->handleMouseEvent(QEvent::MouseButtonRelease, &xe->xbutton);
        break;

    case MotionNotify:
        xw->handleMouseEvent(QEvent::MouseMove, &xe->xbutton);
        break;

        case XKeyPress:
        xw->handleKeyEvent(QEvent::KeyPress, &xe->xkey);
        break;

    case XKeyRelease:
        xw->handleKeyEvent(QEvent::KeyRelease, &xe->xkey);
        break;

    case EnterNotify:
        xw->handleEnterEvent();
        break;

    case LeaveNotify:
        xw->handleLeaveEvent();
        break;

    case XFocusIn:
        xw->handleFocusInEvent();
        break;

    case XFocusOut:
        xw->handleFocusOutEvent();
        break;

    default:
#ifdef MYX11_DEBUG
        qDebug() << hex << xe->xany.window << "Other X event" << xe->type;
#endif
        break;
    }
    return quit;
}

void QTestLiteScreen::eventDispatcher()
{
        ulong marker = XNextRequest(mDisplay);
    //    int i = 0;
        while (XPending(mDisplay)) {
            XEvent event;
            XNextEvent(mDisplay, &event);
            /* done = */
            handleEvent(&event);

            if (event.xany.serial >= marker) {
    #ifdef MYX11_DEBUG
                qDebug() << "potential livelock averted";
    #endif
    #if 0
                if (XEventsQueued(mDisplay, QueuedAfterFlush)) {
                    qDebug() << "	with events queued";
                    QTimer::singleShot(0, this, SLOT(eventDispatcher()));
                }
    #endif
                break;
            }
        }
}

QImage QTestLiteScreen::grabWindow(Window window, int x, int y, int w, int h)
{
    if (w == 0 || h ==0)
        return QImage();

    //WinId 0 means the desktop widget
    if (!window)
        window = rootWindow();

    XWindowAttributes window_attr;
    if (!XGetWindowAttributes(mDisplay, window, &window_attr))
        return QImage();

    if (w < 0)
        w = window_attr.width - x;
    if (h < 0)
        h = window_attr.height - y;

    // Ideally, we should also limit ourselves to the screen area, but the Qt docs say
    // that it's "unsafe" to go outside the screen, so we can ignore that problem.

    //We're definitely not optimizing for speed...
    XImage *xi = XGetImage(mDisplay, window, x, y, w, h, AllPlanes, ZPixmap);

    if (!xi)
        return QImage();

    //taking a copy to make sure we have ownership -- not fast
    QImage result = QImage( (uchar*) xi->data, xi->width, xi->height, xi->bytes_per_line, QImage::Format_RGB32 ).copy();

    XDestroyImage(xi);

    return result;
}

QTestLiteScreen * QTestLiteScreen::testLiteScreenForWidget(QWidget *widget)
{
    QPlatformScreen *platformScreen = platformScreenForWidget(widget);
    return static_cast<QTestLiteScreen *>(platformScreen);
}

Display * QTestLiteScreen::display() const
{
    return mDisplay;
}

int QTestLiteScreen::xScreenNumber() const
{
    return mScreen;
}

Atom QTestLiteScreen::wmProtocolsAtom() const
{
    return mWmProtocolsAtom;
}

Atom QTestLiteScreen::wmDeleteWindowAtom() const
{
    return mWmDeleteWindowAtom;
}

void QTestLiteScreen::setWmDeleteWindowAtom(Atom newDeleteWindowAtom)
{
    mWmDeleteWindowAtom = newDeleteWindowAtom;
}

Atom QTestLiteScreen::atomForMotifWmHints() const
{
    return mWmMotifHintAtom;
}

QT_END_NAMESPACE
