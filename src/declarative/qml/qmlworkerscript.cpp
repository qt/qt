/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlworkerscript_p.h"

#include "qmlengine_p.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptengine.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <QtScript/qscriptvalueiterator.h>
#include <QtCore/qfile.h>

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

class QmlWorkerScriptEnginePrivate : public QObject
{
public:
    QmlWorkerScriptEnginePrivate();

    struct ScriptEngine : public QmlScriptEngine 
    {
        ScriptEngine(QmlWorkerScriptEnginePrivate *parent) : QmlScriptEngine(0), p(parent) {}
        QmlWorkerScriptEnginePrivate *p;
    };
    ScriptEngine *workerEngine;
    static QmlWorkerScriptEnginePrivate *get(QScriptEngine *e) {
        return static_cast<ScriptEngine *>(e)->p;
    }

    QMutex m_lock;
    QWaitCondition m_wait;

    struct WorkerScript {
        WorkerScript();

        int id;
        bool initialized;
        QmlWorkerScript *owner;
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

protected:
    virtual bool event(QEvent *);

private:
    void processMessage(int, const QVariant &);
    void processLoad(int, const QUrl &);
};

QmlWorkerScriptEnginePrivate::QmlWorkerScriptEnginePrivate()
: workerEngine(0), m_nextId(0)
{
}

QScriptValue QmlWorkerScriptEnginePrivate::onMessage(QScriptContext *ctxt, QScriptEngine *engine)
{
    QmlWorkerScriptEnginePrivate *p = QmlWorkerScriptEnginePrivate::get(engine);

    int id = ctxt->thisObject().data().toVariant().toInt();

    WorkerScript *script = p->workers.value(id);
    if (!script)
        return engine->undefinedValue();

    if (ctxt->argumentCount() >= 1) 
        script->callback = ctxt->argument(0);

    return script->callback;
}

QScriptValue QmlWorkerScriptEnginePrivate::sendMessage(QScriptContext *ctxt, QScriptEngine *engine)
{
    if (!ctxt->argumentCount())
        return engine->undefinedValue();

    QmlWorkerScriptEnginePrivate *p = QmlWorkerScriptEnginePrivate::get(engine);

    int id = ctxt->thisObject().data().toVariant().toInt();

    WorkerScript *script = p->workers.value(id);
    if (!script) 
        return engine->undefinedValue();

    p->m_lock.lock();
    if (script->owner) 
        QCoreApplication::postEvent(script->owner, 
                                    new WorkerDataEvent(0, scriptValueToVariant(ctxt->argument(0))));
    p->m_lock.unlock();

    return engine->undefinedValue();
}

QScriptValue QmlWorkerScriptEnginePrivate::getWorker(int id)
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

bool QmlWorkerScriptEnginePrivate::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)WorkerDataEvent::WorkerData) {
        WorkerDataEvent *workerEvent = static_cast<WorkerDataEvent *>(event);
        processMessage(workerEvent->workerId(), workerEvent->data());
        return true;
    } else if (event->type() == (QEvent::Type)WorkerLoadEvent::WorkerLoad) {
        WorkerLoadEvent *workerEvent = static_cast<WorkerLoadEvent *>(event);
        processLoad(workerEvent->workerId(), workerEvent->url());
        return true;
    } else {
        return QObject::event(event);
    }
}

void QmlWorkerScriptEnginePrivate::processMessage(int id, const QVariant &data)
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

void QmlWorkerScriptEnginePrivate::processLoad(int id, const QUrl &url)
{
    if (url.isRelative() || url.scheme() != QLatin1String("file"))
        return;

    QString fileName = url.toLocalFile();

    QFile f(fileName);
    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        QString script = QString::fromUtf8(data);

        QScriptValue activation = getWorker(id);

        QScriptContext *ctxt = workerEngine->pushContext();
        ctxt->setActivationObject(activation);

        workerEngine->baseUrl = url;
        workerEngine->evaluate(script);

        workerEngine->popContext();
    }
}

QVariant QmlWorkerScriptEnginePrivate::scriptValueToVariant(const QScriptValue &value)
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
            QVariant v = scriptValueToVariant(ii);
            list << v;
        }

        return QVariant(list);
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

