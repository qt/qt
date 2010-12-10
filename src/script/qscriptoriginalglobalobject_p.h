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

#ifndef QSCRIPTORIGINALGLOBALOBJECT_P_H
#define QSCRIPTORIGINALGLOBALOBJECT_P_H

#include "QtCore/qglobal.h"
#include "qscriptvalue.h"
#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptValuePrivate;

/*!
    \internal
    This class is a workaround for missing V8 API functionality. This class keeps all important
    properties of an original (default) global object, so we can use it even if the global object was
    changed.

    FIXME this class is a container for workarounds :-) it should be replaced by proper API calls.

    The class have to be created on the QScriptEnginePrivate creation time (before any change got applied to
    global object).

    \attention All methods (apart from constructor) assumes that a context and a scope are prepared correctly.
*/
class QScriptOriginalGlobalObject
{
public:
    inline QScriptOriginalGlobalObject(const QScriptEnginePrivate *engine, v8::Handle<v8::Context> context);
    inline ~QScriptOriginalGlobalObject();
    inline void destroy();

    inline v8::Local<v8::Array> getOwnPropertyNames(v8::Handle<v8::Object> object) const;
    inline QScriptValue::PropertyFlags getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode);
    inline v8::Local<v8::Value> getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    inline void installArgFunctionOnOrgStringPrototype(v8::Handle<v8::Function> arg);
private:
    Q_DISABLE_COPY(QScriptOriginalGlobalObject)
    inline v8::Local<v8::Object> getOwnPropertyDescriptor(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    inline void initializeMember(v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> type, v8::Persistent<v8::Object>& constructor, v8::Persistent<v8::Value>& prototype);

    // Copy of constructors and prototypes used in isType functions.
    v8::Persistent<v8::Object> m_stringConstructor;
    v8::Persistent<v8::Value> m_stringPrototype;
    v8::Persistent<v8::Function> m_ownPropertyDescriptor;
    v8::Persistent<v8::Function> m_ownPropertyNames;
    v8::Persistent<v8::Object> m_globalObject;
};

v8::Handle<v8::Value> functionPrint(const v8::Arguments& args);
v8::Handle<v8::Value> functionGC(const v8::Arguments& args);
v8::Handle<v8::Value> functionVersion(const v8::Arguments& args);

