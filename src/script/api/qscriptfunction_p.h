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

#ifndef QSCRIPTFUNCTION_P_H
#define QSCRIPTFUNCTION_P_H

#include "qscriptengine.h"
#include "qscriptengine_p.h"
#include "qscriptvalue.h"
#include "qscriptvalue_p.h"
#include "qscriptqobject_p.h"

QT_BEGIN_NAMESPACE

struct QScriptNativeFunctionData : public QtDataBase
{
    QScriptNativeFunctionData(QScriptEnginePrivate *engine, QScriptEngine::FunctionSignature fun)
        : QtDataBase(engine)
        , fun(fun)
    {
    }

    QScriptValue call(QScriptContext *scriptContext)
    {
        return fun(scriptContext, QScriptEnginePrivate::get(engine()));
    }

    QScriptEngine::FunctionSignature fun;
};

struct QScriptNativeFunctionWithArgData : public QtDataBase
{
    QScriptNativeFunctionWithArgData(QScriptEnginePrivate *engine, QScriptEngine::FunctionWithArgSignature fun, void *arg)
        : QtDataBase(engine)
        , fun(fun)
        , arg(arg)
    {
    }

    QScriptValue call(QScriptContext *scriptContext)
    {
        return fun(scriptContext, QScriptEnginePrivate::get(engine()), arg);
    }

    QScriptEngine::FunctionWithArgSignature fun;
    void *arg;
};

template <typename T>
void QtNativeFunctionCleanup(v8::Persistent<v8::Value> object, void *parameter)
{
    T *data = reinterpret_cast<T *>(parameter);
    delete data;
    object.Dispose();
    object.Clear();
}

template <typename T>
v8::Handle<v8::Value> QtNativeFunctionCallback(const v8::Arguments& arguments)
{
    v8::HandleScope handleScope;

    v8::Handle<v8::External> wrap = v8::Handle<v8::External>::Cast(arguments.Data());
    T *data = reinterpret_cast<T *>(wrap->Value());

    QScriptEnginePrivate *engine = data->engine();
    QScriptContextPrivate qScriptContext(engine, &arguments);

    // When 'v' gets out of scope, it'll delete 'result'.
    QScriptValue v = data->call(&qScriptContext);
    QScriptValuePrivate *result = QScriptValuePrivate::get(v);

    if (!result->isValid()) {
        return handleScope.Close(engine->makeJSValue(QScriptValue::UndefinedValue));
    }

    // Make sure that the result will be assigned to the correct engine.
    if (!result->engine()) {
        result->assignEngine(engine);
    } else if (result->engine() != engine) {
        qWarning("QScriptValue::call(): Value from different engine returned from native function, returning undefined value instead.");
        return handleScope.Close(engine->makeJSValue(QScriptValue::UndefinedValue));
    }

    // The persistent handle within the 'result' will be deleted, but
    // we let its value escape to the outer scope.
    return handleScope.Close(v8::Handle<v8::Value>(*result));
}

QT_END_NAMESPACE

#endif // QSCRIPTFUNCTION_P_H
