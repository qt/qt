/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qkbdusb_qws.h"

#ifndef QT_NO_QWS_KEYBOARD

#include "qscreen_qws.h"

#include "qwindowsystem_qws.h"
#include "qapplication.h"
#include "qsocketnotifier.h"
#include "qnamespace.h"
#include "qtimer.h"

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#include <linux/input.h>

#ifdef Q_OS_LINUX
#include <sys/kd.h>
#include <sys/vt.h>
#endif

QT_BEGIN_NAMESPACE

/* USB driver */


class QWSUsbKbPrivate : public QObject
{
    Q_OBJECT
public:
    QWSUsbKbPrivate(QWSPC101KeyboardHandler *, const QString &);
    ~QWSUsbKbPrivate();

private slots:
    void readKeyboardData();

private:
    QWSPC101KeyboardHandler *handler;
    int fd;
#ifdef QT_QWS_ZYLONITE
    bool shift;
#endif    
};

QWSUsbKeyboardHandler::QWSUsbKeyboardHandler(const QString &device)
    : QWSPC101KeyboardHandler(device)
{
    d = new QWSUsbKbPrivate(this, device);
}

QWSUsbKeyboardHandler::~QWSUsbKeyboardHandler()
{
    delete d;
}

QWSUsbKbPrivate::QWSUsbKbPrivate(QWSPC101KeyboardHandler *h, const QString &device) : handler(h)
{
#ifdef QT_QWS_ZYLONITE
    shift = FALSE;
#endif    
    fd = ::open(device.isEmpty()?"/dev/input/event1":device.toLocal8Bit(),O_RDONLY, 0);
    if (fd >= 0) {
        QSocketNotifier *notifier;
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, SIGNAL(activated(int)),this,
                 SLOT(readKeyboardData()));
    }
}

QWSUsbKbPrivate::~QWSUsbKbPrivate()
{
    ::close(fd);
}

