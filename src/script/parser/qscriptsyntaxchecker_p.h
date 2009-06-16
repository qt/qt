/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTSYNTAXCHECKER_H
#define QSCRIPTSYNTAXCHECKER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qstring.h>

#ifndef QT_NO_SCRIPT

#include "qscriptgrammar_p.h"

QT_BEGIN_NAMESPACE

namespace QScript {

class Lexer;

class SyntaxChecker: protected QScriptGrammar
{
public:
    enum State {
        Error,
        Intermediate,
        Valid,
    };

    struct Result {
        Result(State s, int ln, int col, const QString &msg)
            : state(s), errorLineNumber(ln), errorColumnNumber(col),
              errorMessage(msg) {}
        State state;
        int errorLineNumber;
        int errorColumnNumber;
        QString errorMessage;
    };

    SyntaxChecker();
    ~SyntaxChecker();

    Result checkSyntax(const QString &code);

protected:
    bool automatic(QScript::Lexer *lexer, int token) const;
    inline void reallocateStack();

protected:
    int tos;
    int stack_size;
    int *state_stack;
};

inline void SyntaxChecker::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    state_stack = reinterpret_cast<int*> (qRealloc(state_stack, stack_size * sizeof(int)));
}

} // namespace QScript

QT_END_NAMESPACE

#endif // QT_NO_SCRIPT

#endif
