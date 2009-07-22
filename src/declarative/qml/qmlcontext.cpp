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
#include <private/qmlexpression_p.h>
#include <private/qmlengine_p.h>
#include <qmlengine.h>
#include <qscriptengine.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdebug.h>

// 6-bits
#define MAXIMUM_DEFAULT_OBJECTS 63

QT_BEGIN_NAMESPACE

QmlContextPrivate::QmlContextPrivate()
: parent(0), engine(0), isInternal(false), notifyIndex(-1), 
  highPriorityCount(0), startLine(-1), endLine(-1)
{
}

void QmlContextPrivate::dump()
{
    dump(0);
}

void QmlContextPrivate::dump(int depth)
{
    QByteArray ba(depth * 4, ' ');
    if (parent)
        parent->d_func()->dump(depth + 1);
}

void QmlContextPrivate::destroyed(QObject *obj)
{
    Q_Q(QmlContext);

    defaultObjects.removeAll(obj);

    QVariant variantObject = QVariant::fromValue(obj);
    QVarLengthArray<int> notifies;
    for (int ii = 0; ii < propertyValues.count(); ++ii) {
        if (propertyValues.at(ii) == variantObject) {
            propertyValues[ii] = QVariant();
            notifies.append(ii);
        }
    }

    // There is no need to emit these notifications if our parent is in the 
    // process of being deleted (which is *probably* why obj has been destroyed
    // anyway), as we're about to get deleted which will invalidate all the
    // expressions that could depend on us
    QObject *parent = q->parent();
    if (!parent || !QObjectPrivate::get(parent)->wasDeleted) {
        for (int ii = 0; ii < notifies.count(); ++ii) {
            QMetaObject::activate(q, notifies[ii] + notifyIndex, 0);
        }
    }
}

