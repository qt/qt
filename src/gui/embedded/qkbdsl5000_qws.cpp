/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qkbdsl5000_qws.h"

#ifndef QT_NO_QWS_KBD_SL5000

#include "qwindowsystem_qws.h"
#include "qwsutils_qws.h"
#include "qscreen_qws.h"

#include "qapplication.h"
#include "qnamespace.h"
#include "qtimer.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <asm/sharp_char.h>

#include <string.h>

QT_BEGIN_NAMESPACE

static const QWSKeyMap sl5000KeyMap[] = {
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 00
    {        Qt::Key_A,                'a'     , 'A'     , 'A'-64  }, // 01
    {        Qt::Key_B,                'b'     , 'B'     , 'B'-64  }, // 02
    {        Qt::Key_C,                'c'     , 'C'     , 'C'-64  }, // 03
    {        Qt::Key_D,                'd'     , 'D'     , 'D'-64  }, // 04
    {        Qt::Key_E,                'e'     , 'E'     , 'E'-64  }, // 05
    {        Qt::Key_F,                'f'     , 'F'     , 'F'-64  }, // 06
    {        Qt::Key_G,                'g'     , 'G'     , 'G'-64  }, // 07
    {        Qt::Key_H,                'h'     , 'H'     , 'H'-64  }, // 08
    {        Qt::Key_I,                'i'     , 'I'     , 'I'-64  }, // 09
    {        Qt::Key_J,                'j'     , 'J'     , 'J'-64  }, // 0a 10
    {        Qt::Key_K,                'k'     , 'K'     , 'K'-64  }, // 0b
    {        Qt::Key_L,                'l'     , 'L'     , 'L'-64  }, // 0c
    {        Qt::Key_M,                'm'     , 'M'     , 'M'-64  }, // 0d
    {        Qt::Key_N,                'n'     , 'N'     , 'N'-64  }, // 0e
    {        Qt::Key_O,                'o'     , 'O'     , 'O'-64  }, // 0f
    {        Qt::Key_P,                'p'     , 'P'     , 'P'-64  }, // 10
    {        Qt::Key_Q,                'q'     , 'Q'     , 'Q'-64  }, // 11
    {        Qt::Key_R,                'r'     , 'R'     , 'R'-64  }, // 12
    {        Qt::Key_S,                's'     , 'S'     , 'S'-64  }, // 13
    {        Qt::Key_T,                't'     , 'T'     , 'T'-64  }, // 14 20
    {        Qt::Key_U,                'u'     , 'U'     , 'U'-64  }, // 15
    {        Qt::Key_V,                'v'     , 'V'     , 'V'-64  }, // 16
    {        Qt::Key_W,                'w'     , 'W'     , 'W'-64  }, // 17
    {        Qt::Key_X,                'x'     , 'X'     , 'X'-64  }, // 18
    {        Qt::Key_Y,                'y'     , 'Y'     , 'Y'-64  }, // 19
    {        Qt::Key_Z,                'z'     , 'Z'     , 'Z'-64  }, // 1a
    {        Qt::Key_Shift,                0xffff  , 0xffff  , 0xffff  }, // 1b
    {        Qt::Key_Return,                13      , 13      , 0xffff  }, // 1c
    {        Qt::Key_F11,                0xffff  , 0xffff  , 0xffff  }, // 1d todo
    {        Qt::Key_F22,                0xffff  , 0xffff  , 0xffff  }, // 1e 30
    {        Qt::Key_Backspace,        8       , 8       , 0xffff  }, // 1f
    {        Qt::Key_F31,                0xffff  , 0xffff  , 0xffff  }, // 20
    {        Qt::Key_F35,                0xffff  , 0xffff  , 0xffff  }, // 21 light
    {        Qt::Key_Escape,                0xffff  , 0xffff  , 0xffff  }, // 22

    // Direction key code are for *UNROTATED* display.
    {        Qt::Key_Up,                0xffff  , 0xffff  , 0xffff  }, // 23
    {        Qt::Key_Right,                0xffff  , 0xffff  , 0xffff  }, // 24
    {        Qt::Key_Left,                0xffff  , 0xffff  , 0xffff  }, // 25
    {        Qt::Key_Down,                0xffff  , 0xffff  , 0xffff  }, // 26

    {        Qt::Key_F33,                0xffff  , 0xffff  , 0xffff  }, // 27 OK
    {        Qt::Key_F12,                0xffff  , 0xffff  , 0xffff  }, // 28 40 home
    {        Qt::Key_1,                '1'     , 'q'     , 'Q'-64  }, // 29
    {        Qt::Key_2,                '2'     , 'w'     , 'W'-64  }, // 2a
    {        Qt::Key_3,                '3'     , 'e'     , 'E'-64  }, // 2b
    {        Qt::Key_4,                '4'     , 'r'     , 'R'-64  }, // 2c
    {        Qt::Key_5,                '5'     , 't'     , 'T'-64  }, // 2d
    {        Qt::Key_6,                '6'     , 'y'     , 'Y'-64  }, // 2e
    {        Qt::Key_7,                '7'     , 'u'     , 'U'-64  }, // 2f
    {        Qt::Key_8,                '8'     , 'i'     , 'I'-64  }, // 30
    {        Qt::Key_9,                '9'     , 'o'     , 'O'-64  }, // 31
    {        Qt::Key_0,                '0'     , 'p'     , 'P'-64  }, // 32 50
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 33
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 34
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 35
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 36
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 37
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 38
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 39
    {        Qt::Key_Minus,                '-'     , 'b'     , 'B'-64  }, // 3a
    {        Qt::Key_Plus,                '+'     , 'n'     , 'N'-64  }, // 3b
    {        Qt::Key_CapsLock,        0xffff  , 0xffff  , 0xffff  }, // 3c 60
    {        Qt::Key_At,                '@'     , 's'     , 'S'-64  }, // 3d
    {        Qt::Key_Question,        '?'     , '?'     , 0xffff  }, // 3e
    {        Qt::Key_Comma,                ','     , ','     , 0xffff  }, // 3f
    {        Qt::Key_Period,                '.'     , '.'     , 0xffff  }, // 40
    {        Qt::Key_Tab,                9       , '\\'    , 0xffff  }, // 41
    {        Qt::Key_X,                0xffff         , 'x'            , 'X'-64  }, // 42
    {        Qt::Key_C,                0xffff         , 'c'     , 'C'-64  }, // 43
    {        Qt::Key_V,                0xffff         , 'v'     , 'V'-64  }, // 44
    {        Qt::Key_Slash,                '/'     , '/'     , 0xffff  }, // 45
    {        Qt::Key_Apostrophe,        '\''    , '\''    , 0xffff  }, // 46 70
    {        Qt::Key_Semicolon,        ';'     , ';'     , 0xffff  }, // 47
    {        Qt::Key_QuoteDbl,        '\"'    , '\"'    , 0xffff  }, // 48
    {        Qt::Key_Colon,                ':'     , ':'     , 0xffff  }, // 49
    {        Qt::Key_NumberSign,        '#'     , 'd'     , 'D'-64  }, // 4a
    {        Qt::Key_Dollar,                '$'     , 'f'     , 'F'-64  }, // 4b
    {        Qt::Key_Percent,        '%'     , 'g'     , 'G'-64  }, // 4c
    {        Qt::Key_Underscore,        '_'     , 'h'     , 'H'-64  }, // 4d
    {        Qt::Key_Ampersand,        '&'     , 'j'     , 'J'-64  }, // 4e
    {        Qt::Key_Asterisk,        '*'     , 'k'     , 'K'-64  }, // 4f
    {        Qt::Key_ParenLeft,        '('     , 'l'     , 'L'-64  }, // 50 80
    {        Qt::Key_Delete,                '['     , '['     , '['     }, // 51
    {        Qt::Key_Z,                0xffff         , 'z'     , 'Z'-64  }, // 52
    {        Qt::Key_Equal,                '='     , 'm'     , 'M'-64  }, // 53
    {        Qt::Key_ParenRight,        ')'     , ']'     , ']'     }, // 54
    {        Qt::Key_AsciiTilde,        '~'     , '^'     , '^'     }, // 55
    {        Qt::Key_Less,                '<'     , '{'     , '{'     }, // 56
    {        Qt::Key_Greater,        '>'     , '}'     , '}'     }, // 57
    {        Qt::Key_F9,                0xffff  , 0xffff  , 0xffff  }, // 58 datebook
    {        Qt::Key_F10,                0xffff  , 0xffff  , 0xffff  }, // 59 address
    {        Qt::Key_F13,                0xffff  , 0xffff  , 0xffff  }, // 5a 90 email
    {        Qt::Key_F30,                ' '      , ' '    , 0xffff  }, // 5b select
    {        Qt::Key_Space,                ' '     , '|'     , '`'     }, // 5c
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 5d
    {        Qt::Key_Exclam,                '!'     , 'a'     , 'A'-64  }, // 5e
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 5f
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 60
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 61
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 62
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 63
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 64
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 65
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 66
    {        Qt::Key_Meta,                0xffff  , 0xffff  , 0xffff  }, // 67
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 68
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 69
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 6a
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 6b
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 6c
    {        Qt::Key_F34,                0xffff  , 0xffff  , 0xffff  }, // 6d power
    {        Qt::Key_F13,                0xffff  , 0xffff  , 0xffff  }, // 6e mail long
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 6f
    {        Qt::Key_NumLock,        0xffff  , 0xffff  , 0xffff  }, // 70
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 71
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 72
    {         0x20ac,        0xffff  , 0x20ac , 0x20ac }, // 73 Euro sign
    {        Qt::Key_unknown,        0xffff  , 0xffff  , 0xffff  }, // 74
    {        Qt::Key_F32,                0xffff  , 0xffff  , 0xffff  }, // 75 Sync
    {        0,                        0xffff  , 0xffff  , 0xffff  }
};

