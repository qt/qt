/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#include "qscriptprogram.h"
#include "qscriptprogram_p.h"
#include "qscriptengine.h"
#include "qscriptengine_p.h"

#include "Executable.h"

QT_BEGIN_NAMESPACE

/*!
  \internal

  \since 4.6
  \class QScriptProgram

  \ingroup script

*/

QScriptProgramPrivate::QScriptProgramPrivate(QScriptEnginePrivate *e,
                                             JSC::EvalExecutable *x,
                                             intptr_t id)
    : engine(e), executable(x), sourceId(id)
{
    ref = 0;
}

QScriptProgramPrivate::~QScriptProgramPrivate()
{
    delete executable;
}

QScriptProgramPrivate *QScriptProgramPrivate::get(const QScriptProgram &q)
{
    return const_cast<QScriptProgramPrivate*>(q.d_func());
}

QScriptProgram QScriptProgramPrivate::create(QScriptEnginePrivate *engine,
                                             JSC::EvalExecutable *executable,
                                             intptr_t sourceId)
{
    QScriptProgramPrivate *d = new QScriptProgramPrivate(engine, executable, sourceId);
    QScriptProgram result;
    result.d_ptr = d;
    return result;
}

/*!
  Constructs an invalid QScriptProgram.
*/
QScriptProgram::QScriptProgram()
    : d_ptr(0)
{
}

/*!
  Constructs a new QScriptProgram that is a copy of \a other.
*/
QScriptProgram::QScriptProgram(const QScriptProgram &other)
    : d_ptr(other.d_ptr)
{
}

/*!
  Destroys this QScriptProgram.
*/
QScriptProgram::~QScriptProgram()
{
    Q_D(QScriptProgram);
    //    if (d->engine && (d->ref == 1))
    //      d->engine->unregisterScriptProgram(d);
}

/*!
  Assigns the \a other value to this QScriptProgram.
*/
QScriptProgram &QScriptProgram::operator=(const QScriptProgram &other)
{
  //    if (d_func() && d_func()->engine && (d_func()->ref == 1))
      //        d_func()->engine->unregisterScriptProgram(d_func());
  //    }
    d_ptr = other.d_ptr;
    return *this;
}

/*!
  Returns true if this QScriptProgram is valid; otherwise
  returns false.
*/
bool QScriptProgram::isValid() const
{
    Q_D(const QScriptProgram);
    return (d && d->engine);
}

/*!
  Returns the source code of this program.
*/
QString QScriptProgram::sourceCode() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return QString();
    return d->executable->source().toString();
}

/*!
  Returns the filename associated with this program.
*/
QString QScriptProgram::fileName() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return QString();
    return d->executable->sourceURL();
}

/*!
  Returns the line number associated with this program.
*/
int QScriptProgram::lineNumber() const
{
    Q_D(const QScriptProgram);
    if (!d)
        return -1;
    return d->executable->lineNo();
}

/*!
  Returns true if this QScriptProgram is equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator==(const QScriptProgram &other) const
{
    return (sourceCode() == other.sourceCode())
        && (fileName() == other.fileName())
        && (lineNumber() == other.lineNumber());
}

/*!
  Returns true if this QScriptProgram is not equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator!=(const QScriptProgram &other) const
{
    return !operator==(other);
}

QT_END_NAMESPACE
