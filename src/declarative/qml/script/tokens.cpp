/****************************************************************************
**
** This file is part of the $PACKAGE_NAME$.
**
** Copyright (C) $THISYEAR$ $COMPANY_NAME$.
**
** $QT_EXTENDED_DUAL_LICENSE$
**
****************************************************************************/

#include "tokens.h"


/*!
  Returns a string representation of token \a tok.
*/
const char *tokenToString(Token tok)
{
    switch(tok) {
#define CASE(X) case X: return #X;
        CASE(NOTOKEN)
        CASE(INCOMPLETE)
        CASE(WHITESPACE)
        CASE(LANGLE)
        CASE(RANGLE)
        CASE(PLUS)
        CASE(MINUS)
        CASE(STAR)
        CASE(EQUALS)
        CASE(DOT)
        CASE(CHARACTER)
        CASE(DIGIT)
        CASE(OTHER)
        CASE(AND)
    case TOKEN_TRUE:
        return "TRUE";
    case TOKEN_FALSE:
        return "FALSE";
#undef CASE
    }
    return 0;
}

