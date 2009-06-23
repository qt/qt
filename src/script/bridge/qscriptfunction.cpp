/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "qscriptfunction_p.h"

#ifndef QT_NO_SCRIPT

#include "private/qscriptengine_p.h"
#include "qscriptcontext.h"
#include "private/qscriptcontext_p.h"

#include "JSGlobalObject.h"

QT_BEGIN_NAMESPACE

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QScript::FunctionWrapper);
ASSERT_CLASS_FITS_IN_CELL(QScript::FunctionWithArgWrapper);
}

namespace QScript
{

FunctionWrapper::FunctionWrapper(QScriptEngine *engine, int length, const JSC::Identifier &name,
                                   QScriptEngine::FunctionSignature function)
    : JSC::PrototypeFunction(QScriptEnginePrivate::get(engine)->globalObject->globalExec(),
                             length, name, proxyCall), data(new Data())
{
    data->engine = engine;
    data->function = function;
}

FunctionWrapper::~FunctionWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWrapper::proxyCall(JSC::ExecState *, JSC::JSObject *callee,
                                        JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(self->data->engine);
    QScriptContextPrivate ctx_p(callee, thisObject, args,
                                /*calledAsConstructor=*/false, eng_p);
    QScriptContext *ctx = QScriptContextPrivate::create(ctx_p);
    QScriptValue result = self->data->function(ctx, self->data->engine);
    if (!result.isValid())
        result = QScriptValue(QScriptValue::UndefinedValue);
    delete ctx;
    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWrapper::proxyConstruct(JSC::ExecState *, JSC::JSObject *callee,
                                               const JSC::ArgList &args)
{
    FunctionWrapper *self = static_cast<FunctionWrapper*>(callee);
    QScriptValue object = self->data->engine->newObject();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(self->data->engine);
    QScriptContextPrivate ctx_p(callee, eng_p->scriptValueToJSCValue(object),
                                args, /*calledAsConstructor=*/true, eng_p);
    QScriptContext *ctx = QScriptContextPrivate::create(ctx_p);
    QScriptValue result = self->data->function(ctx, self->data->engine);
    if (!result.isValid())
        result = QScriptValue(QScriptValue::UndefinedValue);
    delete ctx;
    if (result.isObject())
        return JSC::asObject(eng_p->scriptValueToJSCValue(result));
    return JSC::asObject(eng_p->scriptValueToJSCValue(object));
}

FunctionWithArgWrapper::FunctionWithArgWrapper(QScriptEngine *engine, int length, const JSC::Identifier &name,
                                                 QScriptEngine::FunctionWithArgSignature function, void *arg)
    : JSC::PrototypeFunction(QScriptEnginePrivate::get(engine)->globalObject->globalExec(),
                             length, name, /*proxyCall*/0), data(new Data())
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
    data->engine = engine;
    data->function = function;
    data->arg = arg;
}

FunctionWithArgWrapper::~FunctionWithArgWrapper()
{
    delete data;
}

JSC::ConstructType FunctionWithArgWrapper::getConstructData(JSC::ConstructData& consData)
{
    consData.native.function = proxyConstruct;
    return JSC::ConstructTypeHost;
}

JSC::JSValue FunctionWithArgWrapper::proxyCall(JSC::ExecState *, JSC::JSObject *callee,
                                               JSC::JSValue thisObject, const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(self->data->engine);
    QScriptContextPrivate ctx_p(callee, thisObject, args,
                                /*calledAsConstructor=*/false, eng_p);
    QScriptContext *ctx = QScriptContextPrivate::create(ctx_p);
    QScriptValue result = self->data->function(ctx, self->data->engine, self->data->arg);
    delete ctx;
    return eng_p->scriptValueToJSCValue(result);
}

JSC::JSObject* FunctionWithArgWrapper::proxyConstruct(JSC::ExecState *, JSC::JSObject *callee,
                                                      const JSC::ArgList &args)
{
    FunctionWithArgWrapper *self = static_cast<FunctionWithArgWrapper*>(callee);
    QScriptValue object = self->data->engine->newObject();
    QScriptEnginePrivate *eng_p = QScriptEnginePrivate::get(self->data->engine);
    QScriptContextPrivate ctx_p(callee, eng_p->scriptValueToJSCValue(object),
                                args, /*calledAsConstructor=*/true, eng_p);
    QScriptContext *ctx = QScriptContextPrivate::create(ctx_p);
    QScriptValue result = self->data->function(ctx, self->data->engine, self->data->arg);
    delete ctx;
    if (result.isObject())
        return static_cast<JSC::JSObject*>(JSC::asObject(eng_p->scriptValueToJSCValue(result)));
    return static_cast<JSC::JSObject*>(JSC::asObject(eng_p->scriptValueToJSCValue(object)));
}

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
