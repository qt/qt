/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "config.h"
#include "qscriptstring.h"

#include "qscriptstring_p.h"

QT_BEGIN_NAMESPACE

/*! \internal */
struct QScriptStringPrivatePointerDeleter
{
    static inline void cleanup(QScriptStringPrivate *d)
    {
        if (!d || d->ref.deref())
            return;

        if (d->nameId) {
            d->engine->uninternString(d);
        } else {
            // the engine has already been deleted
            delete d;
        }
    }
};

/*!
  \since 4.4
  \class QScriptString

  \brief The QScriptString class acts as a handle to "interned" strings in a QScriptEngine.

  \ingroup script


  QScriptString can be used to achieve faster (repeated)
  property getting/setting, and comparison of property names, of
  script objects.

  To get a QScriptString representation of a string, pass the string
  to QScriptEngine::toStringHandle(). The typical usage pattern is to
  register one or more pre-defined strings when setting up your script
  environment, then subsequently use the relevant QScriptString as
  argument to e.g. QScriptValue::property().

  Call the toString() function to obtain the string that a
  QScriptString represents.
*/

/*!
  \internal
*/
QScriptStringPrivate::QScriptStringPrivate()
{
    ref = 0;
}

/*!
  \internal
*/
QScriptStringPrivate::~QScriptStringPrivate()
{
}

/*!
  \internal
*/
void QScriptStringPrivate::init(QScriptString &q, QScriptEngine *engine, const JSC::Identifier &value)
{
    Q_ASSERT(!q.isValid());
    q.d_ptr.data_ptr() = new QScriptStringPrivate();
    q.d_ptr->identifier = value;
    q.d_ptr->engine = engine;
    q.d_ptr->ref.ref();
}

/*!
  Constructs an invalid QScriptString.
*/
QScriptString::QScriptString()
    : d_ptr(0)
{
}

/*!
  Constructs a new QScriptString that is a copy of \a other.
*/
QScriptString::QScriptString(const QScriptString &other)
    : d_ptr(other.d_ptr.data())
{
    if (d_ptr)
        d_ptr->ref.ref();
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
QScriptString &QScriptString::operator=(const QScriptString &other)
{
    if (d_ptr == other.d_ptr)
        return *this;
    if (d_ptr && !d_ptr->ref.deref()) {
        delete d_ptr;
    }
    d_ptr.reset(other.d_ptr);
    if (d_ptr)
        d_ptr->ref.ref();
    return *this;
}

/*!
  Returns true if this QScriptString is valid; otherwise
  returns false.
*/
bool QScriptString::isValid() const
{
    Q_D(const QScriptString);
    return (d && d->engine);
}

/*!
  Returns true if this QScriptString is equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator==(const QScriptString &other) const
{
    Q_D(const QScriptString);
    if (d == other.d_func())
        return true;
    if (!d || !other.d_func())
        return false;
    if (d->engine != other.d_func()->engine)
        return false;
    if (!d->engine)
        return true;
    return d->identifier == other.d_func()->identifier;
}

/*!
  Returns true if this QScriptString is not equal to \a other;
  otherwise returns false.
*/
bool QScriptString::operator!=(const QScriptString &other) const
{
    return !operator==(other);
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa isValid()
*/
QString QScriptString::toString() const
{
    Q_D(const QScriptString);
    if (!d || !d->engine)
        return QString();
    return d->identifier.ustring();
}

/*!
  Returns the string that this QScriptString represents, or a
  null string if this QScriptString is not valid.

  \sa toString()
*/
QScriptString::operator QString() const
{
    return toString();
}

QT_END_NAMESPACE
