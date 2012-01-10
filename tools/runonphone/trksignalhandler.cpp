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

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include <QFile>
#include <QDir>
#include "trksignalhandler.h"
#include "trkutils.h"

class CrashState
{
public:
    uint pid;
    uint tid;
    QString crashReason;
    uint crashPC;
};

class TrkSignalHandlerPrivate
{
    friend class TrkSignalHandler;
public:
    TrkSignalHandlerPrivate();
    ~TrkSignalHandlerPrivate();
private:
    QTextStream out;
    QTextStream err;
    int loglevel;
    int lastpercent;
    QList<trk::Library> libraries;
    QFile crashlogtextfile;
    QFile crashstackfile;
    QList<CrashState> queuedCrashes;
    QList<int> dyingThreads;
    QString crashlogPath;
    bool crashlog;
    bool terminateNeeded;
};

void TrkSignalHandler::copyingStarted(const QString &fileName)
{
    if (d->loglevel > 0)
        d->out << "Copying " << fileName << "..." << endl;
}

void TrkSignalHandler::canNotConnect(const QString &errorMessage)
{
    d->err << "Cannot connect - " << errorMessage << endl;
}

void TrkSignalHandler::canNotCreateFile(const QString &filename, const QString &errorMessage)
{
    d->err << "Cannot create file (" << filename << ") - " << errorMessage << endl;
}

void TrkSignalHandler::canNotWriteFile(const QString &filename, const QString &errorMessage)
{
    d->err << "Cannot write file (" << filename << ") - " << errorMessage << endl;
}

void TrkSignalHandler::canNotCloseFile(const QString &filename, const QString &errorMessage)
{
    d->err << "Cannot close file (" << filename << ") - " << errorMessage << endl;
}

void TrkSignalHandler::installingStarted(const QString &packageName)
{
    if (d->loglevel > 0)
        d->out << "Installing " << packageName << "..." << endl;
}

void TrkSignalHandler::canNotInstall(const QString &packageFilename, const QString &errorMessage)
{
    d->err << "Cannot install file (" << packageFilename << ") - " << errorMessage << endl;
}

void TrkSignalHandler::installingFinished()
{
    if (d->loglevel > 0)
        d->out << "Installing finished" << endl;
}

void TrkSignalHandler::startingApplication()
{
    if (d->loglevel > 0)
        d->out << "Starting app..." << endl;
}

void TrkSignalHandler::applicationRunning(uint pid)
{
    Q_UNUSED(pid)
    if (d->loglevel > 0)
        d->out << "Running..." << endl;
}

void TrkSignalHandler::canNotRun(const QString &errorMessage)
{
    d->err << "Cannot run - " << errorMessage << endl;
}

void TrkSignalHandler::finished()
{
    if (d->loglevel > 0)
        d->out << "Done." << endl;
    QCoreApplication::quit();
}

void TrkSignalHandler::applicationOutputReceived(const QString &output)
{
    d->out << output << flush;
}

void TrkSignalHandler::copyProgress(int percent)
{
    if (d->loglevel > 0) {
        if (d->lastpercent == 0)
            d->out << "[                                                 ]\r[" << flush;
        while (percent > d->lastpercent) {
            d->out << QLatin1Char('#');
            d->lastpercent+=2; //because typical console is 80 chars wide
        }
        d->out.flush();
        if (percent==100)
            d->out << endl;
    }
}

void TrkSignalHandler::stateChanged(int state)
{
    if (d->loglevel > 1)
        d->out << "State" << state << endl;
}

void TrkSignalHandler::setLogLevel(int level)
{
    d->loglevel = level;
}

void TrkSignalHandler::setCrashLogging(bool enabled)
{
    d->crashlog = enabled;
}

void TrkSignalHandler::setCrashLogPath(QString path)
{
    d->crashlogPath = path;
}

bool lessThanCodeBase(const trk::Library& cs1, const trk::Library& cs2)
{
    return cs1.codeseg < cs2.codeseg;
}

void TrkSignalHandler::stopped(uint pc, uint pid, uint tid, const QString& reason)
{
    d->err << "STOPPED: pc=" << hex << pc << " pid=" << pid
           << " tid=" << tid << dec << " - " << reason << endl;

    if (d->crashlog) {
        CrashState cs;
        cs.pid = pid;
        cs.tid = tid;
        cs.crashPC = pc;
        cs.crashReason = reason;

        if (d->dyingThreads.contains(tid)) {
            if(d->queuedCrashes.isEmpty())
                emit terminate();
            else
                d->terminateNeeded = true;
        } else {
            d->queuedCrashes.append(cs);
            d->dyingThreads.append(tid);

            if (d->queuedCrashes.count() == 1) {
                d->err << "Fetching registers and stack..." << endl;
                emit getRegistersAndCallStack(pid, tid);
            }
        }
    }
    else
        emit terminate();
}

