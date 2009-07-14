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
#include <private/qobject_p.h>

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
#include <private/qfxperf_p.h>
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
#include <private/qmlbindablevalue_p.h>
#include <private/qmlvme_p.h>

Q_DECLARE_METATYPE(QmlMetaProperty)
Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlDebugger, QML_DEBUGGER)
DEFINE_BOOL_CONFIG_OPTION(qmlImportTrace, QML_IMPORT_TRACE)

QML_DEFINE_TYPE(QObject,Object)

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
    if (mainThread== 0) {
        if (!QCoreApplication::instance())
            return 0;
        mainThread = QCoreApplication::instance()->thread();
    }

    // Note: This is very slightly faster than just using the thread storage
    // for everything.
    QStack<QmlEngine *> *engines = 0;
    if (QThread::currentThread() == mainThread) {
        engines = &mainThreadEngines;
    } else {
        engines = storage.localData();
        if (!engines) {
            engines = new QStack<QmlEngine *>;
            storage.setLocalData(engines);
        }
    }
    return engines;
}


QmlEnginePrivate::QmlEnginePrivate(QmlEngine *e)
: rootContext(0), currentBindContext(0), currentExpression(0), q(e),
  rootComponent(0), networkAccessManager(0), typeManager(e), uniqueId(1)
{
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

    for(int ii = 0; ii < bindValues.count(); ++ii) 
        clear(bindValues[ii]);
    for(int ii = 0; ii < parserStatus.count(); ++ii)
        clear(parserStatus[ii]);
}

void QmlEnginePrivate::clear(SimpleList<QmlBindableValue> &bvs)
{
    for (int ii = 0; ii < bvs.count; ++ii) {
        QmlBindableValue *bv = bvs.at(ii);
        if(bv) {
            QmlBindableValuePrivate *p = 
                static_cast<QmlBindableValuePrivate *>(QObjectPrivate::get(bv));
            p->mePtr = 0;
        }
    }
    bvs.clear();
}

void QmlEnginePrivate::clear(SimpleList<QmlParserStatus> &pss)
{
    for (int ii = 0; ii < pss.count; ++ii) {
        QmlParserStatus *ps = pss.at(ii);
        if(ps) 
            ps->d = 0;
    }
    pss.clear();
}

void QmlEnginePrivate::init()
{
    scriptEngine.installTranslatorFunctions();
    contextClass = new QmlContextScriptClass(q);
    objectClass = new QmlObjectScriptClass(q);
    rootContext = new QmlContext(q);
#ifdef QT_SCRIPTTOOLS_LIB
    if (qmlDebugger()){
        debugger = new QScriptEngineDebugger(q);
        debugger->attachTo(&scriptEngine);
    }
#endif
    //###needed for the other funcs, but should it be exposed?
    scriptEngine.globalObject().setProperty(QLatin1String("qmlEngine"),
            scriptEngine.newQObject(q));
    scriptEngine.globalObject().setProperty(QLatin1String("createQmlObject"),
            scriptEngine.newFunction(QmlEngine::createQmlObject, 1));
    scriptEngine.globalObject().setProperty(QLatin1String("createComponent"),
            scriptEngine.newFunction(QmlEngine::createComponent, 1));
}

QmlContext *QmlEnginePrivate::setCurrentBindContext(QmlContext *c)
{
    QmlContext *old = currentBindContext;
    currentBindContext = c;
    return old;
}

QmlEnginePrivate::CapturedProperty::CapturedProperty(const QmlMetaProperty &p)
: object(p.object()), coreIndex(p.coreIndex()), notifyIndex(p.property().notifySignalIndex())
{
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
    if (prop.type() == QmlMetaProperty::Invalid) {
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

        if (isFunction) {
            *id = QmlScriptClass::FunctionId;
            rv |= QScriptClass::HandlesReadAccess;
        }
    } else {
        *id = QmlScriptClass::PropertyId;
        *id |= prop.save();

        rv |= QScriptClass::HandlesReadAccess;
        if (prop.isWritable())
            rv |= QScriptClass::HandlesWriteAccess;
    }

    return rv;
}

