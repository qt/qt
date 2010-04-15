/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtTest/QtTest>
#include <QtCore/QThread>
#include <QtCore/QSemaphore>
#include <QtCore/QElapsedTimer>
#include <QtNetwork/QTcpSocket>

#include "minihttpserver.h"
#include "../network-settings.h"

#include <qplatformdefs.h>
#ifdef Q_OS_UNIX
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <unistd.h>
#elif defined(Q_OS_WIN)
# include <winsock2.h>
#endif

class tst_QTcpSocket_stresstest : public QObject
{
    Q_OBJECT
public:
    enum { AttemptCount = 100 };
    tst_QTcpSocket_stresstest();
    MiniHttpServer server;

    qint64 byteCounter;

public slots:
    void initTestCase_data();
    void init();

    void slotReadAll() { byteCounter += static_cast<QTcpSocket *>(sender())->readAll().size(); }

private Q_SLOTS:
    void nativeBlockingConnectDisconnect();
    void blockingConnectDisconnect();
    void blockingPipelined();
    void blockingMultipleRequests();
    void connectDisconnect();
    void parallelConnectDisconnect_data();
    void parallelConnectDisconnect();
};

tst_QTcpSocket_stresstest::tst_QTcpSocket_stresstest()
{
#ifdef Q_OS_WIN
    WSAData wsadata;

    // IPv6 requires Winsock v2.0 or better.
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif
}

void tst_QTcpSocket_stresstest::initTestCase_data()
{
    QTest::addColumn<bool>("isLocalhost");
    QTest::addColumn<QString>("hostname");
    QTest::addColumn<int>("port");

    QTest::newRow("localhost") << true << "localhost" << server.port();
    QTest::newRow("remote") << false << QtNetworkSettings::serverName() << 80;
}

void tst_QTcpSocket_stresstest::init()
{
    if (qgetenv("QTCPSOCKET_STRESSTEST").toInt() == 0)
        QSKIP("Stress test disabled", SkipAll);
}

