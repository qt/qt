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

#ifndef TRKSIGNALHANDLER_H
#define TRKSIGNALHANDLER_H
#include <QObject>
#include <QString>

class TrkSignalHandlerPrivate;
class TrkSignalHandler : public QObject
{
    Q_OBJECT
public slots:
    void copyingStarted();
    void canNotConnect(const QString &errorMessage);
    void canNotCreateFile(const QString &filename, const QString &errorMessage);
    void canNotWriteFile(const QString &filename, const QString &errorMessage);
    void canNotCloseFile(const QString &filename, const QString &errorMessage);
    void installingStarted();
    void canNotInstall(const QString &packageFilename, const QString &errorMessage);
    void installingFinished();
    void startingApplication();
    void applicationRunning(uint pid);
    void canNotRun(const QString &errorMessage);
    void finished();
    void applicationOutputReceived(const QString &output);
    void copyProgress(int percent);
    void stateChanged(int);
    void stopped(uint pc, uint pid, uint tid, const QString& reason);
    void timeout();
signals:
    void resume(uint pid, uint tid);
    void terminate();
public:
    TrkSignalHandler();
    ~TrkSignalHandler();
    void setLogLevel(int);
private:
    TrkSignalHandlerPrivate *d;
};

#endif // TRKSIGNALHANDLER_H
