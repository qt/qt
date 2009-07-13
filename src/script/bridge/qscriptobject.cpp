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

#include "qscriptobject_p.h"

#ifndef QT_NO_SCRIPT

QT_BEGIN_NAMESPACE

namespace JSC
{
ASSERT_CLASS_FITS_IN_CELL(QScriptObject);
ASSERT_CLASS_FITS_IN_CELL(QScriptObjectPrototype);
}

// masquerading as JSC::JSObject
const JSC::ClassInfo QScriptObject::info = { "Object", 0, 0, 0 };

QScriptObject::Data::~Data()
{
    delete delegate;
}

QScriptObject::QScriptObject(WTF::PassRefPtr<JSC::Structure> sid)
    : JSC::JSObject(sid), d(0)
{
}

QScriptObject::~QScriptObject()
{
    delete d;
}
    
JSC::JSValue QScriptObject::data() const
{
    if (!d)
        return JSC::JSValue();
    return d->data;
}

void QScriptObject::setData(JSC::JSValue data)
{
    if (!d)
        d = new Data();
    d->data = data;
}

QScriptObjectDelegate *QScriptObject::delegate() const
{
    if (!d)
        return 0;
    return d->delegate;
}

void QScriptObject::setDelegate(QScriptObjectDelegate *delegate)
{
    if (!d)
        d = new Data();
    d->delegate = delegate;
}

bool QScriptObject::getOwnPropertySlot(JSC::ExecState* exec,
                                       const JSC::Identifier& propertyName,
                                       JSC::PropertySlot& slot)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
    return d->delegate->getOwnPropertySlot(this, exec, propertyName, slot);
}

void QScriptObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                        JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (!d || !d->delegate) {
        JSC::JSObject::put(exec, propertyName, value, slot);
        return;
    }
    d->delegate->put(this, exec, propertyName, value, slot);
}

bool QScriptObject::deleteProperty(JSC::ExecState* exec,
                                   const JSC::Identifier& propertyName)
{
    if (!d || !d->delegate)
        return JSC::JSObject::deleteProperty(exec, propertyName);
    return d->delegate->deleteProperty(this, exec, propertyName);
}

bool QScriptObject::getPropertyAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName,
                                          unsigned& attributes) const
{
    if (!d || !d->delegate)
        return JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
    return d->delegate->getPropertyAttributes(this, exec, propertyName, attributes);
}

void QScriptObject::getPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames)
{
    if (!d || !d->delegate) {
        JSC::JSObject::getPropertyNames(exec, propertyNames);
        return;
    }
    d->delegate->getPropertyNames(this, exec, propertyNames);
}

void QScriptObject::mark()
{
    Q_ASSERT(!marked());
    if (d && d->data && !d->data.marked())
        d->data.mark();
    if (!d || !d->delegate) {
        JSC::JSObject::mark();
        return;
    }
    d->delegate->mark(this);
}

JSC::CallType QScriptObject::getCallData(JSC::CallData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getCallData(data);
    return d->delegate->getCallData(this, data);
}

JSC::ConstructType QScriptObject::getConstructData(JSC::ConstructData &data)
{
    if (!d || !d->delegate)
        return JSC::JSObject::getConstructData(data);
    return d->delegate->getConstructData(this, data);
}

bool QScriptObject::hasInstance(JSC::ExecState* exec, JSC::JSValue value, JSC::JSValue proto)
{
    if (!d || !d->delegate)
        return JSC::JSObject::hasInstance(exec, value, proto);
    return d->delegate->hasInstance(this, exec, value, proto);
}

JSC::JSValue QScriptObject::lookupGetter(JSC::ExecState*, const JSC::Identifier& propertyName)
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
}

JSC::JSValue QScriptObject::lookupSetter(JSC::ExecState*, const JSC::Identifier& propertyName)
{
    Q_ASSERT_X(false, Q_FUNC_INFO, "implement me");
}

QScriptObjectPrototype::QScriptObjectPrototype(JSC::ExecState*, WTF::PassRefPtr<JSC::Structure> structure,
                                               JSC::Structure* /*prototypeFunctionStructure*/)
    : QScriptObject(structure)
{
}

QScriptObjectDelegate::QScriptObjectDelegate()
{
}

QScriptObjectDelegate::~QScriptObjectDelegate()
{
}

bool QScriptObjectDelegate::getOwnPropertySlot(QScriptObject* object, JSC::ExecState* exec,
                                               const JSC::Identifier& propertyName,
                                               JSC::PropertySlot& slot)
{
    return object->JSC::JSObject::getOwnPropertySlot(exec, propertyName, slot);
}

void QScriptObjectDelegate::put(QScriptObject* object, JSC::ExecState* exec,
                                const JSC::Identifier& propertyName,
                                JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    object->JSC::JSObject::put(exec, propertyName, value, slot);
}

bool QScriptObjectDelegate::deleteProperty(QScriptObject* object, JSC::ExecState* exec,
                                           const JSC::Identifier& propertyName)
{
    return object->JSC::JSObject::deleteProperty(exec, propertyName);
}

bool QScriptObjectDelegate::getPropertyAttributes(const QScriptObject* object,
                                                  JSC::ExecState* exec,
                                                  const JSC::Identifier& propertyName,
                                                  unsigned& attributes) const
{
    return object->JSC::JSObject::getPropertyAttributes(exec, propertyName, attributes);
}

void QScriptObjectDelegate::getPropertyNames(QScriptObject* object, JSC::ExecState* exec,
                                             JSC::PropertyNameArray& propertyNames)
{
    object->JSC::JSObject::getPropertyNames(exec, propertyNames);
}

void QScriptObjectDelegate::mark(QScriptObject* object)
{
    if (!object->marked())
        object->JSC::JSObject::mark();
}

JSC::CallType QScriptObjectDelegate::getCallData(QScriptObject* object, JSC::CallData& data)
{
    return object->JSC::JSObject::getCallData(data);
}

JSC::ConstructType QScriptObjectDelegate::getConstructData(QScriptObject* object, JSC::ConstructData& data)
{
    return object->JSC::JSObject::getConstructData(data);
}

bool QScriptObjectDelegate::hasInstance(QScriptObject* object, JSC::ExecState* exec,
                                        JSC::JSValue value, JSC::JSValue proto)
{
    return object->JSC::JSObject::hasInstance(exec, value, proto);
}

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT
