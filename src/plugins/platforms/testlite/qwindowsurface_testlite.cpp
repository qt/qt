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

#include "qwindowsurface_testlite.h"
#include "qplatformintegration_testlite.h"

#include <QtCore/qdebug.h>
#include <QWindowSystemInterface>

#include "x11util.h"

QT_BEGIN_NAMESPACE

QTestLiteWindowSurface::QTestLiteWindowSurface
        (QTestLiteIntegration *platformIntegration,
         QTestLiteScreen *screen, QWidget *window)
    : QWindowSurface(window),
      mPlatformIntegration(platformIntegration),
      mScreen(screen),
      xw(0)
{


    xw = new MyWindow(platformIntegration->xd, 0,0,300,300);
    xw->windowSurface = this;

//    qDebug() << "QTestLiteWindowSurface::QTestLiteWindowSurface:" << xw->window;

    setWindowFlags(window->windowFlags()); //##### This should not be the plugin's responsibility
}

QTestLiteWindowSurface::~QTestLiteWindowSurface()
{
//    qDebug() << "~QTestLiteWindowSurface" << xw->window;
    delete xw;
}

QPaintDevice *QTestLiteWindowSurface::paintDevice()
{
    return xw->image();
}

void QTestLiteWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(region);
    Q_UNUSED(offset);

    //   qDebug() << "QTestLiteWindowSurface::flush:" << (long)this;

    xw->paintEvent();
}


void QTestLiteWindowSurface::setGeometry(const QRect &rect)
{
    QRect oldRect = geometry();
    if (rect == oldRect)
        return;

    QWindowSurface::setGeometry(rect);

    //if unchanged ###
//    xw->setSize(rect.width(), rect.height());
    xw->setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}

//### scroll logic copied from QRasterWindowSurface, we should make better API for this

void copied_qt_scrollRectInImage(QImage &img, const QRect &rect, const QPoint &offset)
{
    // make sure we don't detach
    uchar *mem = const_cast<uchar*>(const_cast<const QImage &>(img).bits());

    int lineskip = img.bytesPerLine();
    int depth = img.depth() >> 3;

    const QRect imageRect(0, 0, img.width(), img.height());
    const QRect r = rect & imageRect & imageRect.translated(-offset);
    const QPoint p = rect.topLeft() + offset;

    if (r.isEmpty())
        return;

    const uchar *src;
    uchar *dest;

    if (r.top() < p.y()) {
        src = mem + r.bottom() * lineskip + r.left() * depth;
        dest = mem + (p.y() + r.height() - 1) * lineskip + p.x() * depth;
        lineskip = -lineskip;
    } else {
        src = mem + r.top() * lineskip + r.left() * depth;
        dest = mem + p.y() * lineskip + p.x() * depth;
    }

    const int w = r.width();
    int h = r.height();
    const int bytes = w * depth;

    // overlapping segments?
    if (offset.y() == 0 && qAbs(offset.x()) < w) {
        do {
            ::memmove(dest, src, bytes);
            dest += lineskip;
            src += lineskip;
        } while (--h);
    } else {
        do {
            ::memcpy(dest, src, bytes);
            dest += lineskip;
            src += lineskip;
        } while (--h);
    }
}

bool QTestLiteWindowSurface::scroll(const QRegion &area, int dx, int dy)
{
    if (!xw->image() || xw->image()->isNull())
        return false;

    const QVector<QRect> rects = area.rects();
    for (int i = 0; i < rects.size(); ++i)
        copied_qt_scrollRectInImage(*xw->image(), rects.at(i), QPoint(dx, dy));

    return true;
}


void QTestLiteWindowSurface::beginPaint(const QRegion &region)
{
    Q_UNUSED(region);
    xw->resizeBuffer(geometry().size());
}

