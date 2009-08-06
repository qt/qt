/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLMETATYPE_H
#define QMLMETATYPE_H

#include <QtCore/qglobal.h>
#include <QtCore/qvariant.h>
#include <QtCore/qbitarray.h>
#include <QtDeclarative/qmlprivate.h>
#include <QtDeclarative/qmlparserstatus.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlType;
class QmlCustomParser;
class Q_DECLARATIVE_EXPORT QmlMetaType
{
public:
    static int registerType(const QmlPrivate::MetaTypeIds &, QmlPrivate::Func, const char *, const char *version, const char *qmlName, const QMetaObject *, QmlAttachedPropertiesFunc, const QMetaObject *, int pStatus, int object, QmlPrivate::CreateFunc extFunc, const QMetaObject *extmo, QmlCustomParser *);
    static int registerType(const QmlPrivate::MetaTypeIds &, QmlPrivate::Func, const char *, int vmaj, int vmin_from, int vmin_to, const char *qmlName, const QMetaObject *, QmlAttachedPropertiesFunc, const QMetaObject *, int pStatus, int object, QmlPrivate::CreateFunc extFunc, const QMetaObject *extmo, QmlCustomParser *);
    static int registerInterface(const QmlPrivate::MetaTypeIds &, QmlPrivate::Func, const char *);

    static bool copy(int type, void *data, const void *copy = 0);

    static QList<QByteArray> qmlTypeNames();

    static QmlType *qmlType(const QByteArray &);
    static QmlType *qmlType(const QMetaObject *);

    static QMetaProperty defaultProperty(const QMetaObject *);
    static QMetaProperty defaultProperty(QObject *);
    static QMetaMethod defaultMethod(const QMetaObject *);
    static QMetaMethod defaultMethod(QObject *);
    static QMetaProperty property(QObject *, const QByteArray &);
    static QMetaProperty property(QObject *, const char *);
    static QObject *toQObject(const QVariant &);
    static int qmlParserStatusCast(int);
    static int listType(int);
    static int type(const QByteArray &);
    static int type(const QString &);
    static bool clear(const QVariant &);
    static bool append(const QVariant &, const QVariant &);
    static QVariant fromObject(QObject *, int type);
    static QObject *create(const QByteArray &);
    static const QMetaObject *rawMetaObjectForType(const QByteArray &);
    static const QMetaObject *rawMetaObjectForType(int);
    static const QMetaObject *metaObjectForType(const QByteArray &);
    static const QMetaObject *metaObjectForType(int);
    static int attachedPropertiesFuncId(const QByteArray &);
    static int attachedPropertiesFuncId(const QMetaObject *);
    static QmlAttachedPropertiesFunc attachedPropertiesFuncById(int);
    static QmlAttachedPropertiesFunc attachedPropertiesFunc(const QByteArray &);

    enum TypeCategory { Unknown, Object, List, QmlList };
    static TypeCategory typeCategory(int);
        
    static bool isInterface(int);
    static const char *interfaceIId(int);
    static bool isObject(int);
    static bool isObject(const QMetaObject *);
    static bool isList(int);
    static bool isList(const QVariant &);
    static bool isQmlList(int);
    static int qmlListType(int);
    static int listCount(const QVariant &);
    static QVariant listAt(const QVariant &, int);

    typedef QVariant (*StringConverter)(const QString &);
    static void registerCustomStringConverter(int, StringConverter);
    static StringConverter customStringConverter(int);
};

class QmlTypePrivate;
class Q_DECLARATIVE_EXPORT QmlType
{
public:
    QByteArray typeName() const;
    QByteArray qmlTypeName() const;

    QByteArray hash() const;

    QObject *create() const;

    QmlCustomParser *customParser() const;

    bool isInterface() const;
    int typeId() const;
    int qListTypeId() const;
    int qmlListTypeId() const;

    void listClear(const QVariant &);
    void listAppend(const QVariant &, const QVariant &);
    QVariant listAt(const QVariant &, int);
    int listCount(const QVariant &);

    const QMetaObject *metaObject() const;
    const QMetaObject *baseMetaObject() const;

    QmlAttachedPropertiesFunc attachedPropertiesFunction() const;
    const QMetaObject *attachedPropertiesType() const;

    int parserStatusCast() const;
    QVariant fromObject(QObject *) const;
    const char *interfaceIId() const;

    int index() const;
private:
    friend class QmlMetaType;
    friend class QmlTypePrivate;
    QmlType(int, int, int, QmlPrivate::Func, const char *, int);
    QmlType(int, int, int, QmlPrivate::Func, const char *, const QMetaObject *, QmlAttachedPropertiesFunc, const QMetaObject *, int, QmlPrivate::CreateFunc, const QMetaObject *, int, QmlCustomParser *);
    ~QmlType();