QScriptValue QmlEnginePrivate::propertyObject(const QScriptString &propName,
                                                  QObject *obj, uint id)
{
    if (id == QmlScriptClass::FunctionId) {
        QScriptValue sobj = scriptEngine.newQObject(obj);
        QScriptValue func = sobj.property(propName);
        return func;
    } else {
        QmlMetaProperty prop;
        prop.restore(id, obj);
        if (!prop.isValid())
            return QScriptValue();

        QVariant var = prop.read();
        if (prop.needsChangedNotifier())
            capturedProperties << CapturedProperty(prop);
        QObject *varobj = QmlMetaType::toQObject(var);
        if (!varobj)
            varobj = qvariant_cast<QObject *>(var);
        if (varobj) {
            return scriptEngine.newObject(objectClass, scriptEngine.newVariant(QVariant::fromValue(varobj)));
        } else {
            if (var.type() == QVariant::Bool)
                return QScriptValue(&scriptEngine, var.toBool());
            return scriptEngine.newVariant(var);
        }
    }

    return QScriptValue();
}

void QmlEnginePrivate::contextActivated(QmlContext *)
{
    Q_Q(QmlEngine);
    QmlEngineStack *stack = engineStack();
    if (!stack)
        return;
    QStack<QmlEngine *> *engines = stack->engines();
    if (engines)
        engines->push(q);
}

void QmlEnginePrivate::contextDeactivated(QmlContext *)
{
    QmlEngineStack *stack = engineStack();
    if (!stack)
        return;
    QStack<QmlEngine *> *engines = stack->engines();
    if (engines) {
        Q_ASSERT(engines->top() == q_func());
        engines->pop();
    }
}


////////////////////////////////////////////////////////////////////

bool QmlEnginePrivate::fetchCache(QmlBasicScriptNodeCache &cache, const QString &propName, QObject *obj)
{
    QmlMetaProperty prop(obj, propName);

    if (!prop.isValid())
        return false;

    if (prop.needsChangedNotifier())
        capturedProperties << CapturedProperty(prop);

    if (prop.type() & QmlMetaProperty::Attached) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::Attached;
        cache.attached = prop.d->attachedObject();
        return true;

    } else if (prop.type() & QmlMetaProperty::Property) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::Core;
        cache.core = prop.property().propertyIndex();
        cache.coreType = prop.propertyType();
        return true;

    } else if (prop.type() & QmlMetaProperty::SignalProperty) {

        cache.object = obj;
        cache.type = QmlBasicScriptNodeCache::SignalProperty;
        cache.core = prop.coreIndex();
        return true;

    }

    return false;
}

