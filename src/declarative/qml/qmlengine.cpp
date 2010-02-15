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

#include "qmlengine_p.h"
#include "qmlengine.h"

#include "qmlcontext_p.h"
#include "qmlcompiler_p.h"
#include "qmlglobalscriptclass_p.h"
#include "qml.h"
#include "qmlcontext.h"
#include "qmlexpression.h"
#include "qmlcomponent.h"
#include "qmlmetaproperty_p.h"
#include "qmlmoduleplugin.h"
#include "qmlbinding_p.h"
#include "qmlvme_p.h"
#include "qmlenginedebug_p.h"
#include "qmlstringconverters_p.h"
#include "qmlxmlhttprequest_p.h"
#include "qmlsqldatabase_p.h"
#include "qmltypenamescriptclass_p.h"
#include "qmllistscriptclass_p.h"
#include "qmlscriptstring.h"
#include "qmlglobal_p.h"
#include "qmlworkerscript_p.h"
#include "qmlcomponent_p.h"
#include "qmlscriptclass_p.h"
#include "qmlnetworkaccessmanagerfactory.h"

#include <qfxperf_p_p.h>

#include <QtCore/qmetaobject.h>
#include <QScriptClass>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QDesktopServices>
#include <QTimer>
#include <QList>
#include <QPair>
#include <QDebug>
#include <QMetaObject>
#include <QStack>
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qthreadstorage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qsound.h>
#include <QGraphicsObject>
#include <QtCore/qcryptographichash.h>

#include <private/qfactoryloader_p.h>
#include <private/qobject_p.h>
#include <private/qscriptdeclarativeclass_p.h>

#include <private/qmlgraphicsitemsmodule_p.h>

#ifdef Q_OS_WIN // for %APPDATA%
#include <qt_windows.h>
#include <qlibrary.h>

#define CSIDL_APPDATA		0x001a	// <username>\Application Data
#endif

Q_DECLARE_METATYPE(QmlMetaProperty)
Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlImportTrace, QML_IMPORT_TRACE)

QML_DEFINE_TYPE(Qt,4,6,QtObject,QObject)
/*!
    \qmlclass QtObject QObject
    \brief The QtObject element is the most basic element in QML

    The QtObject element is a non-visual element which contains only
    the objectName property. It is useful for when you need an extremely
    lightweight element to place your own custom properties in.

    It can also be useful for C++ integration, as it is just a plain QObject. See
    the QObject documentation for further details.
*/
/*!
  \qmlproperty string QtObject::objectName
  This property allows you to give a name to this specific object instance.

  See \l{scripting.html#accessing-child-qobjects}{Accessing Child QObjects}
  in the scripting documentation for details how objectName can be used from
  scripts.
*/

struct StaticQtMetaObject : public QObject
{
    static const QMetaObject *get()
        { return &static_cast<StaticQtMetaObject*> (0)->staticQtMetaObject; }
};

static bool qt_QmlQtModule_registered = false;

QmlEnginePrivate::QmlEnginePrivate(QmlEngine *e)
: captureProperties(false), rootContext(0), currentExpression(0), isDebugging(false), 
  contextClass(0), sharedContext(0), sharedScope(0), objectClass(0), valueTypeClass(0), 
  globalClass(0), cleanup(0), erroredBindings(0), inProgressCreations(0), 
  scriptEngine(this), workerScriptEngine(0), componentAttacheds(0), inBeginCreate(false), 
  networkAccessManager(0), networkAccessManagerFactory(0),
  typeManager(e), uniqueId(1)
{
    if (!qt_QmlQtModule_registered) {
        qt_QmlQtModule_registered = true;
        QmlGraphicsItemModule::defineModule();
    }
    globalClass = new QmlGlobalScriptClass(&scriptEngine);
    fileImportPath.append(QLibraryInfo::location(QLibraryInfo::DataPath)+QDir::separator()+QLatin1String("qml"));
}

QUrl QmlScriptEngine::resolvedUrl(QScriptContext *context, const QUrl& url)
{
    if (p) {
        QmlContext *ctxt = QmlEnginePrivate::get(this)->getContext(context);
        Q_ASSERT(ctxt);
        return ctxt->resolvedUrl(url);
    }
    return baseUrl.resolved(url);
}

