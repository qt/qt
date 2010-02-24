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

#ifndef QDECLARATIVEMETATYPE_H
#define QDECLARATIVEMETATYPE_H

#include "qdeclarativeprivate.h"
#include "qdeclarativeparserstatus.h"
#include "qdeclarativepropertyvaluesource.h"
#include "qdeclarativepropertyvalueinterceptor.h"
#include "qdeclarativelist.h"

#include <QtCore/qglobal.h>
#include <QtCore/qvariant.h>
#include <QtCore/qbitarray.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeType;
class QDeclarativeCustomParser;
class Q_DECLARATIVE_EXPORT QDeclarativeMetaType
{
public:
    static int registerType(const QDeclarativePrivate::MetaTypeIds &, QObject *(*)(), const char *, int vmaj, int vmin, const char *qmlName, const QMetaObject *, QDeclarativeAttachedPropertiesFunc, const QMetaObject *, int pStatus, int object, int valueSource, int valueInterceptor, QDeclarativePrivate::CreateFunc extFunc, const QMetaObject *extmo, QDeclarativeCustomParser *);
    static int registerInterface(const QDeclarativePrivate::MetaTypeIds &, const char *);

    static bool copy(int type, void *data, const void *copy = 0);

    static QList<QByteArray> qmlTypeNames();
    static QList<QDeclarativeType*> qmlTypes();

    static QDeclarativeType *qmlType(const QByteArray &, int, int);
    static QDeclarativeType *qmlType(const QMetaObject *);
    static QDeclarativeType *qmlType(int);

    static QMetaProperty defaultProperty(const QMetaObject *);
    static QMetaProperty defaultProperty(QObject *);
    static QMetaMethod defaultMethod(const QMetaObject *);
    static QMetaMethod defaultMethod(QObject *);

    static bool isQObject(int);
    static QObject *toQObject(const QVariant &, bool *ok = 0);

    static int listType(int);
    static int attachedPropertiesFuncId(const QMetaObject *);
    static QDeclarativeAttachedPropertiesFunc attachedPropertiesFuncById(int);

    enum TypeCategory { Unknown, Object, List };
    static TypeCategory typeCategory(int);
        
    static bool isInterface(int);
    static const char *interfaceIId(int);
    static bool isList(int);

    typedef QVariant (*StringConverter)(const QString &);
    static void registerCustomStringConverter(int, StringConverter);
    static StringConverter customStringConverter(int);
};

class QDeclarativeTypePrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeType
{
public:
    QByteArray typeName() const;
    QByteArray qmlTypeName() const;

    int majorVersion() const;
    int minorVersion() const;
    bool availableInVersion(int vmajor, int vminor) const;

    QObject *create() const;

    QDeclarativeCustomParser *customParser() const;

    bool isInterface() const;
    int typeId() const;
    int qListTypeId() const;

    const QMetaObject *metaObject() const;
    const QMetaObject *baseMetaObject() const;

    QDeclarativeAttachedPropertiesFunc attachedPropertiesFunction() const;
    const QMetaObject *attachedPropertiesType() const;

    int parserStatusCast() const;
    QVariant fromObject(QObject *) const;
    const char *interfaceIId() const;
    int propertyValueSourceCast() const;
    int propertyValueInterceptorCast() const;

    int index() const;
private:
    friend class QDeclarativeMetaType;
    friend class QDeclarativeTypePrivate;
    friend struct QDeclarativeMetaTypeData;
    QDeclarativeType(int, int, const char *, int);
    QDeclarativeType(int, int, QObject *(*)(), const char *, int, int, const QMetaObject *, QDeclarativeAttachedPropertiesFunc, const QMetaObject *, int, int, int, QDeclarativePrivate::CreateFunc, const QMetaObject *, int, QDeclarativeCustomParser *);
    ~QDeclarativeType();

    QDeclarativeTypePrivate *d;
};

template<typename T>
int qmlRegisterType(const char *typeName)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    return QDeclarativeMetaType::registerType(ids, 0, 0, 0, 0, 0,
            &T::staticMetaObject,
            QDeclarativePrivate::attachedPropertiesFunc<T>(),
            QDeclarativePrivate::attachedPropertiesMetaObject<T>(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QObject>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),
            0, 0, 0);
}

template<typename T>
int qmlRegisterType(const char *uri, int version_maj, int version_min, const char *qmlName, const char *typeName)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    return QDeclarativeMetaType::registerType(ids, QDeclarativePrivate::create<T>, 
            uri, version_maj, version_min, qmlName, 
            &T::staticMetaObject,
            QDeclarativePrivate::attachedPropertiesFunc<T>(),
            QDeclarativePrivate::attachedPropertiesMetaObject<T>(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QObject>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),
            0, 0, 0);
}

template<typename T, typename E>
int qmlRegisterExtendedType(const char *typeName)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    QDeclarativeAttachedPropertiesFunc attached = 
        QDeclarativePrivate::attachedPropertiesFunc<E>();
    const QMetaObject * attachedMo = 
        QDeclarativePrivate::attachedPropertiesMetaObject<E>();
    if (!attached) {
        attached = QDeclarativePrivate::attachedPropertiesFunc<T>();
        attachedMo = QDeclarativePrivate::attachedPropertiesMetaObject<T>();
    }

    return QDeclarativeMetaType::registerType(ids, 0, 0, 0, 0, 0,
            &T::staticMetaObject, attached, attachedMo,
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QObject>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),
            &QDeclarativePrivate::CreateParent<E>::create, &E::staticMetaObject, 0);
}

template<typename T, typename E>
int qmlRegisterExtendedType(const char *uri, int version_maj, int version_min, const char *qmlName, const char *typeName)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    QDeclarativeAttachedPropertiesFunc attached = 
        QDeclarativePrivate::attachedPropertiesFunc<E>();
    const QMetaObject * attachedMo = 
        QDeclarativePrivate::attachedPropertiesMetaObject<E>(); 
    if (!attached) {
        attached = QDeclarativePrivate::attachedPropertiesFunc<T>();
        attachedMo = QDeclarativePrivate::attachedPropertiesMetaObject<T>();
    }

    return QDeclarativeMetaType::registerType(ids, QDeclarativePrivate::create<T>, 
            uri, version_maj, version_min, qmlName, 
            &T::staticMetaObject,
            attached, attachedMo,
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QObject>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),
            &QDeclarativePrivate::CreateParent<E>::create, 
            &E::staticMetaObject, 0);
}

template<typename T>
int qmlRegisterInterface(const char *typeName)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    return QDeclarativeMetaType::registerInterface(ids, qobject_interface_iid<T *>());
}

template<typename T>
int qmlRegisterCustomType(const char *uri, int version_maj, int version_min, const char *qmlName, const char *typeName, QDeclarativeCustomParser *parser)
{
    QByteArray name(typeName);
    QDeclarativePrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + '*').constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(QByteArray("QDeclarativeListProperty<" + name + ">").constData()),
    };

    return QDeclarativeMetaType::registerType(ids, QDeclarativePrivate::create<T>, 
            uri, version_maj, version_min, qmlName, 
            &T::staticMetaObject,
            QDeclarativePrivate::attachedPropertiesFunc<T>(),
            QDeclarativePrivate::attachedPropertiesMetaObject<T>(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(), 
            QDeclarativePrivate::StaticCastSelector<T,QObject>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
            QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),
            0, 0, parser);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDECLARATIVEMETATYPE_H

