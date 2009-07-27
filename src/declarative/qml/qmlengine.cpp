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

#undef QT3_SUPPORT // don't want it here - it just causes bugs (which is why we removed it)

#include <QMetaProperty>
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>
#include <private/qobject_p.h>
#include <private/qmlcompiler_p.h>

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
#include "qmlengine.h"
#include "qmlcontext.h"
#include "qmlexpression.h"
#include <QtCore/qthreadstorage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <qmlcomponent.h>
#include "private/qmlcomponentjs_p.h"
#include "private/qmlmetaproperty_p.h"
#include <private/qmlbinding_p.h>
#include <private/qmlvme_p.h>
#include <private/qmlenginedebug_p.h>

Q_DECLARE_METATYPE(QmlMetaProperty)
Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlDebugger, QML_DEBUGGER)
DEFINE_BOOL_CONFIG_OPTION(qmlImportTrace, QML_IMPORT_TRACE)

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Object,QObject)

struct StaticQtMetaObject : public QObject
{
    static const QMetaObject *get()
        { return &static_cast<StaticQtMetaObject*> (0)->staticQtMetaObject; }
};

QmlEnginePrivate::QmlEnginePrivate(QmlEngine *e)
: rootContext(0), currentBindContext(0), currentExpression(0), 
  isDebugging(false), contextClass(0), objectClass(0), valueTypeClass(0),
  scriptEngine(this), rootComponent(0), networkAccessManager(0), typeManager(e),
  uniqueId(1)
{
    QScriptValue qtObject = 
        scriptEngine.newQMetaObject(StaticQtMetaObject::get());
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

void QmlEnginePrivate::clear(SimpleList<QmlBinding> &bvs)
{
    for (int ii = 0; ii < bvs.count; ++ii) {
        QmlBinding *bv = bvs.at(ii);
        if(bv) {
            QmlBindingPrivate *p = 
                static_cast<QmlBindingPrivate *>(QObjectPrivate::get(bv));
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

Q_GLOBAL_STATIC(QmlEngineDebugServer, qmlEngineDebugServer);

void QmlEnginePrivate::init()
{
    Q_Q(QmlEngine);
    scriptEngine.installTranslatorFunctions();
    contextClass = new QmlContextScriptClass(q);
    objectClass = new QmlObjectScriptClass(q);
    valueTypeClass = new QmlValueTypeScriptClass(q);
    rootContext = new QmlContext(q,true);
#ifdef QT_SCRIPTTOOLS_LIB
    if (qmlDebugger()){
        debugger = new QScriptEngineDebugger(q);
        debugger->attachTo(&scriptEngine);
    }
#endif

    scriptEngine.globalObject().setProperty(QLatin1String("createQmlObject"),
            scriptEngine.newFunction(QmlEnginePrivate::createQmlObject, 1));
    scriptEngine.globalObject().setProperty(QLatin1String("createComponent"),
            scriptEngine.newFunction(QmlEnginePrivate::createComponent, 1));

    if (QCoreApplication::instance()->thread() == q->thread() && 
        QmlEngineDebugServer::isDebuggingEnabled()) {
        qmlEngineDebugServer();
        isDebugging = true;
        QmlEngineDebugServer::addEngine(q);
    }
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

struct QmlValueTypeReference {
    QmlValueType *type;
    QGuard<QObject> object;
    int property;
};
Q_DECLARE_METATYPE(QmlValueTypeReference);

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

        if (prop.needsChangedNotifier())
            capturedProperties << CapturedProperty(prop);

        int propType = prop.propertyType();
        if (propType < QVariant::UserType && valueTypes[propType]) {
            QmlValueTypeReference ref;
            ref.type = valueTypes[propType];
            ref.object = obj;
            ref.property = prop.coreIndex();
            return scriptEngine.newObject(valueTypeClass, scriptEngine.newVariant(QVariant::fromValue(ref)));
        }

        QVariant var = prop.read();
        QObject *varobj = (propType < QVariant::UserType)?0:QmlMetaType::toQObject(var);
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
    Q_D(QmlEngine);
    if (d->isDebugging)
        QmlEngineDebugServer::remEngine(this);
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
    Sets the common QNetworkAccessManager, \a network, used by all QML elements
    instantiated by this engine.

    Any previously set manager is deleted and \a network is owned by the
    QmlEngine.  This method should only be called before any QmlComponents are
    instantiated.
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
/*
QScriptEngine *QmlEngine::scriptEngine()
{
    Q_D(QmlEngine);
    return &d->scriptEngine;
}
*/

/*!
    Creates a QScriptValue allowing you to use \a object in QML script.
    \a engine is the QmlEngine it is to be created in.

    The QScriptValue returned is a QtScript Object, not a QtScript QObject, due
    to the special needs of QML requiring more functionality than a standard
    QtScript QObject.
*/
QScriptValue QmlEnginePrivate::qmlScriptObject(QObject* object, 
                                               QmlEngine* engine)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    return scriptEngine->newObject(new QmlObjectScriptClass(engine), scriptEngine->newQObject(object));
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
*/
QScriptValue QmlEnginePrivate::createComponent(QScriptContext *ctxt, 
                                               QScriptEngine *engine)
{
    QmlComponentJS* c;

    QmlEnginePrivate *activeEnginePriv = 
        static_cast<QmlScriptEngine*>(engine)->p;
    QmlEngine* activeEngine = activeEnginePriv->q_func();

    QmlContext* context = activeEnginePriv->currentExpression->context();
    if(ctxt->argumentCount() != 1) {
        c = new QmlComponentJS(activeEngine);
    }else{
        QUrl url = QUrl(context->resolvedUrl(ctxt->argument(0).toString()));
        if(!url.isValid())
            url = QUrl(ctxt->argument(0).toString());
        c = new QmlComponentJS(activeEngine, url, activeEngine);
    }
    c->setContext(context);
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
*/
QScriptValue QmlEnginePrivate::createQmlObject(QScriptContext *ctxt, QScriptEngine *engine)
{
    QmlEnginePrivate *activeEnginePriv = 
        static_cast<QmlScriptEngine*>(engine)->p;
    QmlEngine* activeEngine = activeEnginePriv->q_func();

    if(ctxt->argumentCount() < 2) 
        return engine->nullValue();

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
        qWarning() <<"QmlEngine::createQmlObject():";
        foreach (const QmlError &error, errors) 
            qWarning() << "    " << error;

        return engine->nullValue();
    }

    QObject *obj = component.create(qmlCtxt);

    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        qWarning() <<"QmlEngine::createQmlObject():";
        foreach (const QmlError &error, errors) 
            qWarning() << "    " << error;

        return engine->nullValue();
    }

    if(obj) {
        obj->setParent(parentArg);
        obj->setProperty("parent", QVariant::fromValue<QObject*>(parentArg));
        return qmlScriptObject(obj, activeEngine);
    }
    return engine->nullValue();
}

QmlScriptClass::QmlScriptClass(QmlEngine *bindengine)
: QScriptClass(QmlEnginePrivate::getScriptEngine(bindengine)), 
  engine(bindengine)
{
}

QVariant QmlScriptClass::toVariant(QmlEngine *engine, const QScriptValue &val)
{
    QmlEnginePrivate *ep = 
        static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine));

    QScriptClass *sc = val.scriptClass();
    if (!sc) {
        return val.toVariant();
    } else if (sc == ep->contextClass) {
        return QVariant();
    } else if (sc == ep->objectClass) {
        return QVariant::fromValue(val.data().toQObject());
    } else if (sc == ep->valueTypeClass) {
        QmlValueTypeReference ref = 
            qvariant_cast<QmlValueTypeReference>(val.data().toVariant());

        if (!ref.object)
            return QVariant();
        
        QMetaProperty p = ref.object->metaObject()->property(ref.property);
        return p.read(ref.object);
    }

    return QVariant();
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
        rv = QmlEnginePrivate::get(engine)->queryObject(propName, id, bindContext->d_func()->defaultObjects.at(ii));
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

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);

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
            rv = scriptEngine->newObject(ep->objectClass, scriptEngine->newVariant(value));
        } else {
            rv = scriptEngine->newVariant(value);
        }
        ep->capturedProperties << QmlEnginePrivate::CapturedProperty(bindContext, -1, index + bindContext->d_func()->notifyIndex);
        return rv;
    }
    default:
    {
        int objId = (id & ClassIdSelectorMask) >> 24;
        QObject *obj = bindContext->d_func()->defaultObjects.at(objId);
        QScriptValue rv = ep->propertyObject(name, obj,
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

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QScriptValue oldact = scriptEngine->currentContext()->activationObject();
    scriptEngine->currentContext()->setActivationObject(scriptEngine->globalObject());

    QmlMetaProperty prop;
    prop.restore(id, obj);

    QVariant v = QmlScriptClass::toVariant(engine, value);
    prop.write(v);

    scriptEngine->currentContext()->setActivationObject(oldact);
}

/////////////////////////////////////////////////////////////
QmlValueTypeScriptClass::QmlValueTypeScriptClass(QmlEngine *bindEngine)
: QmlScriptClass(bindEngine)
{
}

QmlValueTypeScriptClass::~QmlValueTypeScriptClass()
{
}

QmlValueTypeScriptClass::QueryFlags 
QmlValueTypeScriptClass::queryProperty(const QScriptValue &object,
                                       const QScriptString &name,
                                       QueryFlags flags, uint *id)
{
    QmlValueTypeReference ref = 
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return 0;

    QByteArray propName = name.toString().toUtf8();

    int idx = ref.type->metaObject()->indexOfProperty(propName.constData());
    if (idx == -1)
        return 0;
    *id = idx;

    QMetaProperty prop = ref.object->metaObject()->property(idx);

    QmlValueTypeScriptClass::QueryFlags rv = 
        QmlValueTypeScriptClass::HandlesReadAccess;
    if (prop.isWritable())
        rv |= QmlValueTypeScriptClass::HandlesWriteAccess;

    return rv;
}

QScriptValue QmlValueTypeScriptClass::property(const QScriptValue &object,
                                               const QScriptString &name, 
                                               uint id)
{
    QmlValueTypeReference ref = 
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return QScriptValue();

    ref.type->read(ref.object, ref.property);

    QMetaProperty p = ref.type->metaObject()->property(id);
    QVariant rv = p.read(ref.type);

    return static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine))->scriptEngine.newVariant(rv);
}