QmlScriptEngine::QmlScriptEngine(QmlEnginePrivate *priv)
: p(priv), sqlQueryClass(0), namedNodeMapClass(0), nodeListClass(0)
{
    // Note that all documentation for stuff put on the global object goes in
    // doc/src/declarative/globalobject.qdoc

    bool mainthread = priv != 0;

    QScriptValue qtObject =
        newQMetaObject(StaticQtMetaObject::get());
    globalObject().setProperty(QLatin1String("Qt"), qtObject);

    offlineStoragePath = QDesktopServices::storageLocation(QDesktopServices::DataLocation).replace(QLatin1Char('/'), QDir::separator())
        + QDir::separator() + QLatin1String("QML")
        + QDir::separator() + QLatin1String("OfflineStorage");


    qt_add_qmlxmlhttprequest(this);
    qt_add_qmlsqldatabase(this);
    // XXX A Multimedia "Qt.Sound" class also needs to be made available,
    // XXX but we don't want a dependency in that cirection.
    // XXX When the above a done some better way, that way should also be
    // XXX used to add Qt.Sound class.


    //types
    qtObject.setProperty(QLatin1String("rgba"), newFunction(QmlEnginePrivate::rgba, 4));
    qtObject.setProperty(QLatin1String("hsla"), newFunction(QmlEnginePrivate::hsla, 4));
    qtObject.setProperty(QLatin1String("rect"), newFunction(QmlEnginePrivate::rect, 4));
    qtObject.setProperty(QLatin1String("point"), newFunction(QmlEnginePrivate::point, 2));
    qtObject.setProperty(QLatin1String("size"), newFunction(QmlEnginePrivate::size, 2));
    qtObject.setProperty(QLatin1String("vector3d"), newFunction(QmlEnginePrivate::vector, 3));

    if (mainthread) {
        //color helpers
        qtObject.setProperty(QLatin1String("lighter"), newFunction(QmlEnginePrivate::lighter, 1));
        qtObject.setProperty(QLatin1String("darker"), newFunction(QmlEnginePrivate::darker, 1));
        qtObject.setProperty(QLatin1String("tint"), newFunction(QmlEnginePrivate::tint, 2));
    }

    //misc methods
    qtObject.setProperty(QLatin1String("closestAngle"), newFunction(QmlEnginePrivate::closestAngle, 2));
    qtObject.setProperty(QLatin1String("playSound"), newFunction(QmlEnginePrivate::playSound, 1));
    qtObject.setProperty(QLatin1String("openUrlExternally"),newFunction(QmlEnginePrivate::desktopOpenUrl, 1));
    qtObject.setProperty(QLatin1String("md5"),newFunction(QmlEnginePrivate::md5, 1));
    qtObject.setProperty(QLatin1String("btoa"),newFunction(QmlEnginePrivate::btoa, 1));
    qtObject.setProperty(QLatin1String("atob"),newFunction(QmlEnginePrivate::atob, 1));
    qtObject.setProperty(QLatin1String("quit"), newFunction(QmlEnginePrivate::quit, 0));
    qtObject.setProperty(QLatin1String("resolvedUrl"),newFunction(QmlScriptEngine::resolvedUrl, 1));

    //firebug/webkit compat
    QScriptValue consoleObject = newObject();
    consoleObject.setProperty(QLatin1String("log"),newFunction(QmlEnginePrivate::consoleLog, 1));
    consoleObject.setProperty(QLatin1String("debug"),newFunction(QmlEnginePrivate::consoleLog, 1));
    globalObject().setProperty(QLatin1String("console"), consoleObject);

    if (mainthread) {
        globalObject().setProperty(QLatin1String("createQmlObject"),
                newFunction(QmlEnginePrivate::createQmlObject, 1));
        globalObject().setProperty(QLatin1String("createComponent"),
                newFunction(QmlEnginePrivate::createComponent, 1));
    }

    // translation functions need to be installed
    // before the global script class is constructed (QTBUG-6437)
    installTranslatorFunctions();
}

QmlScriptEngine::~QmlScriptEngine()
{
    delete sqlQueryClass;
    delete nodeListClass;
    delete namedNodeMapClass;
}

QScriptValue QmlScriptEngine::resolvedUrl(QScriptContext *ctxt, QScriptEngine *engine)
{
    QString arg = ctxt->argument(0).toString();
    QUrl r = QmlScriptEngine::get(engine)->resolvedUrl(ctxt,QUrl(arg));
    return QScriptValue(r.toString());
}

QNetworkAccessManager *QmlScriptEngine::networkAccessManager()
{
    return p->getNetworkAccessManager();
}

QmlEnginePrivate::~QmlEnginePrivate()
{
    while (cleanup) {
        QmlCleanup *c = cleanup;
        cleanup = c->next;
        if (cleanup) cleanup->prev = &cleanup;
        c->next = 0;
        c->prev = 0;
        c->clear();
    }

    delete rootContext;
    rootContext = 0;
    delete contextClass;
    contextClass = 0;
    delete objectClass;
    objectClass = 0;
    delete valueTypeClass;
    valueTypeClass = 0;
    delete typeNameClass;
    typeNameClass = 0;
    delete listClass;
    listClass = 0;
    delete globalClass;
    globalClass = 0;

    for(int ii = 0; ii < bindValues.count(); ++ii)
        clear(bindValues[ii]);
    for(int ii = 0; ii < parserStatus.count(); ++ii)
        clear(parserStatus[ii]);
    for(QHash<int, QmlCompiledData*>::ConstIterator iter = m_compositeTypes.constBegin(); iter != m_compositeTypes.constEnd(); ++iter)
        (*iter)->release();
    for(QHash<const QMetaObject *, QmlPropertyCache *>::Iterator iter = propertyCache.begin(); iter != propertyCache.end(); ++iter)
        (*iter)->release();

}

void QmlEnginePrivate::clear(SimpleList<QmlAbstractBinding> &bvs)
{
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
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QmlScriptString>("QmlScriptString");
    qRegisterMetaType<QScriptValue>("QScriptValue");

    contextClass = new QmlContextScriptClass(q);
    objectClass = new QmlObjectScriptClass(q);
    valueTypeClass = new QmlValueTypeScriptClass(q);
    typeNameClass = new QmlTypeNameScriptClass(q);
    listClass = new QmlListScriptClass(q);
    rootContext = new QmlContext(q,true);

    if (QCoreApplication::instance()->thread() == q->thread() &&
        QmlEngineDebugServer::isDebuggingEnabled()) {
        qmlEngineDebugServer();
        isDebugging = true;
        QmlEngineDebugServer::addEngine(q);

        qmlEngineDebugServer()->waitForClients();
    }
}