QScriptOriginalGlobalObject::QScriptOriginalGlobalObject(const QScriptEnginePrivate *engine, v8::Handle<v8::Context> context)
{
    // Please notice that engine is not fully initialized at this point.

    context->Enter(); // Enter the context. We will exit in the QScriptEnginePrivate destructor.
    v8::HandleScope handleScope;
    m_globalObject = v8::Persistent<v8::Object>::New(context->Global());
    initializeMember(v8::String::New("prototype"), v8::String::New("String"), m_stringConstructor, m_stringPrototype);

    v8::Handle<v8::Value> objectConstructor = m_globalObject->Get(v8::String::New("Object"));
    Q_ASSERT(objectConstructor->IsObject());
    {   // Initialize m_ownPropertyDescriptor.
        v8::Handle<v8::Value> ownPropertyDescriptor = objectConstructor->ToObject()->Get(v8::String::New("getOwnPropertyDescriptor"));
        Q_ASSERT(!ownPropertyDescriptor.IsEmpty());
        m_ownPropertyDescriptor = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(ownPropertyDescriptor));
    }
    {   // Initialize m_ownPropertyNames.
        v8::Handle<v8::Value> ownPropertyNames = objectConstructor->ToObject()->Get(v8::String::New("getOwnPropertyNames"));
        Q_ASSERT(!ownPropertyNames.IsEmpty());
        m_ownPropertyNames= v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(ownPropertyNames));
    }

    // Set our default properties.
    v8::Local<v8::Value> eng = v8::External::Wrap(const_cast<QScriptEnginePrivate *>(engine));
    v8::Local<v8::Function> builtinFunc;
    builtinFunc = v8::FunctionTemplate::New(functionPrint, eng)->GetFunction();
    builtinFunc->ForceSet(v8::String::New("name"), v8::String::New("print"), v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
    m_globalObject->Set(v8::String::New("print") , builtinFunc);
    builtinFunc = v8::FunctionTemplate::New(functionGC, eng)->GetFunction();
    builtinFunc->ForceSet(v8::String::New("name"), v8::String::New("gc"), v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
    m_globalObject->Set(v8::String::New("gc") , builtinFunc);
    builtinFunc = v8::FunctionTemplate::New(functionVersion, eng)->GetFunction();
    builtinFunc->ForceSet(v8::String::New("name"), v8::String::New("version"), v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
    m_globalObject->Set(v8::String::New("version") , builtinFunc);
}

inline void QScriptOriginalGlobalObject::initializeMember(v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> type, v8::Persistent<v8::Object>& constructor, v8::Persistent<v8::Value>& prototype)
{
    // Save references to the Type constructor and prototype.
    v8::Handle<v8::Value> typeConstructor = m_globalObject->Get(type);
    Q_ASSERT(typeConstructor->IsObject());
    constructor = v8::Persistent<v8::Object>::New(typeConstructor->ToObject());

    // Note that this is not the [[Prototype]] internal property (which we could
    // get via Object::GetPrototype), but the Type.prototype, that will be set
    // as [[Prototype]] of Type instances.
    prototype = v8::Persistent<v8::Value>::New(constructor->Get(prototypeName));
    Q_ASSERT(prototype->IsObject());
}

QScriptOriginalGlobalObject::~QScriptOriginalGlobalObject()
{
    // QScriptOriginalGlobalObject live as long as QScriptEnginePrivate that keeps it. In ~QSEP
    // the v8 context is removed, so we need to remove our handlers before. It is why destroy method
    // should be called before destructor.
    Q_ASSERT_X(m_stringConstructor.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_stringPrototype.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_ownPropertyDescriptor.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_ownPropertyNames.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_globalObject.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
}

inline void QScriptOriginalGlobalObject::destroy()
{
    m_stringConstructor.Dispose();
    m_stringPrototype.Dispose();
    m_ownPropertyNames.Dispose();
    m_ownPropertyDescriptor.Dispose();
    m_globalObject.Dispose();
#ifndef QT_NO_DEBUG
    m_stringConstructor.Clear();
    m_stringPrototype.Clear();
    m_ownPropertyNames.Clear();
    m_ownPropertyDescriptor.Clear();
    m_globalObject.Clear();
#endif
}

inline v8::Local<v8::Array> QScriptOriginalGlobalObject::getOwnPropertyNames(v8::Handle<v8::Object> object) const
{
    Q_ASSERT(!object.IsEmpty());
    v8::Handle<v8::Value> argv[] = {object};
    return v8::Local<v8::Array>::Cast(m_ownPropertyNames->Call(m_globalObject, /* argc */ 1, argv));
}

inline QScriptValue::PropertyFlags QScriptOriginalGlobalObject::getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode)
{
    Q_ASSERT(object->IsObject());
    Q_ASSERT(!property.IsEmpty());
    v8::Handle<v8::Object> descriptor = getOwnPropertyDescriptor(object, property);
    if (descriptor.IsEmpty()) {
        // Property isn't owned by this object.
        if (!(mode & QScriptValue::ResolvePrototype))
            return 0;
        v8::Handle<v8::Value> prototype = object->GetPrototype();
        if (prototype->IsNull())
            return 0;
        return getPropertyFlags(v8::Handle<v8::Object>::Cast(prototype), property, QScriptValue::ResolvePrototype);
    }
    v8::Local<v8::String> writableName = v8::String::New("writable");
    v8::Local<v8::String> configurableName = v8::String::New("configurable");
    v8::Local<v8::String> enumerableName = v8::String::New("enumerable");
    v8::Local<v8::String> getName = v8::String::New("get");
    v8::Local<v8::String> setName = v8::String::New("set");

    unsigned flags = 0;

    if (!descriptor->Get(writableName)->BooleanValue())
        flags |= QScriptValue::ReadOnly;
    if (!descriptor->Get(configurableName)->BooleanValue())
        flags |= QScriptValue::Undeletable;
    if (!descriptor->Get(enumerableName)->BooleanValue())
        flags |= QScriptValue::SkipInEnumeration;
    if (descriptor->Get(getName)->IsObject())
        flags |= QScriptValue::PropertyGetter;
    if (descriptor->Get(setName)->IsObject())
        flags |= QScriptValue::PropertySetter;

    if (flags & (QScriptValue::PropertySetter | QScriptValue::PropertyGetter))
        flags &= ~QScriptValue::ReadOnly; //that flag is wrongly added by V8

    return QScriptValue::PropertyFlag(flags);
}

inline v8::Local<v8::Value> QScriptOriginalGlobalObject::getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const
{
    Q_ASSERT(object->IsObject());
    Q_ASSERT(!property.IsEmpty());

    // FIXME potentially it is slow, as we need to create property descriptor just to check if a property exists.
    v8::Local<v8::Object> descriptor = getOwnPropertyDescriptor(object, property);
    if (descriptor.IsEmpty())
        return descriptor;
    return object->Get(property);
}

inline void QScriptOriginalGlobalObject::installArgFunctionOnOrgStringPrototype(v8::Handle<v8::Function> arg)
{
    v8::Handle<v8::Object>::Cast(m_stringPrototype)->Set(v8::String::New("arg"), arg);
}

inline v8::Local<v8::Object> QScriptOriginalGlobalObject::getOwnPropertyDescriptor(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const
{
    Q_ASSERT(object->IsObject());
    Q_ASSERT(!property.IsEmpty());
    // FIXME do we need try catch here?
    v8::Handle<v8::Value> argv[] = {object, property};
    v8::Local<v8::Object> descriptor = v8::Local<v8::Object>::Cast(m_ownPropertyDescriptor->Call(m_globalObject, /* argc */ 2, argv));
    if (descriptor.IsEmpty() || !descriptor->IsObject())
        return v8::Local<v8::Object>();
    return descriptor;
}

QT_END_NAMESPACE

#endif
