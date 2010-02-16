/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#ifndef QMLPRIVATE_H
#define QMLPRIVATE_H

#include <QtCore/qglobal.h>

#ifndef Q_OS_WIN
#include <stdint.h>
#endif
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

typedef QObject *(*QmlAttachedPropertiesFunc)(QObject *);

//template<typename T>
//struct qml_hasAttached { static bool const value = false; };

template <typename TYPE>
class QmlTypeInfo
{
public:
    enum {
        hasAttachedProperties = 0
    };
};


namespace QmlPrivate
{
    class ListInterface
    {
    public:
        virtual ~ListInterface() {}
        virtual int type() const = 0;
        virtual void append(void *) = 0;
        virtual void insert(int, void *) = 0;
        virtual void removeAt(int) = 0;
        virtual void at(int, void *) const = 0;
        virtual int count() const = 0;
        virtual void clear() = 0;
    };

    enum ListOp { Append, Set, Insert, Prepend, Length, FromObject, 
                  Object, Create, Value, Clear };

    template<typename T>
    int list_op(ListOp op, int val, 
                const QVariant &vlist, 
                const QVariant &value, 
                void **out);

    template<typename T>
    int list_nocreate_op(ListOp op, int val, 
                         const QVariant &vlist, 
                         const QVariant &value, 
                         void **out);

    template<typename T>
    int list_interface_op(ListOp op, int val, 
                          const QVariant &vlist, 
                          const QVariant &value, 
                          void **out);

    template<class From, class To, int N>
    struct StaticCastSelectorClass
    {
        static inline int cast() { return -1; }
    };

    template<class From, class To>
    struct StaticCastSelectorClass<From, To, sizeof(int)>
    {
        static inline int cast() { return (int)((intptr_t)static_cast<To *>((From *)0x10000000)) - 0x10000000; }
    };

    template<class From, class To>
    struct StaticCastSelector
    {
        typedef int yes_type;
        typedef char no_type;

        static yes_type check(To *);
        static no_type check(...);

        static inline int cast()
        {
            return StaticCastSelectorClass<From, To, sizeof(check((From *)0))>::cast();
        }
    };

    template <typename T>
    struct has_attachedPropertiesMember
    {
        static bool const value = QmlTypeInfo<T>::hasAttachedProperties;
    };

    template <typename T, bool hasMember>
    class has_attachedPropertiesMethod 
    {
        typedef int yes_type;
        typedef char no_type;

        template<typename ReturnType>
        static yes_type check(ReturnType *(*)(QObject *));
        static no_type check(...);

    public:
        static bool const value = sizeof(check(&T::qmlAttachedProperties)) == sizeof(yes_type);
    }; 

    template <typename T>
    class has_attachedPropertiesMethod<T, false>
    {
    public:
        static bool const value = false;
    };

    template<typename T, int N>
    class AttachedPropertySelector
    {
    public:
        static inline QmlAttachedPropertiesFunc func() { return 0; }
        static inline const QMetaObject *metaObject() { return 0; }
    };
    template<typename T>
    class AttachedPropertySelector<T, 1>
    {
        static inline QObject *attachedProperties(QObject *obj) {
            return T::qmlAttachedProperties(obj);
        }
        template<typename ReturnType>
        static inline const QMetaObject *attachedPropertiesMetaObject(ReturnType *(*)(QObject *)) {
            return &ReturnType::staticMetaObject;
        }
    public:
        static inline QmlAttachedPropertiesFunc func() {
            return &attachedProperties;
        }
        static inline const QMetaObject *metaObject() {
            return attachedPropertiesMetaObject(&T::qmlAttachedProperties);
        }
    };

    template<typename T>
    inline QmlAttachedPropertiesFunc attachedPropertiesFunc()
    {
        return AttachedPropertySelector<T, has_attachedPropertiesMethod<T, has_attachedPropertiesMember<T>::value>::value>::func();
    }

    template<typename T>
    inline const QMetaObject *attachedPropertiesMetaObject()
    {
        return AttachedPropertySelector<T, has_attachedPropertiesMethod<T, has_attachedPropertiesMember<T>::value>::value>::metaObject();
    }

    struct MetaTypeIds {
        int typeId;
        int listId;
        int qmlListId;
    };
    typedef int (*Func)(QmlPrivate::ListOp, int, const QVariant &, const QVariant &, void **);
    typedef QObject *(*CreateFunc)(QObject *);

    template<typename T>
    struct CreateParent {
        static QObject *create(QObject *other) {
            return new T(other);
        }
    };

    template<typename T>
    struct CreateNoParent {
        static QObject *create() {
            return new T;
        }
    };

    struct Q_DECLARATIVE_EXPORT InstanceType {
       InstanceType(int);
    };

