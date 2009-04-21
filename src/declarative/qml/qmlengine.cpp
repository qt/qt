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

#include <QMetaProperty>
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>

#ifdef QT_SCRIPTTOOLS_LIB
#include <QScriptEngineDebugger>
#endif

#include <QScriptClass>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QList>
#include <QPair>
#include <QDebug>
#include <QMetaObject>
#include "qml.h"
#include <qfxperf.h>
#include <QStack>
#include "private/qmlbasicscript_p.h"
#include "private/qmlcompiledcomponent_p.h"
#include "qmlengine.h"
#include "qmlcontext.h"
#include "qmlexpression.h"
#include <QtCore/qthreadstorage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <qmlcomponent.h>
#include "private/qmlmetaproperty_p.h"


QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(bindValueDebug, QML_BINDVALUE_DEBUG);
#ifdef QT_SCRIPTTOOLS_LIB
DEFINE_BOOL_CONFIG_OPTION(debuggerEnabled, QML_DEBUGGER);
#endif

Q_DECLARE_METATYPE(QmlMetaProperty);

QML_DEFINE_TYPE(QObject,Object);

static QScriptValue qmlMetaProperty_emit(QScriptContext *ctx, QScriptEngine *engine)
{
    QmlMetaProperty mp = qscriptvalue_cast<QmlMetaProperty>(ctx->thisObject());
    if(mp.type() & QmlMetaProperty::Signal)
        mp.emitSignal();
    return engine->nullValue();
}

struct StaticQtMetaObject : public QObject
{
    static const QMetaObject *get()
        { return &static_cast<StaticQtMetaObject*> (0)->staticQtMetaObject; }
};


struct QmlEngineStack {
    QmlEngineStack();

    QStack<QmlEngine *> mainThreadEngines;
    QThread *mainThread;

    QThreadStorage<QStack<QmlEngine *> *> storage;

    QStack<QmlEngine *> *engines();
};

Q_GLOBAL_STATIC(QmlEngineStack, engineStack);

QmlEngineStack::QmlEngineStack()
: mainThread(0)
{
}

QStack<QmlEngine *> *QmlEngineStack::engines()
{
    if(mainThread== 0) {
        if(!QCoreApplication::instance())
            return 0;
        mainThread = QCoreApplication::instance()->thread();
    }

    // Note: This is very slightly faster than just using the thread storage
    // for everything.
    QStack<QmlEngine *> *engines = 0;
    if(QThread::currentThread() == mainThread) {
        engines = &mainThreadEngines;
    } else {
        engines = storage.localData();
        if(!engines) {
            engines = new QStack<QmlEngine *>;
            storage.setLocalData(engines);
        }
    }
    return engines;
}


QmlEnginePrivate::QmlEnginePrivate(QmlEngine *e)
: rootContext(0), currentBindContext(0), currentExpression(0), q(e),
  rootComponent(0), networkAccessManager(0), typeManager(e)
{
    QScriptValue proto = scriptEngine.newObject();
    proto.setProperty(QLatin1String("emit"), 
                      scriptEngine.newFunction(qmlMetaProperty_emit));
    scriptEngine.setDefaultPrototype(qMetaTypeId<QmlMetaProperty>(), proto);

    QScriptValue qtObject = scriptEngine.newQMetaObject(StaticQtMetaObject::get());
    scriptEngine.globalObject().setProperty(QLatin1String("Qt"), qtObject);
}

QmlEnginePrivate::~QmlEnginePrivate()
{
    delete rootContext;
    rootContext = 0;
    delete contextClass;
    contextClass = 0;
    delete objectClass;
    objectClass = 0;
    delete networkAccessManager;
    networkAccessManager = 0;
}

void QmlEnginePrivate::init()
{
    contextClass = new QmlContextScriptClass(q);
    objectClass = new QmlObjectScriptClass(q);
    rootContext = new QmlContext(q);
#ifdef QT_SCRIPTTOOLS_LIB
    if(debuggerEnabled()){
        debugger = new QScriptEngineDebugger(q);
        debugger->attachTo(&scriptEngine);
    }
#endif
}

QmlContext *QmlEnginePrivate::setCurrentBindContext(QmlContext *c)
{
    QmlContext *old = currentBindContext;
    currentBindContext = c;
    return old;
}

////////////////////////////////////////////////////////////////////
typedef QHash<QPair<const QMetaObject *, QString>, bool> FunctionCache;
Q_GLOBAL_STATIC(FunctionCache, functionCache);

