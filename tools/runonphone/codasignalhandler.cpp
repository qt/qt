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
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QTimer>
#include "codasignalhandler.h"
#include "texttracehandler.h"

static const quint64 DEFAULT_CHUNK_SIZE = 40000;

class CodaSignalHandlerPrivate
{
    friend class CodaSignalHandler;
public:
    CodaSignalHandlerPrivate();
    ~CodaSignalHandlerPrivate();
private:
    SymbianUtils::CodaDevicePtr codaDevice;
    QEventLoop *eventLoop;
    QTextStream out;
    QTextStream err;
    int loglevel;
    int timeout;
    int result;
    CodaAction action;
    QString copySrcFileName;
    QString copyDstFileName;
    QString dlSrcFileName;
    QString dlDstFileName;
    QString appFileName;
    QString commandLineArgs;
    QString serialPortName;
    QString appID;
    QByteArray remoteFileHandle;
    QScopedPointer<QFile> localFile;
    QScopedPointer<QFile> remoteFile;
    quint64 putChunkSize;
    quint64 putLastChunkSize;
    quint64 remoteBytesLeft;
    quint64 remoteFileSize;
    bool putWriteOk;
    bool connected;
    bool debugSessionControl;
};

CodaSignalHandlerPrivate::CodaSignalHandlerPrivate()
    : eventLoop(0),
      out(stdout),
      err(stderr),
      loglevel(0),
      timeout(0),
      result(0),
      action(ActionPingOnly),
      putChunkSize(DEFAULT_CHUNK_SIZE),
      putLastChunkSize(0),
      remoteBytesLeft(0),
      remoteFileSize(0),
      putWriteOk(false),
      connected(false),
      debugSessionControl(false)
{
}

CodaSignalHandlerPrivate::~CodaSignalHandlerPrivate()
{
    delete eventLoop;
    out.flush();
    err.flush();
}

void CodaSignalHandler::error(const QString &errorMessage)
{
    reportError(tr("CODA error: %1").arg(errorMessage));
}

void CodaSignalHandler::logMessage(const QString &logMessage)
{
    reportMessage(tr("CODA log: %1").arg(logMessage));
}

void CodaSignalHandler::serialPong(const QString &codaVersion)
{
    reportMessage(tr("CODA version: %1").arg(codaVersion));
}

void CodaSignalHandler::tcfEvent(const Coda::CodaEvent &event)
{
    reportMessage(tr("CODA event: Type: %1 Message: %2").arg(event.type()).arg(event.toString()));

    switch (event.type()) {
    case Coda::CodaEvent::LocatorHello:
        handleConnected(event);
        break;
    case Coda::CodaEvent::ProcessExitedEvent:
        handleAppExited(event);
        break;
    default:
        reportMessage(tr("CODA unhandled event: Type: %1 Message: %2").arg(event.type()).arg(event.toString()));
        break;
    }
}

void CodaSignalHandler::terminate()
{
    if (d->codaDevice) {
        disconnect(d->codaDevice.data(), 0, this, 0);
        SymbianUtils::SymbianDeviceManager::instance()->releaseCodaDevice(d->codaDevice);
    }
}

void CodaSignalHandler::finished()
{
    if (d->eventLoop)
        d->eventLoop->exit();
}

void CodaSignalHandler::timeout()
{
    reportError(tr("Unable to connect to CODA device. Operation timed out."));
}

int CodaSignalHandler::run()
{
    d->codaDevice = SymbianUtils::SymbianDeviceManager::instance()->getCodaDevice(d->serialPortName);
    bool ok = d->codaDevice && d->codaDevice->device()->isOpen();
    if (!ok) {
        QString deviceError = "No such port";
        if (d->codaDevice)
            deviceError = d->codaDevice->device()->errorString();
        reportError(tr("Could not open serial device: %1").arg(deviceError));
        SymbianUtils::SymbianDeviceManager::instance()->releaseCodaDevice(d->codaDevice);
        return 1;
    }

    TextTraceHandler *traceHandler = new TextTraceHandler(
                SymbianUtils::SymbianDeviceManager::instance()->getOstChannel(d->serialPortName, 2), this);

    if (d->loglevel > 1) {
        d->codaDevice->setVerbose(1);
    }

    connect(d->codaDevice.data(), SIGNAL(error(const QString &)), this, SLOT(error(const QString &)));
    connect(d->codaDevice.data(), SIGNAL(logMessage(const QString &)), this, SLOT(logMessage(const QString &)));
    connect(d->codaDevice.data(), SIGNAL(serialPong(const QString &)), this, SLOT(serialPong(const QString &)));
    connect(d->codaDevice.data(), SIGNAL(tcfEvent(const Coda::CodaEvent &)), this, SLOT(tcfEvent(const Coda::CodaEvent &)));
    connect(this, SIGNAL(done()), this, SLOT(finished()));

    d->codaDevice->sendSerialPing(false);
    QTimer timer;
    if (d->timeout > 0) {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
        timer.setSingleShot(true);
        timer.start(d->timeout);
    }
    d->eventLoop = new QEventLoop();
    d->eventLoop->exec();
    timer.stop();
    int result = d->result;
    reportMessage(tr("Done."));

    delete traceHandler;
    disconnect(d->codaDevice.data(), 0, this, 0);
    SymbianUtils::SymbianDeviceManager::instance()->releaseCodaDevice(d->codaDevice);

    return result;
}

