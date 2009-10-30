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

#include <QtCore/qmetaobject.h>
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>
#include <private/qobject_p.h>
#include <private/qmlcompiler_p.h>
#include <private/qscriptdeclarativeclass_p.h>
#include <private/qmlglobalscriptclass_p.h>

#ifdef QT_SCRIPTTOOLS_LIB
#include <QScriptEngineDebugger>
#endif

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
#include "qml.h"
#include <private/qfxperf_p_p.h>
#include <QStack>
#include "private/qmlbasicscript_p.h"
#include "qmlengine.h"
#include "qmlcontext.h"
#include "qmlexpression.h"
#include <QtCore/qthreadstorage.h>
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>
#include <QtGui/qcolor.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qsound.h>
#include <QGraphicsObject>
#include <qmlcomponent.h>
#include <private/qmlmetaproperty_p.h>
#include <private/qmlbinding_p.h>
#include <private/qmlvme_p.h>
#include <private/qmlenginedebug_p.h>
#include <private/qmlstringconverters_p.h>
#include <private/qmlxmlhttprequest_p.h>
#include <private/qmlsqldatabase_p.h>
#include <private/qmltypenamescriptclass_p.h>
#include <private/qmllistscriptclass_p.h>
#include <QtDeclarative/qmlscriptstring.h>

#ifdef Q_OS_WIN // for %APPDATA%
#include "qt_windows.h"
#include "qlibrary.h"
#define CSIDL_APPDATA		0x001a	// <username>\Application Data
#endif

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

static QString userLocalDataPath(const QString& app)
{
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + QLatin1String("/") + app;
}

QmlEnginePrivate::QmlEnginePrivate(QmlEngine *e)
: rootContext(0), currentExpression(0),
  isDebugging(false), contextClass(0), objectClass(0), valueTypeClass(0), globalClass(0),
  nodeListClass(0), namedNodeMapClass(0), sqlQueryClass(0), cleanup(0), erroredBindings(0), 
  inProgressCreations(0), scriptEngine(this), componentAttacheds(0), rootComponent(0), 
  networkAccessManager(0), typeManager(e), uniqueId(1)
{
    // Note that all documentation for stuff put on the global object goes in
    // doc/src/declarative/globalobject.qdoc
    QScriptValue qtObject =
        scriptEngine.newQMetaObject(StaticQtMetaObject::get());
    scriptEngine.globalObject().setProperty(QLatin1String("Qt"), qtObject);

    offlineStoragePath = userLocalDataPath(QLatin1String("QML/OfflineStorage"));
    qt_add_qmlxmlhttprequest(&scriptEngine);
    qt_add_qmlsqldatabase(&scriptEngine);

    //types
    qtObject.setProperty(QLatin1String("rgba"), scriptEngine.newFunction(QmlEnginePrivate::rgba, 4));
    qtObject.setProperty(QLatin1String("hsla"), scriptEngine.newFunction(QmlEnginePrivate::hsla, 4));
    qtObject.setProperty(QLatin1String("rect"), scriptEngine.newFunction(QmlEnginePrivate::rect, 4));
    qtObject.setProperty(QLatin1String("point"), scriptEngine.newFunction(QmlEnginePrivate::point, 2));
    qtObject.setProperty(QLatin1String("size"), scriptEngine.newFunction(QmlEnginePrivate::size, 2));
    qtObject.setProperty(QLatin1String("vector3d"), scriptEngine.newFunction(QmlEnginePrivate::vector, 3));

    //color helpers
    qtObject.setProperty(QLatin1String("lighter"), scriptEngine.newFunction(QmlEnginePrivate::lighter, 1));
    qtObject.setProperty(QLatin1String("darker"), scriptEngine.newFunction(QmlEnginePrivate::darker, 1));
    qtObject.setProperty(QLatin1String("tint"), scriptEngine.newFunction(QmlEnginePrivate::tint, 2));

    //misc methods
    qtObject.setProperty(QLatin1String("playSound"), scriptEngine.newFunction(QmlEnginePrivate::playSound, 1));
    qtObject.setProperty(QLatin1String("openUrlExternally"),scriptEngine.newFunction(desktopOpenUrl, 1));

    scriptEngine.globalObject().setProperty(QLatin1String("createQmlObject"),
            scriptEngine.newFunction(QmlEnginePrivate::createQmlObject, 1));
    scriptEngine.globalObject().setProperty(QLatin1String("createComponent"),
            scriptEngine.newFunction(QmlEnginePrivate::createComponent, 1));

    globalClass = new QmlGlobalScriptClass(&scriptEngine);
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
    delete networkAccessManager;
    networkAccessManager = 0;
    delete nodeListClass;
    nodeListClass = 0;
    delete namedNodeMapClass;
    namedNodeMapClass = 0;
    delete sqlQueryClass;
    sqlQueryClass = 0;

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

    scriptEngine.installTranslatorFunctions();
    contextClass = new QmlContextScriptClass(q);
    objectClass = new QmlObjectScriptClass(q);
    valueTypeClass = new QmlValueTypeScriptClass(q);
    typeNameClass = new QmlTypeNameScriptClass(q);
    listClass = new QmlListScriptClass(q);
    rootContext = new QmlContext(q,true);
#ifdef QT_SCRIPTTOOLS_LIB
    if (qmlDebugger()){
        debugger = new QScriptEngineDebugger(q);
        debugger->attachTo(&scriptEngine);
    }
#endif

    if (QCoreApplication::instance()->thread() == q->thread() &&
        QmlEngineDebugServer::isDebuggingEnabled()) {
        qmlEngineDebugServer();
        isDebugging = true;
        QmlEngineDebugServer::addEngine(q);

        qmlEngineDebugServer()->waitForClients();
    }
}

