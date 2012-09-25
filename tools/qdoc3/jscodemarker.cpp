/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  jscodemarker.cpp
*/

#include "declarativeparser/qdeclarativejsast_p.h"
#include "declarativeparser/qdeclarativejsengine_p.h"
#include "declarativeparser/qdeclarativejslexer_p.h"
#include "declarativeparser/qdeclarativejsnodepool_p.h"
#include "declarativeparser/qdeclarativejsparser_p.h"

#include "atom.h"
#include "node.h"
#include "jscodemarker.h"
#include "qmlmarkupvisitor.h"
#include "text.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

JsCodeMarker::JsCodeMarker()
{
}

JsCodeMarker::~JsCodeMarker()
{
}

/*!
  Returns true if the \a code is recognized by the parser.
 */
bool JsCodeMarker::recognizeCode(const QString &code)
{
    QDeclarativeJS::Engine engine;
    QDeclarativeJS::Lexer lexer(&engine);
    QDeclarativeJS::Parser parser(&engine);
    QDeclarativeJS::NodePool m_nodePool("<JsCodeMarker::recognizeCode>", &engine);

    QString newCode = code;
    QList<QDeclarativeJS::AST::SourceLocation> pragmas = extractPragmas(newCode);
    lexer.setCode(newCode, 1);

    return parser.parseProgram();
}

/*!
  Returns true if \a ext is any of a list of file extensions
  for the QML language.
 */
bool JsCodeMarker::recognizeExtension(const QString &ext)
{
    return ext == "js";
}

/*!
  Returns true if the \a language is recognized. Only "QML" is
  recognized by this marker.
 */
bool JsCodeMarker::recognizeLanguage(const QString &language)
{
    return language == "JavaScript" || language == "ECMAScript";
}

/*!
  Returns the type of atom used to represent JavaScript code in the documentation.
*/
Atom::Type JsCodeMarker::atomType() const
{
    return Atom::JavaScript;
}

QString JsCodeMarker::markedUpCode(const QString &code,
                                    const Node *relative,
                                    const Location &location)
{
    return addMarkUp(code, relative, location);
}

QString JsCodeMarker::addMarkUp(const QString &code,
                                 const Node * /* relative */,
                                 const Location &location)
{
    QDeclarativeJS::Engine engine;
    QDeclarativeJS::Lexer lexer(&engine);

    QString newCode = code;
    QList<QDeclarativeJS::AST::SourceLocation> pragmas = extractPragmas(newCode);
    lexer.setCode(newCode, 1);

    QDeclarativeJS::Parser parser(&engine);
    QDeclarativeJS::NodePool m_nodePool("<JsCodeMarker::addMarkUp>", &engine);
    QString output;

    if (parser.parseProgram()) {
        QDeclarativeJS::AST::Node *ast = parser.rootNode();
        // Pass the unmodified code to the visitor so that pragmas and other
        // unhandled source text can be output.
        QmlMarkupVisitor visitor(code, pragmas, &engine);
        QDeclarativeJS::AST::Node::accept(ast, &visitor);
        output = visitor.markedUpCode();
    } else {
        location.warning(tr("Unable to parse JavaScript: \"%1\" at line %2, column %3").arg(
            parser.errorMessage()).arg(parser.errorLineNumber()).arg(
            parser.errorColumnNumber()));
        output = protect(code);
    }

    return output;
}

QT_END_NAMESPACE
