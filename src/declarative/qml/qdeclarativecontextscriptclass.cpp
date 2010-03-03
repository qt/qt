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

#include "qdeclarativecontextscriptclass_p.h"

#include "qdeclarativeengine_p.h"
#include "qdeclarativecontext_p.h"
#include "qdeclarativetypenamescriptclass_p.h"
#include "qdeclarativelistscriptclass_p.h"
#include "qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

struct ContextData : public QScriptDeclarativeClass::Object {
    ContextData() : overrideObject(0), isSharedContext(true) {}
    ContextData(QDeclarativeContext *c, QObject *o) : context(c), scopeObject(o), overrideObject(0), isSharedContext(false) {}
    QDeclarativeGuard<QDeclarativeContext> context;
    QDeclarativeGuard<QObject> scopeObject;
    QObject *overrideObject;
    bool isSharedContext;

    QDeclarativeContext *getContext(QDeclarativeEngine *engine) {
        if (isSharedContext) {
            return QDeclarativeEnginePrivate::get(engine)->sharedContext;
        } else {
            return context.data();
        }
    }

    QObject *getScope(QDeclarativeEngine *engine) {
        if (isSharedContext) {
            return QDeclarativeEnginePrivate::get(engine)->sharedScope;
        } else {
            return scopeObject.data();
        }
    }
};

/*
    The QDeclarativeContextScriptClass handles property access for a QDeclarativeContext
    via QtScript.
 */
QDeclarativeContextScriptClass::QDeclarativeContextScriptClass(QDeclarativeEngine *bindEngine)
: QDeclarativeScriptClass(QDeclarativeEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine),
  lastScopeObject(0), lastContext(0), lastData(0), lastPropertyIndex(-1), lastDefaultObject(-1)
{
}

QDeclarativeContextScriptClass::~QDeclarativeContextScriptClass()
{
}

QScriptValue QDeclarativeContextScriptClass::newContext(QDeclarativeContext *context, QObject *scopeObject)
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new ContextData(context, scopeObject));
}

QScriptValue QDeclarativeContextScriptClass::newSharedContext()
{
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new ContextData());
}

QDeclarativeContext *QDeclarativeContextScriptClass::contextFromValue(const QScriptValue &v)
{
    if (scriptClass(v) != this)
        return 0;

    ContextData *data = (ContextData *)object(v);
    return data->getContext(engine);
}

QObject *QDeclarativeContextScriptClass::setOverrideObject(QScriptValue &v, QObject *override)
{
    if (scriptClass(v) != this)
        return 0;

    ContextData *data = (ContextData *)object(v);
    QObject *rv = data->overrideObject;
    data->overrideObject = override;
    return rv;
}

QScriptClass::QueryFlags 
QDeclarativeContextScriptClass::queryProperty(Object *object, const Identifier &name, 
                                     QScriptClass::QueryFlags flags)
{
    Q_UNUSED(flags);
    
    lastScopeObject = 0;
    lastContext = 0;
    lastData = 0;
    lastPropertyIndex = -1;
    lastDefaultObject = -1;

    QDeclarativeContext *bindContext = ((ContextData *)object)->getContext(engine);
    QObject *scopeObject = ((ContextData *)object)->getScope(engine);
    if (!bindContext)
        return 0;

    QObject *overrideObject = ((ContextData *)object)->overrideObject;
    if (overrideObject) {
        QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
        QScriptClass::QueryFlags rv = 
            ep->objectClass->queryProperty(overrideObject, name, flags, bindContext, 
                                           QDeclarativeObjectScriptClass::ImplicitObject | 
                                           QDeclarativeObjectScriptClass::SkipAttachedProperties);
        if (rv) {
            lastScopeObject = overrideObject;
            lastContext = bindContext;
            return rv;
        }
    }

    bool includeTypes = true;
    while (bindContext) {
        QScriptClass::QueryFlags rv = 
            queryProperty(bindContext, scopeObject, name, flags, includeTypes);
        scopeObject = 0; // Only applies to the first context
        includeTypes = false; // Only applies to the first context
        if (rv) return rv;
        bindContext = bindContext->parentContext();
    }

    return 0;
}