QmlEnginePrivate::CapturedProperty::CapturedProperty(const QmlMetaProperty &p)
: object(p.object()), coreIndex(p.coreIndex()), notifyIndex(p.property().notifySignalIndex())
{
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
    QObjectPrivate *priv = QObjectPrivate::get(object);

    QmlDeclarativeData *data =
        static_cast<QmlDeclarativeData *>(priv->declarativeData);

    if (data && data->context) {
        qWarning("QmlEngine::setContextForObject(): Object already has a QmlContext");
        return;
    }

    if (!data) {
        priv->declarativeData = new QmlDeclarativeData(context);
    } else {
        data->context = context;
    }

    context->d_func()->contextObjects.append(object);
}

void qmlExecuteDeferred(QObject *object)
{
    QmlDeclarativeData *data = QmlDeclarativeData::get(object);

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

QmlDeclarativeData::QmlDeclarativeData(QmlContext *ctxt)
: context(ctxt), bindings(0), bindingBitsSize(0), bindingBits(0), 
  outerContext(0), lineNumber(0), columnNumber(0), deferredComponent(0), 
  deferredIdx(0), attachedProperties(0), propertyCache(0)
{
}

void QmlDeclarativeData::destroyed(QObject *object)
{
    if (deferredComponent)
        deferredComponent->release();
    if (attachedProperties)
        delete attachedProperties;
    if (context)
        static_cast<QmlContextPrivate *>(QObjectPrivate::get(context))->contextObjects.removeAll(object);

    QmlAbstractBinding *binding = bindings;
    while (binding) {
        QmlAbstractBinding *next = binding->m_nextBinding;
        binding->m_prevBinding = 0;
        binding->m_nextBinding = 0;
        delete binding;
        binding = next;
    }

    if (bindingBits)
        free(bindingBits);

    if (propertyCache)
        propertyCache->release();

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
    QmlComponent* c;

    QmlEnginePrivate *activeEnginePriv =
        static_cast<QmlScriptEngine*>(engine)->p;
    QmlEngine* activeEngine = activeEnginePriv->q_func();

    QmlContext* context = activeEnginePriv->getContext(ctxt);
    if(ctxt->argumentCount() != 1) {
        c = new QmlComponent(activeEngine);
    }else{
        QUrl url = QUrl(context->resolvedUrl(ctxt->argument(0).toString()));
        if(!url.isValid())
            url = QUrl(ctxt->argument(0).toString());
        c = new QmlComponent(activeEngine, url, activeEngine);
    }
    c->setCreationContext(context);
    return engine->newQObject(c);
}

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
    QObject *parentArg = activeEnginePriv->objectClass->toQObject(ctxt->argument(1));
    QmlContext *qmlCtxt = qmlContext(parentArg);
    if (url.isEmpty()) {
        url = qmlCtxt->resolvedUrl(QUrl(QLatin1String("<Unknown File>")));
    } else {
        url = qmlCtxt->resolvedUrl(url);
    }

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
        QGraphicsObject* gobj = qobject_cast<QGraphicsObject*>(obj);
        QGraphicsObject* gparent = qobject_cast<QGraphicsObject*>(parentArg);
        if(gobj && gparent)
            gobj->setParentItem(gparent);
        return qmlScriptObject(obj, activeEngine);
    }
    return engine->nullValue();
}

QScriptValue QmlEnginePrivate::vector(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 3)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    qsreal z = ctxt->argument(2).toNumber();
    return engine->newVariant(qVariantFromValue(QVector3D(x, y, z)));
}

