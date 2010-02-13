/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qtest.h>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include <QNetworkCookieJar>
#include "testhttpserver.h"

#define SERVER_PORT 14445

class tst_xmlhttprequest : public QObject
{
    Q_OBJECT
public:
    tst_xmlhttprequest() {}

private slots:
    void initTestCase() {
        if (QLocale::system().name().replace(QChar::fromAscii('_'),QChar::fromAscii('-')) != QLatin1String("en-US")) {
            qWarning() << "Test will fail unless LANG is en_US";
        }
    }

    void domExceptionCodes();
    void callbackException();
    void callbackException_data();
    void staticStateValues();
    void instanceStateValues();
    void constructor();
    void defaultState();
    void open();
    void open_invalid_method();
    void open_sync();
    void open_arg_count();
    void setRequestHeader();
    void setRequestHeader_unsent();
    void setRequestHeader_illegalName_data();
    void setRequestHeader_illegalName();
    void setRequestHeader_sent();
    void setRequestHeader_args();
    void send_unsent();
    void send_alreadySent();
    void send_ignoreData();
    void send_withdata();
    void abort();
    void abort_unsent();
    void abort_opened();
    void getResponseHeader();
    void getResponseHeader_unsent();
    void getResponseHeader_sent();
    void getResponseHeader_args();
    void getAllResponseHeaders();
    void getAllResponseHeaders_unsent();
    void getAllResponseHeaders_sent();
    void getAllResponseHeaders_args();
    void status();
    void statusText();
    void responseText();
    void responseXML_invalid();
    void invalidMethodUsage();
    void redirects();

    // Attributes
    void document();
    void element();
    void attr();
    void text();
    void cdata();

    // Crashes
    // void outstanding_request_at_shutdown();

