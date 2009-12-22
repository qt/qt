/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

