#include "baselineserver.h"
#include <QBuffer>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QFileInfo>
#include <QHostInfo>
#include <QTextStream>

QString BaselineServer::storage;

BaselineServer::BaselineServer(QObject *parent)
    : QTcpServer(parent)
{
    QFileInfo me(QCoreApplication::applicationFilePath());
    meLastMod = me.lastModified();
    heartbeatTimer = new QTimer(this);
    connect(heartbeatTimer, SIGNAL(timeout()), this, SLOT(heartbeat()));
    heartbeatTimer->start(HEARTBEAT*1000);
}

QString BaselineServer::storagePath()
{
    if (storage.isEmpty()) {
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cdUp();
        storage =  dir.path() + QLatin1String("/storage/");
    }
    return storage;
}

void BaselineServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "Server: New connection!";
    BaselineThread *thread = new BaselineThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

void BaselineServer::heartbeat()
{
    // The idea is to exit to be restarted when modified, as soon as not actually serving
    QFileInfo me(QCoreApplication::applicationFilePath());
    if (me.lastModified() == meLastMod)
        return;

    // (could close() here to avoid accepting new connections, to avoid livelock)
    // also, could check for a timeout to force exit, to avoid hung threads blocking
    bool isServing = false;
    foreach(BaselineThread *thread, findChildren<BaselineThread *>()) {
        if (thread->isRunning()) {
            isServing = true;
            break;
        }
    }

    if (!isServing)
        QCoreApplication::exit();
}

BaselineThread::BaselineThread(int socketDescriptor, QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{
}

void BaselineThread::run()
{
    BaselineHandler handler(socketDescriptor);
    exec();
}


BaselineHandler::BaselineHandler(int socketDescriptor)
    : QObject(), connectionEstablished(false)
{
    runId = QDateTime::currentDateTime().toString(QLatin1String("MMMdd-hhmmss"));

    connect(&proto.socket, SIGNAL(readyRead()), this, SLOT(receiveRequest()));
    connect(&proto.socket, SIGNAL(disconnected()), this, SLOT(receiveDisconnect()));
    proto.socket.setSocketDescriptor(socketDescriptor);
}

QString BaselineHandler::logtime()
{
    return QTime::currentTime().toString(QLatin1String("mm:ss.zzz"));
}

void BaselineHandler::receiveRequest()
{
    if (!connectionEstablished) {
        if (!proto.acceptConnection(&plat)) {
            qWarning() << runId << logtime() << "Accepting new connection failed. " << proto.errorMessage();
            QThread::currentThread()->exit(1);
            return;
        }
        connectionEstablished = true;
        qDebug() << runId << logtime() << "Connection established with" << plat.hostname << proto.socket.peerAddress().toString() << "Qt version:" << plat.qtVersion << plat.buildKey;
        return;
    }

    QByteArray block;
    BaselineProtocol::Command cmd;
    if (!proto.receiveBlock(&cmd, &block)) {
        qWarning() << runId << logtime() << "Command reception failed. "<< proto.errorMessage();
        QThread::currentThread()->exit(1);
        return;
    }

    switch(cmd) {
    case BaselineProtocol::RequestBaselineChecksums:
        provideBaselineChecksums(block);
        break;
    case BaselineProtocol::AcceptNewBaseline:
        storeImage(block, true);
        break;
    case BaselineProtocol::AcceptMismatch:
        storeImage(block, false);
        break;
    default:
        qWarning() << runId << logtime() << "Unknown command received. " << proto.errorMessage();
        proto.sendBlock(BaselineProtocol::UnknownError, QByteArray());
    }
}


void BaselineHandler::provideBaselineChecksums(const QByteArray &itemListBlock)
{
    ImageItemList itemList;
    QDataStream ds(itemListBlock);
    ds >> itemList;
    qDebug() << runId << logtime() << "Received request for checksums for" << itemList.count() << "items";

    for (ImageItemList::iterator i = itemList.begin(); i != itemList.end(); ++i) {
        if (i->scriptName.startsWith(QLatin1String("porter_duff"))) {
            // Example of blacklisting on server.
            i->status = ImageItem::IgnoreItem;
            continue;
        }
        i->imageChecksums.clear();
        QString prefix = pathForItem(*i, true);
        QFile file(prefix + QLatin1String("metadata"));
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream checkSums(&file);
            checkSums >> i->imageChecksums;
            file.close();
            i->status = ImageItem::Ok;
        }
        if (!i->imageChecksums.count())
            i->status = ImageItem::BaselineNotFound;
    }

    QByteArray block;
    QDataStream ods(&block, QIODevice::WriteOnly);
    ods << itemList;
    proto.sendBlock(BaselineProtocol::Ack, block);
}


