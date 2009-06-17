/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QPACKETPROTOCOL_H
#define QPACKETPROTOCOL_H

#include <QtCore/qobject.h>
#include <QtCore/qdatastream.h>

class QIODevice;
class QBuffer;
class QPacket;
class QPacketAutoSend;
class QPacketProtocolPrivate;

class Q_DECLARATIVE_EXPORT QPacketProtocol : public QObject
{
Q_OBJECT
public:
    explicit QPacketProtocol(QIODevice * dev, QObject * parent = 0);
    virtual ~QPacketProtocol();

    qint32 maximumPacketSize() const;
    qint32 setMaximumPacketSize(qint32);

    QPacketAutoSend send();
    void send(const QPacket &);

    qint64 packetsAvailable() const;
    QPacket read();

    void clear();

    QIODevice * device();

signals:
    void readyRead();
    void invalidPacket();
    void packetWritten();

private:
    QPacketProtocolPrivate * d;
};


class Q_DECLARATIVE_EXPORT QPacket : public QDataStream
{
public:
    QPacket();
    QPacket(const QPacket &);
    virtual ~QPacket();

    void clear();
    bool isEmpty() const;

protected:
    friend class QPacketProtocol;
    QPacket(const QByteArray & ba);
    QByteArray b;
    QBuffer * buf;
};

class Q_DECLARATIVE_EXPORT QPacketAutoSend : public QPacket
{
public:
    virtual ~QPacketAutoSend();

private:
    friend class QPacketProtocol;
    QPacketAutoSend(QPacketProtocol *);
    QPacketProtocol * p;
};

#endif
