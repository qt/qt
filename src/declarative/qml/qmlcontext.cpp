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

#include <qmlcontext.h>
#include <private/qmlcontext_p.h>
#include <private/qmlengine_p.h>
#include <private/qmlcompiledcomponent_p.h>
#include <qmlengine.h>
#include <qscriptengine.h>

#include <qdebug.h>

// 6-bits
#define MAXIMUM_DEFAULT_OBJECTS 63

QT_BEGIN_NAMESPACE

QmlContextPrivate::QmlContextPrivate()
    : parent(0), engine(0), highPriorityCount(0), component(0)
{
}

void QmlContextPrivate::dump()
{
    dump(0);
}

void QmlContextPrivate::dump(int depth)
{
    QByteArray ba(depth * 4, ' ');
    qWarning() << ba << properties.keys();
    qWarning() << ba << variantProperties.keys();
    if (parent)
        parent->d_func()->dump(depth + 1);
}

void QmlContextPrivate::destroyed(QObject *obj)
{
    defaultObjects.removeAll(obj);
    for (QHash<QString, QObject *>::Iterator iter = properties.begin();
            iter != properties.end(); ) {
        if (*iter == obj)
            iter = properties.erase(iter);
        else
            ++iter;
    }
}

void QmlContextPrivate::init()
{
    Q_Q(QmlContext);
    //set scope chain
    QScriptEngine *scriptEngine = engine->scriptEngine();
    QScriptValue scopeObj =
        scriptEngine->newObject(engine->d_func()->contextClass, scriptEngine->newVariant(QVariant::fromValue((QObject*)q)));
    if (!parent)
        scopeChain.append(scriptEngine->globalObject());
    else
        scopeChain = parent->d_func()->scopeChain;
    scopeChain.prepend(scopeObj);

    contextData.context = q;
}

void QmlContextPrivate::addDefaultObject(QObject *object, Priority priority)
{
    if (defaultObjects.count() >= (MAXIMUM_DEFAULT_OBJECTS - 1)) {
        qWarning("QmlContext: Cannot have more than %d default objects on "
                 "one bind context.", MAXIMUM_DEFAULT_OBJECTS - 1);
        return;
    }

    if (priority == HighPriority) {
        defaultObjects.insert(highPriorityCount++, object);
    } else {
        defaultObjects.append(object);
    }
    QObject::connect(object, SIGNAL(destroyed(QObject*)),
                     q_ptr, SLOT(objectDestroyed(QObject*)));
}


/*!
    \class QmlContext
    \brief The QmlContext class defines a context within a QML engine.
    \mainclass

    Contexts allow data to be exposed to the QML components instantiated by the
    QML engine.

    Each QmlContext contains a set of properties, distinct from
    its QObject properties, that allow data to be
    explicitly bound to a context by name.  The context properties are defined or
    updated by calling QmlContext::setContextProperty().  The following example shows
    a Qt model being bound to a context and then accessed from a QML file.

    \code
    QmlEngine engine;
    QmlContext context(engine.rootContext());
    context.setContextProperty("myModel", modelData);

    QmlComponent component("<ListView model=\"{myModel}\" />");
    component.create(&context);
    \endcode

    To simplify binding and maintaining larger data sets, QObject's can be
    added to a QmlContext.  These objects are known as the context's default
    objects.  In this case all the properties of the QObject are
    made available by name in the context, as though they were all individually
    added by calling QmlContext::setContextProperty().  Changes to the property's
    values are detected through the property's notify signal.  This method is
    also slightly more faster than manually adding property values.

    The following example has the same effect as the one above, but it is
    achieved using a default object.

    \code
    class MyDataSet : ... {
        ...
        Q_PROPERTY(QAbstractItemModel *myModel READ model NOTIFY modelChanged);
        ...
    };

    MyDataSet myDataSet;
    QmlEngine engine;
    QmlContext context(engine.rootContext());
    context.addDefaultObject(&myDataSet);

    QmlComponent component("<ListView model=\"{myModel}\" />");
    component.create(&context);
    \endcode

    Each context may have up to 32 default objects, and objects added first take
    precedence over those added later.  All properties added explicitly by
    QmlContext::setContextProperty() take precedence over default object properties.

    Contexts are hierarchal, with the \l {QmlEngine::rootContext()}{root context}
    being created by the QmlEngine.  A component instantiated in a given context
    has access to that context's data, as well as the data defined by its
    ancestor contexts.  Data values (including those added implicitly by the
    default objects) in a context override those in ancestor contexts.  Data
    that should be available to all components instantiated by the QmlEngine
    should be added to the \l {QmlEngine::rootContext()}{root context}.

    In the following example,

    \code
    QmlEngine engine;
    QmlContext context1(engine.rootContext());
    QmlContext context2(&context1);
    QmlContext context3(&context2);

    context1.setContextProperty("a", 12);
    context2.setContextProperty("b", 13);
    context3.setContextProperty("a", 14);
    context3.setContextProperty("c", 14);
    \endcode

    a QML component instantiated in context1 would have access to the "a" data,
    a QML component instantiated in context2 would have access to the "a" and
    "b" data, and a QML component instantiated in context3 would have access to
    the "a", "b" and "c" data - although its "a" data would return 14, unlike
    that in context1 or context2.
*/

