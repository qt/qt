#ifndef BASELINEPROTOCOL_H
#define BASELINEPROTOCOL_H

#include <QDataStream>
#include <QTcpSocket>
#include <QImage>
#include <QVector>

#define FileFormat "png"

struct PlatformInfo
{
    PlatformInfo(bool useLocal = false);

    QString buildKey;
    QString qtVersion;
    QString hostname;
};
QDataStream & operator<< (QDataStream &stream, const PlatformInfo &pinfo);
QDataStream & operator>> (QDataStream& stream, PlatformInfo& pinfo);

struct ImageItem
{
public:
    ImageItem()
        : status(Ok), renderFormat(QImage::Format_Invalid), engine(Raster), imageChecksum(0), scriptChecksum(0)
    {}
    ImageItem(const ImageItem &other)
    { *this = other; }
    ~ImageItem()
    {}
    ImageItem &operator=(const ImageItem &other);
    static quint64 computeChecksum(const QImage& image);
    QString engineAsString();

    enum ItemStatus {
        Ok = 0,
        BaselineNotFound = 1,
        IgnoreItem = 2
    };

    enum GraphicsEngine {
        Raster = 0,
        OpenGL = 1
    };

    QString scriptName;
    ItemStatus status;
    QImage::Format renderFormat;
    GraphicsEngine engine;
    QImage image;
    quint64 imageChecksum;
    // tbd: add diffscore
    quint16 scriptChecksum;
};
QDataStream & operator<< (QDataStream &stream, const ImageItem &ii);
QDataStream & operator>> (QDataStream& stream, ImageItem& ii);

Q_DECLARE_METATYPE(ImageItem);

typedef QVector<ImageItem> ImageItemList;


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
        Timeout = 10000
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
    };

    // For client:
    bool connect();
    bool requestBaselineChecksums(ImageItemList *itemList);
    bool submitNewBaseline(const ImageItem &item, QByteArray *serverMsg);
    bool submitMismatch(const ImageItem &item, QByteArray *serverMsg);

    // For server:
    bool acceptConnection(PlatformInfo *pi);

    QString errorMessage();

private:
    bool sendItem(Command cmd, const ImageItem &item);

    bool sendBlock(Command cmd, const QByteArray &block);
    bool receiveBlock(Command *cmd, QByteArray *block);
    QString errMsg;
    QTcpSocket socket;

    friend class BaselineThread;
    friend class BaselineHandler;
};





#endif // BASELINEPROTOCOL_H