QScriptClass::QueryFlags
QmlEnginePrivate::queryObject(const QString &propName,
                                  uint *id, QObject *obj)
{
    QScriptClass::QueryFlags rv = 0;

    QmlMetaProperty prop(obj, propName);
    if(prop.type() == QmlMetaProperty::Invalid) {
        QPair<const QMetaObject *, QString> key =
            qMakePair(obj->metaObject(), propName);
        bool isFunction = false;
        if (functionCache()->contains(key)) {
            isFunction = functionCache()->value(key);
        } else {
            QScriptValue sobj = scriptEngine.newQObject(obj);
            QScriptValue func = sobj.property(propName);
            isFunction = func.isFunction();
            functionCache()->insert(key, isFunction);
        }

        if(isFunction) {
            *id = QmlScriptClass::FunctionId;
            rv |= QScriptClass::HandlesReadAccess;
        }
    } else {
        *id = QmlScriptClass::PropertyId;
        *id |= prop.save();

        rv |= QScriptClass::HandlesReadAccess;
        if(prop.isWritable())
            rv |= QScriptClass::HandlesWriteAccess;
    }

    return rv;
}

QScriptValue QmlEnginePrivate::propertyObject(const QScriptString &propName,
                                                  QObject *obj, uint id)
{
    if(id == QmlScriptClass::FunctionId) {
        QScriptValue sobj = scriptEngine.newQObject(obj);
        QScriptValue func = sobj.property(propName);
        return func;
    } else {
        QmlMetaProperty prop;
        prop.restore(id, obj);
        if(!prop.isValid())
            return QScriptValue();

        if(prop.type() & QmlMetaProperty::Signal) {
            return scriptEngine.newVariant(qVariantFromValue(prop));
        } else {
            QVariant var = prop.read();
            capturedProperties << prop;
            QObject *varobj = QmlMetaType::toQObject(var);
            if(!varobj)
                varobj = qvariant_cast<QObject *>(var);
            if(varobj) {
                return scriptEngine.newObject(objectClass, scriptEngine.newVariant(QVariant::fromValue(varobj)));
            } else {
                if (var.type() == QVariant::Bool)
                    return QScriptValue(&scriptEngine, var.toBool());
                return scriptEngine.newVariant(var);
            }
        }
    }

    return QScriptValue();
}

void QmlEnginePrivate::contextActivated(QmlContext *)
{
    Q_Q(QmlEngine);
    QmlEngineStack *stack = engineStack();
    if(!stack)
        return;
    QStack<QmlEngine *> *engines = stack->engines();
    if(engines)
        engines->push(q);
}

void QmlEnginePrivate::contextDeactivated(QmlContext *)
{
    QmlEngineStack *stack = engineStack();
    if(!stack)
        return;
    QStack<QmlEngine *> *engines = stack->engines();
    if(engines) {
        Q_ASSERT(engines->top() == q_func());
        engines->pop();
    }
}


////////////////////////////////////////////////////////////////////

bool QmlEnginePrivate::fetchCache(QmlBasicScriptNodeCache &cache, const QString &propName, QObject *obj)
{
    QmlMetaProperty prop(obj, propName);

    if(!prop.isValid())
        return false;

    capturedProperties << prop;

    if(prop.type() & QmlMetaProperty::Attached) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::Attached;
        cache.attached = prop.d->attachedObject();
        return true;

    } else if(prop.type() & QmlMetaProperty::Property) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::Core;
        cache.core = prop.property().propertyIndex();
        cache.coreType = prop.propertyType();
        return true;

    } else if(prop.type() & QmlMetaProperty::SignalProperty) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::SignalProperty;
        cache.core = prop.coreIndex();
        return true;

    } else if(prop.type() & QmlMetaProperty::Signal) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::Signal;
        cache.core = prop.coreIndex();
        return true;

    }

    return false;
}

bool QmlEnginePrivate::loadCache(QmlBasicScriptNodeCache &cache, const QString &propName, QmlContextPrivate *context)
{
    while(context) {
        if(context->variantProperties.contains(propName)) {
            cache.object = 0;
            cache.type = QmlBasicScriptNodeCache::Variant;
            cache.context = context;
            return true;
        }

        if(context->properties.contains(propName)) {
            cache.object = context->properties[propName];
            cache.type = QmlBasicScriptNodeCache::Explicit;
            return true;
        }

        foreach(QObject *obj, context->defaultObjects) {
            if(fetchCache(cache, propName, obj))
                return true;
        }

        if(context->parent)
            context = context->parent->d_func();
        else
            context = 0;
    }
    return false;
}


