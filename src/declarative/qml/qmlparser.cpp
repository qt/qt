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
#include <qmlbasicscript.h>
#include "private/qmetaobjectbuilder_p.h"
#include <private/qmlvmemetaobject_p.h>
#include "private/qmlxmlparser_p.h"
#include <private/qmlcompiler_p.h>

QT_BEGIN_NAMESPACE

using namespace QmlParser;

QmlParser::Object::Object()
: type(-1), metatype(0), extObject(0), defaultProperty(0), line(-1),
  dynamicPropertiesProperty(0), dynamicSignalsProperty(0)
{
}

QmlParser::Object::~Object() 
{ 
    if(defaultProperty) defaultProperty->release();
    foreach(Property *prop, properties)
        prop->release();
    if(dynamicPropertiesProperty) dynamicPropertiesProperty->release();
    if(dynamicSignalsProperty) dynamicSignalsProperty->release();
}

const QMetaObject *Object::metaObject() const
{
    if(extObject && metatype)
        return extObject;
    else
        return metatype;
}

QmlParser::Property *Object::getDefaultProperty()
{
    if(!defaultProperty)
        defaultProperty = new Property;
    return defaultProperty;
}

Property *QmlParser::Object::getProperty(const QByteArray &name, bool create)
{
    if(!properties.contains(name)) {
        if(create)
            properties.insert(name, new Property(name));
        else
            return 0;
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
  onValueChanged(o.onValueChanged),
  defaultValue(o.defaultValue)
{
}

QmlParser::Object::DynamicSignal::DynamicSignal()
{
}

QmlParser::Object::DynamicSignal::DynamicSignal(const DynamicSignal &o)
: name(o.name)
{
}

QmlParser::Property::Property()
: type(0), index(-1), value(0), isDefault(true), line(-1)
{
}

QmlParser::Property::Property(const QByteArray &n)
: type(0), index(-1), value(0), name(n), isDefault(false), line(-1) 
{
}

QmlParser::Property::~Property() 
{ 
    foreach(Value *value, values)
        value->release();
    if(value) value->release(); 
}

Object *QmlParser::Property::getValue()
{
    if(!value) value = new Object;
    return value;
}

void QmlParser::Property::addValue(Value *v)
{
    values << v;
}

QmlParser::Value::Value()
: type(Unknown), object(0), line(-1)
{
}

QmlParser::Value::~Value() 
{ 
    if(object) object->release();
}

QT_END_NAMESPACE
