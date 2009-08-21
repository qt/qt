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
#include "qmlpropertychanges.h"
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlinfo.h>
#include <private/qmlcustomparser_p.h>
#include <private/qmlparser_p.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlbinding.h>
#include <QtDeclarative/qmlcontext.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass PropertyChanges QmlPropertyChanges
    \brief The PropertyChanges element describes new property values for a state.

    PropertyChanges changes the properties of an item. It allows you to specify the property
    names and values similar to how you normally would specify them for the actual item:

    \code
    PropertyChanges {
        target: myRect
        x: 52
        y: 300
        width: 48
    }
    \endcode
*/

/*!
    \internal
    \class QmlPropertyChanges
    \brief The QmlPropertyChanges class describes new property values for a state.
*/

/*!
    \qmlproperty Object PropertyChanges::target
    This property holds the object that the properties to change belong to
*/

class QmlPropertyChangesPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlPropertyChanges)
public:
    QmlPropertyChangesPrivate() : object(0), decoded(true), restore(true),
                                isExplicit(false) {}

    QObject *object;
    QByteArray data;
    bool decoded;
    void decode();

    bool restore;
    bool isExplicit;

    QList<QPair<QByteArray, QVariant> > properties;
    QList<QPair<QByteArray, QmlExpression *> > expressions;

    QmlMetaProperty property(const QByteArray &);
};

class QmlPropertyChangesParser : public QmlCustomParser
{
public:
    void compileList(QList<QPair<QByteArray, QVariant> > &list, const QByteArray &pre, const QmlCustomParserProperty &prop);

    virtual QByteArray compile(const QList<QmlCustomParserProperty> &, bool *ok);
    virtual void setCustomData(QObject *, const QByteArray &);
};

void
QmlPropertyChangesParser::compileList(QList<QPair<QByteArray, QVariant> > &list,
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
QmlPropertyChangesParser::compile(const QList<QmlCustomParserProperty> &props,
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

void QmlPropertyChangesPrivate::decode()
{
    Q_Q(QmlPropertyChanges);
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
            QmlExpression *expression = new QmlExpression(qmlContext(q), data.toString(), object);
            expression->setTrackChange(false);
            expressions << qMakePair(name, expression);
        } else {
            properties << qMakePair(name, data);
        }
    }

    decoded = true;
    data.clear();
}

void QmlPropertyChangesParser::setCustomData(QObject *object,
                                            const QByteArray &data)
{
    QmlPropertyChangesPrivate *p =
        static_cast<QmlPropertyChangesPrivate *>(QObjectPrivate::get(object));
    p->data = data;
    p->decoded = false;
}

QmlPropertyChanges::QmlPropertyChanges()
: QmlStateOperation(*(new QmlPropertyChangesPrivate))
{
}

QmlPropertyChanges::~QmlPropertyChanges()
{
    Q_D(QmlPropertyChanges);
    for(int ii = 0; ii < d->expressions.count(); ++ii)
        delete d->expressions.at(ii).second;
}

QObject *QmlPropertyChanges::object() const
{
    Q_D(const QmlPropertyChanges);
    return d->object;
}

void QmlPropertyChanges::setObject(QObject *o)
{
    Q_D(QmlPropertyChanges);
    d->object = o;
}

bool QmlPropertyChanges::restoreEntryValues() const
{
    Q_D(const QmlPropertyChanges);
    return d->restore;
}

void QmlPropertyChanges::setRestoreEntryValues(bool v)
{
    Q_D(QmlPropertyChanges);
    d->restore = v;
}

QmlMetaProperty
QmlPropertyChangesPrivate::property(const QByteArray &property)
{
    Q_Q(QmlPropertyChanges);
    QmlMetaProperty prop = QmlMetaProperty::createProperty(object, QString::fromLatin1(property));
    if (!prop.isValid()) {
        qmlInfo(q) << "Cannot assign to non-existant property" << property;
        return QmlMetaProperty();
    } else if (!prop.isWritable()) {
        qmlInfo(q) << "Cannot assign to read-only property" << property;
        return QmlMetaProperty();
    }
    return prop;
}

QmlPropertyChanges::ActionList QmlPropertyChanges::actions()
{
    Q_D(QmlPropertyChanges);

    d->decode();

    ActionList list;

    for (int ii = 0; ii < d->properties.count(); ++ii) {

        QByteArray property = d->properties.at(ii).first;

        Action a(d->object, QString::fromLatin1(property),
                 d->properties.at(ii).second);

        if (a.property.isValid()) {
            a.restore = restoreEntryValues();

            if (a.property.propertyType() == QVariant::Url &&
                (a.toValue.type() == QVariant::String || a.toValue.type() == QVariant::ByteArray) && !a.toValue.isNull())
                a.toValue.setValue(qmlContext(this)->resolvedUrl(QUrl(a.toValue.toString())));  //### d->object's context?

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
            a.specifiedObject = d->object;
            a.specifiedProperty = QString::fromLatin1(property);

            if (d->isExplicit) {
                a.toValue = d->expressions.at(ii).second->value();
            } else {
                QmlBinding *newBinding = new QmlBinding(d->expressions.at(ii).second->expression(), object(), qmlContext(this));
                newBinding->setTarget(prop);
                a.toBinding = newBinding;
                a.deletableToBinding = true;
            }

            list << a;
        }
    }

    return list;
}

bool QmlPropertyChanges::isExplicit() const
{
    Q_D(const QmlPropertyChanges);
    return d->isExplicit;
}

void QmlPropertyChanges::setIsExplicit(bool e)
{
    Q_D(QmlPropertyChanges);
    d->isExplicit = e;
}

QML_DEFINE_CUSTOM_TYPE(Qt, 4,6, (QT_VERSION&0x00ff00)>>8, PropertyChanges, QmlPropertyChanges, QmlPropertyChangesParser)

QT_END_NAMESPACE