QmlWorkerScriptEngine *QmlEnginePrivate::getWorkerScriptEngine()
{
    Q_Q(QmlEngine);
    if (!workerScriptEngine) 
        workerScriptEngine = new QmlWorkerScriptEngine(q);
    return workerScriptEngine;
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
    QmlComponent component(&engine);
    component.setData("import Qt 4.6\nText { text: \"Hello world!\" }", QUrl());
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(component.create());

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
    Sets the \a factory to use for creating QNetworkAccessManager(s).

    QNetworkAccessManager is used for all network access by QML.
    By implementing a factory it is possible to create custom
    QNetworkAccessManager with specialized caching, proxy and
    cookie support.

    The factory must be set before exceuting the engine.
*/
void QmlEngine::setNetworkAccessManagerFactory(QmlNetworkAccessManagerFactory *factory)
{
    Q_D(QmlEngine);
    d->networkAccessManagerFactory = factory;
}

/*!
    Returns the current QmlNetworkAccessManagerFactory.

    \sa setNetworkAccessManagerFactory()
*/
QmlNetworkAccessManagerFactory *QmlEngine::networkAccessManagerFactory() const
{
    Q_D(const QmlEngine);
    return d->networkAccessManagerFactory;
}

QNetworkAccessManager *QmlEnginePrivate::getNetworkAccessManager() const
{
    Q_Q(const QmlEngine);

    if (!networkAccessManager) {
        if (networkAccessManagerFactory) {
            networkAccessManager = networkAccessManagerFactory->create(const_cast<QmlEngine*>(q));
        } else {
            networkAccessManager = new QNetworkAccessManager(const_cast<QmlEngine*>(q));
        }
    }
    return networkAccessManager;
}

/*!
    Returns a common QNetworkAccessManager which can be used by any QML element
    instantiated by this engine.

    If a QmlNetworkAccessManagerFactory has been set and a QNetworkAccessManager
    has not yet been created, the QmlNetworkAccessManagerFactory will be used
    to create the QNetworkAccessManager; otherwise the returned QNetworkAccessManager
    will have no proxy or cache set.

    \sa setNetworkAccessManagerFactory()
*/
QNetworkAccessManager *QmlEngine::networkAccessManager() const
{
    Q_D(const QmlEngine);
    return d->getNetworkAccessManager();
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

    QmlDeclarativeData *data =
        static_cast<QmlDeclarativeData *>(priv->declarativeData);

    if (!data)
        return 0;
    else if (data->outerContext)
        return data->outerContext;
    else
        return data->context;
}

/*!
  Sets the QmlContext for the \a object to \a context.
  If the \a object already has a context, a warning is
  output, but the context is not changed.

  When the QmlEngine instantiates a QObject, the context is set automatically.
 */
void QmlEngine::setContextForObject(QObject *object, QmlContext *context)
{
    if (!object || !context)
        return;

    QmlDeclarativeData *data = QmlDeclarativeData::get(object, true);
    if (data->context) {
        qWarning("QmlEngine::setContextForObject(): Object already has a QmlContext");
        return;
    }

    data->context = context;
    data->nextContextObject = context->d_func()->contextObjects;
    if (data->nextContextObject) 
        data->nextContextObject->prevContextObject = &data->nextContextObject;
    data->prevContextObject = &context->d_func()->contextObjects;
    context->d_func()->contextObjects = data;
}

void qmlExecuteDeferred(QObject *object)
{
    QmlDeclarativeData *data = QmlDeclarativeData::get(object);

    if (data && data->deferredComponent) {

        QmlEnginePrivate *ep = QmlEnginePrivate::get(data->context->engine());

        QmlComponentPrivate::ConstructionState state;
        QmlComponentPrivate::beginDeferred(data->context, ep, object, &state);

        data->deferredComponent->release();
        data->deferredComponent = 0;

        QmlComponentPrivate::complete(ep, &state);

        if (!state.errors.isEmpty())
            qWarning() << state.errors;

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
    QmlDeclarativeData *data = QmlDeclarativeData::get(object);
    if (!data)
        return 0; // Attached properties are only on objects created by QML

    QObject *rv = data->attachedProperties?data->attachedProperties->value(id):0;
    if (rv || !create)
        return rv;

    QmlAttachedPropertiesFunc pf = QmlMetaType::attachedPropertiesFuncById(id);
    if (!pf)
        return 0;

    rv = pf(const_cast<QObject *>(object));

    if (rv) {
        if (!data->attachedProperties)
            data->attachedProperties = new QHash<int, QObject *>();
        data->attachedProperties->insert(id, rv);
    }

    return rv;
}

void QmlDeclarativeData::destroyed(QObject *object)
{
    if (deferredComponent)
        deferredComponent->release();
    if (attachedProperties)
        delete attachedProperties;

    if (nextContextObject) 
        nextContextObject->prevContextObject = prevContextObject;
    if (prevContextObject)
        *prevContextObject = nextContextObject;

    QmlAbstractBinding *binding = bindings;
    while (binding) {
        QmlAbstractBinding *next = binding->m_nextBinding;
        binding->m_prevBinding = 0;
        binding->m_nextBinding = 0;
        binding->destroy();
        binding = next;
    }

    if (bindingBits)
        free(bindingBits);

    if (propertyCache)
        propertyCache->release();

    QmlGuard<QObject> *guard = guards;
    while (guard) {
        QmlGuard<QObject> *g = guard;
        guard = guard->next;
        g->o = 0;
        g->prev = 0;
        g->next = 0;
        g->objectDestroyed(object);
    }

    delete this;
}

bool QmlDeclarativeData::hasBindingBit(int bit) const
{
    if (bindingBitsSize > bit) 
        return bindingBits[bit / 32] & (1 << (bit % 32));
    else
        return false;
}

void QmlDeclarativeData::clearBindingBit(int bit)
{
    if (bindingBitsSize > bit) 
        bindingBits[bit / 32] &= ~(1 << (bit % 32));
}

void QmlDeclarativeData::setBindingBit(QObject *obj, int bit)
{
    if (bindingBitsSize <= bit) {
        int props = obj->metaObject()->propertyCount();
        Q_ASSERT(bit < props);

        int arraySize = (props + 31) / 32;
        int oldArraySize = bindingBitsSize / 32;

        bindingBits = (quint32 *)realloc(bindingBits, 
                                         arraySize * sizeof(quint32));

        memset(bindingBits + oldArraySize, 
               0x00,
               sizeof(quint32) * (arraySize - oldArraySize));

        bindingBitsSize = arraySize * 32;
    }

    bindingBits[bit / 32] |= (1 << (bit % 32));
}

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
    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);
    return enginePriv->objectClass->newQObject(object);
}

