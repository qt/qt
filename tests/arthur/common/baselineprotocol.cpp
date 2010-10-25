#include "baselineprotocol.h"
#include <QLibraryInfo>
#include <QImage>
#include <QBuffer>
#include <QHostInfo>
#include <QSysInfo>
#include <QProcess>

PlatformInfo::PlatformInfo(bool useLocal)
{
    if (useLocal) {
        buildKey = QLibraryInfo::buildKey();
        qtVersion = QLatin1String(qVersion());
        hostName = QHostInfo::localHostName();
        osVersion = -1;
#if defined(Q_OS_LINUX)
        osName = QLatin1String("Linux");
#elif defined(Q_OS_WINCE)
        osName = QLatin1String("WinCE");
        osVersion = QSysInfo::windowsVersion();
#elif defined(Q_OS_WIN)
        osName = QLatin1String("Windows");
        osVersion = QSysInfo::windowsVersion();
#elif defined(Q_OS_MAC)
        osName = QLatin1String("MacOS");
        osVersion = qMacVersion();
#elif defined(Q_OS_SYMBIAN)
        osName = QLatin1String("Symbian");
        osVersion = QSysInfo::symbianVersion();
#else
        osName = QLatin1String("Other");
#endif

        QProcess git;
        git.start(QLS("git"), QStringList() << QLS("log") << QLS("--max-count=1") << QLS("--pretty=%H"));
        git.waitForFinished(3000);
        if (!git.exitCode())
            gitCommit = QString::fromLocal8Bit(git.readAllStandardOutput().constData()).trimmed();
        else
            gitCommit = QLS("Unknown");
    }
}

QDataStream & operator<< (QDataStream &stream, const PlatformInfo &p)
{
    stream << p.hostName << p.osName << p.osVersion << p.qtVersion << p.buildKey << p.gitCommit;
    return stream;
}

QDataStream & operator>> (QDataStream& stream, PlatformInfo& p)
{
    stream >> p.hostName >> p.osName >> p.osVersion >> p.qtVersion >> p.buildKey >> p.gitCommit;
    return stream;
}

ImageItem &ImageItem::operator=(const ImageItem &other)
{
    scriptName = other.scriptName;
    scriptChecksum = other.scriptChecksum;
    status = other.status;
    renderFormat = other.renderFormat;
    engine = other.engine;
    image = other.image;
    imageChecksums = other.imageChecksums;
    return *this;
}

// Defined in lookup3.c:
void hashword2 (
const quint32 *k,         /* the key, an array of quint32 values */
size_t         length,    /* the length of the key, in quint32s */
quint32       *pc,        /* IN: seed OUT: primary hash value */
quint32       *pb);       /* IN: more seed OUT: secondary hash value */

quint64 ImageItem::computeChecksum(const QImage &image)
{
    QImage img(image);
    const int bpl = img.bytesPerLine();
    const int padBytes = bpl - (img.width() * img.depth() / 8);
    if (padBytes) {
        uchar *p = img.bits() + bpl - padBytes;
        const int h = img.height();
        for (int y = 0; y < h; ++y) {
            qMemSet(p, 0, padBytes);
            p += bpl;
        }
    }
    if (img.format() == QImage::Format_RGB32) {    // Thank you, Haavard
        quint32 *p = (quint32 *)img.bits();
        const quint32 *end = p + (img.byteCount()/4);
        while (p<end)
            *p++ &= RGB_MASK;
    }

    quint32 h1 = 0xfeedbacc;
    quint32 h2 = 0x21604894;
    hashword2((const quint32 *)img.constBits(), img.byteCount()/4, &h1, &h2);
    return (quint64(h1) << 32) | h2;
}

QString ImageItem::engineAsString() const
{
    switch (engine) {
    case Raster:
        return QLatin1String("Raster");
        break;
    case OpenGL:
        return QLatin1String("OpenGL");
        break;
    default:
        break;
    }
    return QLatin1String("Unknown");
}

QString ImageItem::formatAsString() const
{
    static const int numFormats = 16;
    static const char *formatNames[numFormats] = {
        "Invalid",
        "Mono",
        "MonoLSB",
        "Indexed8",
        "RGB32",
        "ARGB32",
        "ARGB32-Premult",
        "RGB16",
        "ARGB8565-Premult",
        "RGB666",
        "ARGB6666-Premult",
        "RGB555",
        "ARGB8555-Premult",
        "RGB888",
        "RGB444",
        "ARGB4444-Premult"
    };
    if (renderFormat < 0 || renderFormat >= numFormats)
        return QLatin1String("UnknownFormat");
    return QLatin1String(formatNames[renderFormat]);
}

QDataStream & operator<< (QDataStream &stream, const ImageItem &ii)
{
    stream << ii.scriptName << ii.scriptChecksum << quint8(ii.status) << quint8(ii.renderFormat)
           << quint8(ii.engine) << ii.image << ii.imageChecksums;
    return stream;
}

QDataStream & operator>> (QDataStream &stream, ImageItem &ii)
{
    quint8 encFormat, encStatus, encEngine;
    stream >> ii.scriptName >> ii.scriptChecksum >> encStatus >> encFormat
           >> encEngine >> ii.image >> ii.imageChecksums;
    ii.renderFormat = QImage::Format(encFormat);
    ii.status = ImageItem::ItemStatus(encStatus);
    ii.engine = ImageItem::GraphicsEngine(encEngine);
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
    QByteArray serverName(qgetenv("QT_LANCELOT_SERVER"));
    if (serverName.isNull())
        serverName = "lancelot.test.qt.nokia.com";

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


bool BaselineProtocol::requestBaselineChecksums(ImageItemList *itemList)
{
    errMsg.clear();
    if (!itemList)
        return false;
    QByteArray block;
    QDataStream ds(&block, QIODevice::ReadWrite);
    ds << *itemList;
    if (!sendBlock(RequestBaselineChecksums, block))
        return false;
    Command cmd;
    if (!receiveBlock(&cmd, &block))
        return false;
    ds.device()->seek(0);
    ds >> *itemList;
    return true;
}


bool BaselineProtocol::submitNewBaseline(const ImageItem &item, QByteArray *serverMsg)
{
    Command cmd;
    return (sendItem(AcceptNewBaseline, item) && receiveBlock(&cmd, serverMsg) && cmd == Ack);
}


bool BaselineProtocol::submitMismatch(const ImageItem &item, QByteArray *serverMsg)
{
    Command cmd;
    return (sendItem(AcceptMismatch, item) && receiveBlock(&cmd, serverMsg) && cmd == Ack);
}


bool BaselineProtocol::sendItem(Command cmd, const ImageItem &item)
{
    errMsg.clear();
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << item;
    if (!sendBlock(cmd, buf.data())) {
        errMsg.prepend(QLatin1String("Failed to submit image to server. "));
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
