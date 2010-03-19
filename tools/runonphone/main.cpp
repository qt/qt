/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include <QTextStream>
#include <QStringList>
#include <QScopedPointer>
#include <QTimer>
#include "symbianutils/trkutils.h"
#include "symbianutils/trkdevice.h"
#include "symbianutils/launcher.h"

#include "trksignalhandler.h"
#include "serenum.h"

void printUsage(QTextStream& outstream)
{
    outstream << "runtest [options] <program> [program arguments]" << endl
            << "-s, --sis <file>                     specify sis file to install" << endl
            << "-p, --portname <COMx>                specify COM port to use by device name" << endl
            << "-f, --portfriendlyname <substring>   specify COM port to use by friendly name" << endl
            << "-t, --timeout <milliseconds>         terminate test if timeout occurs" << endl
            << "-v, --verbose                        show debugging output" << endl
            << "-q, --quiet                          hide progress messages" << endl
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
    QStringList cmdLine;
    QStringList args = QCoreApplication::arguments();
    QTextStream outstream(stdout);
    QTextStream errstream(stderr);
    int loglevel=1;
    int timeout=0;
    for (int i=1;i<args.size();i++) {
        QString arg = args.at(i);
        if (arg.startsWith("-")) {
            if (args.size() < i+2) {
                errstream << "Command line missing argument parameters" << endl;
                return 1;
            }
            QString param = args.at(i+1);
            if(arg.compare("--portname", Qt::CaseSensitive) == 0
               || arg.compare("-p", Qt::CaseSensitive) == 0) {
                serialPortName = param;
                i++;
            }
            else if(arg.compare("--portfriendlyname", Qt::CaseSensitive) == 0
                    || arg.compare("-f", Qt::CaseSensitive) == 0) {
                serialPortFriendlyName = param;
                i++;
            }
            else if(arg.compare("--sis", Qt::CaseSensitive) == 0
                    || arg.compare("-s", Qt::CaseSensitive) == 0) {
                sisFile = param;
                i++;
            }
            else if(arg.compare("--timeout", Qt::CaseSensitive) == 0
                    || arg.compare("-t", Qt::CaseSensitive) == 0) {
                bool ok;
                timeout = param.toInt(&ok);
                if (!ok) {
                    errstream << "Timeout must be specified in milliseconds" << endl;
                    return 1;
                }
                i++;
            }
            else if(arg.compare("--verbose", Qt::CaseSensitive) == 0
                    || arg.compare("-v", Qt::CaseSensitive) == 0)
                loglevel=2;
            else if(arg.compare("--quiet", Qt::CaseSensitive) == 0
                    || arg.compare("-q", Qt::CaseSensitive) == 0)
                loglevel=0;
            else
                errstream << "unknown command line option " << arg << endl;
        } else {
            exeFile = arg;
            i++;
            for(;i<args.size();i++) {
                cmdLine.append(args.at(i));
            }
        }
    }

    if(exeFile.isEmpty()) {
        printUsage(outstream);
        return 1;
    }

    if (serialPortName.isEmpty()) {
        if (loglevel > 0)
            outstream << "Detecting serial ports" << endl;
        QList <SerialPortId> ports = enumerateSerialPorts();
        foreach(SerialPortId id, ports) {
            if (loglevel > 0)
                outstream << "Port Name: " << id.portName << ", "
                     << "Friendly Name:" << id.friendlyName << endl;
            if (serialPortName.isEmpty()) {
                if (!id.friendlyName.isEmpty()
                        && serialPortFriendlyName.isEmpty()
                        && (id.friendlyName.contains("symbian", Qt::CaseInsensitive)
                            || id.friendlyName.contains("s60", Qt::CaseInsensitive)
                            || id.friendlyName.contains("nokia", Qt::CaseInsensitive)))
                        serialPortName = id.portName;
                else if (!id.friendlyName.isEmpty()
                        && !serialPortFriendlyName.isEmpty()
                        && id.friendlyName.contains(serialPortFriendlyName))
                    serialPortName = id.portName;
            }
        }
        if (serialPortName.isEmpty()) {
            errstream << "No phone found, ensure USB cable is connected or specify manually with -p" << endl;
            return 1;
        }
    }

    QScopedPointer<trk::Launcher> launcher;

    if (sisFile.isEmpty()) {
        launcher.reset(new trk::Launcher(trk::Launcher::ActionCopyRun));
        launcher->setCopyFileName(exeFile, QString("c:\\sys\\bin\\") + exeFile);
        errstream << "System TRK required to copy EXE, use --sis if using Application TRK" << endl;
    } else {
        launcher.reset(new trk::Launcher(trk::Launcher::ActionCopyInstallRun));
        launcher->addStartupActions(trk::Launcher::ActionInstall);
        launcher->setCopyFileName(sisFile, "c:\\data\\testtemp.sis");
        launcher->setInstallFileName("c:\\data\\testtemp.sis");
    }
    if (loglevel > 0)
        outstream << "Connecting to target via " << serialPortName << endl;
    launcher->setTrkServerName(serialPortName);

    launcher->setFileName(QString("c:\\sys\\bin\\") + exeFile);
    launcher->setCommandLineArgs(cmdLine);

    if (loglevel > 1)
        launcher->setVerbose(1);

    TrkSignalHandler handler;
    handler.setLogLevel(loglevel);

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
    QObject::connect(launcher.data(), SIGNAL(processStopped(uint,uint,uint,QString)), &handler, SLOT(stopped(uint,uint,uint,QString)));
    QObject::connect(&handler, SIGNAL(resume(uint,uint)), launcher.data(), SLOT(resumeProcess(uint,uint)));
    QObject::connect(&handler, SIGNAL(terminate()), launcher.data(), SLOT(terminate()));
    QObject::connect(launcher.data(), SIGNAL(finished()), &handler, SLOT(finished()));

    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &handler, SLOT(timeout()));
    if (timeout > 0) {
        timer.start(timeout);
    }

    QString errorMessage;
    if (!launcher->startServer(&errorMessage)) {
        errstream << errorMessage << endl;
        return 1;
    }

    return a.exec();
}