/*!
    \class QmlEngine
    \brief The QmlEngine class provides an environment for instantiating QML components.
    \mainclass

    Each QML component is instantiated in a QmlContext.  QmlContext's are 
    essential for passing data to QML components.  In QML, contexts are arranged
    hierarchically and this hierarchy is managed by the QmlEngine.

    Prior to creating any QML components, an application must have created a
    QmlEngine to gain access to a QML context.  The following example shows how
    to create a simple Text item.
    
    \code
    QmlEngine engine;
    QmlComponent component("<Text text=\"Hello world!\"/>");
    QFxItem *item = qobject_cast<QFxItem *>(component.create(&engine));

    //add item to view, etc
    ...
    \endcode

    In this case, the Text item will be created in the engine's
    \l {QmlEngine::rootContext()}{root context}.

    \sa QmlComponent QmlContext
*/

/*!
    Create a new QmlEngine with the given \a parent.
*/
QmlEngine::QmlEngine(QObject *parent)
: QObject(*new QmlEnginePrivate(this), parent)
{
    Q_D(QmlEngine);
    d->init();

    qRegisterMetaType<QVariant>("QVariant");
}

/*!
    Destroys the QmlEngine.

    Any QmlContext's created on this engine will be invalidated, but not 
    destroyed (unless they are parented to the QmlEngine object).
*/
QmlEngine::~QmlEngine()
{
}

void QmlEngine::clearComponentCache()
{
    Q_D(QmlEngine);
    d->typeManager.clearCache();
}

/*!
    Returns the engine's root context.  

    The root context is automatically created by the QmlEngine.  Data that 
    should be available to all QML component instances instantiated by the
    engine should be put in the root context.

    Additional data that should only be available to a subset of component 
    instances should be added to sub-contexts parented to the root context.
*/
QmlContext *QmlEngine::rootContext()
{
    Q_D(QmlEngine);
    return d->rootContext;
}

/*!
    Returns this engine's active context, or 0 if no context is active on this 
    engine.

    Contexts are activated and deactivated by calling QmlContext::activate() and
    QmlContext::deactivate() respectively.

    Context activation holds no special semantic, other than it allows types
    instantiated by QML to access "their" context without having it passed as
    a parameter in their constructor, as shown below.
    \code
    class MyClass : ... {
    ...
        MyClass() { 
            qWarning() << "I was instantiated in this context:" 
                       << QmlContext::activeContext();
        }
    };
    \endcode
*/
QmlContext *QmlEngine::activeContext()
{
    Q_D(QmlEngine);
    if(d->currentBindContext)
        return d->currentBindContext;
    else
        return 0;
}

/*!
    Sets the mappings from namespace URIs to URL to \a map.

    \sa nameSpacePaths()
*/
void QmlEngine::setNameSpacePaths(const QMap<QString,QString>& map)
{
    Q_D(QmlEngine);
    d->nameSpacePaths = map;
}

/*!
    Adds mappings (given by \a map) from namespace URIs to URL.

    \sa nameSpacePaths()
*/
void QmlEngine::addNameSpacePaths(const QMap<QString,QString>& map)
{
    Q_D(QmlEngine);
    d->nameSpacePaths.unite(map);
}

/*!
    Adds a mapping from namespace URI \a ns to URL \a path.

    \sa nameSpacePaths()
*/
void QmlEngine::addNameSpacePath(const QString& ns, const QString& path)
{
    Q_D(QmlEngine);
    d->nameSpacePaths.insertMulti(ns,path);
}

/*!
    Returns the mapping from namespace URIs to URLs.

    Namespaces in QML allow types to be specified by a URI,
    using standard XML namespaces:

    \code
        <Item xmlns:foo="xyz://abc/def">
            <foo:Bar/>
        </Item>
    \endcode

    Actual QML types can be defined in URLs, in which case a mapping
    may be made from URIs (such as "xyz://abc/def/Bar.qml" above, to
    URLs (such as "file:///opt/abcdef/Bar.qml"):

    \code
        engine->addNameSpacePath("xyz://abc/def","file:///opt/abcdef");
    \endcode

    If only a prefix of the URI is mapped, the path of the URI is
    mapped similarly to the URL:

    \code
        engine->addNameSpacePath("xyz://abc","file:///opt/jkl");
    \endcode

    In the above case, "xyz://abc/def/Bar.qml" would then map to
    "file:///opt/jkl/def/Bar.qml".

    \sa componentUrl()
*/
QMap<QString,QString> QmlEngine::nameSpacePaths() const
{
    Q_D(const QmlEngine);
    return d->nameSpacePaths;
}

