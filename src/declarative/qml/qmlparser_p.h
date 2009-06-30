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

#ifndef QMLPARSER_P_H
#define QMLPARSER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/qstring.h>
#include <QtDeclarative/qml.h>
#include <private/qmlrefcount_p.h>
#include <private/qobject_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

namespace QmlJS { namespace AST { class Node; } }

/*
    XXX

    These types are created (and owned) by the QmlXmlParser and consumed by the 
    QmlCompiler.  During the compilation phase the compiler will update some of
    the fields for both its own use and for the use of the upcoming QmlDom API.

    The types are part of the generic sounding "QmlParser" namespace for legacy 
    reasons (there used to be more in this namespace) and will be cleaned up and
    migrated into a more appropriate location shortly.
*/
namespace QmlParser
{
    struct Location 
    {
        Location() : line(-1), column(-1) {}
        int line;
        int column;
    };

    struct LocationRange
    {
        LocationRange() : offset(0), length(0) {}
        quint32 offset;
        quint32 length;
    };

    struct LocationSpan
    {
        Location start;
        Location end;
        LocationRange range;
    };

    class Property;
    class Object : public QmlRefCount
    {
    public:
        Object();
        virtual ~Object(); 

        // Type of the object.  The integer is an index into the 
        // QmlCompiledData::types array, or -1 if the object is a fetched
        // object.
        int type;
        // The url of this object if it is an external type.  Used by the DOM
        QUrl url;
        // The name of this type
        QByteArray typeName;
        // The id assigned to the object (if any).
        QByteArray id;
        // Custom parsed data
        QByteArray custom;
        // Returns the metaobject for this type, or 0 if not available.  
        // Internally selectd between the metatype and extObject variables
        const QMetaObject *metaObject() const;

        // The compile time metaobject for this type
        const QMetaObject *metatype;
        // The synthesized metaobject, if QML added signals or properties to
        // this type.  Otherwise null
        QMetaObject *extObjectData;
        QAbstractDynamicMetaObject extObject;

        Property *getDefaultProperty();
        Property *getProperty(const QByteArray &name, bool create=true);

        Property *defaultProperty;
        QHash<QByteArray, Property *> properties;

        LocationSpan location;

        struct DynamicProperty {
            DynamicProperty();
            DynamicProperty(const DynamicProperty &);

            enum Type { Variant, Int, Bool, Real, String, Url, Color, Date };

            bool isDefaultProperty;
            Type type;
            QByteArray name;
            QmlParser::Property *defaultValue;
            LocationRange range;
        };
        struct DynamicSignal {
            DynamicSignal();
            DynamicSignal(const DynamicSignal &);

            QByteArray name;
        };
        struct DynamicSlot {
            DynamicSlot();
            DynamicSlot(const DynamicSlot &);

            QByteArray name;
            QString body;
        };

        // The list of dynamic properties
        QList<DynamicProperty> dynamicProperties;
        // The list of dynamic signals
        QList<DynamicSignal> dynamicSignals;
        // The list of dynamic slots
        QList<DynamicSlot> dynamicSlots;

        void dump(int = 0) const;
    };

    class Variant 
    {
    public:
        enum Type {
            Invalid,
            Boolean,
            Number,
            String,
            Script
        };

        Variant();
        Variant(const Variant &);
        Variant(bool);
        Variant(double, const QString &asWritten=QString());
        Variant(const QString &);
        Variant(const QString &, QmlJS::AST::Node *);
        Variant &operator=(const Variant &);

        Type type() const;

        bool isBoolean() const { return type() == Boolean; }
        bool isNumber() const { return type() == Number; }
        bool isString() const { return type() == String; }
        bool isScript() const { return type() == Script; }

        bool asBoolean() const;
        QString asString() const;
        double asNumber() const;
        QString asScript() const;
        QmlJS::AST::Node *asAST() const;

    private:
        Type t;
        union {
            bool b;
            double d;
            QmlJS::AST::Node *n;
        };
        QString s;
    };

    class Value : public QmlRefCount
    {
    public:
        Value();
        virtual ~Value();

        enum Type {
            // The type of this value assignment is not yet known
            Unknown,
            // This is used as a literal property assignment
            Literal,
            // This is used as a property binding assignment
            PropertyBinding,
            // This is used as a QmlPropertyValueSource assignment
            ValueSource,
            // This is used as a property QObject assignment
            CreatedObject,
            // This is used as a signal object assignment
            SignalObject,
            // This is used as a signal expression assignment
            SignalExpression,
            // This is used as an implicit component creation
            Component,
            // This is used as an id assignment only
            Id
        };
        Type type;

        // ### Temporary
        QString primitive() const { return value.asScript(); }

        // Primitive value
        Variant value;
        // Object value
        Object *object;

        LocationSpan location;

        void dump(int = 0) const;
    };

    class Property : public QmlRefCount
    {
    public:
        Property();
        Property(const QByteArray &n);
        virtual ~Property();

        // The Object to which this property is attached
        Object *parent;

        Object *getValue();
        void addValue(Value *v);

        // The QVariant::Type of the property, or 0 (QVariant::Invalid) if 
        // unknown.
        int type;
        // The metaobject index of this property, or -1 if unknown.
        int index;

        // The list of values assigned to this property.  Content in values
        // and value are mutually exclusive
        QList<Value *> values;
        // The accessed property.  This is used to represent dot properties.
        // Content in value and values are mutually exclusive.
        Object *value;
        // The property name
        QByteArray name;
        // True if this property was accessed as the default property.  
        bool isDefault;

        LocationSpan location;
        LocationRange listValueRange;
        QList<int> listCommaPositions;

        void dump(int = 0) const;
    };
}
Q_DECLARE_METATYPE(QmlParser::Variant)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLPARSER_P_H
