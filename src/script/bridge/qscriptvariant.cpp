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
ASSERT_CLASS_FITS_IN_CELL(QScript::QVariantPrototype);
}

namespace QScript
{

JSC::UString qtStringToJSCUString(const QString &str);

QVariantDelegate::QVariantDelegate(const QVariant &value)
    : m_value(value)
{
}

QVariantDelegate::~QVariantDelegate()
{
}

QVariant &QVariantDelegate::value()
{
    return m_value;
}

void QVariantDelegate::setValue(const QVariant &value)
{
    m_value = value;
}

QScriptObjectDelegate::Type QVariantDelegate::type() const
{
    return Variant;
}

static JSC::JSValue JSC_HOST_CALL variantProtoFuncToString(JSC::ExecState *exec, JSC::JSObject*,
                                                           JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QScriptObject::info))
        return throwError(exec, JSC::TypeError, "This object is not a QVariant");
    QScriptObjectDelegate *delegate = static_cast<QScriptObject*>(JSC::asObject(thisValue))->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::Variant))
        return throwError(exec, JSC::TypeError, "This object is not a QVariant");
    const QVariant &v = static_cast<QVariantDelegate*>(delegate)->value();
    // ### call valueOf()
    return JSC::jsString(exec, QScript::qtStringToJSCUString(v.toString()));
}

static JSC::JSValue JSC_HOST_CALL variantProtoFuncValueOf(JSC::ExecState *exec, JSC::JSObject*,
                                                          JSC::JSValue thisValue, const JSC::ArgList&)
{
    if (!thisValue.isObject(&QScriptObject::info))
        return throwError(exec, JSC::TypeError);
    QScriptObjectDelegate *delegate = static_cast<QScriptObject*>(JSC::asObject(thisValue))->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::Variant))
        return throwError(exec, JSC::TypeError);
    const QVariant &v = static_cast<QVariantDelegate*>(delegate)->value();
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
    : QScriptObject(structure)
{
    setDelegate(new QVariantDelegate(QVariant()));

    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, 0, exec->propertyNames().toString, variantProtoFuncToString), JSC::DontEnum);
    putDirectFunction(exec, new (exec) JSC::PrototypeFunction(exec, prototypeFunctionStructure, 0, exec->propertyNames().valueOf, variantProtoFuncValueOf), JSC::DontEnum);
}

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
