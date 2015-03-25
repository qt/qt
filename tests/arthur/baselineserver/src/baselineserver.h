/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
#include <QSettings>

#include "baselineprotocol.h"
#include "report.h"

// #seconds between update checks
#define HEARTBEAT 10
#define MetadataFileExt "metadata"

class BaselineServer : public QTcpServer
{
    Q_OBJECT

public:
    BaselineServer(QObject *parent = 0);

    static QString storagePath();
    static QString baseUrl();
    static QString settingsFilePath();

protected:
    void incomingConnection(int socketDescriptor);

private slots:
    void heartbeat();

private:
    QTimer *heartbeatTimer;
    QDateTime meLastMod;
    QString lastRunId;
    int lastRunIdIdx;
    static QString storage;
    static QString url;
    static QString settingsFile;
};



class BaselineThread : public QThread
{
    Q_OBJECT

public:
    BaselineThread(const QString &runId, int socketDescriptor, QObject *parent);
    void run();

private:
    QString runId;
    int socketDescriptor;
};


class BaselineHandler : public QObject
{
    Q_OBJECT

public:
    BaselineHandler(const QString &runId, int socketDescriptor = -1);
    void testPathMapping();
    QString pathForItem(const ImageItem &item, bool isBaseline = true, bool absolute = true) const;

    // CGI callbacks:
    static QString view(const QString &baseline, const QString &rendered, const QString &compared);
    static QString clearAllBaselines(const QString &context);
    static QString updateBaselines(const QString &context, const QString &mismatchContext, const QString &itemFile);
    static QString blacklistTest(const QString &context, const QString &itemId, bool removeFromBlacklist = false);

private slots:
    void receiveRequest();
    void receiveDisconnect();

private:
    bool establishConnection();
    void provideBaselineChecksums(const QByteArray &itemListBlock);
    void storeImage(const QByteArray &itemBlock, bool isBaseline);
    void storeItemMetadata(const PlatformInfo &metadata, const QString &path);
    PlatformInfo fetchItemMetadata(const QString &path);
    PlatformInfo mapPlatformInfo(const PlatformInfo& orig) const;
    const char *logtime();
    QString computeMismatchScore(const QImage& baseline, const QImage& rendered);

    BaselineProtocol proto;
    PlatformInfo clientInfo;
    mutable PlatformInfo mappedClientInfo;
    mutable PlatformInfo overriddenMappedClientInfo;
    QString runId;
    bool connectionEstablished;
    Report report;
    QSettings *settings;
};

#endif // BASELINESERVER_H
