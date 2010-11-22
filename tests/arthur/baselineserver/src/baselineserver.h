/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#ifndef BASELINESERVER_H
#define BASELINESERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include <QScopedPointer>
#include <QTimer>
#include <QDateTime>

#include "baselineprotocol.h"
#include "htmlpage.h"

// #seconds between update checks
#define HEARTBEAT 10

class BaselineServer : public QTcpServer
{
    Q_OBJECT

public:
    BaselineServer(QObject *parent = 0);

    static QString storagePath();
    static QString baseUrl();

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void heartbeat();

private:
    QTimer *heartbeatTimer;
    QDateTime meLastMod;
    static QString storage;
};



class BaselineThread : public QThread
{
    Q_OBJECT

public:
    BaselineThread(int socketDescriptor, QObject *parent);
    void run();

private:
    int socketDescriptor;
};


class BaselineHandler : public QObject
{
    Q_OBJECT

public:
    BaselineHandler(int socketDescriptor = -1);
    void testPathMapping();

    static QString updateAllBaselines(const QString &host, const QString &id,
                                      const QString &engine, const QString &format);
    static QString updateSingleBaseline(const QString &oldBaseline, const QString &newBaseline);
    static QString blacklistTest(const QString &context, const QString &itemId, bool removeFromBlacklist = false);

private slots:
    void receiveRequest();
    void receiveDisconnect();

private:
    void provideBaselineChecksums(const QByteArray &itemListBlock);
    void storeImage(const QByteArray &itemBlock, bool isBaseline);
    void mapPlatformInfo();
    QString pathForItem(const ImageItem &item, bool isBaseline = true, bool absolute = true);
    const char *logtime();
    QString computeMismatchScore(const QImage& baseline, const QImage& rendered);

    BaselineProtocol proto;
    PlatformInfo plat;
    PlatformInfo mapped;
    bool connectionEstablished;
    QString runId;
    HTMLPage report;
};

#endif // BASELINESERVER_H
