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

#include <QFileInfo>
#include <QStringList>
#include <QtGlobal>
#include "parser/qmljsast_p.h"
#include "parser/qmljsastfwd_p.h"
#include "parser/qmljsengine_p.h"

#include "node.h"
#include "qmlvisitor.h"

DocVisitor::DocVisitor(const QString &filePath, const QString &code,
                       QmlJS::Engine *engine, Tree *tree, QSet<QString> &commands)
{
    this->filePath = filePath;
    this->name = QFileInfo(filePath).baseName();
    document = code;
    this->engine = engine;
    this->tree = tree;
    this->commands = commands;
    current = tree->root();
}

DocVisitor::~DocVisitor()
{
}

QmlJS::AST::SourceLocation DocVisitor::precedingComment(unsigned offset) const
{
    QmlJS::AST::SourceLocation currentLoc;

    foreach (const QmlJS::AST::SourceLocation &loc, engine->comments()) {
        if (loc.begin() > lastEndOffset && loc.end() < offset)
            currentLoc = loc; 
        else
            break;
    }
    if (currentLoc.isValid()) {
        QString comment = document.mid(currentLoc.offset, currentLoc.length);
        if (comment.startsWith("!") || comment.startsWith("*"))
            return currentLoc;
    }

    return QmlJS::AST::SourceLocation();
}

void DocVisitor::applyDocumentation(QmlJS::AST::SourceLocation location,
                                    InnerNode *node)
{
    QmlJS::AST::SourceLocation loc = precedingComment(location.begin());

    if (loc.isValid()) {
        QString source = document.mid(loc.offset, loc.length);
        if (source.startsWith(QLatin1String("!")) ||
            source.startsWith(QLatin1String("*"))) {

            Location start(filePath);
            start.setLineNo(loc.startLine);
            start.setColumnNo(loc.startColumn);
            Location finish(filePath);
            finish.setLineNo(loc.startLine);
            finish.setColumnNo(loc.startColumn);

            Doc doc(start, finish, source.mid(1), commands);
            node->setDoc(doc);
        }
    }
}

/*!
    Visits element definitions, recording them in a tree structure.
*/
bool DocVisitor::visit(QmlJS::AST::UiObjectDefinition *definition)
{
    QString type = definition->qualifiedTypeNameId->name->asString();

    if (current->type() == Node::Namespace) {
        QmlClassNode *component = new QmlClassNode(current, name, 0);
        applyDocumentation(definition->firstSourceLocation(), component);
        current = component;
    }

    return true;
}

void DocVisitor::endVisit(QmlJS::AST::UiObjectDefinition *definition)
{
    lastEndOffset = definition->lastSourceLocation().end();
}

bool DocVisitor::visit(QmlJS::AST::UiImportList *imports)
{
    // Note that the imports list can be traversed by iteration to obtain
    // all the imports in the document at once, having found just one:
    // *it = imports; it; it = it->next

    QString module = document.mid(imports->import->fileNameToken.offset,
                                  imports->import->fileNameToken.length);
    QString version = document.mid(imports->import->versionToken.offset,
                                   imports->import->versionToken.length);
    importList.append(QPair<QString, QString>(module, version));

    return true;
}

/*!
    Visits public member declarations, such as signals and properties.
    These only include custom signals and properties.
*/
bool DocVisitor::visit(QmlJS::AST::UiPublicMember *member)
{
    switch (member->type) {
    case QmlJS::AST::UiPublicMember::Signal:
    {
        QString name = member->name->asString();

        QList<QPair<QString, QString> > parameters;
        for (QmlJS::AST::UiParameterList *it = member->parameters; it; it = it->next) {
            if (it->type && it->name)
                parameters.append(QPair<QString, QString>(it->type->asString(),
                                                          it->name->asString()));
        }

        //current->addSignal(new Signal(name, parameters));
        break;
    }
    case QmlJS::AST::UiPublicMember::Property:
    {
        QString type = member->memberType->asString();
        QString name = member->name->asString();

        //current->addProperty(new Property(type, name));
        break;
    }
    default:
        return false;
    }

    //current->doc = precedingComment(member->firstSourceLocation().begin());
    return true;
}

void DocVisitor::endVisit(QmlJS::AST::UiPublicMember *definition)
{
    lastEndOffset = definition->lastSourceLocation().end();
}

bool DocVisitor::visit(QmlJS::AST::IdentifierPropertyName *idproperty)
{
    return true;
}