QScriptValue QmlEnginePrivate::rgba(QScriptContext *ctxt, QScriptEngine *engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount < 3)
        return engine->nullValue();
    qsreal r = ctxt->argument(0).toNumber();
    qsreal g = ctxt->argument(1).toNumber();
    qsreal b = ctxt->argument(2).toNumber();
    qsreal a = (argCount == 4) ? ctxt->argument(3).toNumber() : 1;
    return qScriptValueFromValue(engine, qVariantFromValue(QColor::fromRgbF(r, g, b, a)));
}

QScriptValue QmlEnginePrivate::hsla(QScriptContext *ctxt, QScriptEngine *engine)
{
    int argCount = ctxt->argumentCount();
    if(argCount < 3)
        return engine->nullValue();
    qsreal h = ctxt->argument(0).toNumber();
    qsreal s = ctxt->argument(1).toNumber();
    qsreal l = ctxt->argument(2).toNumber();
    qsreal a = (argCount == 4) ? ctxt->argument(3).toNumber() : 1;
    return qScriptValueFromValue(engine, qVariantFromValue(QColor::fromHslF(h, s, l, a)));
}

QScriptValue QmlEnginePrivate::rect(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 4)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    qsreal w = ctxt->argument(2).toNumber();
    qsreal h = ctxt->argument(3).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QRectF(x, y, w, h)));
}

QScriptValue QmlEnginePrivate::point(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 2)
        return engine->nullValue();
    qsreal x = ctxt->argument(0).toNumber();
    qsreal y = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QPointF(x, y)));
}

QScriptValue QmlEnginePrivate::size(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 2)
        return engine->nullValue();
    qsreal w = ctxt->argument(0).toNumber();
    qsreal h = ctxt->argument(1).toNumber();
    return qScriptValueFromValue(engine, qVariantFromValue(QSizeF(w, h)));
}

