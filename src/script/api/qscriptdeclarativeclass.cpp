/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qscriptdeclarativeclass_p.h"
#include "qscriptcontext_p.h"
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptengineagent.h>
#include <private/qscriptengine_p.h>
#include <private/qscriptvalue_p.h>
#include <private/qscriptqobject_p.h>
#include <QtCore/qstringlist.h>
#include "qscriptdeclarativeclassobject_p.h"
#include "qscriptisolate_p.h"
#include "qscript_impl_p.h"

QT_BEGIN_NAMESPACE

/*!
\class QScriptDeclarativeClass::Value
\internal
\brief The QScriptDeclarativeClass::Value class acts as a container for JavaScript data types.

QScriptDeclarativeClass::Value class is similar to QScriptValue, but it is slightly faster.  
Unlike QScriptValue, however, Value instances cannot be stored as they may not survive garbage 
collection.  If you need to store a Value, convert it to a QScriptValue and store that.
*/

QScriptDeclarativeClass::Value::Value()
{
}

QScriptDeclarativeClass::Value::Value(const Value &other)
    : value(other.value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, int value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, uint value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, bool value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, double value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, float value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, const QString &value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptContext *, const QScriptValue &value)
    :value(value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, int value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, uint value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, bool value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, double value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, float value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *engine, const QString &value)
    :value(engine, value) { }

QScriptDeclarativeClass::Value::Value(QScriptEngine *, const QScriptValue &value)
    :value(value) { }

QScriptDeclarativeClass::Value::~Value()
{
}

