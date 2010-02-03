/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include <QCoreApplication>
#include <QStringList>
#include <QLocalSocket>
#include <qnetworkconfigmanager.h>
#include <qnetworkconfiguration.h>
#include <qnetworksession.h>

#include <QDebug>

QTM_USE_NAMESPACE


#define NO_DISCOVERED_CONFIGURATIONS_ERROR 1
#define SESSION_OPEN_ERROR 2


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> discovered =
#if defined (Q_OS_SYMBIAN)
        // On Symbian, on the first query (before updateConfigurations() call 
        // the discovered-states are not correct, so defined-state will do.
        manager.allConfigurations(QNetworkConfiguration::Defined);
#else
        manager.allConfigurations(QNetworkConfiguration::Discovered);
#endif
    if (discovered.isEmpty()) {
        return NO_DISCOVERED_CONFIGURATIONS_ERROR;
    }

    // Cannot read/write to processes on WinCE or Symbian.
    // Easiest alternative is to use sockets for IPC.

    QLocalSocket oopSocket;

    oopSocket.connectToServer("tst_qnetworksession");
    oopSocket.waitForConnected(-1);

    qDebug() << "Lackey started";

    QNetworkSession *session = 0;
    do {
        if (session) {
            delete session;
            session = 0;
        }

        qDebug() << "Discovered configurations:" << discovered.count();

        if (discovered.isEmpty()) {
            qDebug() << "No more discovered configurations";
            break;
        }

        qDebug() << "Taking first configuration";

        QNetworkConfiguration config = discovered.takeFirst();

        if ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active) {
            qDebug() << config.name() << " is active, therefore skipping it (looking for configs in 'discovered' state).";
            continue;
        }

        qDebug() << "Creating session for" << config.name() << config.identifier();

        session = new QNetworkSession(config);

        QString output = QString("Starting session for %1\n").arg(config.identifier());
        oopSocket.write(output.toAscii());
        oopSocket.waitForBytesWritten();
        session->open();
        session->waitForOpened();
    } while (!(session && session->isOpen()));

    qDebug() << "lackey: loop done";

    if (!session) {
        qDebug() << "Could not start session";

        oopSocket.disconnectFromServer();
        oopSocket.waitForDisconnected(-1);

        return SESSION_OPEN_ERROR;
    }

    QString output = QString("Started session for %1\n").arg(session->configuration().identifier());
    oopSocket.write(output.toAscii());
    oopSocket.waitForBytesWritten();

    oopSocket.waitForReadyRead();
    oopSocket.readLine();

    session->stop();

    delete session;

    oopSocket.disconnectFromServer();
    oopSocket.waitForDisconnected(-1);

    return 0;
}
