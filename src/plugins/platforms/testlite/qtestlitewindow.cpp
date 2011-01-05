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

#include "qtestlitewindow.h"

#include "qtestliteintegration.h"
#include "qtestlitescreen.h"
#include "qtestlitekeyboard.h"
#include "qtestlitestaticinfo.h"

#include <QtGui/QWindowSystemInterface>
#include <QSocketNotifier>
#include <QApplication>
#include <QDebug>

#include <QtGui/private/qwindowsurface_p.h>
#include <QtGui/private/qapplication_p.h>

#if !defined(QT_NO_OPENGL)
#if !defined(QT_OPENGL_ES_2)
#include "qglxintegration.h"
#else
#include "../eglconvenience/qeglconvenience.h"
#include "../eglconvenience/qeglplatformcontext.h"
#include "qtestliteeglintegration.h"
#endif  //QT_OPENGL_ES_2
#endif //QT_NO_OPENGL

//#define MYX11_DEBUG

QT_BEGIN_NAMESPACE

QTestLiteWindow::QTestLiteWindow(QWidget *window)
    : QPlatformWindow(window)
    , mGLContext(0)
    , mScreen(QTestLiteScreen::testLiteScreenForWidget(window))
{
    int x = window->x();
    int y = window->y();
    int w = window->width();
    int h = window->height();

        if(window->platformWindowFormat().windowApi() == QPlatformWindowFormat::OpenGL
           && QApplicationPrivate::platformIntegration()->hasOpenGL() ) {
        #if !defined(QT_NO_OPENGL)
#if !defined(QT_OPENGL_ES_2)
            XVisualInfo *visualInfo = QGLXContext::findVisualInfo(mScreen,window->platformWindowFormat());
#else
            QPlatformWindowFormat windowFormat = correctColorBuffers(window->platformWindowFormat());

            EGLDisplay eglDisplay = eglGetDisplay(mScreen->display());
            EGLConfig eglConfig = q_configFromQPlatformWindowFormat(eglDisplay,windowFormat);
            VisualID id = QTestLiteEglIntegration::getCompatibleVisualId(mScreen->display(),eglConfig);

            XVisualInfo visualInfoTemplate;
            memset(&visualInfoTemplate, 0, sizeof(XVisualInfo));
            visualInfoTemplate.visualid = id;

            XVisualInfo *visualInfo;
            int matchingCount = 0;
            visualInfo = XGetVisualInfo(mScreen->display(), VisualIDMask, &visualInfoTemplate, &matchingCount);
#endif //!defined(QT_OPENGL_ES_2)
            if (visualInfo) {
                Colormap cmap = XCreateColormap(mScreen->display(),mScreen->rootWindow(),visualInfo->visual,AllocNone);

                XSetWindowAttributes a;
                a.colormap = cmap;
                x_window = XCreateWindow(mScreen->display(), mScreen->rootWindow(),x, y, w, h,
                                          0, visualInfo->depth, InputOutput, visualInfo->visual,
                                          CWColormap, &a);
            } else {
                qFatal("no window!");
            }
#endif //!defined(QT_NO_OPENGL)
        } else {
            x_window = XCreateSimpleWindow(mScreen->display(), mScreen->rootWindow(),
                                           x, y, w, h, 0 /*border_width*/,
                                           mScreen->blackPixel(), mScreen->whitePixel());
        }

#ifdef MYX11_DEBUG
        qDebug() << "QTestLiteWindow::QTestLiteWindow creating" << hex << x_window << window;
#endif

    XSetWindowBackgroundPixmap(mScreen->display(), x_window, XNone);

    XSelectInput(mScreen->display(), x_window,
                 ExposureMask | KeyPressMask | KeyReleaseMask |
                 EnterWindowMask | LeaveWindowMask | FocusChangeMask |
                 PointerMotionMask | ButtonPressMask |  ButtonReleaseMask |
                 ButtonMotionMask | PropertyChangeMask |
                 StructureNotifyMask);

    gc = createGC();

    Atom protocols[5];
    int n = 0;
    protocols[n++] = QTestLiteStatic::atom(QTestLiteStatic::WM_DELETE_WINDOW);        // support del window protocol
    protocols[n++] = QTestLiteStatic::atom(QTestLiteStatic::WM_TAKE_FOCUS);                // support take focus window protocol
    protocols[n++] = QTestLiteStatic::atom(QTestLiteStatic::_NET_WM_PING);                // support _NET_WM_PING protocol
#ifndef QT_NO_XSYNC
    protocols[n++] = QTestLiteStatic::atom(QTestLiteStatic::_NET_WM_SYNC_REQUEST);        // support _NET_WM_SYNC_REQUEST protocol
#endif // QT_NO_XSYNC
    if (window->windowFlags() & Qt::WindowContextHelpButtonHint)
        protocols[n++] = QTestLiteStatic::atom(QTestLiteStatic::_NET_WM_CONTEXT_HELP);
    XSetWMProtocols(mScreen->display(), x_window, protocols, n);
}



