/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qobject.h>
#include <QtDeclarative/qmlengine.h>
#include <private/qmlengine_p.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>
#include <QtNetwork/qnetworkreply.h>
#include "qmlxmlhttprequest_p.h"

#include <QtCore/qdebug.h>

// ### Find real values
#define INVALID_STATE_ERR ((QScriptContext::Error)15)

class QmlXMLHttpRequest : public QObject
{
Q_OBJECT
public:
    enum State { Unsent = 0, 
                 Opened = 1, HeadersReceived = 2,
                 Loading = 3, Done = 4 };

    QmlXMLHttpRequest(QmlEngine *engine);
    virtual ~QmlXMLHttpRequest();

    QScriptValue callback() const;
    void setCallback(const QScriptValue &);

    bool sendFlag() const;
    bool errorFlag() const;
    quint32 readyState() const;
    int replyStatus() const;
    QString replyStatusText() const;

    void open(const QString &, const QUrl &);
    void addHeader(const QString &, const QString &);
    void send(const QByteArray &);
    void abort();

    QString responseBody() const;
private slots:
    void downloadProgress(qint64);
    void error(QNetworkReply::NetworkError);
    void finished();

private:
    QmlEngine *m_engine;

    State m_state;
    bool m_errorFlag;
    bool m_sendFlag;
    QString m_method;
    QUrl m_url;
    QByteArray m_responseEntityBody;

    void dispatchCallback();
    QScriptValue m_callback;

    int m_status;
    QString m_statusText;
    QNetworkRequest m_request;
    QNetworkReply *m_network;
    void destroyNetwork();
};

QmlXMLHttpRequest::QmlXMLHttpRequest(QmlEngine *engine)
: m_engine(engine), m_state(Unsent), m_errorFlag(false), m_sendFlag(false),
  m_network(0)
{
    Q_ASSERT(m_engine);
}

QmlXMLHttpRequest::~QmlXMLHttpRequest()
{
    destroyNetwork();
}

QScriptValue QmlXMLHttpRequest::callback() const
{
    return m_callback;
}

void QmlXMLHttpRequest::setCallback(const QScriptValue &c)
{
    m_callback = c;
}

bool QmlXMLHttpRequest::sendFlag() const
{
    return m_sendFlag;
}

bool QmlXMLHttpRequest::errorFlag() const
{
    return m_errorFlag;
}

quint32 QmlXMLHttpRequest::readyState() const
{
    return m_state;
}

int QmlXMLHttpRequest::replyStatus() const
{
    return m_status;
}

QString QmlXMLHttpRequest::replyStatusText() const
{
    return m_statusText;
}

void QmlXMLHttpRequest::open(const QString &method, const QUrl &url)
{
    destroyNetwork();
    m_sendFlag = false;
    m_errorFlag = false;
    m_responseEntityBody = QByteArray();
    m_method = method;
    m_url = url;
    m_state = Opened;
    dispatchCallback();
}

void QmlXMLHttpRequest::addHeader(const QString &name, const QString &value)
{
    QByteArray utfname = name.toUtf8();

    if (m_request.hasRawHeader(utfname)) {
        m_request.setRawHeader(utfname, m_request.rawHeader(utfname) + "," + value.toUtf8());
    } else {
        m_request.setRawHeader(utfname, value.toUtf8());
    }
}