/*!
    Returns the QmlContext for the executing QScript \a ctxt.
*/
QmlContext *QmlEnginePrivate::getContext(QScriptContext *ctxt)
{
    QScriptValue scopeNode = QScriptDeclarativeClass::scopeChainValue(ctxt, -3);
    Q_ASSERT(scopeNode.isValid());
    Q_ASSERT(QScriptDeclarativeClass::scriptClass(scopeNode) == contextClass);
    return contextClass->contextFromValue(scopeNode);
}

QScriptValue QmlEnginePrivate::createComponent(QScriptContext *ctxt,
                                               QScriptEngine *engine)
{
    QmlEnginePrivate *activeEnginePriv =
        static_cast<QmlScriptEngine*>(engine)->p;
    QmlEngine* activeEngine = activeEnginePriv->q_func();

    QmlContext* context = activeEnginePriv->getContext(ctxt);
    Q_ASSERT(context);
    if(ctxt->argumentCount() != 1) {
        return engine->nullValue();
    }else{
        QString arg = ctxt->argument(0).toString();
        if (arg.isEmpty())
            return engine->nullValue();
        QUrl url = QUrl(context->resolvedUrl(QUrl(arg)));
        QmlComponent *c = new QmlComponent(activeEngine, url, activeEngine);
        c->setCreationContext(context);
        return activeEnginePriv->objectClass->newQObject(c, qMetaTypeId<QmlComponent*>());
    }
}

QScriptValue QmlEnginePrivate::createQmlObject(QScriptContext *ctxt, QScriptEngine *engine)
{
    QmlEnginePrivate *activeEnginePriv =
        static_cast<QmlScriptEngine*>(engine)->p;
    QmlEngine* activeEngine = activeEnginePriv->q_func();

    if(ctxt->argumentCount() < 2 || ctxt->argumentCount() > 3)
        return engine->nullValue();

    QmlContext* context = activeEnginePriv->getContext(ctxt);
    Q_ASSERT(context);

    QString qml = ctxt->argument(0).toString();
    if (qml.isEmpty())
        return engine->nullValue();

    QUrl url;
    if(ctxt->argumentCount() > 2)
        url = QUrl(ctxt->argument(2).toString());
    else
        url = QUrl(QLatin1String("inline"));

    if (url.isValid() && url.isRelative())
        url = context->resolvedUrl(url);

    QObject *parentArg = activeEnginePriv->objectClass->toQObject(ctxt->argument(1));
    if(!parentArg) 
        return engine->nullValue();

    QmlComponent component(activeEngine);
    component.setData(qml.toUtf8(), url);

    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        qWarning().nospace() << "QmlEngine::createQmlObject():";
        foreach (const QmlError &error, errors)
            qWarning().nospace() << "    " << error;

        return engine->nullValue();
    }

    if (!component.isReady()) {
        qWarning().nospace() << "QmlEngine::createQmlObject(): Component is not ready";

        return engine->nullValue();
    }

    QObject *obj = component.create(context);

    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        qWarning().nospace() << "QmlEngine::createQmlObject():";
        foreach (const QmlError &error, errors)
            qWarning().nospace() << "    " << error;

        return engine->nullValue();
    }

    Q_ASSERT(obj);

    obj->setParent(parentArg);
    QGraphicsObject* gobj = qobject_cast<QGraphicsObject*>(obj);
    QGraphicsObject* gparent = qobject_cast<QGraphicsObject*>(parentArg);
    if(gobj && gparent)
        gobj->setParentItem(gparent);

    return qmlScriptObject(obj, activeEngine);
}

QScriptValue QmlEnginePrivate::vector(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 3)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    qsreal z = ctxt->argument(2).toNumber();
    return engine->newVariant(qVariantFromValue(QVector3D(x, y, z)));
}

QScriptValue QmlEnginePrivate::rgba(QScriptContext *ctxt, QScriptEngine *engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount < 3 || argCount > 4)
        return engine->nullValue();
    qsreal r = ctxt->argument(0).toNumber();
    qsreal g = ctxt->argument(1).toNumber();
    qsreal b = ctxt->argument(2).toNumber();
    qsreal a = (argCount == 4) ? ctxt->argument(3).toNumber() : 1;

    if (r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1 || a < 0 || a > 1)
        return engine->nullValue();

    return qScriptValueFromValue(engine, qVariantFromValue(QColor::fromRgbF(r, g, b, a)));
}

QScriptValue QmlEnginePrivate::hsla(QScriptContext *ctxt, QScriptEngine *engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount < 3 || argCount > 4)
        return engine->nullValue();
    qsreal h = ctxt->argument(0).toNumber();
    qsreal s = ctxt->argument(1).toNumber();
    qsreal l = ctxt->argument(2).toNumber();
    qsreal a = (argCount == 4) ? ctxt->argument(3).toNumber() : 1;

    if (h < 0 || h > 1 || s < 0 || s > 1 || l < 0 || l > 1 || a < 0 || a > 1)
        return engine->nullValue();

    return qScriptValueFromValue(engine, qVariantFromValue(QColor::fromHslF(h, s, l, a)));
}

QScriptValue QmlEnginePrivate::rect(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 4)
        return engine->nullValue();

    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    qsreal w = ctxt->argument(2).toNumber();
    qsreal h = ctxt->argument(3).toNumber();

    if (w < 0 || h < 0)
        return engine->nullValue();

    return qScriptValueFromValue(engine, qVariantFromValue(QRectF(x, y, w, h)));
}