void QWSUsbKbPrivate::readKeyboardData()
{
    input_event event;
    if (read(fd, &event, sizeof(input_event)) != sizeof(input_event))
        return;

    if (event.type != EV_KEY)
        return;

#ifdef QT_QWS_ZYLONITE
    qDebug("keypressed: code=%03d (%s)\n",event.code,((event.value)!=0) ? "Down":"Up");
    int modifiers=0; 
    int unicode=0xffff;
    int key_code=0;
    
    switch(event.code)
    {
      case 0xA2:
        key_code = ((!shift) ? Qt::Key_0      : Qt::Key_Plus      );
        unicode  = ((!shift) ? 0x30           : 0x2B              );
        break;
      case 0x70:
        key_code = ((!shift) ? Qt::Key_1      : Qt::Key_At        );
        unicode  = ((!shift) ? 0x31           : 0x40              );
        break;
      case 0x72:
        key_code = ((!shift) ? Qt::Key_2      : Qt::Key_Ampersand );
        unicode  = ((!shift) ? 0x32           : 0x26              );
        break;
      case 0x74:
        key_code = ((!shift) ? Qt::Key_3      : Qt::Key_At        );
        unicode  = ((!shift) ? 0x33           : 0x3F              );
        break;
      case 0x80:
        key_code = ((!shift) ? Qt::Key_4      : Qt::Key_Minus     );
        unicode  = ((!shift) ? 0x34           : 0x2D              );
        break;
      case 0x82:
        key_code = ((!shift) ? Qt::Key_5      : Qt::Key_Apostrophe);
        unicode  = ((!shift) ? 0x35           : 0x27              );
        break;
      case 0x84:
        key_code = ((!shift) ? Qt::Key_6      : Qt::Key_Slash     );
        unicode  = ((!shift) ? 0x36           : 0x5C              );
        break;
      case 0x90:
        key_code = ((!shift) ? Qt::Key_7      : Qt::Key_Colon     );
        unicode  = ((!shift) ? 0x37           : 0x3A              );
        break;
      case 0x92:
        key_code = ((!shift) ? Qt::Key_8      : Qt::Key_Semicolon );
        unicode  = ((!shift) ? 0x38           : 0x3B              );
        break;
      case 0x94:
        key_code = ((!shift) ? Qt::Key_9      : Qt::Key_QuoteDbl  );
        unicode  = ((!shift) ? 0x39           : 0x22              );
        break;
      case 0x0:
        key_code = Qt::Key_A;
        unicode  = ((!shift) ? 0x61           : 0x41              );
        break;
      case 0x10:
        key_code = Qt::Key_B;
        unicode  = ((!shift) ? 0x62           : 0x42              );
        break;
      case 0x20:
        key_code = Qt::Key_C;
        unicode  = ((!shift) ? 0x63           : 0x43              );
        break;
      case 0x30:
        key_code = Qt::Key_D;
        unicode  = ((!shift) ? 0x64           : 0x44              );
        break;
      case 0x40:
        key_code = Qt::Key_E;
        unicode  = ((!shift) ? 0x65           : 0x45              );
        break;
      case 0x50:
        key_code = Qt::Key_F;
        unicode  = ((!shift) ? 0x66           : 0x46              );
        break;
      case 0x01:
        key_code = Qt::Key_G;
        unicode  = ((!shift) ? 0x67           : 0x47              );
        break;
      case 0x11:
        key_code = Qt::Key_H;
        unicode  = ((!shift) ? 0x68           : 0x48              );
        break;
      case 0x21:
        key_code = Qt::Key_I;
        unicode  = ((!shift) ? 0x69           : 0x49              );
        break;
      case 0x31:
        key_code = Qt::Key_J;
        unicode  = ((!shift) ? 0x6A           : 0x4A              );
        break;
      case 0x41:
        key_code = Qt::Key_K;
        unicode  = ((!shift) ? 0x6B           : 0x4B              );
        break;
      case 0x51:
        key_code = Qt::Key_L;
        unicode  = ((!shift) ? 0x6C           : 0x4C              );
        break;
      case 0x02:
        key_code = Qt::Key_M;
        unicode  = ((!shift) ? 0x6D           : 0x4D              );
        break;
      case 0x12:
        key_code = Qt::Key_N;
        unicode  = ((!shift) ? 0x6E           : 0x4E              );
        break;
      case 0x22:
        key_code = Qt::Key_O;
        unicode  = ((!shift) ? 0x6F           : 0x4F              );
        break;
      case 0x32:
        key_code = Qt::Key_P;
        unicode  = ((!shift) ? 0x70           : 0x50              );
        break;
      case 0x42:
        key_code = Qt::Key_Q;
        unicode  = ((!shift) ? 0x71           : 0x51              );
        break;
      case 0x52:
        key_code = Qt::Key_R;
        unicode  = ((!shift) ? 0x72           : 0x52              );
        break;
      case 0x03:
        key_code = Qt::Key_S;
        unicode  = ((!shift) ? 0x73           : 0x53              );
        break;
      case 0x13:
        key_code = Qt::Key_T;
        unicode  = ((!shift) ? 0x74           : 0x54              );
        break;
      case 0x23:
        key_code = Qt::Key_U;
        unicode  = ((!shift) ? 0x75           : 0x55              );
        break;
      case 0x33:
        key_code = Qt::Key_V;
        unicode  = ((!shift) ? 0x76           : 0x56              );
        break;
      case 0x43:
        key_code = Qt::Key_W;
        unicode  = ((!shift) ? 0x77           : 0x57              );
        break;
      case 0x53:
        key_code = Qt::Key_X;
        unicode  = ((!shift) ? 0x78           : 0x58              );
        break;
      case 0x24:
        key_code = Qt::Key_Y;
        unicode  = ((!shift) ? 0x79           : 0x59              );
        break;
      case 0x34:
        key_code = Qt::Key_Z;
        unicode  = ((!shift) ? 0x7A           : 0x5A              );
        break;
      case 0xA4:
        key_code = ((!shift) ? Qt::Key_NumberSign : Qt::Key_Period);
        unicode  = ((!shift) ? 0x23           : 0x2E              );
        break;
      case 0xA0:
        key_code = ((!shift) ? Qt::Key_Asterisk   : Qt::Key_NumberSign );
        unicode  = ((!shift) ? 0x2A           : 0x2C              );
        break;
      case 0x25:
        key_code = Qt::Key_Space; 
        unicode  = 0x20;
        break;
      case 0x06:
        key_code = Qt::Key_Up;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x16:
        key_code = Qt::Key_Down;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x26:
        key_code = Qt::Key_Left;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x36:
        key_code = Qt::Key_Right;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x46:
        key_code = Qt::Key_Select;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x61:
        key_code = Qt::Key_No;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x60:
        key_code = Qt::Key_Call;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x55:
        key_code = Qt::Key_Hangup;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x62:
        key_code = Qt::Key_Context1;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x63:
        key_code = Qt::Key_No;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x05:
        key_code = Qt::Key_Home;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x15:
        key_code = Qt::Key_Shift;
        unicode  = 0xffff; modifiers |= Qt::ShiftModifier;
        if(event.value==0) break;
        if(shift) {
          shift = FALSE;
          qWarning("Caps Off!");
        } else {
          shift = TRUE;
          qWarning("Caps On!");
        }  
        break;
      case 0x1C:
        key_code = ((!shift) ? Qt::Key_Back : Qt::Key_Enter );
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x19:
        key_code = Qt::Key_Context2;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x1A:
        key_code = Qt::Key_Context3;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
      case 0x1B:
        key_code = Qt::Key_Context4;
        unicode  = 0xffff; modifiers |= Qt::KeypadModifier;
        break;
    }
    if(shift) modifiers |= Qt::ShiftModifier;
    handler->processKeyEvent(unicode, key_code, (Qt::KeyboardModifiers)modifiers, event.value!=0, false);
#else

    int key=event.code;
#ifndef QT_QWS_USE_KEYCODES
    // Handle SOME keys, otherwise it's useless.

    if(key==103) {
        handler->processKeyEvent(0, Qt::Key_Up, 0, event.value!=0, false);
    } else if(key==106) {
        handler->processKeyEvent(0, Qt::Key_Right, 0, event.value!=0, false );
    } else if(key==108) {
        handler->processKeyEvent(0, Qt::Key_Down, 0, event.value!=0, false);
    } else if(key==105) {
        handler->processKeyEvent(0, Qt::Key_Left, 0, event.value!=0, false);
    } else

#endif

    {
        if(event.value == 0) {
            key=key | 0x80;
        }
        handler->doKey(key);
    }
#endif
}

QT_END_NAMESPACE

#include "qkbdusb_qws.moc"

#endif // QT_NO_QWS_KEYBOARD
