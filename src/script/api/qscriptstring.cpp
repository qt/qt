/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qscriptstring.h"

#include "qscriptstring_p.h"
#include "qscriptisolate_p.h"
#include "qscriptengine_p.h"
#include "qscript_impl_p.h"
#include "qscriptshareddata_p.h"

QT_BEGIN_NAMESPACE

/*!
  Constructs an invalid QScriptString.
*/
QScriptString::QScriptString()
    : d_ptr(new QScriptStringPrivate())
{
}
/*!
  Constructs an QScriptString from internal representation
  \internal
*/
QScriptString::QScriptString(QScriptStringPrivate* d)
    : d_ptr(d)
{
}

/*!
  Constructs an QScriptString from internal representation
  \internal
*/
QScriptString::QScriptString(QScriptPassPointer<QScriptStringPrivate> d)
    : d_ptr(d.give())
{
}

/*!
  Constructs a new QScriptString that is a copy of \a other.
*/
QScriptString::QScriptString(const QScriptString& other)
{
    d_ptr = other.d_ptr;
}

/*!
  Destroys this QScriptString.
*/
QScriptString::~QScriptString()
{
}

/*!
  Assigns the \a other value to this QScriptString.
*/
QScriptString& QScriptString::operator=(const QScriptString& other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
  Returns true if this QScriptString is valid; otherwise
  returns false.
*/
bool QScriptString::isValid() const
{
    Q_D(const QScriptString);
    return d->isValid();
}

/*!
  Returns true if this QScriptString is equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator==(const QScriptString& other) const
{
    Q_D(const QScriptString);
    QScriptIsolate api(d->engine());
    return d_ptr == other.d_ptr || *d_ptr == *(other.d_ptr);
}

/*!
  Returns true if this QScriptString is not equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator!=(const QScriptString& other) const
{
    Q_D(const QScriptString);
    QScriptIsolate api(d->engine());
    return d_ptr != other.d_ptr || *d_ptr != *(other.d_ptr);
}

/*!
  Attempts to convert this QScriptString to a QtScript array index,
  and returns the result.

  If a conversion error occurs, *\a{ok} is set to false; otherwise
  *\a{ok} is set to true.
*/
quint32 QScriptString::toArrayIndex(bool* ok) const
{
    Q_D(const QScriptString);
    QScriptIsolate api(d->engine());
    return d->toArrayIndex(ok);
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa isValid()
*/
QString QScriptString::toString() const
{
    Q_D(const QScriptString);
    QScriptIsolate api(d->engine());
    return d->toString();
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa toString()
*/
QScriptString::operator QString() const
{
    Q_D(const QScriptString);
    QScriptIsolate api(d->engine());
    return d->toString();
}

uint qHash(const QScriptString& key)
{
    return QScriptStringPrivate::get(key)->id();
}

QT_END_NAMESPACE
