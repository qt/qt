/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenVG module of the Qt Toolkit.
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

#include <QImage>
#include <QSocketNotifier>

#include <qdebug.h>
#include <QTimer>
#include <QApplication>

#include "x11util.h"
#include "qwindowsurface_testlite.h"

#include <stdio.h>
#include <stdlib.h>


#include <X11/Xatom.h>



//### remove stuff we don't want from qt_x11_p.h
#undef ATOM
#undef X11

//#define MYX11_DEBUG

static int (*original_x_errhandler)(Display *dpy, XErrorEvent *);
static bool seen_badwindow;


static Atom wmProtocolsAtom;
static Atom wmDeleteWindowAtom;

//### copied from qapplication_x11.cpp

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









bool MyDisplay::handleEvent(XEvent *xe)
{
    //qDebug() << "handleEvent" << xe->xany.type << xe->xany.window;
    int quit = false;
    MyWindow *xw = 0;
    foreach (MyWindow *w, windowList) {
        if (w->window == xe->xany.window) {
            xw = w;
            break;
        }
    }
    if (!xw) {
#ifdef MYX11_DEBUG
        qWarning() << "Unknown window" << hex << xe->xany.window << "received event" <<  xe->type;
#endif
        return quit;
    }

    switch (xe->type) {

    case ClientMessage:
        if (xe->xclient.format == 32 && xe->xclient.message_type == wmProtocolsAtom) {
            Atom a = xe->xclient.data.l[0];
            if (a == wmDeleteWindowAtom)
                xw->closeEvent();
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
#if 1

    case ButtonPress:
        xw->mousePressEvent(&xe->xbutton);
        break;

    case ButtonRelease:
        xw->mouseReleaseEvent(&xe->xbutton);
        break;

    case MotionNotify:
        xw->mouseMoveEvent(&xe->xbutton);
        break;

    case XKeyPress:
        xw->keyPressEvent(&xe->xkey);
        break;

    case XKeyRelease:
        xw->keyReleaseEvent(&xe->xkey);
        break;
#endif

    default:
#ifdef MYX11_DEBUG
        qDebug() << "Other X event" << hex << xe->type;
#endif
        break;
    }
    return quit;
};


MyDisplay::MyDisplay()
{
    char *display_name = getenv("DISPLAY");
    display = XOpenDisplay(display_name);
    if (!display) {
        fprintf(stderr, "Cannot connect to X server: %s\n",
                display_name);
        exit(1);
    }

    original_x_errhandler = XSetErrorHandler(qt_x_errhandler);

    if (qgetenv("DO_X_SYNCHRONIZE").toInt())
        XSynchronize(display, true);


    screen = DefaultScreen(display);
    width = DisplayWidth(display, screen);
    height = DisplayHeight(display, screen);
    physicalWidth = DisplayWidthMM(display, screen);
    physicalHeight = DisplayHeightMM(display, screen);

    int xSocketNumber = XConnectionNumber(display);
#ifdef MYX11_DEBUG
    qDebug() << "X socket:"<< xSocketNumber;
#endif
    QSocketNotifier *sock = new QSocketNotifier(xSocketNumber, QSocketNotifier::Read, this);
    connect(sock, SIGNAL(activated(int)), this, SLOT(eventDispatcher()));

    wmProtocolsAtom = XInternAtom (display, "WM_PROTOCOLS", False);
    wmDeleteWindowAtom = XInternAtom (display, "WM_DELETE_WINDOW", False);

}


MyDisplay::~MyDisplay()
{
  XCloseDisplay(display);
}


void MyDisplay::eventDispatcher()
{
//    qDebug() << "eventDispatcher";


    ulong marker = XNextRequest(display);
//    int i = 0;
    while (XPending(display)) {
        XEvent event;
        XNextEvent(display, &event);
        /* done = */
        handleEvent(&event);

        if (event.xany.serial >= marker) {
#ifdef MYX11_DEBUG
            qDebug() << "potential livelock averted";
#endif
#if 0
            if (XEventsQueued(display, QueuedAfterFlush)) {
                qDebug() << "	with events queued";
                QTimer::singleShot(0, this, SLOT(eventDispatcher()));
            }
#endif
            break;
        }
    }
}


MyWindow::MyWindow(MyDisplay *display, int x, int y, int w, int h)
{
    xd = display;

    xd->windowList.append(this);

    window = XCreateSimpleWindow(xd->display, xd->rootWindow(),
                                 x, y, w, h, 0 /*border_width*/,
                                 xd->blackPixel(), xd->whitePixel());


#ifdef MYX11_DEBUG
    qDebug() << "MyWindow::MyWindow" << hex << window;
#endif


    width = -1;
    height = -1;
    xpos = -1;
    ypos = -1;

    XSetWindowBackgroundPixmap(xd->display, window, XNone);

    XSelectInput(xd->display, window, ExposureMask | KeyPressMask | KeyReleaseMask |
                 PointerMotionMask | ButtonPressMask |  ButtonReleaseMask | ButtonMotionMask |
                 StructureNotifyMask);

    gc = createGC();

    XChangeProperty (xd->display, window,
			   wmProtocolsAtom,
			   XA_ATOM, 32, PropModeAppend,
			   (unsigned char *) &wmDeleteWindowAtom, 1);
}

MyWindow::~MyWindow()
{
#ifdef MYX11_DEBUG
    qDebug() << "~MyWindow" << hex << window;
#endif
    XFreeGC(xd->display, gc);
    XDestroyWindow(xd->display, window);

    xd->windowList.removeAll(this);
}

GC MyWindow::createGC()
{
    GC gc;

    gc = XCreateGC(xd->display, window, 0, 0);
    if (gc < 0) {
        qWarning("MyWindow::createGC() could not create GC");
    }
    return gc;
}

void MyWindow::closeEvent()
{
    windowSurface->handleCloseEvent();
}

void MyWindow::paintEvent()
{
    Visual *visual = DefaultVisual(xd->display, xd->screen);

    if (!img.isNull()) {
        QImage image = img.convertToFormat(QImage::Format_RGB32);
        XImage *xi = XCreateImage(xd->display, visual, 24, ZPixmap,
                                  0, (char *) image.scanLine(0), image.width(), image.height(),
                                  32, image.bytesPerLine());

        int x = 0;
        int y = 0;

        /*int r =*/  XPutImage(xd->display, window, gc, xi, 0, 0, x, y, image.width(), image.height());

        xi->data = 0; // QImage owns these bits
        XDestroyImage(xi);
    }
}

void MyWindow::resizeEvent(XConfigureEvent *e)
{

    if ((e->width != width || e->height != height) && e->x == 0 && e->y == 0) {
        //qDebug() << "resize with bogus pos" << e->x << e->y << e->width << e->height << "window"<< hex << window;
    } else {
        //qDebug() << "geometry change" << e->x << e->y << e->width << e->height << "window"<< hex << window;
        xpos = e->x;
        ypos = e->y;
    }
    width = e->width;
    height = e->height;

    windowSurface->handleGeometryChange(xpos, ypos, width, height);
}


void MyWindow::setSize(int w, int h)
{
    XResizeWindow(xd->display, window, w, h);
}

void MyWindow::setGeometry(int x, int y, int w, int h)
{
    XMoveResizeWindow(xd->display, window, x, y, w, h);
}


void MyWindow::mousePressEvent(XButtonEvent *e)
{
    static long prevTime = 0;
    static Window prevWindow;
    static int prevX = -999;
    static int prevY = -999;

    QEvent::Type type = QEvent::MouseButtonPress;

    if (e->window == prevWindow && long(e->time) - prevTime < QApplication::doubleClickInterval()
        && qAbs(e->x - prevX) < 5 && qAbs(e->y - prevY) < 5) {
        type = QEvent::MouseButtonDblClick;
        prevTime = e->time - QApplication::doubleClickInterval(); //no double click next time
    } else {
        prevTime = e->time;
    }
    prevWindow = e->window;
    prevX = e->x;
    prevY = e->y;

    windowSurface->handleMouseEvent(type, e);
}

void MyWindow::mouseReleaseEvent(XButtonEvent *e)
{
    windowSurface->handleMouseEvent(QEvent::MouseButtonRelease, e);
}

void MyWindow::mouseMoveEvent(XButtonEvent *e)
{
    windowSurface->handleMouseEvent(QEvent::MouseMove, e);
}

#ifdef KeyPress
#undef KeyPress
#endif

void MyWindow::keyPressEvent(XKeyEvent *e)
{
    windowSurface->handleKeyEvent(QEvent::KeyPress, e);
}

#ifdef KeyRelease
#undef KeyRelease
#endif

void MyWindow::keyReleaseEvent(XKeyEvent *e)
{
    windowSurface->handleKeyEvent(QEvent::KeyRelease, e);
}


// WindowFlag stuff, lots of copied code from qwidget_x11.cpp...

//We're hacking here...


// MWM support
struct QtMWMHints {
    ulong flags, functions, decorations;
    long input_mode;
    ulong status;
};

enum {
    MWM_HINTS_FUNCTIONS   = (1L << 0),

    MWM_FUNC_ALL      = (1L << 0),
    MWM_FUNC_RESIZE   = (1L << 1),
    MWM_FUNC_MOVE     = (1L << 2),
    MWM_FUNC_MINIMIZE = (1L << 3),
    MWM_FUNC_MAXIMIZE = (1L << 4),
    MWM_FUNC_CLOSE    = (1L << 5),

    MWM_HINTS_DECORATIONS = (1L << 1),

    MWM_DECOR_ALL      = (1L << 0),
    MWM_DECOR_BORDER   = (1L << 1),
    MWM_DECOR_RESIZEH  = (1L << 2),
    MWM_DECOR_TITLE    = (1L << 3),
    MWM_DECOR_MENU     = (1L << 4),
    MWM_DECOR_MINIMIZE = (1L << 5),
    MWM_DECOR_MAXIMIZE = (1L << 6),

    MWM_HINTS_INPUT_MODE = (1L << 2),

    MWM_INPUT_MODELESS                  = 0L,
    MWM_INPUT_PRIMARY_APPLICATION_MODAL = 1L,
    MWM_INPUT_FULL_APPLICATION_MODAL    = 3L
};

static Atom mwm_hint_atom = XNone;

static QtMWMHints GetMWMHints(Display *display, Window window)
{
    QtMWMHints mwmhints;

    Atom type;
    int format;
    ulong nitems, bytesLeft;
    uchar *data = 0;
    if ((XGetWindowProperty(display, window, mwm_hint_atom, 0, 5, false,
                            mwm_hint_atom, &type, &format, &nitems, &bytesLeft,
                            &data) == Success)
        && (type == mwm_hint_atom
            && format == 32
            && nitems >= 5)) {
        mwmhints = *(reinterpret_cast<QtMWMHints *>(data));
    } else {
        mwmhints.flags = 0L;
        mwmhints.functions = MWM_FUNC_ALL;
        mwmhints.decorations = MWM_DECOR_ALL;
        mwmhints.input_mode = 0L;
        mwmhints.status = 0L;
    }

    if (data)
        XFree(data);

    return mwmhints;
}

static void SetMWMHints(Display *display, Window window, const QtMWMHints &mwmhints)
{
    if (mwmhints.flags != 0l) {
        XChangeProperty(display, window, mwm_hint_atom, mwm_hint_atom, 32,
                        PropModeReplace, (unsigned char *) &mwmhints, 5);
    } else {
        XDeleteProperty(display, window, mwm_hint_atom);
    }
}

// Returns true if we should set WM_TRANSIENT_FOR on \a w
static inline bool isTransient(const QWidget *w)
{
    return ((w->windowType() == Qt::Dialog
             || w->windowType() == Qt::Sheet
             || w->windowType() == Qt::Tool
             || w->windowType() == Qt::SplashScreen
             || w->windowType() == Qt::ToolTip
             || w->windowType() == Qt::Drawer
             || w->windowType() == Qt::Popup)
            && !w->testAttribute(Qt::WA_X11BypassTransientForHint));
}



Qt::WindowFlags MyWindow::setWindowFlags(Qt::WindowFlags flags)
{

    if (mwm_hint_atom == XNone) {
        mwm_hint_atom = XInternAtom(xd->display, "_MOTIF_WM_HINTS\0", False);
    }

#ifdef MYX11_DEBUG
    qDebug() << "MyWindow::setWindowFlags" << hex << mwm_hint_atom << "flags" << flags;
#endif
    Qt::WindowType type = static_cast<Qt::WindowType>(int(flags & Qt::WindowType_Mask));

    if (type == Qt::ToolTip)
        flags |= Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint;
    if (type == Qt::Popup)
        flags |= Qt::X11BypassWindowManagerHint;

    bool topLevel = (flags & Qt::Window);
    bool popup = (type == Qt::Popup);
    bool dialog = (type == Qt::Dialog
                   || type == Qt::Sheet);
    bool desktop = (type == Qt::Desktop);
    bool tool = (type == Qt::Tool || type == Qt::SplashScreen
                 || type == Qt::ToolTip || type == Qt::Drawer);


    XSetWindowAttributes wsa;

    QtMWMHints mwmhints;
    mwmhints.flags = 0L;
    mwmhints.functions = 0L;
    mwmhints.decorations = 0;
    mwmhints.input_mode = 0L;
    mwmhints.status = 0L;


    ulong wsa_mask = 0;
    if (type != Qt::SplashScreen) { // && customize) {
        mwmhints.flags |= MWM_HINTS_DECORATIONS;

        bool customize = flags & Qt::CustomizeWindowHint;
        if (!(flags & Qt::FramelessWindowHint) && !(customize && !(flags & Qt::WindowTitleHint))) {
            mwmhints.decorations |= MWM_DECOR_BORDER;
            mwmhints.decorations |= MWM_DECOR_RESIZEH;

            if (flags & Qt::WindowTitleHint)
                mwmhints.decorations |= MWM_DECOR_TITLE;

            if (flags & Qt::WindowSystemMenuHint)
                mwmhints.decorations |= MWM_DECOR_MENU;

            if (flags & Qt::WindowMinimizeButtonHint) {
                mwmhints.decorations |= MWM_DECOR_MINIMIZE;
                mwmhints.functions |= MWM_FUNC_MINIMIZE;
            }

            if (flags & Qt::WindowMaximizeButtonHint) {
                mwmhints.decorations |= MWM_DECOR_MAXIMIZE;
                mwmhints.functions |= MWM_FUNC_MAXIMIZE;
            }

            if (flags & Qt::WindowCloseButtonHint)
                mwmhints.functions |= MWM_FUNC_CLOSE;
        }
    } else {
        // if type == Qt::SplashScreen
        mwmhints.decorations = MWM_DECOR_ALL;
    }

    if (tool) {
        wsa.save_under = True;
        wsa_mask |= CWSaveUnder;
    }

    if (flags & Qt::X11BypassWindowManagerHint) {
        wsa.override_redirect = True;
        wsa_mask |= CWOverrideRedirect;
    }
#if 0
    if (wsa_mask && initializeWindow) {
        Q_ASSERT(id);
        XChangeWindowAttributes(dpy, id, wsa_mask, &wsa);
    }
#endif
    if (mwmhints.functions != 0) {
        mwmhints.flags |= MWM_HINTS_FUNCTIONS;
        mwmhints.functions |= MWM_FUNC_MOVE | MWM_FUNC_RESIZE;
    } else {
        mwmhints.functions = MWM_FUNC_ALL;
    }

    if (!(flags & Qt::FramelessWindowHint)
        && flags & Qt::CustomizeWindowHint
        && flags & Qt::WindowTitleHint
        && !(flags &
             (Qt::WindowMinimizeButtonHint
              | Qt::WindowMaximizeButtonHint
              | Qt::WindowCloseButtonHint))) {
        // a special case - only the titlebar without any button
        mwmhints.flags = MWM_HINTS_FUNCTIONS;
        mwmhints.functions = MWM_FUNC_MOVE | MWM_FUNC_RESIZE;
        mwmhints.decorations = 0;
    }

    SetMWMHints(xd->display, window, mwmhints);

//##### only if initializeWindow???

    if (popup) {                        // popup widget
#ifdef MYX11_DEBUG
        qDebug() << "Doing XChangeWindowAttributes for popup" << wsa.override_redirect;
#endif
        // set EWMH window types
        // setNetWmWindowTypes();

        wsa.override_redirect = True;
        wsa.save_under = True;
        XChangeWindowAttributes(xd->display, window, CWOverrideRedirect | CWSaveUnder,
                                &wsa);
    } else {
#ifdef MYX11_DEBUG
        qDebug() << "Doing XChangeWindowAttributes for non-popup";
#endif
    }

    return flags;
}

void MyWindow::setVisible(bool visible)
{
//    qDebug() << "MyWindow::setVisible" << visible << hex << window;
    if (visible)
         XMapWindow(xd->display, window);
    else
        XUnmapWindow(xd->display, window);
}