void TrkSignalHandler::registersAndCallStackReadComplete(const QList<uint>& registers, const QByteArray& stack)
{
    CrashState cs = d->queuedCrashes.first();
    QDir dir(d->crashlogPath);
    d->crashlogtextfile.setFileName(dir.filePath(QString("d_exc_%1.txt").arg(cs.tid)));
    d->crashstackfile.setFileName(dir.filePath(QString("d_exc_%1.stk").arg(cs.tid)));
    d->crashlogtextfile.open(QIODevice::WriteOnly);
    QTextStream crashlog(&d->crashlogtextfile);

    crashlog << "-----------------------------------------------------------------------------" << endl;
    crashlog << "EKA2 USER CRASH LOG" << endl;
    crashlog << "Thread Name: " << QString("ProcessID-%1::ThreadID-%2").arg(cs.pid).arg(cs.tid) << endl;
    crashlog << "Thread ID: " << cs.tid << endl;
    //this is wrong, but TRK doesn't make stack limit available so we lie
    crashlog << QString("User Stack %1-%2").arg(registers.at(13), 8, 16, QChar('0')).arg(registers.at(13) + stack.size(), 8, 16, QChar('0')) << endl;
    //this is also wrong, but TRK doesn't give all information for exceptions
    crashlog << QString("Panic: PC=%1 ").arg(cs.crashPC, 8, 16, QChar('0')) << cs.crashReason << endl;
    crashlog << endl;
    crashlog << "USER REGISTERS:" << endl;
    crashlog << QString("CPSR=%1").arg(registers.at(16), 8, 16, QChar('0')) << endl;
    for (int i=0;i<16;i+=4) {
        crashlog << QString("r%1=%2 %3 %4 %5")
            .arg(i, 2, 10, QChar('0'))
            .arg(registers.at(i), 8, 16, QChar('0'))
            .arg(registers.at(i+1), 8, 16, QChar('0'))
            .arg(registers.at(i+2), 8, 16, QChar('0'))
            .arg(registers.at(i+3), 8, 16, QChar('0')) << endl;
    }
    crashlog << endl;

    //emit info for post mortem debug
    qSort(d->libraries.begin(), d->libraries.end(), lessThanCodeBase);
    d->err << "Code Segments:" << endl;
    crashlog << "CODE SEGMENTS:" << endl;
    for(int i=0; i<d->libraries.count(); i++) {
        const trk::Library& seg = d->libraries.at(i);
        if(seg.pid != cs.pid)
            continue;
        if (d->loglevel > 1) {
            d->err << QString("Code: %1 Data: %2 Name: ")
                .arg(seg.codeseg, 8, 16, QChar('0'))
                .arg(seg.dataseg, 8, 16, QChar('0'))
                << seg.name << endl;
        }

        //produce fake code segment end addresses since we don't get the real ones from TRK
        uint end;
        if (i+1 < d->libraries.count())
            end = d->libraries.at(i+1).codeseg - 1;
        else
            end = 0xFFFFFFFF;

        crashlog << QString("%1-%2 ")
            .arg(seg.codeseg, 8, 16, QChar('0'))
            .arg(end, 8, 16, QChar('0'))
            << seg.name << endl;
    }

    d->crashlogtextfile.close();

    if (d->loglevel > 1) {
        d->err << "Registers:" << endl;
        for (int i=0;i<16;i++) {
            d->err << QString("R%1: %2 ").arg(i, 2, 10, QChar('0')).arg(registers.at(i), 8, 16, QChar('0'));
            if (i % 4 == 3)
                d->err << endl;
        }
        d->err << QString("CPSR: %1").arg(registers.at(16), 8, 16, QChar('0')) << endl;

        d->err << "Stack:" << endl;
        uint sp = registers.at(13);
        for(int i=0; i<stack.size(); i+=16, sp+=16) {
            d->err << QString("%1: ").arg(sp, 8, 16, QChar('0'));
            d->err << trk::stringFromArray(stack.mid(i,16));
            d->err << endl;
        }
    }
    d->crashstackfile.open(QIODevice::WriteOnly);
    d->crashstackfile.write(stack);
    d->crashstackfile.close();

    if (d->loglevel > 0)
        d->err << "Crash logs saved to " << d->crashlogtextfile.fileName() << " & " << d->crashstackfile.fileName() << endl;

    // resume the thread to allow Symbian OS to handle the panic normally.
    // terminate when a non main thread is suspended reboots the phone (TRK bug)
    emit resume(cs.pid, cs.tid);

    //fetch next crashed thread
    d->queuedCrashes.removeFirst();
    if (d->queuedCrashes.count()) {
        cs = d->queuedCrashes.first();
        d->err << "Fetching registers and stack..." << endl;
        emit getRegistersAndCallStack(cs.pid, cs.tid);
    }
    else if (d->terminateNeeded)
        emit terminate();

}

void TrkSignalHandler::libraryLoaded(const trk::Library &lib)
{
    d->libraries << lib;
}

void TrkSignalHandler::libraryUnloaded(const trk::Library &lib)
{
    for (QList<trk::Library>::iterator i = d->libraries.begin(); i != d->libraries.end(); i++) {
        if((*i).name == lib.name && (*i).pid == lib.pid)
            i = d->libraries.erase(i);
    }
}

void TrkSignalHandler::timeout()
{
    d->err << "FAILED: stopping test due to timeout" << endl;
    emit terminate();
}

TrkSignalHandlerPrivate::TrkSignalHandlerPrivate()
    : out(stdout),
    err(stderr),
    loglevel(0),
    lastpercent(0),
    terminateNeeded(false)
{

}

TrkSignalHandlerPrivate::~TrkSignalHandlerPrivate()
{
    out.flush();
    err.flush();
}

TrkSignalHandler::TrkSignalHandler()
    : d(new TrkSignalHandlerPrivate())
{
}

TrkSignalHandler::~TrkSignalHandler()
{
    delete d;
}
