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

#include "qscriptengine.h"

#include <QtCore/qpointer.h>

#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptable;

// A C++ signal-to-JS handler connection.
//
// Acts as a middle-man; intercepts a C++ signal,
// and invokes a JS callback function.
//
class QtSignalData;
class QtConnection : public QObject
{
public:
    QtConnection(QtSignalData *signal);
    ~QtConnection();

    bool connect(v8::Handle<v8::Object> receiver,  v8::Handle<v8::Function> callback, Qt::ConnectionType type);
    bool disconnect();

    v8::Handle<v8::Function> callback() const
    { return m_callback; }

    // This class implements qt_metacall() and friends manually; moc should
    // not process it. But then the Q_OBJECT macro must be manually expanded:
    static const QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **);

    // Slot.
    void onSignal(void **);

private:
    QtSignalData *m_signal;
    v8::Persistent<v8::Function> m_callback;
    v8::Persistent<v8::Object> m_receiver;
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
class QtSignalData
{
public:
    enum ResolveMode {
        ResolvedByName = 0,
        ResolvedBySignature = 1
    };

    QtSignalData(v8::Handle<v8::Object> object, int index, ResolveMode mode)
        : m_object(v8::Persistent<v8::Object>::New(object)),
        m_index(index), m_resolveMode(mode)
    { }
    ~QtSignalData()
    {
        if (!m_object.IsEmpty())
            m_object.Dispose();
    }

    // Gets the QtSignalData pointer from the given object.
    // Assumes that the object is a Qt signal wrapper.
    static QtSignalData *get(v8::Handle<v8::Object> object)
    {
        void *ptr = object->GetPointerFromInternalField(0);
        Q_ASSERT(ptr != 0);
        return static_cast<QtSignalData*>(ptr);
    }

    // The QObject wrapper object that this signal is bound to.
    v8::Handle<v8::Object> object() const
    { return m_object; }

    int index() const
    { return m_index; }

    ResolveMode resolveMode() const
    { return ResolveMode(m_resolveMode); }

    v8::Handle<v8::Value> connect(v8::Handle<v8::Object> receiver,
                                  v8::Handle<v8::Function> slot,
                                  Qt::ConnectionType type = Qt::AutoConnection);
    v8::Handle<v8::Value> disconnect(v8::Handle<v8::Function> callback);

private:
    QList<QtConnection*> m_connections;
    v8::Persistent<v8::Object> m_object;
    uint m_index:31;
    uint m_resolveMode:1;
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
class QtInstanceData
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

    static QtInstanceData *get(v8::Handle<v8::Object> object)
    {
        void *ptr = object->GetPointerFromInternalField(0);
        Q_ASSERT(ptr != 0);
        return static_cast<QtInstanceData*>(ptr);
    }

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
class QtMetaObjectData
{
public:
    QtMetaObjectData(QScriptEnginePrivate *engine, const QMetaObject *mo, const QScriptValue &ctor)
        : m_engine(engine), m_metaObject(mo), m_ctor(ctor)
    { }

    const QMetaObject *metaObject() const
    { return m_metaObject;}

    static QtMetaObjectData *get(v8::Handle<v8::Object> object)
    {
        void *ptr = object->GetPointerFromInternalField(0);
        Q_ASSERT(ptr != 0);
        return static_cast<QtMetaObjectData*>(ptr);
    }

    QScriptEnginePrivate *engine() const
    { return m_engine; }

    QScriptValue constructor() const { return m_ctor; }


private:
    QScriptEnginePrivate *m_engine;
    const QMetaObject *m_metaObject;
    QScriptValue m_ctor;
};

v8::Handle<v8::FunctionTemplate> createQtClassTemplate(QScriptEnginePrivate *, const QMetaObject *);
v8::Handle<v8::FunctionTemplate> createQtSignalTemplate();
v8::Handle<v8::FunctionTemplate> createQtMetaObjectTemplate();

v8::Handle<v8::Object> newQtObject(QScriptEnginePrivate *engine, QObject *object,
                                   QScriptEngine::ValueOwnership own,
                                   const QScriptEngine::QObjectWrapOptions &opt);

QObject *toQtObject(QScriptEnginePrivate *engine, const v8::Handle<v8::Object> &object);


QT_END_NAMESPACE

#endif
