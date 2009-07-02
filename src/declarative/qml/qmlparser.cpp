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

#include "qmlparser_p.h"
#include <QStack>
#include <qmlpropertyvaluesource.h>
#include <QColor>
#include <QPointF>
#include <QSizeF>
#include <QRectF>
#include <private/qmlvme_p.h>
#include <qmlbindablevalue.h>
#include <qfxperf.h>
#include <qml.h>
#include "private/qmlcomponent_p.h"
#include <qmlcomponent.h>
#include "private/qmetaobjectbuilder_p.h"
#include <private/qmlvmemetaobject_p.h>
#include <private/qmlcompiler_p.h>
#include <QtDebug>

QT_BEGIN_NAMESPACE

using namespace QmlParser;

QmlParser::Object::Object()
: type(-1), metatype(0), extObjectData(0), defaultProperty(0)
{
}

QmlParser::Object::~Object() 
{ 
    if (defaultProperty) defaultProperty->release();
    foreach(Property *prop, properties)
        prop->release();
}

const QMetaObject *Object::metaObject() const
{
    if (extObjectData && metatype)
        return &extObject;
    else
        return metatype;
}

QmlParser::Property *Object::getDefaultProperty()
{
    if (!defaultProperty) {
        defaultProperty = new Property;
        defaultProperty->parent = this;
    }
    return defaultProperty;
}

Property *QmlParser::Object::getProperty(const QByteArray &name, bool create)
{
    if (!properties.contains(name)) {
        if (create) {
            Property *property = new Property(name);
            property->parent = this;
            properties.insert(name, property);
        } else {
            return 0;
        }
    }
    return properties[name];
}

QmlParser::Object::DynamicProperty::DynamicProperty()
: isDefaultProperty(false), type(Variant), defaultValue(0)
{
}

QmlParser::Object::DynamicProperty::DynamicProperty(const DynamicProperty &o)
: isDefaultProperty(o.isDefaultProperty),
  type(o.type),
  name(o.name),
  defaultValue(o.defaultValue),
  range(o.range)
{
}

QmlParser::Object::DynamicSignal::DynamicSignal()
{
}

QmlParser::Object::DynamicSignal::DynamicSignal(const DynamicSignal &o)
: name(o.name), parameterTypes(o.parameterTypes), 
  parameterNames(o.parameterNames)
{
}

QmlParser::Object::DynamicSlot::DynamicSlot()
{
}

QmlParser::Object::DynamicSlot::DynamicSlot(const DynamicSlot &o)
: name(o.name), body(o.body)
{
}

void QmlParser::Object::dump(int indent) const
{
    QByteArray ba(indent * 4, ' ');
    if (type != -1) {
        qWarning() << ba.constData() << "Object:" << typeName;
    } else {
        qWarning() << ba.constData() << "Object: fetched";
    }

    for (QHash<QByteArray, Property *>::ConstIterator iter = properties.begin();
            iter != properties.end();
            ++iter) {
        qWarning() << ba.constData() << " Property" << iter.key();
        (*iter)->dump(indent + 1);
    }

    if (defaultProperty) {
        qWarning() << ba.constData() << " Default property";
        defaultProperty->dump(indent + 1);
    }
}

QmlParser::Property::Property()
: parent(0), type(0), index(-1), value(0), isDefault(true)
{
}

QmlParser::Property::Property(const QByteArray &n)
: parent(0), type(0), index(-1), value(0), name(n), isDefault(false)
{
}

QmlParser::Property::~Property() 
{ 
    foreach(Value *value, values)
        value->release();
    if (value) value->release(); 
}

Object *QmlParser::Property::getValue()
{
    if (!value) value = new Object;
    return value;
}

void QmlParser::Property::addValue(Value *v)
{
    values << v;
}

void QmlParser::Property::dump(int indent) const
{
    QByteArray ba(indent * 4, ' ');
    for (int ii = 0; ii < values.count(); ++ii)
        values.at(ii)->dump(indent);
    if (value)
        value->dump(indent);
}

QmlParser::Value::Value()
: type(Unknown), object(0)
{
}

QmlParser::Value::~Value() 
{ 
    if (object) object->release();
}

void QmlParser::Value::dump(int indent) const
{
    QByteArray type;
    switch(this->type) {
    default:
    case Value::Unknown:
        type = "Unknown";
        break;
    case Value::Literal:
        type = "Literal";
        break;
    case Value::PropertyBinding:
        type = "PropertyBinding";
        break;
    case Value::ValueSource:
        type = "ValueSource";
        break;
    case Value::CreatedObject:
        type = "CreatedObject";
        break;
    case Value::SignalObject:
        type = "SignalObject";
        break;
    case Value::SignalExpression:
        type = "SignalExpression";
        break;
    case Value::Component:
        type = "Component";
        break;
    case Value::Id:
        type = "Id";
        break;
    }

    QByteArray primType;
    switch(this->value.type()) {
    default:
    case Variant::Invalid:
        primType = "Invalid";
        break;
    case Variant::Boolean:
        primType = "Boolean";
        break;
    case Variant::Number:
        primType = "Number";
        break;
    case Variant::String:
        primType = "String";
        break;
    case Variant::Script:
        primType = "Script";
        break;
    }

    QByteArray ba(indent * 4, ' ');
    if (object) {
        qWarning() << ba.constData() << "Value (" << type << "):";
        object->dump(indent + 1);
    } else {
        qWarning() << ba.constData() << "Value (" << type << "):" << primType.constData() << primitive();
    }
}

QmlParser::Variant::Variant()
: t(Invalid) {}

QmlParser::Variant::Variant(const Variant &o)
: t(o.t), d(o.d), s(o.s)
{
}

QmlParser::Variant::Variant(bool v)
: t(Boolean), b(v)
{
}

QmlParser::Variant::Variant(double v, const QString &asWritten)
: t(Number), d(v), s(asWritten)
{
}

QmlParser::Variant::Variant(const QString &v)
: t(String), s(v)
{
}

QmlParser::Variant::Variant(const QString &v, QmlJS::AST::Node *n)
: t(Script), n(n), s(v)
{
}

QmlParser::Variant &QmlParser::Variant::operator=(const Variant &o)
{
    t = o.t;
    d = o.d;
    s = o.s;
    return *this;
}

QmlParser::Variant::Type QmlParser::Variant::type() const
{
    return t;
}

bool QmlParser::Variant::asBoolean() const
{
    return b;
}

QString QmlParser::Variant::asString() const
{
    return s;
}

double QmlParser::Variant::asNumber() const
{
    return d;
}

QString QmlParser::Variant::asScript() const
{
    switch(type()) { 
    default:
    case Invalid:
        return QString();
    case Boolean:
        return b?QLatin1String("true"):QLatin1String("false");
    case Number:
        if (s.isEmpty())
            return QString::number(d);
        else
            return s;
    case String:
    case Script:
        return s;
    }
}

QmlJS::AST::Node *QmlParser::Variant::asAST() const
{
    if (type() == Script)
        return n;
    else
        return 0;
}

QT_END_NAMESPACE
