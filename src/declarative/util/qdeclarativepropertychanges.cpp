/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "private/qdeclarativepropertychanges_p.h"

#include "private/qdeclarativeopenmetaobject_p.h"

#include <qdeclarativeinfo.h>
#include <qdeclarativecustomparser_p.h>
#include <qdeclarativeparser_p.h>
#include <qdeclarativeexpression.h>
#include <qdeclarativebinding_p.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeguard_p.h>
#include <qdeclarativeproperty_p.h>
#include <qdeclarativecontext_p.h>

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass PropertyChanges QDeclarativePropertyChanges
    \since 4.7
    \brief The PropertyChanges element describes new property bindings or values for a state.

    PropertyChanges provides a state change that modifies the properties of an item.

    Here is a property change that modifies the text and color of a \l Text element
    when it is clicked:
    
    \qml
    Text {
        id: myText
        width: 100; height: 100
        text: "Hello"
        color: "blue"

        states: State {
            name: "myState"

            PropertyChanges {
                target: myText
                text: "Goodbye"
                color: "red"
            }
        }

        MouseArea { anchors.fill: parent; onClicked: myText.state = 'myState' }
    }
    \endqml

    By default, PropertyChanges will establish new bindings where appropriate.
    For example, the following creates a new binding for myItem's \c height property.

    \qml
    PropertyChanges {
        target: myItem
        height: parent.height
    }
    \endqml

    If you don't want a binding to be established (and instead just want to assign
    the value of the binding at the time the state is entered),
    you should set the PropertyChange's \l{PropertyChanges::explicit}{explicit}
    property to \c true.
    
    State-specific script for signal handlers can also be specified:

    \qml
    PropertyChanges {
        target: myMouseArea
        onClicked: doSomethingDifferent()
    }
    \endqml

    You can reset a property in a state change by assigning \c undefined. In the following
    example we reset \c theText's width when we enter state1. This will give the text its
    natural width (which is the whole string on one line).

    \qml
    import Qt 4.7

    Rectangle {
        width: 640
        height: 480
        Text {
            id: theText
            width: 50
            wrapMode: Text.WordWrap
            text: "a text string that is longer than 50 pixels"
        }

        states: State {
            name: "state1"
            PropertyChanges {
                target: theText
                width: undefined
            }
        }
    }
    \endqml

    Anchor margins should be changed with PropertyChanges, but other anchor changes or changes to
    an Item's parent should be done using the associated change elements
    (ParentChange and AnchorChanges, respectively).

    \sa {declarative/animation/states}{states example}, {qmlstate}{States}, QtDeclarative
*/

/*!
    \internal
    \class QDeclarativePropertyChanges
    \brief The QDeclarativePropertyChanges class describes new property values for a state.
*/

/*!
    \qmlproperty Object PropertyChanges::target
    This property holds the object which contains the properties to be changed.
*/

class QDeclarativeReplaceSignalHandler : public QDeclarativeActionEvent
{
public:
    QDeclarativeReplaceSignalHandler() : expression(0), reverseExpression(0),
                                rewindExpression(0), ownedExpression(0) {}
    ~QDeclarativeReplaceSignalHandler() {
        delete ownedExpression;
    }

    virtual QString typeName() const { return QLatin1String("ReplaceSignalHandler"); }

    QDeclarativeProperty property;
    QDeclarativeExpression *expression;
    QDeclarativeExpression *reverseExpression;
    QDeclarativeExpression *rewindExpression;
    QDeclarativeGuard<QDeclarativeExpression> ownedExpression;

    virtual void execute(Reason) {
        ownedExpression = QDeclarativePropertyPrivate::setSignalExpression(property, expression);
        if (ownedExpression == expression)
            ownedExpression = 0;
    }

    virtual bool isReversable() { return true; }
    virtual void reverse(Reason) {
        ownedExpression = QDeclarativePropertyPrivate::setSignalExpression(property, reverseExpression);
        if (ownedExpression == reverseExpression)
            ownedExpression = 0;
    }

    virtual void saveOriginals() {
        saveCurrentValues();
        reverseExpression = rewindExpression;
    }

    /*virtual void copyOriginals(QDeclarativeActionEvent *other)
    {
        QDeclarativeReplaceSignalHandler *rsh = static_cast<QDeclarativeReplaceSignalHandler*>(other);
        saveCurrentValues();
        if (rsh == this)
            return;
        reverseExpression = rsh->reverseExpression;
        if (rsh->ownedExpression == reverseExpression) {
            ownedExpression = rsh->ownedExpression;
            rsh->ownedExpression = 0;
        }
    }*/

    virtual void rewind() {
        ownedExpression = QDeclarativePropertyPrivate::setSignalExpression(property, rewindExpression);
        if (ownedExpression == rewindExpression)
            ownedExpression = 0;
    }
    virtual void saveCurrentValues() { 
        rewindExpression = QDeclarativePropertyPrivate::signalExpression(property);
    }

