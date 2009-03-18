/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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

#include "qgraphicssceneindex.h"
#include "qgraphicsscene.h"

#ifndef QT_NO_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

/*!
    Constructs an abstract scene index.
*/
QGraphicsSceneIndex::QGraphicsSceneIndex(QObject *parent): QObject(parent)
{
}

/*!
    Destroys the scene index.
*/
QGraphicsSceneIndex::~QGraphicsSceneIndex()
{

}

/*!
    \fn virtual void setRect(const QRectF &rect) = 0

    This pure virtual function is called when the scene changes its bounding
    rectangle.

    \sa rect(), QGraphicsScene::setSceneRect
*/

/*!
    \fn virtual QRectF rect() const = 0

    This pure virtual function returns the bounding rectangle of this
    scene index. It could be as large as or larger than the scene
    bounding rectangle, depending on the implementation of the
    scene index.

    \sa setRect(), QGraphicsScene::sceneRect
*/

/*!
    \fn virtual void clear() = 0

    This pure virtual function removes all items in the scene index.
*/

/*!
    \fn virtual void insertItem(QGraphicsItem *item) = 0

    This pure virtual function inserts an item to the scene index.

    \sa removeItem(), updateItem(), insertItems()
*/

/*!
    \fn virtual void removeItem(QGraphicsItem *item) = 0

    This pure virtual function removes an item to the scene index.

    \sa insertItem(), updateItem(), removeItems()
*/

/*!
    Returns the scene of this scene index.
*/
QGraphicsScene* QGraphicsSceneIndex::scene()
{
     return mscene;
}

/*!
    Updates an item when its geometry has changed.

    The default implemention will remove the item from the index
    and then insert it again.

    \sa insertItem(), removeItem(), updateItems()
*/
void QGraphicsSceneIndex::updateItem(QGraphicsItem *item)
{
    removeItem(item);
    insertItem(item);
}

/*!
    Inserts a list of items to the index.

    The default implemention will insert the items one by one.

    \sa insertItem(), removeItems(), updateItems()
*/
void QGraphicsSceneIndex::insertItems(const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem *item, items)
        insertItem(item);
}

/*!
    Removes a list of items from the index.

    The default implemention will remove the items one by one.

    \sa removeItem(), removeItems(), updateItems()
*/
void QGraphicsSceneIndex::removeItems(const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem *item, items)
        removeItem(item);
}

/*!
    Update a list of items which have changed the geometry.

    The default implemention will update the items one by one.

    \sa updateItem(), insertItems(), removeItems()
*/
void QGraphicsSceneIndex::updateItems(const QList<QGraphicsItem *> &items)
{
    foreach (QGraphicsItem *item, items)
        updateItem(item);
}

QT_END_NAMESPACE

#include "moc_qgraphicssceneindex.cpp"

#endif // QT_NO_GRAPHICSVIEW
