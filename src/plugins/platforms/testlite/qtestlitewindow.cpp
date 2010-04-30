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

#include "qtestliteintegration.h"
#include <QWindowSystemInterface>

#include "qtestlitewindow.h"

#include <QBitmap>
#include <QCursor>
#include <QDateTime>
#include <QPixmap>
#include <QImage>
#include <QSocketNotifier>

#include <qdebug.h>
#include <QTimer>
#include <QApplication>

#include <stdio.h>
#include <stdlib.h>


#include <X11/Xatom.h>

#include <X11/cursorfont.h>



//### remove stuff we don't want from qt_x11_p.h
#undef ATOM
#undef X11

QT_BEGIN_NAMESPACE

static int (*original_x_errhandler)(Display *dpy, XErrorEvent *);
static bool seen_badwindow;


static Atom wmProtocolsAtom;
static Atom wmDeleteWindowAtom;



class MyX11CursorNode
{
public:
    MyX11CursorNode(int id, Cursor c) { idValue = id; cursorValue = c; refCount = 1; }
    QDateTime expiration() { return t; }
    void setExpiration(QDateTime val) { t = val; }
    MyX11CursorNode * ante() { return before; }
    void setAnte(MyX11CursorNode *node) { before = node; }
    MyX11CursorNode * post() { return after; }
    void setPost(MyX11CursorNode *node) { after = node; }
    Cursor cursor() { return cursorValue; }
    int id() { return idValue; }
    unsigned int refCount;

private:
    MyX11CursorNode *before;
    MyX11CursorNode *after;
    QDateTime t;
    Cursor cursorValue;
    int idValue;

    Display * display;
};





class MyX11Cursors : public QObject
{
    Q_OBJECT
public:
    MyX11Cursors(Display * d);
    ~MyX11Cursors() { timer.stop(); }
    void incrementUseCount(int id);
    void decrementUseCount(int id);
    void createNode(int id, Cursor c);
    bool exists(int id) { return lookupMap.contains(id); }
    Cursor cursor(int id);
public slots:
    void timeout();

private:
    void removeNode(MyX11CursorNode *node);
    void insertNode(MyX11CursorNode *node);

    // linked list of cursors currently not assigned to any window
    MyX11CursorNode *firstExpired;
    MyX11CursorNode *lastExpired;

    QHash<int, MyX11CursorNode *> lookupMap;
    QTimer timer;

    Display *display;

    int removalDelay;
};





QTestLiteWindow::QTestLiteWindow(const QTestLiteIntegration *platformIntegration,
                                 QTestLiteScreen */*screen*/, QWidget *window)
    :QPlatformWindow(window)
{
    xd = platformIntegration->xd;
    xd->windowList.append(this);
    {
        int x = 0;
        int y = 0;
        int w = 300;
        int h = 300; //###

        x_window = XCreateSimpleWindow(xd->display, xd->rootWindow(),
                                       x, y, w, h, 0 /*border_width*/,
                                       xd->blackPixel(), xd->whitePixel());

    }

    width = -1;
    height = -1;
    xpos = -1;
    ypos = -1;

    XSetWindowBackgroundPixmap(xd->display, x_window, XNone);

    XSelectInput(xd->display, x_window, ExposureMask | KeyPressMask | KeyReleaseMask |
                 EnterWindowMask | LeaveWindowMask | FocusChangeMask |
                 PointerMotionMask | ButtonPressMask |  ButtonReleaseMask | ButtonMotionMask |
                 StructureNotifyMask);

    gc = createGC();

    XChangeProperty (xd->display, x_window,
			   wmProtocolsAtom,
			   XA_ATOM, 32, PropModeAppend,
			   (unsigned char *) &wmDeleteWindowAtom, 1);


    setWindowTitle(QLatin1String("Qt Lighthouse"));

    currentCursor = -1;

    setWindowFlags(window->windowFlags()); //##### This should not be the plugin's responsibility

    //xw->windowTL = this;
}


