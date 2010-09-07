/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QDebug>
#include <QCoreApplication>
#include <QObject>
#include "trksignalhandler.h"

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
};

void TrkSignalHandler::copyingStarted()
{
    if (d->loglevel > 0)
        d->out << "Copying..." << endl;
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

void TrkSignalHandler::installingStarted()
{
    if (d->loglevel > 0)
        d->out << "Installing..." << endl;
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

void TrkSignalHandler::stopped(uint pc, uint pid, uint tid, const QString& reason)
{
    d->err << "STOPPED: pc=" << hex << pc << " pid=" << pid
           << " tid=" << tid << dec << " - " << reason << endl;
    // if it was a breakpoint, then we could continue with "emit resume(pid, tid);"
    // since we have set no breakpoints, it will be a just in time debug of a panic / exception
    emit terminate();
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
    lastpercent(0)
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
