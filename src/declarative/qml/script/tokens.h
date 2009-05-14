/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#ifndef TOKENS_H
#define TOKENS_H

enum Token {
    // Lexer tokens
    NOTOKEN, 
    INCOMPLETE,
    WHITESPACE,
    LANGLE,
    RANGLE,
    PLUS,
    MINUS,
    STAR,
    EQUALS, 
    AND,
    DOT,
    CHARACTER,
    DIGIT,
    TOKEN_TRUE,
    TOKEN_FALSE,
    OTHER
};

const char *tokenToString(Token);

#endif
