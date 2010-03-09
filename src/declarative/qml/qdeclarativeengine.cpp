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

#include "qdeclarativeengine_p.h"
#include "qdeclarativeengine.h"

#include "qdeclarativecontext_p.h"
#include "qdeclarativecompiler_p.h"
#include "qdeclarativeglobalscriptclass_p.h"
#include "qdeclarative.h"
#include "qdeclarativecontext.h"
#include "qdeclarativeexpression.h"
#include "qdeclarativecomponent.h"
#include "qdeclarativebinding_p_p.h"
#include "qdeclarativevme_p.h"
#include "qdeclarativeenginedebug_p.h"
#include "qdeclarativestringconverters_p.h"
#include "qdeclarativexmlhttprequest_p.h"
#include "qdeclarativesqldatabase_p.h"
#include "qdeclarativetypenamescriptclass_p.h"
#include "qdeclarativelistscriptclass_p.h"
#include "qdeclarativescriptstring.h"
#include "qdeclarativeglobal_p.h"
#include "qdeclarativeworkerscript_p.h"
#include "qdeclarativecomponent_p.h"
#include "qdeclarativescriptclass_p.h"
#include "qdeclarativenetworkaccessmanagerfactory.h"
#include "qdeclarativeimageprovider.h"
#include "qdeclarativedirparser_p.h"
#include "qdeclarativeextensioninterface.h"
#include "qdeclarativelist_p.h"
#include "qdeclarativetypenamecache_p.h"

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
#include <QPluginLoader>
#include <QtCore/qlibraryinfo.h>
#include <QtCore/qthreadstorage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtCore/qmutex.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qsound.h>
#include <QGraphicsObject>
#include <QtCore/qcryptographichash.h>

#include <private/qobject_p.h>
#include <private/qscriptdeclarativeclass_p.h>

#include <private/qdeclarativeitemsmodule_p.h>
#include <private/qdeclarativeutilmodule_p.h>

#ifdef Q_OS_WIN // for %APPDATA%
#include <qt_windows.h>
#include <qlibrary.h>

#define CSIDL_APPDATA		0x001a	// <username>\Application Data
#endif

Q_DECLARE_METATYPE(QDeclarativeProperty)

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(qmlImportTrace, QML_IMPORT_TRACE)

