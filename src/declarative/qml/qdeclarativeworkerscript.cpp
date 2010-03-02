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

#include "qdeclarativeworkerscript_p.h"

#include "qdeclarativeengine_p.h"

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
public:
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

protected:
    virtual bool event(QEvent *);

private:
    void processMessage(int, const QVariant &);
    void processLoad(int, const QUrl &);
};

// Currently this will leak as no-one releases it in the worker thread 
class QDeclarativeWorkerListModelAgent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)

public:
    QDeclarativeWorkerListModelAgent(QDeclarativeWorkerListModel *);
    ~QDeclarativeWorkerListModelAgent();

    void addref();
    void release();

    int count() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void append(const QScriptValue &);
    Q_INVOKABLE void insert(int index, const QScriptValue&);
    Q_INVOKABLE QScriptValue get(int index) const;
    Q_INVOKABLE void set(int index, const QScriptValue &);
    Q_INVOKABLE void sync();

    struct VariantRef
    {
        VariantRef() : a(0) {}
        VariantRef(const VariantRef &r) : a(r.a) { if (a) a->addref(); }
        VariantRef(QDeclarativeWorkerListModelAgent *_a) : a(_a) { if (a) a->addref(); }
        ~VariantRef() { if (a) a->release(); }

        VariantRef &operator=(const VariantRef &o) { 
            if (o.a) o.a->addref(); 
            if (a) a->release(); a = o.a; 
            return *this; 
        }

        QDeclarativeWorkerListModelAgent *a;
    };
protected:
    virtual bool event(QEvent *);

private:
    friend class QDeclarativeWorkerScriptEnginePrivate;
    friend class QDeclarativeWorkerListModel;
    QScriptEngine *m_engine;

    struct Change {
        enum { Inserted, Removed, Moved, Changed } type;
        int index; // Inserted/Removed/Moved/Changed
        int count; // Inserted/Removed/Moved/Changed
        int to;    // Moved
    };

    struct Data {
        QHash<int, QString> roles;
        QHash<QString, int> strings;
        QList<QHash<int, QVariant> > values;
        QList<Change> changes;

        void clearChange();
        void insertChange(int index, int count);
        void removeChange(int index, int count);
        void changedChange(int index, int count);
    };
    Data data;

    struct Sync : public QEvent {
        Sync() : QEvent(QEvent::User) {}
        Data data;
    };

    QAtomicInt m_ref;
    QDeclarativeWorkerListModel *m_model;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeWorkerListModelAgent::VariantRef);

