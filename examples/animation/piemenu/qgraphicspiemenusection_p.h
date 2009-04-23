/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QGRAPHICSPIEMENUSECTION_H
#define QGRAPHICSPIEMENUSECTION_H

#include <QtGui/qgraphicswidget.h>
#include <QtGui/qpainter.h>

class QGraphicsPieMenuSection : public QGraphicsWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
public:
    QGraphicsPieMenuSection(QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), rot(0)
    { }

    qreal rotation() const
    {
        return rot;
    }
    void setRotation(qreal rotation)
    {
        rot = rotation;
        setTransform(QTransform().rotate(rot));
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 1));
        painter->setBrush(QBrush(Qt::gray));
        painter->drawPie(QRectF(-100, -100, 200, 200), 0, -30 * 16);
    }

protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &size = QSizeF()) const
    {
        Q_UNUSED(which);
        Q_UNUSED(size);
        return QSizeF(100, 30);
    }

private:
    qreal rot;
};

#endif