/*!
    Returns the URL for the component source \a src, as mapped
    by the nameSpacePaths(), resolved relative to \a baseUrl.

    \sa nameSpacePaths()
*/
QUrl QmlEngine::componentUrl(const QUrl& src, const QUrl& baseUrl) const
{
    Q_D(const QmlEngine);

    // Find the most-specific namespace matching src.
    // For files, multiple paths can be given, the first found is used.
    QUrl r;
    QMap<QString, QString>::const_iterator i = d->nameSpacePaths.constBegin();
    QString rns=QLatin1String(":"); // ns of r, if file found, initial an imposible namespace
    QString srcstring = src.toString();
    while (i != d->nameSpacePaths.constEnd()) {
        QString ns = i.key();
        QString path = i.value();
        if (ns != rns) {
            if (srcstring.startsWith(ns) && (ns.length()==0 || srcstring[ns.length()]==QLatin1Char('/'))) {
                QString file = ns.length()==0 ? srcstring : srcstring.mid(ns.length()+1);
                QUrl cr = baseUrl.resolved(QUrl(path + QLatin1String("/") + file));
                QString lf = cr.toLocalFile();
                if (lf.isEmpty() || QFile::exists(lf)) {
                    r = cr;
                    rns = ns;
                }
            }
        }
        ++i;
    }
    if (r.isEmpty())
        r = baseUrl.resolved(src);
    return r;
}

/*!
    Sets the common QNetworkAccessManager, \a network, used by all QML elements instantiated
    by this engine.

    Any previously set manager is deleted and \a network is owned by the QmlEngine.  This
    method should only be called before any QmlComponents are instantiated.
*/
void QmlEngine::setNetworkAccessManager(QNetworkAccessManager *network)
{
    Q_D(QmlEngine);
    delete d->networkAccessManager;
    d->networkAccessManager = network;
}

/*!
    Returns the common QNetworkAccessManager used by all QML elements instantiated by
    this engine.

    The default implements no caching, cookiejar, etc., just a default 
    QNetworkAccessManager.
*/
QNetworkAccessManager *QmlEngine::networkAccessManager() const
{
    Q_D(const QmlEngine);
    if(!d->networkAccessManager) 
        d->networkAccessManager = new QNetworkAccessManager;
    return d->networkAccessManager;
}

/*! \internal */
QScriptEngine *QmlEngine::scriptEngine()
{
    Q_D(QmlEngine);
    return &d->scriptEngine;
}

/*!
    Returns the currently active QmlEngine.

    The active engine is the engine associated with the last activated 
    QmlContext.  This method is thread-safe - the "active" engine is maintained
    independently for each thread.
*/
QmlEngine *QmlEngine::activeEngine()
{
    QmlEngineStack *stack = engineStack();
    if(!stack) return 0;

    QStack<QmlEngine *> *engines = stack->engines();
    if(!engines) {
        qWarning("QmlEngine::activeEngine() cannot be called before the construction of QCoreApplication");
        return 0;
    }

    if(engines->isEmpty())
        return 0;
    else
        return engines->top();
}



QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b)
: q(b), ctxt(0), sseData(0), proxy(0), me(0), trackChange(false)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, void *expr, QmlRefCount *rc)
: q(b), ctxt(0), sse((const char *)expr, rc), sseData(0), proxy(0), me(0), trackChange(true)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, const QString &expr, bool ssecompile)
: q(b), ctxt(0), expression(expr), sseData(0), proxy(0), me(0), trackChange(true)
{
    if(ssecompile) {
#ifdef Q_ENABLE_PERFORMANCE_LOG
        QFxPerfTimer<QFxPerf::BindCompile> pt;
#endif
        sse.compile(expr.toLatin1());
    }
}

QmlExpressionPrivate::~QmlExpressionPrivate()
{
    sse.deleteScriptState(sseData);
    sseData = 0;
    delete proxy;
}

/*!
    Create an invalid QmlExpression.

    As the expression will not have an associated QmlContext, this will be a 
    null expression object and its value will always be an invalid QVariant.
 */
QmlExpression::QmlExpression()
: d(new QmlExpressionPrivate(this))
{
}

/*! \internal */
QmlExpression::QmlExpression(QmlContext *ctxt, void *expr, 
                             QmlRefCount *rc, QObject *me)
: d(new QmlExpressionPrivate(this, expr, rc))
{
    d->ctxt = ctxt;
    d->me = me;
}

/*! \internal */
QmlExpression::QmlExpression(QmlContext *ctxt, const QString &expr, 
                             QObject *me, bool ssecompile)
