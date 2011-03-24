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
    inline void destroy();

    inline v8::Local<v8::Array> getOwnPropertyNames(v8::Handle<v8::Object> object) const;
    inline QScriptValue::PropertyFlags getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property, const QScriptValue::ResolveFlags& mode);
    inline v8::Local<v8::Value> getOwnProperty(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    inline void installArgFunctionOnOrgStringPrototype(v8::Handle<v8::Function> arg);
    inline void defineGetterOrSetter(v8::Handle< v8::Object > recv, v8::Handle< v8::String > prototypeName, v8::Handle< v8::Value > value, uint attribs) const;
    inline v8::Local<v8::Object> getOwnPropertyDescriptor(v8::Handle<v8::Object> object, v8::Handle<v8::Value> property) const;
    inline bool strictlyEquals(v8::Handle<v8::Object> object);
private:
    Q_DISABLE_COPY(QScriptOriginalGlobalObject)
    inline void initializeMember(v8::Local<v8::String> prototypeName, v8::Local<v8::Value> type, v8::Local<v8::Object>& constructor, v8::Local<v8::Value>& prototype);

    v8::HandleScope m_scope;
    // Copy of constructors and prototypes used in isType functions.
    v8::Local<v8::Object> m_stringConstructor;
    v8::Local<v8::Value> m_stringPrototype;
    v8::Local<v8::Function> m_ownPropertyDescriptor;
    v8::Local<v8::Function> m_ownPropertyNames;
    v8::Local<v8::Object> m_globalObject;
    v8::Local<v8::Function> m_defineGetter;
    v8::Local<v8::Function> m_defineSetter;
};

v8::Handle<v8::Value> functionPrint(const v8::Arguments& args);
v8::Handle<v8::Value> functionGC(const v8::Arguments& args);
v8::Handle<v8::Value> functionVersion(const v8::Arguments& args);

