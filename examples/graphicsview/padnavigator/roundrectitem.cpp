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

#include "roundrectitem.h"

#include <QtGui/QtGui>

//! [0]
RoundRectItem::RoundRectItem(const QRectF &bounds, const QColor &color,
                             QGraphicsItem *parent)
    : QGraphicsObject(parent), fillRect(false), bounds(bounds)
{
    gradient.setStart(bounds.topLeft());
    gradient.setFinalStop(bounds.bottomRight());
    gradient.setColorAt(0, color);
    gradient.setColorAt(1, color.dark(200));
    setCacheMode(ItemCoordinateCache);
}
//! [0]

//! [1]
QPixmap RoundRectItem::pixmap() const
{
    return pix;
}
void RoundRectItem::setPixmap(const QPixmap &pixmap)
{
    pix = pixmap;
    update();
}
//! [1]

//! [2]
QRectF RoundRectItem::boundingRect() const
{
    return bounds.adjusted(0, 0, 2, 2);
}
//! [2]

//! [3]
void RoundRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 64));
    painter->drawRoundRect(bounds.translated(2, 2));
//! [3]
//! [4]
    if (fillRect)
        painter->setBrush(QApplication::palette().brush(QPalette::Window));
    else
        painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 1));
    painter->drawRoundRect(bounds);
//! [4]
//! [5]
    if (!pix.isNull()) {
        painter->scale(1.95, 1.95);
        painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
    }
}
//! [5]

//! [6]
bool RoundRectItem::fill() const
{
    return fillRect;
}
void RoundRectItem::setFill(bool fill)
{
    fillRect = fill;
    update();
}
//! [6]