QScriptValue QmlEnginePrivate::point(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 2)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QPointF(x, y)));
}

QScriptValue QmlEnginePrivate::size(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 2)
        return engine->nullValue();
    qsreal w = ctxt->argument(0).toNumber();
    qsreal h = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QSizeF(w, h)));
}

QScriptValue QmlEnginePrivate::lighter(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.userType() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        color = QmlStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();
    color = color.lighter();
    return qScriptValueFromValue(engine, qVariantFromValue(color));
}

QScriptValue QmlEnginePrivate::darker(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.userType() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        color = QmlStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();
    color = color.darker();
    return qScriptValueFromValue(engine, qVariantFromValue(color));
}

QScriptValue QmlEnginePrivate::playSound(QScriptContext *ctxt, QScriptEngine *engine)
{
    if (ctxt->argumentCount() != 1)
        return engine->undefinedValue();

    QUrl url(ctxt->argument(0).toString());

    QmlEnginePrivate *enginePriv = QmlEnginePrivate::get(engine);
    if (url.isRelative()) {
        QmlContext *context = enginePriv->getContext(ctxt);
        if (!context)
            return engine->undefinedValue();

        url = context->resolvedUrl(url);
    }

    if (url.scheme() == QLatin1String("file")) {

        QSound::play(url.toLocalFile());

    }
    return engine->undefinedValue();
}

QScriptValue QmlEnginePrivate::desktopOpenUrl(QScriptContext *ctxt, QScriptEngine *e)
{
    if(ctxt->argumentCount() < 1)
        return e->newVariant(QVariant(false));
    bool ret = QDesktopServices::openUrl(QUrl(ctxt->argument(0).toString()));
    return e->newVariant(QVariant(ret));
}

QScriptValue QmlEnginePrivate::md5(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    QByteArray result = QCryptographicHash::hash(data, QCryptographicHash::Md5);

    return QScriptValue(QLatin1String(result.toHex()));
}

QScriptValue QmlEnginePrivate::btoa(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    return QScriptValue(QLatin1String(data.toBase64()));
}

QScriptValue QmlEnginePrivate::atob(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    return QScriptValue(QLatin1String(QByteArray::fromBase64(data)));
}

QScriptValue QmlEnginePrivate::consoleLog(QScriptContext *ctxt, QScriptEngine *e)
{
    if(ctxt->argumentCount() < 1)
        return e->newVariant(QVariant(false));

    QByteArray msg;

    for (int i=0; i<ctxt->argumentCount(); ++i) {
        if (!msg.isEmpty()) msg += ' ';
        msg += ctxt->argument(i).toString().toLocal8Bit();
        // does not support firebug "%[a-z]" formatting, since firebug really
        // does just ignore the format letter, which makes it pointless.
    }

    qDebug("%s",msg.constData());

    return e->newVariant(QVariant(true));
}

void QmlEnginePrivate::sendQuit ()
{
    Q_Q(QmlEngine);
    emit q->quit ();
}

QScriptValue QmlEnginePrivate::quit(QScriptContext * /*ctxt*/, QScriptEngine *e)
{
    QmlEnginePrivate *qe = get (e);
    qe->sendQuit ();
    return QScriptValue();
}

QScriptValue QmlEnginePrivate::closestAngle(QScriptContext *ctxt, QScriptEngine *e)
{
    if(ctxt->argumentCount() < 2)
        return e->newVariant(QVariant(0.0));
    qreal a = ctxt->argument(0).toNumber();
    qreal b = ctxt->argument(1).toNumber();
    qreal ret = b;
    qreal diff = b-a;
    while(diff > 180.0){
        ret -= 360.0;
        diff -= 360.0;
    }
    while(diff < -180.0){
        ret += 360.0;
        diff += 360.0;
    }
    return e->newVariant(QVariant(ret));
}

QScriptValue QmlEnginePrivate::tint(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 2)
        return engine->nullValue();
    //get color
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.userType() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        color = QmlStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();

    //get tint color
    v = ctxt->argument(1).toVariant();
    QColor tintColor;
    if (v.userType() == QVariant::Color)
        tintColor = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        tintColor = QmlStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();

    //tint
    QColor finalColor;
    int a = tintColor.alpha();
    if (a == 0xFF)
        finalColor = tintColor;
    else if (a == 0x00)
        finalColor = color;
    else {
        uint src = tintColor.rgba();
        uint dest = color.rgba();

        uint res = (((a * (src & 0xFF00FF)) +
                    ((0xFF - a) * (dest & 0xFF00FF))) >> 8) & 0xFF00FF;
        res |= (((a * ((src >> 8) & 0xFF00FF)) +
                ((0xFF - a) * ((dest >> 8) & 0xFF00FF)))) & 0xFF00FF00;
        if ((src & 0xFF000000) == 0xFF000000)
            res |= 0xFF000000;

        finalColor = QColor::fromRgba(res);
    }

    return qScriptValueFromValue(engine, qVariantFromValue(finalColor));
}


QScriptValue QmlEnginePrivate::scriptValueFromVariant(const QVariant &val)
{
    bool objOk;
    QObject *obj = QmlMetaType::toQObject(val, &objOk);
    if (objOk) {
        return objectClass->newQObject(obj);
    } else {
        return qScriptValueFromValue(&scriptEngine, val);
    }
}

QVariant QmlEnginePrivate::scriptValueToVariant(const QScriptValue &val)
{
    QScriptDeclarativeClass *dc = QScriptDeclarativeClass::scriptClass(val);
    if (dc == objectClass)
        return QVariant::fromValue(objectClass->toQObject(val));
    else if (dc == contextClass)
        return QVariant();

    QScriptDeclarativeClass *sc = QScriptDeclarativeClass::scriptClass(val);
    if (!sc) {
        return val.toVariant();
    } else if (sc == valueTypeClass) {
        return valueTypeClass->toVariant(val);
    } else {
        return QVariant();
    }
}

