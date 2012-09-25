/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#if !defined(QT_NO_QWS_KEYBOARD) && !defined(QT_NO_QWS_KBD_INTEGRITY)

#include "qkbdintegrity_qws.h"
#include <qwindowsystem_qws.h>
#include <qapplication.h>
#include <qtimer.h>
#include <INTEGRITY.h>
#include <QThread>
#include <device/hiddriver.h>

//===========================================================================

QT_BEGIN_NAMESPACE

//
// INTEGRITY keyboard
//

static const struct KeyMapEntryStruct {
    int key;
    QChar qchar;
} keyMap[] = {
    { 0, 0 },
    { Qt::Key_Escape, 0 },
    { Qt::Key_1, '1' },
    { Qt::Key_2, '2' },
    { Qt::Key_3, '3' },
    { Qt::Key_4, '4' },
    { Qt::Key_5, '5' },
    { Qt::Key_6, '6' },
    { Qt::Key_7, '7' },
    { Qt::Key_8, '8' },
    { Qt::Key_9, '9' },
    { Qt::Key_0, '0' },
    { Qt::Key_Minus, '-' },
    { Qt::Key_Equal, '=' },
    { Qt::Key_Backspace, 0 },
    { Qt::Key_Tab, '\t' },
    { Qt::Key_Q, 'q' },
    { Qt::Key_W, 'w' },
    { Qt::Key_E, 'e' },
    { Qt::Key_R, 'r' },
    { Qt::Key_T, 't' },
    { Qt::Key_Y, 'y' },
    { Qt::Key_U, 'u' },
    { Qt::Key_I, 'i' },
    { Qt::Key_O, 'o' },
    { Qt::Key_P, 'p' },
    { Qt::Key_BraceLeft, '{' },
    { Qt::Key_BraceRight, '}' },
    { Qt::Key_Enter, '\n' },
    { Qt::Key_Control, 0 },
    { Qt::Key_A, 'a' },
    { Qt::Key_S, 's' },
    { Qt::Key_D, 'd' },
    { Qt::Key_F, 'f' },
    { Qt::Key_G, 'g' },
    { Qt::Key_H, 'h' },
    { Qt::Key_J, 'j' },
    { Qt::Key_K, 'k' },
    { Qt::Key_L, 'l' },
    { Qt::Key_Semicolon, ';' },
    { Qt::Key_Apostrophe, '\'' },
    { Qt::Key_Dead_Grave, 0 },
    { Qt::Key_Shift, 0 },
    { Qt::Key_Backslash, '\\' },
    { Qt::Key_Z, 'z' },
    { Qt::Key_X, 'x' },
    { Qt::Key_C, 'c' },
    { Qt::Key_V, 'v' },
    { Qt::Key_B, 'b' },
    { Qt::Key_N, 'n' },
    { Qt::Key_M, 'm' },
    { Qt::Key_Comma, ',' },
    { Qt::Key_NumberSign, '.' },
    { Qt::Key_Slash, '/' },
    { Qt::Key_Shift, 0 },
    { Qt::Key_Asterisk, '*' },
    { Qt::Key_Alt, 0 },
};


class QIntKeyboardListenThread;

class QWSIntKbPrivate : public QObject
{
    Q_OBJECT
    friend class QIntKeyboardListenThread;
public:
    QWSIntKbPrivate(QWSKeyboardHandler *, const QString &device);
    ~QWSIntKbPrivate();
    void dataReady(uint32_t keycode, bool pressed) { emit kbdDataAvailable(keycode, pressed); }

Q_SIGNALS:
    void kbdDataAvailable(uint32_t keycode, bool pressed);

private Q_SLOTS:
    void readKeyboardData(uint32_t keycode, bool pressed);

private:
    QWSKeyboardHandler *handler;
    QIntKeyboardListenThread *kbdthread;
};
class QIntKeyboardListenThread : public QThread
{
protected:
    QWSIntKbPrivate *imp;
    bool loop;
    QList<HIDHandle> handleList;
    QList<Activity> actList;
    Semaphore loopsem;
public:
    QIntKeyboardListenThread(QWSIntKbPrivate *im) : QThread(), imp(im) {};
    ~QIntKeyboardListenThread() {};
    bool setup(QString driverName, uint32_t index);
    void run();
    void stoploop() { loop = false; ReleaseSemaphore(loopsem); };
};


QWSIntKeyboardHandler::QWSIntKeyboardHandler(const QString &device)
    : QWSKeyboardHandler(device)
{
    d = new QWSIntKbPrivate(this, device);
}

