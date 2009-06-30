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

#include <private/qobject_p.h>
#include <qml.h>
#include <QtDeclarative/qmlcontext.h>
#include "qmlbindablevalue.h"
#include "qmlstateoperations.h"
#include <QtCore/qdebug.h>
#include <QtDeclarative/qmlinfo.h>

QT_BEGIN_NAMESPACE

class QmlParentChangePrivate : public QObjectPrivate
{
public:
    QmlParentChangePrivate() : target(0), parent(0) {}

    QObject *target;
    QObject *parent;
};

/*!
    \preliminary
    \qmlclass ParentChange
    \brief The ParentChange element allows you to reparent an object in a state.
*/

QML_DEFINE_TYPE(QmlParentChange,ParentChange)
QmlParentChange::QmlParentChange(QObject *parent)
    : QmlStateOperation(*(new QmlParentChangePrivate), parent)
{
}

QmlParentChange::~QmlParentChange()
{
}

/*!
    \qmlproperty Object ParentChange::target
    This property holds the object to be reparented
*/

QObject *QmlParentChange::object() const
{
    Q_D(const QmlParentChange);
    return d->target;
}
void QmlParentChange::setObject(QObject *target)
{
    Q_D(QmlParentChange);
    d->target = target;
}

/*!
    \qmlproperty Object ParentChange::parent
    This property holds the parent for the object in this state
*/

QObject *QmlParentChange::parent() const
{
    Q_D(const QmlParentChange);
    return d->parent;
}

void QmlParentChange::setParent(QObject *parent)
{
    Q_D(QmlParentChange);
    d->parent = parent;
}

QmlStateOperation::ActionList QmlParentChange::actions()
{
    Q_D(QmlParentChange);
    if (!d->target || !d->parent)
        return ActionList();

    QString propName(QLatin1String("moveToParent"));
    QmlMetaProperty prop(d->target, propName);
    if (!prop.isValid()) {
        qmlInfo(this) << d->target->metaObject()->className()
                      << "has no property named" << propName;
        return ActionList();
    }else if (!prop.isWritable()){
        qmlInfo(this) << d->target->metaObject()->className() << propName
                      << "is not a writable property and cannot be set.";
        return ActionList();
    }
    QVariant cur = prop.read();

    Action a;
    a.property = prop;
    a.fromValue = cur;
    a.toValue = qVariantFromValue(d->parent);

    return ActionList() << a;
}

class QmlRunScriptPrivate : public QObjectPrivate
{
public:
    QmlRunScriptPrivate() {}

    QString script;
    QString name;
};

/*!
    \qmlclass RunScript QmlRunScript
    \brief The RunScript element allows you to run a script in a state.
*/
QML_DEFINE_TYPE(QmlRunScript,RunScript)
QmlRunScript::QmlRunScript(QObject *parent)
: QmlStateOperation(*(new QmlRunScriptPrivate), parent)
{
}

QmlRunScript::~QmlRunScript()
{
}

/*!
    \qmlproperty string RunScript::script
    This property holds the script to run when the state is current.
*/
QString QmlRunScript::script() const
{
    Q_D(const QmlRunScript);
    return d->script;
}

void QmlRunScript::setScript(const QString &s)
{
    Q_D(QmlRunScript);
    d->script = s;
}

QString QmlRunScript::name() const
{
    Q_D(const QmlRunScript);
    return d->name;
}

void QmlRunScript::setName(const QString &n)
{
    Q_D(QmlRunScript);
    d->name = n;
}

void QmlRunScript::execute()
{
    Q_D(QmlRunScript);
    if (!d->script.isEmpty()) {
        QmlExpression expr(qmlContext(this), d->script, this);
        expr.setTrackChange(false);
        expr.value();
    }
}

QmlRunScript::ActionList QmlRunScript::actions()
{
    ActionList rv;
    Action a;
    a.event = this;
    rv << a;
    return rv;
}

/*!
    \qmlclass SetProperty QmlSetProperty
    \brief The SetProperty element describes a new property value or binding for a state.

    The code below changes the position of the Rect depending upon
    the current state:

    \code
    Rect {
        id: myrect
        width: 50
        height: 50
        color: "red"
    }

    states: [
        State {
            name: "Position1"
            SetProperty {
                target: myrect
                property: "x"
                value: 150
            }
            SetProperty {
                target: myrect
                property: "y"
                value: 50
            }
        },
        State {
            name: "Position2"
            SetProperty {
                target: myrect
                property: "y"
                value: 200
            }
        }
    ]
    \endcode

    \sa SetProperties
*/

