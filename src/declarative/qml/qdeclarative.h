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

#ifndef QML_H
#define QML_H

#include "qdeclarativemetatype.h"
#include "qdeclarativemetaproperty.h"
#include "qdeclarativeparserstatus.h"
#include "qdeclarativepropertyvaluesource.h"
#include "qdeclarativelist.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qurl.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_HEADER

#define QML_DECLARE_TYPE(TYPE) \
    Q_DECLARE_METATYPE(TYPE *) \
    Q_DECLARE_METATYPE(QDeclarativeListProperty<TYPE>) 

#define QML_DECLARE_TYPE_HASMETATYPE(TYPE) \
    Q_DECLARE_METATYPE(QDeclarativeListProperty<TYPE>) 

#define QML_DECLARE_INTERFACE(INTERFACE) \
    QML_DECLARE_TYPE(INTERFACE)

#define QML_DECLARE_INTERFACE_HASMETATYPE(INTERFACE) \
    QML_DECLARE_TYPE_HASMETATYPE(INTERFACE)

enum { /* TYPEINFO flags */
    QML_HAS_ATTACHED_PROPERTIES = 0x01,
};

#define QML_DECLARE_TYPEINFO(TYPE, FLAGS) \
QT_BEGIN_NAMESPACE \
template <> \
class QDeclarativeTypeInfo<TYPE > \
{ \
public: \
    enum { \
        hasAttachedProperties = (((FLAGS) & QML_HAS_ATTACHED_PROPERTIES) == QML_HAS_ATTACHED_PROPERTIES) \
    }; \
}; \
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

#define QML_REGISTER_INTERFACE(INTERFACE) \
    qmlRegisterInterface<INTERFACE>(#INTERFACE)

#define QML_REGISTER_EXTENDED_TYPE(URI, VERSION_MAJ, VERSION_MIN, NAME, TYPE, EXTENSION) \
    qmlRegisterExtendedType<TYPE,EXTENSION>(#URI, VERSION_MAJ, VERSION_MIN, #NAME, #TYPE)

#define QML_REGISTER_TYPE(URI,VMAJ,VMIN,TYPE,CLASS) \
    qmlRegisterType<CLASS>(#URI, VMAJ, VMIN, #TYPE, #CLASS)

#define QML_REGISTER_NOCREATE_TYPE(CLASS) \
    qmlRegisterType<CLASS>(#CLASS)

class QDeclarativeContext;
class QDeclarativeEngine;
Q_DECLARATIVE_EXPORT void qmlExecuteDeferred(QObject *);
Q_DECLARATIVE_EXPORT QDeclarativeContext *qmlContext(const QObject *);
Q_DECLARATIVE_EXPORT QDeclarativeEngine *qmlEngine(const QObject *);
Q_DECLARATIVE_EXPORT QObject *qmlAttachedPropertiesObjectById(int, const QObject *, bool create = true);

template<typename T>
QObject *qmlAttachedPropertiesObject(const QObject *obj, bool create = true)
{
    // ### is this threadsafe?
    static int idx = -1;

    if (idx == -1)
        idx = QDeclarativeMetaType::attachedPropertiesFuncId(&T::staticMetaObject);

    if (idx == -1 || !obj)
        return 0;

    return qmlAttachedPropertiesObjectById(idx, obj, create);
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QObject)
Q_DECLARE_METATYPE(QVariant)

QT_END_HEADER

#endif // QML_H
