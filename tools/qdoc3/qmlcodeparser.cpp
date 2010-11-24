/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
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

#include "parser/qmljsast_p.h"
#include "parser/qmljsastvisitor_p.h"
#include "parser/qmljsnodepool_p.h"

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

    lexer = new QmlJS::Lexer(&engine);
    parser = new QmlJS::Parser(&engine);
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
    lexer->setCode(document, 1);

    QSet<QString> topicCommandsAllowed = topicCommands();
    QSet<QString> otherMetacommandsAllowed = otherMetaCommands();
    QSet<QString> metacommandsAllowed = topicCommandsAllowed +
        otherMetacommandsAllowed;

    QmlJS::NodePool m_nodePool(filePath, &engine);

    if (parser->parse()) {
        QmlJS::AST::UiProgram *ast = parser->ast();
        DocVisitor visitor(filePath, document, &engine, tree, metacommandsAllowed);
        QmlJS::AST::Node::accept(ast, &visitor);
    }
}

void QmlCodeParser::doneParsingSourceFiles(Tree *tree)
{
}

/*!
  Returns the set of strings reopresenting the topic commands.
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

QT_END_NAMESPACE