: d(new QmlExpressionPrivate(this, expr, ssecompile))
{
    d->ctxt = ctxt;
    d->me = me;
}

/*!
    Create a QmlExpression object.

    The \a expression ECMAScript will be executed in the \a ctxt QmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QmlExpression::QmlExpression(QmlContext *ctxt, const QString &expression, 
                             QObject *scope)
: d(new QmlExpressionPrivate(this, expression, true))
{
    d->ctxt = ctxt;
    d->me = scope;
}

/*!
    Destroy the QmlExpression instance.
*/
QmlExpression::~QmlExpression()
{
    delete d; d = 0;
}

/*!
    Returns the QmlEngine this expression is associated with, or 0 if there
    is no association or the QmlEngine has been destroyed.
*/
QmlEngine *QmlExpression::engine() const
{
    return d->ctxt->engine();
}

/*!
    Returns teh QmlContext this expression is associated with, or 0 if there
    is no association or the QmlContext has been destroyed.
*/
QmlContext *QmlExpression::context() const
{
    return d->ctxt;
}

/*!
    Returns the expression string.
*/
QString QmlExpression::expression() const
{
    if(d->sse.isValid())
        return QLatin1String(d->sse.expression());
    else
        return d->expression;
}

/*!
    Clear the expression.
*/
void QmlExpression::clearExpression()
{
    setExpression(QString());
}

/*!
    Set the expression to \a expression.
*/
void QmlExpression::setExpression(const QString &expression)
{
    if(d->sseData) {
        d->sse.deleteScriptState(d->sseData);
        d->sseData = 0;
    }

    delete d->proxy; d->proxy = 0;

    d->expression = expression;

    if(d->expression.isEmpty())
        d->sse.clear();
    else
        d->sse.compile(expression.toLatin1());
}

/*!
    Called by QmlExpression each time the expression value changes from the
    last time it was evaluated.  The expression must have been evaluated at 
    least once (by calling QmlExpression::value()) before this callback will
    be made.

    The default implementation does nothing.
*/
void QmlExpression::valueChanged()
{
}

Q_DECLARE_METATYPE(QList<QObject *>);

void BindExpressionProxy::changed()
{
    e->valueChanged();
}

