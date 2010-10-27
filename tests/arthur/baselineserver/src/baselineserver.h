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
    static QString blacklistTest(const QString &scriptName, const QString &host,
                                 const QString &engine, const QString &format);
    static QString whitelistTest(const QString &scriptName, const QString &host,
                                 const QString &engine, const QString &format);

private slots:
    void receiveRequest();
    void receiveDisconnect();

private:
    void provideBaselineChecksums(const QByteArray &itemListBlock);
    void storeImage(const QByteArray &itemBlock, bool isBaseline);
    QString pathForItem(const ImageItem &item, bool isBaseline = true, bool absolute = true);
    QString logtime();
    QString computeMismatchScore(const QImage& baseline, const QImage& rendered);
    QString engineForItem(const ImageItem &item);

    static QString itemSubPath(const QString &engine, const QString &format, bool isBaseline = true);

    BaselineProtocol proto;
    PlatformInfo plat;
    bool connectionEstablished;
    QString runId;
    QString pathForRun;
    HTMLPage report;
};

#endif // BASELINESERVER_H