/*! \internal */
QmlContext::QmlContext(QmlEngine *e)
: QObject(*(new QmlContextPrivate))
{
    Q_D(QmlContext);
    d->engine = e;
    d->init();
}

/*!
    Create a new QmlContext with the given \a parentContext, and the
    QObject \a parent.
*/
QmlContext::QmlContext(QmlContext *parentContext, QObject *parent)
: QObject(*(new QmlContextPrivate), parent)
{
    Q_D(QmlContext);
    d->parent = parentContext;
    d->engine = parentContext->engine();
    d->init();
}

/*!
    Destroys the QmlContext.

    Any expressions, or sub-contexts dependent on this context will be
    invalidated, but not destroyed (unless they are parented to the QmlContext
    object).
 */
QmlContext::~QmlContext()
{
    Q_D(QmlContext);
    if (d->component) d->component->release();
}


/*!
    Return the context's QmlEngine, or 0 if the context has no QmlEngine or the
    QmlEngine was destroyed.
*/
QmlEngine *QmlContext::engine() const
{
    Q_D(const QmlContext);
    return d->engine;
}

/*!
    Return the context's parent QmlContext, or 0 if this context has no
    parent or if the parent has been destroyed.
*/
QmlContext *QmlContext::parentContext() const
{
    Q_D(const QmlContext);
    return d->parent;
}

/*!
    Add a default \a object to this context.  The object will be added after
    any existing default objects.
*/
void QmlContext::addDefaultObject(QObject *defaultObject)
{
    Q_D(QmlContext);
    d->addDefaultObject(defaultObject, QmlContextPrivate::NormalPriority);
}

/*!
    Set a the \a value of the \a name property on this context.
*/
void QmlContext::setContextProperty(const QString &name, const QVariant &value)
{
    Q_D(QmlContext);
    d->variantProperties.insert(name, value);
}

/*!
    Set a the \a value of the \a name property on this context.

    QmlContext does \b not take ownership of \a value.
*/
void QmlContext::setContextProperty(const QString &name, QObject *value)
{
    Q_D(QmlContext);
    d->properties.insert(name, value);
    QObject::connect(value, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
}

/*!
    Activate this bind context.

    \sa QmlEngine::activeContext() QmlContext::activeContext()
*/
void QmlContext::activate()
{
    QmlEnginePrivate *ep = engine()->d_func();
    ep->activeContexts.push(this);
    ep->setCurrentBindContext(this);
    ep->contextActivated(this);
}

/*!
    Deactivate this bind context.  The previously active bind context will
    become active, or, if there was no previously active bind context, no
    context will be active.

    \sa QmlEngine::activeContext() QmlContext::activeContext()
*/
void QmlContext::deactivate()
{
    QmlEnginePrivate *ep = engine()->d_func();
    Q_ASSERT(ep->activeContexts.top() == this);
    ep->activeContexts.pop();
    if (ep->activeContexts.isEmpty())
        ep->setCurrentBindContext(0);
    else
        ep->setCurrentBindContext(ep->activeContexts.top());
    ep->contextDeactivated(this);
}

/*!
    Returns the currently active context, or 0 if no context is active.

    This method is thread-safe.  The active context is maintained individually
    for each thread.  This method is equivalent to
    \code
    QmlEngine::activeEngine()->activeContext()
    \endcode
*/
QmlContext *QmlContext::activeContext()
{
    QmlEngine *engine = QmlEngine::activeEngine();
    if (engine)
        return engine->activeContext();
    else
        return 0;
}

/*!
    Resolves the URL \a src relative to the URL of the
    containing component.

    If \a src is absolute, it is simply returned.

    If there is no containing component, an empty URL is returned.

    \sa componentUrl
*/
QUrl QmlContext::resolvedUrl(const QUrl &src)
{
    QmlContext *ctxt = this;
    if (src.isRelative()) {
        if (ctxt) {
            while(ctxt) {
                if (ctxt->d_func()->component)
                    break;
                else
                    ctxt = ctxt->parentContext();
            }

            if (ctxt)
                return ctxt->d_func()->component->url.resolved(src);
        }
        return QUrl();
    } else {
        return src;
    }
}

/*!
    Resolves the component URI \a src relative to the URL of the
    containing component, and according to the
    \link QmlEngine::nameSpacePaths() namespace paths\endlink of the
    context's engine, returning the resolved URL.

    \sa componentUrl
*/
QUrl QmlContext::resolvedUri(const QUrl &src)
{
    QmlContext *ctxt = this;
    if (src.isRelative()) {
        if (ctxt) {
            while(ctxt) {
                if (ctxt->d_func()->component)
                    break;
                else
                    ctxt = ctxt->parentContext();
            }

            if (ctxt)
                return ctxt->d_func()->engine->componentUrl(src, ctxt->d_func()->component->url);
        }
        return QUrl();
    } else {
        return ctxt->d_func()->engine->componentUrl(src, QUrl());
    }
}

void QmlContext::objectDestroyed(QObject *object)
{
    Q_D(QmlContext);
    d->destroyed(object);
}

QT_END_NAMESPACE
