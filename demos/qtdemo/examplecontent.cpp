/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
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

#include "examplecontent.h"
#include "colors.h"
#include "menumanager.h"
#include "imageitem.h"
#include "headingitem.h"

ExampleContent::ExampleContent(const QString &name, QGraphicsScene *scene, QGraphicsItem *parent)
    : DemoItem(scene, parent)
{
    this->name = name;
    this->heading = 0;
    this->description = 0;
    this->screenshot = 0;
}

void ExampleContent::prepare()
{
    if (!this->prepared){
        this->prepared = true;
        this->createContent();
    }
}

void ExampleContent::animationStopped(int id)
{
    if (id == DemoItemAnimation::ANIM_OUT){
        // Free up some memory:
        delete this->heading;
        delete this->description;
        delete this->screenshot;
        this->heading = 0;
        this->description = 0;
        this->screenshot = 0;
        this->prepared = false;
    }
}

QString ExampleContent::loadDescription()
{
    QByteArray ba = MenuManager::instance()->getHtml(this->name);
    QString errorMsg;
    int errorLine, errorColumn;

    QDomDocument exampleDoc;
    if (ba.isEmpty()) {
        qDebug() << "No documentation found for" << name << "Is the documentation built?";
    } else if (!exampleDoc.setContent(ba, false, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << "Error loading documentation for " << name << ": " << errorMsg << errorLine << errorColumn;
    }

    QDomNodeList paragraphs = exampleDoc.elementsByTagName("p");
    if (paragraphs.length() < 1 && Colors::verbose)
        qDebug() << "- ExampleContent::loadDescription(): Could not load description:"
                 << MenuManager::instance()->info[this->name]["docfile"];
    QString description = Colors::contentColor + QLatin1String(""); 
    //QLatin1String("Could not load description. Ensure that the documentation for Qt is built."); // QTBUG-12522: If there is no description why show an error to the user when qDebug above communications the issue (if it is indeed an issue at all) when demos are built?
    for (int p = 0; p < int(paragraphs.length()); ++p) {
        description = this->extractTextFromParagraph(paragraphs.item(p));
        if (this->isSummary(description)) {
            break;
        }
    }
    return Colors::contentColor + description;
}

bool ExampleContent::isSummary(const QString &text)
{
    return (!text.contains("[") &&
        text.indexOf(QRegExp(QString("(In )?((The|This) )?(%1 )?.*(tutorial|example|demo|application)").arg(this->name),
                             Qt::CaseInsensitive)) != -1);
}

QString ExampleContent::extractTextFromParagraph(const QDomNode &parentNode)
{
    QString description;
    QDomNode node = parentNode.firstChild();

    while (!node.isNull()) {
        QString beginTag;
        QString endTag;
        if (node.isText())
            description += Colors::contentColor + node.nodeValue();
        else if (node.hasChildNodes()) {
            if (node.nodeName() == "b") {
                beginTag = "<b>";
                endTag = "</b>";
            } else if (node.nodeName() == "a") {
                beginTag = Colors::contentColor;
                endTag = "</font>";
            } else if (node.nodeName() == "i") {
                beginTag = "<i>";
                endTag = "</i>";
            } else if (node.nodeName() == "tt") {
                beginTag = "<tt>";
                endTag = "</tt>";
            }
            description += beginTag + this->extractTextFromParagraph(node) + endTag;
        }
        node = node.nextSibling();
    }

    return description;
}

void ExampleContent::createContent()
{
    // Create the items:
    this->heading = new HeadingItem(this->name, this->scene(), this);
    this->description = new DemoTextItem(this->loadDescription(), Colors::contentFont(),
        Colors::heading, 500, this->scene(), this);
    int imgHeight = 340 - int(this->description->boundingRect().height()) + 50;
    this->screenshot = new ImageItem(QImage::fromData(MenuManager::instance()->getImage(this->name)),
          550, imgHeight, this->scene(), this);

    // Place the items on screen:
    this->heading->setPos(0, 3);
    this->description->setPos(0, this->heading->pos().y() + this->heading->boundingRect().height() + 10);
    this->screenshot->setPos(0, this->description->pos().y() + this->description->boundingRect().height() + 10);
}

QRectF ExampleContent::boundingRect() const
{
    return QRectF(0, 0, 500, 100);
}


