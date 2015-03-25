/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbbbuttoneventnotifier.h"

#include <QtGui/QApplication>
#include <qwindowsysteminterface_qpa.h>

#include <QtCore/QDebug>
#include <QtCore/QMetaEnum>
#include <QtCore/QSocketNotifier>
#include <QtCore/private/qcore_unix_p.h>

//#define QBBBUTTON_DEBUG

#ifdef QBBBUTTON_DEBUG
#define qButtonDebug qDebug
#else
#define qButtonDebug QT_NO_QDEBUG_MACRO
#endif

QT_BEGIN_NAMESPACE

static const char *ppsPath = "/pps/system/buttons/status";
static const int ppsBufferSize = 256;

QBBButtonEventNotifier::QBBButtonEventNotifier(QObject *parent)
    : QObject(parent),
      mFd(-1),
      mReadNotifier(0)
{
    // Set initial state of buttons to ButtonUp and
    // fetch the new button ids
    int enumeratorIndex = QBBButtonEventNotifier::staticMetaObject.indexOfEnumerator("ButtonId");
    QMetaEnum enumerator = QBBButtonEventNotifier::staticMetaObject.enumerator(enumeratorIndex);
    for (int buttonId = bid_minus; buttonId < ButtonCount; ++buttonId) {
        mButtonKeys.append(enumerator.valueToKey(buttonId));
        mState[buttonId] = ButtonUp;
    }
}

QBBButtonEventNotifier::~QBBButtonEventNotifier()
{
    close();
}

void QBBButtonEventNotifier::start()
{
    qButtonDebug() << Q_FUNC_INFO << "starting hardware button event processing";
    if (mFd != -1)
        return;

    // Open the pps interface
    errno = 0;
    mFd = qt_safe_open(ppsPath, O_RDONLY);
    if (mFd == -1) {
        qWarning("QQNX: failed to open buttons pps, errno=%d", errno);
        return;
    }

    mReadNotifier = new QSocketNotifier(mFd, QSocketNotifier::Read);
    QObject::connect(mReadNotifier, SIGNAL(activated(int)), this, SLOT(updateButtonStates()));

    qButtonDebug() << Q_FUNC_INFO << "successfully connected to Navigator. fd =" << mFd;
}

void QBBButtonEventNotifier::updateButtonStates()
{
    // Allocate buffer for pps data
    char buffer[ppsBufferSize];

    // Attempt to read pps data
    errno = 0;
    int bytes = qt_safe_read(mFd, buffer, ppsBufferSize - 1);
    qButtonDebug() << "Read" << bytes << "bytes of data";
    if (bytes == -1) {
        qWarning("QQNX: failed to read hardware buttons pps object, errno=%d", errno);
        return;
    }

    // We seem to get a spurious read notification after the real one. Ignore it
    if (bytes == 0)
        return;

    // Ensure data is null terminated
    buffer[bytes] = '\0';

    qButtonDebug() << Q_FUNC_INFO << "received PPS message:\n" << buffer;

    // Process received message
    QByteArray ppsData = QByteArray::fromRawData(buffer, bytes);
    QHash<QByteArray, QByteArray> fields;
    if (!parsePPS(ppsData, &fields))
        return;

    // Update our state and inject key events as needed
    for (int buttonId = bid_minus; buttonId < ButtonCount; ++buttonId) {
        // Extract the new button state
        QByteArray key = mButtonKeys.at(buttonId);
        ButtonState newState = (fields.value(key) == QByteArray("b_up") ? ButtonUp : ButtonDown);

        // If state has changed, update our state and inject a keypress event
        if (mState[buttonId] != newState) {
            qButtonDebug() << "Hardware button event: button =" << key << "state =" << fields.value(key);
            mState[buttonId] = newState;

            // Is it a key press or key release event?
            QEvent::Type type = (newState == ButtonDown) ? QEvent::KeyPress : QEvent::KeyRelease;

            Qt::Key key;
            switch (buttonId) {
                case bid_minus:
                    key = Qt::Key_VolumeDown;
                    break;

                case bid_playpause:
                    key = Qt::Key_Play;
                    break;

                case bid_plus:
                    key = Qt::Key_VolumeUp;
                    break;

                case bid_power:
                    key = Qt::Key_PowerDown;
                    break;

                default:
                    qButtonDebug() << "Unknown hardware button";
                    continue;
            }

            // No modifiers
            Qt::KeyboardModifiers modifier = Qt::NoModifier;

            // Post the event
            QWindowSystemInterface::handleKeyEvent(QApplication::activeWindow(), type, key, modifier);
        }
    }
}

void QBBButtonEventNotifier::close()
{
    delete mReadNotifier;
    mReadNotifier = 0;

    if (mFd != -1) {
        qt_safe_close(mFd);
        mFd = -1;
    }
}

bool QBBButtonEventNotifier::parsePPS(const QByteArray &ppsData, QHash<QByteArray, QByteArray> *messageFields) const
{
    // tokenize pps data into lines
    QList<QByteArray> lines = ppsData.split('\n');

    // validate pps object
    if (lines.size() == 0 || !lines.at(0).contains(QByteArray("@status"))) {
        qWarning("QQNX: unrecognized pps object, data=%s", ppsData.constData());
        return false;
    }

    // parse pps object attributes and extract values
    for (int i = 1; i < lines.size(); i++) {

        // tokenize current attribute
        const QByteArray &attr = lines.at(i);

        qButtonDebug() << Q_FUNC_INFO << "attr=" << attr;

        int doubleColon = attr.indexOf(QByteArray("::"));
        if (doubleColon == -1) {
            // abort - malformed attribute
            continue;
        }

        QByteArray key = attr.left(doubleColon);
        QByteArray value = attr.mid(doubleColon + 2);
        messageFields->insert(key, value);
    }
    return true;
}

QT_END_NAMESPACE
