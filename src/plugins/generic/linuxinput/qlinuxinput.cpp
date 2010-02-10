/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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


#include "qlinuxinput.h"


#include <QSocketNotifier>
#include <QStringList>
#include <QPoint>
#include <QWindowSystemInterface>

#include <qkbd_qws.h>


#include <qplatformdefs.h>
#include <private/qcore_unix_p.h> // overrides QT_OPEN

#include <errno.h>
#include <termios.h>

#include <linux/kd.h>
#include <linux/input.h>

#include <qdebug.h>

QT_BEGIN_NAMESPACE

QLinuxInputMouseHandler::QLinuxInputMouseHandler(const QString &key,
                                                 const QString &specification)
    : m_notify(0), m_x(0), m_y(0), m_buttons(0)
{
    qDebug() << "QLinuxInputMouseHandler" << key << specification;


    setObjectName(QLatin1String("LinuxInputSubsystem Mouse Handler"));

    QString dev = QLatin1String("/dev/input/event0");
    if (specification.startsWith(QLatin1String("/dev/")))
        dev = specification;

    m_fd = QT_OPEN(dev.toLocal8Bit().constData(), O_RDONLY | O_NDELAY, 0);
    if (m_fd >= 0) {
        m_notify = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        connect(m_notify, SIGNAL(activated(int)), this, SLOT(readMouseData()));
    } else {
        qWarning("Cannot open mouse input device '%s': %s", qPrintable(dev), strerror(errno));
        return;
    }

}


QLinuxInputMouseHandler::~QLinuxInputMouseHandler()
{
    if (m_fd >= 0)
        QT_CLOSE(m_fd);
}

void QLinuxInputMouseHandler::readMouseData()
{
    struct ::input_event buffer[32];
    int n = 0;
    bool posChanged = false;

    forever {
        n = QT_READ(m_fd, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);

        if (n == 0) {
            qWarning("Got EOF from the input device.");
            return;
        } else if (n < 0 && (errno != EINTR && errno != EAGAIN)) {
            qWarning("Could not read from input device: %s", strerror(errno));
            return;
        } else if (n % sizeof(buffer[0]) == 0) {
            break;
        }
    }

    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        struct ::input_event *data = &buffer[i];

        bool unknown = false;
        if (data->type == EV_ABS) {
            if (data->code == ABS_X && m_x != data->value) {
                m_x = data->value;
                posChanged = true;
            } else if (data->code == ABS_Y && m_y != data->value) {
                m_y = data->value;
                posChanged = true;
            } else {
                unknown = true;
            }
        } else if (data->type == EV_REL) {
            if (data->code == REL_X) {
                m_x += data->value;
                posChanged = true;
            } else if (data->code == REL_Y) {
                m_y += data->value;
                posChanged = true;
            } else if (data->code == ABS_WHEEL) { // vertical scroll
                // data->value: 1 == up, -1 == down
                int delta = 120 * data->value;
                QWindowSystemInterface::handleWheelEvent(0, QPoint(m_x, m_y),
                                                      QPoint(m_x, m_y),
                                                      delta, Qt::Vertical);
            } else if (data->code == ABS_THROTTLE) { // horizontal scroll
                // data->value: 1 == right, -1 == left
                int delta = 120 * -data->value;
                QWindowSystemInterface::handleWheelEvent(0, QPoint(m_x, m_y),
                                                      QPoint(m_x, m_y),
                                                      delta, Qt::Horizontal);
            } else {
                unknown = true;
            }
        } else if (data->type == EV_KEY && data->code == BTN_TOUCH) {
            m_buttons = data->value ? Qt::LeftButton : Qt::NoButton;
        } else if (data->type == EV_KEY) {
            Qt::MouseButton button = Qt::NoButton;
            switch (data->code) {
            case BTN_LEFT: button = Qt::LeftButton; break;
            case BTN_MIDDLE: button = Qt::MidButton; break;
            case BTN_RIGHT: button = Qt::RightButton; break;
            }
            if (data->value)
                m_buttons |= button;
            else
                m_buttons &= ~button;

            QWindowSystemInterface::handleMouseEvent(0, QPoint(m_x, m_y),
                                                  QPoint(m_x, m_y), m_buttons);
        } else if (data->type == EV_SYN && data->code == SYN_REPORT) {
            if (!posChanged)
                continue;
            posChanged = false;
            QPoint pos(m_x, m_y);

            QWindowSystemInterface::handleMouseEvent(0, pos, pos, m_buttons);

            // pos = m_handler->transform(pos);
            //m_handler->limitToScreen(pos);
            //m_handler->mouseChanged(pos, m_buttons);

        } else if (data->type == EV_MSC && data->code == MSC_SCAN) {
            // kernel encountered an unmapped key - just ignore it
            continue;
        } else {
            unknown = true;
        }
        if (unknown) {
            qWarning("unknown mouse event type=%x, code=%x, value=%x", data->type, data->code, data->value);
        }
    }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Keyboard handler




class QWSLinuxInputKeyboardHandler : public QWSKeyboardHandler
{
public:
    QWSLinuxInputKeyboardHandler(const QString&);
    ~QWSLinuxInputKeyboardHandler();

    virtual bool filterInputEvent(quint16 &input_code, qint32 &input_value);

//private:
//    QWSLinuxInputKbPrivate *d;
};


QWSLinuxInputKeyboardHandler::QWSLinuxInputKeyboardHandler(const QString &device)
    : QWSKeyboardHandler(device)
{
}

