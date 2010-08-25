#ifndef BASELINEPROTOCOL_H
#define BASELINEPROTOCOL_H

#include <QDataStream>
#include <QTcpSocket>

#define FileFormat "png"

struct PlatformInfo
{
public:
    PlatformInfo(bool useLocal = false);

    QString buildKey;
    QString qtVersion;
    QString hostname;
};



class BaselineProtocol
{
public:
    BaselineProtocol() {}
    ~BaselineProtocol();

    // ****************************************************
    // Important constants here
    // ****************************************************
    enum Constant {
        ProtocolVersion = 1,
        ServerPort = 54129,
        Timeout = 100000
    };

    enum Command {
        UnknownError = 0,
        // Queries
        AcceptPlatformInfo = 1,
        RequestBaseline = 2,
        AcceptNewBaseline = 3,
        AcceptMismatch = 4,
        // Responses
        Ack = 128,
        AcceptBaseline = 129,
        BaselineNotPresent = 130,
        IgnoreCase = 131
    };

    // For client:
    bool connect();
    bool requestBaseline(const QString &caseId, Command *response, QImage *baseline);
    bool submitNewBaseline(const QString &caseId, const QImage &baseline);
    bool submitMismatch(const QString &caseId, const QImage &mismatch, QByteArray *failMsg);

    // For server:
    bool acceptConnection(PlatformInfo *pi);

    QString errorMessage();

private:
    bool sendBlock(Command cmd, const QByteArray &block);
    bool receiveBlock(Command *cmd, QByteArray *block);
    QString errMsg;
    QTcpSocket socket;

    friend class BaselineThread;
    friend class BaselineHandler;
};


QDataStream & operator<< (QDataStream &stream, const PlatformInfo &pinfo);

QDataStream & operator>> (QDataStream& stream, PlatformInfo& pinfo);

#endif // BASELINEPROTOCOL_H
