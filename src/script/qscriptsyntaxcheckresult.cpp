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

#include "qscriptsyntaxcheckresult.h"
#include "qscriptsyntaxcheckresult_p.h"

QT_BEGIN_NAMESPACE

/*!
  \class QScriptSyntaxCheckResult

  \brief The QScriptSyntaxCheckResult class provides the result of a script syntax check.

  \ingroup script
  \mainclass

  QScriptSyntaxCheckResult is returned by QScriptEngine::checkSyntax() to
  provide information about the syntactical (in)correctness of a script.
*/

/*!
    \enum QScriptSyntaxCheckResult::State

    This enum specifies the state of a syntax check.

    \value Error The program contains a syntax error.
    \value Intermediate The program is incomplete.
    \value Valid The program is a syntactically correct Qt Script program.
*/

/*!
  Constructs a new QScriptSyntaxCheckResult from the \a other result.
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(const QScriptSyntaxCheckResult& other)
    : d_ptr(other.d_ptr)
{}

/*!
  Constructs a new QScriptSyntaxCheckResult from an internal representation.
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult(QScriptSyntaxCheckResultPrivate* d)
    : d_ptr(d)
{}

/*!
  \internal
*/
QScriptSyntaxCheckResult::QScriptSyntaxCheckResult()
    : d_ptr(0)
{
    Q_ASSERT(false);
}

/*!
  Destroys this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::~QScriptSyntaxCheckResult()
{}

/*!
  Assigns the \a other result to this QScriptSyntaxCheckResult, and returns a
  reference to this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult& QScriptSyntaxCheckResult::operator=(const QScriptSyntaxCheckResult& other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
  Returns the state of this QScriptSyntaxCheckResult.
*/
QScriptSyntaxCheckResult::State QScriptSyntaxCheckResult::state() const
{
    return d_ptr->state();
}

/*!
  Returns the error line number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorMessage()
*/
int QScriptSyntaxCheckResult::errorLineNumber() const
{
    return d_ptr->errorLineNumber();
}

/*!
  Returns the error column number of this QScriptSyntaxCheckResult, or -1 if
  there is no error.

  \sa state(), errorLineNumber()
*/
int QScriptSyntaxCheckResult::errorColumnNumber() const
{
    return d_ptr->errorColumnNumber();
}

/*!
  Returns the error message of this QScriptSyntaxCheckResult, or an empty
  string if there is no error.

  \sa state(), errorLineNumber()
*/
QString QScriptSyntaxCheckResult::errorMessage() const
{
    return d_ptr->errorMessage();
}

QScriptSyntaxCheckResultPrivate::~QScriptSyntaxCheckResultPrivate()
{
}

QT_END_NAMESPACE