/*!
    \internal
    \class QmlSetProperty
    \brief The QmlSetProperty class describes a new property value or binding for a state.

    \ingroup group_states

    \sa QmlSetProperties
*/

class QmlSetPropertyPrivate : public QObjectPrivate
{
public:
    QmlSetPropertyPrivate() : obj(0) {}

    QObject *obj;
    QString prop;
    QVariant value;
    QString binding;
};

QML_DEFINE_TYPE(QmlSetProperty,SetProperty)

QmlSetProperty::QmlSetProperty(QObject *parent)
    : QmlStateOperation(*(new QmlSetPropertyPrivate), parent)
{
}

QmlSetProperty::~QmlSetProperty()
{
}

/*!
    \qmlproperty Object SetProperty::target
    This property holds the object the property to change belongs to
*/

/*!
    \property QmlSetProperty::target
    \brief the object the property to change belongs to
*/
QObject *QmlSetProperty::object()
{
    Q_D(QmlSetProperty);
    return d->obj;
}

void QmlSetProperty::setObject(QObject *o)
{
    Q_D(QmlSetProperty);
    d->obj = o;
}

/*!
    \qmlproperty string SetProperty::property
    This property holds the name of the property to change
*/

/*!
    \property QmlSetProperty::property
    \brief the name of the property to change
*/
QString QmlSetProperty::property() const
{
    Q_D(const QmlSetProperty);
    return d->prop;
}

void QmlSetProperty::setProperty(const QString &p)
{
    Q_D(QmlSetProperty);
    d->prop = p;
}

/*!
    \qmlproperty variant SetProperty::value
    This property holds the value to assign to the property

    You should set either a \c value or a \c binding, but not both.
*/

/*!
    \property QmlSetProperty::value
    \brief the value to assign to the property

    You should set either a value or a binding, not both.
*/
QVariant QmlSetProperty::value() const
{
    Q_D(const QmlSetProperty);
    return d->value;
}

void QmlSetProperty::setValue(const QVariant &v)
{
    Q_D(QmlSetProperty);
    d->value = v;
}

/*!
    \qmlproperty string SetProperty::binding
    This property holds the binding to assign to the property

    You should set either a \c value or a \c binding, but not both.
*/

/*!
    \property QmlSetProperty::binding
    \brief the binding to assign to the property

    You should set either a value or a binding, not both.
*/
QString QmlSetProperty::binding() const
{
    Q_D(const QmlSetProperty);
    return d->binding;
}

void QmlSetProperty::setBinding(const QString &binding)
{
    Q_D(QmlSetProperty);
    d->binding = binding;
}

QmlSetProperty::ActionList QmlSetProperty::actions()
{
    Q_D(QmlSetProperty);
    if (!d->obj)
        return ActionList();

    QObject *obj = d->obj;
    QString propName = d->prop;

    if (d->prop.contains(QLatin1Char('.'))) {  //handle dot properties
        QStringList str = d->prop.split(QLatin1Char('.'));
        for (int ii = 0; ii < str.count()-1; ++ii) {
            const QString &s = str.at(ii);
            QmlMetaProperty prop(obj, s);
            if (!prop.isValid()) {
                qmlInfo(this) << obj->metaObject()->className() 
                              << "has no property named" << s;
                return ActionList();
            }
            QVariant v = prop.read();
            obj = QmlMetaType::toQObject(v);
            if (!obj) {
                qmlInfo(this) << "Unable to coerce value property" 
                              << s << "into a QObject";
                return ActionList();
            }
        }
        propName = str.last();
    }

    QmlMetaProperty prop(obj, propName);
    if (!prop.isValid()) {
        qmlInfo(this) << obj->metaObject()->className() 
                      << "has no property named" << propName;
        return ActionList();
    }else if (!prop.isWritable()){
        qmlInfo(this) << obj->metaObject()->className() << propName
                      << "is not a writable property and cannot be set.";
        return ActionList();
    }
    QVariant cur = prop.read();

    Action a;
    a.property = prop;
    a.fromValue = cur;
    a.toValue = d->value;
    if (!d->binding.isEmpty()) 
        a.toBinding = new QmlBindableValue(d->binding, object(), qmlContext(this));

    return ActionList() << a;
}

QT_END_NAMESPACE
