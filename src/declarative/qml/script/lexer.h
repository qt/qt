/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef LEXER_H
#define LEXER_H

#include <QList>
#include "tokens.h"


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
struct LexerToken
{
    LexerToken() : token(NOTOKEN), start(-1), end(-1), line(-1), offset(-1) {}
    LexerToken(const LexerToken &other) : token(other.token),
                                          start(other.start),
                                          end(other.end),
                                          line(other.line),
                                          offset(other.offset) {}
    LexerToken &operator=(const LexerToken &other) {
        token = other.token;
        start = other.start;
        end = other.end;
        line = other.line;
        offset = other.offset;
        return *this;
    }

    Token token;
    int start;
    int end;
    int line;
    int offset;
};

QList<LexerToken> tokenize(const char *text);
void dumpTokens(const char *text, const QList<LexerToken> &tokens);


QT_END_NAMESPACE

QT_END_HEADER
#endif