QScriptOriginalGlobalObject::QScriptOriginalGlobalObject(const QScriptEnginePrivate *engine, v8::Handle<v8::Context> context)
    : m_scope()
{
    // Please notice that engine is not fully initialized at this point.

    context->Enter(); // Enter the context. We will exit in the QScriptEnginePrivate destructor.
    m_globalObject = context->Global();
    initializeMember(v8::String::New("prototype"), v8::String::New("String"), m_stringConstructor, m_stringPrototype);

    v8::Local<v8::Object> objectConstructor = m_globalObject->Get(v8::String::New("Object"))->ToObject();
    Q_ASSERT(objectConstructor->IsObject());
    {   // Initialize m_ownPropertyDescriptor.
        v8::Local<v8::Value> ownPropertyDescriptor = objectConstructor->Get(v8::String::New("getOwnPropertyDescriptor"));
        Q_ASSERT(!ownPropertyDescriptor.IsEmpty());
        m_ownPropertyDescriptor = v8::Local<v8::Function>::Cast(ownPropertyDescriptor);
    }
    {   // Initialize m_ownPropertyNames.
        v8::Local<v8::Value> ownPropertyNames = objectConstructor->Get(v8::String::New("getOwnPropertyNames"));
        Q_ASSERT(!ownPropertyNames.IsEmpty());
        m_ownPropertyNames = v8::Local<v8::Function>::Cast(ownPropertyNames);
    }
    {
        //initialize m_defineGetter and m_defineSetter
        v8::Local<v8::Value> defineGetter = objectConstructor->Get(v8::String::New("__defineGetter__"));
        v8::Local<v8::Value> defineSetter = objectConstructor->Get(v8::String::New("__defineSetter__"));
        m_defineSetter = v8::Local<v8::Function>::Cast(defineSetter);
        m_defineGetter = v8::Local<v8::Function>::Cast(defineGetter);
    }

    // Set our default properties.
    {
        v8::HandleScope scope;
        v8::Local<v8::Value> eng = v8::External::Wrap(const_cast<QScriptEnginePrivate *>(engine));
        v8::Local<v8::String> nameName = v8::String::New("name");
        v8::Local<v8::String> printName = v8::String::New("print");
        v8::Local<v8::String> gcName = v8::String::New("gc");
        v8::Local<v8::String> versionName = v8::String::New("version");
        v8::Local<v8::Function> builtinFunc;
        builtinFunc = v8::FunctionTemplate::New(functionPrint, eng)->GetFunction();
        builtinFunc->ForceSet(nameName, printName, v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
        m_globalObject->Set(printName, builtinFunc);
        builtinFunc = v8::FunctionTemplate::New(functionGC, eng)->GetFunction();
        builtinFunc->ForceSet(nameName, gcName, v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
        m_globalObject->Set(gcName, builtinFunc);
        builtinFunc = v8::FunctionTemplate::New(functionVersion, eng)->GetFunction();
        builtinFunc->ForceSet(nameName, versionName, v8::PropertyAttribute(v8::ReadOnly | v8::DontEnum | v8::DontDelete));
        m_globalObject->Set(versionName, builtinFunc);
    }
}

inline void QScriptOriginalGlobalObject::initializeMember(v8::Local<v8::String> prototypeName, v8::Local<v8::Value> type, v8::Local<v8::Object>& constructor, v8::Local<v8::Value>& prototype)
{
    // Save references to the Type constructor and prototype.
    v8::Local<v8::Value> typeConstructor = m_globalObject->Get(type);
    Q_ASSERT(typeConstructor->IsObject());
    constructor = typeConstructor->ToObject();

    // Note that this is not the [[Prototype]] internal property (which we could
    // get via Object::GetPrototype), but the Type.prototype, that will be set
    // as [[Prototype]] of Type instances.
    prototype = constructor->Get(prototypeName);
    Q_ASSERT(prototype->IsObject());
}


/*!
    \internal
    QScriptOriginalGlobalObject lives as long as QScriptEnginePrivate that keeps it. In ~QSEP
    the v8 context is removed, so we need to remove our handlers before. to break this dependency
    destroy method should be called before or insight QSEP destructor.
*/
inline void QScriptOriginalGlobalObject::destroy()
{
    m_scope.Close(v8::Handle<v8::Value>());
    // After this line this instance is unusable.
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
    v8::Local<v8::Object> descriptor = getOwnPropertyDescriptor(object, property);
    if (descriptor.IsEmpty()) {
        // Property isn't owned by this object.
        if (!(mode & QScriptValue::ResolvePrototype))
            return 0;
        v8::Local<v8::Value> prototype = object->GetPrototype();
        if (prototype->IsNull())
            return 0;
        return getPropertyFlags(v8::Local<v8::Object>::Cast(prototype), property, QScriptValue::ResolvePrototype);
    }
    v8::Local<v8::String> writableName = v8::String::New("writable");
    v8::Local<v8::String> configurableName = v8::String::New("configurable");
    v8::Local<v8::String> enumerableName = v8::String::New("enumerable");
    v8::Local<v8::String> getName = v8::String::New("get");
    v8::Local<v8::String> setName = v8::String::New("set");

    unsigned flags = 0;

    if (!descriptor->Get(configurableName)->BooleanValue())
        flags |= QScriptValue::Undeletable;
    if (!descriptor->Get(enumerableName)->BooleanValue())
        flags |= QScriptValue::SkipInEnumeration;

    //"writable" is only a property of the descriptor if it is not an accessor
    if(descriptor->Has(writableName)) {
        if (!descriptor->Get(writableName)->BooleanValue())
            flags |= QScriptValue::ReadOnly;
    } else {
        if (descriptor->Get(getName)->IsObject())
            flags |= QScriptValue::PropertyGetter;
        if (descriptor->Get(setName)->IsObject())
            flags |= QScriptValue::PropertySetter;
    }

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
    v8::Local<v8::Object>::Cast(m_stringPrototype)->Set(v8::String::New("arg"), arg);
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

void QScriptOriginalGlobalObject::defineGetterOrSetter(v8::Handle<v8::Object> recv, v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> value, uint attribs) const
{
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> argv[2] = { prototypeName, value };
    if (attribs & QScriptValue::PropertyGetter)
        m_defineGetter->Call(recv, 2, argv);
    if (attribs & QScriptValue::PropertySetter)
        m_defineSetter->Call(recv, 2, argv);
}

inline bool QScriptOriginalGlobalObject::strictlyEquals(v8::Handle<v8::Object> object)
{
    return m_globalObject->GetPrototype()->StrictEquals(object);
}

QT_END_NAMESPACE

#endif
