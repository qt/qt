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

#ifndef QSCRIPTSYNTAXCHECKRESULT_P_H
#define QSCRIPTSYNTAXCHECKRESULT_P_H

#include "qscriptengine_p.h"
#include "qscriptsyntaxcheckresult.h"

#include <v8.h>

QT_BEGIN_NAMESPACE

class QScriptSyntaxCheckResultPrivate : public QScriptSharedData
{
public:
    static inline QScriptSyntaxCheckResult get(QScriptSyntaxCheckResultPrivate* p);
    inline QScriptSyntaxCheckResultPrivate(const QString& program);
    ~QScriptSyntaxCheckResultPrivate();

    inline QScriptSyntaxCheckResult::State state() const;
    inline int errorLineNumber() const;
    inline int errorColumnNumber() const;
    inline QString errorMessage() const;
private:
    int m_errorLineNumber;
    int m_errorColumnNumber;
    QString m_errorMessage;
};

QScriptSyntaxCheckResult QScriptSyntaxCheckResultPrivate::get(QScriptSyntaxCheckResultPrivate* p)
{
    return QScriptSyntaxCheckResult(p);
}

QScriptSyntaxCheckResultPrivate::QScriptSyntaxCheckResultPrivate(const QString& program)
    : m_errorLineNumber(-1)
    , m_errorColumnNumber(-1)
 {
    // FIXME do we really need to create a new context to parse a script?
    v8::Isolate *isolate = v8::Isolate::New();
    isolate->Enter();
    v8::Persistent<v8::Context> context = v8::Context::New();
    {
        v8::Context::Scope contextScope(context);
        v8::HandleScope handleScope;
        v8::TryCatch tryCatch;
        v8::Handle<v8::Script> script = v8::Script::Compile(QScriptConverter::toString(program), v8::String::New("QScriptEngine_checkSyntax"));
        if (script.IsEmpty()) {
            v8::Local<v8::Message> exception = tryCatch.Message();
            m_errorMessage = QScriptConverter::toString(exception->Get()->ToString());
            m_errorLineNumber = exception->GetLineNumber();
            m_errorColumnNumber = exception->GetStartColumn();
        }
    }
    context.Dispose();
    isolate->Exit();
    isolate->Dispose();
}

QScriptSyntaxCheckResult::State QScriptSyntaxCheckResultPrivate::state() const
{
    if (m_errorMessage.isEmpty())
        return QScriptSyntaxCheckResult::Valid;
    else if (m_errorMessage == QLatin1String("Uncaught SyntaxError: Unexpected end of input"))
        return QScriptSyntaxCheckResult::Intermediate;
    else
        return QScriptSyntaxCheckResult::Error;
}

int QScriptSyntaxCheckResultPrivate::errorColumnNumber() const
{
    return m_errorColumnNumber;
}

QString QScriptSyntaxCheckResultPrivate::errorMessage() const
{
    return m_errorMessage;
}

int QScriptSyntaxCheckResultPrivate::errorLineNumber() const
{
    return m_errorLineNumber;
}

QT_END_NAMESPACE

#endif