void CodaSignalHandler::setActionType(CodaAction action)
{
    d->action = CodaAction(d->action | action);
}

void CodaSignalHandler::setAppFileName(const QString &fileName)
{
    d->appFileName = fileName;
}

void CodaSignalHandler::setCodaDevice(SymbianUtils::CodaDevicePtr &codaDevice)
{
    d->codaDevice = codaDevice;
}

void CodaSignalHandler::setCommandLineArgs(const QString &args)
{
    d->commandLineArgs = args;
}

void CodaSignalHandler::setCopyFileName(const QString &srcName, const QString &dstName)
{
    d->copySrcFileName = srcName;
    d->copyDstFileName = dstName;
}

void CodaSignalHandler::setDownloadFileName(const QString &srcName, const QString &dstName)
{
    d->dlSrcFileName = srcName;
    d->dlDstFileName = dstName;
}

void CodaSignalHandler::setLogLevel(int level)
{
    d->loglevel = level;
}

void CodaSignalHandler::setSerialPortName(const QString &serialPortName)
{
    d->serialPortName = serialPortName;
}

void CodaSignalHandler::setTimeout(const int msec)
{
    d->timeout = msec;
}

void CodaSignalHandler::closeFile()
{
    d->remoteFile.reset();
    if (!d->codaDevice) {
        emit done();
        return;
    }

    d->codaDevice->sendFileSystemCloseCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemClose),
                                              d->remoteFileHandle);
}

void CodaSignalHandler::handleConnected(const Coda::CodaEvent &event)
{
    if (d->connected)
        return;

    const Coda::CodaLocatorHelloEvent &hEvent = static_cast<const Coda::CodaLocatorHelloEvent &>(event);
    QStringList services = hEvent.services();
    if (services.contains("DebugSessionControl")) {
        d->debugSessionControl = true;
    }
    d->connected = true;
    handleActions();
}

void CodaSignalHandler::handleActions()
{
    if (d->action & ActionCopy) {
        initFileSending();
    } else if (d->action & ActionInstall) {
        initFileInstallation();
    } else if (d->action & ActionRun) {
        initAppRunning();
    } else if (d->action & ActionDownload) {
        initFileDownloading();
    } else {
        emit done();
    }
}

void CodaSignalHandler::handleAppExited(const Coda::CodaEvent &event)
{
    const Coda::CodaProcessExitedEvent &pEvent = static_cast<const Coda::CodaProcessExitedEvent &>(event);
    QString id = pEvent.idString();
    if (!id.compare(d->appID, Qt::CaseInsensitive)) {
        d->codaDevice->sendDebugSessionControlSessionEndCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleDebugSessionControlEnd));
        d->action = static_cast<CodaAction>(d->action & ~ActionRun);
        handleActions();
    }
}

void CodaSignalHandler::handleAppRunning(const Coda::CodaCommandResult &result)
{
    if (result.type == Coda::CodaCommandResult::SuccessReply) {
        reportMessage(tr("Running..."));

        Coda::JsonValue value = result.values.at(0);
        readAppId(value);
    } else {
        reportError(tr("Launch failed: %1").arg(result.toString()));
    }
}

void CodaSignalHandler::handleDebugSessionControlEnd(const Coda::CodaCommandResult &result)
{
    if (result.type == Coda::CodaCommandResult::SuccessReply) {
        // nothing to do
    }
}