QScriptValue QmlEnginePrivate::lighter(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.type() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.type() == QVariant::String) {
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
    if(ctxt->argumentCount() < 1)
        return engine->nullValue();
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.type() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.type() == QVariant::String) {
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
    if (ctxt->argumentCount() < 1)
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

QScriptValue QmlEnginePrivate::tint(QScriptContext *ctxt, QScriptEngine *engine)
{
    if(ctxt->argumentCount() < 2)
        return engine->nullValue();
    //get color
    QVariant v = ctxt->argument(0).toVariant();
    QColor color;
    if (v.type() == QVariant::Color)
        color = v.value<QColor>();
    else if (v.type() == QVariant::String) {
        bool ok;
        color = QmlStringConverters::colorFromString(v.toString(), &ok);
        if (!ok)
            return engine->nullValue();
    } else
        return engine->nullValue();

    //get tint color
    v = ctxt->argument(1).toVariant();
    QColor tintColor;
    if (v.type() == QVariant::Color)
        tintColor = v.value<QColor>();
    else if (v.type() == QVariant::String) {
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
    if (QmlMetaType::isObject(val.userType())) {
        QObject *rv = *(QObject **)val.constData();
        return objectClass->newQObject(rv);
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

    QScriptClass *sc = val.scriptClass();
    if (!sc) {
        return val.toVariant();
    } else if (sc == valueTypeClass) {
        return valueTypeClass->toVariant(val);
    } else {
        return QVariant();
    }
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

    QScriptDeclarativeClass *dc = QScriptDeclarativeClass::scriptClass(val);
    if (dc == ep->objectClass)
        return QVariant::fromValue(ep->objectClass->toQObject(val));
    else if (dc == ep->contextClass)
        return QVariant();

    QScriptClass *sc = val.scriptClass();
    if (!sc) {
        return val.toVariant();
    } else if (sc == ep->valueTypeClass) {
        return ep->valueTypeClass->toVariant(val);
    }

    return QVariant();
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
    QStringList urls;
    QList<int> majversions;
    QList<int> minversions;
    QList<bool> isLibrary;
    QList<bool> isBuiltin;
    QList<QString> qmlDirContent;

    bool find(const QByteArray& type, int *vmajor, int *vminor, QmlType** type_return, QUrl* url_return) const
    {
        for (int i=0; i<urls.count(); ++i) {
            int vmaj = majversions.at(i);
            int vmin = minversions.at(i);

            if (isBuiltin.at(i)) {
                QByteArray qt = urls.at(i).toUtf8();
                qt += "/";
                qt += type;
                QmlType *t = QmlMetaType::qmlType(qt,vmaj,vmin);
                if (vmajor) *vmajor = vmaj;
                if (vminor) *vminor = vmin;
                if (t) {
                    if (type_return)
                        *type_return = t;
                    return true;
                }
            } else {
                QUrl url = QUrl(urls.at(i) + QLatin1String("/") + QString::fromUtf8(type) + QLatin1String(".qml"));
                QString qmldircontent = qmlDirContent.at(i);
                if (vmaj || vmin || !qmldircontent.isEmpty()) {
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
                            // eg. 1.2-5
                            int space1 = line.indexOf(QLatin1Char(' '));
                            int space2 = space1 >=0 ? line.indexOf(QLatin1Char(' '),space1+1) : -1;
                            QString mapversions = line.mid(space1+1,space2<0?line.length()-space1-1:space2-space1-1);
                            int dot = mapversions.indexOf(QLatin1Char('.'));
                            int dash = mapversions.indexOf(QLatin1Char('-'));
                            int mapvmaj = mapversions.left(dot).toInt();
                            if (mapvmaj==vmaj) {
                                int mapvmin_from = (dash <= 0 ? mapversions.mid(dot+1) : mapversions.mid(dot+1,dash-dot-1)).toInt();
                                int mapvmin_to = dash <= 0 ? mapvmin_from : mapversions.mid(dash+1).toInt();
                                if (vmin >= mapvmin_from && vmin <= mapvmin_to) {
                                    QStringRef mapfile = space2<0 ? QStringRef() : line.midRef(space2+1,line.length()-space2-1);
                                    if (url_return)
                                        *url_return = url.resolved(mapfile.toString());
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
        }
        return false;
    }
};

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
        } else {
            url = base.resolved(QUrl(url)).toString();
        }
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
            while (!s) {
                s = set.value(QString::fromUtf8(type.left(slash)));
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
            if (s->find(unqualifiedtype,vmajor,vminor,type_return,url_return))
                return true;
            if (s->urls.count() == 1 && !s->isBuiltin[0] && !s->isLibrary[0] && url_return) {
                *url_return = QUrl(s->urls[0]+QLatin1String("/")).resolved(QUrl(QString::fromUtf8(unqualifiedtype) + QLatin1String(".qml")));
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
    friend class QmlEnginePrivate::Imports;
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

#include <QtDeclarative/qmlmetatype.h>
#include <private/qmltypenamecache_p.h>
static QmlTypeNameCache *cacheForNamespace(QmlEngine *engine, const QmlEnginePrivate::ImportedNamespace &set, QmlTypeNameCache *cache)
{
    if (!cache)
        cache = new QmlTypeNameCache(engine);

    QList<QmlType *> types = QmlMetaType::qmlTypes();

    for (int ii = 0; ii < set.urls.count(); ++ii) {
        if (!set.isBuiltin.at(ii))
            continue;

        QByteArray base = set.urls.at(ii).toUtf8() + "/";
        int major = set.majversions.at(ii);
        int minor = set.minversions.at(ii);

        foreach (QmlType *type, types) {
            if (type->qmlTypeName().startsWith(base) && 
                type->qmlTypeName().lastIndexOf('/') == (base.length() - 1) && 
                type->majorVersion() == major && type->minMinorVersion() <= minor &&
                type->maxMinorVersion() >= minor) {

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

  The default is QML/OfflineStorage/ in the platform-standard
  user application data directory.
*/
void QmlEngine::setOfflineStoragePath(const QString& dir)
{
    Q_D(QmlEngine);
    d->offlineStoragePath = dir;
}

QString QmlEngine::offlineStoragePath() const
{
    Q_D(const QmlEngine);
    return d->offlineStoragePath;
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
        qDebug() << "QmlEngine::addToImport(" << imports << uri << prefix << vmaj << "." << vmin << (importType==QmlScriptParser::Import::Library? "Library" : "File") << ": " << ok;
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
                    qDebug() << "QmlEngine::resolveType" << type << "=" << (*type_return)->typeName();
                if (url_return)
                    qDebug() << "QmlEngine::resolveType" << type << "=" << *url_return;
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

    QByteArray ptr = name + "*";
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

bool QmlEnginePrivate::isObject(int t)
{
    return m_compositeTypes.contains(t) || QmlMetaType::isObject(t);
}

int QmlEnginePrivate::qmlListType(int t) const
{
    QHash<int, int>::ConstIterator iter = m_qmlLists.find(t);
    if (iter != m_qmlLists.end())
        return *iter;
    else
        return QmlMetaType::qmlListType(t);
}

const QMetaObject *QmlEnginePrivate::rawMetaObjectForType(int t) const
{
    QHash<int, QmlCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        return QmlMetaType::rawMetaObjectForType(t);
    }
}

const QMetaObject *QmlEnginePrivate::metaObjectForType(int t) const
{
    QHash<int, QmlCompiledData*>::ConstIterator iter = m_compositeTypes.find(t);
    if (iter != m_compositeTypes.end()) {
        return (*iter)->root;
    } else {
        return QmlMetaType::metaObjectForType(t);
    }
}

QT_END_NAMESPACE
