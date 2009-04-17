/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
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
