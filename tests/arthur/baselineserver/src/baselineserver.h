#ifndef BASELINESERVER_H
#define BASELINESERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QThread>
#include <QTcpSocket>
#include "baselineprotocol.h"
#include <QScopedPointer>
#include <QTimer>
#include <QDateTime>

// #seconds between update checks
#define HEARTBEAT 10

class BaselineServer : public QTcpServer
{
    Q_OBJECT

public:
    BaselineServer(QObject *parent = 0);

    static QString storagePath();

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
    BaselineHandler(int socketDescriptor);

private slots:
    void receiveRequest();
    void receiveDisconnect();

private:
    void provideBaseline(const QByteArray &caseId);
    void storeImage(const QByteArray &imageBlock, bool isBaseline);
    QString pathForCaseId(const QByteArray &caseId, bool isBaseline = true);
    QString logtime();

    BaselineProtocol proto;
    PlatformInfo plat;
    bool connectionEstablished;
    QString runId;
};

#endif // BASELINESERVER_H
