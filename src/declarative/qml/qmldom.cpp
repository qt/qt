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

#include "qmldom_p.h"
#include "qmldom_p_p.h"

#include "qmlcompositetypedata_p.h"
#include "qmlcompiler_p.h"
#include "qmlengine_p.h"
#include "qmlscriptparser_p.h"
#include "qmlglobal_p.h"

#include <QtCore/QByteArray>
#include <QtCore/QDebug>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

QmlDomDocumentPrivate::QmlDomDocumentPrivate()
: root(0)
{
}

QmlDomDocumentPrivate::~QmlDomDocumentPrivate()
{
    if (root) root->release();
}

/*!
    \class QmlDomDocument
    \internal
    \brief The QmlDomDocument class represents the root of a QML document

    A QML document is a self-contained snippet of QML, usually contained in a
    single file. Each document has a root object, accessible through
    QmlDomDocument::rootObject().

    The QmlDomDocument class allows the programmer to inspect a QML document by
    calling QmlDomDocument::load().

    The following example loads a QML file from disk, and prints out its root
    object type and the properties assigned in the root object.
    \code
    QFile file(inputFileName);
    file.open(QIODevice::ReadOnly);
    QByteArray xmlData = file.readAll();

    QmlDomDocument document;
    document.load(qmlengine, xmlData);

    QmlDomObject rootObject = document.rootObject();
    qDebug() << rootObject.objectType();
    foreach(QmlDomProperty property, rootObject.properties())
        qDebug() << property.propertyName();
    \endcode
*/

/*!
    Construct an empty QmlDomDocument.
*/
QmlDomDocument::QmlDomDocument()
: d(new QmlDomDocumentPrivate)
{
}

