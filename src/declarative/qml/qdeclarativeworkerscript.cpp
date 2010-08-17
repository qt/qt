/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qdeclarativeworkerscript_p.h"
#include "private/qdeclarativelistmodel_p.h"
#include "private/qdeclarativelistmodelworkeragent_p.h"
#include "private/qdeclarativeengine_p.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptengine.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <QtScript/qscriptvalueiterator.h>
#include <QtCore/qfile.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QtDeclarative/qdeclarativeinfo.h>
#include "qdeclarativenetworkaccessmanagerfactory.h"


QT_BEGIN_NAMESPACE

class WorkerDataEvent : public QEvent
{
public:
    enum Type { WorkerData = QEvent::User };

    WorkerDataEvent(int workerId, const QVariant &data);
    virtual ~WorkerDataEvent();

    int workerId() const;
    QVariant data() const;

private:
    int m_id;
    QVariant m_data;
};

class WorkerLoadEvent : public QEvent
{
public:
    enum Type { WorkerLoad = WorkerDataEvent::WorkerData + 1 };

    WorkerLoadEvent(int workerId, const QUrl &url);

    int workerId() const;
    QUrl url() const;

private:
    int m_id;
    QUrl m_url;
};

class WorkerRemoveEvent : public QEvent
{
public:
    enum Type { WorkerRemove = WorkerLoadEvent::WorkerLoad + 1 };

    WorkerRemoveEvent(int workerId);

    int workerId() const;

private:
    int m_id;
};

class QDeclarativeWorkerScriptEnginePrivate : public QObject
{
    Q_OBJECT
public:
    enum WorkerEventTypes {
        WorkerDestroyEvent = QEvent::User + 100
    };

    QDeclarativeWorkerScriptEnginePrivate(QDeclarativeEngine *eng);

    struct ScriptEngine : public QDeclarativeScriptEngine
    {
        ScriptEngine(QDeclarativeWorkerScriptEnginePrivate *parent) : QDeclarativeScriptEngine(0), p(parent), accessManager(0) {}
        ~ScriptEngine() { delete accessManager; }
        QDeclarativeWorkerScriptEnginePrivate *p;
        QNetworkAccessManager *accessManager;

        virtual QNetworkAccessManager *networkAccessManager() {
            if (!accessManager) {
                if (p->qmlengine && p->qmlengine->networkAccessManagerFactory()) {
                    accessManager = p->qmlengine->networkAccessManagerFactory()->create(this);
                } else {
                    accessManager = new QNetworkAccessManager(this);
                }
            }
            return accessManager;
        }
    };
    ScriptEngine *workerEngine;
    static QDeclarativeWorkerScriptEnginePrivate *get(QScriptEngine *e) {
        return static_cast<ScriptEngine *>(e)->p;
    }

    QDeclarativeEngine *qmlengine;

    QMutex m_lock;
    QWaitCondition m_wait;

    struct WorkerScript {
        WorkerScript();

        int id;
        bool initialized;
        QDeclarativeWorkerScript *owner;
        QScriptValue object;

        QScriptValue callback;
    };

    QHash<int, WorkerScript *> workers;
    QScriptValue getWorker(int);

    int m_nextId;

    static QVariant scriptValueToVariant(const QScriptValue &);
    static QScriptValue variantToScriptValue(const QVariant &, QScriptEngine *);

    static QScriptValue onMessage(QScriptContext *ctxt, QScriptEngine *engine);
    static QScriptValue sendMessage(QScriptContext *ctxt, QScriptEngine *engine);

signals:
    void stopThread();

protected:
    virtual bool event(QEvent *);

private:
    void processMessage(int, const QVariant &);
    void processLoad(int, const QUrl &);
};

QDeclarativeWorkerScriptEnginePrivate::QDeclarativeWorkerScriptEnginePrivate(QDeclarativeEngine *engine)
: workerEngine(0), qmlengine(engine), m_nextId(0)
{
}

QScriptValue QDeclarativeWorkerScriptEnginePrivate::onMessage(QScriptContext *ctxt, QScriptEngine *engine)
{
    QDeclarativeWorkerScriptEnginePrivate *p = QDeclarativeWorkerScriptEnginePrivate::get(engine);

    int id = ctxt->thisObject().data().toVariant().toInt();

    WorkerScript *script = p->workers.value(id);
    if (!script)
        return engine->undefinedValue();

    if (ctxt->argumentCount() >= 1) 
        script->callback = ctxt->argument(0);

    return script->callback;
}

