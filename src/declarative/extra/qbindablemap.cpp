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

#include "qbindablemap.h"
#include <qmlopenmetaobject.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

//QBindableMapMetaObject lets us listen for changes coming from QML
//so we can emit the changed signal.
class QBindableMapMetaObject : public QmlOpenMetaObject
{
public:
    QBindableMapMetaObject(QBindableMap *obj) : QmlOpenMetaObject(obj)
    {
        map = obj;
    }

protected:
    virtual void propertyWrite(int index)
    {
        map->emitChanged(QLatin1String(name(index)));
    }

private:
    QBindableMap *map;
};

/*!
    \class QBindableMap
    \brief The QBindableMap class allows you to set key-value pairs that can be used in bindings.

    QBindableMap provides a convenient way to expose domain data to the UI layer.
    The following example shows how you might declare data in C++ and then
    access it in QML.

    Setup in C++:
    \code
    //create our data
    QBindableMap ownerData;
    ownerData.setValue("name", QVariant(QString("John Smith")));
    ownerData.setValue("phone", QVariant(QString("555-5555")));

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

    To detect value changes made in the UI layer you can connect to the changed() signal.
    However, note that changed() is \b NOT emitted when changes are made by calling setValue()
    or clearValue() - it is only emitted when a value is updated from QML.
*/

// is there a more efficient way to store/return keys?
//        (or should we just provide an iterator or something else instead?)
// can we provide a way to clear keys?
// do we want to make any claims regarding key ordering?
// should we have signals for insertion and and deletion -- becoming more model like
// should we emit change for our own changes as well?
// Bug or Feature?: values can be created in QML (owner.somethingElse = "Hello") will create somethingElse property. (need to verify if this is actually the case)
// Bug or Feature?: all values are read-write (there are no read-only values)

/*!
    Constructs a bindable map with parent object \a parent.
*/
QBindableMap::QBindableMap(QObject *parent)
: QObject(parent)
{
    m_mo = new QBindableMapMetaObject(this);
}

/*!
    Destroys the bindable map.
*/
QBindableMap::~QBindableMap()
{
}

/*!
    Clears the value (if any) associated with \a key.
*/
void QBindableMap::clearValue(const QString &key)
{
    //m_keys.remove();   //###
    m_mo->setValue(key.toLatin1(), QVariant());
    //emit changed(key);
}

/*!
    Returns the value associated with \a key.

    If no value has been set for this key (or if the value has been cleared),
    an invalid QVariant is returned.
*/
QVariant QBindableMap::value(const QString &key) const
{
    return m_mo->value(key.toLatin1());
}

/*!
    Sets the value associated with \a key to \a value.

    If the key doesn't exist, it is automatically created.
*/
void QBindableMap::setValue(const QString &key, QVariant value)
{
    if (!m_keys.contains(key))
        m_keys.append(key);
    m_mo->setValue(key.toLatin1(), value);
    //emit changed(key);
}

/*!
    Returns the list of keys.

    Keys that have been cleared will still appear in this list, even though their
    associated values are invalid QVariants.
*/
QStringList QBindableMap::keys() const
{
    return m_keys;
}

/*!
    \fn void QBindableMap::changed(const QString &key)
    This signal is emitted whenever one of the values in the map is changed. \a key
    is the key corresponding to the value that was changed.
  */

void QBindableMap::emitChanged(const QString &key)
{
    emit changed(key);
}
QT_END_NAMESPACE