/*!
    Returns the value of the expression, or an invalid QVariant if the 
    expression is invalid or has an error.
*/
QVariant QmlExpression::value()
{
    if(bindValueDebug())
        qWarning() << "QmlEngine: Evaluating:" << expression();
    QVariant rv;
    if(!d->ctxt || (!d->sse.isValid() && d->expression.isEmpty()))
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValue> perf;
#endif

    QmlBasicScript::CacheState cacheState = QmlBasicScript::Reset;

    QmlEnginePrivate *ep = engine()->d_func();
    QmlExpression *lastCurrentExpression = ep->currentExpression;
    ep->currentExpression = this;
    if(d->sse.isValid()) {
#ifdef Q_ENABLE_PERFORMANCE_LOG
        QFxPerfTimer<QFxPerf::BindValueSSE> perfsse;
#endif

        context()->d_func()->defaultObjects.insert(context()->d_func()->highPriorityCount, d->me);

        if(!d->sseData)
            d->sseData = d->sse.newScriptState();
        rv = d->sse.run(context(), d->sseData, &cacheState);

        context()->d_func()->defaultObjects.removeAt(context()->d_func()->highPriorityCount);
    } else {
#ifdef Q_ENABLE_PERFORMANCE_LOG
        QFxPerfTimer<QFxPerf::BindValueQt> perfqt;
#endif
        context()->d_func()->defaultObjects.insert(context()->d_func()->highPriorityCount, d->me);

        QScriptEngine *scriptEngine = engine()->scriptEngine();
        QScriptValueList oldScopeChain = scriptEngine->currentContext()->scopeChain();
        for (int i = 0; i < oldScopeChain.size(); ++i) {
            scriptEngine->currentContext()->popScope();
        }
        for (int i = context()->d_func()->scopeChain.size() - 1; i > -1; --i) {
            scriptEngine->currentContext()->pushScope(context()->d_func()->scopeChain.at(i));
        }
        QScriptValue svalue = scriptEngine->evaluate(expression());
        if (scriptEngine->hasUncaughtException()) {
            if (scriptEngine->uncaughtException().isError()){
                QScriptValue exception = scriptEngine->uncaughtException();
                if(!exception.property(QLatin1String("fileName")).toString().isEmpty()){
                    qWarning() << exception.property(QLatin1String("fileName")).toString()
                            << scriptEngine->uncaughtExceptionLineNumber()
                            << exception.toString();

                } else {
                    qWarning() << exception.toString();
                }
            }
        }

        context()->d_func()->defaultObjects.removeAt(context()->d_func()->highPriorityCount);
        if(svalue.isArray()) {
            int length = svalue.property(QLatin1String("length")).toInt32();
            if(length && svalue.property(0).isObject()) {
                QList<QObject *> list;
                for(int ii = 0; ii < length; ++ii) {
                    QScriptValue arrayItem = svalue.property(ii);
                    QObject *d = qvariant_cast<QObject *>(arrayItem.data().toVariant());
                    if(d) {
                        list << d;
                    } else {
                        list << 0;
                    }
                }
                rv = QVariant::fromValue(list);
            }
        } /*else if (svalue.isVariant()) {
            rv = svalue.toVariant();
        }*/ else if (svalue.isObject()) {
            QScriptValue objValue = svalue.data();
            if (objValue.isValid())
                rv = objValue.toVariant();
        }
        if(rv.isNull()) {
            rv = svalue.toVariant();
        }

        for (int i = 0; i < context()->d_func()->scopeChain.size(); ++i) {
            scriptEngine->currentContext()->popScope();
        }
        for (int i = oldScopeChain.size() - 1; i > -1; --i) {
            scriptEngine->currentContext()->pushScope(oldScopeChain.at(i));
        }
    }
    ep->currentExpression = lastCurrentExpression;

    if(cacheState != QmlBasicScript::NoChange) {
        if(cacheState != QmlBasicScript::Incremental && d->proxy) {
            delete d->proxy;
            d->proxy = 0;
        }

        if(trackChange() && ep->capturedProperties.count()) {
            if(!d->proxy)
                d->proxy = new BindExpressionProxy(this);

            static int changedIndex = -1;
            if(changedIndex == -1)
                changedIndex = BindExpressionProxy::staticMetaObject.indexOfSlot("changed()");

            if(bindValueDebug())
                qWarning() << "    Depends on:";

            for(int ii = 0; ii < ep->capturedProperties.count(); ++ii) {
                const QmlMetaProperty &prop = 
                    ep->capturedProperties.at(ii);

                if(prop.hasChangedNotifier()) {
                    prop.connectNotifier(d->proxy, changedIndex);
                    if(bindValueDebug())
                        qWarning() << "        property" 
                                   << prop.name()
                                   << prop.object()
                                   << prop.object()->metaObject()->superClass()->className();
                } else if(bindValueDebug()) {
                    qWarning() << "        non-subscribable property" 
                               << prop.name()
                               << prop.object()
                               << prop.object()->metaObject()->superClass()->className();
                }
            }
        }
    }
    ep->capturedProperties.clear();

    if(bindValueDebug())
        qWarning() << "    Result:" << rv 
                   << "(SSE: " << d->sse.isValid() << ")";
    return rv;
}

/*!
    Returns true if the expression results in a constant value.
    QmlExpression::value() must have been invoked at least once before the
    return from this method is valid.
 */
bool QmlExpression::isConstant() const
{
    return d->proxy == 0;
}

/*!
    Returns true if the changes are tracked in the expression's value.
*/
bool QmlExpression::trackChange() const
{
    return d->trackChange;
}

