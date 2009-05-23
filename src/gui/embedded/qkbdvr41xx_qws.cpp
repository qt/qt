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

#include "qkbdvr41xx_qws.h"

#if !defined(QT_NO_QWS_KEYBOARD) && !defined(QT_NO_QWS_KBD_VR41XX)

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#include <qsocketnotifier.h>

QT_BEGIN_NAMESPACE

class QWSVr41xxKbPrivate : public QObject
{
    Q_OBJECT
public:
    QWSVr41xxKbPrivate(QWSVr41xxKeyboardHandler *h, const QString&);
    virtual ~QWSVr41xxKbPrivate();

    bool isOpen() { return buttonFD > 0; }

private slots:
    void readKeyboardData();

private:
    QString terminalName;
    int buttonFD;
    int kbdIdx;
    int kbdBufferLen;
    unsigned char *kbdBuffer;
    QSocketNotifier *notifier;
    QWSVr41xxKeyboardHandler *handler;
};

QWSVr41xxKeyboardHandler::QWSVr41xxKeyboardHandler(const QString &device)
{
    d = new QWSVr41xxKbPrivate(this, device);
}

QWSVr41xxKeyboardHandler::~QWSVr41xxKeyboardHandler()
{
    delete d;
}

QWSVr41xxKbPrivate::QWSVr41xxKbPrivate(QWSVr41xxKeyboardHandler *h, const QString &device) : handler(h)
{
    terminalName = device;
    if (terminalName.isEmpty())
        terminalName = QLatin1String("/dev/buttons");
    buttonFD = -1;
    notifier = 0;

    buttonFD = open(terminalName.toLatin1().constData(), O_RDWR | O_NDELAY, 0);;
    if (buttonFD < 0) {
        qWarning("Cannot open %s\n", qPrintable(terminalName));
        return;
    }

    if (buttonFD >= 0) {
        notifier = new QSocketNotifier(buttonFD, QSocketNotifier::Read, this);
        connect(notifier, SIGNAL(activated(int)),this,
                SLOT(readKeyboardData()));
    }

    kbdBufferLen = 80;
    kbdBuffer = new unsigned char [kbdBufferLen];
    kbdIdx = 0;
}

QWSVr41xxKbPrivate::~QWSVr41xxKbPrivate()
{
    if (buttonFD > 0) {
        ::close(buttonFD);
        buttonFD = -1;
    }
    delete notifier;
    notifier = 0;
    delete [] kbdBuffer;
}

void QWSVr41xxKbPrivate::readKeyboardData()
{
    int n = 0;
    do {
        n  = read(buttonFD, kbdBuffer+kbdIdx, kbdBufferLen - kbdIdx);
        if (n > 0)
            kbdIdx += n;
    } while (n > 0);

    int idx = 0;
    while (kbdIdx - idx >= 2) {
        unsigned char *next = kbdBuffer + idx;
        unsigned short *code = (unsigned short *)next;
        int keycode = Qt::Key_unknown;
        switch ((*code) & 0x0fff) {
            case 0x7:
                keycode = Qt::Key_Up;
                break;
            case 0x9:
                keycode = Qt::Key_Right;
                break;
            case 0x8:
                keycode = Qt::Key_Down;
                break;
            case 0xa:
                keycode = Qt::Key_Left;
                break;
            case 0x3:
                keycode = Qt::Key_Up;
                break;
            case 0x4:
                keycode = Qt::Key_Down;
                break;
            case 0x1:
                keycode = Qt::Key_Return;
                break;
            case 0x2:
                keycode = Qt::Key_F4;
                break;
            default:
                qDebug("Unrecognised key sequence %d", *code);
        }
        if ((*code) & 0x8000)
            handler->processKeyEvent(0, keycode, 0, false, false);
        else
            handler->processKeyEvent(0, keycode, 0, true, false);
        idx += 2;
    }

    int surplus = kbdIdx - idx;
    for (int i = 0; i < surplus; i++)
        kbdBuffer[i] = kbdBuffer[idx+i];
    kbdIdx = surplus;
}

QT_END_NAMESPACE

#include "qkbdvr41xx_qws.moc"

#endif // QT_NO_QWS_KBD_VR41XX
