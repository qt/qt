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

    inline bool isError(const QScriptValuePrivate* value) const;

private:
    bool isType(const QScriptValuePrivate* value, v8::Handle<v8::Object> constructor, v8::Handle<v8::Value> prototype) const;
    inline void initializeMember(v8::Handle<v8::Object> globalObject, v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> type, v8::Persistent<v8::Object>& constructor, v8::Persistent<v8::Value>& prototype);

    // Copy of constructors and prototypes used in isType functions.
    v8::Persistent<v8::Object> m_errorConstructor;
    v8::Persistent<v8::Value> m_errorPrototype;
};

QScriptOriginalGlobalObject::QScriptOriginalGlobalObject(v8::Handle<v8::Context> context)
{
    v8::Context::Scope contextScope(context);
    v8::HandleScope handleScope;
    v8::Local<v8::Object> globalObject = context->Global();
    initializeMember(globalObject, v8::String::New("prototype"), v8::String::New("Error"), m_errorConstructor, m_errorPrototype);
}

inline void QScriptOriginalGlobalObject::initializeMember(v8::Handle<v8::Object> globalObject, v8::Handle<v8::String> prototypeName, v8::Handle<v8::Value> type, v8::Persistent<v8::Object>& constructor, v8::Persistent<v8::Value>& prototype)
{
    // Save references to the Type constructor and prototype.
    v8::Handle<v8::Value> typeConstructor = globalObject->Get(type);
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
}

inline bool QScriptOriginalGlobalObject::isError(const QScriptValuePrivate* value) const
{
    return isType(value, m_errorConstructor, m_errorPrototype);
}

QT_END_NAMESPACE

#endif