void QmlValueTypeScriptClass::setProperty(QScriptValue &object,
                                          const QScriptString &name,
                                          uint id,
                                          const QScriptValue &value)
{
    QmlValueTypeReference ref = 
        qvariant_cast<QmlValueTypeReference>(object.data().toVariant());

    if (!ref.object)
        return;

    QVariant v = QmlScriptClass::toVariant(engine, value);

    ref.type->read(ref.object, ref.property);
    QMetaProperty p = ref.type->metaObject()->property(id);
    p.write(ref.type, v);
    ref.type->write(ref.object, ref.property);
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
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(bindEngine);
    prototypeObject = scriptEngine->newObject();
    prototypeObject.setProperty(QLatin1String("destroy"),
                                scriptEngine->newFunction(QmlObjectDestroy));
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
        rv = QmlEnginePrivate::get(engine)->queryObject(propName, id, obj);

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

    QScriptValue rv = 
        QmlEnginePrivate::get(engine)->propertyObject(name, obj, id);
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

    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);
    QScriptValue oldact = scriptEngine->currentContext()->activationObject();
    scriptEngine->currentContext()->setActivationObject(scriptEngine->globalObject());

    QmlMetaProperty prop;
    prop.restore(id, obj);

    QVariant v = QmlScriptClass::toVariant(engine, value);
    prop.write(v);

    scriptEngine->currentContext()->setActivationObject(oldact);
}