/*!
    Create a copy of \a other QmlDomDocument.
*/
QmlDomDocument::QmlDomDocument(const QmlDomDocument &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomDocument
*/
QmlDomDocument::~QmlDomDocument()
{
}

/*!
    Assign \a other to this QmlDomDocument.
*/
QmlDomDocument &QmlDomDocument::operator=(const QmlDomDocument &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns all import statements in qml.
*/
QList<QmlDomImport> QmlDomDocument::imports() const
{
    return d->imports;
}

/*!
    Loads a QmlDomDocument from \a data.  \a data should be valid QML
    data.  On success, true is returned.  If the \a data is malformed, false
    is returned and QmlDomDocument::errors() contains an error description.

    \sa QmlDomDocument::loadError()
*/
bool QmlDomDocument::load(QmlEngine *engine, const QByteArray &data, const QUrl &url)
{
    d->errors.clear();
    d->imports.clear();

    QmlCompiledData *component = new QmlCompiledData(engine);
    QmlCompiler compiler;

    QmlCompositeTypeData *td = ((QmlEnginePrivate *)QmlEnginePrivate::get(engine))->typeManager.getImmediate(data, url);

    if(td->status == QmlCompositeTypeData::Error) {
        d->errors = td->errors;
        td->release();
        component->release();
        return false;
    } else if(td->status == QmlCompositeTypeData::Waiting ||
              td->status == QmlCompositeTypeData::WaitingResources) {
        QmlError error;
        error.setDescription(QLatin1String("QmlDomDocument supports local types only"));
        d->errors << error;
        td->release();
        component->release();
        return false;
    }

    compiler.compile(engine, td, component);

    if (compiler.isError()) {
        d->errors = compiler.errors();
        td->release();
        component->release();
        return false;
    }

    for (int i = 0; i < td->data.imports().size(); ++i) {
        QmlScriptParser::Import parserImport = td->data.imports().at(i);
        QmlDomImport domImport;
        domImport.d->type = static_cast<QmlDomImportPrivate::Type>(parserImport.type);
        domImport.d->uri = parserImport.uri;
        domImport.d->qualifier = parserImport.qualifier;
        domImport.d->version = parserImport.version;
        d->imports += domImport;
    }

    if (td->data.tree()) {
        d->root = td->data.tree();
        d->root->addref();
    }

    component->release();
    return true;
}

/*!
    Returns the last load errors.  The load errors will be reset after a
    successful call to load().

    \sa load()
*/
QList<QmlError> QmlDomDocument::errors() const
{
    return d->errors;
}

/*!
    Returns the document's root object, or an invalid QmlDomObject if the
    document has no root.

    In the sample QML below, the root object will be the QmlGraphicsItem type.
    \qml
Item {
    Text {
        text: "Hello World"
    }
}
    \endqml
*/
QmlDomObject QmlDomDocument::rootObject() const
{
    QmlDomObject rv;
    rv.d->object = d->root;
    if (rv.d->object) rv.d->object->addref();
    return rv;
}

QmlDomPropertyPrivate::QmlDomPropertyPrivate()
: property(0)
{
}

QmlDomPropertyPrivate::~QmlDomPropertyPrivate()
{
    if (property) property->release();
}

QmlDomDynamicPropertyPrivate::QmlDomDynamicPropertyPrivate():
        valid(false)
{
}

QmlDomDynamicPropertyPrivate::~QmlDomDynamicPropertyPrivate()
{
    if (valid && property.defaultValue) property.defaultValue->release();
}

/*!
    \class QmlDomProperty
    \internal
    \brief The QmlDomProperty class represents one property assignment in the
    QML DOM tree

    Properties in QML can be assigned QML \l {QmlDomValue}{values}.

    \sa QmlDomObject
*/

/*!
    Construct an invalid QmlDomProperty.
*/
QmlDomProperty::QmlDomProperty()
: d(new QmlDomPropertyPrivate)
{
}

/*!
    Create a copy of \a other QmlDomProperty.
*/
QmlDomProperty::QmlDomProperty(const QmlDomProperty &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomProperty.
*/
QmlDomProperty::~QmlDomProperty()
{
}

/*!
    Assign \a other to this QmlDomProperty.
*/
QmlDomProperty &QmlDomProperty::operator=(const QmlDomProperty &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if this is a valid QmlDomProperty, false otherwise.
*/
bool QmlDomProperty::isValid() const
{
    return d->property != 0;
}


/*!
    Return the name of this property.

    \qml
Text {
    x: 10
    y: 10
    font.bold: true
}
    \endqml

    As illustrated above, a property name can be a simple string, such as "x" or
    "y", or a more complex "dot property", such as "font.bold".  In both cases
    the full name is returned ("x", "y" and "font.bold") by this method.

    For dot properties, a split version of the name can be accessed by calling
    QmlDomProperty::propertyNameParts().

    \sa QmlDomProperty::propertyNameParts()
*/
QByteArray QmlDomProperty::propertyName() const
{
    return d->propertyName;
}

/*!
    Return the name of this property, split into multiple parts in the case
    of dot properties.

    \qml
Text {
    x: 10
    y: 10
    font.bold: true
}
    \endqml

    For each of the properties shown above, this method would return ("x"),
    ("y") and ("font", "bold").

    \sa QmlDomProperty::propertyName()
*/
QList<QByteArray> QmlDomProperty::propertyNameParts() const
{
    if (d->propertyName.isEmpty()) return QList<QByteArray>();
    else return d->propertyName.split('.');
}

/*!
    Return true if this property is used as a default property in the QML
    document.

    \qml
<Text text="hello"/>
<Text>hello</Text>
    \endqml

    The above two examples return the same DOM tree, except that the second has
    the default property flag set on the text property.  Observe that whether
    or not a property has isDefaultProperty set is determined by how the
    property is used, and not only by whether the property is the types default
    property.
*/
bool QmlDomProperty::isDefaultProperty() const
{
    return d->property && d->property->isDefault;
}

/*!
    Returns the QmlDomValue that is assigned to this property, or an invalid
    QmlDomValue if no value is assigned.
*/
QmlDomValue QmlDomProperty::value() const
{
    QmlDomValue rv;
    if (d->property) {
        rv.d->property = d->property;
        rv.d->value = d->property->values.at(0);
        rv.d->property->addref();
        rv.d->value->addref();
    }
    return rv;
}

/*!
    Returns the position in the input data where the property ID startd, or -1 if
 the property is invalid.
*/
int QmlDomProperty::position() const
{
    if (d && d->property) {
        return d->property->location.range.offset;
    } else
        return -1;
}

/*!
    Returns the length in the input data from where the property ID started upto
 the end of it, or -1 if the property is invalid.
*/
int QmlDomProperty::length() const
{
    if (d && d->property)
        return d->property->location.range.length;
    else
        return -1;
}

/*!
    Construct an invalid QmlDomDynamicProperty.
*/
QmlDomDynamicProperty::QmlDomDynamicProperty():
        d(new QmlDomDynamicPropertyPrivate)
{
}

/*!
    Create a copy of \a other QmlDomDynamicProperty.
*/
QmlDomDynamicProperty::QmlDomDynamicProperty(const QmlDomDynamicProperty &other):
        d(other.d)
{
}

/*!
    Destroy the QmlDomDynamicProperty.
*/
QmlDomDynamicProperty::~QmlDomDynamicProperty()
{
}

/*!
    Assign \a other to this QmlDomDynamicProperty.
*/
QmlDomDynamicProperty &QmlDomDynamicProperty::operator=(const QmlDomDynamicProperty &other)
{
    d = other.d;
    return *this;
}

bool QmlDomDynamicProperty::isValid() const
{
    return d && d->valid;
}

/*!
    Return the name of this dynamic property.

    \qml
Item {
    property int count: 10;
}
    \endqml

    As illustrated above, a dynamic property name can have a name and a
    default value ("10").
*/
QByteArray QmlDomDynamicProperty::propertyName() const
{
    if (isValid())
        return d->property.name;
    else
        return QByteArray();
}

/*!
   Returns the type of the dynamic property. Note that when the property is an
   alias property, this will return -1. Use QmlDomProperty::isAlias() to check
   if the property is an alias.
*/
int QmlDomDynamicProperty::propertyType() const
{
    if (isValid()) {
        switch (d->property.type) {
            case QmlParser::Object::DynamicProperty::Bool:
                return QMetaType::type("bool");

            case QmlParser::Object::DynamicProperty::Color:
                return QMetaType::type("QColor");

            case QmlParser::Object::DynamicProperty::Date:
                return QMetaType::type("QDate");

            case QmlParser::Object::DynamicProperty::Int:
                return QMetaType::type("int");

            case QmlParser::Object::DynamicProperty::Real:
                return QMetaType::type("double");

            case QmlParser::Object::DynamicProperty::String:
                return QMetaType::type("QString");

            case QmlParser::Object::DynamicProperty::Url:
                return QMetaType::type("QUrl");

            case QmlParser::Object::DynamicProperty::Variant:
                return QMetaType::type("QVariant");

            default:
                break;
        }
    }

    return -1;
}

QByteArray QmlDomDynamicProperty::propertyTypeName() const
{
    if (isValid()) 
        return d->property.customType;

    return QByteArray();
}

/*!
    Return true if this property is used as a default property in the QML
    document.

    \qml
<Text text="hello"/>
<Text>hello</Text>
    \endqml

    The above two examples return the same DOM tree, except that the second has
    the default property flag set on the text property.  Observe that whether
    or not a property has isDefaultProperty set is determined by how the
    property is used, and not only by whether the property is the types default
    property.
*/
bool QmlDomDynamicProperty::isDefaultProperty() const
{
    if (isValid())
        return d->property.isDefaultProperty;
    else
        return false;
}

/*!
    Returns the default value as a QmlDomProperty.
*/
QmlDomProperty QmlDomDynamicProperty::defaultValue() const
{
    QmlDomProperty rp;

    if (isValid() && d->property.defaultValue) {
        rp.d->property = d->property.defaultValue;
        rp.d->propertyName = propertyName();
        rp.d->property->addref();
    }

    return rp;
}

/*!
    Returns true if this dynamic property is an alias for another property,
    false otherwise.
*/
bool QmlDomDynamicProperty::isAlias() const
{
    if (isValid())
        return d->property.type == QmlParser::Object::DynamicProperty::Alias;
    else
        return false;
}

/*!
    Returns the position in the input data where the property ID startd, or 0 if
 the property is invalid.
*/
int QmlDomDynamicProperty::position() const
{
    if (isValid()) {
        return d->property.location.range.offset;
    } else
        return -1;
}

/*!
    Returns the length in the input data from where the property ID started upto
 the end of it, or 0 if the property is invalid.
*/
int QmlDomDynamicProperty::length() const
{
    if (isValid())
        return d->property.location.range.length;
    else
        return -1;
}

QmlDomObjectPrivate::QmlDomObjectPrivate()
: object(0)
{
}

QmlDomObjectPrivate::~QmlDomObjectPrivate()
{
    if (object) object->release();
}

QmlDomObjectPrivate::Properties
QmlDomObjectPrivate::properties() const
{
    Properties rv;

    for (QHash<QByteArray, QmlParser::Property *>::ConstIterator iter =
            object->properties.begin();
            iter != object->properties.end();
            ++iter) {

        rv << properties(*iter);

    }
    return rv;
}

QmlDomObjectPrivate::Properties
QmlDomObjectPrivate::properties(QmlParser::Property *property) const
{
    Properties rv;

    if (property->value) {

        for (QHash<QByteArray, QmlParser::Property *>::ConstIterator iter =
                property->value->properties.begin();
                iter != property->value->properties.end();
                ++iter) {

            rv << properties(*iter);

        }

        QByteArray name(property->name + '.');
        for (Properties::Iterator iter = rv.begin(); iter != rv.end(); ++iter)
            iter->second.prepend(name);

    } else {
        rv << qMakePair(property, property->name);
    }

    return rv;
}

/*!
    \class QmlDomObject
    \internal
    \brief The QmlDomObject class represents an object instantiation.

    Each object instantiated in a QML file has a corresponding QmlDomObject
    node in the QML DOM.

    In addition to the type information that determines the object to
    instantiate, QmlDomObject's also have a set of associated QmlDomProperty's.
    Each QmlDomProperty represents a QML property assignment on the instantiated
    object.  For example,

    \qml
QGraphicsWidget {
    opacity: 0.5
    size: "100x100"
}
    \endqml

    describes a single QmlDomObject - "QGraphicsWidget" - with two properties,
    "opacity" and "size".  Obviously QGraphicsWidget has many more properties than just
    these two, but the QML DOM representation only contains those assigned
    values (or bindings) in the QML file.
*/

/*!
    Construct an invalid QmlDomObject.
*/
QmlDomObject::QmlDomObject()
: d(new QmlDomObjectPrivate)
{
}

/*!
    Create a copy of \a other QmlDomObject.
*/
QmlDomObject::QmlDomObject(const QmlDomObject &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomObject.
*/
QmlDomObject::~QmlDomObject()
{
}

/*!
    Assign \a other to this QmlDomObject.
*/
QmlDomObject &QmlDomObject::operator=(const QmlDomObject &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if this is a valid QmlDomObject, false otherwise.
*/
bool QmlDomObject::isValid() const
{
    return d->object != 0;
}

/*!
    Returns the fully-qualified type name of this object.

    For example, the type of this object would be "Qt/4.6/Rectangle".
    \qml
Rectangle { }
    \endqml
*/
QByteArray QmlDomObject::objectType() const
{
    if (d->object) return d->object->typeName;
    else return QByteArray();
}

/*!
    Returns the type name as referenced in the qml file.

    For example, the type of this object would be "Rectangle".
    \qml
Rectangle { }
    \endqml
*/
QByteArray QmlDomObject::objectClassName() const
{
    if (d->object)
        return d->object->className;
    else
        return QByteArray();
}

int QmlDomObject::objectTypeMajorVersion() const
{
    if (d->object)
        return d->object->majorVersion;
    else
        return -1;
}

int QmlDomObject::objectTypeMinorVersion() const
{
    if (d->object)
        return d->object->minorVersion;
    else
        return -1;
}

/*!
    Returns the QML id assigned to this object, or an empty QByteArray if no id
    has been assigned.

    For example, the object id of this object would be "MyText".
    \qml
Text { id: myText }
    \endqml
*/
QString QmlDomObject::objectId() const
{
    if (d->object) {
        return d->object->id;
    } else {
        return QString();
    }
}

/*!
    Returns the list of assigned properties on this object.

    In the following example, "text" and "x" properties would be returned.
    \qml
Text {
    text: "Hello world!"
    x: 100
}
    \endqml
*/
QList<QmlDomProperty> QmlDomObject::properties() const
{
    QList<QmlDomProperty> rv;

    if (!d->object || isComponent())
        return rv;

    QmlDomObjectPrivate::Properties properties = d->properties();
    for (int ii = 0; ii < properties.count(); ++ii) {

        QmlDomProperty domProperty;
        domProperty.d->property = properties.at(ii).first;
        domProperty.d->property->addref();
        domProperty.d->propertyName = properties.at(ii).second;
        rv << domProperty;

    }

    if (d->object->defaultProperty) {
        QmlDomProperty domProperty;
        domProperty.d->property = d->object->defaultProperty;
        domProperty.d->property->addref();
        domProperty.d->propertyName = d->object->defaultProperty->name;
        rv << domProperty;
    }

    return rv;
}

/*!
    Returns the object's \a name property if a value has been assigned to
    it, or an invalid QmlDomProperty otherwise.

    In the example below, \c {object.property("source")} would return a valid
    QmlDomProperty, and \c {object.property("tile")} an invalid QmlDomProperty.

    \qml
Image { source: "sample.jpg" }
    \endqml
*/
QmlDomProperty QmlDomObject::property(const QByteArray &name) const
{
    QList<QmlDomProperty> props = properties();
    for (int ii = 0; ii < props.count(); ++ii)
        if (props.at(ii).propertyName() == name)
            return props.at(ii);
    return QmlDomProperty();
}

QList<QmlDomDynamicProperty> QmlDomObject::dynamicProperties() const
{
    QList<QmlDomDynamicProperty> properties;

    for (int i = 0; i < d->object->dynamicProperties.size(); ++i) {
        QmlDomDynamicProperty p;
        p.d = new QmlDomDynamicPropertyPrivate;
        p.d->property = d->object->dynamicProperties.at(i);
        p.d->valid = true;

        if (p.d->property.defaultValue)
            p.d->property.defaultValue->addref();

        properties.append(p);
    }

    return properties;
}

QmlDomDynamicProperty QmlDomObject::dynamicProperty(const QByteArray &name) const
{
    QmlDomDynamicProperty p;

    if (!isValid())
        return p;

    for (int i = 0; i < d->object->dynamicProperties.size(); ++i) {
        if (d->object->dynamicProperties.at(i).name == name) {
            p.d = new QmlDomDynamicPropertyPrivate;
            p.d->property = d->object->dynamicProperties.at(i);
            if (p.d->property.defaultValue) p.d->property.defaultValue->addref();
            p.d->valid = true;
        }
    }

    return p;
}

/*!
    Returns true if this object is a custom type.  Custom types are special
    types that allow embeddeding non-QML data, such as SVG or HTML data,
    directly into QML files.

    \note Currently this method will always return false, and is a placekeeper
    for future functionality.

    \sa QmlDomObject::customTypeData()
*/
bool QmlDomObject::isCustomType() const
{
    return false;
}

/*!
    If this object represents a custom type, returns the data associated with
    the custom type, otherwise returns an empty QByteArray().
    QmlDomObject::isCustomType() can be used to check if this object represents
    a custom type.
*/
QByteArray QmlDomObject::customTypeData() const
{
    return QByteArray();
}

/*!
    Returns true if this object is a sub-component object.  Sub-component
    objects can be converted into QmlDomComponent instances by calling
    QmlDomObject::toComponent().

    \sa QmlDomObject::toComponent()
*/
bool QmlDomObject::isComponent() const
{
    return (d->object && d->object->typeName == "Qt/Component");
}

/*!
    Returns a QmlDomComponent for this object if it is a sub-component, or
    an invalid QmlDomComponent if not.  QmlDomObject::isComponent() can be used
    to check if this object represents a sub-component.

    \sa QmlDomObject::isComponent()
*/
QmlDomComponent QmlDomObject::toComponent() const
{
    QmlDomComponent rv;
    if (isComponent())
        rv.d = d;
    return rv;
}

/*!
    Returns the position in the input data where the property assignment started
, or -1 if the property is invalid.
*/
int QmlDomObject::position() const
{
    if (d && d->object)
        return d->object->location.range.offset;
    else
        return -1;
}

/*!
    Returns the length in the input data from where the property assignment star
ted upto the end of it, or -1 if the property is invalid.
*/
int QmlDomObject::length() const
{
    if (d && d->object)
        return d->object->location.range.length;
    else
        return -1;
}

// Returns the URL of the type, if it is an external type, or an empty URL if
// not
QUrl QmlDomObject::url() const
{
    if (d && d->object)
        return d->object->url;
    else
        return QUrl();
}

QmlDomBasicValuePrivate::QmlDomBasicValuePrivate()
: value(0)
{
}

QmlDomBasicValuePrivate::~QmlDomBasicValuePrivate()
{
    if (value) value->release();
}

/*!
    \class QmlDomValueLiteral
    \internal
    \brief The QmlDomValueLiteral class represents a literal value.

    A literal value is a simple value, written inline with the QML.  In the
    example below, the "x", "y" and "color" properties are being assigned
    literal values.

    \qml
Rectangle {
    x: 10
    y: 10
    color: "red"
}
    \endqml
*/

/*!
    Construct an empty QmlDomValueLiteral.
*/
QmlDomValueLiteral::QmlDomValueLiteral():
    d(new QmlDomBasicValuePrivate)
{
}

/*!
    Create a copy of \a other QmlDomValueLiteral.
*/
QmlDomValueLiteral::QmlDomValueLiteral(const QmlDomValueLiteral &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomValueLiteral.
*/
QmlDomValueLiteral::~QmlDomValueLiteral()
{
}

/*!
    Assign \a other to this QmlDomValueLiteral.
*/
QmlDomValueLiteral &QmlDomValueLiteral::operator=(const QmlDomValueLiteral &other)
{
    d = other.d;
    return *this;
}

/*!
    Return the literal value.

    In the example below, the literal value will be the string "10".
    \qml
Rectangle { x: 10 }
    \endqml
*/
QString QmlDomValueLiteral::literal() const
{
    if (d->value) return d->value->primitive();
    else return QString();
}

/*!
    \class QmlDomValueBinding
    \internal
    \brief The QmlDomValueBinding class represents a property binding.

    A property binding is an ECMAScript expression assigned to a property.  In
    the example below, the "x" property is being assigned a property binding.

    \qml
Rectangle { x: Other.x }
    \endqml
*/

/*!
    Construct an empty QmlDomValueBinding.
*/
QmlDomValueBinding::QmlDomValueBinding():
        d(new QmlDomBasicValuePrivate)
{
}

/*!
    Create a copy of \a other QmlDomValueBinding.
*/
QmlDomValueBinding::QmlDomValueBinding(const QmlDomValueBinding &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomValueBinding.
*/
QmlDomValueBinding::~QmlDomValueBinding()
{
}

/*!
    Assign \a other to this QmlDomValueBinding.
*/
QmlDomValueBinding &QmlDomValueBinding::operator=(const QmlDomValueBinding &other)
{
    d = other.d;
    return *this;
}

/*!
    Return the binding expression.

    In the example below, the string "Other.x" will be returned.
    \qml
Rectangle { x: Other.x }
    \endqml
*/
QString QmlDomValueBinding::binding() const
{
    if (d->value)
        return d->value->value.asScript();
    else
        return QString();
}

/*!
    \class QmlDomValueValueSource
    \internal
    \brief The QmlDomValueValueSource class represents a value source assignment value.

    In QML, value sources are special value generating types that may be
    assigned to properties.  Value sources inherit the QmlPropertyValueSource
    class.  In the example below, the "x" property is being assigned the
    NumberAnimation value source.

    \qml
Rectangle {
    x: NumberAnimation {
        from: 0
        to: 100
        repeat: true
        running: true
    }
}
    \endqml
*/

/*!
    Construct an empty QmlDomValueValueSource.
*/
QmlDomValueValueSource::QmlDomValueValueSource():
        d(new QmlDomBasicValuePrivate)
{
}

/*!
    Create a copy of \a other QmlDomValueValueSource.
*/
QmlDomValueValueSource::QmlDomValueValueSource(const QmlDomValueValueSource &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomValueValueSource.
*/
QmlDomValueValueSource::~QmlDomValueValueSource()
{
}

/*!
    Assign \a other to this QmlDomValueValueSource.
*/
QmlDomValueValueSource &QmlDomValueValueSource::operator=(const QmlDomValueValueSource &other)
{
    d = other.d;
    return *this;
}

/*!
    Return the value source object.

    In the example below, an object representing the NumberAnimation will be
    returned.
    \qml
Rectangle {
    x: NumberAnimation {
        from: 0
        to: 100
        repeat: true
        running: true
    }
}
    \endqml
*/
QmlDomObject QmlDomValueValueSource::object() const
{
    QmlDomObject rv;
    if (d->value) {
        rv.d->object = d->value->object;
        rv.d->object->addref();
    }
    return rv;
}

/*!
    \class QmlDomValueValueInterceptor
    \internal
    \brief The QmlDomValueValueInterceptor class represents a value interceptor assignment value.

    In QML, value interceptor are special write-intercepting types that may be
    assigned to properties.  Value interceptor inherit the QmlPropertyValueInterceptor
    class.  In the example below, the "x" property is being assigned the
    Behavior value interceptor.

    \qml
Rectangle {
    x: Behavior { NumberAnimation { duration: 500 } }
}
    \endqml
*/

/*!
    Construct an empty QmlDomValueValueInterceptor.
*/
QmlDomValueValueInterceptor::QmlDomValueValueInterceptor():
        d(new QmlDomBasicValuePrivate)
{
}

/*!
    Create a copy of \a other QmlDomValueValueInterceptor.
*/
QmlDomValueValueInterceptor::QmlDomValueValueInterceptor(const QmlDomValueValueInterceptor &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomValueValueInterceptor.
*/
QmlDomValueValueInterceptor::~QmlDomValueValueInterceptor()
{
}

/*!
    Assign \a other to this QmlDomValueValueInterceptor.
*/
QmlDomValueValueInterceptor &QmlDomValueValueInterceptor::operator=(const QmlDomValueValueInterceptor &other)
{
    d = other.d;
    return *this;
}

/*!
    Return the value interceptor object.

    In the example below, an object representing the Behavior will be
    returned.
    \qml
Rectangle {
    x: Behavior { NumberAnimation { duration: 500 } }
}
    \endqml
*/
QmlDomObject QmlDomValueValueInterceptor::object() const
{
    QmlDomObject rv;
    if (d->value) {
        rv.d->object = d->value->object;
        rv.d->object->addref();
    }
    return rv;
}

QmlDomValuePrivate::QmlDomValuePrivate()
: property(0), value(0)
{
}

QmlDomValuePrivate::~QmlDomValuePrivate()
{
    if (property) property->release();
    if (value) value->release();
}

/*!
    \class QmlDomValue
    \internal
    \brief The QmlDomValue class represents a generic Qml value.

    QmlDomValue's can be assigned to QML \l {QmlDomProperty}{properties}.  In
    QML, properties can be assigned various different values, including basic
    literals, property bindings, property value sources, objects and lists of
    values.  The QmlDomValue class allows a programmer to determine the specific
    value type being assigned and access more detailed information through a
    corresponding value type class.

    For example, in the following example,

    \qml
Text {
    text: "Hello World!"
    y: Other.y
}
    \endqml

    The text property is being assigned a literal, and the y property a property
    binding.  To output the values assigned to the text and y properties in the
    above example from C++,

    \code
    QmlDomDocument document;
    QmlDomObject root = document.rootObject();

    QmlDomProperty text = root.property("text");
    if (text.value().isLiteral()) {
        QmlDomValueLiteral literal = text.value().toLiteral();
        qDebug() << literal.literal();
    }

    QmlDomProperty y = root.property("y");
    if (y.value().isBinding()) {
        QmlDomValueBinding binding = y.value().toBinding();
        qDebug() << binding.binding();
    }
    \endcode
*/

/*!
    Construct an invalid QmlDomValue.
*/
QmlDomValue::QmlDomValue()
: d(new QmlDomValuePrivate)
{
}

/*!
    Create a copy of \a other QmlDomValue.
*/
QmlDomValue::QmlDomValue(const QmlDomValue &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomValue
*/
QmlDomValue::~QmlDomValue()
{
}

/*!
    Assign \a other to this QmlDomValue.
*/
QmlDomValue &QmlDomValue::operator=(const QmlDomValue &other)
{
    d = other.d;
    return *this;
}

/*!
    \enum QmlDomValue::Type

    The type of the QmlDomValue node.

    \value Invalid The QmlDomValue is invalid.
    \value Literal The QmlDomValue is a literal value assignment.  Use QmlDomValue::toLiteral() to access the type instance.
    \value PropertyBinding The QmlDomValue is a property binding.  Use QmlDomValue::toBinding() to access the type instance.
    \value ValueSource The QmlDomValue is a property value source.  Use QmlDomValue::toValueSource() to access the type instance.
    \value ValueInterceptor The QmlDomValue is a property value interceptor.  Use QmlDomValue::toValueInterceptor() to access the type instance.
    \value Object The QmlDomValue is an object assignment.  Use QmlDomValue::toObject() to access the type instnace.
    \value List The QmlDomValue is a list of other values.  Use QmlDomValue::toList() to access the type instance.
*/

/*!
    Returns the type of this QmlDomValue.
*/
QmlDomValue::Type QmlDomValue::type() const
{
    if (d->property)
        if (QmlMetaType::isList(d->property->type) ||
           (d->property && d->property->values.count() > 1))
            return List;

    QmlParser::Value *value = d->value;
    if (!value && !d->property)
        return Invalid;

    switch(value->type) {
    case QmlParser::Value::Unknown:
        return Invalid;
    case QmlParser::Value::Literal:
        return Literal;
    case QmlParser::Value::PropertyBinding:
        return PropertyBinding;
    case QmlParser::Value::ValueSource:
        return ValueSource;
    case QmlParser::Value::ValueInterceptor:
        return ValueInterceptor;
    case QmlParser::Value::CreatedObject:
        return Object;
    case QmlParser::Value::SignalObject:
        return Invalid;
    case QmlParser::Value::SignalExpression:
        return Literal;
    case QmlParser::Value::Id:
        return Literal;
    }
    return Invalid;
}

/*!
    Returns true if this is an invalid value, otherwise false.
*/
bool QmlDomValue::isInvalid() const
{
    return type() == Invalid;
}

/*!
    Returns true if this is a literal value, otherwise false.
*/
bool QmlDomValue::isLiteral() const
{
    return type() == Literal;
}

/*!
    Returns true if this is a property binding value, otherwise false.
*/
bool QmlDomValue::isBinding() const
{
    return type() == PropertyBinding;
}

/*!
    Returns true if this is a value source value, otherwise false.
*/
bool QmlDomValue::isValueSource() const
{
    return type() == ValueSource;
}

/*!
    Returns true if this is a value interceptor value, otherwise false.
*/
bool QmlDomValue::isValueInterceptor() const
{
    return type() == ValueInterceptor;
}

/*!
    Returns true if this is an object value, otherwise false.
*/
bool QmlDomValue::isObject() const
{
    return type() == Object;
}

/*!
    Returns true if this is a list value, otherwise false.
*/
bool QmlDomValue::isList() const
{
    return type() == List;
}

/*!
    Returns a QmlDomValueLiteral if this value is a literal type, otherwise
    returns an invalid QmlDomValueLiteral.

    \sa QmlDomValue::type()
*/
QmlDomValueLiteral QmlDomValue::toLiteral() const
{
    QmlDomValueLiteral rv;
    if (type() == Literal) {
        rv.d->value = d->value;
        rv.d->value->addref();
    }
    return rv;
}

/*!
    Returns a QmlDomValueBinding if this value is a property binding type,
    otherwise returns an invalid QmlDomValueBinding.

    \sa QmlDomValue::type()
*/
QmlDomValueBinding QmlDomValue::toBinding() const
{
    QmlDomValueBinding rv;
    if (type() == PropertyBinding) {
        rv.d->value = d->value;
        rv.d->value->addref();
    }
    return rv;
}

/*!
    Returns a QmlDomValueValueSource if this value is a property value source
    type, otherwise returns an invalid QmlDomValueValueSource.

    \sa QmlDomValue::type()
*/
QmlDomValueValueSource QmlDomValue::toValueSource() const
{
    QmlDomValueValueSource rv;
    if (type() == ValueSource) {
        rv.d->value = d->value;
        rv.d->value->addref();
    }
    return rv;
}

/*!
    Returns a QmlDomValueValueInterceptor if this value is a property value interceptor
    type, otherwise returns an invalid QmlDomValueValueInterceptor.

    \sa QmlDomValue::type()
*/
QmlDomValueValueInterceptor QmlDomValue::toValueInterceptor() const
{
    QmlDomValueValueInterceptor rv;
    if (type() == ValueInterceptor) {
        rv.d->value = d->value;
        rv.d->value->addref();
    }
    return rv;
}

/*!
    Returns a QmlDomObject if this value is an object assignment type, otherwise
    returns an invalid QmlDomObject.

    \sa QmlDomValue::type()
*/
QmlDomObject QmlDomValue::toObject() const
{
    QmlDomObject rv;
    if (type() == Object) {
        rv.d->object = d->value->object;
        rv.d->object->addref();
    }
    return rv;
}

/*!
    Returns a QmlDomList if this value is a list type, otherwise returns an
    invalid QmlDomList.

    \sa QmlDomValue::type()
*/
QmlDomList QmlDomValue::toList() const
{
    QmlDomList rv;
    if (type() == List) {
        rv.d = d;
    }
    return rv;
}

/*!
    Returns the position in the input data where the property value startd, or -1
 if the value is invalid.
*/
int QmlDomValue::position() const
{
    if (type() == Invalid)
        return -1;
    else
        return d->value->location.range.offset;
}

/*!
    Returns the length in the input data from where the property value started u
pto the end of it, or -1 if the value is invalid.
*/
int QmlDomValue::length() const
{
    if (type() == Invalid)
        return -1;
    else
        return d->value->location.range.length;
}

/*!
    \class QmlDomList
    \internal
    \brief The QmlDomList class represents a list of values assigned to a QML property.

    Lists of values can be assigned to properties.  For example, the following
    example assigns multiple objects to Item's "children" property
    \qml
Item {
    children: [
        Text { },
        Rectangle { }
    ]
}
    \endqml

    Lists can also be implicitly created by assigning multiple
    \l {QmlDomValueValueSource}{value sources} or constants to a property.
    \qml
Item {
    x: 10
    x: NumberAnimation {
        running: false
        from: 0
        to: 100
    }
}
    \endqml
*/

/*!
    Construct an empty QmlDomList.
*/
QmlDomList::QmlDomList()
{
}

/*!
    Create a copy of \a other QmlDomList.
*/
QmlDomList::QmlDomList(const QmlDomList &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomList.
*/
QmlDomList::~QmlDomList()
{
}

/*!
    Assign \a other to this QmlDomList.
*/
QmlDomList &QmlDomList::operator=(const QmlDomList &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns the list of QmlDomValue's.
*/
QList<QmlDomValue> QmlDomList::values() const
{
    QList<QmlDomValue> rv;
    if (!d->property)
        return rv;

    for (int ii = 0; ii < d->property->values.count(); ++ii) {
        QmlDomValue v;
        v.d->value = d->property->values.at(ii);
        v.d->value->addref();
        rv << v;
    }

    return rv;
}

/*!
    Returns the position in the input data where the list started, or -1 if
 the property is invalid.
*/
int QmlDomList::position() const
{
    if (d && d->property) {
        return d->property->listValueRange.offset;
    } else
        return -1;
}

/*!
    Returns the length in the input data from where the list started upto
 the end of it, or 0 if the property is invalid.
*/
int QmlDomList::length() const
{
    if (d && d->property)
        return d->property->listValueRange.length;
    else
        return -1;
}

/*!
  Returns a list of positions of the commas in the QML file.
*/
QList<int> QmlDomList:: commaPositions() const
{
    if (d && d->property)
        return d->property->listCommaPositions;
    else
        return QList<int>();
}

/*!
    \class QmlDomComponent
    \internal
    \brief The QmlDomComponent class represents sub-component within a QML document.

    Sub-components are QmlComponents defined within a QML document.  The
    following example shows the definition of a sub-component with the id
    "listDelegate".

    \qml
Item {
    Component {
        id: listDelegate
        Text {
            text: modelData.text
        }
    }
}
    \endqml

    Like QmlDomDocument's, components contain a single root object.
*/

/*!
    Construct an empty QmlDomComponent.
*/
QmlDomComponent::QmlDomComponent()
{
}

/*!
    Create a copy of \a other QmlDomComponent.
*/
QmlDomComponent::QmlDomComponent(const QmlDomComponent &other)
: QmlDomObject(other)
{
}

/*!
    Destroy the QmlDomComponent.
*/
QmlDomComponent::~QmlDomComponent()
{
}

/*!
    Assign \a other to this QmlDomComponent.
*/
QmlDomComponent &QmlDomComponent::operator=(const QmlDomComponent &other)
{
    static_cast<QmlDomObject &>(*this) = other;
    return *this;
}

/*!
    Returns the component's root object.

    In the example below, the root object is the "Text" object.
    \qml
Item {
    Component {
        id: listDelegate
        Text {
            text: modelData.text
        }
    }
}
    \endqml
*/
QmlDomObject QmlDomComponent::componentRoot() const
{
    QmlDomObject rv;
    if (d->object) {
        QmlParser::Object *obj = 0;
        if (d->object->defaultProperty &&
           d->object->defaultProperty->values.count() == 1 &&
           d->object->defaultProperty->values.at(0)->object)
            obj = d->object->defaultProperty->values.at(0)->object;

        if (obj) {
            rv.d->object = obj;
            rv.d->object->addref();
        }
    }

    return rv;
}

QmlDomImportPrivate::QmlDomImportPrivate()
: type(File)
{
}

QmlDomImportPrivate::~QmlDomImportPrivate()
{
}

/*!
    \class QmlDomImport
    \internal
    \brief The QmlDomImport class represents an import statement.
*/

/*!
    Construct an empty QmlDomImport.
*/
QmlDomImport::QmlDomImport()
: d(new QmlDomImportPrivate)
{
}

/*!
    Create a copy of \a other QmlDomImport.
*/
QmlDomImport::QmlDomImport(const QmlDomImport &other)
: d(other.d)
{
}

/*!
    Destroy the QmlDomImport.
*/
QmlDomImport::~QmlDomImport()
{
}

/*!
    Assign \a other to this QmlDomImport.
*/
QmlDomImport &QmlDomImport::operator=(const QmlDomImport &other)
{
    d = other.d;
    return *this;
}

/*!
  Returns the type of the import.
  */
QmlDomImport::Type QmlDomImport::type() const
{
    return static_cast<QmlDomImport::Type>(d->type);
}

/*!
  Returns the URI of the import (e.g. 'subdir' or 'com.nokia.Qt')
  */
QString QmlDomImport::uri() const
{
    return d->uri;
}

/*!
  Returns the version specified by the import. An empty string if no version was specified.
  */
QString QmlDomImport::version() const
{
    return d->version;
}

/*!
  Returns the (optional) qualifier string (the token following the 'as' keyword) of the import.
  */
QString QmlDomImport::qualifier() const
{
    return d->qualifier;
}

QT_END_NAMESPACE
