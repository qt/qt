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

#include <QtCore/qglobal.h>

#ifndef QGRAPHICSBSPTREEINDEX_H
#define QGRAPHICSBSPTREEINDEX_H

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

QT_BEGIN_NAMESPACE

#include <QtCore/qrect.h>
#include <QtCore/qlist.h>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneindex.h>

#include "qgraphicsscene_bsp_p.h"

class Q_AUTOTEST_EXPORT QGraphicsSceneBspTreeIndex : public QGraphicsSceneIndex
{
    Q_OBJECT
public:
    QGraphicsSceneBspTreeIndex(QGraphicsScene *scene = 0);

    void setRect(const QRectF &rect);
    virtual QRectF rect() const;

    void clear();

    void insertItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item, bool itemIsAboutToDie);
    void updateItem(QGraphicsItem *item);

    QList<QGraphicsItem *> items(const QPointF &point);
    QList<QGraphicsItem *> items(const QRectF &rect);

    QList<QGraphicsItem *> indexedItems();

    void updateIndex();

    int bspDepth();
    void setBspDepth(int depth);

protected:
    bool event(QEvent *event);

public slots :
    void _q_updateIndex();

private :
    QGraphicsSceneBspTree bsp;
    QRectF m_sceneRect;
    int bspTreeDepth;
    int indexTimerId;
    bool restartIndexTimer;
    bool regenerateIndex;
    int lastItemCount;

    QList<QGraphicsItem *> m_indexedItems;
    QList<QGraphicsItem *> unindexedItems;
    QList<int> freeItemIndexes;

    bool purgePending;
    QList<QGraphicsItem *> removedItems;
    void purgeRemovedItems();

    void startIndexTimer();
    void resetIndex();

    void addToIndex(QGraphicsItem *item);
    void removeFromIndex(QGraphicsItem *item);
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif // QGRAPHICSBSPTREEINDEX_H