    virtual bool override(QDeclarativeActionEvent*other) {
        if (other == this)
            return true;
        if (other->typeName() != typeName())
            return false;
        if (static_cast<QDeclarativeReplaceSignalHandler*>(other)->property == property)
            return true;
        return false;
    }
};


class QDeclarativePropertyChangesPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePropertyChanges)
public:
    QDeclarativePropertyChangesPrivate() : object(0), decoded(true), restore(true),
                                isExplicit(false) {}

    QObject *object;
    QByteArray data;

    bool decoded : 1;
    bool restore : 1;
    bool isExplicit : 1;

    void decode();

    QList<QPair<QByteArray, QVariant> > properties;
    QList<QPair<QByteArray, QDeclarativeExpression *> > expressions;
    QList<QDeclarativeReplaceSignalHandler*> signalReplacements;

    QDeclarativeProperty property(const QByteArray &);
};

void
QDeclarativePropertyChangesParser::compileList(QList<QPair<QByteArray, QVariant> > &list,
                                     const QByteArray &pre,
                                     const QDeclarativeCustomParserProperty &prop)
{
    QByteArray propName = pre + prop.name();

    QList<QVariant> values = prop.assignedValues();
    for (int ii = 0; ii < values.count(); ++ii) {
        const QVariant &value = values.at(ii);

        if (value.userType() == qMetaTypeId<QDeclarativeCustomParserNode>()) {
            error(qvariant_cast<QDeclarativeCustomParserNode>(value),
                  QDeclarativePropertyChanges::tr("PropertyChanges does not support creating state-specific objects."));
            continue;
        } else if(value.userType() == qMetaTypeId<QDeclarativeCustomParserProperty>()) {

            QDeclarativeCustomParserProperty prop =
                qvariant_cast<QDeclarativeCustomParserProperty>(value);
            QByteArray pre = propName + '.';
            compileList(list, pre, prop);

        } else {
            list << qMakePair(propName, value);
        }
    }
}

QByteArray
QDeclarativePropertyChangesParser::compile(const QList<QDeclarativeCustomParserProperty> &props)
{
    QList<QPair<QByteArray, QVariant> > data;
    for(int ii = 0; ii < props.count(); ++ii)
        compileList(data, QByteArray(), props.at(ii));

    QByteArray rv;
    QDataStream ds(&rv, QIODevice::WriteOnly);

    ds << data.count();
    for(int ii = 0; ii < data.count(); ++ii) {
        QDeclarativeParser::Variant v = qvariant_cast<QDeclarativeParser::Variant>(data.at(ii).second);
        QVariant var;
        bool isScript = v.isScript();
        switch(v.type()) {
        case QDeclarativeParser::Variant::Boolean:
            var = QVariant(v.asBoolean());
            break;
        case QDeclarativeParser::Variant::Number:
            var = QVariant(v.asNumber());
            break;
        case QDeclarativeParser::Variant::String:
            var = QVariant(v.asString());
            break;
        case QDeclarativeParser::Variant::Invalid:
        case QDeclarativeParser::Variant::Script:
            var = QVariant(v.asScript());
            break;
        }

        ds << data.at(ii).first << isScript << var;
    }

    return rv;
}

void QDeclarativePropertyChangesPrivate::decode()
{
    Q_Q(QDeclarativePropertyChanges);
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

        QDeclarativeProperty prop = property(name);      //### better way to check for signal property?
        if (prop.type() & QDeclarativeProperty::SignalProperty) {
            QDeclarativeExpression *expression = new QDeclarativeExpression(qmlContext(q), object, data.toString());
            QDeclarativeData *ddata = QDeclarativeData::get(q);
            if (ddata && ddata->outerContext && !ddata->outerContext->url.isEmpty())
                expression->setSourceLocation(ddata->outerContext->url.toString(), ddata->lineNumber);
            QDeclarativeReplaceSignalHandler *handler = new QDeclarativeReplaceSignalHandler;
            handler->property = prop;
            handler->expression = expression;
            signalReplacements << handler;
        } else if (isScript) {
            QDeclarativeExpression *expression = new QDeclarativeExpression(qmlContext(q), object, data.toString());
            QDeclarativeData *ddata = QDeclarativeData::get(q);
            if (ddata && ddata->outerContext && !ddata->outerContext->url.isEmpty())
                expression->setSourceLocation(ddata->outerContext->url.toString(), ddata->lineNumber);
            expressions << qMakePair(name, expression);
        } else {
            properties << qMakePair(name, data);
        }
    }

    decoded = true;
    data.clear();
}

void QDeclarativePropertyChangesParser::setCustomData(QObject *object,
                                            const QByteArray &data)
{
    QDeclarativePropertyChangesPrivate *p =
        static_cast<QDeclarativePropertyChangesPrivate *>(QObjectPrivate::get(object));
    p->data = data;
    p->decoded = false;
}