QScriptValue QDeclarativeWorkerScriptEnginePrivate::sendMessage(QScriptContext *ctxt, QScriptEngine *engine)
{
    if (!ctxt->argumentCount())
        return engine->undefinedValue();

    QDeclarativeWorkerScriptEnginePrivate *p = QDeclarativeWorkerScriptEnginePrivate::get(engine);

    int id = ctxt->thisObject().data().toVariant().toInt();

    WorkerScript *script = p->workers.value(id);
    if (!script)
        return engine->undefinedValue();

    QMutexLocker(&p->m_lock);

    if (script->owner)
        QCoreApplication::postEvent(script->owner,
                                    new WorkerDataEvent(0, scriptValueToVariant(ctxt->argument(0))));

    return engine->undefinedValue();
}

QScriptValue QDeclarativeWorkerScriptEnginePrivate::getWorker(int id)
{
    QHash<int, WorkerScript *>::ConstIterator iter = workers.find(id);

    if (iter == workers.end())
        return workerEngine->nullValue();

    WorkerScript *script = *iter;
    if (!script->initialized) {

        script->initialized = true;
        script->object = workerEngine->newObject();

        QScriptValue api = workerEngine->newObject();
        api.setData(script->id);

        api.setProperty(QLatin1String("onMessage"), workerEngine->newFunction(onMessage),
                        QScriptValue::PropertyGetter | QScriptValue::PropertySetter);
        api.setProperty(QLatin1String("sendMessage"), workerEngine->newFunction(sendMessage));

        script->object.setProperty(QLatin1String("WorkerScript"), api);
    }

    return script->object;
}

bool QDeclarativeWorkerScriptEnginePrivate::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)WorkerDataEvent::WorkerData) {
        WorkerDataEvent *workerEvent = static_cast<WorkerDataEvent *>(event);
        processMessage(workerEvent->workerId(), workerEvent->data());
        return true;
    } else if (event->type() == (QEvent::Type)WorkerLoadEvent::WorkerLoad) {
        WorkerLoadEvent *workerEvent = static_cast<WorkerLoadEvent *>(event);
        processLoad(workerEvent->workerId(), workerEvent->url());
        return true;
    } else if (event->type() == (QEvent::Type)WorkerDestroyEvent) {
        emit stopThread();
        return true;
    } else {
        return QObject::event(event);
    }
}

void QDeclarativeWorkerScriptEnginePrivate::processMessage(int id, const QVariant &data)
{
    WorkerScript *script = workers.value(id);
    if (!script)
        return;

    if (script->callback.isFunction()) {
        QScriptValue args = workerEngine->newArray(1);
        args.setProperty(0, variantToScriptValue(data, workerEngine));

        script->callback.call(script->object, args);
    }
}

void QDeclarativeWorkerScriptEnginePrivate::processLoad(int id, const QUrl &url)
{
    if (url.isRelative())
        return;

    QString fileName = QDeclarativeEnginePrivate::urlToLocalFileOrQrc(url);

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        QString script = QString::fromUtf8(data);

        QScriptValue activation = getWorker(id);

        QScriptContext *ctxt = QScriptDeclarativeClass::pushCleanContext(workerEngine);
        QScriptValue urlContext = workerEngine->newObject();
        urlContext.setData(QScriptValue(workerEngine, fileName));
        ctxt->pushScope(urlContext);
        ctxt->pushScope(activation);
        ctxt->setActivationObject(activation);
        QDeclarativeScriptParser::extractPragmas(script);

        workerEngine->baseUrl = url;
        workerEngine->evaluate(script);

        workerEngine->popContext();
    } else {
        qWarning().nospace() << "WorkerScript: Cannot find source file " << url.toString();
    }
}

QVariant QDeclarativeWorkerScriptEnginePrivate::scriptValueToVariant(const QScriptValue &value)
{
    if (value.isBool()) {
        return QVariant(value.toBool());
    } else if (value.isString()) {
        return QVariant(value.toString());
    } else if (value.isNumber()) {
        return QVariant((qreal)value.toNumber());
    } else if (value.isArray()) {
        QVariantList list;

        quint32 length = (quint32)value.property(QLatin1String("length")).toNumber();

        for (quint32 ii = 0; ii < length; ++ii) {
            QVariant v = scriptValueToVariant(value.property(ii));
            list << v;
        }

        return QVariant(list);
    } else if (value.isQObject()) {
        QDeclarativeListModel *lm = qobject_cast<QDeclarativeListModel *>(value.toQObject());
        if (lm) {
            QDeclarativeListModelWorkerAgent *agent = lm->agent();
            if (agent) {
                QDeclarativeListModelWorkerAgent::VariantRef v(agent);
                return qVariantFromValue(v);
            } else {
                return QVariant();
            }
        } else {
            // No other QObject's are allowed to be sent
            return QVariant();
        }
    } else if (value.isObject()) {
        QVariantHash hash;

        QScriptValueIterator iter(value);

        while (iter.hasNext()) {
            iter.next();
            hash.insert(iter.name(), scriptValueToVariant(iter.value()));
        }

        return QVariant(hash);
    }

    return QVariant();

}

