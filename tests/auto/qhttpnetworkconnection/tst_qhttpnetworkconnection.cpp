/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include "private/qhttpnetworkconnection_p.h"
#include <QAuthenticator>

#include "../network-settings.h"

class tst_QHttpNetworkConnection: public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void finishedReply();
    void finishedWithError(QNetworkReply::NetworkError errorCode, const QString &detail);
    void challenge401(const QHttpNetworkRequest &request, QAuthenticator *authenticator, const QHttpNetworkConnection *connection);
#ifndef QT_NO_OPENSSL
    void sslErrors(const QList<QSslError> &errors);
#endif
private:
    bool finishedCalled;
    bool finishedWithErrorCalled;
    QNetworkReply::NetworkError netErrorCode;

private Q_SLOTS:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void options_data();
    void options();
    void get_data();
    void get();
    void head_data();
    void head();
    void post_data();
    void post();
    void put_data();
    void put();
    void _delete_data();
    void _delete();
    void trace_data();
    void trace();
    void _connect_data();
    void _connect();
#ifndef QT_NO_COMPRESS
    void compression_data();
    void compression();
#endif
#ifndef QT_NO_OPENSSL
    void ignoresslerror_data();
    void ignoresslerror();
#endif
#ifdef QT_NO_OPENSSL
    void nossl_data();
    void nossl();
#endif
    void get401_data();
    void get401();

};


void tst_QHttpNetworkConnection::initTestCase()
{
}

void tst_QHttpNetworkConnection::cleanupTestCase()
{
}

void tst_QHttpNetworkConnection::init()
{
}

void tst_QHttpNetworkConnection::cleanup()
{
}

void tst_QHttpNetworkConnection::options_data()
{
    // not tested yet
}

void tst_QHttpNetworkConnection::options()
{
    QEXPECT_FAIL("", "not tested yet", Continue);
    QVERIFY(false);
}

void tst_QHttpNetworkConnection::head_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("statusString");
    QTest::addColumn<int>("contentLength");

    QTest::newRow("success-internal") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfc3252.txt" << ushort(80) << false << 200 << "OK" << 25962;
    QTest::newRow("success-external") << "http://" << "www.ietf.org" << "/rfc/rfc3252.txt" << ushort(80) << false << 200 << "OK" << 25962;

    QTest::newRow("failure-path") << "http://" << QtNetworkSettings::serverName() << "/t" << ushort(80) << false << 404 << "Not Found" << -1;
    QTest::newRow("failure-protocol") << "" << QtNetworkSettings::serverName() << "/qtest/rfc3252.txt" << ushort(80) << false << 400 << "Bad Request" << -1;
}

void tst_QHttpNetworkConnection::head()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(int, statusCode);
    QFETCH(QString, statusString);
    QFETCH(int, contentLength);

    QHttpNetworkConnection connection(host);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);

    QHttpNetworkRequest request(protocol + host + path, QHttpNetworkRequest::Head);
    QHttpNetworkReply *reply = connection.sendRequest(request);

    QTime stopWatch;
    stopWatch.start();
    do {
        QCoreApplication::instance()->processEvents();
        if (stopWatch.elapsed() >= 30000)
            break;
    } while (!reply->isFinished());

    QCOMPARE(reply->statusCode(), statusCode);
    QCOMPARE(reply->reasonPhrase(), statusString);
    QCOMPARE(reply->contentLength(), qint64(contentLength));

    QVERIFY(reply->isFinished());

    delete reply;
}

void tst_QHttpNetworkConnection::get_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("statusString");
    QTest::addColumn<int>("contentLength");
    QTest::addColumn<int>("downloadSize");

    QTest::newRow("success-internal") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfc3252.txt" << ushort(80) << false << 200 << "OK" << 25962 << 25962;
    QTest::newRow("success-external") << "http://" << "www.ietf.org" << "/rfc/rfc3252.txt" << ushort(80) << false << 200 << "OK" << 25962 << 25962;

    QTest::newRow("failure-path") << "http://" << QtNetworkSettings::serverName() << "/t" << ushort(80) << false << 404 << "Not Found" << -1 << 1023;
    QTest::newRow("failure-protocol") << "" << QtNetworkSettings::serverName() << "/qtest/rfc3252.txt" << ushort(80) << false << 400 << "Bad Request" << -1 << 956;
}