/*!
    Set whether changes are tracked in the expression's value to \a trackChange.

    If true, the QmlExpression will monitor properties involved in the 
    expression's evaluation, and call QmlExpression::valueChanged() if they have
    changed.  This allows an application to ensure that any value associated
    with the result of the expression remains up to date.

    If false, the QmlExpression will not montitor properties involved in the
    expression's evaluation, and QmlExpression::valueChanged() will never be
    called.  This is more efficient if an application wants a "one off" 
    evaluation of the expression.

    By default, trackChange is true.
*/
void QmlExpression::setTrackChange(bool trackChange)
{
    d->trackChange = trackChange;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QmlContext, the scope 
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QmlExpression::scopeObject() const
{
    return d->me;
}

/*!
    \class QmlExpression
    \brief The QmlExpression class evaluates ECMAScript in a QML context.
*/

/*!
    \class QmlExpressionObject
    \brief The QmlExpressionObject class extends QmlExpression with signals and slots.

    To remain as lightweight as possible, QmlExpression does not inherit QObject
    and consequently cannot use signals or slots.  For the cases where this is
    more convenient in an application, QmlExpressionObject can be used instead.

    QmlExpressionObject behaves identically to QmlExpression, except that the
    QmlExpressionObject::value() method is a slot, and the 
    QmlExpressionObject::valueChanged() callback is a signal.
*/
/*!
    Create a QmlExpression with the specified \a parent.

    As the expression will not have an associated QmlContext, this will be a 
    null expression object and its value will always be an invalid QVariant.
*/
QmlExpressionObject::QmlExpressionObject(QObject *parent)
: QObject(parent)
{
}

/*!
    Create a QmlExpressionObject with the specified \a parent.

    The \a expression ECMAScript will be executed in the \a ctxt QmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QmlExpressionObject::QmlExpressionObject(QmlContext *ctxt, const QString &expression, QObject *scope, QObject *parent)
: QObject(parent), QmlExpression(ctxt, expression, scope, true)
{
}

/*!  \internal */
QmlExpressionObject::QmlExpressionObject(QmlContext *ctxt, const QString &expr, QObject *scope, bool sse)
: QmlExpression(ctxt, expr, scope, sse)
{
}

/*!  \internal */
QmlExpressionObject::QmlExpressionObject(QmlContext *ctxt, void *d, QmlRefCount *rc, QObject *me)
: QmlExpression(ctxt, d, rc, me)
{
}

/*!
    Returns the value of the expression, or an invalid QVariant if the 
    expression is invalid or has an error.
*/
QVariant QmlExpressionObject::value()
{
    return QmlExpression::value();
}

/*!
    \fn void QmlExpressionObject::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by 
    calling QmlExpressionObject::value()) before this signal will be emitted.  
*/

QmlScriptClass::QmlScriptClass(QmlEngine *bindengine)
: QScriptClass(bindengine->scriptEngine()), engine(bindengine)
{
}

/////////////////////////////////////////////////////////////
/*
    The QmlContextScriptClass handles property access for a QmlContext
    via QtScript.
 */
QmlContextScriptClass::QmlContextScriptClass(QmlEngine *bindEngine)
    : QmlScriptClass(bindEngine)
{
}

QmlContextScriptClass::~QmlContextScriptClass()
{
}

QScriptClass::QueryFlags 
QmlContextScriptClass::queryProperty(const QScriptValue &object,
                                         const QScriptString &name,
                                         QueryFlags flags, uint *id)
{
    Q_UNUSED(flags);
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ContextQuery> perf;
#endif
    QmlContext *bindContext = 
        static_cast<QmlContext*>(object.data().toQObject());
    QueryFlags rv = 0;

    QString propName = name.toString();

#ifdef PROPERTY_DEBUG
    qWarning() << "Query Context:" << propName << bindContext;
#endif

    *id = InvalidId;
    if (bindContext->d_func()->variantProperties.contains(propName)) {
        rv |= HandlesReadAccess;
        *id = VariantPropertyId;
    } else if (bindContext->d_func()->properties.contains(propName)) {
        rv |= HandlesReadAccess;
        *id = ObjectListPropertyId;
    }

    for(int ii = 0; !rv && ii < bindContext->d_func()->defaultObjects.count(); ++ii) {
        rv = engine->d_func()->queryObject(propName, id,
                                    bindContext->d_func()->defaultObjects.at(ii));
        if(rv) 
            *id |= (ii << 24);
    }

    return rv;
}

QScriptValue QmlContextScriptClass::property(const QScriptValue &object,
                                                 const QScriptString &name, 
                                                 uint id)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ContextProperty> perf;
#endif
    QmlContext *bindContext = 
        static_cast<QmlContext*>(object.data().toQObject());

#ifdef PROPERTY_DEBUG
    QString propName = name.toString();
    qWarning() << "Context Property:" << propName << bindContext;
#endif

    uint basicId = id & QmlScriptClass::ClassIdMask;

    QScriptEngine *scriptEngine = engine->scriptEngine();

    switch (basicId) {
    case VariantPropertyId:
    {
        QString propName = name.toString();
        QScriptValue rv = scriptEngine->newVariant(bindContext->d_func()->variantProperties[propName]);
#ifdef PROPERTY_DEBUG
        qWarning() << "Context Property: Resolved property" << propName
                   << "to context variant property list" << bindContext <<".  Value:" << rv.toVariant();
#endif
        return rv;
    }
    case ObjectListPropertyId:
    {
        QString propName = name.toString();
        QObject *o = bindContext->d_func()->properties[propName];
        QScriptValue rv = scriptEngine->newObject(engine->d_func()->objectClass, scriptEngine->newVariant(QVariant::fromValue(o)));
#ifdef PROPERTY_DEBUG
        qWarning() << "Context Property: Resolved property" << propName
                   << "to context object property list" << bindContext <<".  Value:" << rv.toVariant();
#endif
        return rv;
    }
    default:
    {
        int objId = (id & ClassIdSelectorMask) >> 24;
        QObject *obj = bindContext->d_func()->defaultObjects.at(objId);
        QScriptValue rv = engine->d_func()->propertyObject(name, obj,
                id & ~QmlScriptClass::ClassIdSelectorMask);
        if(rv.isValid()) {
#ifdef PROPERTY_DEBUG
            qWarning() << "~Property: Resolved property" << propName
                       << "to context default object" << bindContext << obj <<".  Value:" << rv.toVariant();
#endif
            return rv;
        }
        break;
    }
    }

    return QScriptValue();
}

