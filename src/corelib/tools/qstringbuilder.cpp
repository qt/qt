/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qstringbuilder.h"

/*!
    \class QLatin1Literal
    \reentrant
    \since 4.6

    \brief The QLatin1Literal class provides a thin wrapper around string
    literals used in source code.

    \ingroup tools
    \ingroup shared
    \ingroup text
    \mainclass

    Unlike \c QLatin1String, a \c QLatin1Literal can retrieve its size
    without iterating over the literal.

    The main use of \c QLatin1Literal is in conjunction with \c QStringBuilder
    to reduce the number of reallocations needed to build up a string from
    smaller chunks.

    \sa QStringBuilder, QLatin1String, QString, QStringRef
*/

/*! \fn int QLatin1Literal::size() const
 
    Returns the number of characters in the literal \e{excluding} the trailing
    NULL char.
*/

/*! \fn QLatin1Literal::QLatin1Literal(const char(&string)[N])
 
    Constructs a new literal from the given \a string.
*/

/*! \fn char *QLatin1Literal::data() const
 
    Returns a pointer to the first character of the string literal.
    The string literal is terminated by a NUL character.
*/

/*! \fn QLatin1Literal::operator QString() const
 
    Converts the \c QLatin1Literal into a \c QString object.
*/



/*!
    \class QStringBuilder
    \reentrant
    \since 4.6

    \brief The QStringBuilder class is a template class that provides a facility to build
    up QStrings from smaller chunks.

    \ingroup tools
    \ingroup shared
    \ingroup text
    \mainclass

    When creating strings from smaller chunks, typically \c QString::operator+()
    is used, resulting in \e{n - 1} reallocations when operating on \e{n} chunks.

    QStringBuilder uses expression templates to collect the individual parts,
    compute the total size, allocate memory for the resulting QString object,
    and copy the contents of the chunks into the result.

    The QStringBuilder class is not to be used explicitly in user code.
    Instances of the class are created as return values of the operator%()
    function, acting on objects of type \c QString, \c QLatin1String,
    \c QLatin1Literal, \c \QStringRef, \c QChar, \c QCharRef,
    \c QLatin1Char, and \c char.

    Concatenating strings with operator%() generally yields better
    performance then using \c QString::operator+() on the same chunks
    if there are three or more of them, and performs equally well in other
    cases.

    \sa QLatin1Literal, QString
*/

/* \fn template <class A, class B> QStringBuilder<A, B> operator%(const A &a, const B &b)

    Returns a \c QStringBuilder object that is converted to a QString object
    when assigned to a variable of QString type or passed to a function that
    takes a QString parameter.

    This function is usable with arguments of type \c QString,
    \c QLatin1String, \c QLatin1Literal, \c QStringRef, 
    \c QChar, \c QCharRef, \c QLatin1Char, and \c char.
*/

