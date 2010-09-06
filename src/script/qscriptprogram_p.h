/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSCRIPTPROGRAM_P_H
#define QSCRIPTPROGRAM_P_H

#include "qscriptconverter_p.h"
#include "qscriptprogram.h"
#include <QtCore/qstring.h>

#include <v8.h>

QT_BEGIN_NAMESPACE

/*
   FIXME The QScriptProgramPrivate potentially could be much faster.
*/

class QScriptEnginePrivate;

class QScriptProgramPrivate
{
public:
    inline static QScriptProgramPrivate* get(const QScriptProgram& program);
    inline QScriptProgramPrivate();
    inline QScriptProgramPrivate(const QString& sourceCode,
                   const QString fileName,
                   int firstLineNumber);

    inline ~QScriptProgramPrivate();

    inline bool isNull() const;

    inline QString sourceCode() const;
    inline QString fileName() const;
    inline int firstLineNumber() const;

    inline bool operator==(const QScriptProgramPrivate& other) const;
    inline bool operator!=(const QScriptProgramPrivate& other) const;

    inline int line() const;
    inline bool isCompiled() const;
    inline v8::Persistent<v8::Script> compiled(const QScriptEnginePrivate* engine);

    QBasicAtomicInt ref;
    QString m_program;
    QString m_fileName;
    int m_line;
    QScriptEnginePrivate* m_engine;
    v8::Persistent<v8::Script> m_compiled;
};

QScriptProgramPrivate* QScriptProgramPrivate::get(const QScriptProgram& program)
{
    return const_cast<QScriptProgramPrivate*>(program.d_ptr.constData());
}

QScriptProgramPrivate::QScriptProgramPrivate()
    : m_line(-1)
    , m_engine(0)
{}

QScriptProgramPrivate::QScriptProgramPrivate(const QString& sourceCode,
               const QString fileName,
               int firstLineNumber)
                   : m_program(sourceCode)
                   , m_fileName(fileName)
                   , m_line(firstLineNumber)
                   , m_engine(0)
{}

QScriptProgramPrivate::~QScriptProgramPrivate()
{
    if (!m_compiled.IsEmpty())
        m_compiled.Dispose();
}

bool QScriptProgramPrivate::isNull() const
{
    return m_program.isNull();
}

QString QScriptProgramPrivate::sourceCode() const
{
    return m_program;
}

QString QScriptProgramPrivate::fileName() const
{
    return m_fileName;
}

int QScriptProgramPrivate::firstLineNumber() const
{
    return m_line;
}

bool QScriptProgramPrivate::operator==(const QScriptProgramPrivate& other) const
{
    return m_line == other.m_line
            && m_fileName == other.m_fileName
            && m_program == other.m_program;
}

bool QScriptProgramPrivate::operator!=(const QScriptProgramPrivate& other) const
{
    return m_line != other.m_line
            || m_fileName != other.m_fileName
            || m_program != other.m_program;
}

int QScriptProgramPrivate::line() const
{
    return m_line;
}

bool QScriptProgramPrivate::isCompiled() const
{
    return m_engine;
}

/*!
  \internal
  Compiles script. The engine is used only for error checking (warn about engine mixing).
  \attention It assumes that there is created a right context, handleScope and tryCatch on the stack.
*/
inline v8::Persistent<v8::Script> QScriptProgramPrivate::compiled(const QScriptEnginePrivate* engine)
{
    Q_ASSERT(engine);
    if (isCompiled() && engine == m_engine)
        return m_compiled;

    // Recompile the script
    // FIXME maybe we can reuse the same script?
    // FIXME check if we can destroy a Persistent in different context!
    m_engine = const_cast<QScriptEnginePrivate*>(engine);
    m_compiled = v8::Persistent<v8::Script>::New(v8::Script::Compile(QScriptConverter::toString(sourceCode()), QScriptConverter::toString(fileName())));
    return m_compiled;
}

QT_END_NAMESPACE

#endif
