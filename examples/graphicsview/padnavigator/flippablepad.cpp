/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "flippablepad.h"

#include <QtGui/QtGui>

//! [0]
static QRectF boundsFromSize(const QSize &size)
{
    return QRectF((-size.width() / 2.0) * 150, (-size.height() / 2.0) * 150,
                  size.width() * 150, size.height() * 150);
}
//! [0]

//! [1]
static QPointF posForLocation(int column, int row, const QSize &size)
{
    return QPointF(column * 150, row * 150)
        - QPointF((size.width() - 1) * 75, (size.height() - 1) * 75);
}
//! [1]

//! [2]
FlippablePad::FlippablePad(const QSize &size, QGraphicsItem *parent)
    : RoundRectItem(boundsFromSize(size), QColor(226, 255, 92, 64), parent)
{
//! [2]
//! [3]
    int numIcons = size.width() * size.height();
    QList<QPixmap> pixmaps;
    QDirIterator it(":/images", QStringList() << "*.png");
    while (it.hasNext() && pixmaps.size() < numIcons)
        pixmaps << it.next();
//! [3]

//! [4]
    const QRectF iconRect(-54, -54, 108, 108);
    const QColor iconColor(214, 240, 110, 128);
    iconGrid.resize(size.height());
    int n = 0;

    for (int y = 0; y < size.height(); ++y) {
        iconGrid[y].resize(size.width());
        for (int x = 0; x < size.width(); ++x) {
            RoundRectItem *rect = new RoundRectItem(iconRect, iconColor, this);
            rect->setZValue(1);
            rect->setPos(posForLocation(x, y, size));
            rect->setPixmap(pixmaps.at(n++ % pixmaps.size()));
            iconGrid[y][x] = rect;
        }
    }
}
//! [4]

//! [5]
RoundRectItem *FlippablePad::iconAt(int column, int row) const
{
    return iconGrid[row][column];
}
//! [5]
