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

#ifndef QSCRIPTFUNCTION_P_H
#define QSCRIPTFUNCTION_P_H

#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalue.h"
#include "qscriptvalue_p.h"

struct QScriptNativeFunctionData
{
    QScriptNativeFunctionData(QScriptEnginePrivate* engine, QScriptEngine::FunctionSignature fun)
        : engine(engine)
        , fun(fun)
    {
    }

    QScriptValue call(QScriptContext *scriptContext) {
        return fun(scriptContext, QScriptEnginePrivate::get(engine));
    }

    QScriptEnginePrivate* engine;
    QScriptEngine::FunctionSignature fun;
};

struct QScriptNativeFunctionWithArgData
{
    QScriptNativeFunctionWithArgData(QScriptEnginePrivate* engine, QScriptEngine::FunctionWithArgSignature fun, void* arg)
        : engine(engine)
        , fun(fun)
        , arg(arg)
    {
    }

    QScriptValue call(QScriptContext *scriptContext) {
        return fun(scriptContext, QScriptEnginePrivate::get(engine), arg);
    }

    QScriptEnginePrivate* engine;
    QScriptEngine::FunctionWithArgSignature fun;
    void* arg;
};

template <typename T>
void QtNativeFunctionCleanup(v8::Persistent<v8::Value> object, void* parameter)
{
    qWarning("Cleanup called");

    T* data = reinterpret_cast<T*>(parameter);
    delete data;

    object.Dispose();
    object.Clear();
}

template <typename T>
v8::Handle<v8::Value> QtNativeFunctionCallback(const v8::Arguments& arguments)
{
    v8::HandleScope handleScope;

    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(arguments.Data());
    T* data = reinterpret_cast<T*>(wrap->Value());

    // TODO: build a QScriptContext and use it in the native call.
    QScriptContext* scriptContext = 0;

    // When v gets out of scope, it'll delete result.
    QScriptValue v = data->call(scriptContext);
    QScriptValuePrivate* result = QScriptValuePrivate::get(v);

    QScriptEnginePrivate* engine = data->engine;
    if (!result->isValid()) {
        qWarning("Invalid value returned from native function, returning undefined value instead.");
        return engine->makeJSValue(QScriptValue::UndefinedValue);
    }

    // Make sure that the result will be assigned to the correct engine.
    if (!result->engine()) {
        result->assignEngine(engine);
    } else if (result->engine() != engine) {
        qWarning("Value from different engine returned from native function, returning undefined value instead.");
        return engine->makeJSValue(QScriptValue::UndefinedValue);
    }

    // Create a new Persistent handle to return since 'result' will be deleted and
    // Dispose() its internal v8::Value.
    // ### Could this be improved by extracting the m_value from 'result' without disposing it?
    return v8::Persistent<v8::Value>::New(*result);
}

#endif // QSCRIPTFUNCTION_P_H