QScriptValue QmlWorkerScriptEnginePrivate::variantToScriptValue(const QVariant &value, QScriptEngine *engine)
{
    if (value.userType() == QVariant::Bool) {
        return QScriptValue(value.toBool());
    } else if (value.userType() == QVariant::String) {
        return QScriptValue(value.toString());
    } else if (value.userType() == QMetaType::QReal) {
        return QScriptValue(value.toReal());
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

QmlWorkerScriptEngine::QmlWorkerScriptEngine(QObject *parent)
: QThread(parent), d(new QmlWorkerScriptEnginePrivate)
{
    d->m_lock.lock();
    start(QThread::LowPriority);
    d->m_wait.wait(&d->m_lock);
    d->moveToThread(this);
    d->m_lock.unlock();
}

QmlWorkerScriptEngine::~QmlWorkerScriptEngine()
{
    delete d; d = 0;
}

QmlWorkerScriptEnginePrivate::WorkerScript::WorkerScript()
: id(-1), initialized(false), owner(0)
{
}

int QmlWorkerScriptEngine::registerWorkerScript(QmlWorkerScript *owner)
{
    QmlWorkerScriptEnginePrivate::WorkerScript *script = new QmlWorkerScriptEnginePrivate::WorkerScript;
    script->id = d->m_nextId++;
    script->owner = owner;

    d->m_lock.lock();
    d->workers.insert(script->id, script);
    d->m_lock.unlock();

    return script->id;
}

void QmlWorkerScriptEngine::removeWorkerScript(int id)
{
    QCoreApplication::postEvent(d, new WorkerRemoveEvent(id));
}

void QmlWorkerScriptEngine::executeUrl(int id, const QUrl &url)
{
    QCoreApplication::postEvent(d, new WorkerLoadEvent(id, url));
}

void QmlWorkerScriptEngine::sendMessage(int id, const QVariant &data)
{
    QCoreApplication::postEvent(d, new WorkerDataEvent(id, data));
}

void QmlWorkerScriptEngine::run()
{
    d->m_lock.lock();

    d->workerEngine = new QmlWorkerScriptEnginePrivate::ScriptEngine(d);

    d->m_wait.wakeAll();

    d->m_lock.unlock();

    exec();

    delete d->workerEngine; d->workerEngine = 0;
}

QmlWorkerScript::QmlWorkerScript(QObject *parent)
: QObject(parent), m_engine(0), m_scriptId(-1)
{
}

QmlWorkerScript::~QmlWorkerScript()
{
    if (m_scriptId != -1) m_engine->removeWorkerScript(m_scriptId);
}

QUrl QmlWorkerScript::source() const
{
    return m_source;
}

void QmlWorkerScript::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;

    if (m_engine) 
        m_engine->executeUrl(m_scriptId, m_source);

    emit sourceChanged();
}

void QmlWorkerScript::sendMessage(const QScriptValue &message)
{
    if (!m_engine) {
        qWarning("QmlWorkerScript: Attempt to send message before WorkerScript establishment");
        return;
    }

    m_engine->sendMessage(m_scriptId, QmlWorkerScriptEnginePrivate::scriptValueToVariant(message));
}

void QmlWorkerScript::componentComplete()
{
    if (!m_engine) {
        QmlEngine *engine = qmlEngine(this);
        if (!engine) {
            qWarning("QmlWorkerScript: componentComplete() called without qmlEngine() set");
            return;
        }

        m_engine = QmlEnginePrivate::get(engine)->getWorkerScriptEngine();
        m_scriptId = m_engine->registerWorkerScript(this);

        if (m_source.isValid())
            m_engine->executeUrl(m_scriptId, m_source);
    }
}

bool QmlWorkerScript::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)WorkerDataEvent::WorkerData) {
        QmlEngine *engine = qmlEngine(this);
        if (engine) {
            QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
            WorkerDataEvent *workerEvent = static_cast<WorkerDataEvent *>(event);
            QScriptValue value = 
                QmlWorkerScriptEnginePrivate::variantToScriptValue(workerEvent->data(), scriptEngine);
            emit message(value);
        }
        return true;
    } else {
        return QObject::event(event);
    }
}


QML_DEFINE_TYPE(Qt, 4, 6, WorkerScript, QmlWorkerScript);
