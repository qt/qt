/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "config.h"
#include "qscriptactivationobject_p.h"

#include "JSVariableObject.h"

namespace JSC
{
    ASSERT_CLASS_FITS_IN_CELL(QScript::QScriptActivationObject);
}

QT_BEGIN_NAMESPACE

/*!
\class QScriptActivationObject
\internal

  Represent a scope for native function call.
*/

namespace QScript
{

const JSC::ClassInfo QScriptActivationObject::info = { "QScriptActivationObject", 0, 0, 0 };

QScriptActivationObject::QScriptActivationObject(JSC::ExecState *callFrame, JSC::JSObject *delegate)
    : JSC::JSVariableObject(callFrame->globalData().activationStructure,
                            new QScriptActivationObjectData(callFrame->registers(), delegate))
{
}

QScriptActivationObject::~QScriptActivationObject()
{
    delete d;
}

bool QScriptActivationObject::getOwnPropertySlot(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::PropertySlot& slot)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->getOwnPropertySlot(exec, propertyName, slot);
    return JSC::JSVariableObject::getOwnPropertySlot(exec, propertyName, slot);
}

bool QScriptActivationObject::getPropertyAttributes(JSC::ExecState* exec, const JSC::Identifier& propertyName, unsigned& attributes) const
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->getPropertyAttributes(exec, propertyName, attributes);
    return JSC::JSVariableObject::getPropertyAttributes(exec, propertyName, attributes);
}

void QScriptActivationObject::getPropertyNames(JSC::ExecState* exec, JSC::PropertyNameArray& propertyNames, unsigned listedAttributes)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->getPropertyNames(exec, propertyNames, listedAttributes);
        return;
    }
    return JSC::JSVariableObject::getPropertyNames(exec, propertyNames, listedAttributes);
}

void QScriptActivationObject::putWithAttributes(JSC::ExecState *exec, const JSC::Identifier &propertyName, JSC::JSValue value, unsigned attributes)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->putWithAttributes(exec, propertyName, value, attributes);
        return;
    }

    if (symbolTablePutWithAttributes(propertyName, value, attributes))
        return;
    
    JSC::PutPropertySlot slot;
    JSObject::putWithAttributes(exec, propertyName, value, attributes, true, slot);
}

void QScriptActivationObject::put(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSValue value, JSC::PutPropertySlot& slot)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->put(exec, propertyName, value, slot);
        return;
    }
    JSC::JSVariableObject::put(exec, propertyName, value, slot);
}

void QScriptActivationObject::put(JSC::ExecState* exec, unsigned propertyName, JSC::JSValue value)
{
    if (d_ptr()->delegate != 0) {
        d_ptr()->delegate->put(exec, propertyName, value);
        return;
    }
    JSC::JSVariableObject::put(exec, propertyName, value);
}

bool QScriptActivationObject::deleteProperty(JSC::ExecState* exec, const JSC::Identifier& propertyName, bool checkDontDelete)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->deleteProperty(exec, propertyName, checkDontDelete);
    return JSC::JSVariableObject::deleteProperty(exec, propertyName, checkDontDelete);
}

void QScriptActivationObject::defineGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* getterFunction)
{
    if (d_ptr()->delegate != 0)
        d_ptr()->delegate->defineGetter(exec, propertyName, getterFunction);
    else
        JSC::JSVariableObject::defineGetter(exec, propertyName, getterFunction);
}

void QScriptActivationObject::defineSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName, JSC::JSObject* setterFunction)
{
    if (d_ptr()->delegate != 0)
        d_ptr()->delegate->defineSetter(exec, propertyName, setterFunction);
    else
        JSC::JSVariableObject::defineSetter(exec, propertyName, setterFunction);
}

JSC::JSValue QScriptActivationObject::lookupGetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->lookupGetter(exec, propertyName);
    return JSC::JSVariableObject::lookupGetter(exec, propertyName);
}

JSC::JSValue QScriptActivationObject::lookupSetter(JSC::ExecState* exec, const JSC::Identifier& propertyName)
{
    if (d_ptr()->delegate != 0)
        return d_ptr()->delegate->lookupSetter(exec, propertyName);
    return JSC::JSVariableObject::lookupSetter(exec, propertyName);
}

} // namespace QScript

QT_END_NAMESPACE

