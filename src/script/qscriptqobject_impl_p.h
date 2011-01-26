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


#ifndef QSCRIPTQOBJECT_IMPL_P_H
#define QSCRIPTQOBJECT_IMPL_P_H

inline QtDataBase::QtDataBase(QScriptEnginePrivate *engine)
    : m_engine(engine)
{
    Q_ASSERT(engine);
    engine->registerAdditionalResources(this);
}

inline QtDataBase::~QtDataBase()
{}

inline QScriptEnginePrivate *QtDataBase::engine() const
{
    return m_engine;
}

template<class T>
QtData<T>::QtData(QScriptEnginePrivate *engine)
    : QtDataBase(engine)
{ }

template<class T>
T* QtData<T>::get(v8::Handle<v8::Object> object)
{
    Q_ASSERT(object->InternalFieldCount() == 1);
    void *ptr = object->GetPointerFromInternalField(0);
    return static_cast<T*>(ptr);
}

template<class T>
T* QtData<T>::safeGet(v8::Handle<v8::Object> object)
{
    void *ptr = object->InternalFieldCount() > 0 ? object->GetPointerFromInternalField(0) : 0;
    return static_cast<T*>(ptr);
}

template<class T>
void QtData<T>::set(v8::Handle<v8::Object> object, T* data)
{
    T* oldData = safeGet(object);
    delete oldData;
    oldData->engine()->unregisterAdditionalResources(oldData);
    object->SetPointerInInternalField(0, data);
}

inline QtInstanceData::QtInstanceData(QScriptEnginePrivate *engine, QObject *object,
                               QScriptEngine::ValueOwnership own,
                               const QScriptEngine::QObjectWrapOptions &opt)
    : QtData<QtInstanceData>(engine)
    , m_cppObject(object)
    , m_own(own)
    , m_opt(opt)
{
}

inline QtInstanceData::~QtInstanceData()
{
//    qDebug("~QtInstanceData()");
    switch (m_own) {
    case QScriptEngine::QtOwnership:
        break;
    case QScriptEngine::AutoOwnership:
        if (m_cppObject && m_cppObject->parent())
            break;
    case QScriptEngine::ScriptOwnership:
        delete m_cppObject;
        break;
    }
}

inline QObject *QtInstanceData::cppObject(v8::Local<v8::Value> *error) const
{
    if (!m_cppObject) {
        v8::Local<v8::String> msg = v8::String::New("cannot access member of deleted QObject");
        /*v8::Handle<v8::String> msg = v8::String::Concat(
                    v8::String::New("cannot access member `"),
                    v8::String::Concat(property, v8::String::New("' of deleted QObject")));*/
        // FIXME: Workaround for http://code.google.com/p/v8/issues/detail?id=1072
        // We should throw an error here and QSVP::property should return an error value.
        // But because of the v8 bug, tryCatch in QSVP doesn't catch the error and obtain as a result
        // an empty handler.
        // v8::ThrowException(err);
        v8::Local<v8::Value> err = v8::Exception::Error(msg);
        engine()->setException(err);
        if (error) {
            *error = err;
        }
    }
    return m_cppObject;
}

inline QObject *QtInstanceData::cppObject(const Mode mode) const
{
    if (mode == IgnoreException)
        return m_cppObject;
    return cppObject();
}

inline QScriptEngine::ValueOwnership QtInstanceData::ownership() const
{
    return m_own;
}

inline QScriptEngine::QObjectWrapOptions QtInstanceData::options() const
{
    return m_opt;
}

/*!
  Returns a QScriptable if the object is a QScriptable, else, return 0
  \internal
*/
inline QScriptable *QtInstanceData::toQScriptable()
{
    Q_ASSERT(m_cppObject);
    void *ptr = m_cppObject->qt_metacast("QScriptable");
    return reinterpret_cast<QScriptable*>(ptr);
}

inline QtMetaObjectData::QtMetaObjectData(QScriptEnginePrivate *engine, const QMetaObject *mo, v8::Handle<v8::Value> ctor)
    : QtData<QtMetaObjectData>(engine)
    , m_metaObject(mo)
    , m_ctor(v8::Persistent<v8::Value>::New(ctor))
{
    Q_ASSERT(!ctor.IsEmpty());
}

inline QtMetaObjectData::~QtMetaObjectData()
{
    if (!engine()->isDestroyed())
        m_ctor.Dispose();
}

inline const QMetaObject *QtMetaObjectData::metaObject() const
{
    return m_metaObject;
}

inline v8::Handle<v8::Value> QtMetaObjectData::constructor() const
{
    return m_ctor;
}

inline QtVariantData::QtVariantData(QScriptEnginePrivate *engine, const QVariant &value)
    : QtData<QtVariantData>(engine)
    , m_value(value)
{}

inline QVariant &QtVariantData::value()
{
    return m_value;
}

inline void QtVariantData::setValue(const QVariant &value)
{
    m_value = value;
}

#endif // QSCRIPTQOBJECT_IMPL_P_H
