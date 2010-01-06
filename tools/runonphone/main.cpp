/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QScopedPointer>
#include "trkutils.h"
#include "trkdevice.h"
#include "launcher.h"

#include "trksignalhandler.h"
#include "serenum.h"

void printUsage()
{
    qDebug() << "runtest [options] <program> [program arguments]" << endl
            << "-s, --sis <file>                     specify sis file to install" << endl
            << "-p, --portname <COMx>                specify COM port to use by device name" << endl
            << "-f, --portfriendlyname <substring>   specify COM port to use by friendly name" << endl
            << endl
            << "USB COM ports can usually be autodetected" << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString serialPortName;
    QString serialPortFriendlyName;
    QString sisFile;
    QString exeFile;
    QString cmdLine;
    QStringList args = QCoreApplication::arguments();
    for (int i=1;i<args.size();i++) {
        QString arg = args.at(i);
        if (arg.startsWith("-")) {
            if (args.size() < i+2) {
                qWarning("Command line missing argument parameters");
                return 1;
            }
            i++;
            QString param = args.at(i);
            if(arg.compare("--portname", Qt::CaseSensitive) == 0
               || arg.compare("-p", Qt::CaseSensitive) == 0)
                serialPortName = param;
            else if(arg.compare("--portfriendlyname", Qt::CaseSensitive) == 0
                    || arg.compare("-f", Qt::CaseSensitive) == 0)
                serialPortFriendlyName = param;
            else if(arg.compare("--sis", Qt::CaseSensitive) == 0
                    || arg.compare("-s", Qt::CaseSensitive) == 0)
                sisFile = param;
            else
                qWarning() << "unknown command line option " << arg;
        } else {
            exeFile = arg;
            i++;
            for(;i<args.size();i++) {
                cmdLine.append(args.at(i));
                if(i + 1 < args.size()) cmdLine.append(' ');
            }
        }
    }

    if(exeFile.isEmpty()) {
        printUsage();
        return 1;
    }

    if(serialPortName.isEmpty()) {
        qDebug() << "Detecting serial ports" << endl;
        QList <SerialPortId> ports = enumerateSerialPorts();
        foreach(SerialPortId id, ports) {
            qDebug() << "Port Name: " << id.portName << ", "
                     << "Friendly Name:" << id.friendlyName << endl;
            if(serialPortName.isEmpty()) {
                if(id.friendlyName.isEmpty() &&
                    (id.friendlyName.contains("symbian", Qt::CaseInsensitive) ||
                       id.friendlyName.contains("s60", Qt::CaseInsensitive) ||
                       id.friendlyName.contains("nokia", Qt::CaseInsensitive)))
                        serialPortName = id.portName;
                else if (!id.friendlyName.isEmpty() &&
                         id.friendlyName.contains(serialPortFriendlyName))
                        serialPortName = id.portName;
            }
        }
    }

    QScopedPointer<trk::Launcher> launcher;

    if(sisFile.isEmpty()) {
        launcher.reset(new trk::Launcher(trk::Launcher::ActionCopyRun));
        launcher->setCopyFileName(exeFile, QString("c:\\sys\\bin\\") + exeFile);
        qDebug() << "System TRK required to copy EXE, use --sis if using Application TRK" << endl;
    } else {
        launcher.reset(new trk::Launcher(trk::Launcher::ActionCopyInstallRun));
        launcher->addStartupActions(trk::Launcher::ActionInstall);
        launcher->setCopyFileName(sisFile, "c:\\data\\testtemp.sis");
        launcher->setInstallFileName("c:\\data\\testtemp.sis");
    }
    qDebug() << "Connecting to target via " << serialPortName << endl;
    launcher->setTrkServerName(QString("\\\\.\\") + serialPortName);

    launcher->setFileName(QString("c:\\sys\\bin\\") + exeFile);
    launcher->setCommandLineArgs(cmdLine);

    TrkSignalHandler handler;

    QObject::connect(launcher.data(), SIGNAL(copyingStarted()), &handler, SLOT(copyingStarted()));
    QObject::connect(launcher.data(), SIGNAL(canNotConnect(const QString &)), &handler, SLOT(canNotConnect(const QString &)));
    QObject::connect(launcher.data(), SIGNAL(canNotCreateFile(const QString &, const QString &)), &handler, SLOT(canNotCreateFile(const QString &, const QString &)));
    QObject::connect(launcher.data(), SIGNAL(canNotWriteFile(const QString &, const QString &)), &handler, SLOT(canNotWriteFile(const QString &, const QString &)));
    QObject::connect(launcher.data(), SIGNAL(canNotCloseFile(const QString &, const QString &)), &handler, SLOT(canNotCloseFile(const QString &, const QString &)));
    QObject::connect(launcher.data(), SIGNAL(installingStarted()), &handler, SLOT(installingStarted()));
    QObject::connect(launcher.data(), SIGNAL(canNotInstall(const QString &, const QString &)), &handler, SLOT(canNotInstall(const QString &, const QString &)));
    QObject::connect(launcher.data(), SIGNAL(installingFinished()), &handler, SLOT(installingFinished()));
    QObject::connect(launcher.data(), SIGNAL(startingApplication()), &handler, SLOT(startingApplication()));
    QObject::connect(launcher.data(), SIGNAL(applicationRunning(uint)), &handler, SLOT(applicationRunning(uint)));
    QObject::connect(launcher.data(), SIGNAL(canNotRun(const QString &)), &handler, SLOT(canNotRun(const QString &)));
    QObject::connect(launcher.data(), SIGNAL(applicationOutputReceived(const QString &)), &handler, SLOT(applicationOutputReceived(const QString &)));
    QObject::connect(launcher.data(), SIGNAL(copyProgress(int)), &handler, SLOT(copyProgress(int)));
    QObject::connect(launcher.data(), SIGNAL(stateChanged(int)), &handler, SLOT(stateChanged(int)));
    QObject::connect(launcher.data(), SIGNAL(finished()), &handler, SLOT(finished()));

    QString errorMessage;
    if(!launcher->startServer(&errorMessage)) {
        qWarning() << errorMessage;
        return 1;
    }

    return a.exec();
}

