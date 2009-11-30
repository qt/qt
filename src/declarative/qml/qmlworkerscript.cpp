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
#include <QtCore/qcoreevent.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptengine.h>
#include <private/qmlengine_p.h>
#include <QtCore/qmutex.h>
#include <QtCore/qwaitcondition.h>
#include <QtScript/qscriptvalueiterator.h>
#include <QtCore/qfile.h>

class WorkerDataEvent : public QEvent
{
public:
    enum Type { WorkerData = QEvent::User };

    WorkerDataEvent(int workerId, QmlWorkerScriptEngine::Data *data);
    virtual ~WorkerDataEvent();

    int workerId() const;
    QmlWorkerScriptEngine::Data *takeData();

private:
    int m_id;
    QmlWorkerScriptEngine::Data *m_data;
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

class QmlWorkerScriptEnginePrivate : public QObject
{
public:
    QmlWorkerScriptEnginePrivate();

    QScriptEngine *workerEngine;

    QMutex m_lock;
    QWaitCondition m_wait;

    QScriptValue getWorker(int);

    int m_nextId;
    QHash<int, QScriptValue> m_workers;

    static QVariant scriptValueToVariant(const QScriptValue &);
    static QScriptValue variantToScriptValue(const QVariant &, QScriptEngine *);

    static QScriptValue onmessage(QScriptContext *ctxt, QScriptEngine *engine);

protected:
    virtual bool event(QEvent *);

private:
    void processMessage(int, QmlWorkerScriptEngine::Data *);
    void processLoad(int, const QUrl &);
};

QmlWorkerScriptEnginePrivate::QmlWorkerScriptEnginePrivate()
: workerEngine(0), m_nextId(0)
{
}

QScriptValue QmlWorkerScriptEnginePrivate::onmessage(QScriptContext *ctxt, QScriptEngine *engine)
{
    if (ctxt->argumentCount() >= 1) 
        ctxt->thisObject().setData(ctxt->argument(0));

    return ctxt->thisObject().data();
}

QScriptValue QmlWorkerScriptEnginePrivate::getWorker(int id)
{
    QHash<int, QScriptValue>::Iterator iter = m_workers.find(id);
    if (iter == m_workers.end()) {
        QScriptValue worker = workerEngine->newObject();

        worker.setProperty(QLatin1String("onmessage"), workerEngine->newFunction(onmessage), 
                           QScriptValue::PropertyGetter | QScriptValue::PropertySetter);

        iter = m_workers.insert(id, worker);
    }

    return *iter;
}

bool QmlWorkerScriptEnginePrivate::event(QEvent *event)
{
    if (event->type() == (QEvent::Type)WorkerDataEvent::WorkerData) {
        WorkerDataEvent *workerEvent = static_cast<WorkerDataEvent *>(event);
        processMessage(workerEvent->workerId(), workerEvent->takeData());
        return true;
    } else if (event->type() == (QEvent::Type)WorkerLoadEvent::WorkerLoad) {
        WorkerLoadEvent *workerEvent = static_cast<WorkerLoadEvent *>(event);
        processLoad(workerEvent->workerId(), workerEvent->url());
        return true;
    } else {
        return QObject::event(event);
    }
}

void QmlWorkerScriptEnginePrivate::processMessage(int id, QmlWorkerScriptEngine::Data *data)
{
    QScriptValue worker = getWorker(id);
    QScriptValue onmessage = worker.data();

    if (onmessage.isFunction()) {
        QScriptValue args = workerEngine->newArray(1);
        args.setProperty(0, variantToScriptValue(data->var, workerEngine));

        onmessage.call(worker, args);
    }

    if (data) delete data;
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

        workerEngine->evaluate(script);

        workerEngine->popContext();
    }
}

WorkerDataEvent::WorkerDataEvent(int workerId, QmlWorkerScriptEngine::Data *data)
: QEvent((QEvent::Type)WorkerData), m_id(workerId), m_data(data)
{
}

WorkerDataEvent::~WorkerDataEvent()
{
    if (m_data) { delete m_data; m_data = 0; }
}

int WorkerDataEvent::workerId() const
{
    return m_id;
}