QmlScriptClass::QmlScriptClass(QScriptEngine *engine)
: QScriptDeclarativeClass(engine)
{
}

QVariant QmlScriptClass::toVariant(QmlEngine *engine, const QScriptValue &val)
{
    QmlEnginePrivate *ep =
        static_cast<QmlEnginePrivate *>(QObjectPrivate::get(engine));

    return ep->scriptValueToVariant(val);
}

// XXX this beyonds in QUrl::toLocalFile()
static QString toLocalFileOrQrc(const QUrl& url)
{
    QString r = url.toLocalFile();
    if (r.isEmpty() && url.scheme() == QLatin1String("qrc"))
        r = QLatin1Char(':') + url.path();
    return r;
}

/////////////////////////////////////////////////////////////
struct QmlEnginePrivate::ImportedNamespace {
    QStringList uris;
    QStringList urls;
    QList<int> majversions;
    QList<int> minversions;
    QList<bool> isLibrary;
    QList<bool> isBuiltin; // Types provided by C++ code (including plugins)
    QList<QString> qmlDirContent;

    bool find(const QByteArray& type, int *vmajor, int *vminor, QmlType** type_return, QUrl* url_return) const
    {
        for (int i=0; i<urls.count(); ++i) {
            int vmaj = majversions.at(i);
            int vmin = minversions.at(i);

            if (isBuiltin.at(i)) {
                QByteArray qt = uris.at(i).toUtf8();
                qt += '/';
                qt += type;
                if (qmlImportTrace())
                    qDebug() << "Look in" << qt;
                QmlType *t = QmlMetaType::qmlType(qt,vmaj,vmin);
                if (vmajor) *vmajor = vmaj;
                if (vminor) *vminor = vmin;
                if (t) {
                    if (qmlImportTrace())
                        qDebug() << "Found" << qt;
                    if (type_return)
                        *type_return = t;
                    return true;
                }
            }
            QUrl url = QUrl(urls.at(i) + QLatin1Char('/') + QString::fromUtf8(type) + QLatin1String(".qml"));
            QString qmldircontent = qmlDirContent.at(i);
            if (vmaj>=0 || !qmldircontent.isEmpty()) {
                // Check version file - XXX cache these in QmlEngine!
                if (qmldircontent.isEmpty()) {
                    QFile qmldir(toLocalFileOrQrc(QUrl(urls.at(i)+QLatin1String("/qmldir"))));
                    if (qmldir.open(QIODevice::ReadOnly)) {
                        qmldircontent = QString::fromUtf8(qmldir.readAll());
                    }
                }
                QString typespace = QString::fromUtf8(type)+QLatin1Char(' ');
                QStringList lines = qmldircontent.split(QLatin1Char('\n'));
                foreach (QString line, lines) {
                    if (line.isEmpty() || line.at(0) == QLatin1Char('#'))
                        continue;
                    if (line.startsWith(typespace)) {
                        int space1 = line.indexOf(QLatin1Char(' '));
                        int space2 = space1 >=0 ? line.indexOf(QLatin1Char(' '),space1+1) : -1;
                        QString mapversions = line.mid(space1+1,space2<0?line.length()-space1-1:space2-space1-1);
                        int dot = mapversions.indexOf(QLatin1Char('.'));
                        int mapvmaj = mapversions.left(dot).toInt();
                        if (mapvmaj<=vmaj) {
                            if (mapvmaj<vmaj || vmin >= mapversions.mid(dot+1).toInt()) {
                                QStringRef mapfile = space2<0 ? QStringRef() : line.midRef(space2+1,line.length()-space2-1);
                                if (url_return)
                                    *url_return = url.resolved(QUrl(mapfile.toString()));
                                return true;
                            }
                        }
                    }
                }
            } else {
                // XXX search non-files too! (eg. zip files, see QT-524)
                QFileInfo f(toLocalFileOrQrc(url));
                if (f.exists()) {
                    if (url_return)
                        *url_return = url;
                    return true;
                }
            }
        }
        return false;
    }
};

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QmlModuleFactoryInterface_iid, QLatin1String("/qmlmodules")))

class QmlImportsPrivate {
public:
    QmlImportsPrivate() : ref(1)
    {
    }

    ~QmlImportsPrivate()
    {
        foreach (QmlEnginePrivate::ImportedNamespace* s, set.values())
            delete s;
    }

    bool add(const QUrl& base, const QString& qmldircontent, const QString& uri, const QString& prefix, int vmaj, int vmin, QmlScriptParser::Import::Type importType, const QStringList& importPath)
    {
        QmlEnginePrivate::ImportedNamespace *s;
        if (prefix.isEmpty()) {
            s = &unqualifiedset;
        } else {
            s = set.value(prefix);
            if (!s)
                set.insert(prefix,(s=new QmlEnginePrivate::ImportedNamespace));
        }
        QString url = uri;
        bool isbuiltin = false;
        if (importType == QmlScriptParser::Import::Library) {
            url.replace(QLatin1Char('.'),QLatin1Char('/'));
            bool found = false;
            foreach (QString p, importPath) {
                QString dir = p+QLatin1Char('/')+url;
                QFileInfo fi(dir+QLatin1String("/qmldir"));
                if (fi.isFile()) {
                    url = QUrl::fromLocalFile(fi.absolutePath()).toString();
                    found = true;
                    break;
                }
            }
            if (!found) {
                // XXX assume it is a built-in type qualifier
                isbuiltin = true;
            }
            QFactoryLoader *l = loader();
            QmlModuleFactoryInterface *factory =
                qobject_cast<QmlModuleFactoryInterface*>(l->instance(uri));
            if (factory) {
                factory->defineModuleOnce(uri);
                isbuiltin = true;
            }
        } else {
            url = base.resolved(QUrl(url)).toString();
        }
        s->uris.prepend(uri);
        s->urls.prepend(url);
        s->majversions.prepend(vmaj);
        s->minversions.prepend(vmin);
        s->isLibrary.prepend(importType == QmlScriptParser::Import::Library);
        s->isBuiltin.prepend(isbuiltin);
        s->qmlDirContent.prepend(qmldircontent);
        return true;
    }

