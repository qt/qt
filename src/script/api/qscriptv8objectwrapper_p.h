/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qscriptvalue.h"
#include "qscriptvalue_p.h"
#include "qscriptengine_p.h"
#include "qscriptcontext_p.h"
#include <v8.h>

QT_BEGIN_NAMESPACE

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
T *getDataPointer(v8::Handle<v8::Object> self)
{
    Q_ASSERT(!self.IsEmpty());
    Q_ASSERT(self->InternalFieldCount() >= 1);
    T *data = reinterpret_cast<T *>(self->GetPointerFromInternalField(0));
    Q_ASSERT(data != 0);
    return data;
}

template <typename T>
T *getDataPointer(const v8::AccessorInfo &info)
{
    return getDataPointer<T>(info.Holder());
}

template <typename T>
T *getDataPointer(const v8::Arguments& args)
{
    return getDataPointer<T>(args.Holder());
}

#ifdef QTSCRIPT_V8OBJECT_DATA_CALLBACK
#  error QTSCRIPT_V8OBJECT_DATA_CALLBACK name is in use
#else
#define QTSCRIPT_V8OBJECT_DATA_CALLBACK(arg, callback) \
    v8::HandleScope handleScope; \
    T *data = getDataPointer<T>(arg); \
    QScriptContextPrivate qScriptContext(data->engine, &arg); \
    return handleScope.Close(callback);
#endif

template <typename T>
static v8::Handle<v8::Value> namedPropertyGetter(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->property(property));
}

template <typename T>
static v8::Handle<v8::Value> indexedPropertyGetter(uint32_t index, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->property(index));
}

template <typename T>
static v8::Handle<v8::Integer> namedPropertyQuery(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->propertyFlags(property));
}

template <typename T>
static v8::Handle<v8::Integer> indexedPropertyQuery(uint32_t index, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->propertyFlags(index));
}

template <typename T>
static v8::Handle<v8::Boolean> namedPropertyDeleter(v8::Local<v8::String> property, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->removeProperty(property));
}

template <typename T>
static v8::Handle<v8::Boolean> indexedPropertyDeleter(uint32_t index, const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->removeProperty(index));
}

template <typename T>
static v8::Handle<v8::Value> namedPropertySetter(v8::Local<v8::String> property,
                                                  v8::Local<v8::Value> value,
                                                  const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->setProperty(property, value));
}

template <typename T>
static v8::Handle<v8::Value> indexedPropertySetter(uint32_t index,
                                                  v8::Local<v8::Value> value,
                                                  const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->setProperty(index, value));
}

template <typename T>
static v8::Handle<v8::Array> namedPropertyEnumerator(const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->enumerate());
}

template <typename T>
static v8::Handle<v8::Array> indexedPropertyEnumerator(const v8::AccessorInfo &info)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(info, data->enumerate());
}

template <typename T>
v8::Handle<v8::Value> callAsFunction(const v8::Arguments& args)
{
    QTSCRIPT_V8OBJECT_DATA_CALLBACK(args, data->call());
}

#undef QTSCRIPT_V8OBJECT_DATA_CALLBACK

}

//T must inherit from QScriptV8ObjectWrapper<T>
template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
struct QScriptV8ObjectWrapper
{
    QScriptEnginePrivate *engine;

    static T *safeGet(const QScriptValuePrivate *p);
    static T *safeGet(v8::Handle<v8::Object> object, QScriptEnginePrivate *engine);
    static T *get(v8::Handle<v8::Object> object);

    static v8::Handle<v8::Object> createInstance(T *data)
    {
        v8::HandleScope handleScope;
        Q_ASSERT(data->engine);

        if ((data->engine->*functionTemplate).IsEmpty())
            data->engine->*functionTemplate = v8::Persistent<v8::FunctionTemplate>::New(T::createFunctionTemplate(data->engine));

        v8::Handle<v8::ObjectTemplate> instanceTempl = (data->engine->*functionTemplate)->InstanceTemplate();
        v8::Handle<v8::Object> instance = instanceTempl->NewInstance();
        Q_ASSERT(instance->InternalFieldCount() >= 1);
        instance->SetPointerInInternalField(0, data);
        v8::Persistent<v8::Object> persistent = v8::Persistent<v8::Object>::New(instance);
        persistent.MakeWeak(data, QScriptV8ObjectWrapperHelper::weakCallback<T>);
        return handleScope.Close(instance);
    }
};


template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
T* QScriptV8ObjectWrapper<T, functionTemplate>::get(v8::Handle<v8::Object> object)
{
    Q_ASSERT(object->InternalFieldCount() >= 1);
    T *data = reinterpret_cast<T *>(object->GetPointerFromInternalField(0));
    return data;
}

template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
T* QScriptV8ObjectWrapper<T, functionTemplate>::safeGet(const QScriptValuePrivate* p)
{
    // FIXME this algorithm should be shared with the QSEP, as it needs to distinguish between proxy
    // and normal objects.
    // If you need to modify it please update getOwnProperty and other methods checking if an object
    // is a script class instance.
    QScriptEnginePrivate *engine = p->engine();
    if (!engine)
        return 0;
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> value = *p;

    v8::Handle<v8::FunctionTemplate> funcTmpl = engine->*functionTemplate;
    if (!engine->hasInstance(funcTmpl, value))
        return 0;
    v8::Local<v8::Object> object = v8::Object::Cast(*value);
    return get(object);
}

template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
T* QScriptV8ObjectWrapper<T, functionTemplate>::safeGet(v8::Handle<v8::Object> object, QScriptEnginePrivate *engine)
{
    if (!engine || object.IsEmpty())
        return 0;
    v8::HandleScope handleScope;
    v8::Handle<v8::FunctionTemplate> funcTmpl = engine->*functionTemplate;
    if (!engine->hasInstance(funcTmpl, object))
        return 0;
    return get(object);
}

QT_END_NAMESPACE

#endif // QSCRIPTV8OBJECTWRAPPER_P_H