void QTestLiteWindowSurface::endPaint(const QRegion &region)
{
    Q_UNUSED(region);
    xw->painted = true; //there is content in the buffer
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

void QTestLiteWindowSurface::handleMouseEvent(QEvent::Type type, void *ev)
{
    static QPoint mousePoint;

    XButtonEvent *e = static_cast<XButtonEvent*>(ev);

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
                QWindowSystemInterface::handleWheelEvent(window(), e->time,
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

    QWindowSystemInterface::handleMouseEvent(window(), e->time, QPoint(e->x, e->y),
                                          QPoint(e->x_root, e->y_root),
                                          buttons);

    mousePoint = QPoint(e->x_root, e->y_root);
}

void QTestLiteWindowSurface::handleGeometryChange(int x, int y, int w, int h)
{
    QWindowSystemInterface::handleGeometryChange(window(), QRect(x,y,w,h));
}


void QTestLiteWindowSurface::handleCloseEvent()
{
    QWindowSystemInterface::handleCloseEvent(window());
}


void QTestLiteWindowSurface::handleEnterEvent()
{
    QWindowSystemInterface::handleEnterEvent(window());
}

void QTestLiteWindowSurface::handleLeaveEvent()
{
    QWindowSystemInterface::handleLeaveEvent(window());
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

void QTestLiteWindowSurface::handleKeyEvent(QEvent::Type type, void *ev)
{
    XKeyEvent *e = static_cast<XKeyEvent*>(ev);

    KeySym keySym;
    QByteArray chars;
    chars.resize(513);

    int count = XLookupString(e, chars.data(), chars.size(), &keySym, 0);

//      qDebug() << "QTLWS::handleKeyEvent" << count << hex << "XKeysym:" << keySym;
//      if (count)
//          qDebug() << hex << int(chars[0]) << "String:"             << chars;

    Qt::KeyboardModifiers modifiers = translateModifiers(e->state);

    int qtcode = lookupCode(keySym);
//    qDebug() << "lookup: " << hex << keySym << qtcode << "mod" << modifiers;

    //X11 specifies state *before*, Qt expects state *after* the event

    modifiers ^= modifierFromKeyCode(qtcode);

    if (qtcode) {
        QWindowSystemInterface::handleKeyEvent(window(), e->time, type, qtcode, modifiers);
    } else if (chars[0]) {
        int qtcode = chars.toUpper()[0]; //Not exactly right...
	if (modifiers & Qt::ControlModifier && qtcode < ' ')
	  qtcode = chars[0] + '@';
        QWindowSystemInterface::handleKeyEvent(window(), e->time, type, qtcode, modifiers, QString::fromLatin1(chars));
    } else {
        qWarning() << "unknown X keycode" << hex << e->keycode << keySym;
    }
}



Qt::WindowFlags QTestLiteWindowSurface::setWindowFlags(Qt::WindowFlags flags)
{
    Q_ASSERT(flags & Qt::Window);

    window_flags = flags;

    xw->setWindowFlags(flags);

    return window_flags;

}

Qt::WindowFlags QTestLiteWindowSurface::windowFlags() const
{
    return window_flags;
}

void QTestLiteWindowSurface::setVisible(bool visible)
{
    //qDebug() << "QTestLiteWindowSurface::setVisible" << visible << xw->window;
    xw->setVisible(visible);
}


WId QTestLiteWindowSurface::winId() const
{
    if (xw)
        return (WId) xw->window;
    else
        return WId(0);
}

void QTestLiteWindowSurface::raise()
{
    WId window = winId();
    XRaiseWindow(mPlatformIntegration->xd->display, window);
}

void QTestLiteWindowSurface::lower()
{
    WId window = winId();
    XLowerWindow(mPlatformIntegration->xd->display, window);
}

void QTestLiteWindowSurface::setWindowTitle(const QString &title)
{
    xw->setWindowTitle(title);
}

void QTestLiteWindowSurface::setCursor(QCursor *cursor)
{
    xw->setCursor(cursor);
}
QT_END_NAMESPACE
