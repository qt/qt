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

#include "qmlcustomparser_p.h"
#include "qmlcustomparser_p_p.h"

#include "qmlparser_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

using namespace QmlParser;

/*!
    \class QmlCustomParser
    \brief The QmlCustomParser class allows you to add new arbitrary types to QML.
    \internal

    By subclassing QmlCustomParser, you can add a parser for
    building a particular type.

    The subclass must implement compile() and setCustomData(), and define
    itself in the meta type system with the macro:

    \code
    QML_DEFINE_CUSTOM_TYPE(Module, MajorVersion, MinorVersion, Name, TypeClass, ParserClass)
    \endcode
*/

/*
    \fn QByteArray QmlCustomParser::compile(const QList<QmlCustomParserProperty> & properties)

    The custom parser processes \a properties, and returns
    a QByteArray containing data meaningful only to the
    custom parser; the type engine will pass this same data to
    setCustomData() when making an instance of the data.

    Errors must be reported via the error() functions.

    The QByteArray may be cached between executions of the system, so
    it must contain correctly-serialized data (not, for example,
    pointers to stack objects).
*/

/*
    \fn void QmlCustomParser::setCustomData(QObject *object, const QByteArray &data)

    This function sets \a object to have the properties defined
    by \a data, which is a block of data previously returned by a call
    to compile().

    The \a object will be an instance of the TypeClass specified by QML_DEFINE_CUSTOM_TYPE.
*/

QmlCustomParserNode 
QmlCustomParserNodePrivate::fromObject(QmlParser::Object *root)
{
    QmlCustomParserNode rootNode;
    rootNode.d->name = root->typeName;
    rootNode.d->location = root->location.start;

    for(QHash<QByteArray, Property *>::Iterator iter = root->properties.begin();
        iter != root->properties.end();
        ++iter) {

        Property *p = *iter;

        rootNode.d->properties << fromProperty(p);
    }

    return rootNode;
}

QmlCustomParserProperty 
QmlCustomParserNodePrivate::fromProperty(QmlParser::Property *p)
{
    QmlCustomParserProperty prop;
    prop.d->name = p->name;
    prop.d->isList = (p->values.count() > 1);
    prop.d->location = p->location.start;

    if (p->value) {
        QmlCustomParserNode node = fromObject(p->value);
        QList<QmlCustomParserProperty> props = node.properties();
        for (int ii = 0; ii < props.count(); ++ii)
            prop.d->values << QVariant::fromValue(props.at(ii));
    } else {
        for(int ii = 0; ii < p->values.count(); ++ii) {
            Value *v = p->values.at(ii);
            v->type = QmlParser::Value::Literal;

            if(v->object) {
                QmlCustomParserNode node = fromObject(v->object);
                prop.d->values << QVariant::fromValue(node);
            } else {
                prop.d->values << QVariant::fromValue(v->value);
            }

        }
    }

    return prop;
}

QmlCustomParserNode::QmlCustomParserNode()
: d(new QmlCustomParserNodePrivate)
{
}

QmlCustomParserNode::QmlCustomParserNode(const QmlCustomParserNode &other)
: d(new QmlCustomParserNodePrivate)
{
    *this = other;
}

QmlCustomParserNode &QmlCustomParserNode::operator=(const QmlCustomParserNode &other)
{
    d->name = other.d->name;
    d->properties = other.d->properties;
    d->location = other.d->location;
    return *this;
}

QmlCustomParserNode::~QmlCustomParserNode()
{
    delete d; d = 0;
}

QByteArray QmlCustomParserNode::name() const
{
    return d->name;
}

QList<QmlCustomParserProperty> QmlCustomParserNode::properties() const
{
    return d->properties;
}

QmlParser::Location QmlCustomParserNode::location() const
{
    return d->location;
}

QmlCustomParserProperty::QmlCustomParserProperty()
: d(new QmlCustomParserPropertyPrivate)
{
}

QmlCustomParserProperty::QmlCustomParserProperty(const QmlCustomParserProperty &other)
: d(new QmlCustomParserPropertyPrivate)
{
    *this = other;
}

QmlCustomParserProperty &QmlCustomParserProperty::operator=(const QmlCustomParserProperty &other)
{
    d->name = other.d->name;
    d->isList = other.d->isList;
    d->values = other.d->values;
    d->location = other.d->location;
    return *this;
}

QmlCustomParserProperty::~QmlCustomParserProperty()
{
    delete d; d = 0;
}

QByteArray QmlCustomParserProperty::name() const
{
    return d->name;
}

bool QmlCustomParserProperty::isList() const
{
    return d->isList;
}

QmlParser::Location QmlCustomParserProperty::location() const
{
    return d->location;
}

QList<QVariant> QmlCustomParserProperty::assignedValues() const
{
    return d->values;
}

void QmlCustomParser::clearErrors()
{
    exceptions.clear();
}

/*!
    Reports an error in parsing \a prop, with the given \a description.

    An error is generated referring to the position of \a node in the source file.
*/
void QmlCustomParser::error(const QmlCustomParserProperty& prop, const QString& description)
{
    QmlError error;
    QString exceptionDescription;
    error.setLine(prop.location().line);
    error.setColumn(prop.location().column);
    error.setDescription(description);
    exceptions << error;
}

/*!
    Reports an error in parsing \a node, with the given \a description.

    An error is generated referring to the position of \a node in the source file.
*/
void QmlCustomParser::error(const QmlCustomParserNode& node, const QString& description)
{
    QmlError error;
    QString exceptionDescription;
    error.setLine(node.location().line);
    error.setColumn(node.location().column);
    error.setDescription(description);
    exceptions << error;
}

QT_END_NAMESPACE
