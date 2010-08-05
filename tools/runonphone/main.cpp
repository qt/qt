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
#include <QFileInfo>
#include "symbianutils/trkutils.h"
#include "symbianutils/trkdevice.h"
#include "symbianutils/launcher.h"

#include "trksignalhandler.h"
#include "serenum.h"
#include "ossignalconverter.h"

void printUsage(QTextStream& outstream, QString exeName)
{
    outstream << exeName << " [options] [program] [program arguments]" << endl
            << "-s, --sis <file>                         specify sis file to install" << endl
            << "-p, --portname <COMx>                    specify COM port to use by device name" << endl
            << "-f, --portfriendlyname <substring>       specify COM port to use by friendly name" << endl
            << "-t, --timeout <milliseconds>             terminate test if timeout occurs" << endl
            << "-v, --verbose                            show debugging output" << endl
            << "-q, --quiet                              hide progress messages" << endl
            << "-d, --download <remote file> <local file> copy file from phone to PC after running test" << endl
            << "--nocrashlog                             Don't capture call stack if test crashes" << endl
            << "--crashlogpath <dir>                     Path to save crash logs (default=working dir)" << endl
            << endl
            << "USB COM ports can usually be autodetected, use -p or -f to force a specific port." << endl
            << "If using System TRK, it is possible to copy the program directly to sys/bin on the phone." << endl
            << "-s can be used with both System and Application TRK to install the program" << endl;
}