QWSLinuxInputKeyboardHandler::~QWSLinuxInputKeyboardHandler()
{
}

bool QWSLinuxInputKeyboardHandler::filterInputEvent(quint16 &, qint32 &)
{
    return false;
}


QLinuxInputKeyboardHandler::QLinuxInputKeyboardHandler(const QString &key, const QString &specification)
    : m_handler(0), m_fd(-1), m_tty_fd(-1), m_orig_kbmode(K_XLATE)
{
    setObjectName(QLatin1String("LinuxInputSubsystem Keyboard Handler"));

    QString dev = QLatin1String("/dev/input/event1");
    int repeat_delay = -1;
    int repeat_rate = -1;

    QStringList args = specification.split(QLatin1Char(':'));
    foreach (const QString &arg, args) {
        if (arg.startsWith(QLatin1String("repeat-delay=")))
            repeat_delay = arg.mid(13).toInt();
        else if (arg.startsWith(QLatin1String("repeat-rate=")))
            repeat_rate = arg.mid(12).toInt();
        else if (arg.startsWith(QLatin1String("/dev/")))
            dev = arg;
    }

    m_handler = new QWSLinuxInputKeyboardHandler(dev); //This is a hack to avoid copying all the QWS code

    m_fd = QT_OPEN(dev.toLocal8Bit().constData(), O_RDWR, 0);
    if (m_fd >= 0) {
        if (repeat_delay > 0 && repeat_rate > 0) {
            int kbdrep[2] = { repeat_delay, repeat_rate };
            ::ioctl(m_fd, EVIOCSREP, kbdrep);
        }

        QSocketNotifier *notifier;
        notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        connect(notifier, SIGNAL(activated(int)), this, SLOT(readKeycode()));

        // play nice in case we are started from a shell (e.g. for debugging)
        m_tty_fd = isatty(0) ? 0 : -1;

        if (m_tty_fd >= 0) {
            // save tty config for restore.
            tcgetattr(m_tty_fd, &m_tty_attr);

            struct ::termios termdata;
            tcgetattr(m_tty_fd, &termdata);

            // record the original mode so we can restore it again in the destructor.
            ::ioctl(m_tty_fd, KDGKBMODE, &m_orig_kbmode);

            // setting this tranlation mode is even needed in INPUT mode to prevent
            // the shell from also interpreting codes, if the process has a tty
            // attached: e.g. Ctrl+C wouldn't copy, but kill the application.
            ::ioctl(m_tty_fd, KDSKBMODE, K_MEDIUMRAW);

            // set the tty layer to pass-through
            termdata.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
            termdata.c_oflag = 0;
            termdata.c_cflag = CREAD | CS8;
            termdata.c_lflag = 0;
            termdata.c_cc[VTIME]=0;
            termdata.c_cc[VMIN]=1;
            cfsetispeed(&termdata, 9600);
            cfsetospeed(&termdata, 9600);
            tcsetattr(m_tty_fd, TCSANOW, &termdata);
        }
    } else {
        qWarning("Cannot open keyboard input device '%s': %s", qPrintable(dev), strerror(errno));
        return;
    }
}

QLinuxInputKeyboardHandler::~QLinuxInputKeyboardHandler()
{
    if (m_tty_fd >= 0) {
        ::ioctl(m_tty_fd, KDSKBMODE, m_orig_kbmode);
        tcsetattr(m_tty_fd, TCSANOW, &m_tty_attr);
    }
    if (m_fd >= 0)
        QT_CLOSE(m_fd);
    delete m_handler;
}

void QLinuxInputKeyboardHandler::switchLed(int led, bool state)
{
    struct ::input_event led_ie;
    ::gettimeofday(&led_ie.time, 0);
    led_ie.type = EV_LED;
    led_ie.code = led;
    led_ie.value = state;

    QT_WRITE(m_fd, &led_ie, sizeof(led_ie));
}



void QLinuxInputKeyboardHandler::readKeycode()
{
    struct ::input_event buffer[32];
    int n = 0;

    forever {
        n = QT_READ(m_fd, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);

        if (n == 0) {
            qWarning("Got EOF from the input device.");
            return;
        } else if (n < 0 && (errno != EINTR && errno != EAGAIN)) {
            qWarning("Could not read from input device: %s", strerror(errno));
            return;
        } else if (n % sizeof(buffer[0]) == 0) {
            break;
        }
    }

    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        if (buffer[i].type != EV_KEY)
            continue;

        quint16 code = buffer[i].code;
        qint32 value = buffer[i].value;

        if (m_handler->filterInputEvent(code, value))
            continue;

        QWSKeyboardHandler::KeycodeAction ka;
        ka = m_handler->processKeycode(code, value != 0, value == 2);

        switch (ka) {
        case QWSKeyboardHandler::CapsLockOn:
        case QWSKeyboardHandler::CapsLockOff:
            switchLed(LED_CAPSL, ka == QWSKeyboardHandler::CapsLockOn);
            break;

        case QWSKeyboardHandler::NumLockOn:
        case QWSKeyboardHandler::NumLockOff:
            switchLed(LED_NUML, ka == QWSKeyboardHandler::NumLockOn);
            break;

        case QWSKeyboardHandler::ScrollLockOn:
        case QWSKeyboardHandler::ScrollLockOff:
            switchLed(LED_SCROLLL, ka == QWSKeyboardHandler::ScrollLockOn);
            break;

        default:
            // ignore console switching and reboot
            break;
        }
    }
}





QT_END_NAMESPACE