QT_BEGIN_NAMESPACE

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

    p->m_lock.lock();
    if (script->owner) 
        QCoreApplication::postEvent(script->owner, 
                                    new WorkerDataEvent(0, scriptValueToVariant(ctxt->argument(0))));
    p->m_lock.unlock();

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
            QVariant v = scriptValueToVariant(ii);
            list << v;
        }

        return QVariant(list);
    } else if (value.isQObject()) {
        QDeclarativeWorkerListModel *lm = qobject_cast<QDeclarativeWorkerListModel *>(value.toQObject());
        if (lm) {
            QDeclarativeWorkerListModelAgent::VariantRef v(lm->agent());
            return qVariantFromValue(v);
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
    } else if (value.userType() == qMetaTypeId<QDeclarativeWorkerListModelAgent::VariantRef>()) {
        QDeclarativeWorkerListModelAgent::VariantRef vr = qvariant_cast<QDeclarativeWorkerListModelAgent::VariantRef>(value);
        if (vr.a->m_engine == 0)
            vr.a->m_engine = engine;
        else if (vr.a->m_engine != engine)
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
    start(QThread::LowPriority);
    d->m_wait.wait(&d->m_lock);
    d->moveToThread(this);
    d->m_lock.unlock();
}

QDeclarativeWorkerScriptEngine::~QDeclarativeWorkerScriptEngine()
{
    qDeleteAll(d->workers);
    delete d; d = 0;
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

QDeclarativeWorkerScript::QDeclarativeWorkerScript(QObject *parent)
: QObject(parent), m_engine(0), m_scriptId(-1)
{
}

QDeclarativeWorkerScript::~QDeclarativeWorkerScript()
{
    if (m_scriptId != -1) m_engine->removeWorkerScript(m_scriptId);
}

QUrl QDeclarativeWorkerScript::source() const
{
    return m_source;
}

void QDeclarativeWorkerScript::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;

    if (m_engine) 
        m_engine->executeUrl(m_scriptId, m_source);

    emit sourceChanged();
}

void QDeclarativeWorkerScript::sendMessage(const QScriptValue &message)
{
    if (!m_engine) {
        qWarning("QDeclarativeWorkerScript: Attempt to send message before WorkerScript establishment");
        return;
    }

    m_engine->sendMessage(m_scriptId, QDeclarativeWorkerScriptEnginePrivate::scriptValueToVariant(message));
}

void QDeclarativeWorkerScript::componentComplete()
{
    if (!m_engine) {
        QDeclarativeEngine *engine = qmlEngine(this);
        if (!engine) {
            qWarning("QDeclarativeWorkerScript: componentComplete() called without qmlEngine() set");
            return;
        }

        m_engine = QDeclarativeEnginePrivate::get(engine)->getWorkerScriptEngine();
        m_scriptId = m_engine->registerWorkerScript(this);

        if (m_source.isValid())
            m_engine->executeUrl(m_scriptId, m_source);
    }
}

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

void QDeclarativeWorkerListModelAgent::Data::clearChange() 
{ 
    changes.clear(); 
}

void QDeclarativeWorkerListModelAgent::Data::insertChange(int index, int count) 
{
    Change c = { Change::Inserted, index, count, 0 };
    changes << c;
}

void QDeclarativeWorkerListModelAgent::Data::removeChange(int index, int count) 
{
    Change c = { Change::Removed, index, count, 0 };
    changes << c;
}

void QDeclarativeWorkerListModelAgent::Data::changedChange(int index, int count)
{
    Change c = { Change::Changed, index, count, 0 };
    changes << c;
}

QDeclarativeWorkerListModelAgent::QDeclarativeWorkerListModelAgent(QDeclarativeWorkerListModel *m)
: m_engine(0), m_ref(1), m_model(m)
{
    data.roles = m_model->m_roles;
    data.strings = m_model->m_strings;
    data.values = m_model->m_values;
}

QDeclarativeWorkerListModelAgent::~QDeclarativeWorkerListModelAgent()
{
}

void QDeclarativeWorkerListModelAgent::addref()
{
    m_ref.ref();
}

void QDeclarativeWorkerListModelAgent::release()
{
    bool del = !m_ref.deref();

    if (del)
        delete this;
}

int QDeclarativeWorkerListModelAgent::count() const
{
    return data.values.count();
}

void QDeclarativeWorkerListModelAgent::clear()
{
    data.clearChange();
    data.removeChange(0, data.values.count());
    data.values.clear();
}

void QDeclarativeWorkerListModelAgent::remove(int index)
{
    if (data.values.count() <= index)
        return;

    data.values.removeAt(index);
    data.removeChange(index, 1);
}

void QDeclarativeWorkerListModelAgent::append(const QScriptValue &value)
{
    QHash<int, QVariant> row;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = data.strings.find(name);
        if (iter == data.strings.end()) {
            int role = data.roles.count();
            data.roles.insert(role, name);
            iter = data.strings.insert(name, role);
        }
        row.insert(*iter, v);
    }

    data.values.append(row);
    data.insertChange(data.values.count() - 1, 1);
}

void QDeclarativeWorkerListModelAgent::insert(int index, const QScriptValue &value)
{
    if (index > data.values.count())
        return;

    QHash<int, QVariant> row;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = data.strings.find(name);
        if (iter == data.strings.end()) {
            int role = data.roles.count();
            data.roles.insert(role, name);
            iter = data.strings.insert(name, role);
        }
        row.insert(*iter, v);
    }

    data.values.insert(index, row);
    data.insertChange(index, 1);
}

void QDeclarativeWorkerListModelAgent::set(int index, const QScriptValue &value)
{
    if (data.values.count() <= index)
        return;

    QHash<int, QVariant> row;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = data.strings.find(name);
        if (iter == data.strings.end()) {
            int role = data.roles.count();
            data.roles.insert(role, name);
            iter = data.strings.insert(name, role);
        }
        row.insert(*iter, v);
    }

    if (data.values.at(index) != row) {
        data.values[index] = row;
        data.changedChange(index, 1);
    }
}

QScriptValue QDeclarativeWorkerListModelAgent::get(int index) const
{
    if (data.values.count() <= index)
        return m_engine->undefinedValue();

    QScriptValue rv = m_engine->newObject();

    QHash<int, QVariant> row = data.values.at(index);
    for (QHash<int, QVariant>::ConstIterator iter = row.begin(); iter != row.end(); ++iter) 
        rv.setProperty(data.roles.value(iter.key()), qScriptValueFromValue(m_engine, iter.value()));

    return rv;
}

void QDeclarativeWorkerListModelAgent::sync()
{
    Sync *s = new Sync;
    s->data = data;
    data.changes.clear();
    QCoreApplication::postEvent(this, s);
}

