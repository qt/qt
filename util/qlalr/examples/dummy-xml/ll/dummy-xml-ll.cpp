/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QLALR module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <cstdlib>
#include <cstdio>

enum Token {
    EOF_SYMBOL,
    LEFT_ANGLE,
    RIGHT_ANGLE,
    ANY,
};

static int current_char;
static int yytoken;
static bool in_tag = false;

bool parseXmlStream();
bool parseTagOrWord();
bool parseTagName();

inline int nextToken()
{
    current_char = fgetc(stdin);
    if (current_char == EOF) {
        return (yytoken = EOF_SYMBOL);
    } else if (current_char == '<') {
        in_tag = true;
        return (yytoken = LEFT_ANGLE);
    } else if (in_tag && current_char == '>') {
        in_tag = false;
        return (yytoken = RIGHT_ANGLE);
    }
    return (yytoken = ANY);
}

bool parse()
{
    nextToken();
    return parseXmlStream();
}

bool parseXmlStream()
{
    while (parseTagOrWord())
        ;

    return true;
}

bool parseTagOrWord()
{
    if (yytoken == LEFT_ANGLE) {
        nextToken();
        if (! parseTagName())
            return false;
        if (yytoken != RIGHT_ANGLE)
            return false;
        nextToken();

        fprintf (stderr, "*** found a tag\n");

    } else if (yytoken == ANY) {
        nextToken();
    } else {
        return false;
    }
    return true;
}

bool parseTagName()
{
    while (yytoken == ANY)
        nextToken();

    return true;
}

int main()
{
    if (parse())
        printf("OK\n");
    else
        printf("KO\n");
}