/*!
    \qmlclass QtObject QObject
  \since 4.7
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

void QDeclarativeEnginePrivate::defineModule()
{
    QML_REGISTER_TYPE(Qt,4,6,Component,QDeclarativeComponent);
    QML_REGISTER_TYPE(Qt,4,6,QtObject,QObject);
    QML_REGISTER_TYPE(Qt,4,6,WorkerScript,QDeclarativeWorkerScript);
    QML_REGISTER_TYPE(Qt,4,6,WorkerListModel,QDeclarativeWorkerListModel);

    QML_REGISTER_NOCREATE_TYPE(QDeclarativeBinding);
}

QDeclarativeEnginePrivate::QDeclarativeEnginePrivate(QDeclarativeEngine *e)
: captureProperties(false), rootContext(0), currentExpression(0), isDebugging(false), 
  contextClass(0), sharedContext(0), sharedScope(0), objectClass(0), valueTypeClass(0), 
  globalClass(0), cleanup(0), erroredBindings(0), inProgressCreations(0), 
  scriptEngine(this), workerScriptEngine(0), componentAttacheds(0), inBeginCreate(false), 
  networkAccessManager(0), networkAccessManagerFactory(0),
  typeManager(e), uniqueId(1)
{
    if (!qt_QmlQtModule_registered) {
        qt_QmlQtModule_registered = true;
        QDeclarativeItemModule::defineModule();
        QDeclarativeUtilModule::defineModule();
        QDeclarativeEnginePrivate::defineModule();
        QDeclarativeValueTypeFactory::registerValueTypes();
    }
    globalClass = new QDeclarativeGlobalScriptClass(&scriptEngine);

    // env import paths
    QByteArray envImportPath = qgetenv("QML_IMPORT_PATH");
    if (!envImportPath.isEmpty()) {
#if defined(Q_OS_WIN) || defined(Q_OS_SYMBIAN)
        QLatin1Char pathSep(';');
#else
        QLatin1Char pathSep(':');
#endif
        foreach (const QString &path, QString::fromLatin1(envImportPath).split(pathSep, QString::SkipEmptyParts)) {
            QString canonicalPath = QDir(path).canonicalPath();
            if (!canonicalPath.isEmpty() && !environmentImportPath.contains(canonicalPath))
                environmentImportPath.append(canonicalPath);
        }
    }
}

QUrl QDeclarativeScriptEngine::resolvedUrl(QScriptContext *context, const QUrl& url)
{
    if (p) {
        QDeclarativeContext *ctxt = QDeclarativeEnginePrivate::get(this)->getContext(context);
        Q_ASSERT(ctxt);
        return ctxt->resolvedUrl(url);
    }
    return baseUrl.resolved(url);
}

QDeclarativeScriptEngine::QDeclarativeScriptEngine(QDeclarativeEnginePrivate *priv)
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
    qtObject.setProperty(QLatin1String("rgba"), newFunction(QDeclarativeEnginePrivate::rgba, 4));
    qtObject.setProperty(QLatin1String("hsla"), newFunction(QDeclarativeEnginePrivate::hsla, 4));
    qtObject.setProperty(QLatin1String("rect"), newFunction(QDeclarativeEnginePrivate::rect, 4));
    qtObject.setProperty(QLatin1String("point"), newFunction(QDeclarativeEnginePrivate::point, 2));
    qtObject.setProperty(QLatin1String("size"), newFunction(QDeclarativeEnginePrivate::size, 2));
    qtObject.setProperty(QLatin1String("vector3d"), newFunction(QDeclarativeEnginePrivate::vector, 3));

    if (mainthread) {
        //color helpers
        qtObject.setProperty(QLatin1String("lighter"), newFunction(QDeclarativeEnginePrivate::lighter, 1));
        qtObject.setProperty(QLatin1String("darker"), newFunction(QDeclarativeEnginePrivate::darker, 1));
        qtObject.setProperty(QLatin1String("tint"), newFunction(QDeclarativeEnginePrivate::tint, 2));
    }

    //date/time formatting
    qtObject.setProperty(QLatin1String("formatDate"),newFunction(QDeclarativeEnginePrivate::formatDate, 2));
    qtObject.setProperty(QLatin1String("formatTime"),newFunction(QDeclarativeEnginePrivate::formatTime, 2));
    qtObject.setProperty(QLatin1String("formatDateTime"),newFunction(QDeclarativeEnginePrivate::formatDateTime, 2));

    //misc methods
    qtObject.setProperty(QLatin1String("closestAngle"), newFunction(QDeclarativeEnginePrivate::closestAngle, 2));
    qtObject.setProperty(QLatin1String("openUrlExternally"),newFunction(QDeclarativeEnginePrivate::desktopOpenUrl, 1));
    qtObject.setProperty(QLatin1String("md5"),newFunction(QDeclarativeEnginePrivate::md5, 1));
    qtObject.setProperty(QLatin1String("btoa"),newFunction(QDeclarativeEnginePrivate::btoa, 1));
    qtObject.setProperty(QLatin1String("atob"),newFunction(QDeclarativeEnginePrivate::atob, 1));
    qtObject.setProperty(QLatin1String("quit"), newFunction(QDeclarativeEnginePrivate::quit, 0));
    qtObject.setProperty(QLatin1String("resolvedUrl"),newFunction(QDeclarativeScriptEngine::resolvedUrl, 1));

    //firebug/webkit compat
    QScriptValue consoleObject = newObject();
    consoleObject.setProperty(QLatin1String("log"),newFunction(QDeclarativeEnginePrivate::consoleLog, 1));
    consoleObject.setProperty(QLatin1String("debug"),newFunction(QDeclarativeEnginePrivate::consoleLog, 1));
    globalObject().setProperty(QLatin1String("console"), consoleObject);

    if (mainthread) {
        globalObject().setProperty(QLatin1String("createQmlObject"),
                newFunction(QDeclarativeEnginePrivate::createQmlObject, 1));
        globalObject().setProperty(QLatin1String("createComponent"),
                newFunction(QDeclarativeEnginePrivate::createComponent, 1));
    }

    // translation functions need to be installed
    // before the global script class is constructed (QTBUG-6437)
    installTranslatorFunctions();
}

QDeclarativeScriptEngine::~QDeclarativeScriptEngine()
{
    delete sqlQueryClass;
    delete nodeListClass;
    delete namedNodeMapClass;
}

QScriptValue QDeclarativeScriptEngine::resolvedUrl(QScriptContext *ctxt, QScriptEngine *engine)
{
    QString arg = ctxt->argument(0).toString();
    QUrl r = QDeclarativeScriptEngine::get(engine)->resolvedUrl(ctxt,QUrl(arg));
    return QScriptValue(r.toString());
}

QNetworkAccessManager *QDeclarativeScriptEngine::networkAccessManager()
{
    return p->getNetworkAccessManager();
}

QDeclarativeEnginePrivate::~QDeclarativeEnginePrivate()
{
    while (cleanup) {
        QDeclarativeCleanup *c = cleanup;
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
    for(QHash<int, QDeclarativeCompiledData*>::ConstIterator iter = m_compositeTypes.constBegin(); iter != m_compositeTypes.constEnd(); ++iter)
        (*iter)->release();
    for(QHash<const QMetaObject *, QDeclarativePropertyCache *>::Iterator iter = propertyCache.begin(); iter != propertyCache.end(); ++iter)
        (*iter)->release();

}

void QDeclarativeEnginePrivate::clear(SimpleList<QDeclarativeAbstractBinding> &bvs)
{
    bvs.clear();
}

void QDeclarativeEnginePrivate::clear(SimpleList<QDeclarativeParserStatus> &pss)
{
    for (int ii = 0; ii < pss.count; ++ii) {
        QDeclarativeParserStatus *ps = pss.at(ii);
        if(ps)
            ps->d = 0;
    }
    pss.clear();
}

Q_GLOBAL_STATIC(QDeclarativeEngineDebugServer, qmlEngineDebugServer);
Q_GLOBAL_STATIC(QSet<QString>, qmlEnginePluginsWithRegisteredTypes);

void QDeclarativeEnginePrivate::init()
{
    Q_Q(QDeclarativeEngine);
    qRegisterMetaType<QVariant>("QVariant");
    qRegisterMetaType<QDeclarativeScriptString>("QDeclarativeScriptString");
    qRegisterMetaType<QScriptValue>("QScriptValue");

    contextClass = new QDeclarativeContextScriptClass(q);
    objectClass = new QDeclarativeObjectScriptClass(q);
    valueTypeClass = new QDeclarativeValueTypeScriptClass(q);
    typeNameClass = new QDeclarativeTypeNameScriptClass(q);
    listClass = new QDeclarativeListScriptClass(q);
    rootContext = new QDeclarativeContext(q,true);

    if (QCoreApplication::instance()->thread() == q->thread() &&
        QDeclarativeEngineDebugServer::isDebuggingEnabled()) {
        qmlEngineDebugServer();
        isDebugging = true;
        QDeclarativeEngineDebugServer::addEngine(q);

        qmlEngineDebugServer()->waitForClients();
    }
}

QDeclarativeWorkerScriptEngine *QDeclarativeEnginePrivate::getWorkerScriptEngine()
{
    Q_Q(QDeclarativeEngine);
    if (!workerScriptEngine) 
        workerScriptEngine = new QDeclarativeWorkerScriptEngine(q);
    return workerScriptEngine;
}

/*!
    \class QDeclarativeEngine
  \since 4.7
    \brief The QDeclarativeEngine class provides an environment for instantiating QML components.
    \mainclass

    Each QML component is instantiated in a QDeclarativeContext.  QDeclarativeContext's are
    essential for passing data to QML components.  In QML, contexts are arranged
    hierarchically and this hierarchy is managed by the QDeclarativeEngine.

    Prior to creating any QML components, an application must have created a
    QDeclarativeEngine to gain access to a QML context.  The following example shows how
    to create a simple Text item.

    \code
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine);
    component.setData("import Qt 4.6\nText { text: \"Hello world!\" }", QUrl());
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(component.create());

    //add item to view, etc
    ...
    \endcode

    In this case, the Text item will be created in the engine's
    \l {QDeclarativeEngine::rootContext()}{root context}.

    \sa QDeclarativeComponent QDeclarativeContext
*/

/*!
    Create a new QDeclarativeEngine with the given \a parent.
*/
QDeclarativeEngine::QDeclarativeEngine(QObject *parent)
: QObject(*new QDeclarativeEnginePrivate(this), parent)
{
    Q_D(QDeclarativeEngine);
    d->init();
}

/*!
    Destroys the QDeclarativeEngine.

    Any QDeclarativeContext's created on this engine will be invalidated, but not
    destroyed (unless they are parented to the QDeclarativeEngine object).
*/
QDeclarativeEngine::~QDeclarativeEngine()
{
    Q_D(QDeclarativeEngine);
    if (d->isDebugging)
        QDeclarativeEngineDebugServer::remEngine(this);
}

/*! \fn void QDeclarativeEngine::quit()
  This signal is emitted when the QDeclarativeEngine quits.
 */

/*!
  Clears the engine's internal component cache.

  Normally the QDeclarativeEngine caches components loaded from qml files.  This method
  clears this cache and forces the component to be reloaded.
 */
void QDeclarativeEngine::clearComponentCache()
{
    Q_D(QDeclarativeEngine);
    d->typeManager.clearCache();
}

