/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativelist.h"
#include "qdeclarativelist_p.h"
#include "qdeclarativeengine_p.h"
#include "qdeclarativemetaproperty_p.h"

QT_BEGIN_NAMESPACE

QDeclarativeListReferencePrivate::QDeclarativeListReferencePrivate()
: propertyType(-1), refCount(1)
{
}

QDeclarativeListReference QDeclarativeListReferencePrivate::init(const QDeclarativeListProperty<QObject> &prop, int propType, QDeclarativeEngine *engine)
{
    QDeclarativeListReference rv;

    if (!prop.object) return rv;

    QDeclarativeEnginePrivate *p = engine?QDeclarativeEnginePrivate::get(engine):0;

    int listType = p?p->listType(propType):QDeclarativeMetaType::listType(propType);
    if (listType == -1) return rv;

    rv.d = new QDeclarativeListReferencePrivate;
    rv.d->object = prop.object;
    rv.d->elementType = p?p->rawMetaObjectForType(listType):QDeclarativeMetaType::qmlType(listType)->baseMetaObject();
    rv.d->property = prop;
    rv.d->propertyType = propType;

    return rv;
}

void QDeclarativeListReferencePrivate::addref()
{
    Q_ASSERT(refCount > 0);
    ++refCount;
}

void QDeclarativeListReferencePrivate::release()
{
    Q_ASSERT(refCount > 0);
    --refCount;
    if (!refCount)
        delete this;
}

QDeclarativeListReference::QDeclarativeListReference()
: d(0)
{
}

QDeclarativeListReference::QDeclarativeListReference(QObject *o, const char *property, QDeclarativeEngine *engine)
: d(0)
{
    if (!o || !property) return;

    QDeclarativePropertyCache::Data local;
    QDeclarativePropertyCache::Data *data = QDeclarativePropertyCache::property(engine, o, QLatin1String(property), local);

    if (!data || !(data->flags & QDeclarativePropertyCache::Data::IsQList)) return;

    QDeclarativeEnginePrivate *p = engine?QDeclarativeEnginePrivate::get(engine):0;

    int listType = p?p->listType(data->propType):QDeclarativeMetaType::listType(data->propType);
    if (listType == -1) return;

    d = new QDeclarativeListReferencePrivate;
    d->object = o;
    d->elementType = p?p->rawMetaObjectForType(listType):QDeclarativeMetaType::qmlType(listType)->baseMetaObject();
    d->propertyType = data->propType;

    void *args[] = { &d->property, 0 };
    QMetaObject::metacall(o, QMetaObject::ReadProperty, data->coreIndex, args);
}

QDeclarativeListReference::QDeclarativeListReference(const QDeclarativeListReference &o)
: d(o.d)
{
    if (d) d->addref();
}

QDeclarativeListReference &QDeclarativeListReference::operator=(const QDeclarativeListReference &o)
{
    if (o.d) o.d->addref();
    if (d) d->release();
    d = o.d;
    return *this;
}

QDeclarativeListReference::~QDeclarativeListReference()
{
    if (d) d->release();
}

bool QDeclarativeListReference::isValid() const
{
    return d && d->object;
}

QObject *QDeclarativeListReference::object() const
{
    if (isValid()) return d->object;
    else return 0;
}

const QMetaObject *QDeclarativeListReference::listElementType() const
{
    if (isValid()) return d->elementType;
    else return 0;
}

bool QDeclarativeListReference::canAppend() const
{
    return (isValid() && d->property.append);
}

bool QDeclarativeListReference::canAt() const
{
    return (isValid() && d->property.at);
}

bool QDeclarativeListReference::canClear() const
{
    return (isValid() && d->property.clear);
}

bool QDeclarativeListReference::canCount() const
{
    return (isValid() && d->property.count);
}

bool QDeclarativeListReference::append(QObject *o) const
{
    if (!canAppend()) return false;

    if (o && !QDeclarativeMetaPropertyPrivate::canConvert(o->metaObject(), d->elementType))
        return false;

    d->property.append(&d->property, o);

    return true;
}

QObject *QDeclarativeListReference::at(int index) const
{
    if (!canAt()) return 0;

    return d->property.at(&d->property, index);
}

bool QDeclarativeListReference::clear() const
{
    if (!canClear()) return false;

    d->property.clear(&d->property);

    return true;
}

int QDeclarativeListReference::count() const
{
    if (!canCount()) return 0;

    return d->property.count(&d->property);
}

/*!
\class QDeclarativeListProperty
\brief The QDeclarativeListProperty class allows applications to explose list-like 
properties to QML.

QML has many list properties, where more than one object value can be assigned.
The use of a list property from QML looks like this:

\code
FruitBasket {
    fruit: [ 
             Apple {},
             Orange{},
             Banana {}
           ]
}
\endcode

The QDeclarativeListProperty encapsulates a group of function pointers that represet the
set of actions QML can perform on the list - adding items, retrieving items and
clearing the list.  In the future, additional operations may be supported.  All 
list properties must implement the append operation, but the rest are optional.

To provide a list property, a C++ class must implement the operation callbacks, 
and then return an appropriate QDeclarativeListProperty value from the property getter.
List properties should have no setter.  In the example above, the Q_PROPERTY()
declarative will look like this:

\code
Q_PROPERTY(QDeclarativeListProperty<Fruit> fruit READ fruit);
\endcode

QML list properties are typesafe - in this case \c {Fruit} is a QObject type that 
\c {Apple}, \c {Orange} and \c {Banana} all derive from.
*/

/*!
\fn QDeclarativeListProperty::QDeclarativeListProperty() 
\internal
*/

/*!
\fn QDeclarativeListProperty::QDeclarativeListProperty(QObject *object, QList<T *> &list)

Convenience constructor for making a QDeclarativeListProperty value from an existing
QList \a list.  The \a list reference must remain valid for as long as \a object
exists.  \a object must be provided.

Generally this constructor should not be used in production code, as a 
writable QList violates QML's memory management rules.  However, this constructor
can very useful while prototyping.
*/

/*!
\fn QDeclarativeListProperty::QDeclarativeListProperty(QObject *object, void *data, AppendFunction append, 
                                     CountFunction count = 0, AtFunction at = 0, 
                                     ClearFunction clear = 0)

Construct a QDeclarativeListProperty from a set of operation functions.  An opaque \a data handle
may be passed which can be accessed from within the operation functions.  The list property 
remains valid while \a object exists.

The \a append operation is compulsory and must be provided, while the \a count, \a at and
\a clear methods are optional.
*/

/*!
\typedef QDeclarativeListProperty::AppendFunction

Synonym for \c {void (*)(QDeclarativeListProperty<T> *property, T *value)}.

Append the \a value to the list \a property.
*/

/*!
\typedef QDeclarativeListProperty::CountFunction

Synonym for \c {int (*)(QDeclarativeListProperty<T> *property)}.

Return the number of elements in the list \a property.
*/

/*!
\fn bool QDeclarativeListProperty::operator==(const QDeclarativeListProperty &other) const 

Returns true if this QDeclarativeListProperty is equal to \a other, otherwise false.
*/

/*!
\typedef QDeclarativeListProperty::AtFunction

Synonym for \c {T *(*)(QDeclarativeListProperty<T> *property, int index)}.

Return the element at position \a index in the list \a property.
*/

/*!
\typedef QDeclarativeListProperty::ClearFunction

Synonym for \c {void (*)(QDeclarativeListProperty<T> *property)}.

Clear the list \a property.
*/

QT_END_NAMESPACE