void BaselineHandler::storeImage(const QByteArray &itemBlock, bool isBaseline)
{
    QDataStream ds(itemBlock);
    ImageItem item;
    ds >> item;

    QString prefix = pathForItem(item, isBaseline);
    qDebug() << runId << logtime() << "Received" << (isBaseline ? "baseline" : "mismatched") << "image for:" << item.scriptName << "Storing in" << prefix;

    QString dir = prefix.section(QDir::separator(), 0, -2);
    QDir cwd;
    if (!cwd.exists(dir))
        cwd.mkpath(dir);
    item.image.save(prefix + QLatin1String(FileFormat), FileFormat);

    //# Could use QSettings or XML or even DB, could use common file for whole dir or even whole storage - but for now, keep it simple
    QFile file(prefix + QLatin1String("metadata"));
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream checkSums(&file);
    checkSums << item.imageChecksums;
    file.close();

    QByteArray msg(isBaseline ? "Baseline" : "Mismatching" );
    msg += " image stored in "
           + QHostInfo::localHostName().toLatin1() + '.'
           + QHostInfo::localDomainName().toLatin1() + ':'
           + QFileInfo(file).absoluteFilePath().toLatin1();
    proto.sendBlock(BaselineProtocol::Ack, msg);
}


void BaselineHandler::receiveDisconnect()
{
    qDebug() << runId << logtime() << "Client disconnected.";
    QThread::currentThread()->exit(0);
}


QString BaselineHandler::pathForItem(const ImageItem &item, bool isBaseline)
{
    if (pathForRun.isNull()) {
        QString host = plat.hostname;
        if (host == QLatin1String("localhost"))
            host = proto.socket.peerAddress().toString();
        else
            host.replace(QRegExp(QLatin1String("^(bq|oslo)-(.*)-\\d\\d$")), QLatin1String("vm-\\2"));
        pathForRun = BaselineServer::storagePath() + host + QLatin1Char('/');
    }

    QString storePath = pathForRun;
    if (isBaseline)
        storePath += QString(QLatin1String("baselines_%1_%2/")).arg(item.engineAsString(), item.formatAsString());
    else
        storePath += runId + QLatin1Char('/');

    QString itemName = item.scriptName;
    if (itemName.contains(QLatin1Char('.')))
        itemName.replace(itemName.lastIndexOf(QLatin1Char('.')), 1, QLatin1Char('_'));

    return storePath + itemName + QLatin1Char('.');
}

QString BaselineHandler::computeMismatchScore(const QImage &baseline, const QImage &rendered)
{
    if (baseline.size() != rendered.size() || baseline.format() != rendered.format())
        return QLatin1String("[No score, incomparable images.]");
    if (baseline.depth() != 32)
        return QLatin1String("[Score computation not implemented for format.]");

    int w = baseline.width();
    int h = baseline.height();

    uint ncd = 0; // number of differing color pixels
    uint nad = 0; // number of differing alpha pixels
    uint scd = 0; // sum of color pixel difference
    uint sad = 0; // sum of alpha pixel difference

    for (int y=0; y<h; ++y) {
        const QRgb *bl = (const QRgb *) baseline.constScanLine(y);
        const QRgb *rl = (const QRgb *) rendered.constScanLine(y);
        for (int x=0; x<w; ++x) {
            QRgb b = bl[x];
            QRgb r = rl[x];
            if (r != b) {
                int dr = qAbs(qRed(b) - qRed(r));
                int dg = qAbs(qGreen(b) - qGreen(r));
                int db = qAbs(qBlue(b) - qBlue(r));
                int ds = dr + dg + db;
                int da = qAbs(qAlpha(b) - qAlpha(r));
                if (ds) {
                    ncd++;
                    scd += ds;
                }
                if (da) {
                    nad++;
                    sad += da;
                }
            }
        }
    }

    double pcd = 100.0 * ncd / (w*h);  // percent of pixels that differ
    double acd = ncd ? double(scd) / (3*ncd) : 0;         // avg. difference
    QString res = QString(QLatin1String("Diffscore: %1% (Num:%2 Avg:%3)")).arg(pcd, 0, 'g', 2).arg(ncd).arg(acd, 0, 'g', 2);
    if (baseline.hasAlphaChannel()) {
        double pad = 100.0 * nad / (w*h);  // percent of pixels that differ
        double aad = nad ? double(sad) / (3*nad) : 0;         // avg. difference
        res += QString(QLatin1String(" Alpha-diffscore: %1% (Num:%2 Avg:%3)")).arg(pad, 0, 'g', 2).arg(nad).arg(aad, 0, 'g', 2);
    }
    return res;
}
