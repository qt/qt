/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef BLUETOOTHLISTENER_H
#define BLUETOOTHLISTENER_H

#include "symbianutils_global.h"

#include <QtCore/QObject>
#include <QtCore/QProcess>

namespace trk {
struct BluetoothListenerPrivate;

/* BluetoothListener: Starts a helper process watching connections on a
 * Bluetooth device, Linux only:
 * The rfcomm command is used. It process can be started in the background
 * while connection attempts (TrkDevice::open()) are made in the foreground. */

class SYMBIANUTILS_EXPORT BluetoothListener : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(BluetoothListener)
public:
    // The Mode property must be set before calling start().
    enum Mode {
        Listen, /* Terminate after client closed (read: Trk app
                 * on the phone terminated or disconnected).*/
        Watch   // Keep running, watch for next connection from client
    };

    explicit BluetoothListener(QObject *parent = 0);
    virtual ~BluetoothListener();

    Mode mode() const;
    void setMode(Mode m);

    bool start(const QString &device, QString *errorMessage);

    // Print messages on the console.
    bool printConsoleMessages() const;
    void setPrintConsoleMessages(bool p);

signals:
    void terminated();
    void message(const QString &);

public slots:
    void emitMessage(const QString &m); // accessed by starter

private slots:
    void slotStdOutput();
    void slotStdError();
    void slotProcessFinished(int, QProcess::ExitStatus);
    void slotProcessError(QProcess::ProcessError error);

private:
    int terminateProcess();

    BluetoothListenerPrivate *d;
};

} // namespace trk

#endif // BLUETOOTHLISTENER_H