QWSIntKeyboardHandler::~QWSIntKeyboardHandler()
{
    delete d;
}

bool QIntKeyboardListenThread::setup(QString driverName, uint32_t index)
{
    int i;
    int devices;
    Error driverError, deviceError;
    HIDDriver *driver;
    HIDHandle handle;
    /* FIXME : take a list of driver names/indexes for setup */
    devices = 0;
    i = 0;
    do {
        driverError = gh_hid_get_driver(i, &driver);
        if (driverError == Success) {
            int j = 0;
            do {
                deviceError = gh_hid_init_device(driver, j, &handle);
                if (deviceError == Success) {
                    int32_t type;
                    /* only accept non-pointing devices */
                    deviceError = gh_hid_get_setting(handle, HID_SETTING_CAPABILITIES, &type);
                    if ((deviceError == Success) && !(type & HID_TYPE_AXIS)) {
                        handleList.append(handle);
                        devices++;
                    } else
                        gh_hid_close(handle);
                        j++;
                }
            } while (deviceError == Success);
            i++;
        }
    } while (driverError == Success);
    return (devices > 0);
}

void QIntKeyboardListenThread::run(void)
{
    Value id;
    HIDEvent event;
    Activity loopact;
    QPoint currentpos(0,0);
    Qt::MouseButtons currentbutton = Qt::NoButton;
    Qt::KeyboardModifiers keymod;

    /* first create all Activities for the main loop.
     * We couldn't do this in setup() because this Task is different */
    Activity act;
    int i = 0;
    foreach (HIDHandle h, handleList) {
        CheckSuccess(CreateActivity(CurrentTask(), 2, false, i, &act));
        actList.append(act);
        i++;
        CheckSuccess(gh_hid_async_wait_for_event(h, act));
    }

    /* setup a Semaphore used to watch for a request for exit */
    CheckSuccess(CreateSemaphore(0, &loopsem));
    CheckSuccess(CreateActivity(CurrentTask(), 2, false, 0, &loopact));
    CheckSuccess(AsynchronousReceive(loopact, (Object)loopsem, NULL));

    loop = true;
    do {
        Boolean nokeynotify = false;
        uint32_t num_events = 1;
        WaitForActivity(&id);
        if (loop) {
            while (gh_hid_get_event(handleList.at(id), &event, &num_events) == Success) {
                if (event.type == HID_TYPE_KEY) {
                switch (event.index) {
                    case HID_KEY_LEFTALT:
                    case HID_KEY_RIGHTALT:
                        if (event.value)
                            keymod |= Qt::AltModifier;
                        else
                            keymod &= ~Qt::AltModifier;
                        break;
                    case HID_KEY_LEFTSHIFT:
                    case HID_KEY_RIGHTSHIFT:
                        if (event.value)
                            keymod |= Qt::ShiftModifier;
                        else
                            keymod &= ~Qt::ShiftModifier;
                        break;
                    case HID_KEY_LEFTCTRL:
                    case HID_KEY_RIGHTCTRL:
                        if (event.value)
                            keymod |= Qt::ControlModifier;
                        else
                            keymod &= ~Qt::ControlModifier;
                        break;
                    default:
                        break;
                }
                QEvent::Type type;
                if (event.value)
                    type = QEvent::KeyPress;
                else
                    type = QEvent::KeyRelease;
                //QWindowSystemInterface::handleKeyEvent(0, type,
                //keyMap[event.index].key, keymod, keyMap[event.index].qchar);
                imp->dataReady(event.index, event.value);
                }
            }
            CheckSuccess(gh_hid_async_wait_for_event(handleList.at(id), actList.at(id)));
        }
    } while (loop);
    QThread::exit(0);
}

void QWSIntKbPrivate::readKeyboardData(uint32_t keycode, bool pressed)
{
    handler->processKeycode(keycode, pressed, false);
}

QWSIntKbPrivate::QWSIntKbPrivate(QWSKeyboardHandler *h, const QString &device) : handler(h)
{
    connect(this, SIGNAL(kbdDataAvailable(uint32_t, bool)), this, SLOT(readKeyboardData(uint32_t, bool)));
    this->handler = handler;
    qDebug("Opening INTEGRITY keyboard");
    kbdthread = new QIntKeyboardListenThread(this);
    kbdthread->start();
}

QWSIntKbPrivate::~QWSIntKbPrivate()
{
    kbdthread->stoploop();
    kbdthread->wait();
    delete kbdthread;
}

QT_END_NAMESPACE

#include "qkbdintegrity_qws.moc"

#endif // QT_NO_QWS_KEYBOARD || QT_NO_QWS_KBD_TTY