bool QmlEnginePrivate::loadCache(QmlBasicScriptNodeCache &cache, const QString &propName, QmlContextPrivate *context)
{
    while(context) {

        QHash<QString, int>::ConstIterator iter = 
            context->propertyNames.find(propName);
        if (iter != context->propertyNames.end()) {
            cache.object = 0;
            cache.type = QmlBasicScriptNodeCache::Variant;
            cache.context = context;
            cache.contextIndex = *iter;
            capturedProperties << CapturedProperty(context->q_ptr, -1, *iter + context->notifyIndex);
            return true;
        }

        foreach(QObject *obj, context->defaultObjects) {
            if (fetchCache(cache, propName, obj))
                return true;
        }

        if (context->parent)
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
    QmlComponent component(&engine, "Text { text: \"Hello world!\" }");
    QFxItem *item = qobject_cast<QFxItem *>(component.create());

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

/*!
  Clears the engine's internal component cache.

  Normally the QmlEngine caches components loaded from qml files.  This method
  clears this cache and forces the component to be reloaded.
 */
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
    if (d->currentBindContext)
        return d->currentBindContext;
    else
        return 0;
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
    Returns the common QNetworkAccessManager used by all QML elements
    instantiated by this engine.

    The default implements no caching, cookiejar, etc., just a default 
    QNetworkAccessManager.
*/
QNetworkAccessManager *QmlEngine::networkAccessManager() const
{
    Q_D(const QmlEngine);
    if (!d->networkAccessManager) 
        d->networkAccessManager = new QNetworkAccessManager;
    return d->networkAccessManager;
}

/*!
    Return the base URL for this engine.  The base URL is only used to resolve
    components when a relative URL is passed to the QmlComponent constructor.

    If a base URL has not been explicitly set, this method returns the 
    application's current working directory.

    \sa setBaseUrl()
*/
QUrl QmlEngine::baseUrl() const
{
    Q_D(const QmlEngine);
    if (d->baseUrl.isEmpty()) {
        return QUrl::fromLocalFile(QDir::currentPath() + QDir::separator());
    } else {
        return d->baseUrl;
    }
}

/*!
    Set the  base URL for this engine to \a url.  

    \sa baseUrl()
*/
void QmlEngine::setBaseUrl(const QUrl &url)
{
    Q_D(QmlEngine);
    d->baseUrl = url;
}

/*!
  Returns the QmlContext for the \a object, or 0 if no context has been set.

  When the QmlEngine instantiates a QObject, the context is set automatically.
  */
QmlContext *QmlEngine::contextForObject(const QObject *object)
{
    if(!object)
        return 0;

    QObjectPrivate *priv = QObjectPrivate::get(const_cast<QObject *>(object));

    QmlSimpleDeclarativeData *data = 
        static_cast<QmlSimpleDeclarativeData *>(priv->declarativeData);

    return data?data->context:0;
}

/*!
  Sets the QmlContext for the \a object to \a context.
  If the \a object already has a context, a warning is
  output, but the context is not changed.

  When the QmlEngine instantiates a QObject, the context is set automatically.
 */
void QmlEngine::setContextForObject(QObject *object, QmlContext *context)
{
    QObjectPrivate *priv = QObjectPrivate::get(object);

    QmlSimpleDeclarativeData *data = 
        static_cast<QmlSimpleDeclarativeData *>(priv->declarativeData);

    if (data && data->context) {
        qWarning("QmlEngine::setContextForObject(): Object already has a QmlContext");
        return;
    }

    if (!data) {
        priv->declarativeData = &context->d_func()->contextData;
    } else {
        data->context = context;
    }

    context->d_func()->contextObjects.append(object);
}

void qmlExecuteDeferred(QObject *object)
{
    QmlInstanceDeclarativeData *data = QmlInstanceDeclarativeData::get(object);

    if (data && data->deferredComponent) {
        QmlVME vme;
        vme.runDeferred(object);

        data->deferredComponent->release();
        data->deferredComponent = 0;
    }
}

QmlContext *qmlContext(const QObject *obj)
{
    return QmlEngine::contextForObject(obj);
}

QmlEngine *qmlEngine(const QObject *obj)
{
    QmlContext *context = QmlEngine::contextForObject(obj);
    return context?context->engine():0;
}

QObject *qmlAttachedPropertiesObjectById(int id, const QObject *object, bool create)
{
    QmlExtendedDeclarativeData *edata = 
        QmlExtendedDeclarativeData::get(const_cast<QObject *>(object), true);

    QObject *rv = edata->attachedProperties.value(id);
    if (rv || !create)
        return rv;

    QmlAttachedPropertiesFunc pf = QmlMetaType::attachedPropertiesFuncById(id);
    if (!pf)
        return 0;

    rv = pf(const_cast<QObject *>(object));

    if (rv) 
        edata->attachedProperties.insert(id, rv);

    return rv;
}

void QmlSimpleDeclarativeData::destroyed(QObject *object)
{
    if (context) 
        context->d_func()->contextObjects.removeAll(object);
}

void QmlInstanceDeclarativeData::destroyed(QObject *object)
{
    QmlSimpleDeclarativeData::destroyed(object);
    if (deferredComponent)
        deferredComponent->release();
    delete this;
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
    if (!stack) return 0;

    QStack<QmlEngine *> *engines = stack->engines();
    if (!engines) {
        qWarning("QmlEngine::activeEngine() cannot be called before the construction of QCoreApplication");
        return 0;
    }

    if (engines->isEmpty())
        return 0;
    else
        return engines->top();
}

/*!
    Creates a QScriptValue allowing you to use \a object in QML script.
    \a engine is the QmlEngine it is to be created in.

    The QScriptValue returned is a QtScript Object, not a QtScript QObject, due
    to the special needs of QML requiring more functionality than a standard
    QtScript QObject.

    You'll want to use this function if you are writing C++ code which
    dynamically creates and returns objects when called from QtScript,
    and these objects are visual items in the QML tree.

    \sa QScriptEngine::newQObject()
*/
QScriptValue QmlEngine::qmlScriptObject(QObject* object, QmlEngine* engine)
{
    return engine->scriptEngine()->newObject(new QmlObjectScriptClass(engine),
            engine->scriptEngine()->newQObject(object));
}

/*!
    This function is intended for use inside QML only. In C++ just create a
    component object as usual.

    This function takes the URL of a QML file as its only argument. It returns
    a component object which can be used to create and load that QML file.

    Example QmlJS is below, remember that QML files that might be loaded
    over the network cannot be expected to be ready immediately.
    \code
        var component;
        var sprite;
        function finishCreation(){
            if(component.isReady()){
                sprite = component.createObject();
                if(sprite == 0){
                    // Error Handling
                }else{
                    sprite.parent = page;
                    sprite.x = 200;
                    //...
                }
            }else if(component.isError()){
                // Error Handling
            }
        }

        component = createComponent("Sprite.qml");
        if(component.isReady()){
            finishCreation();
        }else{
            component.statusChanged.connect(finishCreation);
        }
    \endcode

    If you are certain the files will be local, you could simplify to

    \code
        component = createComponent("Sprite.qml");
        sprite = component.createObject();
        if(sprite == 0){
            // Error Handling
            print(component.errorsString());
        }else{
            sprite.parent = page;
            sprite.x = 200;
            //...
        }
    \endcode

    If you want to just create an arbitrary string of QML, instead of
    loading a qml file, consider the createQmlObject() function.
    \sa QmlComponent::createObject(), QmlEngine::createQmlObject()
*/
QScriptValue QmlEngine::createComponent(QScriptContext *ctxt, QScriptEngine *engine)
{
    QmlComponent* c;
    QmlEngine* activeEngine = qobject_cast<QmlEngine*>(
            engine->globalObject().property(QLatin1String("qmlEngine")).toQObject());
    if(ctxt->argumentCount() != 1 || !activeEngine){
        c = new QmlComponent(activeEngine);
    }else{
        QUrl url = QUrl(activeEngine->d_func()->currentExpression->context()
                ->resolvedUrl(ctxt->argument(0).toString()));
        if(!url.isValid()){
            qDebug() << "Error A:" << url << activeEngine->activeContext() << QmlEngine::activeEngine() << activeEngine;
            url = QUrl(ctxt->argument(0).toString());
        }
        c = new QmlComponent(activeEngine, url, activeEngine);
    }
    return engine->newQObject(c);
}

/*!
    Creates a new object from the specified string of QML. It requires a
    second argument, which is the id of an existing QML object to use as
    the new object's parent. If a third argument is provided, this is used
    as the filepath that the qml came from.

    Example (where targetItem is the id of an existing QML item):
    \code
    newObject = createQmlObject('Rect {color: "red"; width: 20; height: 20}',
        targetItem, "dynamicSnippet1");
    \endcode

    This function is intended for use inside QML only. It is intended to behave
    similarly to eval, but for creating QML elements.

    Returns the created object, or null if there is an error. In the case of an
    error, details of the error are output using qWarning().

    Note that this function returns immediately, and therefore may not work if
    the QML loads new components. If you are trying to load a new component,
    for example from a QML file, consider the createComponent() function
    instead. 'New components' refers to external QML files that have not yet
    been loaded, and so it is safe to use createQmlObject to load built-in
    components.

    \sa QmlEngine::createComponent()
*/
QScriptValue QmlEngine::createQmlObject(QScriptContext *ctxt, QScriptEngine *engine)
{
    QmlEngine* activeEngine = qobject_cast<QmlEngine*>(
            engine->globalObject().property(QLatin1String("qmlEngine")).toQObject());
    if(ctxt->argumentCount() < 2 || !activeEngine){
        if(ctxt->argumentCount() < 2){
            qWarning() << "createQmlObject requires two arguments, A QML string followed by an existing QML item id.";
        }else{
            qWarning() << "createQmlObject cannot find engine.";
        }
        return engine->nullValue();
    }

    QString qml = ctxt->argument(0).toString();
    QUrl url;
    if(ctxt->argumentCount() > 2)
        url = QUrl(ctxt->argument(2).toString());
    QObject *parentArg = ctxt->argument(1).data().toQObject();
    QmlContext *qmlCtxt = qmlContext(parentArg);
    url = qmlCtxt->resolvedUrl(url);
    QmlComponent component(activeEngine, qml.toUtf8(), url);
    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        foreach (const QmlError &error, errors) {
            qWarning() <<"Error in createQmlObject(): "<< error;
        }

        return engine->nullValue();
    }

    QObject *obj = component.create(qmlCtxt);
    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        foreach (const QmlError &error, errors) {
            qWarning() <<"Error in createQmlObject(): "<< error;
        }

        return engine->nullValue();
    }

    if(obj){
        obj->setParent(parentArg);
        obj->setProperty("parent", QVariant::fromValue<QObject*>(parentArg));
        return qmlScriptObject(obj, activeEngine);
    }
    return engine->nullValue();
}

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
    QmlContext *bindContext = 
        static_cast<QmlContext*>(object.data().toQObject());
    QueryFlags rv = 0;

    QString propName = name.toString();