void tst_QHttpNetworkConnection::get()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(int, statusCode);
    QFETCH(QString, statusString);
    QFETCH(int, contentLength);
    QFETCH(int, downloadSize);

    QHttpNetworkConnection connection(host);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);

    QHttpNetworkRequest request(protocol + host + path);
    QHttpNetworkReply *reply = connection.sendRequest(request);

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }

    QCOMPARE(reply->statusCode(), statusCode);
    QCOMPARE(reply->reasonPhrase(), statusString);
    QCOMPARE(reply->contentLength(), qint64(contentLength));

    stopWatch.start();
    QByteArray ba;
    do {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            ba += reply->read();
        if (stopWatch.elapsed() >= 30000)
            break;
    } while (!reply->isFinished());

    QVERIFY(reply->isFinished());
    QCOMPARE(ba.size(), downloadSize);

    delete reply;
}

void tst_QHttpNetworkConnection::finishedReply()
{
    finishedCalled = true;
}

void tst_QHttpNetworkConnection::finishedWithError(QNetworkReply::NetworkError errorCode, const QString &detail)
{
    Q_UNUSED(detail)
    finishedWithErrorCalled = true;
    netErrorCode = errorCode;
}

void tst_QHttpNetworkConnection::put_data()
{

    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<QString>("data");
    QTest::addColumn<bool>("succeed");

    QTest::newRow("success-internal") << "http://" << QtNetworkSettings::serverName() << "/dav/file1.txt" << ushort(80) << false << "Hello World\nEnd of file\n"<<true;
    QTest::newRow("fail-internal") << "http://" << QtNetworkSettings::serverName() << "/dav2/file1.txt" << ushort(80) << false << "Hello World\nEnd of file\n"<<false;
    QTest::newRow("fail-host") << "http://" << "fluke-nosuchhost.troll.no" << "/dav2/file1.txt" << ushort(80) << false << "Hello World\nEnd of file\n"<<false;
}

void tst_QHttpNetworkConnection::put()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(QString, data);
    QFETCH(bool, succeed);

    QHttpNetworkConnection connection(host);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);

    QHttpNetworkRequest request(protocol + host + path, QHttpNetworkRequest::Put);

    QByteArray array = data.toLatin1();
    QBuffer buffer(&array);
    request.setData(&buffer);

    finishedCalled = false;
    finishedWithErrorCalled = false;

    QHttpNetworkReply *reply = connection.sendRequest(request);
    connect(reply, SIGNAL(finished()), SLOT(finishedReply()));
    connect(reply, SIGNAL(finishedWithError(QNetworkReply::NetworkError, const QString &)),
        SLOT(finishedWithError(QNetworkReply::NetworkError, const QString &)));
    connect(&connection, SIGNAL(error(QNetworkReply::NetworkError, const QString &)),
        SLOT(finishedWithError(QNetworkReply::NetworkError, const QString &)));

    QTime stopWatch;
    stopWatch.start();
    do {
        QCoreApplication::instance()->processEvents();
        if (stopWatch.elapsed() >= 30000)
            break;
    } while (!reply->isFinished() && !finishedCalled && !finishedWithErrorCalled);

    if (reply->isFinished()) {
        QByteArray ba;
        ba += reply->read();
    } else if(finishedWithErrorCalled) {
        if(!succeed) {
            delete reply;
            return;
        } else {
            QFAIL("Error in PUT");
        }
    } else {
        QFAIL("PUT timed out");
    }

    int status = reply->statusCode();
    if (status != 200 && status != 201 && status != 204) {
        if (succeed) {
            qDebug()<<"PUT failed, Status Code:" <<status;
            QFAIL("Error in PUT");
        }
    } else {
        if (!succeed) {
            qDebug()<<"PUT Should fail, Status Code:" <<status;
            QFAIL("Error in PUT");
        }
    }
    delete reply;
}