QmlWorkerScriptEngine::Data *WorkerDataEvent::takeData()
{
    QmlWorkerScriptEngine::Data *rv = m_data;
    m_data = 0;
    return rv;
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

QmlWorkerScriptEngine::QmlWorkerScriptEngine(QObject *parent)
: QThread(parent), d(new QmlWorkerScriptEnginePrivate)
{
    d->m_lock.lock();
    start(QThread::LowPriority);
    d->m_wait.wait(&d->m_lock);
    d->moveToThread(this);
}

QmlWorkerScriptEngine::~QmlWorkerScriptEngine()
{
    delete d; d = 0;
}

QmlWorkerScriptEngine::WorkerScript::WorkerScript()
: engine(0), id(0)
{
}

void QmlWorkerScriptEngine::WorkerScript::executeUrl(const QUrl &url)
{
    engine->executeUrl(this, url);
}

void QmlWorkerScriptEngine::WorkerScript::sendMessage(Data *data)
{
    engine->sendMessage(this, data);
}

void QmlWorkerScriptEngine::executeUrl(WorkerScript *script, const QUrl &data)
{
    QCoreApplication::postEvent(d, new WorkerLoadEvent(script->id, data));
}

/*!
    Ownership of \a data transfers to QmlWorkerScriptEngine.  It can not be modified by 
    the caller.
*/
void QmlWorkerScriptEngine::sendMessage(WorkerScript *script, Data *data)
{
    QCoreApplication::postEvent(d, new WorkerDataEvent(script->id, data));
}

QmlWorkerScriptEngine::WorkerScript *QmlWorkerScriptEngine::createWorkerScript()
{
    WorkerScript *rv = new WorkerScript;
    rv->engine = this;
    rv->id = d->m_nextId++;
    return rv;
}

void QmlWorkerScriptEngine::run()
{
    d->m_lock.lock();

    d->workerEngine = new QScriptEngine;

    d->m_wait.wakeAll();

    d->m_lock.unlock();

    exec();

    delete d->workerEngine; d->workerEngine = 0;
}

QmlWorkerScript::QmlWorkerScript(QObject *parent)
: QObject(parent), m_script(0)
{
}

QmlWorkerScript::~QmlWorkerScript()
{
    if (m_script) { delete m_script; m_script = 0; }
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

        quint32 length = (quint32)value.property("length").toNumber();

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
    if (value.type() == QVariant::Bool) {
        return QScriptValue(value.toBool());
    } else if (value.type() == QVariant::String) {
        return QScriptValue(value.toString());
    } else if (value.type() == (QVariant::Type)QMetaType::QReal) {
        return QScriptValue(value.toReal());
    } else if (value.type() == (QVariant::Type)QMetaType::QVariantList) {
        QVariantList list = qvariant_cast<QVariantList>(value);
        QScriptValue rv = engine->newArray(list.count());

        for (quint32 ii = 0; ii < list.count(); ++ii) 
            rv.setProperty(ii, variantToScriptValue(list.at(ii), engine));

        return rv;
    } else if (value.type() == (QVariant::Type)QMetaType::QVariantHash) {

        QVariantHash hash = qvariant_cast<QVariantHash>(value);

        QScriptValue rv = engine->newObject();

        for (QVariantHash::ConstIterator iter = hash.begin(); iter != hash.end(); ++iter) 
            rv.setProperty(iter.key(), variantToScriptValue(iter.value(), engine));

        return rv;
    } else {
        return engine->nullValue();
    }
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

    if (m_script) 
        m_script->executeUrl(m_source);

    emit sourceChanged();
}

void QmlWorkerScript::sendMessage(const QScriptValue &message)
{
    if (!m_script) {
        qWarning("QmlWorkerScript: Attempt to send message before WorkerScript establishment");
        return;
    }

    QmlWorkerScriptEngine::Data *data = new QmlWorkerScriptEngine::Data;
    data->var = QmlWorkerScriptEnginePrivate::scriptValueToVariant(message);
    m_script->sendMessage(data);
}

void QmlWorkerScript::componentComplete()
{
    if (!m_script) {
        QmlEngine *engine = qmlEngine(this);
        if (!engine) {
            qWarning("QmlWorkerScript: componentComplete() called without qmlEngine() set");
            return;
        }
        m_script = QmlEnginePrivate::get(engine)->getWorkerScriptEngine()->createWorkerScript();

        if (m_source.isValid())
            m_script->executeUrl(m_source);
    }
}

QML_DEFINE_TYPE(Qt, 4, 6, WorkerScript, QmlWorkerScript);
