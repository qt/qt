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

QT_BEGIN_NAMESPACE

namespace JavaScript {
class NameId;
}

uint qHash(const JavaScript::NameId &id);

namespace JavaScript {

class Node;
class Lexer;
class NodePool;

namespace AST {
class Node;
} // end of namespace AST

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

class Engine
{
    Lexer *_lexer;
    NodePool *_nodePool;
    AST::Node *_ast;
    QSet<NameId> _literals;

public:
    Engine()
        : _lexer(0), _nodePool(0), _ast(0)
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

    AST::Node *ast() const
    { return _ast; }

    AST::Node *changeAbstractSyntaxTree(AST::Node *node)
    {
        AST::Node *previousAST = _ast;
        _ast = node;
        return previousAST;
    }
};

} // end of namespace JavaScript

QT_END_NAMESPACE

#endif // JAVASCRIPTENGINE_P_H