    template<typename T, int VMAJ, int VMIN>
    struct Define {
        static InstanceType instance;
    };
  
    template<typename T>
    struct ExtCreate {
        static QObject *create(QObject *other) {
            return new T(other);
        }
    };
}

template<typename T>
int QmlPrivate::list_op(QmlPrivate::ListOp op, int val, 
                        const QVariant &vlist, 
                        const QVariant &value, 
                        void **out) 
{ 
    if (op == QmlPrivate::Create) { 
        QObject *obj = static_cast<QObject *>(new T);
        *((QObject **)out) = obj; 
        return 0; 
    } 
    QList<T *> *list = vlist.value<QList<T *> *>(); 
    switch(op) { 
    case QmlPrivate::Append: 
        list->append(value.value<T *>()); 
        break; 
    case QmlPrivate::Set: 
        (*list)[val] = value.value<T *>(); 
        break; 
    case QmlPrivate::Insert: 
        list->insert(val, value.value<T *>()); 
        break; 
    case QmlPrivate::Prepend: 
        list->prepend(value.value<T *>()); 
        break; 
    case QmlPrivate::Length: 
        return list->count(); 
        break; 
    case QmlPrivate::Clear: 
        list->clear(); 
        return 0; 
        break; 
    case QmlPrivate::Create: 
        break; 
    case QmlPrivate::Object: 
        *out =  static_cast<QObject *>(value.value<T *>()); 
        break; 
    case QmlPrivate::FromObject: 
        { 
            QObject *fromObj = value.value<QObject *>(); 
            T *me = qobject_cast<T *>(fromObj); 
            if (me) { 
                *((QVariant *)*out) = QVariant::fromValue(me); 
            } 
        } 
        break; 
    case QmlPrivate::Value: 
        if (list->count() <= val) *((QVariant *)*out) = QVariant();
        else *((QVariant *)*out) = QVariant::fromValue(list->at(val)); 
        break; 
    } 
    return 0; 
} 

template<typename T>
int QmlPrivate::list_nocreate_op(QmlPrivate::ListOp op, int val, 
                                 const QVariant &vlist, 
                                 const QVariant &value, 
                                 void **out) 
{ 
    QList<T *> *list = vlist.value<QList<T *> *>(); 
    switch(op) { 
    case QmlPrivate::Append: 
        list->append(value.value<T *>()); 
        break; 
    case QmlPrivate::Set: 
        (*list)[val] = value.value<T *>(); 
        break; 
    case QmlPrivate::Insert: 
        list->insert(val, value.value<T *>()); 
        break; 
    case QmlPrivate::Prepend: 
        list->prepend(value.value<T *>()); 
        break; 
    case QmlPrivate::Length: 
        return list->count(); 
        break; 
    case QmlPrivate::Clear: 
        list->clear(); 
        return 0; 
        break; 
    case QmlPrivate::Create: 
        break; 
    case QmlPrivate::Object: 
        *out =  static_cast<QObject *>(value.value<T *>()); 
        break; 
    case QmlPrivate::FromObject: 
        { 
            QObject *fromObj = value.value<QObject *>(); 
            T *me = qobject_cast<T *>(fromObj); 
            if (me) { 
                *((QVariant *)*out) = QVariant::fromValue(me); 
            } 
        } 
        break; 
    case QmlPrivate::Value: 
        *((QVariant *)*out) = QVariant::fromValue(list->at(val)); 
        break; 
    } 
    return 0; 
} 

template<typename T>
int QmlPrivate::list_interface_op(QmlPrivate::ListOp op, int val, 
                                  const QVariant &vlist, 
                                  const QVariant &value, 
                                  void **out) 
{ 
    QList<T *> *list = vlist.value<QList<T *> *>(); 
    switch(op) { 
    case QmlPrivate::Append: 
        list->append(value.value<T *>()); 
        break; 
    case QmlPrivate::Set: 
        (*list)[val] = value.value<T *>(); 
        break; 
    case QmlPrivate::Insert: 
        list->insert(val, value.value<T *>()); 
        break; 
    case QmlPrivate::Prepend: 
        list->prepend(value.value<T *>()); 
        break; 
    case QmlPrivate::Length: 
        return list->count(); 
        break; 
    case QmlPrivate::Clear: 
        list->clear(); 
        return 0; 
        break; 
    case QmlPrivate::Create: 
        break; 
    case QmlPrivate::Object: 
        break; 
    case QmlPrivate::FromObject: 
        break; 
    case QmlPrivate::Value: 
        *((QVariant *)*out) = QVariant::fromValue(list->at(val)); 
        break; 
    } 
    return 0; 
} 

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLPRIVATE_H
