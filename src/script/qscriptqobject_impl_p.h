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

template<class T>
QtData<T>::QtData(QScriptEnginePrivate *engine)
    : m_engine(engine)
{
}

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
    void *ptr = object->GetPointerFromInternalField(0);
    return static_cast<T*>(ptr);
}

template<class T>
void QtData<T>::set(v8::Handle<v8::Object> object, T* data)
{
    delete safeGet(object);
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

template<class T>
inline QScriptEnginePrivate *QtData<T>::engine() const
{
    return m_engine;
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

inline QObject *QtInstanceData::cppObject() const
{
    return m_cppObject;
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
