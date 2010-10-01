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
    inline QScriptOriginalGlobalObject(v8::Handle<v8::Context> context);
    inline ~QScriptOriginalGlobalObject();
    inline void destroy();

    inline bool isError(const QScriptValuePrivate* value) const;
    inline bool isInvalid(v8::Handle<v8::Value> value) const;
    inline QScriptValue::PropertyFlags getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::String> property, const QScriptValue::ResolveFlags& mode);
    inline void installArgFunctionOnOrgStringPrototype(v8::Handle<v8::Function> arg);

    inline v8::Handle<v8::Value> invalid() const;
private:
    bool isType(const QScriptValuePrivate* value, v8::Handle<v8::Object> constructor, v8::Handle<v8::Value> prototype) const;
    inline void initializeMember(v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> type, v8::Persistent<v8::Object>& constructor, v8::Persistent<v8::Value>& prototype);

    // Copy of constructors and prototypes used in isType functions.
    v8::Persistent<v8::Object> m_errorConstructor;
    v8::Persistent<v8::Value> m_errorPrototype;
    v8::Persistent<v8::Object> m_stringConstructor;
    v8::Persistent<v8::Value> m_stringPrototype;
    v8::Persistent<v8::Function> m_ownPropertyDescriptor;
    v8::Persistent<v8::Object> m_globalObject;
    v8::Persistent<v8::Value> m_invalidValue;
};

QScriptOriginalGlobalObject::QScriptOriginalGlobalObject(v8::Handle<v8::Context> context)
{
    v8::Context::Scope contextScope(context);
    v8::HandleScope handleScope;
    m_globalObject = v8::Persistent<v8::Object>::New(context->Global());
    initializeMember(v8::String::New("prototype"), v8::String::New("Error"), m_errorConstructor, m_errorPrototype);
    initializeMember(v8::String::New("prototype"), v8::String::New("String"), m_stringConstructor, m_stringPrototype);

    {   // Initialize m_ownPropertyDescriptor.
        v8::Handle<v8::Value> objectConstructor = m_globalObject->Get(v8::String::New("Object"));
        Q_ASSERT(objectConstructor->IsObject());
        v8::Handle<v8::Value> ownPropertyDescriptor = objectConstructor->ToObject()->Get(v8::String::New("getOwnPropertyDescriptor"));
        Q_ASSERT(!ownPropertyDescriptor.IsEmpty());
        m_ownPropertyDescriptor = v8::Persistent<v8::Function>::New(v8::Handle<v8::Function>::Cast(ownPropertyDescriptor));
    }
    m_invalidValue = v8::Persistent<v8::Value>::New(v8::Object::New());
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
    Q_ASSERT_X(m_errorConstructor.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_errorPrototype.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_stringConstructor.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_stringPrototype.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_ownPropertyDescriptor.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_globalObject.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
    Q_ASSERT_X(m_invalidValue.IsEmpty(), Q_FUNC_INFO, "QScriptOriginalGlobalObject should be destroyed before context");
}

inline void QScriptOriginalGlobalObject::destroy()
{
    m_errorConstructor.Dispose();
    m_errorPrototype.Dispose();
    m_stringConstructor.Dispose();
    m_stringPrototype.Dispose();
    m_ownPropertyDescriptor.Dispose();
    m_globalObject.Dispose();
    m_invalidValue.Dispose();
#ifndef QT_NO_DEBUG
    m_errorConstructor.Clear();
    m_errorPrototype.Clear();
    m_stringConstructor.Clear();
    m_stringPrototype.Clear();
    m_ownPropertyDescriptor.Clear();
    m_globalObject.Clear();
    m_invalidValue.Clear();
#endif
}

inline bool QScriptOriginalGlobalObject::isError(const QScriptValuePrivate* value) const
{
    return isType(value, m_errorConstructor, m_errorPrototype);
}

inline bool QScriptOriginalGlobalObject::isInvalid(v8::Handle<v8::Value> value) const
{
    return m_invalidValue->StrictEquals(value);
}

inline QScriptValue::PropertyFlags QScriptOriginalGlobalObject::getPropertyFlags(v8::Handle<v8::Object> object, v8::Handle<v8::String> property, const QScriptValue::ResolveFlags& mode)
{
    Q_ASSERT(object->IsObject());
    Q_ASSERT(!property.IsEmpty());
    // FIXME do we need try catch here?
    v8::Handle<v8::Value> argv[] = {object, property};
    v8::Handle<v8::Object> descriptor = v8::Handle<v8::Object>::Cast(m_ownPropertyDescriptor->Call(m_globalObject, /* argc */ 2, argv));
    if (descriptor.IsEmpty()) {
        // exception ?
        return QScriptValue::PropertyFlag(0);
    }
    if (!descriptor->IsObject()) {
        // Property isn't owned by this object.
        v8::Handle<v8::Value> prototype = object->GetPrototype();
        if (mode == QScriptValue::ResolveLocal || prototype->IsNull())
            return QScriptValue::PropertyFlag(0);
        return getPropertyFlags(v8::Handle<v8::Object>::Cast(prototype), property, QScriptValue::ResolvePrototype);
    }
    v8::Local<v8::String> writableName = v8::String::New("writable");
    v8::Local<v8::String> configurableName = v8::String::New("configurable");
    v8::Local<v8::String> enumerableName = v8::String::New("enumerable");

    unsigned flags = 0;

    if (!descriptor->Get(writableName)->BooleanValue())
        flags |= QScriptValue::ReadOnly;
    if (!descriptor->Get(configurableName)->BooleanValue())
        flags |= QScriptValue::Undeletable;
    if (!descriptor->Get(enumerableName)->BooleanValue())
        flags |= QScriptValue::SkipInEnumeration;

    return QScriptValue::PropertyFlag(flags);
}

inline void QScriptOriginalGlobalObject::installArgFunctionOnOrgStringPrototype(v8::Handle<v8::Function> arg)
{
    v8::Handle<v8::Object>::Cast(m_stringPrototype)->Set(v8::String::New("arg"), arg);
}

inline v8::Handle<v8::Value> QScriptOriginalGlobalObject::invalid() const
{
    return m_invalidValue;
}

QT_END_NAMESPACE

#endif
