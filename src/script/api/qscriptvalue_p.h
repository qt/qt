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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QSCRIPTVALUE_P_H
#define QSCRIPTVALUE_P_H

#include <v8.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qmath.h>
#include <QtCore/qvarlengtharray.h>
#include <qdebug.h>

#include "qscriptconverter_p.h"
#include "qscripttools_p.h"
#include "qscriptshareddata_p.h"
#include "qscriptvalue.h"
#include "qscriptstring_p.h"

QT_BEGIN_NAMESPACE

class QScriptClassPrivate;
/*!
  \internal
  \class QScriptValuePrivate
*/
class QScriptValuePrivate
        : public QSharedData
        , public QScriptLinkedNode
{
public:
    inline QScriptValuePrivate();
    inline static QScriptValuePrivate* get(const QScriptValue& q);
    inline static QScriptValue get(const QScriptValuePrivate* d);
    inline static QScriptValue get(QScriptValuePrivate* d);
    inline static QScriptValue get(QScriptPassPointer<QScriptValuePrivate> d);
    inline ~QScriptValuePrivate();

    inline QScriptValuePrivate(bool value);
    inline QScriptValuePrivate(int value);
    inline QScriptValuePrivate(uint value);
    inline QScriptValuePrivate(qsreal value);
    inline QScriptValuePrivate(const QString& value);
    inline QScriptValuePrivate(QScriptValue::SpecialValue value);

    inline QScriptValuePrivate(QScriptEnginePrivate* engine, bool value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, int value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, uint value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, qsreal value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, const QString& value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, QScriptValue::SpecialValue value);
    inline QScriptValuePrivate(QScriptEnginePrivate* engine, v8::Handle<v8::Value>);
    inline void reinitialize();
    inline void reinitialize(QScriptEnginePrivate* engine, v8::Handle<v8::Value> value);

    inline bool toBool() const;
    inline qsreal toNumber() const;
    inline QScriptPassPointer<QScriptValuePrivate> toObject() const;
    inline QScriptPassPointer<QScriptValuePrivate> toObject(QScriptEnginePrivate* engine) const;
    inline QString toString() const;
    inline qsreal toInteger() const;
    inline qint32 toInt32() const;
    inline quint32 toUInt32() const;
    inline quint16 toUInt16() const;
    inline QDateTime toDataTime() const;
    inline QRegExp toRegExp() const;
    inline QObject *toQObject() const;
    inline QVariant toVariant() const;
    inline const QMetaObject *toQMetaObject() const;

    inline bool isArray() const;
    inline bool isBool() const;
    inline bool isCallable() const;
    inline bool isError() const;
    inline bool isFunction() const;
    inline bool isNull() const;
    inline bool isNumber() const;
    inline bool isObject() const;
    inline bool isString() const;
    inline bool isUndefined() const;
    inline bool isValid() const;
    inline bool isVariant() const;
    inline bool isDate() const;
    inline bool isRegExp() const;
    inline bool isQObject() const;
    inline bool isQMetaObject() const;

    inline bool equals(QScriptValuePrivate* other);
    inline bool strictlyEquals(QScriptValuePrivate* other);
    inline bool lessThan(QScriptValuePrivate *other) const;
    inline bool instanceOf(QScriptValuePrivate*) const;
    inline bool instanceOf(v8::Handle<v8::Object> other) const;

    inline QScriptPassPointer<QScriptValuePrivate> prototype() const;
    inline void setPrototype(QScriptValuePrivate* prototype);
    QScriptClassPrivate* scriptClass() const;
    void setScriptClass(QScriptClassPrivate* scriptclass);

    inline void setProperty(const QScriptStringPrivate *name, QScriptValuePrivate *value, uint attribs = 0);
    inline void setProperty(const QString &name, QScriptValuePrivate *value, uint attribs = 0);
    inline void setProperty(v8::Handle<v8::String> name, QScriptValuePrivate *value, uint attribs = 0);
    inline void setProperty(quint32 index, QScriptValuePrivate* value, uint attribs = 0);
    inline QScriptPassPointer<QScriptValuePrivate> property(const QString& name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptPassPointer<QScriptValuePrivate> property(quint32 index, const QScriptValue::ResolveFlags& mode) const;
    template<typename T>
    inline QScriptPassPointer<QScriptValuePrivate> property(T name, const QScriptValue::ResolveFlags& mode) const;
    inline bool deleteProperty(const QString& name);
    inline QScriptValue::PropertyFlags propertyFlags(const QString& name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptValue::PropertyFlags propertyFlags(const QScriptStringPrivate* name, const QScriptValue::ResolveFlags& mode) const;
    inline QScriptValue::PropertyFlags propertyFlags(v8::Handle<v8::String> name, const QScriptValue::ResolveFlags& mode) const;
    inline void setData(QScriptValuePrivate* value) const;
    inline QScriptPassPointer<QScriptValuePrivate> data() const;

    inline int convertArguments(QVarLengthArray<v8::Handle<v8::Value>, 8> *argv, const QScriptValue& arguments);

    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, const QScriptValueList& args);
    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, const QScriptValue& arguments);
    inline QScriptPassPointer<QScriptValuePrivate> call(QScriptValuePrivate* thisObject, int argc, v8::Handle< v8::Value >* argv);
    inline QScriptPassPointer<QScriptValuePrivate> construct(int argc, v8::Handle<v8::Value> *argv);
    inline QScriptPassPointer<QScriptValuePrivate> construct(const QScriptValueList& args);
    inline QScriptPassPointer<QScriptValuePrivate> construct(const QScriptValue& arguments);

    inline bool assignEngine(QScriptEnginePrivate* engine);
    inline QScriptEnginePrivate* engine() const;

    inline operator v8::Handle<v8::Value>() const;
    inline operator v8::Handle<v8::Object>() const;
    inline v8::Handle<v8::Value> asV8Value(QScriptEnginePrivate* engine);
    inline qint64 objectId() const;
private:
    QScriptEnginePrivate *m_engine;

    // Please, update class documentation when you change the enum.
    enum State {
        Invalid = 0,
        CString = 0x1000,
        CNumber,
        CBool,
        CNull,
        CUndefined,
        JSValue = 0x2000, // V8 values are equal or higher then this value.
        // JSPrimitive,
        // JSObject
    } m_state;

    union CValue {
        bool m_bool;
        qsreal m_number;
        QString* m_string;

        CValue() : m_number(0) {}
        CValue(bool value) : m_bool(value) {}
        CValue(int number) : m_number(number) {}
        CValue(uint number) : m_number(number) {}
        CValue(qsreal number) : m_number(number) {}
        CValue(QString* string) : m_string(string) {}
    } u;
    // v8::Persistent is not a POD, so can't be part of the union.
    v8::Persistent<v8::Value> m_value;

    Q_DISABLE_COPY(QScriptValuePrivate)
    inline bool isJSBased() const;
    inline bool isNumberBased() const;
    inline bool isStringBased() const;
    inline bool prepareArgumentsForCall(v8::Handle<v8::Value> argv[], const QScriptValueList& arguments) const;

    friend bool qScriptConnect(QObject *, const char *, const QScriptValue &, const QScriptValue &);
    friend bool qScriptDisconnect(QObject *, const char *, const QScriptValue &, const QScriptValue &);
};

template<>
class QGlobalStaticDeleter<QScriptValuePrivate>
{
public:
    QGlobalStatic<QScriptValuePrivate> &globalStatic;
    QGlobalStaticDeleter(QGlobalStatic<QScriptValuePrivate> &_globalStatic)
        : globalStatic(_globalStatic)
    {
        globalStatic.pointer->ref.ref();
    }

    inline ~QGlobalStaticDeleter()
    {
        if (!globalStatic.pointer->ref.deref()) { // Logic copy & paste from SharedDataPointer
            delete globalStatic.pointer;
        }
        globalStatic.pointer = 0;
        globalStatic.destroyed = true;
    }
};

Q_GLOBAL_STATIC(QScriptValuePrivate, InvalidValue)

QT_END_NAMESPACE

#endif