bool QDeclarativeWorkerListModelAgent::event(QEvent *e)
{
    if (e->type() == QEvent::User) {
        Sync *s = static_cast<Sync *>(e);

        const QList<Change> &changes = s->data.changes;

        if (m_model) {
            bool cc = m_model->m_values.count() != s->data.values.count();

            m_model->m_roles = s->data.roles;
            m_model->m_strings = s->data.strings;
            m_model->m_values = s->data.values;

            for (int ii = 0; ii < changes.count(); ++ii) {
                const Change &change = changes.at(ii);
                switch (change.type) {
                case Change::Inserted:
                    emit m_model->itemsInserted(change.index, change.count);
                    break;
                case Change::Removed:
                    emit m_model->itemsRemoved(change.index, change.count);
                    break;
                case Change::Moved:
                    emit m_model->itemsMoved(change.index, change.to, change.count);
                    break;
                case Change::Changed:
                    emit m_model->itemsMoved(change.index, change.to, change.count);
                    break;
                }
            }

            if (cc)
                emit m_model->countChanged();
        }
    }

    return QObject::event(e);
}

/*!
    \qmlclass WorkerListModel QDeclarativeWorkerListModel
    \brief The WorkerListModel element provides a threaded list model.

    Use WorkerListModel together with WorkerScript to define a list model 
    that is controlled by a separate thread. This is useful if list modification 
    operations are synchronous and take some time: using WorkerListModel
    moves these operations to a different thread and avoids blocking of the
    main GUI thread.

    The thread that creates the WorkerListModel can modify the model for any
    initial set-up requirements. However, once the model has been modified by
    the associated WorkerScript, the model can only be modified by that worker
    script and becomes read-only to all other threads.
    
    Here is an example application that uses WorkerScript to append the
    current time to a WorkerListModel:

    \snippet examples/declarative/workerlistmodel/timedisplay.qml 0

    The included file, \tt dataloader.js, looks like this:

    \snippet examples/declarative/workerlistmodel/dataloader.js 0

    The application's \tt Timer object periodically sends a message to the
    worker script by calling \tt WorkerScript::sendMessage(). When this message
    is received, \tt WorkerScript.onMessage() is invoked in
    \tt dataloader.js, which appends the current time to the worker list
    model.

    Note that unlike ListModel, WorkerListModel does not have \tt move() and
    \tt setProperty() methods.

    \sa WorkerScript, ListModel
*/
QDeclarativeWorkerListModel::QDeclarativeWorkerListModel(QObject *parent)
: QListModelInterface(parent), m_agent(0)
{
}

QDeclarativeWorkerListModel::~QDeclarativeWorkerListModel()
{
    if (m_agent) {
        m_agent->m_model = 0;
        m_agent->release();
    }
}

/*!
    \qmlmethod WorkerListModel::clear()

    Deletes all content from the model. The properties are cleared such that
    different properties may be set on subsequent additions.

    \sa append() remove()
*/
void QDeclarativeWorkerListModel::clear()
{
    if (m_agent) {
        qmlInfo(this) << "List can only be modified from a WorkerScript";
        return;
    }

    int count = m_values.count();
    m_values.clear();
    if (count) {
        emit itemsRemoved(0, count);
        emit countChanged();
    }
}

/*!
    \qmlmethod WorkerListModel::remove(int index)

    Deletes the content at \a index from the model.

    \sa clear()
*/
void QDeclarativeWorkerListModel::remove(int index)
{
    if (m_agent) {
        qmlInfo(this) << "List can only be modified from a WorkerScript";
        return;
    }

    if (m_values.count() <= index)
        return;

    m_values.removeAt(index);
    emit itemsRemoved(index, 1);
    emit countChanged();
}

/*!
    \qmlmethod WorkerListModel::append(jsobject dict)

    Adds a new item to the end of the list model, with the
    values in \a dict.

    \code
        FruitModel.append({"cost": 5.95, "name":"Pizza"})
    \endcode

    \sa set() remove()
*/
void QDeclarativeWorkerListModel::append(const QScriptValue &value)
{
    if (m_agent) {
        qmlInfo(this) << "List can only be modified from a WorkerScript";
        return;
    }

    QHash<int, QVariant> data;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = m_strings.find(name);
        if (iter == m_strings.end()) {
            int role = m_roles.count();
            m_roles.insert(role, name);
            iter = m_strings.insert(name, role);
        }
        data.insert(*iter, v);
    }

    m_values.append(data);

    emit itemsInserted(m_values.count() - 1, 1);
    emit countChanged();
}

