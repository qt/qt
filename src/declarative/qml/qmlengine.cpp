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
: object(p.object()), notifyIndex(p.property().notifySignalIndex())
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
            capturedProperties << CapturedProperty(context->q_ptr, *iter + context->notifyIndex);
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

    Currently, only the empty namespace is supported
    (i.e. types cannot be qualified with a namespace).

    The QML \c import statement can be used to import a directory of
    components into the empty namespace.

    \qml
    import "MyModuleDirectory"
    \endqml

    This is also possible from C++:

    \code
        engine->addNameSpacePath("","file:///opt/abcdef");
    \endcode

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
  Returns the list of base urls the engine browses to find sub-components.

  The search path consists of the base of the \a url, and, in the case of local files,
  the directories imported using the "import" statement in \a qml.
  */
QList<QUrl> QmlEngine::componentSearchPath(const QByteArray &qml, const QUrl &url) const
{
    QList<QUrl> searchPath;

    searchPath << url.resolved(QUrl(QLatin1String(".")));

    if (QFileInfo(url.toLocalFile()).exists()) {
        QmlScriptParser parser;
        if (parser.parse(qml, url)) {
            for (int i = 0; i < parser.imports().size(); ++i) {
                QUrl importUrl = QUrl(parser.imports().at(i).uri);
                if (importUrl.isRelative()) {
                    searchPath << url.resolved(importUrl);
                } else {
                    searchPath << importUrl;
                }
            }
        }
    }

    return searchPath;
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

QObject *qmlAttachedPropertiesObjectById(int id, const QObject *object)
{
    QmlExtendedDeclarativeData *edata = 
        QmlExtendedDeclarativeData::get(const_cast<QObject *>(object), true);

    QObject *rv = edata->attachedProperties.value(id);
    if (rv)
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
    if(qmlCtxt)
        qmlCtxt->activate();
    QmlComponent component(activeEngine, qml.toUtf8(), url);
    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        foreach (const QmlError &error, errors) {
            qWarning() << error;
        }

        if(qmlCtxt)
            qmlCtxt->deactivate();
        return engine->nullValue();
    }

    QObject *obj = component.create();
    if(qmlCtxt)
        qmlCtxt->deactivate();
    if(component.isError()) {
        QList<QmlError> errors = component.errors();
        foreach (const QmlError &error, errors) {
            qWarning() << error;
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

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b)
: q(b), ctxt(0), sseData(0), proxy(0), me(0), trackChange(false), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, void *expr, QmlRefCount *rc)
: q(b), ctxt(0), sse((const char *)expr, rc), sseData(0), proxy(0), me(0), trackChange(true), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::QmlExpressionPrivate(QmlExpression *b, const QString &expr)
: q(b), ctxt(0), expression(expr), sseData(0), proxy(0), me(0), trackChange(true), line(-1), id(0), log(0)
{
}

QmlExpressionPrivate::~QmlExpressionPrivate()
{
    sse.deleteScriptState(sseData);
    sseData = 0;
    delete proxy;
    delete log;
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
    if(ctxt && ctxt->engine())
        d->id = ctxt->engine()->d_func()->getUniqueId();
    if(ctxt)
        ctxt->d_func()->childExpressions.insert(this);
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
: d(new QmlExpressionPrivate(this, expression))
{
    d->ctxt = ctxt;
    if(ctxt && ctxt->engine())
        d->id = ctxt->engine()->d_func()->getUniqueId();
    if(ctxt)
        ctxt->d_func()->childExpressions.insert(this);
    d->me = scope;
}

/*!
    Destroy the QmlExpression instance.
*/
QmlExpression::~QmlExpression()
{
    if (d->ctxt)
        d->ctxt->d_func()->childExpressions.remove(this);
    delete d; d = 0;
}

/*!
    Returns the QmlEngine this expression is associated with, or 0 if there
    is no association or the QmlEngine has been destroyed.
*/
QmlEngine *QmlExpression::engine() const
{
    return d->ctxt?d->ctxt->engine():0;
}

/*!
    Returns the QmlContext this expression is associated with, or 0 if there
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
    if (d->sse.isValid())
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
    if (d->sseData) {
        d->sse.deleteScriptState(d->sseData);
        d->sseData = 0;
    }

    delete d->proxy; d->proxy = 0;

    d->expression = expression;

    d->sse.clear();
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
    QVariant rv;
    if (!d->ctxt || !engine() || (!d->sse.isValid() && d->expression.isEmpty()))
        return rv;

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindValue> perf;
#endif

    QmlBasicScript::CacheState cacheState = QmlBasicScript::Reset;

    QmlEnginePrivate *ep = engine()->d_func();
    QmlExpression *lastCurrentExpression = ep->currentExpression;
    ep->currentExpression = this;
    if (d->sse.isValid()) {
#ifdef Q_ENABLE_PERFORMANCE_LOG
        QFxPerfTimer<QFxPerf::BindValueSSE> perfsse;
#endif

        context()->d_func()->defaultObjects.insert(context()->d_func()->highPriorityCount, d->me);

        if (!d->sseData)
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
        QScriptValue svalue = scriptEngine->evaluate(expression(), d->fileName.toString(), d->line);
        if (scriptEngine->hasUncaughtException()) {
            if (scriptEngine->uncaughtException().isError()){
                QScriptValue exception = scriptEngine->uncaughtException();
                if (!exception.property(QLatin1String("fileName")).toString().isEmpty()){
                    qWarning() << exception.property(QLatin1String("fileName")).toString()
                            << scriptEngine->uncaughtExceptionLineNumber()
                            << exception.toString();

                } else {
                    qWarning() << exception.toString();
                }
            }
        }

        context()->d_func()->defaultObjects.removeAt(context()->d_func()->highPriorityCount);
        if (svalue.isArray()) {
            int length = svalue.property(QLatin1String("length")).toInt32();
            if (length && svalue.property(0).isObject()) {
                QList<QObject *> list;
                for (int ii = 0; ii < length; ++ii) {
                    QScriptValue arrayItem = svalue.property(ii);
                    QObject *d = qvariant_cast<QObject *>(arrayItem.data().toVariant());
                    if (d) {
                        list << d;
                    } else {
                        list << 0;
                    }
                }
                rv = QVariant::fromValue(list);
            }
        } else if (svalue.isObject() && 
                   !svalue.isNumber() &&
                   !svalue.isString() &&
                   !svalue.isDate() && 
                   !svalue.isError() && 
                   !svalue.isFunction() &&
                   !svalue.isNull() &&
                   !svalue.isQMetaObject() &&
                   !svalue.isQObject() &&
                   !svalue.isRegExp()) {
            QScriptValue objValue = svalue.data();
            if (objValue.isValid()) {
                QVariant var = objValue.toVariant();
                if (var.userType() >= (int)QVariant::UserType &&
                    QmlMetaType::isObject(var.userType()))
                    rv = var;
            }
        }
        if (rv.isNull()) 
            rv = svalue.toVariant();

        for (int i = 0; i < context()->d_func()->scopeChain.size(); ++i) {
            scriptEngine->currentContext()->popScope();
        }
        for (int i = oldScopeChain.size() - 1; i > -1; --i) {
            scriptEngine->currentContext()->pushScope(oldScopeChain.at(i));
        }
    }
    ep->currentExpression = lastCurrentExpression;

    if (cacheState != QmlBasicScript::NoChange) {
        if (cacheState != QmlBasicScript::Incremental && d->proxy) {
            delete d->proxy;
            d->proxy = 0;
        }

        if (trackChange() && ep->capturedProperties.count()) {
            if (!d->proxy)
                d->proxy = new BindExpressionProxy(this);

            static int changedIndex = -1;
            if (changedIndex == -1)
                changedIndex = BindExpressionProxy::staticMetaObject.indexOfSlot("changed()");

            if(qmlDebugger()) {
                QmlExpressionLog log;
                log.setTime(engine()->d_func()->getUniqueId());
                log.setExpression(expression());
                log.setResult(rv);

                for (int ii = 0; ii < ep->capturedProperties.count(); ++ii) {
                    const QmlEnginePrivate::CapturedProperty &prop =
                        ep->capturedProperties.at(ii);

                    if (prop.notifyIndex != -1) {
                        QMetaObject::connect(prop.object, prop.notifyIndex,
                                             d->proxy, changedIndex);
                    } else {
                        // ### FIXME
                        //QString warn = QLatin1String("Expression depends on property without a NOTIFY signal: [") + QLatin1String(prop.object->metaObject()->className()) + QLatin1String("].") + prop.name;
                        //log.addWarning(warn);
                    }
                }
                d->addLog(log);

            } else {
                for (int ii = 0; ii < ep->capturedProperties.count(); ++ii) {
                    const QmlEnginePrivate::CapturedProperty &prop =
                        ep->capturedProperties.at(ii);

                    if (prop.notifyIndex != -1) 
                        QMetaObject::connect(prop.object, prop.notifyIndex,
                                             d->proxy, changedIndex);
                }
            }
        } else {
            QmlExpressionLog log;
            log.setTime(engine()->d_func()->getUniqueId());
            log.setExpression(expression());
            log.setResult(rv);
            d->addLog(log);
        }

    } else {
        if(qmlDebugger()) {
            QmlExpressionLog log;
            log.setTime(engine()->d_func()->getUniqueId());
            log.setExpression(expression());
            log.setResult(rv);
            d->addLog(log);
        }
    }

    ep->capturedProperties.clear();

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
    Set the location of this expression to \a line of \a fileName. This information
    is used by the script engine.
*/
void QmlExpression::setSourceLocation(const QUrl &fileName, int line)
{
    d->fileName = fileName;
    d->line = line;
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
    \internal
*/
quint32 QmlExpression::id() const
{
    return d->id;
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
: QObject(parent), QmlExpression(ctxt, expression, scope)
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
        engine->d_func()->capturedProperties << QmlEnginePrivate::CapturedProperty(bindContext, index + bindContext->d_func()->notifyIndex);
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

void QmlExpressionPrivate::addLog(const QmlExpressionLog &l)
{
    if (!log)
        log = new QList<QmlExpressionLog>();
    log->append(l);
}

QmlExpressionLog::QmlExpressionLog()
{
}

QmlExpressionLog::QmlExpressionLog(const QmlExpressionLog &o)
: m_time(o.m_time),
  m_expression(o.m_expression),
  m_result(o.m_result),
  m_warnings(o.m_warnings)
{
}

QmlExpressionLog::~QmlExpressionLog()
{
}

QmlExpressionLog &QmlExpressionLog::operator=(const QmlExpressionLog &o)
{
    m_time = o.m_time;
    m_expression = o.m_expression;
    m_result = o.m_result;
    m_warnings = o.m_warnings;
    return *this;
}

void QmlExpressionLog::setTime(quint32 time)
{
    m_time = time;
}

quint32 QmlExpressionLog::time() const
{
    return m_time;
}

QString QmlExpressionLog::expression() const
{
    return m_expression;
}

void QmlExpressionLog::setExpression(const QString &e)
{
    m_expression = e;
}

QStringList QmlExpressionLog::warnings() const
{
    return m_warnings;
}

void QmlExpressionLog::addWarning(const QString &w)
{
    m_warnings << w;
}

QVariant QmlExpressionLog::result() const
{
    return m_result;
}

void QmlExpressionLog::setResult(const QVariant &r)
{
    m_result = r;
}

QT_END_NAMESPACE
