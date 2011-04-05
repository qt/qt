/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BASELINEPROTOCOL_H
#define BASELINEPROTOCOL_H

#include <QDataStream>
#include <QTcpSocket>
#include <QImage>
#include <QVector>
#include <QMap>
#include <QPointer>

#define QLS QLatin1String
#define QLC QLatin1Char

#define FileFormat "png"

const QString PI_TestCase(QLS("TestCase"));
const QString PI_HostName(QLS("HostName"));
const QString PI_HostAddress(QLS("HostAddress"));
const QString PI_OSName(QLS("OSName"));
const QString PI_OSVersion(QLS("OSVersion"));
const QString PI_QtVersion(QLS("QtVersion"));
const QString PI_BuildKey(QLS("BuildKey"));
const QString PI_GitCommit(QLS("GitCommit"));
const QString PI_QMakeSpec(QLS("QMakeSpec"));
const QString PI_PulseGitBranch(QLS("PulseGitBranch"));
const QString PI_PulseTestrBranch(QLS("PulseTestrBranch"));

class PlatformInfo : public QMap<QString, QString>
{
public:
    PlatformInfo(bool useLocal = false);
};

struct ImageItem
{
public:
    ImageItem()
        : status(Ok), itemChecksum(0)
    {}
    ImageItem(const ImageItem &other)
    { *this = other; }
    ~ImageItem()
    {}
    ImageItem &operator=(const ImageItem &other);

    static quint64 computeChecksum(const QImage& image);

    enum ItemStatus {
        Ok = 0,
        BaselineNotFound = 1,
        IgnoreItem = 2,
        Mismatch = 3
    };

    QString testFunction;
    QString itemName;
    ItemStatus status;
    QImage image;
    QList<quint64> imageChecksums;
    quint16 itemChecksum;

    void writeImageToStream(QDataStream &stream) const;
    void readImageFromStream(QDataStream &stream);
};
QDataStream & operator<< (QDataStream &stream, const ImageItem &ii);
QDataStream & operator>> (QDataStream &stream, ImageItem& ii);

Q_DECLARE_METATYPE(ImageItem);

typedef QVector<ImageItem> ImageItemList;


class BaselineProtocol
{
public:
    BaselineProtocol();
    ~BaselineProtocol();

    static BaselineProtocol *instance(QObject *parent = 0);

    // ****************************************************
    // Important constants here
    // ****************************************************
    enum Constant {
        ProtocolVersion = 4,
        ServerPort = 54129,
        Timeout = 5000
    };

    enum Command {
        UnknownError = 0,
        // Queries
        AcceptPlatformInfo = 1,
        RequestBaselineChecksums = 2,
        AcceptNewBaseline = 4,
        AcceptMismatch = 5,
        // Responses
        Ack = 128,
        Abort = 129,
        DoDryRun = 130
    };

    // For client:

    // For advanced client:
    bool connect(const QString &testCase, bool *dryrun = 0);
    bool requestBaselineChecksums(const QString &testFunction, ImageItemList *itemList);
    bool submitNewBaseline(const ImageItem &item, QByteArray *serverMsg);
    bool submitMismatch(const ImageItem &item, QByteArray *serverMsg);

    // For server:
    bool acceptConnection(PlatformInfo *pi);

    QString errorMessage();

private:
    bool sendItem(Command cmd, const ImageItem &item);

    bool sendBlock(Command cmd, const QByteArray &block);
    bool receiveBlock(Command *cmd, QByteArray *block);
    void sysSleep(int ms);

    QString errMsg;
    QTcpSocket socket;

    friend class BaselineThread;
    friend class BaselineHandler;
};


#endif // BASELINEPROTOCOL_H