/*!
    Returns the engine's root context.

    The root context is automatically created by the QDeclarativeEngine.  Data that
    should be available to all QML component instances instantiated by the
    engine should be put in the root context.

    Additional data that should only be available to a subset of component
    instances should be added to sub-contexts parented to the root context.
*/
QDeclarativeContext *QDeclarativeEngine::rootContext()
{
    Q_D(QDeclarativeEngine);
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
void QDeclarativeEngine::setNetworkAccessManagerFactory(QDeclarativeNetworkAccessManagerFactory *factory)
{
    Q_D(QDeclarativeEngine);
    QMutexLocker locker(&d->mutex);
    d->networkAccessManagerFactory = factory;
}

/*!
    Returns the current QDeclarativeNetworkAccessManagerFactory.

    \sa setNetworkAccessManagerFactory()
*/
QDeclarativeNetworkAccessManagerFactory *QDeclarativeEngine::networkAccessManagerFactory() const
{
    Q_D(const QDeclarativeEngine);
    return d->networkAccessManagerFactory;
}

QNetworkAccessManager *QDeclarativeEnginePrivate::createNetworkAccessManager(QObject *parent) const
{
    QMutexLocker locker(&mutex);
    QNetworkAccessManager *nam;
    if (networkAccessManagerFactory) {
        nam = networkAccessManagerFactory->create(parent);
    } else {
        nam = new QNetworkAccessManager(parent);
    }

    return nam;
}

QNetworkAccessManager *QDeclarativeEnginePrivate::getNetworkAccessManager() const
{
    Q_Q(const QDeclarativeEngine);
    if (!networkAccessManager)
        networkAccessManager = createNetworkAccessManager(const_cast<QDeclarativeEngine*>(q));
    return networkAccessManager;
}

/*!
    Returns a common QNetworkAccessManager which can be used by any QML element
    instantiated by this engine.

    If a QDeclarativeNetworkAccessManagerFactory has been set and a QNetworkAccessManager
    has not yet been created, the QDeclarativeNetworkAccessManagerFactory will be used
    to create the QNetworkAccessManager; otherwise the returned QNetworkAccessManager
    will have no proxy or cache set.

    \sa setNetworkAccessManagerFactory()
*/
QNetworkAccessManager *QDeclarativeEngine::networkAccessManager() const
{
    Q_D(const QDeclarativeEngine);
    return d->getNetworkAccessManager();
}

/*!
    Sets the \a provider to use for images requested via the \e image: url
    scheme, with host \a providerId.

    QDeclarativeImageProvider allows images to be provided to QML asynchronously.
    The image request will be run in a low priority thread.  This allows
    potentially costly image loading to be done in the background, without
    affecting the performance of the UI.

    Note that images loaded from a QDeclarativeImageProvider are cached by
    QPixmapCache, similar to any image loaded by QML.

    The QDeclarativeEngine assumes ownership of the provider.

    This example creates a provider with id \e colors:

    \snippet examples/declarative/imageprovider/main.cpp 0

    \snippet examples/declarative/imageprovider/view.qml 0

    \sa removeImageProvider()
*/
void QDeclarativeEngine::addImageProvider(const QString &providerId, QDeclarativeImageProvider *provider)
{
    Q_D(QDeclarativeEngine);
    QMutexLocker locker(&d->mutex);
    d->imageProviders.insert(providerId, provider);
}

/*!
    Returns the QDeclarativeImageProvider set for \a providerId.
*/
QDeclarativeImageProvider *QDeclarativeEngine::imageProvider(const QString &providerId) const
{
    Q_D(const QDeclarativeEngine);
    QMutexLocker locker(&d->mutex);
    return d->imageProviders.value(providerId);
}

/*!
    Removes the QDeclarativeImageProvider for \a providerId.

    Returns the provider if it was found; otherwise returns 0.

    \sa addImageProvider()
*/
void QDeclarativeEngine::removeImageProvider(const QString &providerId)
{
    Q_D(QDeclarativeEngine);
    QMutexLocker locker(&d->mutex);
    delete d->imageProviders.take(providerId);
}

QImage QDeclarativeEnginePrivate::getImageFromProvider(const QUrl &url)
{
    QMutexLocker locker(&mutex);
    QImage image;
    QDeclarativeImageProvider *provider = imageProviders.value(url.host());
    if (provider)
        image = provider->request(url.path().mid(1));
    return image;
}

/*!
    Return the base URL for this engine.  The base URL is only used to resolve
    components when a relative URL is passed to the QDeclarativeComponent constructor.

    If a base URL has not been explicitly set, this method returns the
    application's current working directory.

    \sa setBaseUrl()
*/
QUrl QDeclarativeEngine::baseUrl() const
{
    Q_D(const QDeclarativeEngine);
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
void QDeclarativeEngine::setBaseUrl(const QUrl &url)
{
    Q_D(QDeclarativeEngine);
    d->baseUrl = url;
}

/*!
  Returns the QDeclarativeContext for the \a object, or 0 if no context has been set.

  When the QDeclarativeEngine instantiates a QObject, the context is set automatically.
  */
QDeclarativeContext *QDeclarativeEngine::contextForObject(const QObject *object)
{
    if(!object)
        return 0;

    QObjectPrivate *priv = QObjectPrivate::get(const_cast<QObject *>(object));

    QDeclarativeDeclarativeData *data =
        static_cast<QDeclarativeDeclarativeData *>(priv->declarativeData);

    if (!data)
        return 0;
    else if (data->outerContext)
        return data->outerContext;
    else
        return data->context;
}

/*!
  Sets the QDeclarativeContext for the \a object to \a context.
  If the \a object already has a context, a warning is
  output, but the context is not changed.

  When the QDeclarativeEngine instantiates a QObject, the context is set automatically.
 */
void QDeclarativeEngine::setContextForObject(QObject *object, QDeclarativeContext *context)
{
    if (!object || !context)
        return;

    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(object, true);
    if (data->context) {
        qWarning("QDeclarativeEngine::setContextForObject(): Object already has a QDeclarativeContext");
        return;
    }

    data->context = context;
    data->nextContextObject = context->d_func()->contextObjects;
    if (data->nextContextObject) 
        data->nextContextObject->prevContextObject = &data->nextContextObject;
    data->prevContextObject = &context->d_func()->contextObjects;
    context->d_func()->contextObjects = data;
}

/*!
\enum QDeclarativeEngine::ObjectOwnership

Ownership controls whether or not QML automatically destroys the QObject when the object 
is garbage collected by the JavaScript engine.  The two ownership options are:

\o CppOwnership The object is owned by C++ code, and will never be deleted by QML.  The
JavaScript destroy() method cannot be used on objects with CppOwnership.  This option
is similar to QScriptEngine::QtOwnership.

\o JavaScriptOwnership The object is owned by JavaScript.  When the object is returned to QML 
as the return value of a method call or property access, QML will delete the object if there
are no remaining JavaScript references to it and it has no QObject::parent().  This option
is similar to QScriptEngine::ScriptOwnership.

Generally an application doesn't need to set an object's ownership explicitly.  QML uses
a heuristic to set the default object ownership.  By default, an object that is created by
QML has JavaScriptOwnership.  The exception to this are the root objects created by calling
QDeclarativeCompnent::create() or QDeclarativeComponent::beginCreate() which have 
CppOwnership by default.  The ownership of these root-level objects is considered to have
been transfered to the C++ caller.

Objects not-created by QML have CppOwnership by default.  The exception to this is objects
returned from a C++ method call.  The ownership of these objects is passed to JavaScript.

Calling setObjectOwnership() overrides the default ownership heuristic used by QML.
*/

/*!
Sets the \a ownership of \a object.
*/
void QDeclarativeEngine::setObjectOwnership(QObject *object, ObjectOwnership ownership)
{
    QDeclarativeDeclarativeData *ddata = QDeclarativeDeclarativeData::get(object, true);
    if (!ddata)
        return;

    ddata->indestructible = (ownership == CppOwnership)?true:false;
    ddata->explicitIndestructibleSet = true;
}

/*!
Returns the ownership of \a object.
*/
QDeclarativeEngine::ObjectOwnership QDeclarativeEngine::objectOwnership(QObject *object)
{
    QDeclarativeDeclarativeData *ddata = QDeclarativeDeclarativeData::get(object, false);
    if (!ddata) 
        return CppOwnership;
    else
        return ddata->indestructible?CppOwnership:JavaScriptOwnership;
}

void qmlExecuteDeferred(QObject *object)
{
    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(object);

    if (data && data->deferredComponent) {

        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(data->context->engine());

        QDeclarativeComponentPrivate::ConstructionState state;
        QDeclarativeComponentPrivate::beginDeferred(data->context, ep, object, &state);

        data->deferredComponent->release();
        data->deferredComponent = 0;

        QDeclarativeComponentPrivate::complete(ep, &state);

        if (!state.errors.isEmpty())
            qWarning() << state.errors;

    }
}

QDeclarativeContext *qmlContext(const QObject *obj)
{
    return QDeclarativeEngine::contextForObject(obj);
}

QDeclarativeEngine *qmlEngine(const QObject *obj)
{
    QDeclarativeContext *context = QDeclarativeEngine::contextForObject(obj);
    return context?context->engine():0;
}

QObject *qmlAttachedPropertiesObjectById(int id, const QObject *object, bool create)
{
    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(object);
    if (!data)
        return 0; // Attached properties are only on objects created by QML

    QObject *rv = data->attachedProperties?data->attachedProperties->value(id):0;
    if (rv || !create)
        return rv;

    QDeclarativeAttachedPropertiesFunc pf = QDeclarativeMetaType::attachedPropertiesFuncById(id);
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

QObject *qmlAttachedPropertiesObject(int *idCache, const QObject *object, 
                                     const QMetaObject *attachedMetaObject, bool create)
{
    if (*idCache == -1)
        *idCache = QDeclarativeMetaType::attachedPropertiesFuncId(attachedMetaObject);

    if (*idCache == -1 || !object)
        return 0;

    return qmlAttachedPropertiesObjectById(*idCache, object, create);
}

void QDeclarativeDeclarativeData::destroyed(QObject *object)
{
    if (deferredComponent)
        deferredComponent->release();
    if (attachedProperties)
        delete attachedProperties;

    if (nextContextObject) 
        nextContextObject->prevContextObject = prevContextObject;
    if (prevContextObject)
        *prevContextObject = nextContextObject;

    QDeclarativeAbstractBinding *binding = bindings;
    while (binding) {
        QDeclarativeAbstractBinding *next = binding->m_nextBinding;
        binding->m_prevBinding = 0;
        binding->m_nextBinding = 0;
        binding->destroy();
        binding = next;
    }

    if (bindingBits)
        free(bindingBits);

    if (propertyCache)
        propertyCache->release();

    QDeclarativeGuard<QObject> *guard = guards;
    while (guard) {
        QDeclarativeGuard<QObject> *g = guard;
        guard = guard->next;
        g->o = 0;
        g->prev = 0;
        g->next = 0;
        g->objectDestroyed(object);
    }

    delete this;
}

void QDeclarativeDeclarativeData::parentChanged(QObject *, QObject *parent)
{
    if (!parent && scriptValue.isValid()) scriptValue = QScriptValue();
}

bool QDeclarativeDeclarativeData::hasBindingBit(int bit) const
{
    if (bindingBitsSize > bit) 
        return bindingBits[bit / 32] & (1 << (bit % 32));
    else
        return false;
}

void QDeclarativeDeclarativeData::clearBindingBit(int bit)
{
    if (bindingBitsSize > bit) 
        bindingBits[bit / 32] &= ~(1 << (bit % 32));
}

void QDeclarativeDeclarativeData::setBindingBit(QObject *obj, int bit)
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
    \a engine is the QDeclarativeEngine it is to be created in.

    The QScriptValue returned is a QtScript Object, not a QtScript QObject, due
    to the special needs of QML requiring more functionality than a standard
    QtScript QObject.
*/
QScriptValue QDeclarativeEnginePrivate::qmlScriptObject(QObject* object,
                                               QDeclarativeEngine* engine)
{
    QDeclarativeEnginePrivate *enginePriv = QDeclarativeEnginePrivate::get(engine);
    return enginePriv->objectClass->newQObject(object);
}

/*!
    Returns the QDeclarativeContext for the executing QScript \a ctxt.
*/
QDeclarativeContext *QDeclarativeEnginePrivate::getContext(QScriptContext *ctxt)
{
    QScriptValue scopeNode = QScriptDeclarativeClass::scopeChainValue(ctxt, -3);
    Q_ASSERT(scopeNode.isValid());
    Q_ASSERT(QScriptDeclarativeClass::scriptClass(scopeNode) == contextClass);
    return contextClass->contextFromValue(scopeNode);
}

QScriptValue QDeclarativeEnginePrivate::createComponent(QScriptContext *ctxt,
                                               QScriptEngine *engine)
{
    QDeclarativeEnginePrivate *activeEnginePriv =
        static_cast<QDeclarativeScriptEngine*>(engine)->p;
    QDeclarativeEngine* activeEngine = activeEnginePriv->q_func();

    QDeclarativeContext* context = activeEnginePriv->getContext(ctxt);
    Q_ASSERT(context);
    if(ctxt->argumentCount() != 1) {
        return engine->nullValue();
    }else{
        QString arg = ctxt->argument(0).toString();
        if (arg.isEmpty())
            return engine->nullValue();
        QUrl url = QUrl(context->resolvedUrl(QUrl(arg)));
        QDeclarativeComponent *c = new QDeclarativeComponent(activeEngine, url, activeEngine);
        c->setCreationContext(context);
        QDeclarativeDeclarativeData::get(c, true)->setImplicitDestructible();
        return activeEnginePriv->objectClass->newQObject(c, qMetaTypeId<QDeclarativeComponent*>());
    }
}

QScriptValue QDeclarativeEnginePrivate::createQmlObject(QScriptContext *ctxt, QScriptEngine *engine)
{
    QDeclarativeEnginePrivate *activeEnginePriv =
        static_cast<QDeclarativeScriptEngine*>(engine)->p;
    QDeclarativeEngine* activeEngine = activeEnginePriv->q_func();

    if(ctxt->argumentCount() < 2 || ctxt->argumentCount() > 3)
        return engine->nullValue();

    QDeclarativeContext* context = activeEnginePriv->getContext(ctxt);
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

    QDeclarativeComponent component(activeEngine);
    component.setData(qml.toUtf8(), url);

    if(component.isError()) {
        QList<QDeclarativeError> errors = component.errors();
        qWarning().nospace() << "QDeclarativeEngine::createQmlObject():";
        foreach (const QDeclarativeError &error, errors)
            qWarning().nospace() << "    " << error;

        return engine->nullValue();
    }

    if (!component.isReady()) {
        qWarning().nospace() << "QDeclarativeEngine::createQmlObject(): Component is not ready";

        return engine->nullValue();
    }

    QObject *obj = component.create(context);

    if(component.isError()) {
        QList<QDeclarativeError> errors = component.errors();
        qWarning().nospace() << "QDeclarativeEngine::createQmlObject():";
        foreach (const QDeclarativeError &error, errors)
            qWarning().nospace() << "    " << error;

        return engine->nullValue();
    }

    Q_ASSERT(obj);

    obj->setParent(parentArg);
    QGraphicsObject* gobj = qobject_cast<QGraphicsObject*>(obj);
    QGraphicsObject* gparent = qobject_cast<QGraphicsObject*>(parentArg);
    if(gobj && gparent)
        gobj->setParentItem(gparent);

    QDeclarativeDeclarativeData::get(obj, true)->setImplicitDestructible();
    return activeEnginePriv->objectClass->newQObject(obj, QMetaType::QObjectStar);
}

QScriptValue QDeclarativeEnginePrivate::vector(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 3)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    qsreal z = ctxt->argument(2).toNumber();
    return engine->newVariant(qVariantFromValue(QVector3D(x, y, z)));
}

QScriptValue QDeclarativeEnginePrivate::formatDate(QScriptContext*ctxt, QScriptEngine*engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount == 0 || argCount > 2)
        return engine->nullValue();

    QDate date = ctxt->argument(0).toDateTime().date();
    Qt::DateFormat enumFormat = Qt::DefaultLocaleShortDate;
    if (argCount == 2) {
        if (ctxt->argument(1).isString()) {
            QString format = ctxt->argument(1).toString();
            return engine->newVariant(qVariantFromValue(date.toString(format)));
        } else if (ctxt->argument(1).isNumber()) {
            enumFormat = Qt::DateFormat(ctxt->argument(1).toUInt32());
        } else
           return engine->nullValue();
    }
    return engine->newVariant(qVariantFromValue(date.toString(enumFormat)));
}

QScriptValue QDeclarativeEnginePrivate::formatTime(QScriptContext*ctxt, QScriptEngine*engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount == 0 || argCount > 2)
        return engine->nullValue();

    QTime date = ctxt->argument(0).toDateTime().time();
    Qt::DateFormat enumFormat = Qt::DefaultLocaleShortDate;
    if (argCount == 2) {
        if (ctxt->argument(1).isString()) {
            QString format = ctxt->argument(1).toString();
            return engine->newVariant(qVariantFromValue(date.toString(format)));
        } else if (ctxt->argument(1).isNumber()) {
            enumFormat = Qt::DateFormat(ctxt->argument(1).toUInt32());
        } else
           return engine->nullValue();
    }
    return engine->newVariant(qVariantFromValue(date.toString(enumFormat)));
}

