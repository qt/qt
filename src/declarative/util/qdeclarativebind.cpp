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

#include "private/qdeclarativebind_p.h"

#include "private/qdeclarativenullablevalue_p_p.h"

#include <qdeclarativeengine.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeproperty.h>

#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeBindPrivate : public QObjectPrivate
{
public:
    QDeclarativeBindPrivate() : when(true), componentComplete(true), obj(0) {}

    bool when : 1;
    bool componentComplete : 1;
    QObject *obj;
    QString prop;
    QDeclarativeNullableValue<QVariant> value;
};


/*!
    \qmlclass Binding QDeclarativeBind
    \since 4.7
    \brief The Binding element allows arbitrary property bindings to be created.

    Sometimes it is necessary to bind to a property of an object that wasn't
    directly instantiated by QML - generally a property of a class exported
    to QML by C++. In these cases, regular property binding doesn't work. Binding
    allows you to bind any value to any property.

    For example, imagine a C++ application that maps an "app.enteredText"
    property into QML. You could use Binding to update the enteredText property
    like this.
    \code
    TextEdit { id: myTextField; text: "Please type here..." }
    Binding { target: app; property: "enteredText"; value: myTextField.text }
    \endcode
    Whenever the text in the TextEdit is updated, the C++ property will be
    updated also.

    If the binding target or binding property is changed, the bound value is
    immediately pushed onto the new target.

    \sa QtDeclarative
    */
/*!
    \internal
    \class QDeclarativeBind
    \brief The QDeclarativeBind class allows arbitrary property bindings to be created.

    Simple bindings are usually earier to do in-place rather than creating a
    QDeclarativeBind item. For that reason, QDeclarativeBind is usually used to transfer property information
    from Qml to C++.

    \sa cppqml
    */
QDeclarativeBind::QDeclarativeBind(QObject *parent)
    : QObject(*(new QDeclarativeBindPrivate), parent)
{
}

QDeclarativeBind::~QDeclarativeBind()
{
}

/*!
    \qmlproperty bool Binding::when

    This property holds when the binding is active.
    This should be set to an expression that evaluates to true when you want the binding to be active.

    \code
    Binding {
        target: contactName; property: 'text'
        value: name; when: list.ListView.isCurrentItem
    }
    \endcode
*/
bool QDeclarativeBind::when() const
{
    Q_D(const QDeclarativeBind);
    return d->when;
}

void QDeclarativeBind::setWhen(bool v)
{
    Q_D(QDeclarativeBind);
    d->when = v;
    eval();
}

/*!
    \qmlproperty Object Binding::target

    The object to be updated.
*/
QObject *QDeclarativeBind::object()
{
    Q_D(const QDeclarativeBind);
    return d->obj;
}

void QDeclarativeBind::setObject(QObject *obj)
{
    Q_D(QDeclarativeBind);
    d->obj = obj;
    eval();
}

/*!
    \qmlproperty string Binding::property

    The property to be updated.
*/
QString QDeclarativeBind::property() const
{
    Q_D(const QDeclarativeBind);
    return d->prop;
}

void QDeclarativeBind::setProperty(const QString &p)
{
    Q_D(QDeclarativeBind);
    d->prop = p;
    eval();
}

/*!
    \qmlproperty any Binding::value

    The value to be set on the target object and property.  This can be a
    constant (which isn't very useful), or a bound expression.
*/
QVariant QDeclarativeBind::value() const
{
    Q_D(const QDeclarativeBind);
    return d->value.value;
}

void QDeclarativeBind::setValue(const QVariant &v)
{
    Q_D(QDeclarativeBind);
    d->value.value = v;
    d->value.isNull = false;
    eval();
}

void QDeclarativeBind::classBegin()
{
    Q_D(QDeclarativeBind);
    d->componentComplete = false;
}

void QDeclarativeBind::componentComplete()
{
    Q_D(QDeclarativeBind);
    d->componentComplete = true;
    eval();
}

void QDeclarativeBind::eval()
{
    Q_D(QDeclarativeBind);
    if (!d->obj || d->value.isNull || !d->when || !d->componentComplete)
        return;

    QDeclarativeProperty prop(d->obj, d->prop);
    prop.write(d->value.value);
}

QT_END_NAMESPACE
