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

#include "qmlpropertymap.h"

#include "qmlopenmetaobject_p.h"

#include <QDebug>

QT_BEGIN_NAMESPACE

//QmlPropertyMapMetaObject lets us listen for changes coming from QML
//so we can emit the changed signal.
class QmlPropertyMapMetaObject : public QmlOpenMetaObject
{
public:
    QmlPropertyMapMetaObject(QmlPropertyMap *obj, QmlPropertyMapPrivate *objPriv);

protected:
    virtual void propertyWrite(int index);

private:
    QmlPropertyMap *map;
    QmlPropertyMapPrivate *priv;
};

class QmlPropertyMapPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlPropertyMap)
public:
    QmlPropertyMapMetaObject *mo;
    QStringList keys;
    void emitChanged(const QString &key);
};

void QmlPropertyMapPrivate::emitChanged(const QString &key)
{
    Q_Q(QmlPropertyMap);
    emit q->valueChanged(key);
}

QmlPropertyMapMetaObject::QmlPropertyMapMetaObject(QmlPropertyMap *obj, QmlPropertyMapPrivate *objPriv) : QmlOpenMetaObject(obj)
{
    map = obj;
    priv = objPriv;
}

void QmlPropertyMapMetaObject::propertyWrite(int index)
{
    priv->emitChanged(QString::fromUtf8(name(index)));
}

/*!
    \class QmlPropertyMap
    \brief The QmlPropertyMap class allows you to set key-value pairs that can be used in bindings.

    QmlPropertyMap provides a convenient way to expose domain data to the UI layer.
    The following example shows how you might declare data in C++ and then
    access it in QML.

    Setup in C++:
    \code
    //create our data
    QmlPropertyMap ownerData;
    ownerData.insert("name", QVariant(QString("John Smith")));
    ownerData.insert("phone", QVariant(QString("555-5555")));

    //expose it to the UI layer
    QmlContext *ctxt = view->bindContext();
    ctxt->setProperty("owner", &data);
    \endcode

    Then, in QML:
    \code
    Text { text: owner.name }
    Text { text: owner.phone }
    \endcode

    The binding is dynamic - whenever a key's value is updated, anything bound to that
    key will be updated as well.

    To detect value changes made in the UI layer you can connect to the valueChanged() signal.
    However, note that valueChanged() is \bold NOT emitted when changes are made by calling insert()
    or clear() - it is only emitted when a value is updated from QML.

    \note It is not possible to remove keys from the map; once a key has been added, you can only
    modify or clear its associated value.
*/

/*!
    Constructs a bindable map with parent object \a parent.
*/
QmlPropertyMap::QmlPropertyMap(QObject *parent)
: QObject(*(new QmlPropertyMapPrivate), parent)
{
    Q_D(QmlPropertyMap);
    d->mo = new QmlPropertyMapMetaObject(this, d);
}

/*!
    Destroys the bindable map.
*/
QmlPropertyMap::~QmlPropertyMap()
{
}

/*!
    Clears the value (if any) associated with \a key.
*/
void QmlPropertyMap::clear(const QString &key)
{
    Q_D(QmlPropertyMap);
    d->mo->setValue(key.toUtf8(), QVariant());
}

/*!
    Returns the value associated with \a key.

    If no value has been set for this key (or if the value has been cleared),
    an invalid QVariant is returned.
*/
QVariant QmlPropertyMap::value(const QString &key) const
{
    Q_D(const QmlPropertyMap);
    return d->mo->value(key.toUtf8());
}

/*!
    Sets the value associated with \a key to \a value.

    If the key doesn't exist, it is automatically created.
*/
void QmlPropertyMap::insert(const QString &key, const QVariant &value)
{
    Q_D(QmlPropertyMap);
    if (!d->keys.contains(key))
        d->keys.append(key);
    d->mo->setValue(key.toUtf8(), value);
}

/*!
    Returns the list of keys.

    Keys that have been cleared will still appear in this list, even though their
    associated values are invalid QVariants.
*/
QStringList QmlPropertyMap::keys() const
{
    Q_D(const QmlPropertyMap);
    return d->keys;
}

/*!
    \overload

    Same as size().
*/
int QmlPropertyMap::count() const
{
    Q_D(const QmlPropertyMap);
    return d->keys.count();
}

/*!
    Returns the number of keys in the map.

    \sa isEmpty(), count()
*/
int QmlPropertyMap::size() const
{
    Q_D(const QmlPropertyMap);
    return d->keys.size();
}

/*!
    Returns true if the map contains no keys; otherwise returns
    false.

    \sa size()
*/
bool QmlPropertyMap::isEmpty() const
{
    Q_D(const QmlPropertyMap);
    return d->keys.isEmpty();
}

/*!
    Returns true if the map contains \a key.

    \sa size()
*/
bool QmlPropertyMap::contains(const QString &key) const
{
    Q_D(const QmlPropertyMap);
    return d->keys.contains(key);
}

/*!
    Returns the value associated with the key \a key as a modifiable
    reference.

    If the map contains no item with key \a key, the function inserts
    an invalid QVariant into the map with key \a key, and
    returns a reference to it.

    \sa insert(), value()
*/
QVariant &QmlPropertyMap::operator[](const QString &key)
{
    //### optimize
    Q_D(QmlPropertyMap);
    QByteArray utf8key = key.toUtf8();
    if (!d->keys.contains(key)) {
        d->keys.append(key);
        d->mo->setValue(utf8key, QVariant());   //force creation -- needed below
    }

    return (*(d->mo))[utf8key];
}

/*!
    \overload

    Same as value().
*/
const QVariant QmlPropertyMap::operator[](const QString &key) const
{
    return value(key);
}

/*!
    \fn void QmlPropertyMap::valueChanged(const QString &key)
    This signal is emitted whenever one of the values in the map is changed. \a key
    is the key corresponding to the value that was changed.

    \note valueChanged() is \bold NOT emitted when changes are made by calling insert()
    or clear() - it is only emitted when a value is updated from QML.
*/

QT_END_NAMESPACE