    // void network_errors()
    // void readyState()

private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

// Test that the dom exception codes are correct
void tst_xmlhttprequest::domExceptionCodes()
{
    QmlComponent component(&engine, TEST_FILE("domExceptionCodes.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("index_size_err").toInt(), 1);
    QCOMPARE(object->property("domstring_size_err").toInt(), 2);
    QCOMPARE(object->property("hierarchy_request_err").toInt(), 3);
    QCOMPARE(object->property("wrong_document_err").toInt(), 4);
    QCOMPARE(object->property("invalid_character_err").toInt(), 5);
    QCOMPARE(object->property("no_data_allowed_err").toInt(), 6);
    QCOMPARE(object->property("no_modification_allowed_err").toInt(), 7);
    QCOMPARE(object->property("not_found_err").toInt(), 8);
    QCOMPARE(object->property("not_supported_err").toInt(), 9);
    QCOMPARE(object->property("inuse_attribute_err").toInt(), 10);
    QCOMPARE(object->property("invalid_state_err").toInt(), 11);
    QCOMPARE(object->property("syntax_err").toInt(), 12);
    QCOMPARE(object->property("invalid_modification_err").toInt(), 13);
    QCOMPARE(object->property("namespace_err").toInt(), 14);
    QCOMPARE(object->property("invalid_access_err").toInt(), 15);
    QCOMPARE(object->property("validation_err").toInt(), 16);
    QCOMPARE(object->property("type_mismatch_err").toInt(), 17);

    delete object;
}

#define TRY_WAIT(expr) \
    do { \
        for (int ii = 0; ii < 6; ++ii) { \
            if ((expr)) break; \
            QTest::qWait(50); \
        } \
        QVERIFY((expr)); \
    } while (false) 


void tst_xmlhttprequest::callbackException_data()
{
    QTest::addColumn<QString>("which");
    QTest::addColumn<int>("line");

    QTest::newRow("on-opened") << "1" << 15;
    QTest::newRow("on-loading") << "3" << 15;
    QTest::newRow("on-done") << "4" << 15;
}

void tst_xmlhttprequest::callbackException()
{
    // Test exception reporting for exceptions thrown at various points.

    QFETCH(QString, which);
    QFETCH(int, line);
    
    QString expect = TEST_FILE("callbackException.qml").toString() + ":"+QString::number(line)+": Error: Exception from Callback";
    QTest::ignoreMessage(QtWarningMsg, expect.toLatin1());

    QmlComponent component(&engine, TEST_FILE("callbackException.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "testdocument.html");
    object->setProperty("which", which);
    component.completeCreate();

    TRY_WAIT(object->property("threw").toBool() == true);

    delete object;
}

// Test that the state value properties on the XMLHttpRequest constructor have the correct values.
// ### WebKit does not do this, but it seems to fit the standard and QML better
void tst_xmlhttprequest::staticStateValues()
{
    QmlComponent component(&engine, TEST_FILE("staticStateValues.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("unsent").toInt(), 0);
    QCOMPARE(object->property("opened").toInt(), 1);
    QCOMPARE(object->property("headers_received").toInt(), 2);
    QCOMPARE(object->property("loading").toInt(), 3);
    QCOMPARE(object->property("done").toInt(), 4);

    delete object;
}

// Test that the state value properties on instances have the correct values.
void tst_xmlhttprequest::instanceStateValues()
{
    QmlComponent component(&engine, TEST_FILE("instanceStateValues.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("unsent").toInt(), 0);
    QCOMPARE(object->property("opened").toInt(), 1);
    QCOMPARE(object->property("headers_received").toInt(), 2);
    QCOMPARE(object->property("loading").toInt(), 3);
    QCOMPARE(object->property("done").toInt(), 4);

    delete object;
}

// Test calling constructor 
void tst_xmlhttprequest::constructor()
{
    QmlComponent component(&engine, TEST_FILE("constructor.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("calledAsConstructor").toBool(), true);
    QCOMPARE(object->property("calledAsFunction").toBool(), true);

    delete object;
}

// Test that all the properties are set correctly before any request is sent
void tst_xmlhttprequest::defaultState()
{
    QmlComponent component(&engine, TEST_FILE("defaultState.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("readState").toInt(), 0);
    QCOMPARE(object->property("statusIsException").toBool(), true);
    QCOMPARE(object->property("statusTextIsException").toBool(), true);
    QCOMPARE(object->property("responseText").toString(), QString());
    QCOMPARE(object->property("responseXMLIsNull").toBool(), true);

    delete object;
}

// Test valid XMLHttpRequest.open() calls
void tst_xmlhttprequest::open()
{
    // Relative url
    {
        QmlComponent component(&engine, TEST_FILE("open.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "testdocument.html");
        component.completeCreate();

        QCOMPARE(object->property("readyState").toBool(), true);
        QCOMPARE(object->property("openedState").toBool(), true);
        QCOMPARE(object->property("status").toBool(), true);
        QCOMPARE(object->property("statusText").toBool(), true);
        QCOMPARE(object->property("responseText").toBool(), true);
        QCOMPARE(object->property("responseXML").toBool(), true);

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Absolute url
    {
        QmlComponent component(&engine, TEST_FILE("open.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", TEST_FILE("testdocument.html").toString());
        component.completeCreate();

        QCOMPARE(object->property("readyState").toBool(), true);
        QCOMPARE(object->property("openedState").toBool(), true);
        QCOMPARE(object->property("status").toBool(), true);
        QCOMPARE(object->property("statusText").toBool(), true);
        QCOMPARE(object->property("responseText").toBool(), true);
        QCOMPARE(object->property("responseXML").toBool(), true);

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Absolute network url
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("open_network.expect"), 
                            TEST_FILE("open_network.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("open.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        QCOMPARE(object->property("readyState").toBool(), true);
        QCOMPARE(object->property("openedState").toBool(), true);
        QCOMPARE(object->property("status").toBool(), true);
        QCOMPARE(object->property("statusText").toBool(), true);
        QCOMPARE(object->property("responseText").toBool(), true);
        QCOMPARE(object->property("responseXML").toBool(), true);

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // User/pass
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("open_network.expect"), 
                            TEST_FILE("open_network.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("open_user.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        QCOMPARE(object->property("readyState").toBool(), true);
        QCOMPARE(object->property("openedState").toBool(), true);
        QCOMPARE(object->property("status").toBool(), true);
        QCOMPARE(object->property("statusText").toBool(), true);
        QCOMPARE(object->property("responseText").toBool(), true);
        QCOMPARE(object->property("responseXML").toBool(), true);

        TRY_WAIT(object->property("dataOK").toBool() == true);

        // ### Check that the username/password were sent to the server

        delete object;
    }
}

// Test that calling XMLHttpRequest.open() with an invalid method raises an exception
void tst_xmlhttprequest::open_invalid_method()
{
    QmlComponent component(&engine, TEST_FILE("open_invalid_method.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("exceptionThrown").toBool(), true);

    delete object;
}

// Test that calling XMLHttpRequest.open() with sync raises an exception
void tst_xmlhttprequest::open_sync()
{
    QmlComponent component(&engine, TEST_FILE("open_sync.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("exceptionThrown").toBool(), true);

    delete object;
}

// Calling with incorrect arg count raises an exception
void tst_xmlhttprequest::open_arg_count()
{
    {
        QmlComponent component(&engine, TEST_FILE("open_arg_count.1.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("exceptionThrown").toBool(), true);

        delete object;
    }

    {
        QmlComponent component(&engine, TEST_FILE("open_arg_count.2.qml"));
        QObject *object = component.create();
        QVERIFY(object != 0);

        QCOMPARE(object->property("exceptionThrown").toBool(), true);

        delete object;
    }
}

// Test valid setRequestHeader() calls
void tst_xmlhttprequest::setRequestHeader()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("setRequestHeader.expect"), 
                        TEST_FILE("setRequestHeader.reply"), 
                        TEST_FILE("testdocument.html")));

    QmlComponent component(&engine, TEST_FILE("setRequestHeader.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    component.completeCreate();

    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Test setting headers before open() throws exception
void tst_xmlhttprequest::setRequestHeader_unsent()
{
    QmlComponent component(&engine, TEST_FILE("setRequestHeader_unsent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

void tst_xmlhttprequest::setRequestHeader_illegalName_data()
{
    QTest::addColumn<QString>("name");

    QTest::newRow("Accept-Charset") << "AccePT-CHArset";
    QTest::newRow("Accept-Encoding") << "AccEpt-EnCOding";
    QTest::newRow("Connection") << "ConnECtion";
    QTest::newRow("Content-Length") << "ContEnt-LenGth";
    QTest::newRow("Cookie") << "CookIe";
    QTest::newRow("Cookie2") << "CoOkie2";
    QTest::newRow("Content-Transfer-Encoding") << "ConteNT-tRANSFER-eNCOding";
    QTest::newRow("Date") << "DaTE";
    QTest::newRow("Expect") << "ExPect";
    QTest::newRow("Host") << "HoST";
    QTest::newRow("Keep-Alive") << "KEEP-aLive";
    QTest::newRow("Referer") << "ReferEr";
    QTest::newRow("TE") << "Te";
    QTest::newRow("Trailer") << "TraILEr";
    QTest::newRow("Transfer-Encoding") << "tRANsfer-Encoding";
    QTest::newRow("Upgrade") << "UpgrADe";
    QTest::newRow("User-Agent") << "uSEr-Agent";
    QTest::newRow("Via") << "vIa";
    QTest::newRow("Proxy-") << "ProXy-";
    QTest::newRow("Sec-") << "SeC-";
    QTest::newRow("Proxy-*") << "Proxy-BLAH";
    QTest::newRow("Sec-*") << "Sec-F";
}

// Tests that using illegal header names has no effect
void tst_xmlhttprequest::setRequestHeader_illegalName()
{
    QFETCH(QString, name);

    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("open_network.expect"), 
                        TEST_FILE("open_network.reply"), 
                        TEST_FILE("testdocument.html")));

    QmlComponent component(&engine, TEST_FILE("setRequestHeader_illegalName.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    object->setProperty("header", name);
    component.completeCreate();

    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("openedState").toBool(), true);
    QCOMPARE(object->property("status").toBool(), true);
    QCOMPARE(object->property("statusText").toBool(), true);
    QCOMPARE(object->property("responseText").toBool(), true);
    QCOMPARE(object->property("responseXML").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Test that attempting to set a header after a request is sent throws an exception
void tst_xmlhttprequest::setRequestHeader_sent()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("open_network.expect"), 
                        TEST_FILE("open_network.reply"), 
                        TEST_FILE("testdocument.html")));

    QmlComponent component(&engine, TEST_FILE("setRequestHeader_sent.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("test").toBool(), true);
    
    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Invalid arg count throws exception
void tst_xmlhttprequest::setRequestHeader_args()
{
    QmlComponent component(&engine, TEST_FILE("setRequestHeader_args.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("exceptionThrown").toBool(), true);

    delete object;
}

// Test that calling send() in UNSENT state throws an exception
void tst_xmlhttprequest::send_unsent()
{
    QmlComponent component(&engine, TEST_FILE("send_unsent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

// Test attempting to resend a sent request throws an exception
void tst_xmlhttprequest::send_alreadySent()
{
    QmlComponent component(&engine, TEST_FILE("send_alreadySent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);
    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Test that send for a GET or HEAD ignores data
void tst_xmlhttprequest::send_ignoreData()
{
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_ignoreData_GET.expect"), 
                            TEST_FILE("send_ignoreData.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_ignoreData.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("reqType", "GET");
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_ignoreData_PUT.expect"), 
                            TEST_FILE("send_ignoreData.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_ignoreData.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("reqType", "HEAD");
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }
}

// Test that send()'ing data works
void tst_xmlhttprequest::send_withdata()
{
    // No content-type
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.1.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.1.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Correct content-type
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.1.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.2.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Incorrect content-type
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.1.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.3.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }
    
    // Correct content-type - out of order
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.4.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.4.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Incorrect content-type - out of order
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.4.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.5.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // PUT
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.6.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.6.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }

    // Correct content-type - no charset
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("send_data.1.expect"), 
                            TEST_FILE("send_data.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("send_data.7.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        delete object;
    }
}

// Test abort() has no effect in unsent state
void tst_xmlhttprequest::abort_unsent()
{
    QmlComponent component(&engine, TEST_FILE("abort_unsent.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("openedState").toBool(), true);
    QCOMPARE(object->property("status").toBool(), true);
    QCOMPARE(object->property("statusText").toBool(), true);
    QCOMPARE(object->property("responseText").toBool(), true);
    QCOMPARE(object->property("responseXML").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Test abort() cancels an open (but unsent) request
void tst_xmlhttprequest::abort_opened()
{
    QmlComponent component(&engine, TEST_FILE("abort_opened.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("openedState").toBool(), true);
    QCOMPARE(object->property("status").toBool(), true);
    QCOMPARE(object->property("statusText").toBool(), true);
    QCOMPARE(object->property("responseText").toBool(), true);
    QCOMPARE(object->property("responseXML").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

// Test abort() aborts in progress send
void tst_xmlhttprequest::abort()
{
    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("abort.expect"), 
                        TEST_FILE("abort.reply"), 
                        TEST_FILE("testdocument.html")));

    QmlComponent component(&engine, TEST_FILE("abort.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("urlDummy", "http://127.0.0.1:14449/testdocument.html");
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("seenDone").toBool(), true);
    QCOMPARE(object->property("didNotSeeUnsent").toBool(), true);
    QCOMPARE(object->property("endStateUnsent").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    delete object;
}

void tst_xmlhttprequest::getResponseHeader()
{
    QmlEngine engine; // Avoid cookie contamination

    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("getResponseHeader.expect"), 
                        TEST_FILE("getResponseHeader.reply"), 
                        TEST_FILE("testdocument.html")));


    QmlComponent component(&engine, TEST_FILE("getResponseHeader.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("unsentException").toBool(), true);
    QCOMPARE(object->property("openedException").toBool(), true);
    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("openedState").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("headersReceivedState").toBool(), true);
    QCOMPARE(object->property("headersReceivedNullHeader").toBool(), true);
    QCOMPARE(object->property("headersReceivedValidHeader").toBool(), true);
    QCOMPARE(object->property("headersReceivedMultiValidHeader").toBool(), true);
    QCOMPARE(object->property("headersReceivedCookieHeader").toBool(), true);

    QCOMPARE(object->property("doneState").toBool(), true);
    QCOMPARE(object->property("doneNullHeader").toBool(), true);
    QCOMPARE(object->property("doneValidHeader").toBool(), true);
    QCOMPARE(object->property("doneMultiValidHeader").toBool(), true);
    QCOMPARE(object->property("doneCookieHeader").toBool(), true);

    delete object;
}

// Test getResponseHeader throws an exception in an invalid state
void tst_xmlhttprequest::getResponseHeader_unsent()
{
    QmlComponent component(&engine, TEST_FILE("getResponseHeader_unsent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

// Test getResponseHeader throws an exception in an invalid state
void tst_xmlhttprequest::getResponseHeader_sent()
{
    QmlComponent component(&engine, TEST_FILE("getResponseHeader_sent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

// Invalid arg count throws exception
void tst_xmlhttprequest::getResponseHeader_args()
{
    QmlComponent component(&engine, TEST_FILE("getResponseHeader_args.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("exceptionThrown").toBool() == true);

    delete object;
}

void tst_xmlhttprequest::getAllResponseHeaders()
{
    QmlEngine engine; // Avoid cookie contamination

    TestHTTPServer server(SERVER_PORT);
    QVERIFY(server.isValid());
    QVERIFY(server.wait(TEST_FILE("getResponseHeader.expect"), 
                        TEST_FILE("getResponseHeader.reply"), 
                        TEST_FILE("testdocument.html")));

    QmlComponent component(&engine, TEST_FILE("getAllResponseHeaders.qml"));
    QObject *object = component.beginCreate(engine.rootContext());
    QVERIFY(object != 0);
    object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
    component.completeCreate();

    QCOMPARE(object->property("unsentException").toBool(), true);
    QCOMPARE(object->property("openedException").toBool(), true);
    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("openedState").toBool(), true);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("headersReceivedState").toBool(), true);
    QCOMPARE(object->property("headersReceivedHeader").toBool(), true);

    QCOMPARE(object->property("doneState").toBool(), true);
    QCOMPARE(object->property("doneHeader").toBool(), true);

    delete object;
}

// Test getAllResponseHeaders throws an exception in an invalid state
void tst_xmlhttprequest::getAllResponseHeaders_unsent()
{
    QmlComponent component(&engine, TEST_FILE("getAllResponseHeaders_unsent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

// Test getAllResponseHeaders throws an exception in an invalid state
void tst_xmlhttprequest::getAllResponseHeaders_sent()
{
    QmlComponent component(&engine, TEST_FILE("getAllResponseHeaders_sent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test").toBool(), true);

    delete object;
}

// Invalid arg count throws exception
void tst_xmlhttprequest::getAllResponseHeaders_args()
{
    QmlComponent component(&engine, TEST_FILE("getAllResponseHeaders_args.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("exceptionThrown").toBool() == true);

    delete object;
}

void tst_xmlhttprequest::status()
{
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.200.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("status.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedStatus", 200);
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsentException").toBool(), true);
        QCOMPARE(object->property("openedException").toBool(), true);
        QCOMPARE(object->property("sentException").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("resetException").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.404.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("status.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedStatus", 404);
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsentException").toBool(), true);
        QCOMPARE(object->property("openedException").toBool(), true);
        QCOMPARE(object->property("sentException").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("resetException").toBool(), true);

        delete object;
    }
}

void tst_xmlhttprequest::statusText()
{
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.200.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("statusText.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedStatus", "OK");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsentException").toBool(), true);
        QCOMPARE(object->property("openedException").toBool(), true);
        QCOMPARE(object->property("sentException").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("resetException").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.404.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("statusText.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedStatus", "Document not found");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsentException").toBool(), true);
        QCOMPARE(object->property("openedException").toBool(), true);
        QCOMPARE(object->property("sentException").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("resetException").toBool(), true);

        delete object;
    }
}

void tst_xmlhttprequest::responseText()
{
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.200.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("responseText.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedText", "QML Rocks!\n");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsent").toBool(), true);
        QCOMPARE(object->property("opened").toBool(), true);
        QCOMPARE(object->property("sent").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("reset").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.200.reply"), 
                            QUrl()));

        QmlComponent component(&engine, TEST_FILE("responseText.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedText", "");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsent").toBool(), true);
        QCOMPARE(object->property("opened").toBool(), true);
        QCOMPARE(object->property("sent").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("reset").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        QVERIFY(server.wait(TEST_FILE("status.expect"), 
                            TEST_FILE("status.404.reply"), 
                            TEST_FILE("testdocument.html")));

        QmlComponent component(&engine, TEST_FILE("responseText.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/testdocument.html");
        object->setProperty("expectedText", "");
        component.completeCreate();

        TRY_WAIT(object->property("dataOK").toBool() == true);

        QCOMPARE(object->property("unsent").toBool(), true);
        QCOMPARE(object->property("opened").toBool(), true);
        QCOMPARE(object->property("sent").toBool(), true);
        QCOMPARE(object->property("headersReceived").toBool(), true);
        QCOMPARE(object->property("loading").toBool(), true);
        QCOMPARE(object->property("done").toBool(), true);
        QCOMPARE(object->property("reset").toBool(), true);

        delete object;
    }
}

// Test that calling hte XMLHttpRequest methods on a non-XMLHttpRequest object
// throws an exception
void tst_xmlhttprequest::invalidMethodUsage()
{
    QmlComponent component(&engine, TEST_FILE("invalidMethodUsage.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("onreadystatechange").toBool(), true);
    QCOMPARE(object->property("readyState").toBool(), true);
    QCOMPARE(object->property("status").toBool(), true);
    QCOMPARE(object->property("statusText").toBool(), true);
    QCOMPARE(object->property("responseText").toBool(), true);
    QCOMPARE(object->property("responseXML").toBool(), true);

    QCOMPARE(object->property("open").toBool(), true);
    QCOMPARE(object->property("setRequestHeader").toBool(), true);
    QCOMPARE(object->property("send").toBool(), true);
    QCOMPARE(object->property("abort").toBool(), true);
    QCOMPARE(object->property("getResponseHeader").toBool(), true);
    QCOMPARE(object->property("getAllResponseHeaders").toBool(), true);

    delete object;
}

// Test that XMLHttpRequest transparently redirects
void tst_xmlhttprequest::redirects()
{
    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        server.addRedirect("redirect.html", "http://127.0.0.1:14445/redirecttarget.html");
        server.serveDirectory("data");

        QmlComponent component(&engine, TEST_FILE("redirects.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/redirect.html");
        object->setProperty("expectedText", "");
        component.completeCreate();

        TRY_WAIT(object->property("done").toBool() == true);
        QCOMPARE(object->property("dataOK").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        server.addRedirect("redirect.html", "http://127.0.0.1:14445/redirectmissing.html");
        server.serveDirectory("data");

        QmlComponent component(&engine, TEST_FILE("redirectError.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/redirect.html");
        object->setProperty("expectedText", "");
        component.completeCreate();

        TRY_WAIT(object->property("done").toBool() == true);
        QCOMPARE(object->property("dataOK").toBool(), true);

        delete object;
    }

    {
        TestHTTPServer server(SERVER_PORT);
        QVERIFY(server.isValid());
        server.addRedirect("redirect.html", "http://127.0.0.1:14445/redirect.html");
        server.serveDirectory("data");

        QmlComponent component(&engine, TEST_FILE("redirectRecur.qml"));
        QObject *object = component.beginCreate(engine.rootContext());
        QVERIFY(object != 0);
        object->setProperty("url", "http://127.0.0.1:14445/redirect.html");
        object->setProperty("expectedText", "");
        component.completeCreate();

        for (int ii = 0; ii < 60; ++ii) { 
            if (object->property("done").toBool()) break; 
            QTest::qWait(50); 
        } 
        QVERIFY(object->property("done").toBool() == true);

        QCOMPARE(object->property("dataOK").toBool(), true);

        delete object;
    }
}

void tst_xmlhttprequest::responseXML_invalid()
{
    QmlComponent component(&engine, TEST_FILE("responseXML_invalid.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlNull").toBool(), true);

    delete object;
}

// Test the Document DOM element
void tst_xmlhttprequest::document()
{
    QmlComponent component(&engine, TEST_FILE("document.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlTest").toBool(), true);

    delete object;
}

// Test the Element DOM element
void tst_xmlhttprequest::element()
{
    QmlComponent component(&engine, TEST_FILE("element.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlTest").toBool(), true);

    delete object;
}

// Test the Attr DOM element
void tst_xmlhttprequest::attr()
{
    QmlComponent component(&engine, TEST_FILE("attr.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlTest").toBool(), true);

    delete object;
}

// Test the Text DOM element
void tst_xmlhttprequest::text()
{
    QmlComponent component(&engine, TEST_FILE("text.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlTest").toBool(), true);

    delete object;
}

// Test the CDataSection DOM element
void tst_xmlhttprequest::cdata()
{
    QmlComponent component(&engine, TEST_FILE("cdata.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    TRY_WAIT(object->property("dataOK").toBool() == true);

    QCOMPARE(object->property("xmlTest").toBool(), true);

    delete object;
}

QTEST_MAIN(tst_xmlhttprequest)

#include "tst_qmlxmlhttprequest.moc"