QTestLiteWindow::~QTestLiteWindow()
{
#ifdef MYX11_DEBUG
    qDebug() << "~QTestLiteWindow" << hex << x_window;
#endif
    delete mGLContext;
    XFreeGC(mScreen->display(), gc);
    XDestroyWindow(mScreen->display(), x_window);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Mouse event stuff
static Qt::MouseButtons translateMouseButtons(int s)
{
    Qt::MouseButtons ret = 0;
    if (s & Button1Mask)
        ret |= Qt::LeftButton;
    if (s & Button2Mask)
        ret |= Qt::MidButton;
    if (s & Button3Mask)
        ret |= Qt::RightButton;
    return ret;
}



void QTestLiteWindow::handleMouseEvent(QEvent::Type type, XButtonEvent *e)
{
    static QPoint mousePoint;

    Qt::MouseButton button = Qt::NoButton;
    Qt::MouseButtons buttons = translateMouseButtons(e->state);
    Qt::KeyboardModifiers modifiers = mScreen->keyboard()->translateModifiers(e->state);
    if (type != QEvent::MouseMove) {
        switch (e->button) {
        case Button1: button = Qt::LeftButton; break;
        case Button2: button = Qt::MidButton; break;
        case Button3: button = Qt::RightButton; break;
        case Button4:
        case Button5:
        case 6:
        case 7: {
            //mouse wheel
            if (type == QEvent::MouseButtonPress) {
                //logic borrowed from qapplication_x11.cpp
                int delta = 120 * ((e->button == Button4 || e->button == 6) ? 1 : -1);
                bool hor = (((e->button == Button4 || e->button == Button5)
                             && (modifiers & Qt::AltModifier))
                            || (e->button == 6 || e->button == 7));
                QWindowSystemInterface::handleWheelEvent(widget(), e->time,
                                                      QPoint(e->x, e->y),
                                                      QPoint(e->x_root, e->y_root),
                                                      delta, hor ? Qt::Horizontal : Qt::Vertical);
            }
            return;
        }
        default: break;
        }
    }

    buttons ^= button; // X event uses state *before*, Qt uses state *after*

    QWindowSystemInterface::handleMouseEvent(widget(), e->time, QPoint(e->x, e->y),
                                          QPoint(e->x_root, e->y_root),
                                          buttons);

    mousePoint = QPoint(e->x_root, e->y_root);
}

void QTestLiteWindow::handleCloseEvent()
{
    QWindowSystemInterface::handleCloseEvent(widget());
}


void QTestLiteWindow::handleEnterEvent()
{
    QWindowSystemInterface::handleEnterEvent(widget());
}

void QTestLiteWindow::handleLeaveEvent()
{
    QWindowSystemInterface::handleLeaveEvent(widget());
}

void QTestLiteWindow::handleFocusInEvent()
{
    QWindowSystemInterface::handleWindowActivated(widget());
}

void QTestLiteWindow::handleFocusOutEvent()
{
    QWindowSystemInterface::handleWindowActivated(0);
}



void QTestLiteWindow::setGeometry(const QRect &rect)
{
    XMoveResizeWindow(mScreen->display(), x_window, rect.x(), rect.y(), rect.width(), rect.height());
    QPlatformWindow::setGeometry(rect);
}


Qt::WindowFlags QTestLiteWindow::windowFlags() const
{
    return window_flags;
}

WId QTestLiteWindow::winId() const
{
    return x_window;
}

void QTestLiteWindow::setParent(const QPlatformWindow *window)
{
    QPoint topLeft = geometry().topLeft();
    XReparentWindow(mScreen->display(),x_window,window->winId(),topLeft.x(),topLeft.y());
}

void QTestLiteWindow::raise()
{
    XRaiseWindow(mScreen->display(), x_window);
}

void QTestLiteWindow::lower()
{
    XLowerWindow(mScreen->display(), x_window);
}

void QTestLiteWindow::setWindowTitle(const QString &title)
{
    QByteArray ba = title.toLatin1(); //We're not making a general solution here...
    XTextProperty windowName;
    windowName.value    = (unsigned char *)ba.constData();
    windowName.encoding = XA_STRING;
    windowName.format   = 8;
    windowName.nitems   = ba.length();

    XSetWMName(mScreen->display(), x_window, &windowName);
}

GC QTestLiteWindow::createGC()
{
    GC gc;

    gc = XCreateGC(mScreen->display(), x_window, 0, 0);
    if (gc < 0) {
        qWarning("QTestLiteWindow::createGC() could not create GC");
    }
    return gc;
}

void QTestLiteWindow::paintEvent()
{
#ifdef MYX11_DEBUG
//    qDebug() << "QTestLiteWindow::paintEvent" << shm_img.size() << painted;
#endif

    if (QWindowSurface *surface = widget()->windowSurface())
        surface->flush(widget(), widget()->geometry(), QPoint());
}

void QTestLiteWindow::requestActivateWindow()
{
    XSetInputFocus(mScreen->display(), x_window, XRevertToParent, CurrentTime);
}

void QTestLiteWindow::resizeEvent(XConfigureEvent *e)
{
    int xpos = geometry().x();
    int ypos = geometry().y();
    if ((e->width != geometry().width() || e->height != geometry().height()) && e->x == 0 && e->y == 0) {
        //qDebug() << "resize with bogus pos" << e->x << e->y << e->width << e->height << "window"<< hex << window;
    } else {
        //qDebug() << "geometry change" << e->x << e->y << e->width << e->height << "window"<< hex << window;
        xpos = e->x;
        ypos = e->y;
    }
#ifdef MYX11_DEBUG
    qDebug() << hex << x_window << dec << "ConfigureNotify" << e->x << e->y << e->width << e->height << "geometry" << xpos << ypos << width << height;
#endif

    QRect newRect(xpos, ypos, e->width, e->height);
    QWindowSystemInterface::handleGeometryChange(widget(), newRect);
}

void QTestLiteWindow::mousePressEvent(XButtonEvent *e)
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

    handleMouseEvent(type, e);
}