void QmlContextScriptClass::setProperty(QScriptValue &object,
                                            const QScriptString &name,
                                            uint id,
                                            const QScriptValue &value)
{
    Q_UNUSED(name);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ObjectSetProperty> perf;
#endif
    QmlContext *bindContext = 
        static_cast<QmlContext*>(object.data().toQObject());

#ifdef PROPERTY_DEBUG
    QString propName = name.toString();
    qWarning() << "Set QmlObject Property" << name.toString() << value.toVariant();
#endif

    int objIdx = (id & QmlScriptClass::ClassIdSelectorMask) >> 24;
    QObject *obj = bindContext->d_func()->defaultObjects.at(objIdx);

    QScriptEngine *scriptEngine = engine->scriptEngine();
    QScriptValue oldact = scriptEngine->currentContext()->activationObject();
    scriptEngine->currentContext()->setActivationObject(scriptEngine->globalObject());

    QmlMetaProperty prop;
    prop.restore(id, obj);

    QVariant v;
    QObject *data = value.data().toQObject();
    if (data) {
        v = QVariant::fromValue(data);
    } else {
        v = value.toVariant();
    }
    prop.write(v);

    scriptEngine->currentContext()->setActivationObject(oldact);
}

/////////////////////////////////////////////////////////////
/*
    The QmlObjectScriptClass handles property access for QObjects
    via QtScript.
 */
QmlObjectScriptClass::QmlObjectScriptClass(QmlEngine *bindEngine)
    : QmlScriptClass(bindEngine)
{
    engine = bindEngine;
}

QmlObjectScriptClass::~QmlObjectScriptClass()
{
}

QScriptClass::QueryFlags QmlObjectScriptClass::queryProperty(const QScriptValue &object,
                                    const QScriptString &name,
                                    QueryFlags flags, uint *id)
{
    Q_UNUSED(flags);
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ObjectQuery> perf;
#endif
    QObject *obj = object.data().toQObject();
    QueryFlags rv = 0;
    QString propName = name.toString();

#ifdef PROPERTY_DEBUG
    qWarning() << "Query QmlObject:" << propName << obj;
#endif

    if (obj)
        rv = engine->d_func()->queryObject(propName, id, obj);

    return rv;
}

QScriptValue QmlObjectScriptClass::property(const QScriptValue &object,
                                const QScriptString &name, 
                                uint id)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ObjectProperty> perf;
#endif
    QObject *obj = object.data().toQObject();

#ifdef PROPERTY_DEBUG
    QString propName = name.toString();
    qWarning() << "QmlObject Property:" << propName << obj;
#endif

    QScriptValue rv = engine->d_func()->propertyObject(name, obj, id);
    if(rv.isValid()) {
#ifdef PROPERTY_DEBUG
        qWarning() << "~Property: Resolved property" << propName
                   << "to object" << obj <<".  Value:" << rv.toVariant();
#endif
        return rv;
    }

    return QScriptValue();
}

void QmlObjectScriptClass::setProperty(QScriptValue &object,
                                       const QScriptString &name,
                                       uint id,
                                       const QScriptValue &value)
{
    Q_UNUSED(name);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ObjectSetProperty> perf;
#endif
    QObject *obj = object.data().toQObject();

#ifdef PROPERTY_DEBUG
    QString propName = name.toString();
    qWarning() << "Set QmlObject Property" << name.toString() << value.toVariant();
#endif

    QScriptEngine *scriptEngine = engine->scriptEngine();
    QScriptValue oldact = scriptEngine->currentContext()->activationObject();
    scriptEngine->currentContext()->setActivationObject(scriptEngine->globalObject());

    QmlMetaProperty prop;
    prop.restore(id, obj);

    QVariant v;
    QObject *data = value.data().toQObject();
    if (data) {
        v = QVariant::fromValue(data);
    } else {
        v = value.toVariant();
    }
    prop.write(v);

    scriptEngine->currentContext()->setActivationObject(oldact);
}

QT_END_NAMESPACE
