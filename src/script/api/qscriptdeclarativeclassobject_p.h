/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTDECLARATIVECLASSOBJECT_P_H
#define QSCRIPTDECLARATIVECLASSOBJECT_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qscriptdeclarativeclass_p.h"
#include "qscriptv8objectwrapper_p.h"
#include "qscriptcontext_p.h"

#include <QtCore/QScopedValueRollback>

QT_BEGIN_NAMESPACE

class QScriptDeclarativeClassPrivate
{
public:
    QScriptDeclarativeClassPrivate() : engine(0), q_ptr(0), context(0), supportsCall(false) {}

    QScriptEngine *engine;
    QScriptDeclarativeClass *q_ptr;
    QScriptContext *context;
    //FIXME: avoid global statics
    static QSet<QString> identifiers;
    bool supportsCall:1;

    static QScriptDeclarativeClassPrivate *get(QScriptDeclarativeClass *c) {
        return c->d_ptr.data();
    }
};


struct QScriptDeclarativeClassObject : QScriptV8ObjectWrapper<QScriptDeclarativeClassObject, &QScriptEnginePrivate::declarativeClassTemplate> {
    QScopedPointer<QScriptDeclarativeClass::Object> obj;
    QScriptDeclarativeClass* scriptClass;

    v8::Handle<v8::Value> property(v8::Local<v8::String> property)
    {
        QScriptDeclarativeClassPrivate* scriptDeclarativeClassP = QScriptDeclarativeClassPrivate::get(scriptClass);
        QScopedValueRollback<QScriptContext *> saveContext(scriptDeclarativeClassP->context);
        scriptDeclarativeClassP->context = engine->currentContext();

        QScriptDeclarativeClass::PersistentIdentifier identifier =
            scriptClass->createPersistentIdentifier(QScriptConverter::toString(property));
        QScriptClass::QueryFlags fl = scriptClass->queryProperty(obj.data(), identifier.identifier, QScriptClass::HandlesReadAccess);
        if (fl & QScriptClass::HandlesReadAccess) {
            QScriptValue result = scriptClass->property(obj.data(), identifier.identifier).toScriptValue(QScriptEnginePrivate::get(engine));
            return QScriptValuePrivate::get(result)->asV8Value(engine);
        }
        return v8::Handle<v8::Value>();
    }

    v8::Handle<v8::Value> setProperty(v8::Local<v8::String> property, v8::Local<v8::Value> value)
    {
        QScriptDeclarativeClassPrivate* scriptDeclarativeClassP = QScriptDeclarativeClassPrivate::get(scriptClass);
        QScopedValueRollback<QScriptContext *> saveContext(scriptDeclarativeClassP->context);
        scriptDeclarativeClassP->context = engine->currentContext();

        QScriptDeclarativeClass::PersistentIdentifier identifier =
            scriptClass->createPersistentIdentifier(QScriptConverter::toString(property));
        QScriptClass::QueryFlags fl = scriptClass->queryProperty(obj.data(), identifier.identifier, QScriptClass::HandlesWriteAccess);
        if (fl & QScriptClass::HandlesWriteAccess) {
            scriptClass->setProperty(obj.data(), identifier.identifier, QScriptValuePrivate::get(new QScriptValuePrivate(engine, value)));
            return value;
        }
        return v8::Handle<v8::Value>();
    }

    v8::Handle<v8::Value> property(uint32_t index)
    {
        return property(v8::Integer::New(index)->ToString());
    }

    v8::Handle<v8::Value> setProperty(uint32_t index, v8::Local<v8::Value> value)
    {
        return setProperty(v8::Integer::New(index)->ToString(), value);
    }

    v8::Handle<v8::Value> call()
    {
        QScriptValue result = scriptClass->call(obj.data(), engine->currentContext()).toScriptValue(QScriptEnginePrivate::get(engine));
        return QScriptValuePrivate::get(result)->asV8Value(engine);
    }

    static v8::Handle<v8::FunctionTemplate> createFunctionTemplate(QScriptEnginePrivate *engine)
    {
        using namespace QScriptV8ObjectWrapperHelper;
        v8::HandleScope handleScope;
        v8::Handle<v8::FunctionTemplate> funcTempl = v8::FunctionTemplate::New();
        v8::Handle<v8::ObjectTemplate> instTempl = funcTempl->InstanceTemplate();
        instTempl->SetInternalFieldCount(1);
        instTempl->SetCallAsFunctionHandler(callAsFunction<QScriptDeclarativeClassObject>);
        instTempl->SetNamedPropertyHandler(namedPropertyGetter<QScriptDeclarativeClassObject>, namedPropertySetter<QScriptDeclarativeClassObject>);
        instTempl->SetIndexedPropertyHandler(indexedPropertyGetter<QScriptDeclarativeClassObject>, indexedPropertySetter<QScriptDeclarativeClassObject>);
        return handleScope.Close(funcTempl);
    }

    static v8::Handle<v8::Value> newInstance(QScriptEnginePrivate *engine, QScriptDeclarativeClass *scriptClass,
                                                QScriptDeclarativeClass::Object *object)
    {
        v8::HandleScope handleScope;
        QScriptDeclarativeClassObject *data = new QScriptDeclarativeClassObject;
        data->engine = engine;
        data->obj.reset(object);
        data->scriptClass = scriptClass;
        return handleScope.Close(createInstance(data));
    }

    static inline QScriptDeclarativeClass *declarativeClass(const QScriptValuePrivate *v)
    {
        QScriptDeclarativeClassObject *o = safeGet(v);
        if (o)
            return o->scriptClass;
        return 0;
    }

    static inline QScriptDeclarativeClass::Object *object(const QScriptValuePrivate *v)
    {
        QScriptDeclarativeClassObject *o = safeGet(v);
        if (o)
            return o->obj.data();
        return 0;
    }

};

QT_END_NAMESPACE

#endif