    bool find(const QByteArray& type, int *vmajor, int *vminor, QmlType** type_return, QUrl* url_return)
    {
        QmlEnginePrivate::ImportedNamespace *s = 0;
        int slash = type.indexOf('/');
        if (slash >= 0) {
            s = set.value(QString::fromUtf8(type.left(slash)));
            if (!s)
                return false; // qualifier must be a namespace
            int nslash = type.indexOf('/',slash+1);
            if (nslash > 0)
                return false; // only single qualification allowed
        } else {
            s = &unqualifiedset;
        }
        QByteArray unqualifiedtype = slash < 0 ? type : type.mid(slash+1); // common-case opt (QString::mid works fine, but slower)
        if (s) {
            if (s->find(unqualifiedtype,vmajor,vminor,type_return,url_return))
                return true;
            if (s->urls.count() == 1 && !s->isBuiltin[0] && !s->isLibrary[0] && url_return) {
                *url_return = QUrl(s->urls[0]+QLatin1Char('/')).resolved(QUrl(QString::fromUtf8(unqualifiedtype) + QLatin1String(".qml")));
                return true;
            }
        }
        if (url_return) {
            *url_return = base.resolved(QUrl(QString::fromUtf8(type + ".qml")));
            return true;
        } else {
            return false;
        }
    }

    QmlEnginePrivate::ImportedNamespace *findNamespace(const QString& type)
    {
        return set.value(type);
    }

    QUrl base;
    int ref;

private:
    friend struct QmlEnginePrivate::Imports;
    QmlEnginePrivate::ImportedNamespace unqualifiedset;
    QHash<QString,QmlEnginePrivate::ImportedNamespace* > set;
};

QmlEnginePrivate::Imports::Imports(const Imports &copy) :
    d(copy.d)
{
    ++d->ref;
}

QmlEnginePrivate::Imports &QmlEnginePrivate::Imports::operator =(const Imports &copy)
{
    ++copy.d->ref;
    if (--d->ref == 0)
        delete d;
    d = copy.d;
    return *this;
}

QmlEnginePrivate::Imports::Imports() :
    d(new QmlImportsPrivate)
{
}

QmlEnginePrivate::Imports::~Imports()
{
    if (--d->ref == 0)
        delete d;
}

#include "qmlmetatype.h"
#include "qmltypenamecache_p.h"

static QmlTypeNameCache *cacheForNamespace(QmlEngine *engine, const QmlEnginePrivate::ImportedNamespace &set, QmlTypeNameCache *cache)
{
    if (!cache)
        cache = new QmlTypeNameCache(engine);

    QList<QmlType *> types = QmlMetaType::qmlTypes();

    for (int ii = 0; ii < set.uris.count(); ++ii) {
        if (!set.isBuiltin.at(ii))
            continue;

        QByteArray base = set.uris.at(ii).toUtf8() + '/';
        int major = set.majversions.at(ii);
        int minor = set.minversions.at(ii);

        foreach (QmlType *type, types) {
            if (type->qmlTypeName().startsWith(base) && 
                type->qmlTypeName().lastIndexOf('/') == (base.length() - 1) && 
                type->availableInVersion(major,minor))
            {
                QString name = QString::fromUtf8(type->qmlTypeName().mid(base.length()));

                cache->add(name, type);
            }
        }
    }

    return cache;
}

QmlTypeNameCache *QmlEnginePrivate::Imports::cache(QmlEngine *engine) const
{
    const QmlEnginePrivate::ImportedNamespace &set = d->unqualifiedset;

    QmlTypeNameCache *cache = new QmlTypeNameCache(engine);

    for (QHash<QString,QmlEnginePrivate::ImportedNamespace* >::ConstIterator iter = d->set.begin();
         iter != d->set.end(); ++iter) {

        QmlTypeNameCache::Data *d = cache->data(iter.key());
        if (d) {
            if (!d->typeNamespace)
                cacheForNamespace(engine, *(*iter), d->typeNamespace);
        } else {
            QmlTypeNameCache *nc = cacheForNamespace(engine, *(*iter), 0);
            cache->add(iter.key(), nc);
            nc->release();
        }
    }

    cacheForNamespace(engine, set, cache);

    return cache;
}

/*
QStringList QmlEnginePrivate::Imports::unqualifiedSet() const
{
    QStringList rv;

    const QmlEnginePrivate::ImportedNamespace &set = d->unqualifiedset;

    for (int ii = 0; ii < set.urls.count(); ++ii) {
        if (set.isBuiltin.at(ii))
            rv << set.urls.at(ii);
    }

    return rv;
}
*/

/*!
  Sets the base URL to be used for all relative file imports added.
*/
void QmlEnginePrivate::Imports::setBaseUrl(const QUrl& url)
{
    d->base = url;
}

/*!
  Returns the base URL to be used for all relative file imports added.
*/
QUrl QmlEnginePrivate::Imports::baseUrl() const
{
    return d->base;
}

