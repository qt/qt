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

    By subclassing QmlCustomParser, you can add an XML parser for
    building a particular type.

    The subclass must implement compile() and create(), and define
    itself in the meta type system with one of the macros:

    \code
    QML_DEFINE_CUSTOM_PARSER(Name, parserClass)
    \endcode

    \code
    QML_DEFINE_CUSTOM_PARSER_NS("XmlNamespaceUri", Name, parserClass)
    \endcode
*/

/*
    \fn QByteArray QmlCustomParser::compile(QXmlStreamReader& reader, bool *ok)

    Upon entry to this function, \a reader is positioned on a
    QXmlStreamReader::StartElement with the name specified when the
    class was defined with the QML_DEFINE_CUSTOM_PARSER macro.

    The custom parser must consume tokens from \a reader until the
    EndElement matching the initial start element is reached, or until
    error.

    On return, \c *ok indicates success.

    The returned QByteArray contains data meaningful only to the
    custom parser; the type engine will pass this same data to
    create() when making an instance of the data.

    The QByteArray may be cached between executions of the system, so
    it must contain correctly-serialized data (not, for example,
    pointers to stack objects).
*/

/*
    \fn QVariant QmlCustomParser::create(const QByteArray &data)

    This function returns a QVariant containing the value represented
    by \a data, which is a block of data previously returned by a call
    to compile().

    If the compile is for a type, the variant should be a pointer to
    the correctly-named QObject subclass (i.e. the one defined by
    QML_DEFINE_TYPE for the same-named type as this custom parser is
    defined for).
*/

QmlCustomParserNode 
QmlCustomParserNodePrivate::fromObject(QmlParser::Object *root)
{
    QmlCustomParserNode rootNode;
    rootNode.d->name = root->typeName;

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

QList<QVariant> QmlCustomParserProperty::assignedValues() const
{
    return d->values;
}

QByteArray QmlCustomParser::compile(const QList<QmlCustomParserProperty> &, bool *ok)
{
    Q_UNUSED(ok);
    return QByteArray();
}

void QmlCustomParser::setCustomData(QObject *, const QByteArray &)
{
}

QT_END_NAMESPACE