static const int keyMSize = sizeof(sl5000KeyMap)/sizeof(QWSKeyMap)-1;

QWSSL5000KeyboardHandler::QWSSL5000KeyboardHandler(const QString &device)
    : QWSTtyKeyboardHandler(device)
{
    meta = false;
    fn = false;
    numLock = false;

    sharp_kbdctl_modifstat  st;
    int dev = ::open(device.isEmpty()?"/dev/sharp_kbdctl":device.toLocal8Bit().constData(), O_RDWR);
    if (dev >= 0) {
        memset(&st, 0, sizeof(st));
        st.which = 3;
        int ret = ioctl(dev, SHARP_KBDCTL_GETMODIFSTAT, (char*)&st);
        if(!ret)
            numLock = (bool)st.stat;
        ::close(dev);
    }
}

QWSSL5000KeyboardHandler::~QWSSL5000KeyboardHandler()
{
}

const QWSKeyMap *QWSSL5000KeyboardHandler::keyMap() const
{
    return sl5000KeyMap;
}

void QWSSL5000KeyboardHandler::doKey(uchar code)
{
    int keyCode = Qt::Key_unknown;
    bool release = false;

    if (code & 0x80) {
        release = true;
        code &= 0x7f;
    }

    if (fn && !meta && (code >= 0x42 && code <= 0x52)) {
        ushort unicode=0;
        int scan=0;
        if (code == 0x42) { unicode='X'-'@'; scan=Qt::Key_X; } // Cut
        else if (code == 0x43) { unicode='C'-'@'; scan=Qt::Key_C; } // Copy
        else if (code == 0x44) { unicode='V'-'@'; scan=Qt::Key_V; } // Paste
        else if (code == 0x52) { unicode='Z'-'@'; scan=Qt::Key_Z; } // Undo
        if (scan) {
            processKeyEvent(unicode, scan, Qt::ControlModifier, !release, false);
            return;
        }
    }

    if (code < keyMSize) {
        keyCode = keyMap()[code].key_code;
    }

    bool repeatable = true;

    if (release && (keyCode == Qt::Key_F34 || keyCode == Qt::Key_F35))
        return; // no release for power and light keys
    if (keyCode >= Qt::Key_F1 && keyCode <= Qt::Key_F35
            || keyCode == Qt::Key_Escape || keyCode == Qt::Key_Home
            || keyCode == Qt::Key_Shift || keyCode == Qt::Key_Meta)
        repeatable = false;

    if (qt_screen->isTransformed()
            && keyCode >= Qt::Key_Left && keyCode <= Qt::Key_Down)
    {
        keyCode = transformDirKey(keyCode);
    }

    // Ctrl-Alt-Delete exits qws
    if (ctrl && alt && keyCode == Qt::Key_Delete) {
        qApp->quit();
    }

    if (keyCode == Qt::Key_F22) { /* Fn key */
        fn = !release;
    } else if (keyCode == Qt::Key_NumLock) {
        if (release)
            numLock = !numLock;
    } else if (keyCode == Qt::AltModifier) {
        alt = !release;
    } else if (keyCode == Qt::ControlModifier) {
        ctrl = !release;
    } else if (keyCode == Qt::ShiftModifier) {
        shift = !release;
    } else if (keyCode == Qt::MetaModifier) {
        meta = !release;
    } else if (keyCode == Qt::Key_CapsLock && release) {
        caps = !caps;
    }
    if (keyCode != Qt::Key_unknown) {
        bool bAlt = alt;
        bool bCtrl = ctrl;
        bool bShift = shift;
        int unicode = 0;
        if (code < keyMSize) {
            bool bCaps = caps ^ shift;
            if (fn) {
                if (shift) {
                    bCaps = bShift = false;
                    bCtrl = true;
                }
                if (meta) {
                    bCaps = bShift = true;
                    bAlt = true;
                }
            } else if (meta) {
                bCaps = bShift = true;
            }
            if (code > 40 && caps) {
                // fn-keys should only react to shift, not caps
                bCaps = bShift = shift;
            }
            if (numLock) {
                if (keyCode != Qt::Key_Space && keyCode != Qt::Key_Tab)
                    bCaps = bShift = false;
            }
            if (keyCode == Qt::Key_Delete && (bAlt || bCtrl)) {
                keyCode = Qt::Key_BraceLeft;
                unicode = '[';
                bCaps = bShift = bAlt = bCtrl = false;
            } else if (keyCode == Qt::Key_F31 && bCtrl) {
                keyCode = Qt::Key_QuoteLeft;
                unicode = '`';
            } else if (bCtrl)
                unicode =  keyMap()[code].ctrl_unicode ?  keyMap()[code].ctrl_unicode : 0xffff;
            else if (bCaps)
                unicode =  keyMap()[code].shift_unicode ?  keyMap()[code].shift_unicode : 0xffff;
            else
                unicode =  keyMap()[code].unicode ?  keyMap()[code].unicode : 0xffff;
        }

        modifiers = 0;
        if (bAlt) modifiers |= Qt::AltModifier;
        if (bCtrl) modifiers |= Qt::ControlModifier;
        if (bShift) modifiers |= Qt::ShiftModifier;

        // looks wrong -- WWA
        bool repeat = false;
        if (prevuni == unicode && prevkey == keyCode && !release)
            repeat = true;

        processKeyEvent(unicode, keyCode, modifiers, !release, repeat);

        if (!release) {
            prevuni = unicode;
            prevkey = keyCode;
        } else {
            prevkey = prevuni = 0;
        }
    }

    if (repeatable && !release)
        beginAutoRepeat(prevuni, prevkey, modifiers);
    else
        endAutoRepeat();
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_KBD_SL5000