void tst_QHttpNetworkConnection::post_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<QString>("data");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("statusString");
    QTest::addColumn<int>("contentLength");
    QTest::addColumn<int>("downloadSize");

    QTest::newRow("success-internal") << "http://" << QtNetworkSettings::serverName() << "/cgi-bin/echo.cgi" << ushort(80) << false << "7 bytes" << 200 << "OK" << 7 << 7;
    QTest::newRow("failure-internal") << "http://" << QtNetworkSettings::serverName() << "/t" << ushort(80) << false << "Hello World" << 404 << "Not Found" << -1 << 1023;
}

void tst_QHttpNetworkConnection::post()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(QString, data);
    QFETCH(int, statusCode);
    QFETCH(QString, statusString);
    QFETCH(int, contentLength);
    QFETCH(int, downloadSize);

    QHttpNetworkConnection connection(host);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);

    QHttpNetworkRequest request(protocol + host + path, QHttpNetworkRequest::Post);

    QByteArray array = data.toLatin1();
    QBuffer buffer(&array);
    request.setData(&buffer);

    QHttpNetworkReply *reply = connection.sendRequest(request);

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }

    QCOMPARE(reply->statusCode(), statusCode);
    QCOMPARE(reply->reasonPhrase(), statusString);
    QCOMPARE(reply->contentLength(), qint64(contentLength));

    stopWatch.start();
    QByteArray ba;
    do {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            ba += reply->read();
        if (stopWatch.elapsed() >= 30000)
            break;
    } while (!reply->isFinished());

    QVERIFY(reply->isFinished());
    QCOMPARE(ba.size(), downloadSize);
    delete reply;
}

void tst_QHttpNetworkConnection::_delete_data()
{
    // not tested yet
}

void tst_QHttpNetworkConnection::_delete()
{
    QEXPECT_FAIL("", "not tested yet", Continue);
    QVERIFY(false);
}

void tst_QHttpNetworkConnection::trace_data()
{
    // not tested yet
}

void tst_QHttpNetworkConnection::trace()
{
    QEXPECT_FAIL("", "not tested yet", Continue);
    QVERIFY(false);
}

void tst_QHttpNetworkConnection::_connect_data()
{
    // not tested yet
}

void tst_QHttpNetworkConnection::_connect()
{
    QEXPECT_FAIL("", "not tested yet", Continue);
    QVERIFY(false);
}

void tst_QHttpNetworkConnection::challenge401(const QHttpNetworkRequest &request,
                                                        QAuthenticator *authenticator,
                                                        const QHttpNetworkConnection *connection)
{
    Q_UNUSED(request)
    Q_UNUSED(connection)

    QHttpNetworkConnection *c = qobject_cast<QHttpNetworkConnection*>(sender());
    if (connection) {
        QVariant val = c->property("setCredentials");
        if (val.toBool()) {
            QVariant user = c->property("username");
            QVariant password = c->property("password");
            authenticator->setUser(user.toString());
            authenticator->setPassword(password.toString());
            c->setProperty("setCredentials", false);
        }
    }
}

void tst_QHttpNetworkConnection::get401_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<bool>("setCredentials");
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<int>("statusCode");

    QTest::newRow("no-credentials") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfcs-auth/index.html" << ushort(80) << false << false << "" << ""<<401;
    QTest::newRow("invalid-credentials") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfcs-auth/index.html" << ushort(80) << false << true << "test" << "test"<<401;
    QTest::newRow("valid-credentials") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfcs-auth/index.html" << ushort(80) << false << true << "httptest" << "httptest"<<200;
    QTest::newRow("digest-authentication-invalid") << "http://" << QtNetworkSettings::serverName() << "/qtest/auth-digest/index.html" << ushort(80) << false << true << "wrong" << "wrong"<<401;
    QTest::newRow("digest-authentication-valid") << "http://" << QtNetworkSettings::serverName() << "/qtest/auth-digest/index.html" << ushort(80) << false << true << "httptest" << "httptest"<<200;
}

