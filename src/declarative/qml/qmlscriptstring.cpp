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

#include "qmlscriptstring.h"

QT_BEGIN_NAMESPACE

class QmlScriptStringPrivate : public QSharedData
{
public:
    QmlScriptStringPrivate() : context(0), scope(0) {}

    QmlContext *context;
    QObject *scope;
    QString script;
};

/*!
\class QmlScriptString
\brief The QmlScriptString class encapsulates a script and its context.

The QmlScriptString is used by properties that want to accept a script "assignment" from QML.

Normally, the following code would result in a binding being established for the \c script
property.  If the property had a type of QmlScriptString, the script - \e {console.log(1921)} - itself
would be passed to the property and it could choose how to handle it.

\code
MyType {
    script: console.log(1921)
}
\endcode
*/

/*!
Construct an empty instance.
*/
QmlScriptString::QmlScriptString()
:  d(new QmlScriptStringPrivate)
{
}

/*!
Copy \a other.
*/
QmlScriptString::QmlScriptString(const QmlScriptString &other)
: d(other.d)
{
}

/*!
\internal
*/
QmlScriptString::~QmlScriptString()
{
}

/*!
Assign \a other to this.
*/
QmlScriptString &QmlScriptString::operator=(const QmlScriptString &other)
{
    d = other.d;
    return *this;
}

/*!
Return the context for the script.
*/
QmlContext *QmlScriptString::context() const
{
    return d->context;
}

/*!
Sets the \a context for the script.
*/
void QmlScriptString::setContext(QmlContext *context)
{
    d->context = context;
}

/*!
Returns the scope object for the script.
*/
QObject *QmlScriptString::scopeObject() const
{
    return d->scope;
}

/*!
Sets the scope \a object for the script.
*/
void QmlScriptString::setScopeObject(QObject *object)
{
    d->scope = object;
}

/*!
Returns the script text.
*/
QString QmlScriptString::script() const
{
    return d->script;
}

/*!
Sets the \a script text.
*/
void QmlScriptString::setScript(const QString &script)
{
    d->script = script;
}

QT_END_NAMESPACE