QtMWMHints QTestLiteWindow::getMWMHints() const
{
    QtMWMHints mwmhints;

    Atom type;
    int format;
    ulong nitems, bytesLeft;
    uchar *data = 0;
    Atom atomForMotifWmHints = QTestLiteStatic::atom(QTestLiteStatic::_MOTIF_WM_HINTS);
    if ((XGetWindowProperty(mScreen->display(), x_window, atomForMotifWmHints, 0, 5, false,
                            atomForMotifWmHints, &type, &format, &nitems, &bytesLeft,
                            &data) == Success)
        && (type == atomForMotifWmHints
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

void QTestLiteWindow::setMWMHints(const QtMWMHints &mwmhints)
{
    Atom atomForMotifWmHints = QTestLiteStatic::atom(QTestLiteStatic::_MOTIF_WM_HINTS);
    if (mwmhints.flags != 0l) {
        XChangeProperty(mScreen->display(), x_window,
                        atomForMotifWmHints, atomForMotifWmHints, 32,
                        PropModeReplace, (unsigned char *) &mwmhints, 5);
    } else {
        XDeleteProperty(mScreen->display(), x_window, atomForMotifWmHints);
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

Qt::WindowFlags QTestLiteWindow::setWindowFlags(Qt::WindowFlags flags)
{
//    Q_ASSERT(flags & Qt::Window);
    window_flags = flags;

#ifdef MYX11_DEBUG
    qDebug() << "QTestLiteWindow::setWindowFlags" << hex << x_window << "flags" << flags;
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

    Q_UNUSED(topLevel);
    Q_UNUSED(dialog);
    Q_UNUSED(desktop);

    bool tooltip = (type == Qt::ToolTip);

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

    setMWMHints(mwmhints);

//##### only if initializeWindow???

    if (popup || tooltip) {                        // popup widget
#ifdef MYX11_DEBUG
        qDebug() << "Doing XChangeWindowAttributes for popup" << wsa.override_redirect;
#endif
        // set EWMH window types
        // setNetWmWindowTypes();

        wsa.override_redirect = True;
        wsa.save_under = True;
        XChangeWindowAttributes(mScreen->display(), x_window, CWOverrideRedirect | CWSaveUnder,
                                &wsa);
    } else {
#ifdef MYX11_DEBUG
        qDebug() << "Doing XChangeWindowAttributes for non-popup";
#endif
    }

    return flags;
}

void QTestLiteWindow::setVisible(bool visible)
{
#ifdef MYX11_DEBUG
    qDebug() << "QTestLiteWindow::setVisible" << visible << hex << x_window;
#endif
    if (visible) {
        //ensure that the window is viewed in correct position.
        doSizeHints();
        XMapWindow(mScreen->display(), x_window);
    } else {
        XUnmapWindow(mScreen->display(), x_window);
    }
}

void QTestLiteWindow::setCursor(const Cursor &cursor)
{
    XDefineCursor(mScreen->display(), x_window, cursor);
    XFlush(mScreen->display());
}

QPlatformGLContext *QTestLiteWindow::glContext() const
{
    if (!QApplicationPrivate::platformIntegration()->hasOpenGL())
        return 0;
    if (!mGLContext) {
        QTestLiteWindow *that = const_cast<QTestLiteWindow *>(this);
#if !defined(QT_NO_OPENGL)
#if !defined(QT_OPENGL_ES_2)
        that->mGLContext = new QGLXContext(x_window, mScreen,widget()->platformWindowFormat());
#else
        EGLDisplay display = eglGetDisplay(mScreen->display());

        QPlatformWindowFormat windowFormat = correctColorBuffers(widget()->platformWindowFormat());

        EGLConfig config = q_configFromQPlatformWindowFormat(display,windowFormat);
        QVector<EGLint> eglContextAttrs;
        eglContextAttrs.append(EGL_CONTEXT_CLIENT_VERSION);
        eglContextAttrs.append(2);
        eglContextAttrs.append(EGL_NONE);

        EGLSurface eglSurface = eglCreateWindowSurface(display,config,(EGLNativeWindowType)x_window,0);
        that->mGLContext = new QEGLPlatformContext(display, config, eglContextAttrs.data(), eglSurface, EGL_OPENGL_ES_API);
#endif
#endif
    }
    return mGLContext;
}

Window QTestLiteWindow::xWindow() const
{
    return x_window;
}

GC QTestLiteWindow::graphicsContext() const
{
    return gc;
}

void QTestLiteWindow::doSizeHints()
{
    Q_ASSERT(widget()->testAttribute(Qt::WA_WState_Created));
    XSizeHints s;
    s.flags = 0;
    QRect g = geometry();
    s.x = g.x();
    s.y = g.y();
    s.width = g.width();
    s.height = g.height();
    s.flags |= USPosition;
    s.flags |= PPosition;
    s.flags |= USSize;
    s.flags |= PSize;
    s.flags |= PWinGravity;
    s.win_gravity = QApplication::isRightToLeft() ? NorthEastGravity : NorthWestGravity;
    XSetWMNormalHints(mScreen->display(), x_window, &s);
}

QPlatformWindowFormat QTestLiteWindow::correctColorBuffers(const QPlatformWindowFormat &platformWindowFormat) const
{
    // I have only tested this setup on a dodgy intel setup, where I didn't use standard libs,
    // so this might be not what we want to do :)
    if ( !(platformWindowFormat.redBufferSize() == -1   &&
           platformWindowFormat.greenBufferSize() == -1 &&
           platformWindowFormat.blueBufferSize() == -1))
        return platformWindowFormat;

    QPlatformWindowFormat windowFormat = platformWindowFormat;
    if (mScreen->depth() == 16) {
        windowFormat.setRedBufferSize(5);
        windowFormat.setGreenBufferSize(6);
        windowFormat.setBlueBufferSize(5);
    } else {
        windowFormat.setRedBufferSize(8);
        windowFormat.setGreenBufferSize(8);
        windowFormat.setBlueBufferSize(8);
    }

    return windowFormat;
}

QT_END_NAMESPACE