    QmlTypePrivate *d;
};

template<typename T>
int qmlRegisterType(const char *typeName)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    return QmlMetaType::registerType(ids, QmlPrivate::list_nocreate_op<T>, 0, 0, 0,
            &T::staticMetaObject,
            QmlPrivate::attachedPropertiesFunc<T>(),
            QmlPrivate::attachedPropertiesMetaObject<T>(),
            QmlPrivate::StaticCastSelector<T,QmlParserStatus>::cast(), 
            QmlPrivate::StaticCastSelector<T,QObject>::cast(), 
            0, 0, 0);
}

template<typename T>
int qmlRegisterType(const char *uri, int version_maj, int version_min_from, int version_min_to, const char *qmlName, const char *typeName)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    return QmlMetaType::registerType(ids, QmlPrivate::list_op<T>,
            uri, version_maj, version_min_from, version_min_to, qmlName, 
            &T::staticMetaObject,
            QmlPrivate::attachedPropertiesFunc<T>(),
            QmlPrivate::attachedPropertiesMetaObject<T>(),
            QmlPrivate::StaticCastSelector<T,QmlParserStatus>::cast(), 
            QmlPrivate::StaticCastSelector<T,QObject>::cast(), 
            0, 0, 0);
}

template<typename T, typename E>
int qmlRegisterExtendedType(const char *typeName)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    QmlAttachedPropertiesFunc attached = 
        QmlPrivate::attachedPropertiesFunc<E>();
    const QMetaObject * attachedMo = 
        QmlPrivate::attachedPropertiesMetaObject<E>();
    if (!attached) {
        attached = QmlPrivate::attachedPropertiesFunc<T>();
        attachedMo = QmlPrivate::attachedPropertiesMetaObject<T>();
    }

    return QmlMetaType::registerType(ids, QmlPrivate::list_nocreate_op<T>, 0, 0, 0,
            &T::staticMetaObject, attached, attachedMo,
            QmlPrivate::StaticCastSelector<T,QmlParserStatus>::cast(), 
            QmlPrivate::StaticCastSelector<T,QObject>::cast(), 
            &QmlPrivate::CreateParent<E>::create, &E::staticMetaObject, 0);
}

template<typename T, typename E>
int qmlRegisterExtendedType(const char *uri, int version_maj, int version_min_from, int version_min_to, const char *qmlName, const char *typeName)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    QmlAttachedPropertiesFunc attached = 
        QmlPrivate::attachedPropertiesFunc<E>();
    const QMetaObject * attachedMo = 
        QmlPrivate::attachedPropertiesMetaObject<E>(); 
    if (!attached) {
        attached = QmlPrivate::attachedPropertiesFunc<T>();
        attachedMo = QmlPrivate::attachedPropertiesMetaObject<T>();
    }

    return QmlMetaType::registerType(ids, QmlPrivate::list_op<T>, 
            uri, version_maj, version_min_from, version_min_to, qmlName, 
            &T::staticMetaObject,
            attached, attachedMo,
            QmlPrivate::StaticCastSelector<T,QmlParserStatus>::cast(), 
            QmlPrivate::StaticCastSelector<T,QObject>::cast(), 
            &QmlPrivate::CreateParent<E>::create, 
            &E::staticMetaObject, 0);
}

template<typename T>
int qmlRegisterInterface(const char *typeName)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    return QmlMetaType::registerInterface(ids, 
            QmlPrivate::list_interface_op<T>, 
            qobject_interface_iid<T *>());
}

template<typename T>
int qmlRegisterCustomType(const char *uri, int version_maj, int version_min_from, int version_min_to, const char *qmlName, const char *typeName, QmlCustomParser *parser)
{
    QByteArray name(typeName);
    QmlPrivate::MetaTypeIds ids = {
        qRegisterMetaType<T *>(QByteArray(name + "*").constData()),
        qRegisterMetaType<T *>(QByteArray("QList<" + name + "*>*").constData()),
        qRegisterMetaType<T *>(QByteArray("QmlList<" + name + "*>*").constData())
    };

    return QmlMetaType::registerType(ids, QmlPrivate::list_op<T>,
            uri, version_maj, version_min_from, version_min_to, qmlName, 
            &T::staticMetaObject,
            QmlPrivate::attachedPropertiesFunc<T>(),
            QmlPrivate::attachedPropertiesMetaObject<T>(),
            QmlPrivate::StaticCastSelector<T,QmlParserStatus>::cast(), 
            QmlPrivate::StaticCastSelector<T,QObject>::cast(), 
            0, 0, parser);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLMETATYPE_H

