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

#include "qmouseqnx_qws.h"

#include "qplatformdefs.h"
#include "qsocketnotifier.h"
#include "private/qcore_unix_p.h"

#include <sys/dcmd_input.h>
#include <errno.h>

QT_BEGIN_NAMESPACE

/*!
    \class QQnxMouseHandler
    \preliminary
    \ingroup qws
    \internal
    \since 4.6

    \brief The QQnxMouseHandler class implements a mouse driver
    for the QNX \c{devi-hid} input manager.

    To be able to compile this mouse handler, \l{Qt for Embedded Linux}
    must be configured with the \c -qt-mouse-qnx option, see the
    \l{Qt for Embedded Linux Pointer Handling}{Pointer Handling} documentation for details.

    In order to use this mouse handler, the \c{devi-hid} input manager
    must be set up and run with the resource manager interface (option \c{-r}).
    Also, Photon must not be running.

    Example invocation from command line: \c{/usr/photon/bin/devi-hid -Pr kbd mouse}
    Note that after running \c{devi-hid}, you will not be able to use the local
    shell anymore. It is suggested to run the command in a shell scrip, that launches
    a Qt application after invocation of \c{devi-hid}.

    To make \l{Qt for Embedded Linux} explicitly choose the qnx mouse
    handler, set the QWS_MOUSE_PROTO environment variable to \c{qnx}. By default,
    the first mouse device (\c{/dev/devi/mouse0}) is used. To override, pass a device
    name as the first and only parameter, for example
    \c{QWS_MOUSE_PROTO=qnx:/dev/devi/mouse1; export QWS_MOUSE_PROTO}.

    \sa {Qt for Embedded Linux Pointer Handling}{Pointer Handling}, {Qt for Embedded Linux}
*/

/*!
    Constructs a mouse handler for the specified \a device, defaulting to \c{/dev/devi/mouse0}.
    The \a driver parameter must be \c{"qnx"}.

    Note that you should never instanciate this class, instead let QMouseDriverFactory
    handle the mouse handlers.

    \sa QMouseDriverFactory
 */
QQnxMouseHandler::QQnxMouseHandler(const QString & driver, const QString &device)
    : QObject(), QWSMouseHandler(driver, device), mouseButtons(Qt::NoButton)
{
    // open the mouse device with O_NONBLOCK so reading won't block when there's no data
    mouseFD = QT_OPEN(device.isEmpty() ? "/dev/devi/mouse0" : device.toLatin1().constData(),
                      QT_OPEN_RDONLY | O_NONBLOCK);
    if (mouseFD == -1) {
        qErrnoWarning(errno, "QQnxMouseHandler: Unable to open mouse device");
    } else {
        struct _pointer_info data;
        if (devctl(mouseFD, _POINTERGETINFO, &data, sizeof(data), NULL) == EOK)
            absolutePositioning = (data.flags & _POINTER_FLAG_ABSOLUTE);
        else
            absolutePositioning = !device.isEmpty() && device.contains(QLatin1String("touch"));

        // register a socket notifier on the file descriptor so we'll wake up whenever
        // there's a mouse move waiting for us.
        mouseNotifier = new QSocketNotifier(mouseFD, QSocketNotifier::Read, this);
        connect(mouseNotifier, SIGNAL(activated(int)), SLOT(socketActivated()));

        qDebug("QQnxMouseHandler: connected.");
    }
}

/*!
    Destroys this mouse handler and closes the connection to the mouse device.
 */
QQnxMouseHandler::~QQnxMouseHandler()
{
    if (mouseFD != -1)
        QT_CLOSE(mouseFD);
}

/*! \reimp */
void QQnxMouseHandler::resume()
{
    if (mouseNotifier)
        mouseNotifier->setEnabled(true);
}

/*! \reimp */
void QQnxMouseHandler::suspend()
{
    if (mouseNotifier)
        mouseNotifier->setEnabled(false);
}

/*! \internal

  This function is called whenever there is activity on the mouse device.
  By default, it reads up to 10 mouse move packets and calls mouseChanged()
  for each of them.
*/
void QQnxMouseHandler::socketActivated()
{
    QPoint queuedPos = mousePos;

    // _mouse_packet is a QNX structure. devi-hid is nice enough to translate
    // the raw byte data from mouse devices into generic format for us.
    struct _mouse_packet buffer[32];
    int n = 0;

    forever {
        int bytesRead = QT_READ(mouseFD, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);
        if (bytesRead == -1) {
            // EAGAIN means that there are no more mouse events to read
            if (errno != EAGAIN)
                qErrnoWarning(errno, "QQnxMouseHandler: Could not read from input device");
            break;
        }

        n += bytesRead;
        if (n % sizeof(buffer[0]) == 0)
            break;
    }
    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        const struct _mouse_packet &packet = buffer[i];

        // translate the coordinates from the QNX data structure to the Qt coordinates
        if (absolutePositioning) {
            queuedPos = QPoint(packet.dx, packet.dy);
        } else {
            // note the swapped y axis
            queuedPos += QPoint(packet.dx, -packet.dy);

            // QNX only tells us relative mouse movements, not absolute ones, so
            // limit the cursor position manually to the screen
            limitToScreen(queuedPos);
        }

        // translate the QNX mouse button bitmask to Qt buttons
        int buttons = Qt::NoButton;
        if (packet.hdr.buttons & _POINTER_BUTTON_LEFT)
            buttons |= Qt::LeftButton;
        if (packet.hdr.buttons & _POINTER_BUTTON_MIDDLE)
            buttons |= Qt::MidButton;
        if (packet.hdr.buttons & _POINTER_BUTTON_RIGHT)
            buttons |= Qt::RightButton;

        if (buttons != mouseButtons) {
            // send the MouseEvent to avoid missing any clicks
            mouseChanged(queuedPos, buttons, 0);
            // mousePos updated by the mouseChanged()
            queuedPos = mousePos;
            mouseButtons = buttons;
        }
    }

    if (queuedPos != mousePos)
        mouseChanged(queuedPos, mouseButtons, 0);
}

QT_END_NAMESPACE
