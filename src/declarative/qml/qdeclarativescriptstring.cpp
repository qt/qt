/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativescriptstring.h"

QT_BEGIN_NAMESPACE

class QDeclarativeScriptStringPrivate : public QSharedData
{
public:
    QDeclarativeScriptStringPrivate() : context(0), scope(0) {}

    QDeclarativeContext *context;
    QObject *scope;
    QString script;
};

/*!
\class QDeclarativeScriptString
\since 4.7
\brief The QDeclarativeScriptString class encapsulates a script and its context.

QDeclarativeScriptString is used to create QObject properties that accept a script "assignment" from QML.

Normally, the following QML would result in a binding being established for the \c script
property; i.e. \c script would be assigned the value obtained from running \c {myObj.value = Math.max(myValue, 100)}

\qml
MyType {
    script: myObj.value = Math.max(myValue, 100)
}
\endqml

If instead the property had a type of QDeclarativeScriptString,
the script itself -- \e {myObj.value = Math.max(myValue, 100)} -- would be passed to the \c script property
and the class could choose how to handle it. Typically, the class will evaluate
the script at some later time using a QDeclarativeExpression.

\code
QDeclarativeExpression expr(scriptString.context(), scriptString.script(), scriptStr.scopeObject());
expr.value();
\endcode

\sa QDeclarativeExpression
*/

/*!
Constructs an empty instance.
*/
QDeclarativeScriptString::QDeclarativeScriptString()
:  d(new QDeclarativeScriptStringPrivate)
{
}

/*!
Copies \a other.
*/
QDeclarativeScriptString::QDeclarativeScriptString(const QDeclarativeScriptString &other)
: d(other.d)
{
}

/*!
\internal
*/
QDeclarativeScriptString::~QDeclarativeScriptString()
{
}

/*!
Assigns \a other to this.
*/
QDeclarativeScriptString &QDeclarativeScriptString::operator=(const QDeclarativeScriptString &other)
{
    d = other.d;
    return *this;
}

/*!
Returns the context for the script.
*/
QDeclarativeContext *QDeclarativeScriptString::context() const
{
    return d->context;
}

/*!
Sets the \a context for the script.
*/
void QDeclarativeScriptString::setContext(QDeclarativeContext *context)
{
    d->context = context;
}

/*!
Returns the scope object for the script.
*/
QObject *QDeclarativeScriptString::scopeObject() const
{
    return d->scope;
}

/*!
Sets the scope \a object for the script.
*/
void QDeclarativeScriptString::setScopeObject(QObject *object)
{
    d->scope = object;
}

/*!
Returns the script text.
*/
QString QDeclarativeScriptString::script() const
{
    return d->script;
}

/*!
Sets the \a script text.
*/
void QDeclarativeScriptString::setScript(const QString &script)
{
    d->script = script;
}

QT_END_NAMESPACE

