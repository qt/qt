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

#ifndef QSCRIPTQOBJECT_P_H
#define QSCRIPTQOBJECT_P_H

#include <QtCore/qmetaobject.h>
#include <QtCore/qsharedpointer.h>
#include "qscripttools_p.h"
#include "qscriptv8objectwrapper_p.h"
#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptable;

class QtDataBase : public QScriptLinkedNode
{
public:
    // QtData use virtual destructor for deleting "unspecified" data from QSEP::dealllocateAddtionalData.
    inline QtDataBase(QScriptEnginePrivate *engine);
    inline virtual ~QtDataBase();
    inline QScriptEnginePrivate *engine() const;
private:
    QScriptEnginePrivate *m_engine;
};

template<class T>
class QtData : public QtDataBase
{
public:
    inline QtData(QScriptEnginePrivate *engine);
    static T *get(v8::Handle<v8::Object> object);
    static void set(v8::Handle<v8::Object> object, T* data);
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
class QScriptQObjectData : public QtData<QScriptQObjectData>
{
public:
    enum Mode {RaiseException, IgnoreException};

    inline QScriptQObjectData(QScriptEnginePrivate *, QObject *, QScriptEngine::ValueOwnership, const QScriptEngine::QObjectWrapOptions &);
    inline ~QScriptQObjectData();
    inline QObject *cppObject(v8::Local<v8::Value> *error = 0) const;
    inline QObject *cppObject(const Mode mode) const;
    inline QScriptEngine::ValueOwnership ownership() const;
    inline QScriptEngine::QObjectWrapOptions options() const;
    inline QScriptable *toQScriptable();

private:
    QWeakPointer<QObject> m_cppObject;
    QScriptEngine::ValueOwnership m_own;
    QScriptEngine::QObjectWrapOptions m_opt;
};

// Data associated with a QMetaObject JS wrapper object.
//
class QtMetaObjectData : public QtData<QtMetaObjectData>
{
public:
    inline QtMetaObjectData(QScriptEnginePrivate *engine, const QMetaObject *mo, v8::Handle<v8::Value> ctor);
    inline ~QtMetaObjectData();
    inline const QMetaObject *metaObject() const;
    inline v8::Handle<v8::Value> constructor() const;
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
    inline QtVariantData(QScriptEnginePrivate *engine, const QVariant &value);
    inline QVariant &value();
    inline void setValue(const QVariant &value);
private:
    QVariant m_value;
};

v8::Handle<v8::FunctionTemplate> createQtClassTemplate(QScriptEnginePrivate *, const QMetaObject *, const QScriptEngine::QObjectWrapOptions &options);

v8::Handle<v8::Value> QtDynamicPropertyGetter(v8::Local<v8::String> property,
                                                     const v8::AccessorInfo& info);
void QtDynamicPropertySetter(v8::Local<v8::String> property,
                                    v8::Local<v8::Value> value,
                                    const v8::AccessorInfo& info);

v8::Handle<v8::Value> QtLazyPropertyGetter(v8::Local<v8::String> property,
                                                  const v8::AccessorInfo& info);
v8::Handle<v8::Value> QtLazyPropertySetter(v8::Local<v8::String> property,
                                                  v8::Local<v8::Value> value,
                                                  const v8::AccessorInfo& info);

v8::Handle<v8::Value> QtMetaObjectCallback(const v8::Arguments& args);
v8::Handle<v8::Value> QtMetaObjectPropertyGetter(v8::Local<v8::String> property,
                                                 const v8::AccessorInfo& info);
v8::Handle<v8::Array> QtMetaObjectEnumerator(const v8::AccessorInfo& info);
QObject *toQtObject(QScriptEnginePrivate *engine, const v8::Handle<v8::Object> &object);

union QScriptMetaMethodInfo {
    QScriptMetaMethodInfo(): intData(0)
    { }

    uint32_t intData;
    struct {
        uint index: 28;
        uint resolveMode: 1;
        uint overloaded: 1;
        uint voidvoid: 1;
        uint padding: 1; // Make sure the struct fits in an SMI
    };
};

template <typename T, v8::Persistent<v8::FunctionTemplate> QScriptEnginePrivate::*functionTemplate>
class QScriptGenericMetaMethodData : public QScriptV8ObjectWrapper<T, functionTemplate> {
public:
    enum ResolveMode {
        ResolvedByName = 0,
        ResolvedBySignature = 1
    };

