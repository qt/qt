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

#ifndef QMLVISITOR_H
#define QMLVISITOR_H

#include <QString>
#include "declarativeparser/qdeclarativejsastvisitor_p.h"
#include "node.h"
#include "tree.h"

QT_BEGIN_NAMESPACE

class QmlDocVisitor : public QDeclarativeJS::AST::Visitor
{
public:
    QmlDocVisitor(const QString &filePath, const QString &code,
               QDeclarativeJS::Engine *engine, Tree *tree, QSet<QString> &commands);
    virtual ~QmlDocVisitor();

    bool visit(QDeclarativeJS::AST::UiImportList *imports);
    void endVisit(QDeclarativeJS::AST::UiImportList *definition);

    bool visit(QDeclarativeJS::AST::UiObjectDefinition *definition);
    void endVisit(QDeclarativeJS::AST::UiObjectDefinition *definition);

    bool visit(QDeclarativeJS::AST::UiPublicMember *member);
    void endVisit(QDeclarativeJS::AST::UiPublicMember *definition);

    bool visit(QDeclarativeJS::AST::IdentifierPropertyName *idproperty);

private:
    QDeclarativeJS::AST::SourceLocation precedingComment(quint32 offset) const;
    void applyDocumentation(QDeclarativeJS::AST::SourceLocation location, Node *node);

    QDeclarativeJS::Engine *engine;
    quint32 lastEndOffset;
    QString filePath;
    QString name;
    QString document;
    QList<QPair<QString, QString> > importList;
    QSet<QString> commands;
    QSet<quint32> usedComments;
    Tree *tree;
    InnerNode *current;
};

QT_END_NAMESPACE

#endif