QTestLiteWindow::~QTestLiteWindow()
{
#ifdef MYX11_DEBUG
    qDebug() << "~QTestLiteWindow" << hex << x_window;
#endif
    XFreeGC(xd->display, gc);
    XDestroyWindow(xd->display, x_window);

    xd->windowList.removeAll(this);
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


static Qt::KeyboardModifiers translateModifiers(int s)
{
    const uchar qt_alt_mask = Mod1Mask;
    const uchar qt_meta_mask = Mod4Mask;


    Qt::KeyboardModifiers ret = 0;
    if (s & ShiftMask)
        ret |= Qt::ShiftModifier;
    if (s & ControlMask)
        ret |= Qt::ControlModifier;
    if (s & qt_alt_mask)
        ret |= Qt::AltModifier;
    if (s & qt_meta_mask)
        ret |= Qt::MetaModifier;
#if 0
    if (s & qt_mode_switch_mask)
        ret |= Qt::GroupSwitchModifier;
#endif
    return ret;
}

void QTestLiteWindow::handleMouseEvent(QEvent::Type type, XButtonEvent *e)
{
    static QPoint mousePoint;

    Qt::MouseButton button = Qt::NoButton;
    Qt::MouseButtons buttons = translateMouseButtons(e->state);
    Qt::KeyboardModifiers modifiers = translateModifiers(e->state);
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


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Key event stuff -- not pretty either
//
// What we want to do is to port Robert's keytable code properly

// keyboard mapping table
static const unsigned int keyTbl[] = {

    // misc keys

    XK_Escape,                  Qt::Key_Escape,
    XK_Tab,                     Qt::Key_Tab,
    XK_ISO_Left_Tab,            Qt::Key_Backtab,
    XK_BackSpace,               Qt::Key_Backspace,
    XK_Return,                  Qt::Key_Return,
    XK_Insert,                  Qt::Key_Insert,
    XK_Delete,                  Qt::Key_Delete,
    XK_Clear,                   Qt::Key_Delete,
    XK_Pause,                   Qt::Key_Pause,
    XK_Print,                   Qt::Key_Print,
    0x1005FF60,                 Qt::Key_SysReq,         // hardcoded Sun SysReq
    0x1007ff00,                 Qt::Key_SysReq,         // hardcoded X386 SysReq

    // cursor movement

    XK_Home,                    Qt::Key_Home,
    XK_End,                     Qt::Key_End,
    XK_Left,                    Qt::Key_Left,
    XK_Up,                      Qt::Key_Up,
    XK_Right,                   Qt::Key_Right,
    XK_Down,                    Qt::Key_Down,
    XK_Prior,                   Qt::Key_PageUp,
    XK_Next,                    Qt::Key_PageDown,

    // modifiers

    XK_Shift_L,                 Qt::Key_Shift,
    XK_Shift_R,                 Qt::Key_Shift,
    XK_Shift_Lock,              Qt::Key_Shift,
    XK_Control_L,               Qt::Key_Control,
    XK_Control_R,               Qt::Key_Control,
    XK_Meta_L,                  Qt::Key_Meta,
    XK_Meta_R,                  Qt::Key_Meta,
    XK_Alt_L,                   Qt::Key_Alt,
    XK_Alt_R,                   Qt::Key_Alt,
    XK_Caps_Lock,               Qt::Key_CapsLock,
    XK_Num_Lock,                Qt::Key_NumLock,
    XK_Scroll_Lock,             Qt::Key_ScrollLock,
    XK_Super_L,                 Qt::Key_Super_L,
    XK_Super_R,                 Qt::Key_Super_R,
    XK_Menu,                    Qt::Key_Menu,
    XK_Hyper_L,                 Qt::Key_Hyper_L,
    XK_Hyper_R,                 Qt::Key_Hyper_R,
    XK_Help,                    Qt::Key_Help,
    0x1000FF74,                 Qt::Key_Backtab,        // hardcoded HP backtab
    0x1005FF10,                 Qt::Key_F11,            // hardcoded Sun F36 (labeled F11)
    0x1005FF11,                 Qt::Key_F12,            // hardcoded Sun F37 (labeled F12)

    // numeric and function keypad keys

    XK_KP_Space,                Qt::Key_Space,
    XK_KP_Tab,                  Qt::Key_Tab,
    XK_KP_Enter,                Qt::Key_Enter,
    //XK_KP_F1,                 Qt::Key_F1,
    //XK_KP_F2,                 Qt::Key_F2,
    //XK_KP_F3,                 Qt::Key_F3,
    //XK_KP_F4,                 Qt::Key_F4,
    XK_KP_Home,                 Qt::Key_Home,
    XK_KP_Left,                 Qt::Key_Left,
    XK_KP_Up,                   Qt::Key_Up,
    XK_KP_Right,                Qt::Key_Right,
    XK_KP_Down,                 Qt::Key_Down,
    XK_KP_Prior,                Qt::Key_PageUp,
    XK_KP_Next,                 Qt::Key_PageDown,
    XK_KP_End,                  Qt::Key_End,
    XK_KP_Begin,                Qt::Key_Clear,
    XK_KP_Insert,               Qt::Key_Insert,
    XK_KP_Delete,               Qt::Key_Delete,
    XK_KP_Equal,                Qt::Key_Equal,
    XK_KP_Multiply,             Qt::Key_Asterisk,
    XK_KP_Add,                  Qt::Key_Plus,
    XK_KP_Separator,            Qt::Key_Comma,
    XK_KP_Subtract,             Qt::Key_Minus,
    XK_KP_Decimal,              Qt::Key_Period,
    XK_KP_Divide,               Qt::Key_Slash,

    // International input method support keys

    // International & multi-key character composition
    XK_ISO_Level3_Shift,        Qt::Key_AltGr,
    XK_Multi_key,		Qt::Key_Multi_key,
    XK_Codeinput,		Qt::Key_Codeinput,
    XK_SingleCandidate,		Qt::Key_SingleCandidate,
    XK_MultipleCandidate,	Qt::Key_MultipleCandidate,
    XK_PreviousCandidate,	Qt::Key_PreviousCandidate,

    // Misc Functions
    XK_Mode_switch,		Qt::Key_Mode_switch,
    XK_script_switch,		Qt::Key_Mode_switch,

    // Japanese keyboard support
    XK_Kanji,			Qt::Key_Kanji,
    XK_Muhenkan,		Qt::Key_Muhenkan,
    //XK_Henkan_Mode,		Qt::Key_Henkan_Mode,
    XK_Henkan_Mode,		Qt::Key_Henkan,
    XK_Henkan,			Qt::Key_Henkan,
    XK_Romaji,			Qt::Key_Romaji,
    XK_Hiragana,		Qt::Key_Hiragana,
    XK_Katakana,		Qt::Key_Katakana,
    XK_Hiragana_Katakana,	Qt::Key_Hiragana_Katakana,
    XK_Zenkaku,			Qt::Key_Zenkaku,
    XK_Hankaku,			Qt::Key_Hankaku,
    XK_Zenkaku_Hankaku,		Qt::Key_Zenkaku_Hankaku,
    XK_Touroku,			Qt::Key_Touroku,
    XK_Massyo,			Qt::Key_Massyo,
    XK_Kana_Lock,		Qt::Key_Kana_Lock,
    XK_Kana_Shift,		Qt::Key_Kana_Shift,
    XK_Eisu_Shift,		Qt::Key_Eisu_Shift,
    XK_Eisu_toggle,		Qt::Key_Eisu_toggle,
    //XK_Kanji_Bangou,		Qt::Key_Kanji_Bangou,
    //XK_Zen_Koho,		Qt::Key_Zen_Koho,
    //XK_Mae_Koho,		Qt::Key_Mae_Koho,
    XK_Kanji_Bangou,		Qt::Key_Codeinput,
    XK_Zen_Koho,		Qt::Key_MultipleCandidate,
    XK_Mae_Koho,		Qt::Key_PreviousCandidate,

#ifdef XK_KOREAN
    // Korean keyboard support
    XK_Hangul,			Qt::Key_Hangul,
    XK_Hangul_Start,		Qt::Key_Hangul_Start,
    XK_Hangul_End,		Qt::Key_Hangul_End,
    XK_Hangul_Hanja,		Qt::Key_Hangul_Hanja,
    XK_Hangul_Jamo,		Qt::Key_Hangul_Jamo,
    XK_Hangul_Romaja,		Qt::Key_Hangul_Romaja,
    //XK_Hangul_Codeinput,	Qt::Key_Hangul_Codeinput,
    XK_Hangul_Codeinput,	Qt::Key_Codeinput,
    XK_Hangul_Jeonja,		Qt::Key_Hangul_Jeonja,
    XK_Hangul_Banja,		Qt::Key_Hangul_Banja,
    XK_Hangul_PreHanja,		Qt::Key_Hangul_PreHanja,
    XK_Hangul_PostHanja,	Qt::Key_Hangul_PostHanja,
    //XK_Hangul_SingleCandidate,Qt::Key_Hangul_SingleCandidate,
    //XK_Hangul_MultipleCandidate,Qt::Key_Hangul_MultipleCandidate,
    //XK_Hangul_PreviousCandidate,Qt::Key_Hangul_PreviousCandidate,
    XK_Hangul_SingleCandidate,	Qt::Key_SingleCandidate,
    XK_Hangul_MultipleCandidate,Qt::Key_MultipleCandidate,
    XK_Hangul_PreviousCandidate,Qt::Key_PreviousCandidate,
    XK_Hangul_Special,		Qt::Key_Hangul_Special,
    //XK_Hangul_switch,		Qt::Key_Hangul_switch,
    XK_Hangul_switch,		Qt::Key_Mode_switch,
#endif  // XK_KOREAN

    // dead keys
    XK_dead_grave,              Qt::Key_Dead_Grave,
    XK_dead_acute,              Qt::Key_Dead_Acute,
    XK_dead_circumflex,         Qt::Key_Dead_Circumflex,
    XK_dead_tilde,              Qt::Key_Dead_Tilde,
    XK_dead_macron,             Qt::Key_Dead_Macron,
    XK_dead_breve,              Qt::Key_Dead_Breve,
    XK_dead_abovedot,           Qt::Key_Dead_Abovedot,
    XK_dead_diaeresis,          Qt::Key_Dead_Diaeresis,
    XK_dead_abovering,          Qt::Key_Dead_Abovering,
    XK_dead_doubleacute,        Qt::Key_Dead_Doubleacute,
    XK_dead_caron,              Qt::Key_Dead_Caron,
    XK_dead_cedilla,            Qt::Key_Dead_Cedilla,
    XK_dead_ogonek,             Qt::Key_Dead_Ogonek,
    XK_dead_iota,               Qt::Key_Dead_Iota,
    XK_dead_voiced_sound,       Qt::Key_Dead_Voiced_Sound,
    XK_dead_semivoiced_sound,   Qt::Key_Dead_Semivoiced_Sound,
    XK_dead_belowdot,           Qt::Key_Dead_Belowdot,
    XK_dead_hook,               Qt::Key_Dead_Hook,
    XK_dead_horn,               Qt::Key_Dead_Horn,

#if 0
    // Special multimedia keys
    // currently only tested with MS internet keyboard

    // browsing keys
    XF86XK_Back,                Qt::Key_Back,
    XF86XK_Forward,             Qt::Key_Forward,
    XF86XK_Stop,                Qt::Key_Stop,
    XF86XK_Refresh,             Qt::Key_Refresh,
    XF86XK_Favorites,           Qt::Key_Favorites,
    XF86XK_AudioMedia,          Qt::Key_LaunchMedia,
    XF86XK_OpenURL,             Qt::Key_OpenUrl,
    XF86XK_HomePage,            Qt::Key_HomePage,
    XF86XK_Search,              Qt::Key_Search,

    // media keys
    XF86XK_AudioLowerVolume,    Qt::Key_VolumeDown,
    XF86XK_AudioMute,           Qt::Key_VolumeMute,
    XF86XK_AudioRaiseVolume,    Qt::Key_VolumeUp,
    XF86XK_AudioPlay,           Qt::Key_MediaPlay,
    XF86XK_AudioStop,           Qt::Key_MediaStop,
    XF86XK_AudioPrev,           Qt::Key_MediaPrevious,
    XF86XK_AudioNext,           Qt::Key_MediaNext,
    XF86XK_AudioRecord,         Qt::Key_MediaRecord,

    // launch keys
    XF86XK_Mail,                Qt::Key_LaunchMail,
    XF86XK_MyComputer,          Qt::Key_Launch0,
    XF86XK_Calculator,          Qt::Key_Launch1,
    XF86XK_Standby,             Qt::Key_Standby,

    XF86XK_Launch0,             Qt::Key_Launch2,
    XF86XK_Launch1,             Qt::Key_Launch3,
    XF86XK_Launch2,             Qt::Key_Launch4,
    XF86XK_Launch3,             Qt::Key_Launch5,
    XF86XK_Launch4,             Qt::Key_Launch6,
    XF86XK_Launch5,             Qt::Key_Launch7,
    XF86XK_Launch6,             Qt::Key_Launch8,
    XF86XK_Launch7,             Qt::Key_Launch9,
    XF86XK_Launch8,             Qt::Key_LaunchA,
    XF86XK_Launch9,             Qt::Key_LaunchB,
    XF86XK_LaunchA,             Qt::Key_LaunchC,
    XF86XK_LaunchB,             Qt::Key_LaunchD,
    XF86XK_LaunchC,             Qt::Key_LaunchE,
    XF86XK_LaunchD,             Qt::Key_LaunchF,
#endif

#if 0
    // Qtopia keys
    QTOPIAXK_Select,            Qt::Key_Select,
    QTOPIAXK_Yes,               Qt::Key_Yes,
    QTOPIAXK_No,                Qt::Key_No,
    QTOPIAXK_Cancel,            Qt::Key_Cancel,
    QTOPIAXK_Printer,           Qt::Key_Printer,
    QTOPIAXK_Execute,           Qt::Key_Execute,
    QTOPIAXK_Sleep,             Qt::Key_Sleep,
    QTOPIAXK_Play,              Qt::Key_Play,
    QTOPIAXK_Zoom,              Qt::Key_Zoom,
    QTOPIAXK_Context1,          Qt::Key_Context1,
    QTOPIAXK_Context2,          Qt::Key_Context2,
    QTOPIAXK_Context3,          Qt::Key_Context3,
    QTOPIAXK_Context4,          Qt::Key_Context4,
    QTOPIAXK_Call,              Qt::Key_Call,
    QTOPIAXK_Hangup,            Qt::Key_Hangup,
    QTOPIAXK_Flip,              Qt::Key_Flip,
#endif
    0,                          0
};


static int lookupCode(unsigned int xkeycode)
{
    if (xkeycode >= XK_F1 && xkeycode <= XK_F35)
        return Qt::Key_F1 + (int(xkeycode) - XK_F1);

    const unsigned int *p = keyTbl;
    while (*p) {
        if (*p == xkeycode)
            return *++p;
        p += 2;
    }

    return 0;
}


static Qt::KeyboardModifiers modifierFromKeyCode(int qtcode)
{
    switch (qtcode) {
    case Qt::Key_Control:
        return Qt::ControlModifier;
    case Qt::Key_Alt:
        return Qt::AltModifier;
    case Qt::Key_Shift:
        return Qt::ShiftModifier;
    case Qt::Key_Meta:
        return Qt::MetaModifier;
    default:
        return Qt::NoModifier;
    }
}

void QTestLiteWindow::handleKeyEvent(QEvent::Type type, void *ev)
{
    XKeyEvent *e = static_cast<XKeyEvent*>(ev);

    KeySym keySym;
    QByteArray chars;
    chars.resize(513);

    int count = XLookupString(e, chars.data(), chars.size(), &keySym, 0);
    Q_UNUSED(count);
//      qDebug() << "QTLWS::handleKeyEvent" << count << hex << "XKeysym:" << keySym;
//      if (count)
//          qDebug() << hex << int(chars[0]) << "String:"             << chars;

    Qt::KeyboardModifiers modifiers = translateModifiers(e->state);

    int qtcode = lookupCode(keySym);
//    qDebug() << "lookup: " << hex << keySym << qtcode << "mod" << modifiers;

    //X11 specifies state *before*, Qt expects state *after* the event

    modifiers ^= modifierFromKeyCode(qtcode);

    if (qtcode) {
        QWindowSystemInterface::handleKeyEvent(widget(), e->time, type, qtcode, modifiers);
    } else if (chars[0]) {
        int qtcode = chars.toUpper()[0]; //Not exactly right...
	if (modifiers & Qt::ControlModifier && qtcode < ' ')
	  qtcode = chars[0] + '@';
        QWindowSystemInterface::handleKeyEvent(widget(), e->time, type, qtcode, modifiers, QString::fromLatin1(chars));
    } else {
        qWarning() << "unknown X keycode" << hex << e->keycode << keySym;
    }
}

void QTestLiteWindow::setGeometry(const QRect &rect)
{
    QRect oldRect = geometry();
    if (rect == oldRect)
        return;

    //if unchanged ###
    XMoveResizeWindow(xd->display, x_window, rect.x(), rect.y(), rect.width(), rect.height());
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

void QTestLiteWindow::raise()
{
    XRaiseWindow(xd->display, x_window);
}

void QTestLiteWindow::lower()
{
    XLowerWindow(xd->display, x_window);
}

void QTestLiteWindow::setWindowTitle(const QString &title)
{
    QByteArray ba = title.toLatin1(); //We're not making a general solution here...
    XTextProperty windowName;
    windowName.value    = (unsigned char *)ba.constData();
    windowName.encoding = XA_STRING;
    windowName.format   = 8;
    windowName.nitems   = ba.length();

    XSetWMName(xd->display, x_window, &windowName);
}



GC QTestLiteWindow::createGC()
{
    GC gc;

    gc = XCreateGC(xd->display, x_window, 0, 0);
    if (gc < 0) {
        qWarning("QTestLiteWindow::createGC() could not create GC");
    }
    return gc;
}


void QTestLiteWindow::paintEvent()
{
#ifdef MYX11_DEBUG
    qDebug() << "QTestLiteWindow::paintEvent" << shm_img.size() << painted;
#endif

    widget()->windowSurface()->flush(widget(), QRect(xpos,ypos,width, height), QPoint());
}


void QTestLiteWindow::resizeEvent(XConfigureEvent *e)
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

#ifdef MYX11_DEBUG
    qDebug() << hex << window << dec << "ConfigureNotify" << e->x << e->y << e->width << e->height << "geometry" << xpos << ypos << width << height << "img:" << shm_img.size();
#endif

    QWindowSystemInterface::handleGeometryChange(widget(), QRect(xpos, ypos, width, height));
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

#if 0
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
#endif

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



Qt::WindowFlags QTestLiteWindow::setWindowFlags(Qt::WindowFlags flags)
{
    Q_ASSERT(flags & Qt::Window);
    window_flags = flags;

    if (mwm_hint_atom == XNone) {
        mwm_hint_atom = XInternAtom(xd->display, "_MOTIF_WM_HINTS\0", False);
    }

#ifdef MYX11_DEBUG
    qDebug() << "QTestLiteWindow::setWindowFlags" << hex << window << "flags" << flags;
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

    SetMWMHints(xd->display, x_window, mwmhints);

//##### only if initializeWindow???

    if (popup || tooltip) {                        // popup widget
#ifdef MYX11_DEBUG
        qDebug() << "Doing XChangeWindowAttributes for popup" << wsa.override_redirect;
#endif
        // set EWMH window types
        // setNetWmWindowTypes();

        wsa.override_redirect = True;
        wsa.save_under = True;
        XChangeWindowAttributes(xd->display, x_window, CWOverrideRedirect | CWSaveUnder,
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
    qDebug() << "QTestLiteWindow::setVisible" << visible << hex << window;
#endif
    if (visible)
         XMapWindow(xd->display, x_window);
    else
        XUnmapWindow(xd->display, x_window);
}


void QTestLiteWindow::setCursor(QCursor * cursor)
{
    int id = cursor->handle();
    if (id == currentCursor)
        return;
    Cursor c;
    if (!xd->cursors->exists(id)) {
        if (cursor->shape() == Qt::BitmapCursor)
            c = createCursorBitmap(cursor);
        else
            c = createCursorShape(cursor->shape());
        if (!c) {
            return;
        }
        xd->cursors->createNode(id, c);
    } else {
        xd->cursors->incrementUseCount(id);
        c = xd->cursors->cursor(id);
    }

    if (currentCursor != -1)
        xd->cursors->decrementUseCount(currentCursor);
    currentCursor = id;

    XDefineCursor(xd->display, x_window, c);
    XFlush(xd->display);
}

Cursor QTestLiteWindow::createCursorBitmap(QCursor * cursor)
{
    XColor bg, fg;
    bg.red   = 255 << 8;
    bg.green = 255 << 8;
    bg.blue  = 255 << 8;
    fg.red   = 0;
    fg.green = 0;
    fg.blue  = 0;
    QPoint spot = cursor->hotSpot();
    Window rootwin = x_window;

    QImage mapImage = cursor->bitmap()->toImage().convertToFormat(QImage::Format_MonoLSB);
    QImage maskImage = cursor->mask()->toImage().convertToFormat(QImage::Format_MonoLSB);

    int width = cursor->bitmap()->width();
    int height = cursor->bitmap()->height();
    int bytesPerLine = mapImage.bytesPerLine();
    int destLineSize = width / 8;
    if (width % 8)
        destLineSize++;

    const uchar * map = mapImage.bits();
    const uchar * mask = maskImage.bits();

    char * mapBits = new char[height * destLineSize];
    char * maskBits = new char[height * destLineSize];
    for (int i = 0; i < height; i++) {
        memcpy(mapBits + (destLineSize * i),map + (bytesPerLine * i), destLineSize);
        memcpy(maskBits + (destLineSize * i),mask + (bytesPerLine * i), destLineSize);
    }

    Pixmap cp = XCreateBitmapFromData(xd->display, rootwin, mapBits, width, height);
    Pixmap mp = XCreateBitmapFromData(xd->display, rootwin, maskBits, width, height);
    Cursor c = XCreatePixmapCursor(xd->display, cp, mp, &fg, &bg, spot.x(), spot.y());
    XFreePixmap(xd->display, cp);
    XFreePixmap(xd->display, mp);
    delete[] mapBits;
    delete[] maskBits;

    return c;
}

Cursor QTestLiteWindow::createCursorShape(int cshape)
{
    Cursor cursor = 0;

    if (cshape < 0 || cshape > Qt::LastCursor)
        return 0;

    switch (cshape) {
    case Qt::ArrowCursor:
        cursor =  XCreateFontCursor(xd->display, XC_left_ptr);
        break;
    case Qt::UpArrowCursor:
        cursor =  XCreateFontCursor(xd->display, XC_center_ptr);
        break;
    case Qt::CrossCursor:
        cursor =  XCreateFontCursor(xd->display, XC_crosshair);
        break;
    case Qt::WaitCursor:
        cursor =  XCreateFontCursor(xd->display, XC_watch);
        break;
    case Qt::IBeamCursor:
        cursor =  XCreateFontCursor(xd->display, XC_xterm);
        break;
    case Qt::SizeAllCursor:
        cursor =  XCreateFontCursor(xd->display, XC_fleur);
        break;
    case Qt::PointingHandCursor:
        cursor =  XCreateFontCursor(xd->display, XC_hand2);
        break;
    case Qt::SizeBDiagCursor:
        cursor =  XCreateFontCursor(xd->display, XC_top_right_corner);
        break;
    case Qt::SizeFDiagCursor:
        cursor =  XCreateFontCursor(xd->display, XC_bottom_right_corner);
        break;
    case Qt::SizeVerCursor:
    case Qt::SplitVCursor:
        cursor = XCreateFontCursor(xd->display, XC_sb_v_double_arrow);
        break;
    case Qt::SizeHorCursor:
    case Qt::SplitHCursor:
        cursor = XCreateFontCursor(xd->display, XC_sb_h_double_arrow);
        break;
    case Qt::WhatsThisCursor:
        cursor = XCreateFontCursor(xd->display, XC_question_arrow);
        break;
    case Qt::ForbiddenCursor:
        cursor = XCreateFontCursor(xd->display, XC_circle);
        break;
    case Qt::BusyCursor:
        cursor = XCreateFontCursor(xd->display, XC_watch);
        break;

    default: //default cursor for all the rest
        break;
    }
    return cursor;
}


MyX11Cursors::MyX11Cursors(Display * d) : firstExpired(0), lastExpired(0), display(d), removalDelay(3)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void MyX11Cursors::insertNode(MyX11CursorNode * node)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime timeout = now.addSecs(removalDelay);
    node->setExpiration(timeout);
    node->setPost(0);
    if (lastExpired) {
        lastExpired->setPost(node);
        node->setAnte(lastExpired);
    }
    lastExpired = node;
    if (!firstExpired) {
        firstExpired = node;
        node->setAnte(0);
        int interval = removalDelay * 1000;
        timer.setInterval(interval);
        timer.start();
    }
}

void MyX11Cursors::removeNode(MyX11CursorNode * node)
{
    MyX11CursorNode *pre = node->ante();
    MyX11CursorNode *post = node->post();
    if (pre)
        pre->setPost(post);
    if (post)
        post->setAnte(pre);
    if (node == lastExpired)
        lastExpired = pre;
    if (node == firstExpired) {
        firstExpired = post;
        if (!firstExpired) {
            timer.stop();
            return;
        }
        int interval = QDateTime::currentDateTime().secsTo(firstExpired->expiration()) * 1000;
        timer.stop();
        timer.setInterval(interval);
        timer.start();
    }
}

void MyX11Cursors::incrementUseCount(int id)
{
    MyX11CursorNode * node = lookupMap.value(id);
    Q_ASSERT(node);
    if (!node->refCount)
        removeNode(node);
    node->refCount++;
}

void MyX11Cursors::decrementUseCount(int id)
{
    MyX11CursorNode * node = lookupMap.value(id);
    Q_ASSERT(node);
    node->refCount--;
    if (!node->refCount)
        insertNode(node);
}

void MyX11Cursors::createNode(int id, Cursor c)
{
    MyX11CursorNode * node = new MyX11CursorNode(id, c);
    lookupMap.insert(id, node);
}

void MyX11Cursors::timeout()
{
    MyX11CursorNode * node;
    node = firstExpired;
    QDateTime now = QDateTime::currentDateTime();
    while (node && now.secsTo(node->expiration()) < 1) {
        Cursor c = node->cursor();
        int id = node->id();
        lookupMap.take(id);
        MyX11CursorNode * tmp = node;
        node = node->post();
        if (node)
            node->setAnte(0);
        delete tmp;
        XFreeCursor(display, c);
    }
    firstExpired = node;
    if (node == 0) {
        timer.stop();
        lastExpired = 0;
    }
    else {
        int interval = QDateTime::currentDateTime().secsTo(firstExpired->expiration()) * 1000;
        timer.setInterval(interval);
        timer.start();
    }
}

Cursor MyX11Cursors::cursor(int id)
{
    MyX11CursorNode * node = lookupMap.value(id);
    Q_ASSERT(node);
    return node->cursor();
}



/********************************************************************

MyDisplay class - perhaps better placed in qplatformintegration_testlite?

*********************************************************************/

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


#ifdef KeyPress
#undef KeyPress
#endif
#ifdef KeyRelease
#undef KeyRelease
#endif

bool MyDisplay::handleEvent(XEvent *xe)
{
    //qDebug() << "handleEvent" << xe->xany.type << xe->xany.window;
    int quit = false;
    QTestLiteWindow *xw = 0;
    foreach (QTestLiteWindow *w, windowList) {
        if (w->winId() == xe->xany.window) {
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

    default:
#ifdef MYX11_DEBUG
        qDebug() << hex << xe->xany.window << "Other X event" << xe->type;
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

#ifndef DONT_USE_MIT_SHM
    Status MIT_SHM_extension_supported = XShmQueryExtension (display);
    Q_ASSERT(MIT_SHM_extension_supported == True);
#endif
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

    cursors = new MyX11Cursors(display);
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


QImage MyDisplay::grabWindow(Window window, int x, int y, int w, int h)
{
    if (w == 0 || h ==0)
        return QImage();

    //WinId 0 means the desktop widget
    if (!window)
        window = rootWindow();

    XWindowAttributes window_attr;
    if (!XGetWindowAttributes(display, window, &window_attr))
        return QImage();

    if (w < 0)
        w = window_attr.width - x;
    if (h < 0)
        h = window_attr.height - y;

    // Ideally, we should also limit ourselves to the screen area, but the Qt docs say
    // that it's "unsafe" to go outside the screen, so we can ignore that problem.

    //We're definitely not optimizing for speed...
    XImage *xi = XGetImage(display, window, x, y, w, h, AllPlanes, ZPixmap);

    if (!xi)
        return QImage();

    //taking a copy to make sure we have ownership -- not fast
    QImage result = QImage( (uchar*) xi->data, xi->width, xi->height, xi->bytes_per_line, QImage::Format_RGB32 ).copy();

    XDestroyImage(xi);

    return result;
}







QT_END_NAMESPACE
#include "qtestlitewindow.moc"