    QScriptGenericMetaMethodData(QScriptEnginePrivate *eng, v8::Handle<v8::Object> object,
                                 QScriptMetaMethodInfo info)
        : m_object(v8::Persistent<v8::Object>::New(object)), m_info(info)
    {
        this->engine = eng;
        // We cannot keep a persistant reference to the object, else it would never be garbage collected.
        //  (the object also reference us,  and persistent object are automatically marked.
        //  A reference is kept in the second internal field of the v8 method object.
        m_object.MakeWeak(this, objectDestroyed);
    }
    ~QScriptGenericMetaMethodData()
    {
        m_object.Dispose();
    }

    // The QObject wrapper object that this signal is bound to.
    v8::Handle<v8::Object> object() const
    { return m_object; }

    int index() const
    { return m_info.index; }

    ResolveMode resolveMode() const
    { return ResolveMode(m_info.resolveMode); }

    v8::Handle<v8::Value> call();

    v8::Persistent<v8::Object> m_object;
    QScriptMetaMethodInfo m_info;
private:
    static void objectDestroyed(v8::Persistent<v8::Value> object, void *data) {
        QScriptGenericMetaMethodData *that = static_cast<QScriptGenericMetaMethodData *>(data);
        Q_ASSERT(that->m_object == object);
        that->m_object.Clear();
        object.Dispose();
        // Note that since the method keep a reference to the object in its internal field,
        // this is only called when the QScriptGenericMetaMethodData is about to be garbage collected as well.
    }
};

class QScriptMetaMethodData : public QScriptGenericMetaMethodData<QScriptMetaMethodData, &QScriptEnginePrivate::metaMethodTemplate>
{
    typedef QScriptGenericMetaMethodData<QScriptMetaMethodData, &QScriptEnginePrivate::metaMethodTemplate> Base;
public:
    QScriptMetaMethodData(QScriptEnginePrivate *engine, v8::Handle<v8::Object> object, QScriptMetaMethodInfo info)
    : Base(engine, object, info)
    { }

    static v8::Handle<v8::FunctionTemplate> createFunctionTemplate(QScriptEnginePrivate *engine);
};

// Data associated with a signal JS wrapper object.
//
// A signal wrapper is bound to the particular Qt wrapper object
// where it was looked up as a member, i.e. signal wrappers are
// _per instance_, not per class (prototype). This is in order
// to support the connect() and disconnect() syntax:
//
// button1.clicked.connect(...);
// button2.clicked.connect(...);
//
// When connect() is called, the this-object will be the signal
// wrapper, not the QObject. Hence, in order to know which object's
// clicked() signal to connect to, the signal must be bound to
// that object.
//
// - object: The Qt wrapper object that this signal is bound to.
//
// - index: The index of the C++ signal.
//
// - resolve mode: How the signal was resolved; by name or signature.
//   If it was resolved by name, there's a chance the signal might have overloads.
//

class QScriptConnection;
class QScriptSignalData : public QScriptGenericMetaMethodData<QScriptSignalData, &QScriptEnginePrivate::signalTemplate>
{
    typedef QScriptGenericMetaMethodData<QScriptSignalData, &QScriptEnginePrivate::signalTemplate> Base;
public:

    QScriptSignalData(QScriptEnginePrivate *engine, v8::Handle<v8::Object> object, QScriptMetaMethodInfo info)
        : Base(engine, object, info)
    { }

    ~QScriptSignalData();

    static v8::Handle<v8::FunctionTemplate> createFunctionTemplate(QScriptEnginePrivate *engine);

    v8::Handle<v8::Value> connect(v8::Handle<v8::Object> receiver,
                                  v8::Handle<v8::Object> slot,
                                  Qt::ConnectionType type = Qt::AutoConnection);
    v8::Handle<v8::Value> disconnect(v8::Handle<v8::Function> callback);

    static QScriptSignalData *get(v8::Handle<v8::Object> object)
    {
        void *ptr = object->GetPointerFromInternalField(0);
        Q_ASSERT(ptr != 0);
        return static_cast<QScriptSignalData*>(ptr);
    }

    void unregisterQScriptConnection(QScriptConnection *connection) { m_connections.removeAll(connection); }
private:
    static v8::Handle<v8::Value> QtConnectCallback(const v8::Arguments& args);
    static v8::Handle<v8::Value> QtDisconnectCallback(const v8::Arguments& args);
    QList<QScriptConnection*> m_connections;
};

QT_END_NAMESPACE

#endif