QScriptValue QDeclarativeEnginePrivate::formatDateTime(QScriptContext*ctxt, QScriptEngine*engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount == 0 || argCount > 2)
        return engine->nullValue();

    QDateTime date = ctxt->argument(0).toDateTime();
    Qt::DateFormat enumFormat = Qt::DefaultLocaleShortDate;
    if (argCount == 2) {
        if (ctxt->argument(1).isString()) {
            QString format = ctxt->argument(1).toString();
            return engine->newVariant(qVariantFromValue(date.toString(format)));
        } else if (ctxt->argument(1).isNumber()) {
            enumFormat = Qt::DateFormat(ctxt->argument(1).toUInt32());
        } else
           return engine->nullValue();
    }
    return engine->newVariant(qVariantFromValue(date.toString(enumFormat)));
}

QScriptValue QDeclarativeEnginePrivate::rgba(QScriptContext *ctxt, QScriptEngine *engine)
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

QScriptValue QDeclarativeEnginePrivate::hsla(QScriptContext *ctxt, QScriptEngine *engine)
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

QScriptValue QDeclarativeEnginePrivate::rect(QScriptContext *ctxt, QScriptEngine *engine)
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

QScriptValue QDeclarativeEnginePrivate::point(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 2)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QPointF(x, y)));
}

