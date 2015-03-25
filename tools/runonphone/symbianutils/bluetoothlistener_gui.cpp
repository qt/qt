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

#include "bluetoothlistener_gui.h"
#include "bluetoothlistener.h"
#include "communicationstarter.h"

#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

namespace trk {

SYMBIANUTILS_EXPORT PromptStartCommunicationResult
    promptStartCommunication(BaseCommunicationStarter &starter,
                             const QString &msgBoxTitle,
                             const QString &msgBoxText,
                             QWidget *msgBoxParent,
                             QString *errorMessage)
{
    errorMessage->clear();
    // Initial connection attempt.
    switch (starter.start()) {
    case BaseCommunicationStarter::Started:
        break;
    case BaseCommunicationStarter::ConnectionSucceeded:
        return PromptStartCommunicationConnected;
    case BaseCommunicationStarter::StartError:
        *errorMessage = starter.errorString();
        return PromptStartCommunicationError;
    }
    // Run the starter with the event loop of a message box, have the box
    // closed by the signals of the starter.
    QMessageBox messageBox(QMessageBox::Information, msgBoxTitle, msgBoxText, QMessageBox::Cancel, msgBoxParent);
    QObject::connect(&starter, SIGNAL(connected()), &messageBox, SLOT(close()));
    QObject::connect(&starter, SIGNAL(timeout()), &messageBox, SLOT(close()));
    messageBox.exec();    
    // Only starter.state() is reliable here to obtain the state.
    switch (starter.state()) {
    case AbstractBluetoothStarter::Running:
        *errorMessage = QCoreApplication::translate("trk::promptStartCommunication", "Connection on %1 canceled.").arg(starter.device());
        return PromptStartCommunicationCanceled;
    case AbstractBluetoothStarter::TimedOut:
        *errorMessage = starter.errorString();
        return PromptStartCommunicationError;
    case AbstractBluetoothStarter::Connected:
        break;
    }
    return PromptStartCommunicationConnected;
}

SYMBIANUTILS_EXPORT PromptStartCommunicationResult
    promptStartSerial(BaseCommunicationStarter &starter,
                         QWidget *msgBoxParent,
                         QString *errorMessage)
{
    const QString title = QCoreApplication::translate("trk::promptStartCommunication", "Waiting for App TRK");
    const QString message = QCoreApplication::translate("trk::promptStartCommunication", "Waiting for App TRK to start on %1...").arg(starter.device());
    return promptStartCommunication(starter, title, message, msgBoxParent, errorMessage);
}

SYMBIANUTILS_EXPORT PromptStartCommunicationResult
    promptStartBluetooth(BaseCommunicationStarter &starter,
                         QWidget *msgBoxParent,
                         QString *errorMessage)
{
    const QString title = QCoreApplication::translate("trk::promptStartCommunication", "Waiting for Bluetooth Connection");
    const QString message = QCoreApplication::translate("trk::promptStartCommunication", "Connecting to %1...").arg(starter.device());
    return promptStartCommunication(starter, title, message, msgBoxParent, errorMessage);
}

} // namespace trk