void tst_QHttpNetworkConnection::get401()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(bool, setCredentials);
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(int, statusCode);

    QHttpNetworkConnection connection(host, port, encrypt);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);
    connect(&connection, SIGNAL(authenticationRequired(const QHttpNetworkRequest&, QAuthenticator *, const QHttpNetworkConnection*)),
                SLOT(challenge401(const QHttpNetworkRequest&, QAuthenticator *, const QHttpNetworkConnection*)));
    connection.setProperty("setCredentials", setCredentials);
    connection.setProperty("username", username);
    connection.setProperty("password", password);

    QHttpNetworkRequest request(protocol + host + path);
    QHttpNetworkReply *reply = connection.sendRequest(request);

    finishedCalled = false;
    finishedWithErrorCalled = false;

    connect(reply, SIGNAL(finished()), SLOT(finishedReply()));
    connect(reply, SIGNAL(finishedWithError(QNetworkReply::NetworkError, const QString &)),
        SLOT(finishedWithError(QNetworkReply::NetworkError, const QString &)));

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (finishedCalled)
            break;
        if (finishedWithErrorCalled)
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }
    QCOMPARE(reply->statusCode(), statusCode);
    delete reply;
}

#ifndef QT_NO_COMPRESS
void tst_QHttpNetworkConnection::compression_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<int>("statusCode");
    QTest::addColumn<QString>("statusString");
    QTest::addColumn<int>("contentLength");
    QTest::addColumn<int>("downloadSize");
    QTest::addColumn<bool>("autoCompress");
    QTest::addColumn<QString>("contentCoding");

    QTest::newRow("success-autogzip-temp") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfcs/rfc2616.html" << ushort(80) << false << 200 << "OK" << -1 << 418321 << true << "";
    QTest::newRow("success-nogzip-temp") << "http://" << QtNetworkSettings::serverName() << "/qtest/rfcs/rfc2616.html" << ushort(80) << false << 200 << "OK" << 418321 << 418321 << false << "identity";
    QTest::newRow("success-manualgzip-temp") << "http://" << QtNetworkSettings::serverName() << "/qtest/deflate/rfc2616.html" << ushort(80) << false << 200 << "OK" << 119124 << 119124 << false << "gzip";

}

void tst_QHttpNetworkConnection::compression()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(int, statusCode);
    QFETCH(QString, statusString);
    QFETCH(int, contentLength);
    QFETCH(int, downloadSize);
    QFETCH(bool, autoCompress);
    QFETCH(QString, contentCoding);

    QHttpNetworkConnection connection(host);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    QCOMPARE(connection.isEncrypted(), encrypt);

    QHttpNetworkRequest request(protocol + host + path);
    if (!autoCompress)
        request.setHeaderField("Accept-Encoding", contentCoding.toLatin1());
    QHttpNetworkReply *reply = connection.sendRequest(request);
    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }

    QCOMPARE(reply->statusCode(), statusCode);
    QCOMPARE(reply->reasonPhrase(), statusString);
    bool isLengthOk = (reply->contentLength() == qint64(contentLength)
    				  || reply->contentLength() == qint64(downloadSize));

    QVERIFY(isLengthOk);

    stopWatch.start();
    QByteArray ba;
    do {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            ba += reply->read();
        if (stopWatch.elapsed() >= 30000)
            break;
    } while (!reply->isFinished());

    QVERIFY(reply->isFinished());
    QCOMPARE(ba.size(), downloadSize);

    delete reply;
}
#endif

#ifndef QT_NO_OPENSSL
void tst_QHttpNetworkConnection::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors)

    QHttpNetworkConnection *connection = qobject_cast<QHttpNetworkConnection*>(sender());
    if (connection) {
        QVariant val = connection->property("ignoreFromSignal");
        if (val.toBool())
            connection->ignoreSslErrors();
        finishedWithErrorCalled = true;
    }
}

