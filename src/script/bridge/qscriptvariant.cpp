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

#include "qscriptvariant_p.h"

#ifndef QT_NO_SCRIPT

#include "Error.h"
#include "PrototypeFunction.h"
#include "JSString.h"

QT_BEGIN_NAMESPACE

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QScript::QVariantWrapperObject);
ASSERT_CLASS_FITS_IN_CELL(QScript::QVariantPrototype);
}

namespace QScript
{

JSC::UString qtStringToJSCUString(const QString &str);

const JSC::ClassInfo QVariantWrapperObject::info = { "QVariant", 0, 0, 0 };

QVariantWrapperObject::QVariantWrapperObject(WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), data(new Data())
{
}

QVariantWrapperObject::~QVariantWrapperObject()
{
    delete data;
}

static JSC::JSValue JSC_HOST_CALL variantProtoFuncToString(JSC::ExecState *exec, JSC::JSObject*,
                                                           JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QVariantWrapperObject::info))
        return throwError(exec, JSC::TypeError);
    const QVariant &v = static_cast<QVariantWrapperObject*>(JSC::asObject(thisValue))->value();
    // ### check the type
    return JSC::jsString(exec, QScript::qtStringToJSCUString(v.toString()));
}

static JSC::JSValue JSC_HOST_CALL variantProtoFuncValueOf(JSC::ExecState *exec, JSC::JSObject*,
                                                          JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QVariantWrapperObject::info))
        return throwError(exec, JSC::TypeError);
    const QVariant &v = static_cast<QVariantWrapperObject*>(JSC::asObject(thisValue))->value();
    switch (v.type()) {
    case QVariant::Invalid:
        return JSC::jsUndefined();

    case QVariant::String:
        return JSC::jsString(exec, QScript::qtStringToJSCUString(v.toString()));

    case QVariant::Int:
        return JSC::jsNumber(exec, v.toInt());

    case QVariant::Bool:
        return JSC::jsBoolean(v.toBool());

    case QVariant::Double:
        return JSC::jsNumber(exec, v.toDouble());

//    case QVariant::Char:
//        return JSC::jsNumber(exec, v.toChar().unicode());

    case QVariant::UInt:
        return JSC::jsNumber(exec, v.toUInt());
    default:
        ;
    }
    return thisValue;
}

QVariantPrototype::QVariantPrototype(JSC::ExecState* exec, WTF::PassRefPtr<JSC::Structure> structure,
                                     JSC::Structure* prototypeFunctionStructure)
    : QVariantWrapperObject(structure)
{
    setValue(QVariant());

    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, 0, exec->propertyNames().toString, variantProtoFuncToString), JSC::DontEnum);
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, 0, exec->propertyNames().valueOf, variantProtoFuncValueOf), JSC::DontEnum);
}

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