/*!
    \qmlmethod WorkerListModel::insert(int index, jsobject dict)

    Adds a new item to the list model at position \a index, with the
    values in \a dict.

    \code
        FruitModel.insert(2, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be to an existing item in the list, or one past
    the end of the list (equivalent to append).

    \sa set() append()
*/
void QDeclarativeWorkerListModel::insert(int index, const QScriptValue &value)
{
    if (m_agent) {
        qmlInfo(this) << "List can only be modified from a WorkerScript";
        return;
    }

    if (index > m_values.count())
        return;

    QHash<int, QVariant> data;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = m_strings.find(name);
        if (iter == m_strings.end()) {
            int role = m_roles.count();
            m_roles.insert(role, name);
            iter = m_strings.insert(name, role);
        }
        data.insert(*iter, v);
    }

    m_values.insert(index, data);
    emit itemsInserted(index, 1);
    emit countChanged();
}

/*!
    \qmlmethod object ListModel::get(int index)

    Returns the item at \a index in the list model.

    \code
        FruitModel.append({"cost": 5.95, "name":"Jackfruit"})
        FruitModel.get(0).cost
    \endcode

    The \a index must be an element in the list.

    Note that properties of the returned object that are themselves objects
    will also be models, and this get() method is used to access elements:

    \code
        FruitModel.append(..., "attributes":
            [{"name":"spikes","value":"7mm"},
             {"name":"color","value":"green"}]);
        FruitModel.get(0).attributes.get(1).value; // == "green"
    \endcode

    \sa append()
*/
QScriptValue QDeclarativeWorkerListModel::get(int index) const
{
    QDeclarativeEngine *engine = qmlEngine(this);
    if (!engine || m_values.count() <= index)
        return QScriptValue();

    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);
    QScriptValue rv = scriptEngine->newObject();

    QHash<int, QVariant> data = m_values.at(index);
    for (QHash<int, QVariant>::ConstIterator iter = data.begin(); iter != data.end(); ++iter) 
        rv.setProperty(m_roles.value(iter.key()), qScriptValueFromValue(scriptEngine, iter.value()));

    return rv;
}

/*!
    \qmlmethod WorkerListModel::set(int index, jsobject dict)

    Changes the item at \a index in the list model with the
    values in \a dict. Properties not appearing in \a valuemap
    are left unchanged.

    \code
        FruitModel.set(3, {"cost": 5.95, "name":"Pizza"})
    \endcode

    The \a index must be an element in the list.

    \sa append()
*/
void QDeclarativeWorkerListModel::set(int index, const QScriptValue &value)
{
    if (m_agent) {
        qmlInfo(this) << "List can only be modified from a WorkerScript";
        return;
    }

    if (m_values.count() <= index)
        return;

    QHash<int, QVariant> data;

    QScriptValueIterator it(value);
    while (it.hasNext()) {
        it.next();
        QString name = it.name();
        QVariant v = it.value().toVariant();

        QHash<QString, int>::Iterator iter = m_strings.find(name);
        if (iter == m_strings.end()) {
            int role = m_roles.count();
            m_roles.insert(role, name);
            iter = m_strings.insert(name, role);
        }
        data.insert(*iter, v);
    }

    if (m_values.at(index) != data) {
        m_values[index] = data;
        emit itemsChanged(index, 1, m_roles.keys());
    }
}

/*!
    \qmlmethod WorkerListModel::sync()

    Writes any unsaved changes to the list model. This must be called after
    changes have been made to the list model in the worker script.

    Note that this method can only be called from the associated worker script.
*/
void QDeclarativeWorkerListModel::sync()
{
    // This is really a dummy method to make it look like sync() exists in
    // WorkerListModel (and not QDeclarativeWorkerListModelAgent) and to let
    // us document sync().
    qmlInfo(this) << "sync() can only be called from a WorkerScript";
}

QDeclarativeWorkerListModelAgent *QDeclarativeWorkerListModel::agent()
{
    if (!m_agent) 
        m_agent = new QDeclarativeWorkerListModelAgent(this);

    return m_agent;
}

QList<int> QDeclarativeWorkerListModel::roles() const
{
    return m_roles.keys();
}

QString QDeclarativeWorkerListModel::toString(int role) const
{
    return m_roles.value(role);
}

/*!
    \qmlproperty int ListModel::count
    The number of data entries in the model.
*/
int QDeclarativeWorkerListModel::count() const
{
    return m_values.count();
}

QHash<int,QVariant> QDeclarativeWorkerListModel::data(int index, const QList<int> &) const
{
    if (m_values.count() <= index)
        return QHash<int, QVariant>();
    else
        return m_values.at(index);
}

QVariant QDeclarativeWorkerListModel::data(int index, int role) const
{
    if (m_values.count() <= index)
        return QVariant();
    else
        return m_values.at(index).value(role);
}

QT_END_NAMESPACE

#include "qdeclarativeworkerscript.moc"