void CodaSignalHandler::handleDebugSessionControlStart(const Coda::CodaCommandResult &result)
{
    if (result.type == Coda::CodaCommandResult::SuccessReply) {
        d->codaDevice->sendRunProcessCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleAppRunning),
                                             d->appFileName.toAscii(),
                                             d->commandLineArgs.split(' '));
        reportMessage(tr("Launching %1...").arg(QFileInfo(d->appFileName).fileName()));
    } else {
        reportError(tr("Failed to start CODA debug session control."));
    }
}

void CodaSignalHandler::handleFileSystemClose(const Coda::CodaCommandResult &result)
{
    if (result.type == Coda::CodaCommandResult::SuccessReply) {
        reportMessage(tr("File closed."));
        if (d->action & ActionCopy) {
            d->action = static_cast<CodaAction>(d->action & ~ActionCopy);
        } else if (d->action & ActionDownload) {
            d->action = static_cast<CodaAction>(d->action & ~ActionDownload);
        }
        handleActions();
    } else {
        reportError(tr("Failed to close the remote file: %1").arg(result.toString()));
    }
}

void CodaSignalHandler::handleFileSystemOpen(const Coda::CodaCommandResult &result)
{
    if (result.type != Coda::CodaCommandResult::SuccessReply) {
        reportError(tr("Could not open remote file: %1").arg(result.errorString()));
        return;
    }

    if (result.values.size() < 1 || result.values.at(0).data().isEmpty()) {
        reportError(tr("Internal error: No filehandle obtained"));
        return;
    }

    if (d->action & ActionCopy) {
        d->remoteFileHandle = result.values.at(0).data();
        d->remoteFile.reset(new QFile(d->copySrcFileName));
        if (!d->remoteFile->open(QIODevice::ReadOnly)) { // Should not fail, was checked before
            reportError(tr("Could not open local file %1").arg(d->copySrcFileName));
            return;
        }
        putSendNextChunk();
    } else if (d->action & ActionDownload) {
        d->remoteFileHandle = result.values.at(0).data();
        d->localFile.reset(new QFile(d->dlDstFileName));
        // remove any existing file with the same name
        if (d->localFile->exists() && !d->localFile->remove()) {
            reportError(tr("Could not create host file: %1 due to error: %2").arg(d->localFile->fileName(), d->localFile->errorString()));
            return;
        }
        // open local file in write-only mode
        if (!d->localFile->open(QFile::WriteOnly)) {
            reportError(tr("Could not open host file for writing: %1 due to error: %2").arg(d->localFile->fileName(), d->localFile->errorString()));
            return;
        }
        d->codaDevice->sendFileSystemFstatCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemStart),
                                                  d->remoteFileHandle);
    }
}

void CodaSignalHandler::handleFileSystemRead(const Coda::CodaCommandResult &result)
{
    if (result.type != Coda::CodaCommandResult::SuccessReply || result.values.size() != 2) {
        reportError(tr("Could not read remote file: %1").arg(result.errorString()));
        return;
    }

    QByteArray data = QByteArray::fromBase64(result.values.at(0).data());
    bool eof = result.values.at(1).toVariant().toBool();
    qint64 written = d->localFile->write(data);
    if (written < 0) {
        reportError(tr("Could not write data to host file: %1 due to error: %2").arg(d->localFile->fileName(), d->localFile->errorString()));
        return;
    }

    d->remoteBytesLeft -= written;
    if (!eof && d->remoteBytesLeft > 0) {
        readNextChunk();
    }
    else {
        d->localFile->flush();
        d->localFile->close();
        closeFile();
    }
}

void CodaSignalHandler::handleFileSystemStart(const Coda::CodaCommandResult &result)
{
    if (result.type != Coda::CodaCommandResult::SuccessReply) {
        reportError(tr("Could not open remote file: %1").arg(result.errorString()));
        return;
    }

    if (result.values.size() < 1 || result.values.at(0).children().isEmpty()) {
        reportError(tr("Could not get file attributes"));
        return;
    }

    Coda::JsonValue val = result.values.at(0).findChild("Size");
    d->remoteFileSize = val.isValid() ? val.data().toLong() : -1L;
    if (d->remoteFileSize < 0) {
        reportError(tr("Could not get file size"));
        return;
    }

    d->remoteBytesLeft = d->remoteFileSize;
    readNextChunk();
}

void CodaSignalHandler::handleFileSystemWrite(const Coda::CodaCommandResult &result)
{
    // Close remote file even if copy fails
    d->putWriteOk = result;
    if (!d->putWriteOk) {
        QString fileName = QFileInfo(d->copyDstFileName).fileName();
        reportError(tr("Could not write to file %1 on device: %2").arg(fileName).arg(result.errorString()));
    }

    if (!d->putWriteOk || d->putLastChunkSize < d->putChunkSize) {
        closeFile();
    } else {
        putSendNextChunk();
    }
}

