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

#include "qmlcontextscriptclass_p.h"
#include <private/qmlengine_p.h>
#include <private/qmlcontext_p.h>
#include <private/qmltypenamescriptclass_p.h>

QT_BEGIN_NAMESPACE

struct ContextData : public QScriptDeclarativeClass::Object {
    ContextData(QmlContext *c) : context(c) {}
    QGuard<QmlContext> context;
};

/*
    The QmlContextScriptClass handles property access for a QmlContext
    via QtScript.
 */
QmlContextScriptClass::QmlContextScriptClass(QmlEngine *bindEngine)
: QScriptDeclarativeClass(QmlEnginePrivate::getScriptEngine(bindEngine)), engine(bindEngine),
  lastData(0), lastPropertyIndex(-1), lastDefaultObject(-1)
{
}

QmlContextScriptClass::~QmlContextScriptClass()
{
}

QScriptValue QmlContextScriptClass::newContext(QmlContext *context)
{
    QScriptEngine *scriptEngine = QmlEnginePrivate::getScriptEngine(engine);

    return newObject(scriptEngine, this, new ContextData(context));
}

QmlContext *QmlContextScriptClass::contextFromValue(const QScriptValue &v)
{
    if (scriptClass(v) != this)
        return 0;

    ContextData *data = (ContextData *)object(v);
    return data->context;
}

#include <QDebug>
QScriptClass::QueryFlags 
QmlContextScriptClass::queryProperty(Object *object, const Identifier &name, 
                                     QScriptClass::QueryFlags flags)
{
    Q_UNUSED(flags);
    
    lastContext = 0;
    lastData = 0;
    lastPropertyIndex = -1;
    lastDefaultObject = -1;

    QmlContext *bindContext = ((ContextData *)object)->context.data();
    if (!bindContext)
        return 0;

    while (bindContext) {
        QScriptClass::QueryFlags rv = queryProperty(bindContext, name, flags);
        if (rv) return rv;
        bindContext = bindContext->parentContext();
    }

    return 0;
}

QScriptClass::QueryFlags 
QmlContextScriptClass::queryProperty(QmlContext *bindContext, const Identifier &name,
                                     QScriptClass::QueryFlags flags)
{
    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);

    lastPropertyIndex = cp->propertyNames?cp->propertyNames->value(name):-1;
    if (lastPropertyIndex != -1) {
        lastContext = bindContext;
        return QScriptClass::HandlesReadAccess;
    }

    if (cp->imports) { 
        QmlTypeNameCache::Data *data = cp->imports->data(name);

        if (data)  {
            lastData = data;
            lastContext = bindContext;
            return QScriptClass::HandlesReadAccess;
        }
    }

    for (int ii = 0; ii < cp->defaultObjects.count(); ++ii) {
        QScriptClass::QueryFlags rv = 
            ep->objectClass->queryProperty(cp->defaultObjects.at(ii), name, flags, 0);

        if (rv) {
            lastDefaultObject = ii;
            lastContext = bindContext;
            return rv;
        }
    }

    for (int ii = 0; ii < cp->scripts.count(); ++ii) {
        lastFunction = QScriptDeclarativeClass::function(cp->scripts.at(ii), name);
        if (lastFunction.isValid()) {
            lastContext = bindContext;
            return QScriptClass::HandlesReadAccess;
        }
    }
    return 0;
}

QScriptValue QmlContextScriptClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);

    QmlContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);


    if (lastData) {

        if (lastData->type)
            return ep->typeNameClass->newObject(cp->defaultObjects.at(0), lastData->type);
        else
            return ep->typeNameClass->newObject(cp->defaultObjects.at(0), lastData->typeNamespace);

    } else if (lastPropertyIndex != -1) {

        QScriptValue rv;
        if (lastPropertyIndex < cp->idValueCount) {
            rv =  ep->objectClass->newQObject(cp->idValues[lastPropertyIndex].data());
        } else {
            QVariant value = cp->propertyValues.at(lastPropertyIndex);
            rv = ep->scriptValueFromVariant(value);
        }

        ep->capturedProperties << 
            QmlEnginePrivate::CapturedProperty(bindContext, -1, lastPropertyIndex + cp->notifyIndex);

        return rv;
    } else if(lastDefaultObject != -1) {

        // Default object property
        return ep->objectClass->property(cp->defaultObjects.at(lastDefaultObject), name);

    } else {

        return lastFunction;

    }
}

void QmlContextScriptClass::setProperty(Object *object, const Identifier &name, 
                                        const QScriptValue &value)
{
    Q_ASSERT(lastDefaultObject != -1);

    QmlContext *bindContext = lastContext;
    Q_ASSERT(bindContext);

    QmlEnginePrivate *ep = QmlEnginePrivate::get(engine);
    QmlContextPrivate *cp = QmlContextPrivate::get(bindContext);

    ep->objectClass->setProperty(cp->defaultObjects.at(lastDefaultObject), name, value);
}

QT_END_NAMESPACE