#define CHECK_PARAMETER_EXISTS if(!it.hasNext()) { printUsage(outstream, args[0]); return 1; }
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
    QString downloadRemoteFile;
    QString downloadLocalFile;
    int loglevel=1;
    int timeout=0;
    bool crashlog = true;
    QString crashlogpath;
    QListIterator<QString> it(args);
    it.next(); //skip name of program
    while (it.hasNext()) {
        QString arg = it.next();

        if (arg.startsWith("-")) {
            if (arg == "--portname" || arg == "-p") {
                CHECK_PARAMETER_EXISTS
                serialPortName = it.next();
            }
            else if (arg == "--portfriendlyname" || arg == "-f") {
                CHECK_PARAMETER_EXISTS
                serialPortFriendlyName = it.next();
            }
            else if (arg == "--sis" || arg == "-s") {
                CHECK_PARAMETER_EXISTS
                sisFile = it.next();
                if (!QFileInfo(sisFile).exists()) {
                    errstream << "Sis file (" << sisFile << ") doesn't exist" << endl;
                    return 1;
                }
            }
            else if (arg == "--download" || arg == "-d") {
                CHECK_PARAMETER_EXISTS
                downloadRemoteFile = it.next();
                CHECK_PARAMETER_EXISTS
                downloadLocalFile = it.next();
            }
            else if (arg == "--timeout" || arg == "-t") {
                CHECK_PARAMETER_EXISTS
                bool ok;
                timeout = it.next().toInt(&ok);
                if (!ok) {
                    errstream << "Timeout must be specified in milliseconds" << endl;
                    return 1;
                }
            }
            else if (arg == "--verbose" || arg == "-v")
                loglevel=2;
            else if (arg == "--quiet" || arg == "-q")
                loglevel=0;
            else if (arg == "--nocrashlog")
                crashlog = false;
            else if (arg == "--crashlogpath") {
                CHECK_PARAMETER_EXISTS
                crashlogpath = it.next();
            }
            else
                errstream << "unknown command line option " << arg << endl;
        } else {
            exeFile = arg;
            while(it.hasNext()) {
                cmdLine.append(it.next());
            }
        }
    }

    if (exeFile.isEmpty() && sisFile.isEmpty() && 
        (downloadLocalFile.isEmpty() || downloadRemoteFile.isEmpty())) {
        printUsage(outstream, args[0]);
        return 1;
    }

    if (serialPortName.isEmpty()) {
        if (loglevel > 0)
            outstream << "Detecting serial ports" << endl;
        foreach (const SerialPortId &id, enumerateSerialPorts(loglevel)) {
            if (loglevel > 0)
                outstream << "Port Name: " << id.portName << ", "
                     << "Friendly Name:" << id.friendlyName << endl;
            if (!id.friendlyName.isEmpty()
                    && serialPortFriendlyName.isEmpty()
                    && (id.friendlyName.contains("symbian", Qt::CaseInsensitive)
                        || id.friendlyName.contains("s60", Qt::CaseInsensitive)
                        || id.friendlyName.contains("nokia", Qt::CaseInsensitive))) {
                serialPortName = id.portName;
                break;
            } else if (!id.friendlyName.isEmpty()
                    && !serialPortFriendlyName.isEmpty()
                    && id.friendlyName.contains(serialPortFriendlyName)) {
                serialPortName = id.portName;
                break;
            }
        }
        if (serialPortName.isEmpty()) {
            errstream << "No phone found, ensure USB cable is connected or specify manually with -p" << endl;
            return 1;
        }
    }

    QScopedPointer<trk::Launcher> launcher;
    launcher.reset(new trk::Launcher(trk::Launcher::ActionPingOnly));
    QFileInfo info(exeFile);
    if (!sisFile.isEmpty()) {
        launcher->addStartupActions(trk::Launcher::ActionCopyInstall);
        launcher->setCopyFileName(sisFile, "c:\\data\\testtemp.sis");
        launcher->setInstallFileName("c:\\data\\testtemp.sis");
    }
    else if (info.exists()) {
        launcher->addStartupActions(trk::Launcher::ActionCopy);
        launcher->setCopyFileName(exeFile, QString("c:\\sys\\bin\\") + info.fileName());
    }
    if (!exeFile.isEmpty()) {
        launcher->addStartupActions(trk::Launcher::ActionRun);
        launcher->setFileName(QString("c:\\sys\\bin\\") + info.fileName());
        launcher->setCommandLineArgs(cmdLine);
    }
    if (!downloadRemoteFile.isEmpty() && !downloadLocalFile.isEmpty()) {
        launcher->addStartupActions(trk::Launcher::ActionDownload);
        launcher->setDownloadFileName(downloadRemoteFile, downloadLocalFile);
    }
    if (loglevel > 0)
        outstream << "Connecting to target via " << serialPortName << endl;
    launcher->setTrkServerName(serialPortName);

    if (loglevel > 1)
        launcher->setVerbose(1);

    TrkSignalHandler handler;
    handler.setLogLevel(loglevel);
    handler.setCrashLogging(crashlog);
    handler.setCrashLogPath(crashlogpath);

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
    QObject::connect(launcher.data(), SIGNAL(libraryLoaded(trk::Library)), &handler, SLOT(libraryLoaded(trk::Library)));
    QObject::connect(launcher.data(), SIGNAL(libraryUnloaded(trk::Library)), &handler, SLOT(libraryUnloaded(trk::Library)));
    QObject::connect(launcher.data(), SIGNAL(registersAndCallStackReadComplete(const QList<uint> &,const QByteArray &)), &handler, SLOT(registersAndCallStackReadComplete(const QList<uint> &,const QByteArray &)));
    QObject::connect(&handler, SIGNAL(resume(uint,uint)), launcher.data(), SLOT(resumeProcess(uint,uint)));
    QObject::connect(&handler, SIGNAL(terminate()), launcher.data(), SLOT(terminate()));
    QObject::connect(&handler, SIGNAL(getRegistersAndCallStack(uint,uint)), launcher.data(), SLOT(getRegistersAndCallStack(uint,uint)));
    QObject::connect(launcher.data(), SIGNAL(finished()), &handler, SLOT(finished()));

    QObject::connect(OsSignalConverter::instance(), SIGNAL(terminate()), launcher.data(), SLOT(terminate()), Qt::QueuedConnection);

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

