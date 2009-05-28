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

#include "qgraphicsanchorlayout_p.h"

QGraphicsAnchorLayout::QGraphicsAnchorLayout(QGraphicsLayoutItem *parent)
    : QGraphicsLayout(parent), d_ptr(new QGraphicsAnchorLayoutPrivate())
{
    // ### REMOVE THAT
    d_ptr->q_ptr = this;

    d_ptr->createLayoutEdges();
}

QGraphicsAnchorLayout::~QGraphicsAnchorLayout()
{
    Q_D(QGraphicsAnchorLayout);

    for (int i = count() - 1; i >= 0; --i) {
        QGraphicsLayoutItem *item = d->items.at(i);
        removeAt(i);
        if (item) {
            if (item->ownedByLayout())
                delete item;
        }
    }
}

void QGraphicsAnchorLayout::anchor(QGraphicsLayoutItem *firstItem,
                                   Edge firstEdge,
                                   QGraphicsLayoutItem *secondItem,
                                   Edge secondEdge, qreal spacing)
{
    Q_D(QGraphicsAnchorLayout);
    if ((firstItem == 0) || (secondItem == 0)) {
        qWarning("QGraphicsAnchorLayout::anchor: "
                 "Cannot anchor NULL items");
        return;
    }

    if (firstItem == secondItem) {
        qWarning("QGraphicsAnchorLayout::anchor: "
                 "Cannot anchor the item to itself");
        return;
    }

    if (d->edgeOrientation(secondEdge) != d->edgeOrientation(firstEdge)) {
        qWarning("QGraphicsAnchorLayout::anchor: "
                 "Cannot anchor edges of different orientations");
        return;
    }

    // In QGraphicsAnchorLayout, items are represented in its internal
    // graph as four anchors that connect:
    //  - Left -> HCenter
    //  - HCenter-> Right
    //  - Top -> VCenter
    //  - VCenter -> Bottom

    // Ensure that the internal anchors have been created for both items.
    if (firstItem != this && !d->items.contains(firstItem)) {
        d->createItemEdges(firstItem);
        d->addChildItem(firstItem);
    }
    if (secondItem != this && !d->items.contains(secondItem)) {
        d->createItemEdges(secondItem);
        d->addChildItem(secondItem);
    }

    // Use heuristics to find out what the user meant with this anchor.
    d->correctEdgeDirection(firstItem, firstEdge, secondItem, secondEdge);

    // Create actual anchor between firstItem and secondItem.
    AnchorData *data;
    if (spacing >= 0) {
        data = new AnchorData(spacing);
        d->addAnchor(firstItem, firstEdge, secondItem, secondEdge, data);
    } else {
        data = new AnchorData(-spacing);
        d->addAnchor(secondItem, secondEdge, firstItem, firstEdge, data);
    }

    invalidate();
}

void QGraphicsAnchorLayout::removeAnchor(QGraphicsLayoutItem *firstItem, Edge firstEdge,
                                         QGraphicsLayoutItem *secondItem, Edge secondEdge)
{
    Q_D(QGraphicsAnchorLayout);
    if ((firstItem == 0) || (secondItem == 0)) {
        qWarning("QGraphicsAnchorLayout::removeAnchor: "
                 "Cannot remove anchor between NULL items");
        return;
    }

    if (firstItem == secondItem) {
        qWarning("QGraphicsAnchorLayout::removeAnchor: "
                 "Cannot remove anchor from the item to itself");
        return;
    }

    if (d->edgeOrientation(secondEdge) != d->edgeOrientation(firstEdge)) {
        qWarning("QGraphicsAnchorLayout::removeAnchor: "
                 "Cannot remove anchor from edges of different orientations");
        return;
    }

    d->removeAnchor(firstItem, firstEdge, secondItem, secondEdge);

    invalidate();
}

void QGraphicsAnchorLayout::setGeometry(const QRectF &geom)
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsLayout::setGeometry(geom);
    d->calculateVertexPositions(QGraphicsAnchorLayoutPrivate::Horizontal);
    d->calculateVertexPositions(QGraphicsAnchorLayoutPrivate::Vertical);
    d->setItemsGeometries();
}

void QGraphicsAnchorLayout::removeAt(int index)
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsLayoutItem *item = d->items.value(index);

    if (item) {
        d->items.remove(index);
        d->removeAnchors(item);
        item->setParentLayoutItem(0);
    }
}

int QGraphicsAnchorLayout::count() const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->items.size();
}

QGraphicsLayoutItem *QGraphicsAnchorLayout::itemAt(int index) const
{
    Q_D(const QGraphicsAnchorLayout);
    return d->items.value(index);
}

void QGraphicsAnchorLayout::invalidate()
{
    Q_D(QGraphicsAnchorLayout);
    QGraphicsLayout::invalidate();
    d->calculateGraphCacheDirty = 1;
}

QSizeF QGraphicsAnchorLayout::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which);
    Q_UNUSED(constraint);
    Q_D(const QGraphicsAnchorLayout);

    // Some setup calculations are delayed until the information is
    // actually needed, avoiding unnecessary recalculations when
    // adding multiple anchors.

    // sizeHint() / effectiveSizeHint() already have a cache
    // mechanism, using invalidate() to force recalculation. However
    // sizeHint() is called three times after invalidation (for max,
    // min and pref), but we just need do our setup once.

    const_cast<QGraphicsAnchorLayoutPrivate *>(d)->calculateGraphs();

    // ### apply constraint!
    QSizeF engineSizeHint(
        d->sizeHints[QGraphicsAnchorLayoutPrivate::Horizontal][which],
        d->sizeHints[QGraphicsAnchorLayoutPrivate::Vertical][which]);

    qreal left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    return engineSizeHint + QSizeF(left + right, top + bottom);
}

//////// DEBUG /////////
#include <QFile>
void QGraphicsAnchorLayout::dumpGraph()
{
    Q_D(QGraphicsAnchorLayout);

    QFile file(QString::fromAscii("anchorlayout.dot"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        qWarning("Could not write to %s", file.fileName().toLocal8Bit().constData());

    QString dotContents = d->graph[0].serializeToDot();
    file.write(dotContents.toLocal8Bit());
    dotContents = d->graph[1].serializeToDot();
    file.write(dotContents.toLocal8Bit());

    file.close();
}
