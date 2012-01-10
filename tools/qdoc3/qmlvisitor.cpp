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

#include <QFileInfo>
#include <QStringList>
#include <QtGlobal>
#include "declarativeparser/qdeclarativejsast_p.h"
#include "declarativeparser/qdeclarativejsastfwd_p.h"
#include "declarativeparser/qdeclarativejsengine_p.h"

#include "node.h"
#include "qmlvisitor.h"

QT_BEGIN_NAMESPACE

QmlDocVisitor::QmlDocVisitor(const QString &filePath, const QString &code,
                       QDeclarativeJS::Engine *engine, Tree *tree, QSet<QString> &commands)
{
    this->filePath = filePath;
    this->name = QFileInfo(filePath).baseName();
    document = code;
    this->engine = engine;
    this->tree = tree;
    this->commands = commands;
    current = tree->root();
}

QmlDocVisitor::~QmlDocVisitor()
{
}

QDeclarativeJS::AST::SourceLocation QmlDocVisitor::precedingComment(quint32 offset) const
{
    QListIterator<QDeclarativeJS::AST::SourceLocation> it(engine->comments());
    it.toBack();

    while (it.hasPrevious()) {

        QDeclarativeJS::AST::SourceLocation loc = it.previous();

        if (loc.begin() <= lastEndOffset)
            // Return if we reach the end of the preceding structure.
            break;

        else if (usedComments.contains(loc.begin()))
            // Return if we encounter a previously used comment.
            break;

        else if (loc.begin() > lastEndOffset && loc.end() < offset) {

            // Only examine multiline comments in order to avoid snippet markers.
            if (document.mid(loc.offset - 1, 1) == "*") {
                QString comment = document.mid(loc.offset, loc.length);
                if (comment.startsWith("!") || comment.startsWith("*"))
                    return loc;
            }
        }
    }

    return QDeclarativeJS::AST::SourceLocation();
}

void QmlDocVisitor::applyDocumentation(QDeclarativeJS::AST::SourceLocation location,
                                    Node *node)
{
    QDeclarativeJS::AST::SourceLocation loc = precedingComment(location.begin());

    if (loc.isValid()) {
        QString source = document.mid(loc.offset, loc.length);

        Location start(filePath);
        start.setLineNo(loc.startLine);
        start.setColumnNo(loc.startColumn);
        Location finish(filePath);
        finish.setLineNo(loc.startLine);
        finish.setColumnNo(loc.startColumn);

        Doc doc(start, finish, source.mid(1), commands);
        node->setDoc(doc);

        usedComments.insert(loc.offset);
    }
}

/*!
    Visits element definitions, recording them in a tree structure.
*/
bool QmlDocVisitor::visit(QDeclarativeJS::AST::UiObjectDefinition *definition)
{
    QString type = definition->qualifiedTypeNameId->name->asString();

    if (current->type() == Node::Namespace) {
        QmlClassNode *component = new QmlClassNode(current, name, 0);
        component->setTitle(QLatin1String("QML ") + name + QLatin1String(" Component"));

        QmlClassNode::addInheritedBy(type, component);
        component->setLink(Node::InheritsLink, type, type);

        applyDocumentation(definition->firstSourceLocation(), component);

        current = component;
    }

    return true;
}

void QmlDocVisitor::endVisit(QDeclarativeJS::AST::UiObjectDefinition *definition)
{
    lastEndOffset = definition->lastSourceLocation().end();
}

bool QmlDocVisitor::visit(QDeclarativeJS::AST::UiImportList *imports)
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

void QmlDocVisitor::endVisit(QDeclarativeJS::AST::UiImportList *definition)
{
    lastEndOffset = definition->lastSourceLocation().end();
}

/*!
    Visits public member declarations, such as signals and properties.
    These only include custom signals and properties.
*/
bool QmlDocVisitor::visit(QDeclarativeJS::AST::UiPublicMember *member)
{
    switch (member->type) {
    case QDeclarativeJS::AST::UiPublicMember::Signal:
    {
        if (current->type() == Node::Fake) {
            QmlClassNode *qmlClass = static_cast<QmlClassNode *>(current);
            if (qmlClass) {

                QString name = member->name->asString();
                FunctionNode *qmlSignal = new FunctionNode(Node::QmlSignal, current, name, false);

                QList<Parameter> parameters;
                for (QDeclarativeJS::AST::UiParameterList *it = member->parameters; it; it = it->next) {
                    if (it->type && it->name)
                        parameters.append(Parameter(it->type->asString(), "", it->name->asString()));
                }

                qmlSignal->setParameters(parameters);
                applyDocumentation(member->firstSourceLocation(), qmlSignal);
            }
        }
        break;
    }
    case QDeclarativeJS::AST::UiPublicMember::Property:
    {
        QString type = member->memberType->asString();
        QString name = member->name->asString();

        if (current->type() == Node::Fake) {
            QmlClassNode *qmlClass = static_cast<QmlClassNode *>(current);
            if (qmlClass) {

                QString name = member->name->asString();
                QmlPropGroupNode *qmlPropGroup = new QmlPropGroupNode(qmlClass, name, false);
                if (member->isDefaultMember)
                    qmlPropGroup->setDefault();
                QmlPropertyNode *qmlPropNode = new QmlPropertyNode(qmlPropGroup, name, type, false);
                qmlPropNode->setWritable(!member->isReadonlyMember);
                applyDocumentation(member->firstSourceLocation(), qmlPropGroup);
            }
        }
        break;
    }
    default:
        return false;
    }

    return true;
}

void QmlDocVisitor::endVisit(QDeclarativeJS::AST::UiPublicMember *definition)
{
    lastEndOffset = definition->lastSourceLocation().end();
}

bool QmlDocVisitor::visit(QDeclarativeJS::AST::IdentifierPropertyName *idproperty)
{
    return true;
}

QT_END_NAMESPACE
