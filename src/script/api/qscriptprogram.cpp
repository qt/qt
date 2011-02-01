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

#include "qscriptprogram.h"
#include "qscriptprogram_p.h"
#include "qscriptisolate_p.h"
#include "qscriptengine_p.h"
#include "qscriptable_p.h"
#include "qscript_impl_p.h"

QT_BEGIN_NAMESPACE

/*!
  \internal

  \class QScriptProgram

  \brief The QScriptProgram class encapsulates a Qt Script program.

  \ingroup script

  QScriptProgram retains the compiled representation of the script if
  possible. Thus, QScriptProgram can be used to evaluate the same
  script multiple times more efficiently.

  \code
  QScriptEngine engine;
  QScriptProgram program("1 + 2");
  QScriptValue result = engine.evaluate(program);
  \endcode
*/

/*!
  Constructs a null QScriptProgram.
*/
QScriptProgram::QScriptProgram()
    : d_ptr(new QScriptProgramPrivate)
{}

/*!
  Constructs a new QScriptProgram with the given \a sourceCode, \a
  fileName and \a firstLineNumber.
*/
QScriptProgram::QScriptProgram(const QString& sourceCode,
               const QString fileName,
               int firstLineNumber)
    : d_ptr(new QScriptProgramPrivate(sourceCode, fileName, firstLineNumber))
{}

/*!
  Destroys this QScriptProgram.
*/
QScriptProgram::~QScriptProgram()
{}

/*!
  Constructs a new QScriptProgram that is a copy of \a other.
*/
QScriptProgram::QScriptProgram(const QScriptProgram& other)
{
    d_ptr = other.d_ptr;
}

/*!
  Assigns the \a other value to this QScriptProgram.
*/
QScriptProgram& QScriptProgram::operator=(const QScriptProgram& other)
{
    d_ptr = other.d_ptr;
    return *this;
}

/*!
  Returns true if this QScriptProgram is null; otherwise
  returns false.
*/
bool QScriptProgram::isNull() const
{
    return d_ptr->isNull();
}

/*!
  Returns the source code of this program.
*/
QString QScriptProgram::sourceCode() const
{
    return d_ptr->sourceCode();
}

/*!
  Returns the filename associated with this program.
*/
QString QScriptProgram::fileName() const
{
    return d_ptr->fileName();
}

/*!
  Returns the line number associated with this program.
*/
int QScriptProgram::firstLineNumber() const
{
    return d_ptr->firstLineNumber();
}

/*!
  Returns true if this QScriptProgram is equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator==(const QScriptProgram& other) const
{
    return d_ptr == other.d_ptr || *d_ptr == *other.d_ptr;
}

/*!
  Returns true if this QScriptProgram is not equal to \a other;
  otherwise returns false.
*/
bool QScriptProgram::operator!=(const QScriptProgram& other) const
{
    return d_ptr != other.d_ptr && *d_ptr != *other.d_ptr;
}

/*!
 *  \internal
 *  Compiles script. The engine is used only for error checking (warn about engine mixing).
 *  \attention It assumes that there is created a right context, handleScope and tryCatch on the stack.
 */
v8::Persistent<v8::Script> QScriptProgramPrivate::compiled(const QScriptEnginePrivate* engine)
{
    Q_ASSERT(engine);
    if (isCompiled() && m_engine == engine)
        return m_compiled;

    if (m_engine) {
        //Different engine, we need to dicard the old handle with the other isolate
        QScriptIsolate api(m_engine, QScriptIsolate::NotNullEngine);
        Q_ASSERT(!m_compiled.IsEmpty());
        m_compiled.Dispose();
        m_compiled.Clear();
    }
    // Recompile the script
    // FIXME maybe we can reuse the same script?
    m_engine = const_cast<QScriptEnginePrivate*>(engine);
    m_compiled = v8::Persistent<v8::Script>::New(v8::Script::Compile(QScriptConverter::toString(sourceCode()), QScriptConverter::toString(fileName())));
    return m_compiled;
}


QT_END_NAMESPACE
