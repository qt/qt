/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTV8OBJECTWRAPPER_P_H
#define QSCRIPTV8OBJECTWRAPPER_P_H

#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalue.h"

#include <v8.h>

class QScriptEnginePrivate;

namespace QScriptV8ObjectWrapperHelper {

template <typename T>
void weakCallback(v8::Persistent<v8::Value> object, void *parameter)
{
    Q_ASSERT(object.IsNearDeath());
    T *data = reinterpret_cast<T *>(parameter);
    delete data;
    object.Dispose();
}

template <typename T>
static v8::Handle<v8::Value> namedPropertyGetter(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> self = info.This();
    Q_ASSERT(self->InternalFieldCount() == 1);
    T *data = reinterpret_cast<T *>(self->GetPointerFromInternalField(0));
    Q_ASSERT(data != 0);
    QScriptContextPrivate qScriptContext(data->engine, &info);
    return handleScope.Close(data->property(property));
}

template <typename T>
static v8::Handle<v8::Value> namedPropertySetter(v8::Local<v8::String> property,
                                                  v8::Local<v8::Value> value,
                                                  const v8::AccessorInfo &info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> self = info.This();
    Q_ASSERT(self->InternalFieldCount() == 1);
    T *data = reinterpret_cast<T *>(self->GetPointerFromInternalField(0));
    Q_ASSERT(data != 0);
    QScriptContextPrivate qScriptContext(data->engine, &info);
    return handleScope.Close(data->setProperty(property, value));
}

template <typename T>
static v8::Handle<v8::Array> namedPropertyEnumerator(const v8::AccessorInfo &info)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> self = info.This();
    Q_ASSERT(self->InternalFieldCount() == 1);
    T *data = reinterpret_cast<T *>(self->GetPointerFromInternalField(0));
    Q_ASSERT(data != 0);
    QScriptContextPrivate qScriptContext(data->engine, &info);
    return handleScope.Close(data->enumerate());
}

template <typename T>
v8::Handle<v8::Value> callAsFunction(const v8::Arguments& args)
{
    v8::HandleScope handleScope;
    v8::Local<v8::Object> self = args.Holder();
    Q_ASSERT(self->InternalFieldCount() == 1);
    T *data = reinterpret_cast<T *>(self->GetPointerFromInternalField(0));
    Q_ASSERT(data != 0);
    QScriptContextPrivate qScriptContext(data->engine, &args);
    return handleScope.Close(data->call());
}

}

//T must inherit from QScriptV8ObjectWrapper<T>
template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
struct QScriptV8ObjectWrapper
{
    QScriptEnginePrivate *engine;

    static T *safeGet(const QScriptValuePrivate *p);

    static v8::Handle<v8::Object> createInstance(T *data)
    {
        v8::HandleScope handleScope;
        Q_ASSERT(data->engine);

        if ((data->engine->*functionTemplate).IsEmpty())
            data->engine->*functionTemplate = v8::Persistent<v8::FunctionTemplate>::New(T::createFunctionTemplate(data->engine));

        v8::Handle<v8::ObjectTemplate> instanceTempl = (data->engine->*functionTemplate)->InstanceTemplate();
        v8::Handle<v8::Object> instance = instanceTempl->NewInstance();
        Q_ASSERT(instance->InternalFieldCount() == 1);
        instance->SetPointerInInternalField(0, data);
        v8::Persistent<v8::Object> persistent = v8::Persistent<v8::Object>::New(instance);
        persistent.MakeWeak(data, QScriptV8ObjectWrapperHelper::weakCallback<T>);
        return handleScope.Close(instance);
    }
};


QT_BEGIN_INCLUDE_NAMESPACE
#include "qscriptvalue_p.h"
#include "qscriptisolate_p.h"
QT_END_INCLUDE_NAMESPACE

template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
T* QScriptV8ObjectWrapper<T, functionTemplate>::safeGet(const QScriptValuePrivate* p)
{
    QScriptEnginePrivate *engine = p->engine();
    if (!engine)
        return 0;
    QScriptIsolate api(engine, QScriptIsolate::NotNullEngine);
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> value = *p;

    v8::Handle<v8::FunctionTemplate> funcTmpl = engine->*functionTemplate;
    if (funcTmpl.IsEmpty())
        return 0;
    if (!funcTmpl->HasInstance(value))
        return 0;
    v8::Local<v8::Object> object = v8::Object::Cast(*value);
    Q_ASSERT(object->InternalFieldCount() == 1);
    T *data = reinterpret_cast<T *>(object->GetPointerFromInternalField(0));
    return data;
}


#endif // QSCRIPTFUNCTION_P_H
