/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptdeclarativeclass_p.h"
#include "qscriptdeclarativeobject_p.h"
#include "qscriptobject_p.h"
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <private/qscriptengine_p.h>
#include <private/qscriptvalue_p.h>
#include <private/qscriptqobject_p.h>
#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier()
{
    new (&d) JSC::Identifier();
}

QScriptDeclarativeClass::PersistentIdentifier::~PersistentIdentifier()
{
    ((JSC::Identifier &)d).JSC::Identifier::~Identifier();
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier(const PersistentIdentifier &other)
{
    identifier = other.identifier;
    new (&d) JSC::Identifier((JSC::Identifier &)(other.d));
}

QScriptDeclarativeClass::PersistentIdentifier &
QScriptDeclarativeClass::PersistentIdentifier::operator=(const PersistentIdentifier &other)
{
    identifier = other.identifier;
    ((JSC::Identifier &)d) = (JSC::Identifier &)(other.d);
    return *this;
}

class QScriptDeclarativeClassPrivate
{
public:
    QScriptDeclarativeClassPrivate() {}

    QScriptEngine *engine;
    QScriptDeclarativeClass *q_ptr;
};

QScriptDeclarativeClass::QScriptDeclarativeClass(QScriptEngine *engine)
: d_ptr(new QScriptDeclarativeClassPrivate)
{
    Q_ASSERT(sizeof(void*) == sizeof(JSC::Identifier));
    d_ptr->q_ptr = this;
    d_ptr->engine = engine;
}

QScriptValue QScriptDeclarativeClass::newObject(QScriptEngine *engine, 
                                                QScriptDeclarativeClass *scriptClass, 
                                                Object *object)
{
    Q_ASSERT(engine);
    Q_ASSERT(scriptClass);

    QScriptEnginePrivate *p = static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(engine)); 

    JSC::ExecState* exec = p->currentFrame;
    QScriptObject *result = new (exec) QScriptObject(p->scriptObjectStructure);
    result->setDelegate(new QScript::DeclarativeObjectDelegate(scriptClass, object));
    return p->scriptValueFromJSCValue(result);
}

QScriptDeclarativeClass *QScriptDeclarativeClass::scriptClass(const QScriptValue &v)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);
    if (!d || !d->isJSC() || !d->jscValue.isObject(&QScriptObject::info))
        return 0;
    QScriptObject *scriptObject = static_cast<QScriptObject*>(JSC::asObject(d->jscValue));
    QScriptObjectDelegate *delegate = scriptObject->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::DeclarativeClassObject))
        return 0;
    return static_cast<QScript::DeclarativeObjectDelegate*>(delegate)->scriptClass();
}

QScriptDeclarativeClass::Object *QScriptDeclarativeClass::object(const QScriptValue &v)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);
    if (!d || !d->isJSC() || !d->jscValue.isObject(&QScriptObject::info))
        return 0;
    QScriptObject *scriptObject = static_cast<QScriptObject*>(JSC::asObject(d->jscValue));
    QScriptObjectDelegate *delegate = scriptObject->delegate();
    if (!delegate || (delegate->type() != QScriptObjectDelegate::DeclarativeClassObject))
        return 0;
    return static_cast<QScript::DeclarativeObjectDelegate*>(delegate)->object();
}

QScriptValue QScriptDeclarativeClass::function(const QScriptValue &v, const Identifier &name)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);

    if (!d->isObject())
        return QScriptValue();

    JSC::ExecState *exec = d->engine->currentFrame;
    JSC::JSObject *object = d->jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;

    JSC::Identifier id(exec, (JSC::UString::Rep *)name);

    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
        if (QScript::isFunction(result))
            return d->engine->scriptValueFromJSCValue(result);
    }

    return QScriptValue();
}

QScriptValue QScriptDeclarativeClass::property(const QScriptValue &v, const Identifier &name)
{
    QScriptValuePrivate *d = QScriptValuePrivate::get(v);

    if (!d->isObject())
        return QScriptValue();

    JSC::ExecState *exec = d->engine->currentFrame;
    JSC::JSObject *object = d->jscValue.getObject();
    JSC::PropertySlot slot(const_cast<JSC::JSObject*>(object));
    JSC::JSValue result;

    JSC::Identifier id(exec, (JSC::UString::Rep *)name);

    if (const_cast<JSC::JSObject*>(object)->getOwnPropertySlot(exec, id, slot)) {
        result = slot.getValue(exec, id);
        return d->engine->scriptValueFromJSCValue(result);
    }

    return QScriptValue();
}

QScriptDeclarativeClass::~QScriptDeclarativeClass()
{
}

QScriptEngine *QScriptDeclarativeClass::engine() const
{
    return d_ptr->engine;
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const QString &str)
{
    QScriptEnginePrivate *p = 
        static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(d_ptr->engine)); 
    JSC::ExecState* exec = p->currentFrame;

    PersistentIdentifier rv(true);
    new (&rv.d) JSC::Identifier(exec, (UChar *)str.constData(), str.size());
    rv.identifier = (void *)((JSC::Identifier &)rv.d).ustring().rep();
    return rv;
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const Identifier &id)
{
    QScriptEnginePrivate *p = 
        static_cast<QScriptEnginePrivate *>(QObjectPrivate::get(d_ptr->engine)); 
    JSC::ExecState* exec = p->currentFrame;

    PersistentIdentifier rv(true);
    new (&rv.d) JSC::Identifier(exec, (JSC::UString::Rep *)id);
    rv.identifier = (void *)((JSC::Identifier &)rv.d).ustring().rep();
    return rv;
}

QString QScriptDeclarativeClass::toString(const Identifier &identifier)
{
    JSC::UString::Rep *r = (JSC::UString::Rep *)identifier;
    return QString((QChar *)r->data(), r->size());
}

QScriptClass::QueryFlags 
QScriptDeclarativeClass::queryProperty(Object *object, const Identifier &name, 
                                       QScriptClass::QueryFlags flags)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(flags);
    return 0;
}

QScriptValue QScriptDeclarativeClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    return QScriptValue();
}

void QScriptDeclarativeClass::setProperty(Object *object, const Identifier &name, 
                                          const QScriptValue &value)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    Q_UNUSED(value);
}

QScriptValue::PropertyFlags 
QScriptDeclarativeClass::propertyFlags(Object *object, const Identifier &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    return 0;
}

QStringList QScriptDeclarativeClass::propertyNames(Object *object)
{
    Q_UNUSED(object);
    return QStringList();
}

QObject *QScriptDeclarativeClass::toQObject(Object *, bool *ok)
{
    if (ok) *ok = false;
    return 0;
}

QVariant QScriptDeclarativeClass::toVariant(Object *, bool *ok)
{
    if (ok) *ok = false;
    return QVariant();
}

