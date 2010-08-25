
#include "baselineprotocol.h"
#include <QLibraryInfo>
#include <QImage>
#include <QBuffer>
#include <QHostInfo>

PlatformInfo::PlatformInfo(bool useLocal)
{
    if (useLocal) {
        buildKey = QLibraryInfo::buildKey();
        qtVersion = QLatin1String(qVersion());
        hostname = QHostInfo::localHostName();
    }
}

QDataStream & operator<< (QDataStream &stream, const PlatformInfo &pinfo)
{
    stream << pinfo.buildKey << pinfo.qtVersion << pinfo.hostname;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, PlatformInfo& pinfo)
{
    stream >> pinfo.buildKey >> pinfo.qtVersion >> pinfo.hostname;
    return stream;
}

BaselineProtocol::~BaselineProtocol()
{
    socket.close();
    if (socket.state() != QTcpSocket::UnconnectedState)
        socket.waitForDisconnected(Timeout);
}


bool BaselineProtocol::connect()
{
    errMsg.clear();
    //###TBD: determine server address; for now local devhost
    QLatin1String serverName("chimera.europe.nokia.com");

    socket.connectToHost(serverName, ServerPort);

    if (!socket.waitForConnected(Timeout)) {
        errMsg += QLatin1String("TCP connectToHost failed. Host:") + serverName + QLatin1String(" port:") + QString::number(ServerPort);
        return false;
    }

    PlatformInfo pi(true);
    QByteArray block;
    QDataStream ds(&block, QIODevice::ReadWrite);
    ds << pi;
    if (!sendBlock(AcceptPlatformInfo, block)) {
        errMsg += QLatin1String("Failed to send data to server.");
        return false;
    }

    Command cmd = Ack;
    if (!receiveBlock(&cmd, &block) || cmd != Ack) {
        errMsg += QLatin1String("Failed to get response from server.");
        return false;
    }

    return true;
}


bool BaselineProtocol::acceptConnection(PlatformInfo *pi)
{
    errMsg.clear();

    QByteArray block;
    Command cmd = AcceptPlatformInfo;
    if (!receiveBlock(&cmd, &block) || cmd != AcceptPlatformInfo)
        return false;

    if (pi) {
        QDataStream ds(block);
        ds >> *pi;
    }

    if (!sendBlock(Ack, QByteArray()))
        return false;
    return true;
}


bool BaselineProtocol::requestBaseline(const QString &caseId, Command *response, QImage *baseline)
{
    errMsg.clear();
    if (!sendBlock(RequestBaseline, caseId.toLatin1()))
        return false;
    QByteArray imgData;
    Command cmd;
    if (!receiveBlock(&cmd, &imgData))
        return false;
    if (response)
        *response = cmd;
    if (cmd == BaselineNotPresent || cmd == IgnoreCase)
        return true;
    else if (cmd == AcceptBaseline) {
        if (baseline) {
            // hm, get the name also, for checking?
            *baseline = QImage::fromData(imgData);
            if (baseline->isNull()) {
                errMsg.prepend(QLatin1String("Invalid baseline image data received. "));
                return false;
            }
        }
        return true;
    }
    errMsg.prepend(QLatin1String("Unexpected reply from server on baseline request. "));
    return false;
}


bool BaselineProtocol::submitNewBaseline(const QString &caseId, const QImage &baseline)
{
    errMsg.clear();
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << caseId.toLatin1();
    if (!baseline.save(&buf, FileFormat)) {
        errMsg = QLatin1String("Failed to convert new baseline image to ") + QLatin1String(FileFormat);
        return false;
    }
    if (!sendBlock(AcceptNewBaseline, buf.data())) {
        errMsg.prepend(QLatin1String("Failed to submit new baseline to server. "));
        return false;
    }
    Command cmd;
    receiveBlock(&cmd, 0);  // Just wait for the pong; ignore reply contents
    return true;
}


bool BaselineProtocol::submitMismatch(const QString &caseId, const QImage &mismatch, QByteArray *failMsg)
{
    errMsg.clear();
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << caseId.toLatin1();
    if (!mismatch.save(&buf, FileFormat)) {
        errMsg = QLatin1String("Failed to convert mismatched image to ") + QLatin1String(FileFormat);
        return false;
    }
    if (!sendBlock(AcceptMismatch, buf.data())) {
        errMsg.prepend(QLatin1String("Failed to submit mismatched image to server. "));
        return false;
    }
    Command cmd;
    if (!receiveBlock(&cmd, failMsg)) {
        errMsg.prepend(QLatin1String("Failed to receive response on mismatched image from server. "));
        return false;
    }
    return true;
}


bool BaselineProtocol::sendBlock(Command cmd, const QByteArray &block)
{
    QDataStream s(&socket);
    // TBD: set qds version as a constant
    s << quint16(ProtocolVersion) << quint16(cmd);
    s.writeBytes(block.constData(), block.size());
    return true;
}


bool BaselineProtocol::receiveBlock(Command *cmd, QByteArray *block)
{
    while (socket.bytesAvailable() < int(2*sizeof(quint16) + sizeof(quint32))) {
        if (!socket.waitForReadyRead(Timeout))
            return false;
    }
    QDataStream ds(&socket);
    quint16 rcvProtocolVersion, rcvCmd;
    ds >> rcvProtocolVersion >> rcvCmd;
    if (rcvProtocolVersion != ProtocolVersion) {
        // TBD: More resilient handling of this case; the server should accept client's version
        errMsg = QLatin1String("Server protocol version mismatch, received:") + QString::number(rcvProtocolVersion);

        return false;
    }
    if (cmd)
        *cmd = Command(rcvCmd);

    QByteArray uMsg;
    quint32 remaining;
    ds >> remaining;
    uMsg.resize(remaining);
    int got = 0;
    char* uMsgBuf = uMsg.data();
    do {
        got = ds.readRawData(uMsgBuf, remaining);
        remaining -= got;
        uMsgBuf += got;
    } while (remaining && got >= 0 && socket.waitForReadyRead(Timeout));

    if (got < 0)
        return false;

    if (block)
        *block = uMsg;

    return true;
}


QString BaselineProtocol::errorMessage()
{
    QString ret = errMsg;
    if (socket.error() >= 0)
        ret += QLatin1String(" Socket state: ") + socket.errorString();
    return ret;
}