QScriptValue QScriptDeclarativeClass::Value::toScriptValue(QScriptEngine *engine) const
{
    return value;
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier()
    : identifier(0)
{
}

QScriptDeclarativeClass::PersistentIdentifier::~PersistentIdentifier()
{
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier(const PersistentIdentifier &other)
    : identifier(other.identifier), str(other.str)
{
}

QScriptDeclarativeClass::PersistentIdentifier &
QScriptDeclarativeClass::PersistentIdentifier::operator=(const PersistentIdentifier &other)
{
    identifier = other.identifier;
    str = other.str;
    return *this;
}

QScriptDeclarativeClass::QScriptDeclarativeClass(QScriptEngine *engine)
: d_ptr(new QScriptDeclarativeClassPrivate)
{
    d_ptr->q_ptr = this;
    d_ptr->engine = engine;
}

QScriptValue QScriptDeclarativeClass::newObject(QScriptEngine *engine, 
                                                QScriptDeclarativeClass *scriptClass, 
                                                Object *object)
{
    Q_ASSERT(engine);
    QScriptEnginePrivate *engine_p = QScriptEnginePrivate::get(engine);
    QScriptIsolate api(engine_p);
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> result = QScriptDeclarativeClassObject::newInstance(engine_p, scriptClass, object);
    return QScriptValuePrivate::get(new QScriptValuePrivate(engine_p, result));
}

QScriptDeclarativeClass::Value 
QScriptDeclarativeClass::newObjectValue(QScriptEngine *engine, 
                                        QScriptDeclarativeClass *scriptClass, 
                                        Object *object)
{
    Q_ASSERT(engine);
    Q_UNUSED(scriptClass)
    return Value(engine, newObject(engine, scriptClass, object));
}

QScriptDeclarativeClass *QScriptDeclarativeClass::scriptClass(const QScriptValue &value)
{
    QScriptValuePrivate *v = QScriptValuePrivate::get(value);
    QScriptIsolate api(v->engine());
    return QScriptDeclarativeClassObject::declarativeClass(v);
}

QScriptDeclarativeClass::Object *QScriptDeclarativeClass::object(const QScriptValue &value)
{
    QScriptValuePrivate *v = QScriptValuePrivate::get(value);
    QScriptIsolate api(v->engine());
    return QScriptDeclarativeClassObject::object(v);
}

QScriptValue QScriptDeclarativeClass::function(const QScriptValue &v, const Identifier &name)
{
    return v.property(*reinterpret_cast<const QString *>(name));
}

QScriptValue QScriptDeclarativeClass::property(const QScriptValue &v, const Identifier &name)
{
    return v.property(*reinterpret_cast<const QString *>(name));
}

QScriptDeclarativeClass::Value
QScriptDeclarativeClass::functionValue(const QScriptValue &v, const Identifier &name)
{
    return Value(static_cast<QScriptEngine *>(0) , v.property(*reinterpret_cast<const QString *>(name)));
}

QScriptDeclarativeClass::Value
QScriptDeclarativeClass::propertyValue(const QScriptValue &v, const Identifier &name)
{
    return Value(static_cast<QScriptEngine *>(0), v.property(*reinterpret_cast<const QString *>(name)));
}

/*
Returns the scope chain entry at \a index.  If index is less than 0, returns
entries starting at the end.  For example, scopeChainValue(context, -1) will return
the value last in the scope chain.
*/
QScriptValue QScriptDeclarativeClass::scopeChainValue(QScriptContext *context, int index)
{
    QScriptValueList chain = context->scopeChain();
    if (index >= 0)
        return chain.value(index);
    else
        return chain.value(chain.count() + index);
}

/*!
  Enters a new execution context and returns the associated
  QScriptContext object.

  Once you are done with the context, you should call popContext() to
  restore the old context.

  By default, the `this' object of the new context is the Global Object.
  The context's \l{QScriptContext::callee()}{callee}() will be invalid.

  Unlike pushContext(), the default scope chain is reset to include
  only the global object and the QScriptContext's activation object.

  \sa QScriptEngine::popContext()
*/
QScriptContext * QScriptDeclarativeClass::pushCleanContext(QScriptEngine *engine)
{
    // QScriptEngine::pushContext() has precisely the behavior described above.
    return engine->pushContext();
}

QScriptDeclarativeClass::~QScriptDeclarativeClass()
{
}

QScriptEngine *QScriptDeclarativeClass::engine() const
{
    return d_ptr->engine;
}

bool QScriptDeclarativeClass::supportsCall() const
{
    return d_ptr->supportsCall;
}

void QScriptDeclarativeClass::setSupportsCall(bool c)
{
    d_ptr->supportsCall = c;
}

QSet<QString> QScriptDeclarativeClassPrivate::identifiers;

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const QString &str)
{
    return PersistentIdentifier(&(*d_ptr->identifiers.insert(str)));
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const Identifier &id)
{
    return PersistentIdentifier(createPersistentIdentifier(toString(id)));
}

QString QScriptDeclarativeClass::toString(const Identifier &identifier)
{
    return *reinterpret_cast<const QString *>(identifier);
}

bool QScriptDeclarativeClass::startsWithUpper(const Identifier &identifier)
{
    QString str = toString(identifier);
    if (str.size() < 1)
        return false;
    return QChar::category(str.at(0).unicode()) == QChar::Letter_Uppercase;
}

quint32 QScriptDeclarativeClass::toArrayIndex(const Identifier &identifier, bool *ok)
{
    quint32 idx = QScriptConverter::toArrayIndex(toString(identifier));
    if (ok)
        *ok = idx != 0xffffffff;
    return idx;
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

QScriptDeclarativeClass::Value
QScriptDeclarativeClass::property(Object *object, const Identifier &name)
{
    Q_UNUSED(object);
    Q_UNUSED(name);
    return Value();
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

QScriptDeclarativeClass::Value QScriptDeclarativeClass::call(Object *object, 
                                                             QScriptContext *ctxt)
{
    Q_UNUSED(object);
    Q_UNUSED(ctxt);
    return Value();
}

bool QScriptDeclarativeClass::compare(Object *o, Object *o2)
{
    return o == o2;
}

QStringList QScriptDeclarativeClass::propertyNames(Object *object)
{
    Q_UNUSED(object);
    return QStringList();
}

bool QScriptDeclarativeClass::isQObject() const
{
    return false;
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

QScriptContext *QScriptDeclarativeClass::context() const
{
    return d_ptr->context;
}

/*!
  Creates a scope object with a fixed set of undeletable properties.
*/
QScriptValue QScriptDeclarativeClass::newStaticScopeObject(
    QScriptEngine *engine, int propertyCount, const QString *names,
    const QScriptValue *values, const QScriptValue::PropertyFlags *flags)
{
    QScriptValue result = engine->newObject();
    for (int i = 0; i < propertyCount; ++i) {
        result.setProperty(names[i], values[i], flags[i]);
    }
    return result;
}

/*!
  Creates a static scope object that's initially empty, but to which new
  properties can be added.
*/
QScriptValue QScriptDeclarativeClass::newStaticScopeObject(QScriptEngine *engine)
{
    return engine->newObject();
}

QT_END_NAMESPACE
