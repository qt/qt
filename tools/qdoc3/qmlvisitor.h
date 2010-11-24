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

#ifndef DOCVISITOR_H
#define DOCVISITOR_H

#include <QString>
#include "parser/qmljsastvisitor_p.h"
#include "node.h"
#include "tree.h"

class DocVisitor : public QmlJS::AST::Visitor
{
public:
    DocVisitor(const QString &filePath, const QString &code,
               QmlJS::Engine *engine, Tree *tree, QSet<QString> &commands);
    virtual ~DocVisitor();

    bool visit(QmlJS::AST::UiImportList *imports);

    bool visit(QmlJS::AST::UiObjectDefinition *definition);
    void endVisit(QmlJS::AST::UiObjectDefinition *definition);

    bool visit(QmlJS::AST::UiPublicMember *member);
    void endVisit(QmlJS::AST::UiPublicMember *definition);

    bool visit(QmlJS::AST::IdentifierPropertyName *idproperty);

private:
    QmlJS::AST::SourceLocation precedingComment(unsigned offset) const;
    void applyDocumentation(QmlJS::AST::SourceLocation location,
                            InnerNode *node);

    QmlJS::Engine *engine;
    quint32 lastEndOffset;
    QString filePath;
    QString name;
    QString document;
    QList<QPair<QString, QString> > importList;
    QSet<QString> commands;
    Tree *tree;
    InnerNode *current;
};

#endif