void tst_QHttpNetworkConnection::ignoresslerror_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<bool>("ignoreInit");
    QTest::addColumn<bool>("ignoreFromSignal");
    QTest::addColumn<int>("statusCode");

    // This test will work only if the website has ssl errors.
    // fluke's certificate is signed by a non-standard authority.
    // Since we don't introduce that CA into the SSL verification chain,
    // connecting should fail.
    QTest::newRow("success-init") << "https://" << QtNetworkSettings::serverName() << "/" << ushort(443) << true << true << false << 200;
    QTest::newRow("success-fromSignal") << "https://" << QtNetworkSettings::serverName() << "/" << ushort(443) << true << false << true << 200;
    QTest::newRow("failure") << "https://" << QtNetworkSettings::serverName() << "/" << ushort(443) << true << false << false << 100;
}

void tst_QHttpNetworkConnection::ignoresslerror()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(bool, ignoreInit);
    QFETCH(bool, ignoreFromSignal);
    QFETCH(int, statusCode);

    QHttpNetworkConnection connection(host, port, encrypt);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();
    if (ignoreInit)
        connection.ignoreSslErrors();
    QCOMPARE(connection.isEncrypted(), encrypt);
    connect(&connection, SIGNAL(sslErrors(const QList<QSslError>&)),
        SLOT(sslErrors(const QList<QSslError>&)));
    connection.setProperty("ignoreFromSignal", ignoreFromSignal);

    QHttpNetworkRequest request(protocol + host + path);
    QHttpNetworkReply *reply = connection.sendRequest(request);

    finishedWithErrorCalled = false;

    connect(reply, SIGNAL(finished()), SLOT(finishedReply()));

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (reply->bytesAvailable())
            break;
        if (statusCode == 100 && finishedWithErrorCalled)
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }
    QCOMPARE(reply->statusCode(), statusCode);
    delete reply;
}
#endif

#ifdef QT_NO_OPENSSL
Q_DECLARE_METATYPE(QNetworkReply::NetworkError)
void tst_QHttpNetworkConnection::nossl_data()
{
    QTest::addColumn<QString>("protocol");
    QTest::addColumn<QString>("host");
    QTest::addColumn<QString>("path");
    QTest::addColumn<ushort>("port");
    QTest::addColumn<bool>("encrypt");
    QTest::addColumn<QNetworkReply::NetworkError>("networkError");

    QTest::newRow("protocol-error") << "https://" << QtNetworkSettings::serverName() << "/" << ushort(443) << true <<QNetworkReply::ProtocolUnknownError;
}

void tst_QHttpNetworkConnection::nossl()
{
    QFETCH(QString, protocol);
    QFETCH(QString, host);
    QFETCH(QString, path);
    QFETCH(ushort, port);
    QFETCH(bool, encrypt);
    QFETCH(QNetworkReply::NetworkError, networkError);

    QHttpNetworkConnection connection(host, port, encrypt);
    QCOMPARE(connection.port(), port);
    QCOMPARE(connection.hostName(), host);
    if (encrypt)
        connection.enableEncryption();

    QHttpNetworkRequest request(protocol + host + path);
    QHttpNetworkReply *reply = connection.sendRequest(request);

    finishedWithErrorCalled = false;
    netErrorCode = QNetworkReply::NoError;

    connect(reply, SIGNAL(finished()), SLOT(finishedReply()));
    connect(reply, SIGNAL(finishedWithError(QNetworkReply::NetworkError, const QString &)),
        SLOT(finishedWithError(QNetworkReply::NetworkError, const QString &)));

    QTime stopWatch;
    stopWatch.start();
    forever {
        QCoreApplication::instance()->processEvents();
        if (finishedWithErrorCalled)
            break;
        if (stopWatch.elapsed() >= 30000)
            break;
    }
    QCOMPARE(netErrorCode, networkError);
    delete reply;
}
#endif

QTEST_MAIN(tst_QHttpNetworkConnection)
#include "tst_qhttpnetworkconnection.moc"