QScriptValue QDeclarativeEnginePrivate::size(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 2)
        return engine->nullValue();
    qsreal w = ctxt->argument(0).toNumber();
    qsreal h = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QSizeF(w, h)));
}

QScriptValue QDeclarativeEnginePrivate::lighter(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.userType() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        color = QDeclarativeStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();
    color = color.lighter();
    return qScriptValueFromValue(engine, qVariantFromValue(color));
}

QScriptValue QDeclarativeEnginePrivate::darker(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() != 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.userType() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.userType() == QVariant::String) {
        bool ok;
        color = QDeclarativeStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();
    color = color.darker();
    return qScriptValueFromValue(engine, qVariantFromValue(color));
}

QScriptValue QDeclarativeEnginePrivate::desktopOpenUrl(QScriptContext *ctxt, QScriptEngine *e)
{
    if(ctxt->argumentCount() < 1)
        return e->newVariant(QVariant(false));
    bool ret = QDesktopServices::openUrl(QUrl(ctxt->argument(0).toString()));
    return e->newVariant(QVariant(ret));
}

QScriptValue QDeclarativeEnginePrivate::md5(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    QByteArray result = QCryptographicHash::hash(data, QCryptographicHash::Md5);

    return QScriptValue(QLatin1String(result.toHex()));
}

QScriptValue QDeclarativeEnginePrivate::btoa(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    return QScriptValue(QLatin1String(data.toBase64()));
}

QScriptValue QDeclarativeEnginePrivate::atob(QScriptContext *ctxt, QScriptEngine *)
{
    QByteArray data;

    if (ctxt->argumentCount() >= 1)
        data = ctxt->argument(0).toString().toUtf8();

    return QScriptValue(QLatin1String(QByteArray::fromBase64(data)));
}

QScriptValue QDeclarativeEnginePrivate::consoleLog(QScriptContext *ctxt, QScriptEngine *e)
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

void QDeclarativeEnginePrivate::sendQuit ()
{
    Q_Q(QDeclarativeEngine);
    emit q->quit();
}

QScriptValue QDeclarativeEnginePrivate::quit(QScriptContext * /*ctxt*/, QScriptEngine *e)
{
    QDeclarativeEnginePrivate *qe = get (e);
    qe->sendQuit ();
    return QScriptValue();
}

QScriptValue QDeclarativeEnginePrivate::closestAngle(QScriptContext *ctxt, QScriptEngine *e)
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

QScriptValue QDeclarativeEnginePrivate::tint(QScriptContext *ctxt, QScriptEngine *engine)
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
        color = QDeclarativeStringConverters::colorFromString(v.toString(), &ok);
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
        tintColor = QDeclarativeStringConverters::colorFromString(v.toString(), &ok);
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
        qreal a = tintColor.alphaF();
        qreal inv_a = 1.0 - a;

        finalColor.setRgbF(tintColor.redF() * a + color.redF() * inv_a,
                           tintColor.greenF() * a + color.greenF() * inv_a,
                           tintColor.blueF() * a + color.blueF() * inv_a,
                           a + inv_a * color.alphaF());
    }

    return qScriptValueFromValue(engine, qVariantFromValue(finalColor));
}


