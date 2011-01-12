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

#ifndef QSCRIPTQOBJECT_P_H
#define QSCRIPTQOBJECT_P_H

#include <QtCore/qmetaobject.h>
#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptable;

template<class T>
class QtData
{
public:
    static T *get(v8::Handle<v8::Object> object)
    {
        Q_ASSERT(object->InternalFieldCount() == 1);
        void *ptr = object->GetPointerFromInternalField(0);
        return static_cast<T*>(ptr);
    }

    static T *safeGet(v8::Handle<v8::Object> object)
    {
        void *ptr = object->GetPointerFromInternalField(0);
        return static_cast<T*>(ptr);
    }

    static void set(v8::Handle<v8::Object> object, T* data)
    {
        delete safeGet(object);
        object->SetPointerInInternalField(0, data);
    }
};

// Data associated with a QObject JS wrapper object.
//
// There can exist an arbitrary number of JS wrappers per C++ object,
// in the same script engine or different ones.
//
// - cppObject: The C++ object that is being wrapped.
//
// - ownership: Determines whether the C++ object is destroyed
//   when the JS wrapper is garbage-collected.
//
// - options: Flags that configure the binding
//   (e.g. exclude super-class contents, skip methods in enumeration)
//
class QtInstanceData : public QtData<QtInstanceData>
{
public:
    QtInstanceData(QScriptEnginePrivate *, QObject *, QScriptEngine::ValueOwnership, const QScriptEngine::QObjectWrapOptions &);
    ~QtInstanceData();

    QScriptEnginePrivate *engine() const
    { return m_engine; }

    QObject *cppObject() const
    { return m_cppObject; }

    QScriptEngine::ValueOwnership ownership() const
    { return m_own; }

    QScriptEngine::QObjectWrapOptions options() const
    { return m_opt; }

    //returns a QScriptable if the object is a QScriptable, else, return 0
    QScriptable *toQScriptable()
    {
        Q_ASSERT(m_cppObject);
        void *ptr = m_cppObject->qt_metacast("QScriptable");
        return reinterpret_cast<QScriptable*>(ptr);
    }

private:
    QScriptEnginePrivate *m_engine;
    QPointer<QObject> m_cppObject;
    QScriptEngine::ValueOwnership m_own;
    QScriptEngine::QObjectWrapOptions m_opt;
};

// Data associated with a QMetaObject JS wrapper object.
//
class QtMetaObjectData : public QtData<QtMetaObjectData>
{
public:
    QtMetaObjectData(QScriptEnginePrivate *engine, const QMetaObject *mo, v8::Handle<v8::Value> ctor)
        : m_engine(engine), m_metaObject(mo), m_ctor(v8::Persistent<v8::Value>::New(ctor))
    {
        Q_ASSERT(!ctor.IsEmpty());
    }

    ~QtMetaObjectData()
    {
        m_ctor.Dispose();
    }

    const QMetaObject *metaObject() const
    { return m_metaObject;}

    QScriptEnginePrivate *engine() const
    { return m_engine; }

    v8::Handle<v8::Value> constructor() const { return m_ctor; }


private:
    QScriptEnginePrivate *m_engine;
    const QMetaObject *m_metaObject;
    v8::Persistent<v8::Value> m_ctor;
};

// Data associated with a QVariant JS wrapper object.
//
// When converting a QVariant to JS, QtScript will attempt
// to convert the QVariant to a "real" JS value, but in case
// it can't (for example, the type is a custom type with no
// conversion functions registered), the QVariant is wrapped
// in a custom JS object.
//
// It's also possible to explicitly create a QVariant wrapper
// object by calling QScriptEngine::newVariant().
//
class QtVariantData : public QtData<QtVariantData>
{
public:
    QtVariantData(const QVariant &value)
        : m_value(value)
    { }

    QVariant &value()
    { return m_value; }
    void setValue(const QVariant &value)
    { m_value = value; }

private:
    QVariant m_value;
};

v8::Handle<v8::FunctionTemplate> createQtClassTemplate(QScriptEnginePrivate *, const QMetaObject *);

v8::Handle<v8::Value> QtDynamicPropertyGetter(v8::Local<v8::String> property,
                                                     const v8::AccessorInfo& info);
void QtDynamicPropertySetter(v8::Local<v8::String> property,
                                    v8::Local<v8::Value> value,
                                    const v8::AccessorInfo& info);
v8::Handle<v8::Value> QtMetaObjectCallback(const v8::Arguments& args);
v8::Handle<v8::Value> QtMetaObjectPropertyGetter(v8::Local<v8::String> property,
                                                 const v8::AccessorInfo& info);
QObject *toQtObject(QScriptEnginePrivate *engine, const v8::Handle<v8::Object> &object);




QT_END_NAMESPACE

#endif