/*!
  Adds \a path as a directory where installed QML components are
  defined in a URL-based directory structure.

  For example, if you add \c /opt/MyApp/lib/qml and then load QML
  that imports \c com.mycompany.Feature, then QmlEngine will look
  in \c /opt/MyApp/lib/qml/com/mycompany/Feature/ for the components
  provided by that module (and in the case of versioned imports,
  for the \c qmldir file definiting the type version mapping.

  By default, only the "qml" subdirectory of QLibraryInfo::location(QLibraryInfo::DataPath)
  is included on the import path.
*/
void QmlEngine::addImportPath(const QString& path)
{
    if (qmlImportTrace())
        qDebug() << "QmlEngine::addImportPath" << path;
    Q_D(QmlEngine);
    d->fileImportPath.prepend(path);
}

/*!
  \property QmlEngine::offlineStoragePath
  \brief the directory for storing offline user data

  Returns the directory where SQL and other offline
  storage is placed.

  QmlGraphicsWebView and the SQL databases created with openDatabase()
  are stored here.

  The default is QML/OfflineStorage in the platform-standard
  user application data directory.

  Note that the path may not currently exist on the filesystem, so
  callers wanting to \e create new files at this location should create
  it first - see QDir::mkpath().
*/
void QmlEngine::setOfflineStoragePath(const QString& dir)
{
    Q_D(QmlEngine);
    d->scriptEngine.offlineStoragePath = dir;
}

QString QmlEngine::offlineStoragePath() const
{
    Q_D(const QmlEngine);
    return d->scriptEngine.offlineStoragePath;
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
bool QmlEnginePrivate::addToImport(Imports* imports, const QString& qmldircontent, const QString& uri, const QString& prefix, int vmaj, int vmin, QmlScriptParser::Import::Type importType) const
{
    bool ok = imports->d->add(imports->d->base,qmldircontent,uri,prefix,vmaj,vmin,importType,fileImportPath);
    if (qmlImportTrace())
        qDebug() << "QmlEngine::addToImport(" << imports << uri << prefix << vmaj << '.' << vmin << (importType==QmlScriptParser::Import::Library? "Library" : "File") << ": " << ok;
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
bool QmlEnginePrivate::resolveType(const Imports& imports, const QByteArray& type, QmlType** type_return, QUrl* url_return, int *vmaj, int *vmin, ImportedNamespace** ns_return) const
{
    ImportedNamespace* ns = imports.d->findNamespace(QString::fromUtf8(type));
    if (ns) {
        if (qmlImportTrace())
            qDebug() << "QmlEngine::resolveType" << type << "is namespace for" << ns->urls;
        if (ns_return)
            *ns_return = ns;
        return true;
    }
    if (type_return || url_return) {
        if (imports.d->find(type,vmaj,vmin,type_return,url_return)) {
            if (qmlImportTrace()) {
                if (type_return && *type_return)
                    qDebug() << "QmlEngine::resolveType" << type << '=' << (*type_return)->typeName();
                if (url_return)
                    qDebug() << "QmlEngine::resolveType" << type << '=' << *url_return;
            }
            return true;
        }
        if (qmlImportTrace())
            qDebug() << "QmlEngine::resolveType" << type << "not found";
    }
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
void QmlEnginePrivate::resolveTypeInNamespace(ImportedNamespace* ns, const QByteArray& type, QmlType** type_return, QUrl* url_return, int *vmaj, int *vmin ) const
{
    ns->find(type,vmaj,vmin,type_return,url_return);
}

static void voidptr_destructor(void *v)
{
    void **ptr = (void **)v;
    delete ptr;
}

static void *voidptr_constructor(const void *v)
{
    if (!v) {
        return new void*;
    } else {
        return new void*(*(void **)v);
    }
}

void QmlEnginePrivate::registerCompositeType(QmlCompiledData *data)
{
    QByteArray name = data->root->className();

    QByteArray ptr = name + '*';
    QByteArray lst = "QmlList<" + ptr + ">*";

    int ptr_type = QMetaType::registerType(ptr.constData(), voidptr_destructor,
                                           voidptr_constructor);
    int lst_type = QMetaType::registerType(lst.constData(), voidptr_destructor,
                                           voidptr_constructor);

    m_qmlLists.insert(lst_type, ptr_type);
    m_compositeTypes.insert(ptr_type, data);
    data->addref();
}

bool QmlEnginePrivate::isQmlList(int t) const
{
    return m_qmlLists.contains(t) || QmlMetaType::isQmlList(t);
}

bool QmlEnginePrivate::isQObject(int t)
{
    return m_compositeTypes.contains(t) || QmlMetaType::isQObject(t);
}

QObject *QmlEnginePrivate::toQObject(const QVariant &v, bool *ok) const
{
    int t = v.userType();
    if (m_compositeTypes.contains(t)) {
        if (ok) *ok = true;
        return *(QObject **)(v.constData());
    } else {
        return QmlMetaType::toQObject(v, ok);
    }
}

int QmlEnginePrivate::qmlListType(int t) const
{
    QHash<int, int>::ConstIterator iter = m_qmlLists.find(t);
    if (iter != m_qmlLists.end())
        return *iter;
    else
        return QmlMetaType::qmlListType(t);
}

QmlMetaType::TypeCategory QmlEnginePrivate::typeCategory(int t) const
{
    if (m_compositeTypes.contains(t))
        return QmlMetaType::Object;
    else if (m_qmlLists.contains(t))
        return QmlMetaType::QmlList;
    else
        return QmlMetaType::typeCategory(t);
}

const QMetaObject *QmlEnginePrivate::rawMetaObjectForType(int t) const
{
    QHash<int, QmlCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        QmlType *type = QmlMetaType::qmlType(t);
        return type?type->baseMetaObject():0;
    }
}

const QMetaObject *QmlEnginePrivate::metaObjectForType(int t) const
{
    QHash<int, QmlCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        QmlType *type = QmlMetaType::qmlType(t);
        return type?type->metaObject():0;
    }
}

QT_END_NAMESPACE