void tst_QTcpSocket_stresstest::nativeBlockingConnectDisconnect()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    double avg = 0;
    for (int i = 0; i < AttemptCount; ++i) {
        QElapsedTimer timeout;
        byteCounter = 0;
        timeout.start();

        // look up the host
#ifdef Q_OS_WIN
        SOCKET fd;
#else
        int fd;
#endif
        {
#if !defined(QT_NO_GETADDRINFO) && 0
            addrinfo *res = 0;
            struct addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = PF_UNSPEC;

            int result = getaddrinfo(QUrl::toAce(hostname).constData(), QByteArray::number(port).constData(), &hints, &res);
            QCOMPARE(result, 0);

            // connect loop
            fd = -1;
            for (addrinfo *node = res; fd == -1 && node; node = node->ai_next) {
                fd = ::socket(node->ai_family, node->ai_socktype, node->ai_protocol);
                if (fd == -1)
                    continue;
                if (::connect(fd, node->ai_addr, node->ai_addrlen) == -1) {
                    ::close(fd);
                    fd = -1;
                } else {
                    break;
                }
            }
            QVERIFY(fd != -1);
#else
            hostent *result = gethostbyname(QUrl::toAce(hostname).constData());
            QVERIFY(result);
            QCOMPARE(result->h_addrtype, AF_INET);
            struct sockaddr_in s;
            QT_SOCKLEN_T len = sizeof s;
            s.sin_family = AF_INET;

            fd = ::socket(AF_INET, SOCK_STREAM, 0);
# ifdef Q_OS_WIN
            QVERIFY(fd != INVALID_SOCKET);

            WSAHtons(fd, port, &(s.sin_port));
            s.sin_addr.s_addr = *(u_long *) result->h_addr_list[0]

            QVERIFY(::connect(fd, (sockaddr*)&s, len) != SOCKET_ERROR);
# else
            QVERIFY(fd != -1);

            s.sin_port = htons(port);
            s.sin_addr =  *(struct in_addr *) result->h_addr_list[0];

            QVERIFY(::connect(fd, (sockaddr*)&s, len) == 0);
#endif
#endif
        }

        // send request
        {
            QByteArray request = "GET /qtest/mediumfile HTTP/1.1\r\n"
                                 "Connection: close\r\n"
                                 "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                                 "Host: " + hostname.toLatin1() + "\r\n"
                                 "\r\n";
            qint64 bytesWritten = 0;
            while (bytesWritten < request.size()) {
                qint64 ret = ::write(fd, request.constData() + bytesWritten, request.size() - bytesWritten);
                if (ret == -1) {
                    ::close(fd);
                    QFAIL("Timeout");
                }
                bytesWritten += ret;
            }
        }

        // receive reply
        char buf[16384];
        while (true) {
            qint64 ret = ::read(fd, buf, sizeof buf);
            if (ret == -1) {
                ::close(fd);
                QFAIL("Timeout");
            } else if (ret == 0) {
                break; // EOF
            }
            byteCounter += ret;
        }
        ::close(fd);

        double rate = (byteCounter / timeout.elapsed());
        avg = (i * avg + rate) / (i + 1);
        qDebug() << i << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                << (rate / 1024.0 / 1024 * 1000) << "MB/s";
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

void tst_QTcpSocket_stresstest::blockingConnectDisconnect()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    double avg = 0;
    for (int i = 0; i < AttemptCount; ++i) {
        QElapsedTimer timeout;
        byteCounter = 0;
        timeout.start();

        QTcpSocket socket;
        socket.connectToHost(hostname, port);
        QVERIFY2(socket.waitForConnected(), "Timeout");

        socket.write("GET /qtest/mediumfile HTTP/1.1\r\n"
                     "Connection: close\r\n"
                     "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                     "Host: " + hostname.toLatin1() + "\r\n"
                     "\r\n");
        while (socket.bytesToWrite())
            QVERIFY2(socket.waitForBytesWritten(), "Timeout");

        while (socket.state() == QAbstractSocket::ConnectedState && !timeout.hasExpired(10000)) {
            socket.waitForReadyRead();
            byteCounter += socket.readAll().size(); // discard
        }
        double rate = (byteCounter / timeout.elapsed());
        avg = (i * avg + rate) / (i + 1);
        qDebug() << i << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                << (rate / 1024.0 / 1024 * 1000) << "MB/s";
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

void tst_QTcpSocket_stresstest::blockingPipelined()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    double avg = 0;
    for (int i = 0; i < AttemptCount / 2; ++i) {
        QElapsedTimer timeout;
        byteCounter = 0;
        timeout.start();

        QTcpSocket socket;
        socket.connectToHost(hostname, port);
        QVERIFY2(socket.waitForConnected(), "Timeout");

        for (int j = 0 ; j < 3; ++j) {
            qDebug("Attempt %d%c", i, 'a' + j);
            socket.write("GET /qtest/mediumfile HTTP/1.1\r\n"
                         "Connection: " + QByteArray(j == 2 ? "close" : "keep-alive") + "\r\n"
                         "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                         "Host: " + hostname.toLatin1() + "\r\n"
                         "\r\n");
            while (socket.bytesToWrite())
                QVERIFY2(socket.waitForBytesWritten(), "Timeout");
        }

        while (socket.state() == QAbstractSocket::ConnectedState && !timeout.hasExpired(10000)) {
            socket.waitForReadyRead();
            byteCounter += socket.readAll().size(); // discard
        }

        double rate = (byteCounter / timeout.elapsed());
        avg = (i * avg + rate) / (i + 1);
        qDebug() << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                << (rate / 1024.0 / 1024 * 1000) << "MB/s";
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

void tst_QTcpSocket_stresstest::blockingMultipleRequests()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    double avg = 0;
    for (int i = 0; i < AttemptCount / 5; ++i) {
        QTcpSocket socket;
        socket.connectToHost(hostname, port);
        QVERIFY2(socket.waitForConnected(), "Timeout");

        for (int j = 0 ; j < 5; ++j) {
            QElapsedTimer timeout;
            byteCounter = 0;
            timeout.start();

            socket.write("GET /qtest/mediumfile HTTP/1.1\r\n"
                         "Connection: keep-alive\r\n"
                         "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                         "Host: " + hostname.toLatin1() + "\r\n"
                         "\r\n");
            while (socket.bytesToWrite())
                QVERIFY2(socket.waitForBytesWritten(), "Timeout");

            qint64 bytesRead = 0;
            qint64 bytesExpected = -1;
            QByteArray buffer;
            while (socket.state() == QAbstractSocket::ConnectedState && !timeout.hasExpired(10000)) {
                socket.waitForReadyRead();
                buffer += socket.readAll();
                byteCounter += buffer.size();
                int pos = buffer.indexOf("\r\n\r\n");
                if (pos == -1)
                    continue;

                bytesRead = buffer.length() - pos - 4;

                buffer.truncate(pos + 2);
                buffer = buffer.toLower();
                pos = buffer.indexOf("\r\ncontent-length: ");
                if (pos == -1) {
                    qWarning() << "no content-length:" << QString(buffer);
                    break;
                }
                pos += strlen("\r\ncontent-length: ");

                int eol = buffer.indexOf("\r\n", pos + 2);
                if (eol == -1) {
                    qWarning() << "invalid header";
                    break;
                }

                bytesExpected = buffer.mid(pos, eol - pos).toLongLong();
                break;
            }
            QVERIFY(bytesExpected > 0);
            QVERIFY2(!timeout.hasExpired(10000), "Timeout");

            while (socket.state() == QAbstractSocket::ConnectedState && !timeout.hasExpired(10000) && bytesExpected > bytesRead) {
                socket.waitForReadyRead();
                int blocklen = socket.read(bytesExpected - bytesRead).length(); // discard
                bytesRead += blocklen;
                byteCounter += blocklen;
            }
            QVERIFY2(!timeout.hasExpired(10000), "Timeout");
            QCOMPARE(bytesRead, bytesExpected);

            double rate = (byteCounter / timeout.elapsed());
            avg = (i * avg + rate) / (i + 1);
            qDebug() << i * 5 + j << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                    << (rate / 1024.0 / 1024 * 1000) << "MB/s";
        }

        socket.disconnectFromHost();
        QVERIFY(socket.state() == QAbstractSocket::UnconnectedState);
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

void tst_QTcpSocket_stresstest::connectDisconnect()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);

    double avg = 0;
    for (int i = 0; i < AttemptCount; ++i) {
        QElapsedTimer timeout;
        byteCounter = 0;
        timeout.start();

        QTcpSocket socket;
        socket.connectToHost(hostname, port);

        socket.write("GET /qtest/mediumfile HTTP/1.1\r\n"
                     "Connection: close\r\n"
                     "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                     "Host: " + hostname.toLatin1() + "\r\n"
                     "\r\n");
        connect(&socket, SIGNAL(readyRead()), SLOT(slotReadAll()));

        QTestEventLoop::instance().connect(&socket, SIGNAL(disconnected()), SLOT(exitLoop()));
        QTestEventLoop::instance().enterLoop(30);
        QVERIFY2(!QTestEventLoop::instance().timeout(), "Timeout");
        double rate = (byteCounter / timeout.elapsed());
        avg = (i * avg + rate) / (i + 1);
        qDebug() << i << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                << (rate / 1024.0 / 1024 * 1000) << "MB/s";
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

void tst_QTcpSocket_stresstest::parallelConnectDisconnect_data()
{
    QTest::addColumn<int>("parallelAttempts");
    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("4") << 4;
    QTest::newRow("5") << 5;
    QTest::newRow("6") << 6;
    QTest::newRow("8") << 8;
    QTest::newRow("10") << 10;
    QTest::newRow("25") << 25;
    QTest::newRow("100") << 100;
    QTest::newRow("500") << 500;
}

void tst_QTcpSocket_stresstest::parallelConnectDisconnect()
{
    QFETCH_GLOBAL(QString, hostname);
    QFETCH_GLOBAL(int, port);
    QFETCH(int, parallelAttempts);

    if (parallelAttempts > 100) {
        QFETCH_GLOBAL(bool, isLocalhost);
        if (!isLocalhost)
            QSKIP("Localhost-only test", SkipSingle);
    }

    double avg = 0;
    for (int i = 0; i < qMax(2, AttemptCount/qMax(2, parallelAttempts/4)); ++i) {
        QElapsedTimer timeout;
        byteCounter = 0;
        timeout.start();

        QTcpSocket *socket = new QTcpSocket[parallelAttempts];
        for (int j = 0; j < parallelAttempts; ++j) {
            socket[j].connectToHost(hostname, port);

            socket[j].write("GET /qtest/mediumfile HTTP/1.1\r\n"
                            "Connection: close\r\n"
                            "User-Agent: tst_QTcpSocket_stresstest/1.0\r\n"
                            "Host: " + hostname.toLatin1() + "\r\n"
                            "\r\n");
            connect(&socket[j], SIGNAL(readyRead()), SLOT(slotReadAll()));

            QTestEventLoop::instance().connect(&socket[j], SIGNAL(disconnected()), SLOT(exitLoop()));
        }

        while (!timeout.hasExpired(30000)) {
            QTestEventLoop::instance().enterLoop(10);
            int done = 0;
            for (int j = 0; j < parallelAttempts; ++j)
                done += socket[j].state() == QAbstractSocket::UnconnectedState ? 1 : 0;
            if (done == parallelAttempts)
                break;
        }
        delete[] socket;
        QVERIFY2(!timeout.hasExpired(30000), "Timeout");
        double rate = (byteCounter / timeout.elapsed());
        avg = (i * avg + rate) / (i + 1);
        qDebug() << i << byteCounter << "bytes in" << timeout.elapsed() << "ms:"
                << (rate / 1024.0 / 1024 * 1000) << "MB/s";
    }
    qDebug() << "Average transfer rate was" << (avg / 1024.0 / 1024 * 1000) << "MB/s";
}

QTEST_MAIN(tst_QTcpSocket_stresstest);

#include "tst_qtcpsocket_stresstest.moc"
