/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "xbelreader.h"

//! [0]
XbelReader::XbelReader(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    QStyle *style = treeWidget->style();

    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
    bookmarkIcon.addPixmap(style->standardPixmap(QStyle::SP_FileIcon));
}
//! [0]

//! [1]
bool XbelReader::read(QIODevice *device)
{
    setDevice(device);

    while (!atEnd()) {
        readNext();

        if (isStartElement()) {
            if (name() == "xbel" && attributes().value("version") == "1.0")
                readXBEL();
            else
                raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
        }
    }

    return !error();
}
//! [1]

//! [2]
void XbelReader::readUnknownElement()
{
    Q_ASSERT(isStartElement());

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement())
            readUnknownElement();
    }
}
//! [2]

//! [3]
void XbelReader::readXBEL()
{
    Q_ASSERT(isStartElement() && name() == "xbel");

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "folder")
                readFolder(0);
            else if (name() == "bookmark")
                readBookmark(0);
            else if (name() == "separator")
                readSeparator(0);
            else
                readUnknownElement();
        }
    }
}
//! [3]

//! [4]
void XbelReader::readTitle(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "title");

    QString title = readElementText();
    item->setText(0, title);
}
//! [4]

//! [5]
void XbelReader::readSeparator(QTreeWidgetItem *item)
{
    QTreeWidgetItem *separator = createChildItem(item);
    separator->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    separator->setText(0, QString(30, 0xB7));
    readElementText();
}
//! [5]

void XbelReader::readFolder(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "folder");

    QTreeWidgetItem *folder = createChildItem(item);
    bool folded = (attributes().value("folded") != "no");
    treeWidget->setItemExpanded(folder, !folded);

    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(folder);
            else if (name() == "folder")
                readFolder(folder);
            else if (name() == "bookmark")
                readBookmark(folder);
            else if (name() == "separator")
                readSeparator(folder);
            else
                readUnknownElement();
        }
    }
}

void XbelReader::readBookmark(QTreeWidgetItem *item)
{
    Q_ASSERT(isStartElement() && name() == "bookmark");

    QTreeWidgetItem *bookmark = createChildItem(item);
    bookmark->setFlags(bookmark->flags() | Qt::ItemIsEditable);
    bookmark->setIcon(0, bookmarkIcon);
    bookmark->setText(0, QObject::tr("Unknown title"));
    bookmark->setText(1, attributes().value("href").toString());
    while (!atEnd()) {
        readNext();

        if (isEndElement())
            break;

        if (isStartElement()) {
            if (name() == "title")
                readTitle(bookmark);
            else
                readUnknownElement();
        }
    }
}

QTreeWidgetItem *XbelReader::createChildItem(QTreeWidgetItem *item)
{
    QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
        childItem = new QTreeWidgetItem(treeWidget);
    }
    childItem->setData(0, Qt::UserRole, name().toString());
    return childItem;
}
