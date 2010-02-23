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

#include "qmllist.h"
#include "qmllist_p.h"
#include "qmlengine_p.h"
#include "qmlmetaproperty_p.h"

QT_BEGIN_NAMESPACE

QmlListReferencePrivate::QmlListReferencePrivate()
: propertyType(-1), refCount(1)
{
}

QmlListReference QmlListReferencePrivate::init(const QmlListProperty<QObject> &prop, int propType, QmlEngine *engine)
{
    QmlListReference rv;

    if (!prop.object) return rv;

    QmlEnginePrivate *p = engine?QmlEnginePrivate::get(engine):0;

    int listType = p?p->listType(propType):QmlMetaType::listType(propType);
    if (listType == -1) return rv;

    rv.d = new QmlListReferencePrivate;
    rv.d->object = prop.object;
    rv.d->elementType = p?p->rawMetaObjectForType(listType):QmlMetaType::qmlType(listType)->baseMetaObject();
    rv.d->property = prop;
    rv.d->propertyType = propType;

    return rv;
}

void QmlListReferencePrivate::addref()
{
    Q_ASSERT(refCount > 0);
    ++refCount;
}

void QmlListReferencePrivate::release()
{
    Q_ASSERT(refCount > 0);
    --refCount;
    if (!refCount)
        delete this;
}

QmlListReference::QmlListReference()
: d(0)
{
}

QmlListReference::QmlListReference(QObject *o, const char *property, QmlEngine *engine)
: d(0)
{
    if (!o || !property) return;

    QmlPropertyCache::Data local;
    QmlPropertyCache::Data *data = QmlPropertyCache::property(engine, o, QLatin1String(property), local);

    if (!data || !(data->flags & QmlPropertyCache::Data::IsQList)) return;

    QmlEnginePrivate *p = engine?QmlEnginePrivate::get(engine):0;

    int listType = p?p->listType(data->propType):QmlMetaType::listType(data->propType);
    if (listType == -1) return;

    d = new QmlListReferencePrivate;
    d->object = o;
    d->elementType = p?p->rawMetaObjectForType(listType):QmlMetaType::qmlType(listType)->baseMetaObject();
    d->propertyType = data->propType;

    void *args[] = { &d->property, 0 };
    QMetaObject::metacall(o, QMetaObject::ReadProperty, data->coreIndex, args);
}

QmlListReference::QmlListReference(const QmlListReference &o)
: d(o.d)
{
    if (d) d->addref();
}

QmlListReference &QmlListReference::operator=(const QmlListReference &o)
{
    if (o.d) o.d->addref();
    if (d) d->release();
    d = o.d;
    return *this;
}

QmlListReference::~QmlListReference()
{
    if (d) d->release();
}

bool QmlListReference::isValid() const
{
    return d && d->object;
}

QObject *QmlListReference::object() const
{
    if (isValid()) return d->object;
    else return 0;
}

const QMetaObject *QmlListReference::listElementType() const
{
    if (isValid()) return d->elementType;
    else return 0;
}

bool QmlListReference::canAppend() const
{
    return (isValid() && d->property.append);
}

bool QmlListReference::canAt() const
{
    return (isValid() && d->property.at);
}

bool QmlListReference::canClear() const
{
    return (isValid() && d->property.clear);
}

bool QmlListReference::canCount() const
{
    return (isValid() && d->property.count);
}

bool QmlListReference::append(QObject *o) const
{
    if (!canAppend()) return false;

    if (o && !QmlMetaPropertyPrivate::canConvert(o->metaObject(), d->elementType))
        return false;

    d->property.append(&d->property, o);

    return true;
}

QObject *QmlListReference::at(int index) const
{
    if (!canAt()) return 0;

    return d->property.at(&d->property, index);
}

bool QmlListReference::clear() const
{
    if (!canClear()) return false;

    d->property.clear(&d->property);

    return true;
}

int QmlListReference::count() const
{
    if (!canCount()) return 0;

    return d->property.count(&d->property);
}

/*!
\class QmlListProperty
\brief The QmlListProperty class allows applications to explose list-like 
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

The QmlListProperty encapsulates a group of function pointers that represet the
set of actions QML can perform on the list - adding items, retrieving items and
clearing the list.  In the future, additional operations may be supported.  All 
list properties must implement the append operation, but the rest are optional.

To provide a list property, a C++ class must implement the operation callbacks, 
and then return an appropriate QmlListProperty value from the property getter.
List properties should have no setter.  In the example above, the Q_PROPERTY()
declarative will look like this:

\code
Q_PROPERTY(QmlListProperty<Fruit> fruit READ fruit);
\endcode

QML list properties are typesafe - in this case \c {Fruit} is a QObject type that 
\c {Apple}, \c {Orange} and \c {Banana} all derive from.
*/

/*!
\fn QmlListProperty::QmlListProperty() 
\internal
*/

/*!
\fn QmlListProperty::QmlListProperty(QObject *object, QList<T *> &list)

Convenience constructor for making a QmlListProperty value from an existing
QList \a list.  The \a list reference must remain valid for as long as \a object
exists.  \a object must be provided.

Generally this constructor should not be used in production code, as a 
writable QList violates QML's memory management rules.  However, this constructor
can very useful while prototyping.
*/

/*!
\fn QmlListProperty::QmlListProperty(QObject *object, void *data, AppendFunction append, 
                                     CountFunction count = 0, AtFunction at = 0, 
                                     ClearFunction clear = 0)

Construct a QmlListProperty from a set of operation functions.  An opaque \a data handle
may be passed which can be accessed from within the operation functions.  The list property 
remains valid while \a object exists.

The \a append operation is compulsory and must be provided, while the \a count, \a at and
\a clear methods are optional.
*/

/*!
\typedef QmlListProperty::AppendFunction

Synonym for \c {void (*)(QmlListProperty<T> *property, T *value)}.

Append the \a value to the list \a property.
*/

/*!
\typedef QmlListProperty::CountFunction

Synonym for \c {int (*)(QmlListProperty<T> *property)}.

Return the number of elements in the list \a property.
*/

/*!
\fn bool QmlListProperty::operator==(const QmlListProperty &other) const 

Returns true if this QmlListProperty is equal to \a other, otherwise false.
*/

/*!
\typedef QmlListProperty::AtFunction

Synonym for \c {T *(*)(QmlListProperty<T> *property, int index)}.

Return the element at position \a index in the list \a property.
*/

/*!
\typedef QmlListProperty::ClearFunction

Synonym for \c {void (*)(QmlListProperty<T> *property)}.

Clear the list \a property.
*/

QT_END_NAMESPACE
