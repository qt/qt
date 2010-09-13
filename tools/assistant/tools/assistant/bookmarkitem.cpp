/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "bookmarkitem.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

BookmarkItem::BookmarkItem(const DataVector &data, BookmarkItem *parent)
    : m_data(data)
    , m_parent(parent)
{
}

BookmarkItem::~BookmarkItem()
{
    qDeleteAll(m_children);
}

BookmarkItem*
BookmarkItem::parent() const
{
    return m_parent;
}

void
BookmarkItem::setParent(BookmarkItem *parent)
{
    m_parent = parent;
}

void
BookmarkItem::addChild(BookmarkItem *child)
{
    child->setParent(this);
    m_children.append(child);
}

BookmarkItem*
BookmarkItem::child(int number) const
{
    if (number >= 0 && number < m_children.count())
        return m_children[number];
    return 0;
}

int BookmarkItem::childCount() const
{
    return m_children.count();
}

int BookmarkItem::childNumber() const
{
     if (m_parent)
         return m_parent->m_children.indexOf(const_cast<BookmarkItem*>(this));
     return 0;
}

QVariant
BookmarkItem::data(int column) const
{
    if (column == 0)
        return m_data[0];

    if (column == 1 || column == UserRoleUrl)
        return m_data[1];

    if (column == UserRoleFolder)
        return m_data[1].toString() == QLatin1String("Folder");

    if (column == UserRoleExpanded)
        return m_data[2];

    return QVariant();
}

void
BookmarkItem::setData(const DataVector &data)
{
    m_data = data;
}

bool
BookmarkItem::setData(int column, const QVariant &newValue)
{
    int index = -1;
    if (column == 0 || column == 1)
        index = column;

    if (column == UserRoleUrl || column == UserRoleFolder)
        index = 1;

    if (column == UserRoleExpanded)
        index = 2;

    if (index < 0)
        return false;

    m_data[index] = newValue;
    return true;
}

bool
BookmarkItem::insertChildren(bool isFolder, int position, int count)
{
    if (position < 0 || position > m_children.size())
        return false;

    for (int row = 0; row < count; ++row) {
        m_children.insert(position, new BookmarkItem(DataVector()
            << (isFolder
                ? QCoreApplication::translate("BookmarkItem", "New Folder")
                : QCoreApplication::translate("BookmarkItem", "Untitled"))
            << (isFolder ? "Folder" : "about:blank") << false, this));
    }

    return true;
}

bool
BookmarkItem::removeChildren(int position, int count)
{
    if (position < 0 || position > m_children.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_children.takeAt(position);

    return true;
}

void
BookmarkItem::dumpTree(int indent) const
{
    const QString tree(indent, ' ');
    qDebug() << tree + (data(UserRoleFolder).toBool() ? "Folder" : "Bookmark")
        << "Label:" << data(0).toString() << "parent:" << m_parent << "this:"
        << this;

    foreach (BookmarkItem *item, m_children)
        item->dumpTree(indent + 4);
}

QT_END_NAMESPACE
