/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
  qmlcodeparser.cpp
*/

#include "declarativeparser/qdeclarativejsast_p.h"
#include "declarativeparser/qdeclarativejsastvisitor_p.h"
#include "declarativeparser/qdeclarativejsnodepool_p.h"

#include "qmlcodeparser.h"
#include "node.h"
#include "tree.h"
#include "config.h"
#include "qmlvisitor.h"

QT_BEGIN_NAMESPACE

#define COMMAND_STARTPAGE               Doc::alias("startpage")
#define COMMAND_VARIABLE                Doc::alias("variable")

#define COMMAND_QMLCLASS                Doc::alias("qmlclass")
#define COMMAND_QMLPROPERTY             Doc::alias("qmlproperty")
#define COMMAND_QMLATTACHEDPROPERTY     Doc::alias("qmlattachedproperty")
#define COMMAND_QMLINHERITS             Doc::alias("inherits")
#define COMMAND_QMLSIGNAL               Doc::alias("qmlsignal")
#define COMMAND_QMLATTACHEDSIGNAL       Doc::alias("qmlattachedsignal")
#define COMMAND_QMLMETHOD               Doc::alias("qmlmethod")
#define COMMAND_QMLATTACHEDMETHOD       Doc::alias("qmlattachedmethod")
#define COMMAND_QMLDEFAULT              Doc::alias("default")
#define COMMAND_QMLBASICTYPE            Doc::alias("qmlbasictype")

QmlCodeParser::QmlCodeParser()
{
}

QmlCodeParser::~QmlCodeParser()
{
}

/*!
  Initialize the code parser base class.
 */
void QmlCodeParser::initializeParser(const Config &config)
{
    CodeParser::initializeParser(config);

    lexer = new QDeclarativeJS::Lexer(&engine);
    parser = new QDeclarativeJS::Parser(&engine);
}

void QmlCodeParser::terminateParser()
{
    delete lexer;
    delete parser;
}

QString QmlCodeParser::language()
{
    return "QML";
}

QStringList QmlCodeParser::sourceFileNameFilter()
{
    return QStringList("*.qml");
}

void QmlCodeParser::parseSourceFile(const Location& location,
                                    const QString& filePath,
                                    Tree *tree)
{
    QFile in(filePath);
    if (!in.open(QIODevice::ReadOnly)) {
        location.error(tr("Cannot open QML file '%1'").arg(filePath));
        return;
    }

    QString document = in.readAll();
    in.close();

    Location fileLocation(filePath);

    QString newCode = document;
    extractPragmas(newCode);
    lexer->setCode(newCode, 1);

    QSet<QString> topicCommandsAllowed = topicCommands();
    QSet<QString> otherMetacommandsAllowed = otherMetaCommands();
    QSet<QString> metacommandsAllowed = topicCommandsAllowed +
        otherMetacommandsAllowed;

    QDeclarativeJS::NodePool m_nodePool(filePath, &engine);

    if (parser->parse()) {
        QDeclarativeJS::AST::UiProgram *ast = parser->ast();
        QmlDocVisitor visitor(filePath, newCode, &engine, tree, metacommandsAllowed);
        QDeclarativeJS::AST::Node::accept(ast, &visitor);
    }
}

void QmlCodeParser::doneParsingSourceFiles(Tree *tree)
{
}

/*!
  Returns the set of strings representing the topic commands.
 */
QSet<QString> QmlCodeParser::topicCommands()
{
    return QSet<QString>() << COMMAND_VARIABLE
                           << COMMAND_QMLCLASS
                           << COMMAND_QMLPROPERTY
                           << COMMAND_QMLATTACHEDPROPERTY
                           << COMMAND_QMLSIGNAL
                           << COMMAND_QMLATTACHEDSIGNAL
                           << COMMAND_QMLMETHOD
                           << COMMAND_QMLATTACHEDMETHOD
                           << COMMAND_QMLBASICTYPE;
}

/*!
  Returns the set of strings representing the common metacommands
  plus some other metacommands.
 */
QSet<QString> QmlCodeParser::otherMetaCommands()
{
    return commonMetaCommands() << COMMAND_STARTPAGE
                                << COMMAND_QMLINHERITS
                                << COMMAND_QMLDEFAULT;
}

/*
Copied and pasted from src/declarative/qml/qdeclarativescriptparser.cpp.
*/
static void replaceWithSpace(QString &str, int idx, int n) 
{
    QChar *data = str.data() + idx;
    const QChar space(QLatin1Char(' '));
    for (int ii = 0; ii < n; ++ii)
        *data++ = space;
}

/*
Copied and pasted from src/declarative/qml/qdeclarativescriptparser.cpp then
modified to return no values.

Searches for ".pragma <value>" declarations within \a script.  Currently supported pragmas
are:
    library
*/
void QmlCodeParser::extractPragmas(QString &script)
{
    const QString pragma(QLatin1String("pragma"));
    const QString library(QLatin1String("library"));

    QDeclarativeJS::Lexer l(0);
    l.setCode(script, 0);

    int token = l.lex();

    while (true) {
        if (token != QDeclarativeJSGrammar::T_DOT)
            return;

        int startOffset = l.tokenOffset();
        int startLine = l.currentLineNo();

        token = l.lex();

        if (token != QDeclarativeJSGrammar::T_IDENTIFIER ||
            l.currentLineNo() != startLine ||
            script.mid(l.tokenOffset(), l.tokenLength()) != pragma)
            return;

        token = l.lex();

        if (token != QDeclarativeJSGrammar::T_IDENTIFIER ||
            l.currentLineNo() != startLine)
            return;

        QString pragmaValue = script.mid(l.tokenOffset(), l.tokenLength());
        int endOffset = l.tokenLength() + l.tokenOffset();

        token = l.lex();
        if (l.currentLineNo() == startLine)
            return;

        if (pragmaValue == QLatin1String("library"))
            replaceWithSpace(script, startOffset, endOffset - startOffset);
        else
            return;
    }
    return;
}

QT_END_NAMESPACE
