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

#ifndef QSCRIPTENGINE_P_H
#define QSCRIPTENGINE_P_H

#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>

#include <private/qobject_p.h>

#include "qscriptconverter_p.h"
#include "qscriptprogram_p.h"
#include "qscriptqobject_p.h"
#include "qscriptoriginalglobalobject_p.h"
#include "qscriptvalue.h"

#include <v8.h>

QT_BEGIN_NAMESPACE

class QDateTime;
class QScriptEngine;

class QScriptEnginePrivate
    : public QSharedData
{
    Q_DECLARE_PUBLIC(QScriptEngine)
public:
    static QScriptEnginePrivate* get(QScriptEngine* q) { Q_ASSERT(q); return q->d_func(); }
    static QScriptEngine* get(QScriptEnginePrivate* d) { Q_ASSERT(d); return d->q_func(); }

    QScriptEnginePrivate(QScriptEngine*);
    ~QScriptEnginePrivate();

    inline QScriptValuePrivate* evaluate(const QString& program, const QString& fileName, int lineNumber);
    inline QScriptValuePrivate* evaluate(QScriptProgramPrivate* program);
    QScriptValuePrivate* evaluate(v8::Handle<v8::Script> script, v8::TryCatch& tryCatch);
    inline void collectGarbage();
    inline void reportAdditionalMemoryCost(int cost);
    QScriptValuePrivate* globalObject() const;

    QScriptContextPrivate* currentContext();

    QScriptValuePrivate* newArray(uint length);
    QScriptValuePrivate* newObject();
    v8::Handle<v8::Object> newQObject(
        QObject *object, QScriptEngine::ValueOwnership own = QScriptEngine::QtOwnership,
        const QScriptEngine::QObjectWrapOptions &opt = 0);
    v8::Handle<v8::Object> newSignal(v8::Handle<v8::Object>, int index, QtSignalData::ResolveMode);

    v8::Handle<v8::Object> newVariant(const QVariant &value);

    v8::Handle<v8::FunctionTemplate> createSignalTemplate();
    v8::Handle<v8::FunctionTemplate> createMetaObjectTemplate();
    v8::Handle<v8::FunctionTemplate> createVariantTemplate();

    v8::Handle<v8::FunctionTemplate> qtClassTemplate(const QMetaObject *);
    v8::Handle<v8::FunctionTemplate> qobjectTemplate();

    inline v8::Persistent<v8::Value> makeJSValue(bool value);
    inline v8::Persistent<v8::Value> makeJSValue(int value);
    inline v8::Persistent<v8::Value> makeJSValue(uint value);
    inline v8::Persistent<v8::Value> makeJSValue(qsreal value);
    inline v8::Persistent<v8::Value> makeJSValue(QScriptValue::SpecialValue value);
    inline v8::Persistent<v8::Value> makeJSValue(const QString& value);
    inline bool isError(const QScriptValuePrivate* value) const;

    QDateTime qtDateTimeFromJS(v8::Handle<v8::Date> jsDate);
    v8::Handle<v8::Date> qtDateTimeToJS(const QDateTime &dt);

#ifndef QT_NO_REGEXP
    QRegExp qtRegExpFromJS(v8::Handle<v8::Object> jsRegExp);
    v8::Handle<v8::Object> qtRegExpToJS(const QRegExp &re);
#endif

    v8::Handle<v8::Array> stringListToJS(const QStringList &lst);
    QStringList stringListFromJS(v8::Handle<v8::Array> jsArray);

    v8::Handle<v8::Array> variantListToJS(const QVariantList &lst);
    QVariantList variantListFromJS(v8::Handle<v8::Array> jsArray);

    v8::Handle<v8::Object> variantMapToJS(const QVariantMap &vmap);
    QVariantMap variantMapFromJS(v8::Handle<v8::Object> jsObject);

    v8::Handle<v8::Value> variantToJS(const QVariant &value);
    QVariant variantFromJS(v8::Handle<v8::Value> value);

    v8::Handle<v8::Value> metaTypeToJS(int type, const void *data);
    bool metaTypeFromJS(v8::Handle<v8::Value> value, int type, void *data);

    bool isQtObject(v8::Handle<v8::Value> value);
    QObject *qtObjectFromJS(v8::Handle<v8::Value> value);
    bool convertToNativeQObject(v8::Handle<v8::Value> value,
                                const QByteArray &targetType,
                                void **result);

    bool isQtVariant(v8::Handle<v8::Value> value);
    bool isQtSignal(v8::Handle<v8::Value> value);
    QVariant &variantValue(v8::Handle<v8::Value> value);

    QScriptValue scriptValueFromInternal(v8::Handle<v8::Value>);

    inline operator v8::Persistent<v8::Context>();
private:
    // FIXME check if we can reuse QObjectProvate::q_ptr
    QScriptEngine* q_ptr;
    v8::Persistent<v8::Context> m_context;
    QScriptOriginalGlobalObject m_globalObject;

    QHash<const QMetaObject *, v8::Persistent<v8::FunctionTemplate> > m_qtClassTemplates;
    v8::Persistent<v8::FunctionTemplate> m_signalTemplate;
    v8::Persistent<v8::FunctionTemplate> m_metaObjectTemplate;
    v8::Persistent<v8::FunctionTemplate> m_variantTemplate;
};

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(bool value)
{
    return v8::Persistent<v8::Value>::New(value ? v8::True() : v8::False());
}

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(int value)
{
    return v8::Persistent<v8::Value>::New(v8::Integer::New(value));
}

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(uint value)
{
    return v8::Persistent<v8::Value>::New(v8::Integer::NewFromUnsigned(value));
}

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(qsreal value)
{
    return v8::Persistent<v8::Value>::New(v8::Number::New(value));
}

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(QScriptValue::SpecialValue value) {
    if (value == QScriptValue::NullValue)
        return v8::Persistent<v8::Value>::New(v8::Null());
    return v8::Persistent<v8::Value>::New(v8::Undefined());
}

v8::Persistent<v8::Value> QScriptEnginePrivate::makeJSValue(const QString& value)
{
    return v8::Persistent<v8::Value>::New(QScriptConverter::toString(value));
}

inline QScriptEnginePrivate::operator v8::Persistent<v8::Context>()
{
    return m_context;
}

/*!
  \internal
  Check if given value is an Error instance
  \attention value had to be an object.
*/
inline bool QScriptEnginePrivate::isError(const QScriptValuePrivate* value) const
{
    v8::Context::Scope contextScope(m_context);
    v8::HandleScope handleScope;
    return m_globalObject.isError(value);
}

QScriptValuePrivate* QScriptEnginePrivate::evaluate(const QString& program, const QString& fileName, int lineNumber)
{
    Q_UNUSED(lineNumber);
    v8::Context::Scope contextScope(m_context);
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> script = v8::Script::Compile(QScriptConverter::toString(program), QScriptConverter::toString(fileName));
    return evaluate(script, tryCatch);
}

inline QScriptValuePrivate* QScriptEnginePrivate::evaluate(QScriptProgramPrivate* program)
{
    v8::Context::Scope contextScope(m_context);
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Script> script = program->compiled(this);
    return evaluate(script, tryCatch);
}

void QScriptEnginePrivate::collectGarbage()
{
    v8::V8::LowMemoryNotification();
}

void QScriptEnginePrivate::reportAdditionalMemoryCost(int cost)
{
    // The check is needed only for compatibility.
    if (cost > 0)
        v8::V8::AdjustAmountOfExternalAllocatedMemory(cost);
}

QT_END_NAMESPACE

#endif
