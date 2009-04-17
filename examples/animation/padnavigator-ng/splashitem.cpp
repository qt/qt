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

#include "splashitem.h"

#include <QtGui/QtGui>

SplashItem::SplashItem(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{

    text = tr("Welcome to the Pad Navigator Example. You can use the"
              " keyboard arrows to navigate the icons, and press enter"
              " to activate an item. Please press any key to continue.");
    resize(400, 175);
}

void SplashItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(QPen(Qt::black, 2));
    painter->setBrush(QColor(245, 245, 255, 220));
    painter->setClipRect(rect());
    painter->drawRoundRect(3, -100 + 3, 400 - 6, 250 - 6);

    QRectF textRect = rect().adjusted(10, 10, -10, -10);
    int flags = Qt::AlignTop | Qt::AlignLeft | Qt::TextWordWrap;

    QFont font;
    font.setPixelSize(18);
    painter->setPen(Qt::black);
    painter->setFont(font);
    painter->drawText(textRect, flags, text);
}

void SplashItem::keyPressEvent(QKeyEvent * /* event */)
{
    QVariantAnimation *anim = new QPropertyAnimation(this, "pos");
    anim->setEndValue(QPointF(x(), scene()->sceneRect().top() - rect().height()));
    anim->setDuration(350);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    anim = new QPropertyAnimation(this, "opacity");
    anim->setEndValue(0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    connect(anim, SIGNAL(finished()), SLOT(close()));
}