QScriptValue QDeclarativeWorkerScriptEnginePrivate::variantToScriptValue(const QVariant &value, QScriptEngine *engine)
{
    if (value.userType() == QVariant::Bool) {
        return QScriptValue(value.toBool());
    } else if (value.userType() == QVariant::String) {
        return QScriptValue(value.toString());
    } else if (value.userType() == QMetaType::QReal) {
        return QScriptValue(value.toReal());
    } else if (value.userType() == qMetaTypeId<QDeclarativeListModelWorkerAgent::VariantRef>()) {
        QDeclarativeListModelWorkerAgent::VariantRef vr = qvariant_cast<QDeclarativeListModelWorkerAgent::VariantRef>(value);
        if (vr.a->scriptEngine() == 0)
            vr.a->setScriptEngine(engine);
        else if (vr.a->scriptEngine() != engine)
            return engine->nullValue();
        QScriptValue o = engine->newQObject(vr.a);
        o.setData(engine->newVariant(value)); // Keeps the agent ref so that it is cleaned up on gc
        return o;
    } else if (value.userType() == QMetaType::QVariantList) {
        QVariantList list = qvariant_cast<QVariantList>(value);
        QScriptValue rv = engine->newArray(list.count());

        for (quint32 ii = 0; ii < quint32(list.count()); ++ii)
            rv.setProperty(ii, variantToScriptValue(list.at(ii), engine));

        return rv;
    } else if (value.userType() == QMetaType::QVariantHash) {

        QVariantHash hash = qvariant_cast<QVariantHash>(value);

        QScriptValue rv = engine->newObject();

        for (QVariantHash::ConstIterator iter = hash.begin(); iter != hash.end(); ++iter)
            rv.setProperty(iter.key(), variantToScriptValue(iter.value(), engine));

        return rv;
    } else {
        return engine->nullValue();
    }
}

WorkerDataEvent::WorkerDataEvent(int workerId, const QVariant &data)
: QEvent((QEvent::Type)WorkerData), m_id(workerId), m_data(data)
{
}

WorkerDataEvent::~WorkerDataEvent()
{
}

int WorkerDataEvent::workerId() const
{
    return m_id;
}

QVariant WorkerDataEvent::data() const
{
    return m_data;
}

WorkerLoadEvent::WorkerLoadEvent(int workerId, const QUrl &url)
: QEvent((QEvent::Type)WorkerLoad), m_id(workerId), m_url(url)
{
}

int WorkerLoadEvent::workerId() const
{
    return m_id;
}

QUrl WorkerLoadEvent::url() const
{
    return m_url;
}

WorkerRemoveEvent::WorkerRemoveEvent(int workerId)
: QEvent((QEvent::Type)WorkerRemove), m_id(workerId)
{
}

int WorkerRemoveEvent::workerId() const
{
    return m_id;
}

QDeclarativeWorkerScriptEngine::QDeclarativeWorkerScriptEngine(QDeclarativeEngine *parent)
: QThread(parent), d(new QDeclarativeWorkerScriptEnginePrivate(parent))
{
    d->m_lock.lock();
    connect(d, SIGNAL(stopThread()), this, SLOT(quit()), Qt::DirectConnection);
    start(QThread::LowPriority);
    d->m_wait.wait(&d->m_lock);
    d->moveToThread(this);
    d->m_lock.unlock();
}

QDeclarativeWorkerScriptEngine::~QDeclarativeWorkerScriptEngine()
{
    d->m_lock.lock();
    qDeleteAll(d->workers);
    d->workers.clear();
    QCoreApplication::postEvent(d, new QEvent((QEvent::Type)QDeclarativeWorkerScriptEnginePrivate::WorkerDestroyEvent));
    d->m_lock.unlock();

    wait();
    d->deleteLater();
}

QDeclarativeWorkerScriptEnginePrivate::WorkerScript::WorkerScript()
: id(-1), initialized(false), owner(0)
{
}

int QDeclarativeWorkerScriptEngine::registerWorkerScript(QDeclarativeWorkerScript *owner)
{
    QDeclarativeWorkerScriptEnginePrivate::WorkerScript *script = new QDeclarativeWorkerScriptEnginePrivate::WorkerScript;
    script->id = d->m_nextId++;
    script->owner = owner;

    d->m_lock.lock();
    d->workers.insert(script->id, script);
    d->m_lock.unlock();

    return script->id;
}

void QDeclarativeWorkerScriptEngine::removeWorkerScript(int id)
{
    QCoreApplication::postEvent(d, new WorkerRemoveEvent(id));
}

void QDeclarativeWorkerScriptEngine::executeUrl(int id, const QUrl &url)
{
    QCoreApplication::postEvent(d, new WorkerLoadEvent(id, url));
}

