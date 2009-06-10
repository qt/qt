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
#include <private/qmlcustomparser_p.h>
#include <private/qmlparser_p.h>
#include <QtDeclarative/qmlexpression.h>


QT_BEGIN_NAMESPACE
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

/*!
    \qmlproperty Object SetProperties::target
    This property holds the object that the properties to change belong to
*/

/*!
    \property QmlSetProperties::target
    \brief the object that the properties to change belong to
*/
class QmlSetPropertiesPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlSetProperties)
public:
    QmlSetPropertiesPrivate() : object(0), decoded(true), restore(true) {}

    QObject *object;
    QByteArray data;
    bool decoded;
    void decode();

    bool restore;

    QList<QPair<QByteArray, QVariant> > properties;
    QList<QPair<QByteArray, QmlExpression *> > expressions;

    QmlMetaProperty property(const QByteArray &);
};

class QmlSetPropertiesParser : public QmlCustomParser
{
public:
    void compileList(QList<QPair<QByteArray, QVariant> > &list, const QByteArray &pre, const QmlCustomParserProperty &prop);

    virtual QByteArray compile(const QList<QmlCustomParserProperty> &, bool *ok);
    virtual void setCustomData(QObject *, const QByteArray &);
};

void 
QmlSetPropertiesParser::compileList(QList<QPair<QByteArray, QVariant> > &list, 
                                     const QByteArray &pre, 
                                     const QmlCustomParserProperty &prop)
{
    QByteArray propName = pre + prop.name();

    QList<QVariant> values = prop.assignedValues();
    for (int ii = 0; ii < values.count(); ++ii) {
        const QVariant &value = values.at(ii);

        if (value.userType() == qMetaTypeId<QmlCustomParserNode>()) {
            continue;
        } else if(value.userType() == qMetaTypeId<QmlCustomParserProperty>()) {

            QmlCustomParserProperty prop = 
                qvariant_cast<QmlCustomParserProperty>(value);
            QByteArray pre = propName + ".";
            compileList(list, pre, prop);

        } else {
            list << qMakePair(propName, value);
        }
    }
}

QByteArray 
QmlSetPropertiesParser::compile(const QList<QmlCustomParserProperty> &props, 
                                bool *ok)
{
    *ok = true;

    QList<QPair<QByteArray, QVariant> > data;
    for(int ii = 0; ii < props.count(); ++ii)
        compileList(data, QByteArray(), props.at(ii));

    QByteArray rv;
    QDataStream ds(&rv, QIODevice::WriteOnly);

    ds << data.count();
    for(int ii = 0; ii < data.count(); ++ii) {
        QmlParser::Variant v = qvariant_cast<QmlParser::Variant>(data.at(ii).second);
        QVariant var;
        bool isScript = v.isScript();
        switch(v.type()) {
        case QmlParser::Variant::Boolean:
            var = QVariant(v.asBoolean());
            break;
        case QmlParser::Variant::Number:
            var = QVariant(v.asNumber());
            break;
        case QmlParser::Variant::String:
            var = QVariant(v.asString());
            break;
        case QmlParser::Variant::Invalid:
        case QmlParser::Variant::Script:
            var = QVariant(v.asScript());
            break;
        }

        ds << data.at(ii).first << isScript << var;
    }

    return rv;
}

void QmlSetPropertiesPrivate::decode()
{
    if (decoded)
        return;

    QDataStream ds(&data, QIODevice::ReadOnly);

    int count;
    ds >> count;
    for (int ii = 0; ii < count; ++ii) {
        QByteArray name;
        bool isScript;
        QVariant data;
        ds >> name;
        ds >> isScript;
        ds >> data;

        if (isScript) {
            QmlExpression *expression = new QmlExpression(qmlContext(object), data.toString(), object);
            expression->setTrackChange(false);
            expressions << qMakePair(name, expression);
        } else {
            properties << qMakePair(name, data);
        }
    }

    decoded = true;
    data.clear();
}

void QmlSetPropertiesParser::setCustomData(QObject *object, 
                                            const QByteArray &data)
{
    QmlSetPropertiesPrivate *p = 
        static_cast<QmlSetPropertiesPrivate *>(QObjectPrivate::get(object));
    p->data = data;
    p->decoded = false;
}

QmlSetProperties::QmlSetProperties()
: QmlStateOperation(*(new QmlSetPropertiesPrivate))
{
}

QmlSetProperties::~QmlSetProperties()
{
    Q_D(QmlSetProperties);
    for(int ii = 0; ii < d->expressions.count(); ++ii)
        delete d->expressions.at(ii).second;
}

QObject *QmlSetProperties::object() const
{
    Q_D(const QmlSetProperties);
    return d->object;
}

void QmlSetProperties::setObject(QObject *o)
{
    Q_D(QmlSetProperties);
    d->object = o;
}

bool QmlSetProperties::restoreEntryValues() const
{
    Q_D(const QmlSetProperties);
    return d->restore;
}

void QmlSetProperties::setRestoreEntryValues(bool v)
{
    Q_D(QmlSetProperties);
    d->restore = v;
}

QmlMetaProperty 
QmlSetPropertiesPrivate::property(const QByteArray &property) 
{
    Q_Q(QmlSetProperties);
    QList<QByteArray> path = property.split('.');

    QObject *obj = this->object;

    for (int jj = 0; jj < path.count() - 1; ++jj) {
        const QByteArray &pathName = path.at(jj);
        QmlMetaProperty prop(obj, QLatin1String(pathName));
        QObject *objVal = QmlMetaType::toQObject(prop.read());
        if (!objVal) {
            qmlInfo(q) << obj->metaObject()->className()
                       << "has no object property named" << pathName;
            return QmlMetaProperty();
        }
        obj = objVal;
    }

    const QByteArray &name = path.last();
    QmlMetaProperty prop(obj, QLatin1String(name));
    if (!prop.isValid()) {
        qmlInfo(q) << obj->metaObject()->className()
                   << "has no property named" << name;
        return QmlMetaProperty();
    } else if (!prop.isWritable()) {
        qmlInfo(q) << obj->metaObject()->className()
                   << name << "is not writable, and cannot be set.";
        return QmlMetaProperty();
    } else {
        return prop;
    }
}

QmlSetProperties::ActionList QmlSetProperties::actions()
{
    Q_D(QmlSetProperties);

    d->decode();

    ActionList list;

    for (int ii = 0; ii < d->properties.count(); ++ii) {

        QByteArray property = d->properties.at(ii).first;
        QmlMetaProperty prop = d->property(property);

        if (prop.isValid()) {
            Action a;
            a.restore = restoreEntryValues();
            a.property = prop;
            a.fromValue = a.property.read();
            a.toValue = d->properties.at(ii).second;

            list << a;
        }
    }

    for (int ii = 0; ii < d->expressions.count(); ++ii) {

        QByteArray property = d->expressions.at(ii).first;
        QmlMetaProperty prop = d->property(property);

        if (prop.isValid()) {
            Action a;
            a.restore = restoreEntryValues();
            a.property = prop;
            a.fromValue = a.property.read();
            a.toValue = d->expressions.at(ii).second->value();

            list << a;
        }

    }

    return list;
}

QML_DEFINE_CUSTOM_TYPE(QmlSetProperties,SetProperties,QmlSetPropertiesParser)

QT_END_NAMESPACE