void QmlXMLHttpRequest::send(const QByteArray &data)
{
    m_errorFlag = false;
    m_sendFlag = true;

    dispatchCallback();

    m_request.setUrl(m_url);

    if (m_method == QLatin1String("GET"))
        m_network = m_engine->networkAccessManager()->get(m_request);
    else if (m_method == QLatin1String("HEAD"))
        m_network = m_engine->networkAccessManager()->head(m_request);
    else if(m_method == QLatin1String("POST"))
        m_network = m_engine->networkAccessManager()->post(m_request, data);
    else if(m_method == QLatin1String("PUT"))
        m_network = m_engine->networkAccessManager()->put(m_request, data);

    QObject::connect(m_network, SIGNAL(downloadProgress(qint64,qint64)), 
                     this, SLOT(downloadProgress(qint64)));
    QObject::connect(m_network, SIGNAL(error(QNetworkReply::NetworkError)),
                     this, SLOT(error(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished()),
                     this, SLOT(finished()));
}

void QmlXMLHttpRequest::abort()
{
    destroyNetwork();
    m_responseEntityBody = QByteArray();
    m_errorFlag = true;
    m_request = QNetworkRequest();

    if (!(m_state == Unsent || 
          (m_state == Opened && !m_sendFlag) ||
          m_state == Done)) {

        m_state = Done;
        m_sendFlag = false;
        dispatchCallback();
    }

    m_state = Unsent;
}

void QmlXMLHttpRequest::downloadProgress(qint64 bytes)
{
    m_status = 
        m_network->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_statusText =
        QLatin1String(m_network->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray());

    // ### We assume if this is called the headers are now available
    if (m_state < HeadersReceived) {
        m_state = HeadersReceived;
        dispatchCallback();
    }

    bool wasEmpty = m_responseEntityBody.isEmpty();
    m_responseEntityBody.append(m_network->readAll());
    if (wasEmpty && !m_responseEntityBody.isEmpty()) {
        m_state = Loading;
        dispatchCallback();
    }
}

void QmlXMLHttpRequest::error(QNetworkReply::NetworkError error)
{
    m_responseEntityBody = QByteArray();
    m_errorFlag = true;
    m_request = QNetworkRequest();

    destroyNetwork();

    m_state = Done;
    dispatchCallback();
}

void QmlXMLHttpRequest::finished()
{
    // ### We need to transparently redirect as dictated by the spec

    if (m_state < HeadersReceived) {
        m_state = HeadersReceived;
        dispatchCallback();
    }
    m_responseEntityBody.append(m_network->readAll());
    destroyNetwork();
    if (m_state < Loading) {
        m_state = Loading;
        dispatchCallback();
    }
    m_state = Done;
    dispatchCallback();
}


QString QmlXMLHttpRequest::responseBody() const
{
    return QString::fromUtf8(m_responseEntityBody);
}

void QmlXMLHttpRequest::dispatchCallback()
{
    m_callback.call();
}

void QmlXMLHttpRequest::destroyNetwork()
{
    if (m_network) {
        m_network->disconnect();
        m_network->deleteLater();
        m_network = 0;
    }
}

// XMLHttpRequest methods
static QScriptValue qmlxmlhttprequest_open(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() < 2 || context->argumentCount() > 5)
        return context->throwError(QScriptContext::SyntaxError, "Incorrect argument count");

    // Argument 0 - Method
    QString method = context->argument(0).toString().toUpper();
    if (method != QLatin1String("GET") && 
        method != QLatin1String("PUT") &&
        method != QLatin1String("HEAD") &&
        method != QLatin1String("POST"))
        return context->throwError(QScriptContext::SyntaxError, "Unsupported method");


    // Argument 1 - URL
    QUrl url(context->argument(1).toString()); // ### Need to resolve correctly

    if (url.isRelative()) // ### Fix me
        return context->throwError(QScriptContext::SyntaxError, "Relative URLs not supported");

    // Argument 2 - async (optional)
    if (context->argumentCount() > 2 && !context->argument(2).toBoolean())
        return context->throwError(QScriptContext::SyntaxError, "Synchronous call not supported");


    // Argument 3/4 - user/pass (optional)
    QString username, password;
    if (context->argumentCount() > 3)
        username = context->argument(3).toString();
    if (context->argumentCount() > 4)
        password = context->argument(4).toString();


    // Clear the fragment (if any)
    url.setFragment(QString());
    // Set username/password
    if (!username.isNull()) url.setUserName(username);
    if (!password.isNull()) url.setPassword(password);

    request->open(method, url);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_setRequestHeader(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount() != 2)
        return context->throwError(QScriptContext::SyntaxError, "Incorrect argument count");


    if (request->readyState() != QmlXMLHttpRequest::Opened ||
        request->sendFlag())
        return context->throwError(INVALID_STATE_ERR, "Invalid state");


    QString name = context->argument(0).toString();
    QString value = context->argument(1).toString();

    // ### Check that name and value are well formed

    QString nameUpper = name.toUpper();
    if (nameUpper == QLatin1String("ACCEPT-CHARSET") ||
        nameUpper == QLatin1String("ACCEPT-ENCODING") ||
        nameUpper == QLatin1String("CONNECTION") ||
        nameUpper == QLatin1String("CONTENT-LENGTH") ||
        nameUpper == QLatin1String("COOKIE") ||
        nameUpper == QLatin1String("COOKIE2") ||
        nameUpper == QLatin1String("CONTENT-TRANSFER-ENCODING") ||
        nameUpper == QLatin1String("DATE") ||
        nameUpper == QLatin1String("EXPECT") ||
        nameUpper == QLatin1String("HOST") ||
        nameUpper == QLatin1String("KEEP-ALIVE") ||
        nameUpper == QLatin1String("REFERER") ||
        nameUpper == QLatin1String("TE") ||
        nameUpper == QLatin1String("TRAILER") ||
        nameUpper == QLatin1String("TRANSFER-ENCODING") ||
        nameUpper == QLatin1String("UPGRADE") ||
        nameUpper == QLatin1String("USER-AGENT") ||
        nameUpper == QLatin1String("VIA") ||
        nameUpper.startsWith(QLatin1String("PROXY-")) ||
        nameUpper.startsWith(QLatin1String("SEC-"))) 
        return engine->undefinedValue();

    request->addHeader(nameUpper, value);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_send(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() != QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, "Invalid state");

    if (request->sendFlag())
        return context->throwError(INVALID_STATE_ERR, "Invalid state");

    QByteArray data;
    if (context->argumentCount() > 0)
        data = context->argument(0).toString().toUtf8();

    request->send(data);

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_abort(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    request->abort();

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_getResponseHeader(QScriptContext *context, QScriptEngine *engine)
{
    // ### Implement

    return engine->undefinedValue();
}

static QScriptValue qmlxmlhttprequest_getAllResponseHeaders(QScriptContext *context, QScriptEngine *engine)
{
    // ### Implement

    return engine->undefinedValue();
}

// XMLHttpRequest properties
static QScriptValue qmlxmlhttprequest_readyState(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    return QScriptValue(request->readyState());
}

static QScriptValue qmlxmlhttprequest_status(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() == QmlXMLHttpRequest::Unsent ||
        request->readyState() == QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, "Invalid state");

    if (request->errorFlag())
        return QScriptValue(0);
    else
        return QScriptValue(request->replyStatus());
}

static QScriptValue qmlxmlhttprequest_statusText(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() == QmlXMLHttpRequest::Unsent ||
        request->readyState() == QmlXMLHttpRequest::Opened)
        return context->throwError(INVALID_STATE_ERR, "Invalid state");

    if (request->errorFlag())
        return QScriptValue(0);
    else
        return QScriptValue(request->replyStatusText());
}

