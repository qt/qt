/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact:  Qt Software Information (qt-info@nokia.com)
**
** Commercial Usage
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
**
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
**************************************************************************/

#ifndef JAVASCRIPTENGINE_P_H
#define JAVASCRIPTENGINE_P_H

#include <QString>
#include <QSet>

#include "javascriptastfwd_p.h"

QT_BEGIN_NAMESPACE

namespace JavaScript {
class NameId;
}

uint qHash(const JavaScript::NameId &id);

namespace JavaScript {

class Lexer;
class NodePool;

namespace Ecma {

class RegExp
{
public:
    enum RegExpFlag {
        Global     = 0x01,
        IgnoreCase = 0x02,
        Multiline  = 0x04
    };

public:
    static int flagFromChar(const QChar &);
    static QString flagsToString(int flags);
};

} // end of namespace Ecma


class NameId
{
    QString _text;

public:
    NameId(const QChar *u, int s)
        : _text(u, s)
    { }

    const QString asString() const
    { return _text; }

    bool operator == (const NameId &other) const
    { return _text == other._text; }

    bool operator != (const NameId &other) const
    { return _text != other._text; }

    bool operator < (const NameId &other) const
    { return _text < other._text; }
};

class DiagnosticMessage
{
public:
    enum Kind { Warning, Error };

    DiagnosticMessage()
        : kind(Error) {}

    DiagnosticMessage(Kind kind, const AST::SourceLocation &loc, const QString &message)
        : kind(kind), loc(loc), message(message) {}

    bool isWarning() const
    { return kind == Warning; }

    bool isError() const
    { return kind == Error; }

    Kind kind;
    AST::SourceLocation loc;
    QString message;
};

class Engine
{
    Lexer *_lexer;
    NodePool *_nodePool;
    QSet<NameId> _literals;

public:
    Engine()
        : _lexer(0), _nodePool(0)
    { }

    QSet<NameId> literals() const
    { return _literals; }

    NameId *intern(const QChar *u, int s)
    { return const_cast<NameId *>(&*_literals.insert(NameId(u, s))); }

    static QString toString(NameId *id)
    { return id->asString(); }

    Lexer *lexer() const
    { return _lexer; }

    void setLexer(Lexer *lexer)
    { _lexer = lexer; }

    NodePool *nodePool() const
    { return _nodePool; }

    void setNodePool(NodePool *nodePool)
    { _nodePool = nodePool; }
};

} // end of namespace JavaScript

QT_END_NAMESPACE

#endif // JAVASCRIPTENGINE_P_H
