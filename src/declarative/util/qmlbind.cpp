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

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <private/qobject_p.h>
#include <QtCore/qfile.h>
#include <QtCore/qdebug.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptcontext.h>
#include <QtScript/qscriptengine.h>
#include <private/qmlnullablevalue_p.h>
#include "qmlbind.h"

QT_BEGIN_NAMESPACE

class QmlBindPrivate : public QObjectPrivate
{
public:
    QmlBindPrivate() : when(true), obj(0) {}

    bool when;
    QObject *obj;
    QString prop;
    QmlNullableValue<QVariant> value;
};

QML_DEFINE_TYPE(QmlBind,Bind)
/*!
    \qmlclass Bind QmlBind
    \brief The Bind element allows arbitrary property bindings to be created.

    Sometimes it is necessary to bind to a property of an object that wasn't
    directly instantiated by QML - generally a property of a class exported
    to QML by C++.  In these cases, regular property binding doesn't work.  Bind
    allows you to bind any value to any property.

    For example, imagine a C++ application that maps an "app.enteredText" 
    property into QML.  You could use Bind to update the enteredText property 
    like this.
    \code
    TextEdit { id: myTextField; text: "Please type here..." }
    Bind { target: app; property: "enteredText"; value: myTextField.text />
    \endcode
    Whenever the text in the TextEdit is updated, the C++ property will be
    updated also.

    If the bind target or bind property is changed, the bound value is 
    immediately pushed onto the new target.

    \sa {qmlforcpp}{Qt Declarative Markup Language For C++ Programmers}
    */
/*!
    \internal
    \class QmlBind
    \ingroup group_utility
    \brief The QmlBind class allows arbitrary property bindings to be created.

    Simple bindings are usually earier to do in-place rather than creating a
    QmlBind item. For that reason, QmlBind is usually used to transfer property information
    from Qml to C++.

    \sa cppqml
    */
QmlBind::QmlBind(QObject *parent)
    : QObject(*(new QmlBindPrivate), parent)
{
}

QmlBind::~QmlBind()
{
}

bool QmlBind::when() const
{
    Q_D(const QmlBind);
    return d->when;
}

void QmlBind::setWhen(bool v)
{
    Q_D(QmlBind);
    d->when = v;
    eval();
}

/*!
    \qmlproperty Object Bind::target

    The object to be updated.
 */
/*!
    \property QmlBind::target
    \brief the object to be updated.
*/
QObject *QmlBind::object()
{
    Q_D(const QmlBind);
    return d->obj;
}

void QmlBind::setObject(QObject *obj)
{
    Q_D(QmlBind);
    d->obj = obj;
    eval();
}

/*!
    \qmlproperty string Bind::property

    The property to be updated.
 */
/*!
    \property QmlBind::property
    \brief the property of the target to be updated.
*/
QString QmlBind::property() const
{
    Q_D(const QmlBind);
    return d->prop;
}

void QmlBind::setProperty(const QString &p)
{
    Q_D(QmlBind);
    d->prop = p;
    eval();
}

/*!
    \qmlproperty any Bind::value

    The value to be set on the target object and property.  This can be a 
    constant (which isn't very useful), or a bound expression.
 */
/*!
    \property QmlBind::value
    \brief the value to bind to.
*/
QVariant QmlBind::value() const
{
    Q_D(const QmlBind);
    return d->value.value;
}

void QmlBind::setValue(const QVariant &v)
{
    Q_D(QmlBind);
    d->value.value = v;
    d->value.isNull = false;
    eval();
}

void QmlBind::eval()
{
    Q_D(QmlBind);
    if (!d->obj || d->value.isNull || !d->when)
        return;

    QmlMetaProperty prop(d->obj, d->prop);
    prop.write(d->value.value);
}

QT_END_NAMESPACE