static QScriptValue qmlxmlhttprequest_responseText(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (request->readyState() != QmlXMLHttpRequest::Loading &&
        request->readyState() != QmlXMLHttpRequest::Done)
        return QScriptValue(QString());
    else
        return QScriptValue(request->responseBody());
}

static QScriptValue qmlxmlhttprequest_onreadystatechange(QScriptContext *context, QScriptEngine *engine)
{
    QmlXMLHttpRequest *request = qobject_cast<QmlXMLHttpRequest *>(context->thisObject().data().toQObject());
    if (!request) return context->throwError(QScriptContext::ReferenceError, QLatin1String("Not an XMLHttpRequest object"));

    if (context->argumentCount())
        request->setCallback(context->argument(0));

    return request->callback();
}

// Constructor
static QScriptValue qmlxmlhttprequest_new(QScriptContext *context, QScriptEngine *engine)
{
    QScriptValue rv = engine->newObject();
    rv.setPrototype(context->callee().data());
    rv.setData(engine->newQObject(new QmlXMLHttpRequest(QmlEnginePrivate::getEngine(engine)), QScriptEngine::ScriptOwnership));
    return rv;
}

void qt_add_qmlxmlhttprequest(QScriptEngine *engine)
{
    QScriptValue prototype = engine->newObject();

    // Constants
    prototype.setProperty(QLatin1String("UNSENT"), 0, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    prototype.setProperty(QLatin1String("OPENED"), 1, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    prototype.setProperty(QLatin1String("HEADERS_RECEIVED"), 2, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    prototype.setProperty(QLatin1String("LOADING"), 3, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);
    prototype.setProperty(QLatin1String("DONE"), 4, QScriptValue::ReadOnly | QScriptValue::Undeletable | QScriptValue::SkipInEnumeration);

    // Methods
    prototype.setProperty(QLatin1String("open"), engine->newFunction(qmlxmlhttprequest_open, 2));
    prototype.setProperty(QLatin1String("setRequestHeader"), engine->newFunction(qmlxmlhttprequest_setRequestHeader, 2));
    prototype.setProperty(QLatin1String("send"), engine->newFunction(qmlxmlhttprequest_send));
    prototype.setProperty(QLatin1String("abort"), engine->newFunction(qmlxmlhttprequest_abort));
    prototype.setProperty(QLatin1String("getResponseHeader"), engine->newFunction(qmlxmlhttprequest_getResponseHeader, 1));
    prototype.setProperty(QLatin1String("getAllResponseHeaders"), engine->newFunction(qmlxmlhttprequest_getAllResponseHeaders));

    // Read-only properties
    prototype.setProperty(QLatin1String("readyState"), engine->newFunction(qmlxmlhttprequest_readyState), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("status"), engine->newFunction(qmlxmlhttprequest_status), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("statusText"), engine->newFunction(qmlxmlhttprequest_statusText), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("responseText"), engine->newFunction(qmlxmlhttprequest_responseText), QScriptValue::ReadOnly | QScriptValue::PropertyGetter);
    prototype.setProperty(QLatin1String("onreadystatechange"), engine->newFunction(qmlxmlhttprequest_onreadystatechange), QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

    // Constructor
    QScriptValue constructor = engine->newFunction(qmlxmlhttprequest_new);
    constructor.setData(prototype);
    engine->globalObject().setProperty(QLatin1String("XMLHttpRequest"), constructor);
}

#include "qmlxmlhttprequest.moc"