void QDeclarativeWorkerScriptEngine::sendMessage(int id, const QVariant &data)
{
    QCoreApplication::postEvent(d, new WorkerDataEvent(id, data));
}

void QDeclarativeWorkerScriptEngine::run()
{
    d->m_lock.lock();

    d->workerEngine = new QDeclarativeWorkerScriptEnginePrivate::ScriptEngine(d);

    d->m_wait.wakeAll();

    d->m_lock.unlock();

    exec();

    delete d->workerEngine; d->workerEngine = 0;
}


/*!
    \qmlclass WorkerScript QDeclarativeWorkerScript
  \ingroup qml-utility-elements
    \brief The WorkerScript element enables the use of threads in QML.

    Use WorkerScript to run operations in a new thread.
    This is useful for running operations in the background so
    that the main GUI thread is not blocked.

    Messages can be passed between the new thread and the parent thread
    using \l sendMessage() and the \l {WorkerScript::onMessage}{onMessage()} handler.

    An example:

    \snippet doc/src/snippets/declarative/workerscript.qml 0

    The above worker script specifies a javascript file, "script.js", that handles
    the operations to be performed in the new thread. Here is \c script.js:

    \qml
    WorkerScript.onMessage = function(message) {
        // ... long-running operations and calculations are done here
        WorkerScript.sendMessage({ 'reply': 'Mouse is at ' + message.x + ',' + message.y })
    }
    \endqml

    When the user clicks anywhere within the rectangle, \c sendMessage() is
    called, triggering the \tt WorkerScript.onMessage() handler in
    \tt script.js. This in turn sends a reply message that is then received
    by the \tt onMessage() handler of \tt myWorker.

    \sa {declarative/threading/workerscript}{WorkerScript example},
        {declarative/threading/threadedlistmodel}{Threaded ListModel example}
*/
QDeclarativeWorkerScript::QDeclarativeWorkerScript(QObject *parent)
: QObject(parent), m_engine(0), m_scriptId(-1), m_componentComplete(true)
{
}

QDeclarativeWorkerScript::~QDeclarativeWorkerScript()
{
    if (m_scriptId != -1) m_engine->removeWorkerScript(m_scriptId);
}

/*!
    \qmlproperty url WorkerScript::source

    This holds the url of the JavaScript file that implements the
    \tt WorkerScript.onMessage() handler for threaded operations.
*/
QUrl QDeclarativeWorkerScript::source() const
{
    return m_source;
}

void QDeclarativeWorkerScript::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;

    if (engine())
        m_engine->executeUrl(m_scriptId, m_source);

    emit sourceChanged();
}

/*!
    \qmlmethod WorkerScript::sendMessage(jsobject message)

    Sends the given \a message to a worker script handler in another
    thread. The other worker script handler can receive this message
    through the onMessage() handler.
*/
void QDeclarativeWorkerScript::sendMessage(const QScriptValue &message)
{
    if (!engine()) {
        qWarning("QDeclarativeWorkerScript: Attempt to send message before WorkerScript establishment");
        return;
    }

    m_engine->sendMessage(m_scriptId, QDeclarativeWorkerScriptEnginePrivate::scriptValueToVariant(message));
}

void QDeclarativeWorkerScript::classBegin()
{
    m_componentComplete = false;
}

QDeclarativeWorkerScriptEngine *QDeclarativeWorkerScript::engine()
{
    if (m_engine) return m_engine;
    if (m_componentComplete) {
        QDeclarativeEngine *engine = qmlEngine(this);
        if (!engine) {
            qWarning("QDeclarativeWorkerScript: engine() called without qmlEngine() set");
            return 0;
        }

        m_engine = QDeclarativeEnginePrivate::get(engine)->getWorkerScriptEngine();
        m_scriptId = m_engine->registerWorkerScript(this);

        if (m_source.isValid())
            m_engine->executeUrl(m_scriptId, m_source);

        return m_engine;
    }
    return 0;
}

void QDeclarativeWorkerScript::componentComplete()
{
    m_componentComplete = true;
    engine(); // Get it started now.
}

/*!
    \qmlsignal WorkerScript::onMessage(jsobject msg)

    This handler is called when a message \a msg is received from a worker
    script in another thread through a call to sendMessage().
*/

bool QDeclarativeWorkerScript::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)WorkerDataEvent::WorkerData) {
        QDeclarativeEngine *engine = qmlEngine(this);
        if (engine) {
            QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);
            WorkerDataEvent *workerEvent = static_cast<WorkerDataEvent *>(event);
            QScriptValue value =
                QDeclarativeWorkerScriptEnginePrivate::variantToScriptValue(workerEvent->data(), scriptEngine);
            emit message(value);
        }
        return true;
    } else {
        return QObject::event(event);
    }
}

QT_END_NAMESPACE

#include <qdeclarativeworkerscript.moc>

