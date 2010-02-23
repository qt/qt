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

#include "qmlcontextscriptclass_p.h"

#include "qmlengine_p.h"
#include "qmlcontext_p.h"
#include "qmltypenamescriptclass_p.h"
#include "qmllistscriptclass_p.h"
#include "qmlguard_p.h"

QT_BEGIN_NAMESPACE

struct ContextData : public QScriptDeclarativeClass::Object {
    ContextData() : isSharedContext(true) {}
    ContextData(QmlContext *c, QObject *o) : context(c), scopeObject(o), isSharedContext(false) {}
    QmlGuard<QmlContext> context;
    QmlGuard<QObject> scopeObject;
    bool isSharedContext;

    QmlContext *getContext(QmlEngine *engine) {
        if (isSharedContext) {
            return QmlEnginePrivate::get(engine)->sharedContext;
        } else {
            return context.data();
        }
    }

    QObject *getScope(QmlEngine *engine) {
        if (isSharedContext) {
            return QmlEnginePrivate::get(engine)->sharedScope;
        } else {
            return scopeObject.data();
        }
    }
};

/*
    The QmlContextScriptClass handles property access for a QmlContext
    via QtScript.
 */
QmlContextScriptClass::QmlContextScriptClass(QmlEngine *bindEngine)
: QmlScriptClass(QmlEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine),
  lastScopeObject(0), lastContext(0), lastData(0), lastPropertyIndex(-1), lastDefaultObject(-1)
{
}

QmlContextScriptClass::~QmlContextScriptClass()
{
}

QScriptValue QmlContextScriptClass::newContext(QmlContext *context, QObject *scopeObject)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new ContextData(context, scopeObject));
}

QScriptValue QmlContextScriptClass::newSharedContext()
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new ContextData());
}

QmlContext *QmlContextScriptClass::contextFromValue(const QScriptValue &v)
{
    if (scriptClass(v) != this)
        return 0;

    ContextData *data = (ContextData *)object(v);
    return data->getContext(engine);
}

QScriptClass::QueryFlags 
QmlContextScriptClass::queryProperty(Object *object, const Identifier &name, 
                                     QScriptClass::QueryFlags flags)
{
    Q_UNUSED(flags);
    
    lastScopeObject = 0;
    lastContext = 0;
    lastData = 0;
    lastPropertyIndex = -1;
    lastDefaultObject = -1;

    QmlContext *bindContext = ((ContextData *)object)->getContext(engine);
    QObject *scopeObject = ((ContextData *)object)->getScope(engine);
    if (!bindContext)
        return 0;

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
QmlContextScriptClass::queryProperty(QmlContext *bindContext, QObject *scopeObject,
                                     const Identifier &name,
                                     QScriptClass::QueryFlags flags, 
                                     bool includeTypes)
{
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);

    lastPropertyIndex = cp->propertyNames?cp->propertyNames->value(name):-1;
    if (lastPropertyIndex != -1) {
        lastContext = bindContext;
        return QScriptClass::HandlesReadAccess;
    }

    if (includeTypes && cp->imports) { 
        QmlTypeNameCache::Data *data = cp->imports->data(name);

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
                                           QmlObjectScriptClass::ImplicitObject | QmlObjectScriptClass::SkipAttachedProperties);
        if (rv) {
            lastScopeObject = scopeObject;
            lastContext = bindContext;
            return rv;
        }
    }

    for (int ii = cp->defaultObjects.count() - 1; ii >= 0; --ii) {
        QScriptClass::QueryFlags rv = 
            ep->objectClass->queryProperty(cp->defaultObjects.at(ii), name, flags, bindContext, 
                                           QmlObjectScriptClass::ImplicitObject | QmlObjectScriptClass::SkipAttachedProperties);

        if (rv) {
            lastDefaultObject = ii;
            lastContext = bindContext;
            return rv;
        }
    }

    return 0;
}

QmlContextScriptClass::ScriptValue
QmlContextScriptClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);

    QmlContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

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
                rv = ep->listClass->newList(QmlListProperty<QObject>(bindContext, (void*)lastPropertyIndex, 0, QmlContextPrivate::context_count, QmlContextPrivate::context_at), qMetaTypeId<QmlListProperty<QObject> >());
            } else {
                rv = ep->scriptValueFromVariant(value);
            }
        }

        ep->capturedProperties << 
            QmlEnginePrivate::CapturedProperty(bindContext, -1, lastPropertyIndex + cp->notifyIndex);

        return Value(scriptEngine, rv);
    } else if(lastDefaultObject != -1) {

        // Default object property
        return ep->objectClass->property(cp->defaultObjects.at(lastDefaultObject), name);

    } else {

        return Value(scriptEngine, lastFunction);

    }
}

void QmlContextScriptClass::setProperty(Object *object, const Identifier &name, 
                                        const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_ASSERT(lastScopeObject || lastDefaultObject != -1);

    QmlContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);

    if (lastScopeObject) {
        ep->objectClass->setProperty(lastScopeObject, name, value, bindContext);
    } else {
        ep->objectClass->setProperty(cp->defaultObjects.at(lastDefaultObject), name, value,
                                     bindContext);
    }
}

QT_END_NAMESPACE
