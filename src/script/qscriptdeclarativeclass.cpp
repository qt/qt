/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qscriptdeclarativeobject_p.h"
#include <QtScript/qscriptstring.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptengineagent.h>
#include <private/qscriptengine_p.h>
#include <private/qscriptvalue_p.h>
#include <private/qscriptqobject_p.h>
#include <QtCore/qstringlist.h>

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
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(const Value &other)
{
    Q_UNUSED(other);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, int value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, uint value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *, bool value)
{
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, double value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, float value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, const QString &value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptContext *ctxt, const QScriptValue &value)
{
    Q_UNUSED(ctxt);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, int value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, uint value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, bool value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, double value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, float value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, const QString &value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::Value(QScriptEngine *eng, const QScriptValue &value)
{
    Q_UNUSED(eng);
    Q_UNUSED(value);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::Value::~Value()
{
    Q_UNIMPLEMENTED();
}

QScriptValue QScriptDeclarativeClass::Value::toScriptValue(QScriptEngine *engine) const
{
    Q_UNUSED(engine);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier()
    : identifier(0), engine(0)
{
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::PersistentIdentifier::~PersistentIdentifier()
{
    if (engine) {
        Q_UNIMPLEMENTED();
    }
}

QScriptDeclarativeClass::PersistentIdentifier::PersistentIdentifier(const PersistentIdentifier &other)
{
    Q_UNUSED(other);
    Q_UNIMPLEMENTED();
}

QScriptDeclarativeClass::PersistentIdentifier &
QScriptDeclarativeClass::PersistentIdentifier::operator=(const PersistentIdentifier &other)
{
    Q_UNUSED(other);
    Q_UNIMPLEMENTED();
    return *this;
}

QScriptDeclarativeClass::QScriptDeclarativeClass(QScriptEngine *engine)
: d_ptr(new QScriptDeclarativeClassPrivate)
{
    Q_UNUSED(engine);
    Q_UNIMPLEMENTED();
}

QScriptValue QScriptDeclarativeClass::newObject(QScriptEngine *engine, 
                                                QScriptDeclarativeClass *scriptClass, 
                                                Object *object)
{
    Q_ASSERT(engine);
    Q_ASSERT(scriptClass);

    Q_UNUSED(engine);
    Q_UNUSED(scriptClass);
    Q_UNUSED(object);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

QScriptDeclarativeClass::Value 
QScriptDeclarativeClass::newObjectValue(QScriptEngine *engine, 
                                        QScriptDeclarativeClass *scriptClass, 
                                        Object *object)
{
    Q_ASSERT(engine);
    Q_ASSERT(scriptClass);

    Q_UNUSED(engine);
    Q_UNUSED(scriptClass);
    Q_UNUSED(object);
    Q_UNIMPLEMENTED();
    return Value();
}

QScriptDeclarativeClass *QScriptDeclarativeClass::scriptClass(const QScriptValue &v)
{
    Q_UNUSED(v);
    Q_UNIMPLEMENTED();
    return 0;
}

QScriptDeclarativeClass::Object *QScriptDeclarativeClass::object(const QScriptValue &v)
{
    Q_UNUSED(v);
    Q_UNIMPLEMENTED();
    return 0;
}

QScriptValue QScriptDeclarativeClass::function(const QScriptValue &v, const Identifier &name)
{
    Q_UNUSED(v);
    Q_UNUSED(name);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

QScriptValue QScriptDeclarativeClass::property(const QScriptValue &v, const Identifier &name)
{
    Q_UNUSED(v);
    Q_UNUSED(name);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

QScriptDeclarativeClass::Value
QScriptDeclarativeClass::functionValue(const QScriptValue &v, const Identifier &name)
{
    Q_UNUSED(v);
    Q_UNUSED(name);
    Q_UNIMPLEMENTED();
    return Value();
}

QScriptDeclarativeClass::Value
QScriptDeclarativeClass::propertyValue(const QScriptValue &v, const Identifier &name)
{
    Q_UNUSED(v);
    Q_UNUSED(name);
    Q_UNIMPLEMENTED();
    return Value();
}

/*
Returns the scope chain entry at \a index.  If index is less than 0, returns
entries starting at the end.  For example, scopeChainValue(context, -1) will return
the value last in the scope chain.
*/
QScriptValue QScriptDeclarativeClass::scopeChainValue(QScriptContext *context, int index)
{
    Q_UNUSED(context);
    Q_UNUSED(index);
    Q_UNIMPLEMENTED();
    return QScriptValue();
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
    Q_UNUSED(engine);
    Q_UNIMPLEMENTED();
    return 0;
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

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const QString &str)
{
    Q_UNUSED(str);
    Q_UNIMPLEMENTED();
    return PersistentIdentifier();
}

QScriptDeclarativeClass::PersistentIdentifier 
QScriptDeclarativeClass::createPersistentIdentifier(const Identifier &id)
{
    Q_UNUSED(id);
    Q_UNIMPLEMENTED();
    return PersistentIdentifier();
}

QString QScriptDeclarativeClass::toString(const Identifier &identifier)
{
    Q_UNUSED(identifier);
    Q_UNIMPLEMENTED();
    return QString();
}

quint32 QScriptDeclarativeClass::toArrayIndex(const Identifier &identifier, bool *ok)
{
    Q_UNUSED(identifier);
    Q_UNUSED(ok);
    Q_UNIMPLEMENTED();
    return 0;
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
    Q_UNUSED(engine);
    Q_UNUSED(propertyCount);
    Q_UNUSED(names);
    Q_UNUSED(values);
    Q_UNUSED(flags);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

/*!
  Creates a static scope object that's initially empty, but to which new
  properties can be added.
*/
QScriptValue QScriptDeclarativeClass::newStaticScopeObject(QScriptEngine *engine)
{
    Q_UNUSED(engine);
    Q_UNIMPLEMENTED();
    return QScriptValue();
}

QT_END_NAMESPACE