#ifdef PROPERTY_DEBUG
    qWarning() << "Query Context:" << propName << bindContext;
#endif

    *id = InvalidId;
    if (bindContext->d_func()->propertyNames.contains(propName)) {
        rv |= HandlesReadAccess;
        *id = VariantPropertyId;
    } 

    for (int ii = 0; !rv && ii < bindContext->d_func()->defaultObjects.count(); ++ii) {
        rv = engine->d_func()->queryObject(propName, id,
                                    bindContext->d_func()->defaultObjects.at(ii));
        if (rv) 
            *id |= (ii << 24);
    }

    return rv;
}

QScriptValue QmlContextScriptClass::property(const QScriptValue &object,
                                                 const QScriptString &name, 
                                                 uint id)
{
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
        int index = bindContext->d_func()->propertyNames.value(propName);
        QVariant value = bindContext->d_func()->propertyValues.at(index);
#ifdef PROPERTY_DEBUG
        qWarning() << "Context Property: Resolved property" << propName
                   << "to context variant property list" << bindContext <<".  Value:" << rv.toVariant();
#endif
        QScriptValue rv;
        if (QmlMetaType::isObject(value.userType())) {
            rv = scriptEngine->newObject(engine->d_func()->objectClass, scriptEngine->newVariant(value));
        } else {
            rv = scriptEngine->newVariant(value);
        }
        engine->d_func()->capturedProperties << QmlEnginePrivate::CapturedProperty(bindContext, -1, index + bindContext->d_func()->notifyIndex);
        return rv;
    }
    default:
    {
        int objId = (id & ClassIdSelectorMask) >> 24;
        QObject *obj = bindContext->d_func()->defaultObjects.at(objId);
        QScriptValue rv = engine->d_func()->propertyObject(name, obj,
                id & ~QmlScriptClass::ClassIdSelectorMask);
        if (rv.isValid()) {
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
    via QtScript. It is also used to provide a more useful API in
    QtScript for QML.
 */

QScriptValue QmlObjectDestroy(QScriptContext *context, QScriptEngine *engine)
{
    QObject* obj = context->thisObject().data().toQObject();
    if(obj)
        delete obj;
    context->thisObject().setData(QScriptValue(engine, 0));
    return engine->nullValue();
}

QmlObjectScriptClass::QmlObjectScriptClass(QmlEngine *bindEngine)
    : QmlScriptClass(bindEngine)
{
    engine = bindEngine;
    prototypeObject = engine->scriptEngine()->newObject();
    prototypeObject.setProperty(QLatin1String("destroy"),
            engine->scriptEngine()->newFunction(QmlObjectDestroy));
}

QmlObjectScriptClass::~QmlObjectScriptClass()
{
}

QScriptValue QmlObjectScriptClass::prototype() const
{
    return prototypeObject;
}

QScriptClass::QueryFlags QmlObjectScriptClass::queryProperty(const QScriptValue &object,
                                    const QScriptString &name,
                                    QueryFlags flags, uint *id)
{
    Q_UNUSED(flags);
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
    QObject *obj = object.data().toQObject();

#ifdef PROPERTY_DEBUG
    QString propName = name.toString();
    qWarning() << "QmlObject Property:" << propName << obj;
#endif

    QScriptValue rv = engine->d_func()->propertyObject(name, obj, id);
    if (rv.isValid()) {
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


class QmlImportsPrivate {
public:
    bool add(const QString& uri, const QString& prefix, const QString& version, QmlEngine::ImportType importType, const QStringList& importPath)
    {
        TypeSet *s;
        if (prefix.isEmpty()) {
            if (importType == QmlEngine::LibraryImport && version.isEmpty()) {
                // unversioned library imports are always qualified - if only by final URI component
                int lastdot = uri.lastIndexOf(QLatin1Char('.'));
                QString defaultprefix = uri.mid(lastdot+1);
                s = set.value(defaultprefix);
                if (!s)
                    set.insert(defaultprefix,(s=new TypeSet));
            } else {
                s = &unqualifiedset;
            }
        } else {
            s = set.value(prefix);
            if (!s)
                set.insert(prefix,(s=new TypeSet));
        }
        QString url = uri;
        if (importType == QmlEngine::LibraryImport) {
            url.replace(QLatin1Char('.'),QLatin1Char('/'));
            bool found = false;
            foreach (QString p, importPath) {
                QString dir = p+QLatin1Char('/')+url;
                if (QFile::exists(dir+QLatin1String("/qmldir"))) {
                    url = dir;
                    found = true;
                    break;
                }
            }
            if (!found)
                return false;
        }
        s->urls.append(url);
        s->versions.append(version);
        s->isLibrary.append(importType == QmlEngine::LibraryImport);
        return true;
    }

    QUrl find(const QString& base, const QString& type)
    {
        TypeSet *s = 0;
        int slash = type.indexOf(QLatin1Char('/'));
        if (slash >= 0) {
            while (!s) {
                s = set.value(type.left(slash));
                int nslash = type.indexOf(QLatin1Char('/'),slash+1);
                if (nslash > 0)
                    slash = nslash;
                else
                    break;
            }
        } else {
            s = &unqualifiedset;
        }
        QString unqualifiedtype = type.mid(slash+1);
        QUrl baseUrl(base);
        if (s) {
            for (int i=0; i<s->urls.count(); ++i) {
                QUrl url = baseUrl.resolved(QUrl(s->urls.at(i) +QLatin1String("/")+ unqualifiedtype + QLatin1String(".qml")));
                QString version = s->versions.at(i);
                // XXX search non-files too! (eg. zip files, see QT-524)
                QFileInfo f(url.toLocalFile());
                if (f.exists()) {
                    bool ok=true;
                    if (!version.isEmpty()) {
                        ok=false;
                        // Check version file - XXX cache these in QmlEngine!
                        QFile qmldir(s->urls.at(i)+QLatin1String("/qmldir"));
                        if (qmldir.open(QIODevice::ReadOnly)) {
                            do {
                                QString line = QString::fromUtf8(qmldir.readLine());
                                if (line.at(0) == QLatin1Char('#'))
                                    continue;
                                int space1 = line.indexOf(QLatin1Char(' '));
                                int space2 = space1 >=0 ? line.indexOf(QLatin1Char(' '),space1+1) : -1;
                                QStringRef maptype = line.leftRef(space1);
                                QStringRef mapversion = line.midRef(space1+1,space2<0?line.length()-space1-2:space2-space1-1);
                                QStringRef mapfile = space2<0 ? QStringRef() : line.midRef(space2+1,line.length()-space2-2);
                                if (maptype==unqualifiedtype && mapversion==version) {
                                    if (mapfile.isEmpty())
                                        return url;
                                    else
                                        return url.resolved(mapfile.toString());
                                }
                            } while (!qmldir.atEnd());
                        }
                    }
                    if (ok)
                        return url;
                }
            }
        }
        return baseUrl.resolved(QUrl(type + QLatin1String(".qml")));
    }

    QmlType *findBuiltin(const QString&, const QByteArray& type)
    {
        TypeSet *s = 0;
        int slash = type.indexOf('/');
        if (slash >= 0) {
            while (!s) {
                s = set.value(QString::fromLatin1(type.left(slash)));
                int nslash = type.indexOf('/',slash+1);
                if (nslash > 0)
                    slash = nslash;
                else
                    break;
            }
        } else {
            s = &unqualifiedset;
        }
        QByteArray unqualifiedtype = slash < 0 ? type : type.mid(slash+1); // common-case opt (QString::mid works fine, but slower)
        if (s) {
            for (int i=0; i<s->urls.count(); ++i) {
                QmlType *t = QmlMetaType::qmlType(s->urls.at(i).toLatin1()+"/"+unqualifiedtype);
                if (t) return t;
            }
        }
        return QmlMetaType::qmlType(type);
    }

private:
    struct TypeSet {
        QStringList urls;
        QStringList versions;
        QList<bool> isLibrary;
    };
    TypeSet unqualifiedset;
    QHash<QString,TypeSet* > set;
};

QmlEngine::Imports::Imports() :
    d(new QmlImportsPrivate)
{
}

QmlEngine::Imports::~Imports()
{
}

void QmlEngine::Imports::setBaseUrl(const QUrl& url)
{
    base = url;
}

void QmlEngine::addImportPath(const QString& path)
{
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::addImportPath" << path;
    Q_D(QmlEngine);
    d->fileImportPath.prepend(path);
}

bool QmlEngine::addToImport(Imports* imports, const QString& uri, const QString& prefix, const QString& version, ImportType importType) const
{
    Q_D(const QmlEngine);
    bool ok = imports->d->add(uri,prefix,version,importType,d->fileImportPath);
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::addToImport(" << imports << uri << prefix << version << (importType==LibraryImport ? "Library" : "File") << ": " << ok;
    return ok;
}

bool QmlEngine::resolveType(const Imports& imports, const QByteArray& type, QmlType** type_return, QUrl* url_return) const
{
    Q_D(const QmlEngine);
    QmlType* t = imports.d->findBuiltin(imports.base,type);
    if (t) {
        if (type_return) *type_return = t;
        if (qmlImportTrace()) 
            qDebug() << "QmlEngine::resolveType" << type << "= (builtin)";
        return true;
    }
    QUrl url = imports.d->find(imports.base,type);
    if (url.isValid()) {
        if (url_return) *url_return = url;
        if (qmlImportTrace()) 
            qDebug() << "QmlEngine::resolveType" << type << "=" << url;
        return true;
    }
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::resolveType" << type << " not found";
    return false;
}


QT_END_NAMESPACE