QScriptClass::QueryFlags 
QDeclarativeContextScriptClass::queryProperty(QDeclarativeContext *bindContext, QObject *scopeObject,
                                     const Identifier &name,
                                     QScriptClass::QueryFlags flags, 
                                     bool includeTypes)
{
    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
    QDeclarativeContextPrivate *cp = QDeclarativeContextPrivate::get(bindContext);

    lastPropertyIndex = cp->propertyNames?cp->propertyNames->value(name):-1;
    if (lastPropertyIndex != -1) {
        lastContext = bindContext;
        return QScriptClass::HandlesReadAccess;
    }

    if (includeTypes && cp->imports) { 
        QDeclarativeTypeNameCache::Data *data = cp->imports->data(name);

        if (data)  {
            lastData = data;
            lastContext = bindContext;
            return QScriptClass::HandlesReadAccess;
        }
    }

    for (int ii = 0; ii < cp->scripts.count(); ++ii) {
        lastFunction = QScriptDeclarativeClass::function(cp->scripts.at(ii), name);
        if (lastFunction.isValid()) {
            lastContext = bindContext;
            return QScriptClass::HandlesReadAccess;
        }
    }

    if (scopeObject) {
        QScriptClass::QueryFlags rv = 
            ep->objectClass->queryProperty(scopeObject, name, flags, bindContext, 
                                           QDeclarativeObjectScriptClass::ImplicitObject | QDeclarativeObjectScriptClass::SkipAttachedProperties);
        if (rv) {
            lastScopeObject = scopeObject;
            lastContext = bindContext;
            return rv;
        }
    }

    for (int ii = cp->defaultObjects.count() - 1; ii >= 0; --ii) {
        QScriptClass::QueryFlags rv = 
            ep->objectClass->queryProperty(cp->defaultObjects.at(ii), name, flags, bindContext, 
                                           QDeclarativeObjectScriptClass::ImplicitObject | QDeclarativeObjectScriptClass::SkipAttachedProperties);

        if (rv) {
            lastDefaultObject = ii;
            lastContext = bindContext;
            return rv;
        }
    }

    return 0;
}

QDeclarativeContextScriptClass::ScriptValue
QDeclarativeContextScriptClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);

    QDeclarativeContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
    QDeclarativeContextPrivate *cp = QDeclarativeContextPrivate::get(bindContext);
    QScriptEngine *scriptEngine = QDeclarativeEnginePrivate::getScriptEngine(engine);

    if (lastScopeObject) {

        return ep->objectClass->property(lastScopeObject, name);

    } else if (lastData) {

        if (lastData->type)
            return Value(scriptEngine, ep->typeNameClass->newObject(cp->defaultObjects.at(0), lastData->type));
        else
            return Value(scriptEngine, ep->typeNameClass->newObject(cp->defaultObjects.at(0), lastData->typeNamespace));

    } else if (lastPropertyIndex != -1) {

        QScriptValue rv;
        if (lastPropertyIndex < cp->idValueCount) {
            rv =  ep->objectClass->newQObject(cp->idValues[lastPropertyIndex].data());
        } else {
            const QVariant &value = cp->propertyValues.at(lastPropertyIndex);
            if (value.userType() == qMetaTypeId<QList<QObject*> >()) {
                rv = ep->listClass->newList(QDeclarativeListProperty<QObject>(bindContext, (void*)lastPropertyIndex, 0, QDeclarativeContextPrivate::context_count, QDeclarativeContextPrivate::context_at), qMetaTypeId<QDeclarativeListProperty<QObject> >());
            } else {
                rv = ep->scriptValueFromVariant(value);
            }
        }

        ep->capturedProperties << 
            QDeclarativeEnginePrivate::CapturedProperty(bindContext, -1, lastPropertyIndex + cp->notifyIndex);

        return Value(scriptEngine, rv);
    } else if(lastDefaultObject != -1) {

        // Default object property
        return ep->objectClass->property(cp->defaultObjects.at(lastDefaultObject), name);

    } else {

        return Value(scriptEngine, lastFunction);

    }
}

void QDeclarativeContextScriptClass::setProperty(Object *object, const Identifier &name, 
                                        const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_ASSERT(lastScopeObject || lastDefaultObject != -1);

    QDeclarativeContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(engine);
    QDeclarativeContextPrivate *cp = QDeclarativeContextPrivate::get(bindContext);

    if (lastScopeObject) {
        ep->objectClass->setProperty(lastScopeObject, name, value, bindContext);
    } else {
        ep->objectClass->setProperty(cp->defaultObjects.at(lastDefaultObject), name, value,
                                     bindContext);
    }
}

QT_END_NAMESPACE