struct QmlEnginePrivate::ImportedNamespace {
    QStringList urls;
    QStringList versions;
    QList<bool> isLibrary;

    QUrl find(const QString& type) const
    {
        for (int i=0; i<urls.count(); ++i) {
            QUrl url = QUrl(urls.at(i) + QLatin1String("/") + type + QLatin1String(".qml"));
            QString version = versions.at(i);
            // XXX search non-files too! (eg. zip files, see QT-524)
            QFileInfo f(url.toLocalFile());
            if (f.exists()) {
                bool ok=true;
                if (!version.isEmpty()) {
                    ok=false;
                    // Check version file - XXX cache these in QmlEngine!
                    QFile qmldir(urls.at(i)+QLatin1String("/qmldir"));
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
                            if (maptype==type && mapversion==version) {
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
        return QUrl();
    }

    QmlType *findBuiltin(const QByteArray& type, QByteArray* found=0) const
    {
        for (int i=0; i<urls.count(); ++i) {
            QByteArray version = versions.at(i).toLatin1();
            QByteArray qt = urls.at(i).toLatin1();
            if (version.isEmpty())
                qt += "/";
            else
                qt += "/" + version + "/";
            qt += type;
            QmlType *t = QmlMetaType::qmlType(qt);
            if (found) *found = qt;
            if (t) return t;
        }
        return 0;
    }
};

class QmlImportsPrivate {
public:
    bool add(const QUrl& base, const QString& uri, const QString& prefix, const QString& version, QmlScriptParser::Import::Type importType, const QStringList& importPath)
    {
        QmlEnginePrivate::ImportedNamespace *s;
        if (prefix.isEmpty()) {
            if (importType == QmlScriptParser::Import::Library && version.isEmpty()) {
                // unversioned library imports are always qualified - if only by final URI component
                int lastdot = uri.lastIndexOf(QLatin1Char('.'));
                QString defaultprefix = uri.mid(lastdot+1);
                s = set.value(defaultprefix);
                if (!s)
                    set.insert(defaultprefix,(s=new QmlEnginePrivate::ImportedNamespace));
            } else {
                s = &unqualifiedset;
            }
        } else {
            s = set.value(prefix);
            if (!s)
                set.insert(prefix,(s=new QmlEnginePrivate::ImportedNamespace));
        }
        QString url = uri;
        if (importType == QmlScriptParser::Import::Library) {
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
            if (!found) {
                // XXX assume it is a built-in type qualifier
            }
        } else {
            url = base.resolved(QUrl(url)).toString();
        }
        s->urls.append(url);
        s->versions.append(version);
        s->isLibrary.append(importType == QmlScriptParser::Import::Library);
        return true;
    }

    QUrl find(const QString& type) const
    {
        const QmlEnginePrivate::ImportedNamespace *s = 0;
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
        if (s)
            return s->find(unqualifiedtype);
        else
            return QUrl();
    }


    QmlType *findBuiltin(const QByteArray& type, QByteArray* found=0)
    {
        QmlEnginePrivate::ImportedNamespace *s = 0;
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
        if (s)
            return s->findBuiltin(unqualifiedtype,found);
        else
            return 0;
    }

    QmlEnginePrivate::ImportedNamespace *findNamespace(const QString& type)
    {
        return set.value(type);
    }

private:
    QmlEnginePrivate::ImportedNamespace unqualifiedset;
    QHash<QString,QmlEnginePrivate::ImportedNamespace* > set;
};

QmlEnginePrivate::Imports::Imports() :
    d(new QmlImportsPrivate)
{
}

QmlEnginePrivate::Imports::~Imports()
{
}

/*!
  Sets the base URL to be used for all relative file imports added.
*/
void QmlEnginePrivate::Imports::setBaseUrl(const QUrl& url)
{
    base = url;
}

/*!
  Adds \a path as a directory where installed QML components are
  defined in a URL-based directory structure.

  For example, if you add \c /opt/MyApp/lib/qml and then load QML
  that imports \c com.mycompany.Feature, then QmlEngine will look
  in \c /opt/MyApp/lib/qml/com/mycompany/Feature/ for the components
  provided by that module (and in the case of versioned imports,
  for the \c qmldir file definiting the type version mapping.
*/
void QmlEngine::addImportPath(const QString& path)
{
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::addImportPath" << path;
    Q_D(QmlEngine);
    d->fileImportPath.prepend(path);
}

/*!
  \internal

  Adds information to \a imports such that subsequent calls to resolveType()
  will resolve types qualified by \a prefix by considering types found at the given \a uri.

  The uri is either a directory (if importType is FileImport), or a URI resolved using paths
  added via addImportPath() (if importType is LibraryImport).

  The \a prefix may be empty, in which case the import location is considered for
  unqualified types.

  The base URL must already have been set with Import::setBaseUrl().
*/
bool QmlEnginePrivate::addToImport(Imports* imports, const QString& uri, const QString& prefix, const QString& version, QmlScriptParser::Import::Type importType) const
{
    bool ok = imports->d->add(imports->base,uri,prefix,version,importType,fileImportPath);
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::addToImport(" << imports << uri << prefix << version << (importType==QmlScriptParser::Import::Library? "Library" : "File") << ": " << ok;
    return ok;
}

/*!
  \internal

  Using the given \a imports, the given (namespace qualified) \a type is resolved to either
  an ImportedNamespace stored at \a ns_return,
  a QmlType stored at \a type_return, or
  a component located at \a url_return.

  If any return pointer is 0, the corresponding search is not done.

  \sa addToImport()
*/
bool QmlEnginePrivate::resolveType(const Imports& imports, const QByteArray& type, QmlType** type_return, QUrl* url_return, ImportedNamespace** ns_return) const
{
    if (ns_return) {
        *ns_return = imports.d->findNamespace(QLatin1String(type));
        if (*ns_return)
            return true;
    }
    if (type_return) {
        QmlType* t = imports.d->findBuiltin(type);
        if (!t) t = QmlMetaType::qmlType(type);
        if (t) {
            if (type_return) *type_return = t;
            if (qmlImportTrace()) 
                qDebug() << "QmlEngine::resolveType" << type << "= (builtin)";
            return true;
        }
    }
    if (url_return) {
        QUrl url = imports.d->find(QLatin1String(type));
        if (!url.isValid())
            url = imports.base.resolved(QUrl(QLatin1String(type + ".qml")));

        if (url.isValid()) {
            if (url_return) *url_return = url;
            if (qmlImportTrace()) 
                qDebug() << "QmlEngine::resolveType" << type << "=" << url;
            return true;
        }
    }
    if (qmlImportTrace()) 
        qDebug() << "QmlEngine::resolveType" << type << " not found";
    return false;
}

/*!
  \internal

  Searching \e only in the namespace \a ns (previously returned in a call to
  resolveType(), \a type is found and returned to either
  a QmlType stored at \a type_return, or
  a component located at \a url_return.

  If either return pointer is 0, the corresponding search is not done.
*/
void QmlEnginePrivate::resolveTypeInNamespace(ImportedNamespace* ns, const QByteArray& type, QmlType** type_return, QUrl* url_return ) const
{
    if (type_return) {
        *type_return = ns->findBuiltin(type);
    }
    if (url_return) {
        *url_return = ns->find(QLatin1String(type));
    }
}

QT_END_NAMESPACE
