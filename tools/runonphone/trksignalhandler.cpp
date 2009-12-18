/**************************************************************************
**
** This file is part of the tools applications of the Qt Toolkit.
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
**************************************************************************/
#include <QDebug>
#include <QCoreApplication>
#include "trksignalhandler.h"

void TrkSignalHandler::copyingStarted()
{
    qDebug() << "Copying...\n";
}

void TrkSignalHandler::canNotConnect(const QString &errorMessage)
{
    qWarning() << "Cannot Connect - " << errorMessage;
}

void TrkSignalHandler::canNotCreateFile(const QString &filename, const QString &errorMessage)
{
    qWarning() << "Cannot create file (" << filename << ") - " << errorMessage << "\n";
}

void TrkSignalHandler::canNotWriteFile(const QString &filename, const QString &errorMessage)
{
    qWarning() << "Cannot write file (" << filename << ") - " << errorMessage << "\n";
}

void TrkSignalHandler::canNotCloseFile(const QString &filename, const QString &errorMessage)
{
    qWarning() << "Cannot close file (" << filename << ") - " << errorMessage << "\n";
}

void TrkSignalHandler::installingStarted()
{
    qDebug() << "Installing...\n";
}

void TrkSignalHandler::canNotInstall(const QString &packageFilename, const QString &errorMessage)
{
    qWarning() << "Cannot install file (" << packageFilename << ") - " << errorMessage << "\n";
}

void TrkSignalHandler::installingFinished()
{
    qDebug() << "Installing finished\n";
}

void TrkSignalHandler::startingApplication()
{
    qDebug() << "Starting app...\n";
}

void TrkSignalHandler::applicationRunning(uint pid)
{
    qDebug() << "Running...\n";
}

void TrkSignalHandler::canNotRun(const QString &errorMessage)
{
    qWarning() << "Cannot run - " << errorMessage << "\n";
}

void TrkSignalHandler::finished()
{
    qDebug() << "Done.\n";
    QCoreApplication::quit();
}

void TrkSignalHandler::applicationOutputReceived(const QString &output)
{
    qDebug() << "> " << output;
}

void TrkSignalHandler::copyProgress(int percent)
{
    qDebug() << percent << "%";
}

void TrkSignalHandler::stateChanged(int state)
{
    qDebug() << "State" << state;
}