QScriptValue QDeclarativeEnginePrivate::scriptValueFromVariant(const QVariant &val)
{
    if (val.userType() == qMetaTypeId<QDeclarativeListReference>()) {
        QDeclarativeListReferencePrivate *p = QDeclarativeListReferencePrivate::get((QDeclarativeListReference*)val.constData());
        if (p->object) {
            return listClass->newList(p->property, p->propertyType);
        } else {
            return scriptEngine.nullValue();
        }
    }

    bool objOk;
    QObject *obj = QDeclarativeMetaType::toQObject(val, &objOk);
    if (objOk) {
        return objectClass->newQObject(obj);
    } else {
        return qScriptValueFromValue(&scriptEngine, val);
    }
}

QVariant QDeclarativeEnginePrivate::scriptValueToVariant(const QScriptValue &val)
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

QDeclarativeScriptClass::QDeclarativeScriptClass(QScriptEngine *engine)
: QScriptDeclarativeClass(engine)
{
}

QVariant QDeclarativeScriptClass::toVariant(QDeclarativeEngine *engine, const QScriptValue &val)
{
    QDeclarativeEnginePrivate *ep =
        static_cast<QDeclarativeEnginePrivate *>(QObjectPrivate::get(engine));

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
struct QDeclarativeEnginePrivate::ImportedNamespace {
    QStringList uris;
    QStringList urls;
    QList<int> majversions;
    QList<int> minversions;
    QList<bool> isLibrary;
    QList<QString> qmlDirContent;

    bool find(const QByteArray& type, int *vmajor, int *vminor, QDeclarativeType** type_return, QUrl* url_return) const
    {
        for (int i=0; i<urls.count(); ++i) {
            int vmaj = majversions.at(i);
            int vmin = minversions.at(i);

            QByteArray qt = uris.at(i).toUtf8();
            qt += '/';
            qt += type;

            if (qmlImportTrace())
                qDebug() << "Look in" << qt;
            QDeclarativeType *t = QDeclarativeMetaType::qmlType(qt,vmaj,vmin);
            if (t) {
                if (vmajor) *vmajor = vmaj;
                if (vminor) *vminor = vmin;
                if (qmlImportTrace())
                    qDebug() << "Found" << qt;
                if (type_return)
                    *type_return = t;
                return true;
            }

            QUrl url = QUrl(urls.at(i) + QLatin1Char('/') + QString::fromUtf8(type) + QLatin1String(".qml"));
            QString qmldircontent = qmlDirContent.at(i);
            if (vmaj>=0 || !qmldircontent.isEmpty()) {
                // Check version file - XXX cache these in QDeclarativeEngine!
                if (qmldircontent.isEmpty()) {
                    QFile qmldir(toLocalFileOrQrc(QUrl(urls.at(i)+QLatin1String("/qmldir"))));
                    if (qmldir.open(QIODevice::ReadOnly)) {
                        qmldircontent = QString::fromUtf8(qmldir.readAll());
                    }
                }

                const QString typeName = QString::fromUtf8(type);

                QDeclarativeDirParser qmldirParser;
                qmldirParser.setUrl(url);
                qmldirParser.setSource(qmldircontent);
                qmldirParser.parse();

                foreach (const QDeclarativeDirParser::Component &c, qmldirParser.components()) { // ### TODO: cache the components
                    if (c.majorVersion < vmaj || (c.majorVersion == vmaj && vmin >= c.minorVersion)) {
                        if (c.typeName == typeName) {
                            if (url_return)
                                *url_return = url.resolved(QUrl(c.fileName));

                            return true;
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

class QDeclarativeImportsPrivate {
public:
    QDeclarativeImportsPrivate() : ref(1)
    {
    }

    ~QDeclarativeImportsPrivate()
    {
        foreach (QDeclarativeEnginePrivate::ImportedNamespace* s, set.values())
            delete s;
    }

    QSet<QString> qmlDirFilesForWhichPluginsHaveBeenLoaded;

    bool add(const QUrl& base, const QString& qmldircontent, const QString& uri, const QString& prefix, int vmaj, int vmin, QDeclarativeScriptParser::Import::Type importType, const QStringList& importPath, QDeclarativeEngine *engine)
    {
        QDeclarativeEnginePrivate::ImportedNamespace *s;
        if (prefix.isEmpty()) {
            s = &unqualifiedset;
        } else {
            s = set.value(prefix);
            if (!s)
                set.insert(prefix,(s=new QDeclarativeEnginePrivate::ImportedNamespace));
        }
        QString url = uri;
        if (importType == QDeclarativeScriptParser::Import::Library) {
            url.replace(QLatin1Char('.'), QLatin1Char('/'));
            bool found = false;
            QString content;
            QString dir;            

            // user import paths
            QStringList paths;

            // base..
            paths += QFileInfo(base.toLocalFile()).path();
            paths += importPath;
            paths += QDeclarativeEnginePrivate::get(engine)->environmentImportPath;
#if (QT_VERSION >= QT_VERSION_CHECK(4,7,0))
            QString builtinPath = QLibraryInfo::location(QLibraryInfo::ImportsPath);
#else
            QString builtinPath;
#endif
            if (!builtinPath.isEmpty())
                paths += builtinPath;

            foreach (const QString &p, paths) {
                dir = p+QLatin1Char('/')+url;
                QFileInfo fi(dir+QLatin1String("/qmldir"));
                const QString absoluteFilePath = fi.absoluteFilePath();

                if (fi.isFile()) {
                    found = true;

                    url = QUrl::fromLocalFile(fi.absolutePath()).toString();

                    QFile file(absoluteFilePath);
                    if (file.open(QFile::ReadOnly))
                        content = QString::fromUtf8(file.readAll());

                    if (! qmlDirFilesForWhichPluginsHaveBeenLoaded.contains(absoluteFilePath)) {
                        qmlDirFilesForWhichPluginsHaveBeenLoaded.insert(absoluteFilePath);

                        QDeclarativeDirParser qmldirParser;
                        qmldirParser.setSource(content);
                        qmldirParser.parse();

                        foreach (const QDeclarativeDirParser::Plugin &plugin, qmldirParser.plugins()) {
                            QString resolvedFilePath = QDeclarativeEnginePrivate::get(engine)->resolvePlugin(QDir(dir + QDir::separator() + plugin.path),
                                                                                                    plugin.name);

                            if (!resolvedFilePath.isEmpty())
                                engine->importExtension(resolvedFilePath, uri);
                        }
                    }

                    break;
                }
            }

        } else {
            url = base.resolved(QUrl(url)).toString();
        }

        s->uris.prepend(uri);
        s->urls.prepend(url);
        s->majversions.prepend(vmaj);
        s->minversions.prepend(vmin);
        s->isLibrary.prepend(importType == QDeclarativeScriptParser::Import::Library);
        s->qmlDirContent.prepend(qmldircontent);
        return true;
    }

    bool find(const QByteArray& type, int *vmajor, int *vminor, QDeclarativeType** type_return, QUrl* url_return)
    {
        QDeclarativeEnginePrivate::ImportedNamespace *s = 0;
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
            if (s->urls.count() == 1 && !s->isLibrary[0] && url_return) {
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

    QDeclarativeEnginePrivate::ImportedNamespace *findNamespace(const QString& type)
    {
        return set.value(type);
    }

    QUrl base;
    int ref;

private:
    friend struct QDeclarativeEnginePrivate::Imports;
    QDeclarativeEnginePrivate::ImportedNamespace unqualifiedset;
    QHash<QString,QDeclarativeEnginePrivate::ImportedNamespace* > set;
};

QDeclarativeEnginePrivate::Imports::Imports(const Imports &copy) :
    d(copy.d)
{
    ++d->ref;
}

QDeclarativeEnginePrivate::Imports &QDeclarativeEnginePrivate::Imports::operator =(const Imports &copy)
{
    ++copy.d->ref;
    if (--d->ref == 0)
        delete d;
    d = copy.d;
    return *this;
}

QDeclarativeEnginePrivate::Imports::Imports() :
    d(new QDeclarativeImportsPrivate)
{
}

QDeclarativeEnginePrivate::Imports::~Imports()
{
    if (--d->ref == 0)
        delete d;
}

static QDeclarativeTypeNameCache *cacheForNamespace(QDeclarativeEngine *engine, const QDeclarativeEnginePrivate::ImportedNamespace &set, QDeclarativeTypeNameCache *cache)
{
    if (!cache)
        cache = new QDeclarativeTypeNameCache(engine);

    QList<QDeclarativeType *> types = QDeclarativeMetaType::qmlTypes();

    for (int ii = 0; ii < set.uris.count(); ++ii) {
        QByteArray base = set.uris.at(ii).toUtf8() + '/';
        int major = set.majversions.at(ii);
        int minor = set.minversions.at(ii);

        foreach (QDeclarativeType *type, types) {
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

QDeclarativeTypeNameCache *QDeclarativeEnginePrivate::Imports::cache(QDeclarativeEngine *engine) const
{
    const QDeclarativeEnginePrivate::ImportedNamespace &set = d->unqualifiedset;

    QDeclarativeTypeNameCache *cache = new QDeclarativeTypeNameCache(engine);

    for (QHash<QString,QDeclarativeEnginePrivate::ImportedNamespace* >::ConstIterator iter = d->set.begin();
         iter != d->set.end(); ++iter) {

        QDeclarativeTypeNameCache::Data *d = cache->data(iter.key());
        if (d) {
            if (!d->typeNamespace)
                cacheForNamespace(engine, *(*iter), d->typeNamespace);
        } else {
            QDeclarativeTypeNameCache *nc = cacheForNamespace(engine, *(*iter), 0);
            cache->add(iter.key(), nc);
            nc->release();
        }
    }

    cacheForNamespace(engine, set, cache);

    return cache;
}

/*
QStringList QDeclarativeEnginePrivate::Imports::unqualifiedSet() const
{
    QStringList rv;

    const QDeclarativeEnginePrivate::ImportedNamespace &set = d->unqualifiedset;

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
void QDeclarativeEnginePrivate::Imports::setBaseUrl(const QUrl& url)
{
    d->base = url;
}

/*!
  Returns the base URL to be used for all relative file imports added.
*/
QUrl QDeclarativeEnginePrivate::Imports::baseUrl() const
{
    return d->base;
}

/*!
  Adds \a path as a directory where installed QML components are
  defined in a URL-based directory structure.

  For example, if you add \c /opt/MyApp/lib/imports and then load QML
  that imports \c com.mycompany.Feature, then QDeclarativeEngine will look
  in \c /opt/MyApp/lib/imports/com/mycompany/Feature/ for the components
  provided by that module. A \c qmldir file is required for definiting the
  type version mapping and possibly declarative extensions plugins.

  The engine searches in the base directory of the qml file, then
  the paths added via addImportPath(), then the paths specified in the
  \c QML_IMPORT_PATH environment variable, then the builtin \c ImportsPath from
  QLibraryInfo.

*/
void QDeclarativeEngine::addImportPath(const QString& path)
{
    if (qmlImportTrace())
        qDebug() << "QDeclarativeEngine::addImportPath" << path;
    Q_D(QDeclarativeEngine);
    d->fileImportPath.prepend(path);
}

/*!
  Imports the given \a extension into this QDeclarativeEngine.  Returns
  true if the extension was successfully imported.

  \sa QDeclarativeExtensionInterface
*/
bool QDeclarativeEngine::importExtension(const QString &fileName, const QString &uri)
{
    QFileInfo fileInfo(fileName);
    const QString absoluteFilePath = fileInfo.absoluteFilePath();
    QPluginLoader loader(absoluteFilePath);

    if (QDeclarativeExtensionInterface *iface = qobject_cast<QDeclarativeExtensionInterface *>(loader.instance())) {
        const QByteArray bytes = uri.toUtf8();
        const char *moduleId = bytes.constData();

        // ### this code should probably be protected with a mutex.
        if (! qmlEnginePluginsWithRegisteredTypes()->contains(absoluteFilePath)) {
            // types should only be registered once (they're global).

            qmlEnginePluginsWithRegisteredTypes()->insert(absoluteFilePath);
            iface->registerTypes(moduleId);
        }

        QDeclarativeEnginePrivate *d = QDeclarativeEnginePrivate::get(this);

        if (! d->initializedPlugins.contains(absoluteFilePath)) {
            // things on the engine (eg. adding new global objects) have to be done for every engine.

            // protect against double initialization
            d->initializedPlugins.insert(absoluteFilePath);
            iface->initializeEngine(this, moduleId);
        }

        return true;
    }

    return false;
}

/*!
  \property QDeclarativeEngine::offlineStoragePath
  \brief the directory for storing offline user data

  Returns the directory where SQL and other offline
  storage is placed.

  QDeclarativeWebView and the SQL databases created with openDatabase()
  are stored here.

  The default is QML/OfflineStorage in the platform-standard
  user application data directory.

  Note that the path may not currently exist on the filesystem, so
  callers wanting to \e create new files at this location should create
  it first - see QDir::mkpath().
*/
void QDeclarativeEngine::setOfflineStoragePath(const QString& dir)
{
    Q_D(QDeclarativeEngine);
    d->scriptEngine.offlineStoragePath = dir;
}

QString QDeclarativeEngine::offlineStoragePath() const
{
    Q_D(const QDeclarativeEngine);
    return d->scriptEngine.offlineStoragePath;
}

/*!
  \internal

  Returns the result of the merge of \a baseName with \a dir, \a suffixes, and \a prefix.
  The \a prefix must contain the dot.
 */
QString QDeclarativeEnginePrivate::resolvePlugin(const QDir &dir, const QString &baseName,
                                        const QStringList &suffixes,
                                        const QString &prefix)
{
    foreach (const QString &suffix, suffixes) {
        QString pluginFileName = prefix;

        pluginFileName += baseName;
        pluginFileName += suffix;

        QFileInfo fileInfo(dir, pluginFileName);

        if (fileInfo.exists())
            return fileInfo.absoluteFilePath();
    }

    return QString();
}

/*!
  \internal

  Returns the result of the merge of \a baseName with \a dir and the platform suffix.

  \table
  \header \i Platform \i Valid suffixes
  \row \i Windows     \i \c .dll
  \row \i Unix/Linux  \i \c .so
  \row \i AIX  \i \c .a
  \row \i HP-UX       \i \c .sl, \c .so (HP-UXi)
  \row \i Mac OS X    \i \c .dylib, \c .bundle, \c .so
  \row \i Symbian     \i \c .dll
  \endtable

  Version number on unix are ignored.
*/
QString QDeclarativeEnginePrivate::resolvePlugin(const QDir &dir, const QString &baseName)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WINCE)
    return resolvePlugin(dir, baseName,
                         QStringList()
# ifdef QT_DEBUG
                         << QLatin1String("d.dll") // try a qmake-style debug build first
# endif
                         << QLatin1String(".dll"));
#elif defined(Q_OS_SYMBIAN)
    return resolvePlugin(dir, baseName,
                         QStringList()
                         << QLatin1String(".dll")
                         << QLatin1String(".qtplugin"));
#else

# if defined(Q_OS_DARWIN)

    return resolvePlugin(dir, baseName,
                         QStringList()
                         << QLatin1String(".dylib")
                         << QLatin1String(".so")
                         << QLatin1String(".bundle"),
                         QLatin1String("lib"));
# else  // Generic Unix
    QStringList validSuffixList;

#  if defined(Q_OS_HPUX)
/*
    See "HP-UX Linker and Libraries User's Guide", section "Link-time Differences between PA-RISC and IPF":
    "In PA-RISC (PA-32 and PA-64) shared libraries are suffixed with .sl. In IPF (32-bit and 64-bit),
    the shared libraries are suffixed with .so. For compatibility, the IPF linker also supports the .sl suffix."
 */
    validSuffixList << QLatin1String(".sl");
#   if defined __ia64
    validSuffixList << QLatin1String(".so");
#   endif
#  elif defined(Q_OS_AIX)
    validSuffixList << QLatin1String(".a") << QLatin1String(".so");
#  elif defined(Q_OS_UNIX)
    validSuffixList << QLatin1String(".so");
#  endif

    // Examples of valid library names:
    //  libfoo.so

    return resolvePlugin(dir, baseName, validSuffixList, QLatin1String("lib"));
# endif

#endif
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
bool QDeclarativeEnginePrivate::addToImport(Imports* imports, const QString& qmldircontent, const QString& uri, const QString& prefix, int vmaj, int vmin, QDeclarativeScriptParser::Import::Type importType) const
{
    QDeclarativeEngine *engine = QDeclarativeEnginePrivate::get(const_cast<QDeclarativeEnginePrivate *>(this));
    bool ok = imports->d->add(imports->d->base,qmldircontent,uri,prefix,vmaj,vmin,importType,fileImportPath, engine);
    if (qmlImportTrace())
        qDebug() << "QDeclarativeEngine::addToImport(" << imports << uri << prefix << vmaj << '.' << vmin << (importType==QDeclarativeScriptParser::Import::Library? "Library" : "File") << ": " << ok;
    return ok;
}

/*!
  \internal

  Using the given \a imports, the given (namespace qualified) \a type is resolved to either
  an ImportedNamespace stored at \a ns_return,
  a QDeclarativeType stored at \a type_return, or
  a component located at \a url_return.

  If any return pointer is 0, the corresponding search is not done.

  \sa addToImport()
*/
bool QDeclarativeEnginePrivate::resolveType(const Imports& imports, const QByteArray& type, QDeclarativeType** type_return, QUrl* url_return, int *vmaj, int *vmin, ImportedNamespace** ns_return) const
{
    ImportedNamespace* ns = imports.d->findNamespace(QString::fromUtf8(type));
    if (ns) {
        if (qmlImportTrace())
            qDebug() << "QDeclarativeEngine::resolveType" << type << "is namespace for" << ns->urls;
        if (ns_return)
            *ns_return = ns;
        return true;
    }
    if (type_return || url_return) {
        if (imports.d->find(type,vmaj,vmin,type_return,url_return)) {
            if (qmlImportTrace()) {
                if (type_return && *type_return)
                    qDebug() << "QDeclarativeEngine::resolveType" << type << '=' << (*type_return)->typeName();
                if (url_return)
                    qDebug() << "QDeclarativeEngine::resolveType" << type << '=' << *url_return;
            }
            return true;
        }
        if (qmlImportTrace())
            qDebug() << "QDeclarativeEngine::resolveType" << type << "not found";
    }
    return false;
}

/*!
  \internal

  Searching \e only in the namespace \a ns (previously returned in a call to
  resolveType(), \a type is found and returned to either
  a QDeclarativeType stored at \a type_return, or
  a component located at \a url_return.

  If either return pointer is 0, the corresponding search is not done.
*/
void QDeclarativeEnginePrivate::resolveTypeInNamespace(ImportedNamespace* ns, const QByteArray& type, QDeclarativeType** type_return, QUrl* url_return, int *vmaj, int *vmin ) const
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

void QDeclarativeEnginePrivate::registerCompositeType(QDeclarativeCompiledData *data)
{
    QByteArray name = data->root->className();

    QByteArray ptr = name + '*';
    QByteArray lst = "QDeclarativeListProperty<" + name + ">";

    int ptr_type = QMetaType::registerType(ptr.constData(), voidptr_destructor,
                                           voidptr_constructor);
    int lst_type = QMetaType::registerType(lst.constData(), voidptr_destructor,
                                           voidptr_constructor);

    m_qmlLists.insert(lst_type, ptr_type);
    m_compositeTypes.insert(ptr_type, data);
    data->addref();
}

bool QDeclarativeEnginePrivate::isList(int t) const
{
    return m_qmlLists.contains(t) || QDeclarativeMetaType::isList(t);
}

int QDeclarativeEnginePrivate::listType(int t) const
{
    QHash<int, int>::ConstIterator iter = m_qmlLists.find(t);
    if (iter != m_qmlLists.end())
        return *iter;
    else
        return QDeclarativeMetaType::listType(t);
}

bool QDeclarativeEnginePrivate::isQObject(int t)
{
    return m_compositeTypes.contains(t) || QDeclarativeMetaType::isQObject(t);
}

QObject *QDeclarativeEnginePrivate::toQObject(const QVariant &v, bool *ok) const
{
    int t = v.userType();
    if (m_compositeTypes.contains(t)) {
        if (ok) *ok = true;
        return *(QObject **)(v.constData());
    } else {
        return QDeclarativeMetaType::toQObject(v, ok);
    }
}

QDeclarativeMetaType::TypeCategory QDeclarativeEnginePrivate::typeCategory(int t) const
{
    if (m_compositeTypes.contains(t))
        return QDeclarativeMetaType::Object;
    else if (m_qmlLists.contains(t))
        return QDeclarativeMetaType::List;
    else
        return QDeclarativeMetaType::typeCategory(t);
}

const QMetaObject *QDeclarativeEnginePrivate::rawMetaObjectForType(int t) const
{
    QHash<int, QDeclarativeCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        QDeclarativeType *type = QDeclarativeMetaType::qmlType(t);
        return type?type->baseMetaObject():0;
    }
}

const QMetaObject *QDeclarativeEnginePrivate::metaObjectForType(int t) const
{
    QHash<int, QDeclarativeCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        QDeclarativeType *type = QDeclarativeMetaType::qmlType(t);
        return type?type->metaObject():0;
    }
}

QT_END_NAMESPACE