void CodaSignalHandler::handleSymbianInstall(const Coda::CodaCommandResult &result)
{
    if (result.type == Coda::CodaCommandResult::SuccessReply) {
        reportMessage(tr("Installation has finished."));
        d->action = static_cast<CodaAction>(d->action & ~ActionInstall);
        handleActions();
    } else {
        reportError(tr("Installation failed: %1").arg(result.errorString()));
    }
}

void CodaSignalHandler::initAppRunning()
{
    if (!d->codaDevice || d->appFileName.isEmpty()) {
        emit done();
        return;
    }

    if (!d->debugSessionControl) {
        reportError(tr("CODA DebugSessionControl service not found, please update to CODA v4.0.23 or later."));
    }

    d->codaDevice->sendDebugSessionControlSessionStartCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleDebugSessionControlStart));
}

void CodaSignalHandler::initFileDownloading()
{
    if (!d->codaDevice || d->dlSrcFileName.isEmpty()) {
        emit done();
        return;
    }

    d->codaDevice->sendFileSystemOpenCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemOpen),
                                             d->dlSrcFileName.toAscii(), Coda::CodaDevice::FileSystem_TCF_O_READ);
    reportMessage(tr("Downloading %1...").arg(QFileInfo(d->dlSrcFileName).fileName()));
}

void CodaSignalHandler::initFileInstallation()
{
    if (!d->codaDevice || d->copyDstFileName.isEmpty()) {
        emit done();
        return;
    }

    QString installationDrive = "C";
    d->codaDevice->sendSymbianInstallSilentInstallCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleSymbianInstall),
                                                          d->copyDstFileName.toAscii(),
                                                          installationDrive.toAscii());
    reportMessage(tr("Installing package \"%1\" on drive %2...").arg(QFileInfo(d->copyDstFileName).fileName(), installationDrive));
}

void CodaSignalHandler::initFileSending()
{
    if (!d->codaDevice || d->copySrcFileName.isEmpty()) {
        emit done();
        return;
    }

    const unsigned flags =
            Coda::CodaDevice::FileSystem_TCF_O_WRITE
            |Coda::CodaDevice::FileSystem_TCF_O_CREAT
            |Coda::CodaDevice::FileSystem_TCF_O_TRUNC;
    d->putWriteOk = false;
    d->codaDevice->sendFileSystemOpenCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemOpen),
                                             d->copyDstFileName.toAscii(), flags);
    reportMessage(tr("Copying %1...").arg(QFileInfo(d->copyDstFileName).fileName()));
}

void CodaSignalHandler::putSendNextChunk()
{
    if (!d->codaDevice || !d->remoteFile) {
        emit done();
        return;
    }

    // Read and send off next chunk
    const quint64 pos = d->remoteFile->pos();
    const QByteArray data = d->remoteFile->read(d->putChunkSize);
    if (data.isEmpty()) {
        d->putWriteOk = true;
        closeFile();
    } else {
        d->putLastChunkSize = data.size();
        d->codaDevice->sendFileSystemWriteCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemWrite),
                                                  d->remoteFileHandle, data, unsigned(pos));
    }
}

void CodaSignalHandler::readNextChunk()
{
    const quint64 pos = d->remoteFileSize - d->remoteBytesLeft;
    const quint64 size = qMin(d->remoteBytesLeft, DEFAULT_CHUNK_SIZE);
    d->codaDevice->sendFileSystemReadCommand(Coda::CodaCallback(this, &CodaSignalHandler::handleFileSystemRead),
                                             d->remoteFileHandle, pos, size);
}

void CodaSignalHandler::readAppId(Coda::JsonValue value)
{
    if (value.isObject()) {
        Coda::JsonValue idValue = value.findChild("ID");
        if (idValue.isString()) {
            d->appID = idValue.data();
            return;
        }
    }

    reportError(tr("Could not get process ID of %1.").arg(QFileInfo(d->appFileName).fileName()));
}

void CodaSignalHandler::reportError(const QString &message)
{
    d->err << message << endl;
    d->result = 1;
    emit done();
}

void CodaSignalHandler::reportMessage(const QString &message)
{
    if (d->loglevel > 0)
        d->out << message << endl;
}

CodaSignalHandler::CodaSignalHandler()
    : d(new CodaSignalHandlerPrivate())
{
}

CodaSignalHandler::~CodaSignalHandler()
{
    delete d;
}