QDeclarativePropertyChanges::QDeclarativePropertyChanges()
: QDeclarativeStateOperation(*(new QDeclarativePropertyChangesPrivate))
{
}

QDeclarativePropertyChanges::~QDeclarativePropertyChanges()
{
    Q_D(QDeclarativePropertyChanges);
    for(int ii = 0; ii < d->expressions.count(); ++ii)
        delete d->expressions.at(ii).second;
    for(int ii = 0; ii < d->signalReplacements.count(); ++ii)
        delete d->signalReplacements.at(ii);
}

QObject *QDeclarativePropertyChanges::object() const
{
    Q_D(const QDeclarativePropertyChanges);
    return d->object;
}

void QDeclarativePropertyChanges::setObject(QObject *o)
{
    Q_D(QDeclarativePropertyChanges);
    d->object = o;
}

/*!
    \qmlproperty bool PropertyChanges::restoreEntryValues
    
    Whether or not the previous values should be restored when
    leaving the state. By default, restoreEntryValues is true.

    By setting restoreEntryValues to false, you can create a temporary state
    that has permanent effects on property values.
*/
bool QDeclarativePropertyChanges::restoreEntryValues() const
{
    Q_D(const QDeclarativePropertyChanges);
    return d->restore;
}

void QDeclarativePropertyChanges::setRestoreEntryValues(bool v)
{
    Q_D(QDeclarativePropertyChanges);
    d->restore = v;
}

QDeclarativeProperty
QDeclarativePropertyChangesPrivate::property(const QByteArray &property)
{
    Q_Q(QDeclarativePropertyChanges);
    QDeclarativeProperty prop(object, QString::fromUtf8(property), qmlContext(q));
    if (!prop.isValid()) {
        qmlInfo(q) << QDeclarativePropertyChanges::tr("Cannot assign to non-existent property \"%1\"").arg(QString::fromUtf8(property));
        return QDeclarativeProperty();
    } else if (!(prop.type() & QDeclarativeProperty::SignalProperty) && !prop.isWritable()) {
        qmlInfo(q) << QDeclarativePropertyChanges::tr("Cannot assign to read-only property \"%1\"").arg(QString::fromUtf8(property));
        return QDeclarativeProperty();
    }
    return prop;
}

QDeclarativePropertyChanges::ActionList QDeclarativePropertyChanges::actions()
{
    Q_D(QDeclarativePropertyChanges);

    d->decode();

    ActionList list;

    for (int ii = 0; ii < d->properties.count(); ++ii) {

        QByteArray property = d->properties.at(ii).first;

        QDeclarativeAction a(d->object, QString::fromUtf8(property),
                 qmlContext(this), d->properties.at(ii).second);

        if (a.property.isValid()) {
            a.restore = restoreEntryValues();
            list << a;
        }
    }

    for (int ii = 0; ii < d->signalReplacements.count(); ++ii) {

        QDeclarativeReplaceSignalHandler *handler = d->signalReplacements.at(ii);

        if (handler->property.isValid()) {
            QDeclarativeAction a;
            a.event = handler;
            list << a;
        }
    }

    for (int ii = 0; ii < d->expressions.count(); ++ii) {

        QByteArray property = d->expressions.at(ii).first;
        QDeclarativeProperty prop = d->property(property);

        if (prop.isValid()) {
            QDeclarativeAction a;
            a.restore = restoreEntryValues();
            a.property = prop;
            a.fromValue = a.property.read();
            a.specifiedObject = d->object;
            a.specifiedProperty = QString::fromUtf8(property);

            if (d->isExplicit) {
                a.toValue = d->expressions.at(ii).second->evaluate();
            } else {
                QDeclarativeExpression *e = d->expressions.at(ii).second;
                QDeclarativeBinding *newBinding = 
                    new QDeclarativeBinding(e->expression(), object(), qmlContext(this));
                newBinding->setTarget(prop);
                newBinding->setSourceLocation(e->sourceFile(), e->lineNumber());
                a.toBinding = newBinding;
                a.deletableToBinding = true;
            }

            list << a;
        }
    }

    return list;
}

/*!
    \qmlproperty bool PropertyChanges::explicit

    If explicit is set to true, any potential bindings will be interpreted as
    once-off assignments that occur when the state is entered.

    In the following example, the addition of explicit prevents \c myItem.width from
    being bound to \c parent.width. Instead, it is assigned the value of \c parent.width
    at the time of the state change.
    \qml
    PropertyChanges {
        target: myItem
        explicit: true
        width: parent.width
    }
    \endqml

    By default, explicit is false.
*/
bool QDeclarativePropertyChanges::isExplicit() const
{
    Q_D(const QDeclarativePropertyChanges);
    return d->isExplicit;
}

void QDeclarativePropertyChanges::setIsExplicit(bool e)
{
    Q_D(QDeclarativePropertyChanges);
    d->isExplicit = e;
}

QT_END_NAMESPACE