void QmlContextPrivate::init()
{
    Q_Q(QmlContext);

    if (parent) 
        parent->d_func()->childContexts.insert(q);

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

    QmlComponent component(&engine, "ListView { model=myModel }");
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

    QmlComponent component(&engine, "ListView { model=myModel }");
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
QmlContext::QmlContext(QmlEngine *e, bool)
: QObject(*(new QmlContextPrivate))
{
    Q_D(QmlContext);
    d->engine = e;
    d->init();
}

/*!
    Create a new QmlContext as a child of \a engine's root context, and the
    QObject \a parent.
*/
QmlContext::QmlContext(QmlEngine *engine, QObject *parent)
: QObject(*(new QmlContextPrivate), parent)
{
    Q_D(QmlContext);
    QmlContext *parentContext = engine?engine->rootContext():0;
    d->parent = parentContext;
    d->engine = parentContext->engine();
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
    \internal
*/
QmlContext::QmlContext(QmlContext *parentContext, QObject *parent, bool)
: QObject(*(new QmlContextPrivate), parent)
{
    Q_D(QmlContext);
    d->parent = parentContext;
    d->engine = parentContext->engine();
    d->isInternal = true;
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
    if (d->parent) 
        d->parent->d_func()->childContexts.remove(this);

    for (QSet<QmlContext *>::ConstIterator iter = d->childContexts.begin();
            iter != d->childContexts.end();
            ++iter) {
        (*iter)->d_func()->invalidateEngines();
        (*iter)->d_func()->parent = 0;
    }

    for (QSet<QmlExpression *>::ConstIterator iter = 
            d->childExpressions.begin();
            iter != d->childExpressions.end();
            ++iter) {
        (*iter)->d_func()->ctxt = 0;
    }

    for (int ii = 0; ii < d->contextObjects.count(); ++ii) {
        QObjectPrivate *p = QObjectPrivate::get(d->contextObjects.at(ii));
        QmlSimpleDeclarativeData *data = 
            static_cast<QmlSimpleDeclarativeData *>(p->declarativeData);
        if(data && (data->flags & QmlSimpleDeclarativeData::Extended)) {
            data->context = 0;
        } else {
            p->declarativeData = 0;
        }
    }
    d->contextObjects.clear();
}

void QmlContextPrivate::invalidateEngines()
{
    if (!engine)
        return;
    engine = 0;
    for (QSet<QmlContext *>::ConstIterator iter = childContexts.begin();
            iter != childContexts.end();
            ++iter) {
        (*iter)->d_func()->invalidateEngines();
    }
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
void QmlContext::addDefaultObject(QObject *object)
{
    Q_D(QmlContext);
    d->addDefaultObject(object, QmlContextPrivate::NormalPriority);
}

/*!
    Set a the \a value of the \a name property on this context.
*/
void QmlContext::setContextProperty(const QString &name, const QVariant &value)
{
    Q_D(QmlContext);
    if (d->notifyIndex == -1)
        d->notifyIndex = this->metaObject()->methodCount();

    if (QmlMetaType::isObject(value.userType())) {
        QObject *o = QmlMetaType::toQObject(value);
        setContextProperty(name, o);
    } else {
        QHash<QString, int>::ConstIterator iter = d->propertyNames.find(name);
        if(iter == d->propertyNames.end()) {
            d->propertyNames.insert(name, d->propertyValues.count());
            d->propertyValues.append(value);
        } else {
            d->propertyValues[*iter] = value;
            QMetaObject::activate(this, *iter + d->notifyIndex, 0);
        }
    }
}

/*!
    Set a the \a value of the \a name property on this context.

    QmlContext does \b not take ownership of \a value.
*/
void QmlContext::setContextProperty(const QString &name, QObject *value)
{
    Q_D(QmlContext);
    if (d->notifyIndex == -1)
        d->notifyIndex = this->metaObject()->methodCount();

    QObject::connect(value, SIGNAL(destroyed(QObject*)), 
                     this, SLOT(objectDestroyed(QObject*)));

    QHash<QString, int>::ConstIterator iter = d->propertyNames.find(name);
    if(iter == d->propertyNames.end()) {
        d->propertyNames.insert(name, d->propertyValues.count());
        d->propertyValues.append(QVariant::fromValue(value));
    } else {
        int idx = *iter;
        if (QmlMetaType::isObject(d->propertyValues.at(idx).userType())) {
            QObject *old = QmlMetaType::toQObject(d->propertyValues.at(idx));
            QObject::disconnect(old, SIGNAL(destroyed(QObject*)), 
                             this, SLOT(objectDestroyed(QObject*)));
        }
        d->propertyValues[*iter] = QVariant::fromValue(value);
        QMetaObject::activate(this, *iter + d->notifyIndex, 0);
    }
}

/*!
    Resolves the URL \a src relative to the URL of the
    containing component. If \a src is absolute, it is
    simply returned. If there is no containing component,
    an empty URL is returned.

    \sa QmlEngine::componentUrl(), setBaseUrl()
*/
QUrl QmlContext::resolvedUrl(const QUrl &src)
{
    QmlContext *ctxt = this;
    if (src.isRelative()) {
        if (ctxt) {
            while(ctxt) {
                if(ctxt->d_func()->url.isValid())
                    break;
                else
                    ctxt = ctxt->parentContext();
            }

            if (ctxt)
                return ctxt->d_func()->url.resolved(src);
        }
        return QUrl();
    } else {
        return src;
    }
}

/*!
    Explicitly sets the url both resolveUri() and resolveUrl() will
    use for relative references to \a baseUrl.

    Calling this function will override the url of the containing
    component used by default.

    \sa resolvedUrl(), resolvedUri()
*/
void QmlContext::setBaseUrl(const QUrl &baseUrl)
{
    d_func()->url = baseUrl;
}

void QmlContext::objectDestroyed(QObject *object)
{
    Q_D(QmlContext);
    d->destroyed(object);
}

QT_END_NAMESPACE
