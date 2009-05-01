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

#include "private/qobject_p.h"
#include "qmlopenmetaobject.h"
#include "qmlsetproperties.h"
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlinfo.h>


QT_BEGIN_NAMESPACE
class QmlSetPropertiesMetaObject : public QmlOpenMetaObject
{
public:
    QmlSetPropertiesMetaObject(QObject *);

protected:
    virtual void propertyRead(int);
    virtual void propertyWrite(int);
};

class QmlSetPropertiesProxyObject : public QObject
{
Q_OBJECT
public:
    QmlSetPropertiesProxyObject(QObject *);

    QmlSetPropertiesMetaObject *fxMetaObject() const { return _mo; }
private:
    QmlSetPropertiesMetaObject *_mo;
};

QmlSetPropertiesProxyObject::QmlSetPropertiesProxyObject(QObject *parent)
: QObject(parent), _mo(new QmlSetPropertiesMetaObject(this))
{
}

QmlSetPropertiesMetaObject::QmlSetPropertiesMetaObject(QObject *obj)
: QmlOpenMetaObject(obj)
{
}

void QmlSetPropertiesMetaObject::propertyRead(int id)
{
    if (!value(id).isValid())  
        setValue(id, QVariant::fromValue((QObject *)new QmlSetPropertiesProxyObject(object())));

    QmlOpenMetaObject::propertyRead(id);
}

void QmlSetPropertiesMetaObject::propertyWrite(int id)
{
    if (value(id).userType() == qMetaTypeId<QObject *>()) {
        QObject *val = qvariant_cast<QObject *>(value(id));
        QmlSetPropertiesProxyObject *proxy = qobject_cast<QmlSetPropertiesProxyObject *>(val);
        if (proxy) {
            setValue(id, QVariant());
            delete proxy;
        }
    }
    QmlOpenMetaObject::propertyWrite(id);
}

/*!
    \qmlclass SetProperties QmlSetProperties
    \brief The SetProperties element describes new property values for a state.

    SetProperties is a convenience element for changing many properties on a single
    object. It allows you to specify the property names and values similar to how
    you normally would specify them for the actual item:

    \code
    SetProperties {
        target: myRect
        x: 52
        y: 300
        width: 48
    }
    \endcode

    \c target is a property of \c SetProperties, so if the property you want to change
    is named \e target you will have to use \l SetProperty instead. You should also 
    use \l SetProperty if you want to update the binding for a property, 
    as SetProperties does not support this.
*/

/*!
    \internal
    \class QmlSetProperties
    \brief The QmlSetProperties class describes new property values for a state.

    \ingroup group_states

    QmlSetProperties is a convenience class for changing many properties on a single
    object. It allows you to specify the property names and values similar to how
    you normally would specify them for the actual item:

    \code
    SetProperties {
        target: myRect
        x: 52
        y: 300
        width: 48
    }
    \endcode

    \c target is a property of \c SetProperties, so if the property you want to change
    is named \e target you will have to use QmlSetProperty instead. You should also use QmlSetProperty
    if you want to update the binding for a property, as QmlSetProperties does not support this.

    \sa QmlSetProperty
*/

class QmlSetPropertiesPrivate : public QObjectPrivate
{
public:
    QmlSetPropertiesPrivate() : obj(0), mo(0) {}

    QObject *obj;
    QmlSetPropertiesMetaObject *mo;
};

QML_DEFINE_TYPE(QmlSetProperties,SetProperties);
QmlSetProperties::QmlSetProperties()
    : QmlStateOperation(*(new QmlSetPropertiesPrivate))
{
    Q_D(QmlSetProperties);
    d->mo = new QmlSetPropertiesMetaObject(this);
}

QmlSetProperties::QmlSetProperties(QObject *parent)
    : QmlStateOperation(*(new QmlSetPropertiesPrivate), parent)
{
    Q_D(QmlSetProperties);
    d->mo = new QmlSetPropertiesMetaObject(this);
}

QmlSetProperties::~QmlSetProperties()
{
}

/*!
    \qmlproperty Object SetProperties::target
    This property holds the object that the properties to change belong to
*/

/*!
    \property QmlSetProperties::target
    \brief the object that the properties to change belong to
*/
QObject *QmlSetProperties::object()
{
    Q_D(QmlSetProperties);
    return d->obj;
}

void QmlSetProperties::setObject(QObject *o)
{
    Q_D(QmlSetProperties);
    d->obj = o;
}

QmlSetProperties::ActionList 
QmlSetProperties::doAction(QmlSetPropertiesMetaObject *metaObject, 
                           QObject *object)
{
    ActionList list;

    for (int ii = 0; ii < metaObject->count(); ++ii) {

        QByteArray name = metaObject->name(ii);
        QVariant value = metaObject->value(ii);

        QmlSetPropertiesProxyObject *po = qobject_cast<QmlSetPropertiesProxyObject *>(qvariant_cast<QObject *>(value));

        QmlMetaProperty prop(object, QLatin1String(name));

        if (po) {
            QObject *objVal = QmlMetaType::toQObject(prop.read());
            if (!objVal) {
                qmlInfo(this) << object->metaObject()->className()
                              << "has no object property named" << name;
                continue;
            }

            list << doAction(po->fxMetaObject(), objVal);
        } else if (!prop.isValid()) {
            qmlInfo(this) << object->metaObject()->className()
                          << "has no property named" << name;
            continue;
        } else if (!prop.isWritable()) {
            qmlInfo(this) << object->metaObject()->className()
                          << name << "is not writable, and cannot be set.";
            continue;
        } else {
            //append action
            Action a;
            a.property = prop;
            a.fromValue = prop.read();
            a.toValue = value;

            list << a;
        }
    }

    return list;
}

QmlSetProperties::ActionList QmlSetProperties::actions()
{
    Q_D(QmlSetProperties);
    if (!d->obj)
        return ActionList();

    return doAction(d->mo, d->obj);
}

QT_END_NAMESPACE
#include "qmlsetproperties.moc"
