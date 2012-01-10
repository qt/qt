/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "symbianutils/codadevice.h"
#include "symbianutils/trkutils.h"
#include "symbianutils/trkdevice.h"
#include "symbianutils/launcher.h"
#include "symbianutils/symbiandevicemanager.h"

#include "codasignalhandler.h"
#include "trksignalhandler.h"
#include "serenum.h"
#include "ossignalconverter.h"

void printUsage(QTextStream& outstream, QString exeName)
{
    outstream << exeName << " [options] [program] [program arguments]" << endl
            << "-s, --sis <local file>                   specify sis file to install" << endl
            << "-p, --portname <COMx>                    specify COM port to use by device name" << endl
            << "-f, --portfriendlyname <substring>       specify COM port to use by friendly name" << endl
            << "-t, --timeout <milliseconds>             terminate test if timeout occurs" << endl
            << "-v, --verbose                            show debugging output" << endl
            << "-q, --quiet                              hide progress messages" << endl
            << "-u, --upload <local file> <remote file>  upload file to phone" << endl
            << "-d, --download <remote file> <local file> copy file from phone to PC after running test" << endl
            << "-T, --tempfile <remote file>             specify temporary sis file name" << endl
            << "--nocrashlog                             Don't capture call stack if test crashes" << endl
            << "--crashlogpath <dir>                     Path to save crash logs (default=working dir)" << endl
            << "--coda                                   Use CODA instead of detecting the debug agent" << endl
            << "--trk                                    Use TRK instead of detecting the debug agent" << endl
            << endl
            << "USB COM ports can usually be autodetected, use -p or -f to force a specific port." << endl
            << "TRK is the default debugging agent, use --coda option when using CODA instead of TRK." << endl
            << "If using System TRK, it is possible to copy the program directly to sys/bin on the phone." << endl
            << "-s can be used with both System and Application TRK/CODA to install the program" << endl;
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
    QString uploadLocalFile;
    QString uploadRemoteFile;
    QString downloadRemoteFile;
    QString downloadLocalFile;
    QString dstName = "c:\\data\\testtemp.sis";
    int loglevel=1;
    int timeout=0;
    bool crashlog = true;
    enum {AgentUnknown, AgentCoda, AgentTRK} debugAgent = AgentUnknown;
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
            else if (arg == "--upload" || arg == "-u") {
                CHECK_PARAMETER_EXISTS
                uploadLocalFile = it.next();
                if (!QFileInfo(uploadLocalFile).exists()) {
                    errstream << "Executable file (" << uploadLocalFile << ") doesn't exist" << endl;
                    return 1;
                }
                CHECK_PARAMETER_EXISTS
                uploadRemoteFile = it.next();
            }
            else if (arg == "--download" || arg == "-d") {
                CHECK_PARAMETER_EXISTS
                downloadRemoteFile = it.next();
                CHECK_PARAMETER_EXISTS
                downloadLocalFile = it.next();
                QFileInfo downloadInfo(downloadLocalFile);
                if (downloadInfo.exists() && !downloadInfo.isFile()) {
                    errstream << downloadLocalFile << " is not a file." << endl;
                    return 1;
                }
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
            else if (arg == "--coda")
                debugAgent = AgentCoda;
            else if (arg == "--trk")
                debugAgent = AgentTRK;
            else if (arg == "--tempfile" || arg == "-T") {
                CHECK_PARAMETER_EXISTS
                dstName = it.next();
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
        (uploadLocalFile.isEmpty() || uploadRemoteFile.isEmpty()) &&
        (downloadLocalFile.isEmpty() || downloadRemoteFile.isEmpty())) {
        printUsage(outstream, args[0]);
        return 1;
    }

    if (!uploadLocalFile.isEmpty() && (!downloadLocalFile.isEmpty() || !downloadRemoteFile.isEmpty())) {
        errstream << "Upload option can't be used together with download" << endl;
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

    CodaSignalHandler codaHandler;
    QScopedPointer<trk::Launcher> launcher;
    TrkSignalHandler trkHandler;
    QFileInfo info(exeFile);
    QFileInfo uploadInfo(uploadLocalFile);

    if (debugAgent == AgentUnknown) {
        outstream << "detecting debug agent..." << endl;
        CodaSignalHandler codaDetector;
        //auto detect agent
        codaDetector.setSerialPortName(serialPortName);
        codaDetector.setLogLevel(loglevel);
        codaDetector.setActionType(ActionPingOnly);
        codaDetector.setTimeout(1000);
        if (!codaDetector.run()) {
            debugAgent = AgentCoda;
            outstream << " - Coda is found" << endl;
        } else {
            debugAgent = AgentTRK;
            outstream << " - Coda is not found, defaulting to TRK" << endl;
        }
    }

    if (debugAgent == AgentCoda) {
        codaHandler.setSerialPortName(serialPortName);
        codaHandler.setLogLevel(loglevel);

        if (!sisFile.isEmpty()) {
            codaHandler.setActionType(ActionCopyInstall);
            codaHandler.setCopyFileName(sisFile, dstName);
        }
        else if (!uploadLocalFile.isEmpty() && uploadInfo.exists()) {
            codaHandler.setActionType(ActionCopy);
            codaHandler.setCopyFileName(uploadLocalFile, uploadRemoteFile);
        }
        if (!exeFile.isEmpty()) {
            codaHandler.setActionType(ActionRun);
            codaHandler.setAppFileName(QString("c:\\sys\\bin\\") + info.fileName());
            codaHandler.setCommandLineArgs(cmdLine.join(QLatin1String(", ")));
        }
        if (!downloadRemoteFile.isEmpty() && !downloadLocalFile.isEmpty()) {
            codaHandler.setActionType(ActionDownload);
            codaHandler.setDownloadFileName(downloadRemoteFile, downloadLocalFile);
        }

        if (loglevel > 0)
            outstream << "Connecting to target via " << serialPortName << endl;

        if (timeout > 0)
            codaHandler.setTimeout(timeout);

        QObject::connect(OsSignalConverter::instance(), SIGNAL(terminate()), &codaHandler, SLOT(terminate()), Qt::QueuedConnection);
        return codaHandler.run();

    } else {
        launcher.reset(new trk::Launcher(trk::Launcher::ActionPingOnly,
            SymbianUtils::SymbianDeviceManager::instance()->acquireDevice(serialPortName)));
        QStringList srcNames, dstNames;
        if (!sisFile.isEmpty()) {
            launcher->addStartupActions(trk::Launcher::ActionCopyInstall);
            srcNames.append(sisFile);
            dstNames.append(dstName);
            launcher->setInstallFileNames(QStringList(dstName));
        }
        if (!uploadLocalFile.isEmpty() && uploadInfo.exists()) {
            launcher->addStartupActions(trk::Launcher::ActionCopy);
            srcNames.append(uploadLocalFile);
            dstNames.append(uploadRemoteFile);
        }
        launcher->setCopyFileNames(srcNames, dstNames);
        if (!exeFile.isEmpty()) {
            launcher->addStartupActions(trk::Launcher::ActionRun);
            launcher->setFileName(QString("c:\\sys\\bin\\") + info.fileName());
            launcher->setCommandLineArgs(cmdLine.join(QLatin1String(" ")));
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

        trkHandler.setLogLevel(loglevel);
        trkHandler.setCrashLogging(crashlog);
        trkHandler.setCrashLogPath(crashlogpath);

        QObject::connect(launcher.data(), SIGNAL(copyingStarted(const QString &)), &trkHandler, SLOT(copyingStarted(const QString &)));
        QObject::connect(launcher.data(), SIGNAL(canNotConnect(const QString &)), &trkHandler, SLOT(canNotConnect(const QString &)));
        QObject::connect(launcher.data(), SIGNAL(canNotCreateFile(const QString &, const QString &)), &trkHandler, SLOT(canNotCreateFile(const QString &, const QString &)));
        QObject::connect(launcher.data(), SIGNAL(canNotWriteFile(const QString &, const QString &)), &trkHandler, SLOT(canNotWriteFile(const QString &, const QString &)));
        QObject::connect(launcher.data(), SIGNAL(canNotCloseFile(const QString &, const QString &)), &trkHandler, SLOT(canNotCloseFile(const QString &, const QString &)));
        QObject::connect(launcher.data(), SIGNAL(installingStarted(const QString &)), &trkHandler, SLOT(installingStarted(const QString &)));
        QObject::connect(launcher.data(), SIGNAL(canNotInstall(const QString &, const QString &)), &trkHandler, SLOT(canNotInstall(const QString &, const QString &)));
        QObject::connect(launcher.data(), SIGNAL(installingFinished()), &trkHandler, SLOT(installingFinished()));
        QObject::connect(launcher.data(), SIGNAL(startingApplication()), &trkHandler, SLOT(startingApplication()));
        QObject::connect(launcher.data(), SIGNAL(applicationRunning(uint)), &trkHandler, SLOT(applicationRunning(uint)));
        QObject::connect(launcher.data(), SIGNAL(canNotRun(const QString &)), &trkHandler, SLOT(canNotRun(const QString &)));
        QObject::connect(launcher.data(), SIGNAL(applicationOutputReceived(const QString &)), &trkHandler, SLOT(applicationOutputReceived(const QString &)));
        QObject::connect(launcher.data(), SIGNAL(copyProgress(int)), &trkHandler, SLOT(copyProgress(int)));
        QObject::connect(launcher.data(), SIGNAL(stateChanged(int)), &trkHandler, SLOT(stateChanged(int)));
        QObject::connect(launcher.data(), SIGNAL(processStopped(uint,uint,uint,QString)), &trkHandler, SLOT(stopped(uint,uint,uint,QString)));
        QObject::connect(launcher.data(), SIGNAL(libraryLoaded(trk::Library)), &trkHandler, SLOT(libraryLoaded(trk::Library)));
        QObject::connect(launcher.data(), SIGNAL(libraryUnloaded(trk::Library)), &trkHandler, SLOT(libraryUnloaded(trk::Library)));
        QObject::connect(launcher.data(), SIGNAL(registersAndCallStackReadComplete(const QList<uint> &,const QByteArray &)), &trkHandler, SLOT(registersAndCallStackReadComplete(const QList<uint> &,const QByteArray &)));
        QObject::connect(&trkHandler, SIGNAL(resume(uint,uint)), launcher.data(), SLOT(resumeProcess(uint,uint)));
        QObject::connect(&trkHandler, SIGNAL(terminate()), launcher.data(), SLOT(terminate()));
        QObject::connect(&trkHandler, SIGNAL(getRegistersAndCallStack(uint,uint)), launcher.data(), SLOT(getRegistersAndCallStack(uint,uint)));
        QObject::connect(launcher.data(), SIGNAL(finished()), &trkHandler, SLOT(finished()));

        QObject::connect(OsSignalConverter::instance(), SIGNAL(terminate()), launcher.data(), SLOT(terminate()), Qt::QueuedConnection);

        QTimer timer;
        timer.setSingleShot(true);
        QObject::connect(&timer, SIGNAL(timeout()), &trkHandler, SLOT(timeout()));
        if (timeout > 0) {
            timer.start(timeout);
        }

        QString errorMessage;
        if (!launcher->startServer(&errorMessage)) {
            errstream << errorMessage << endl;
            return 1;
        }
    }

    return a.exec();
}

