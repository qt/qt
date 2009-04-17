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

#include "roundrectitem.h"

#include <QtGui/QtGui>

RoundRectItem::RoundRectItem(const QRectF &rect, const QBrush &brush, QWidget *embeddedWidget)
    : QGraphicsWidget(),
      brush(brush),
      proxyWidget(0),
      m_rect(rect)
{
    if (embeddedWidget) {
        proxyWidget = new QGraphicsProxyWidget(this);
        proxyWidget->setFocusPolicy(Qt::StrongFocus);
        proxyWidget->setWidget(embeddedWidget);
    }
}

void RoundRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    const bool widgetHidden = parentItem() == 0 || qAbs(static_cast<QGraphicsWidget*>(parentItem())->yRotation()) < 90;
    
    if (proxyWidget) {
        if (widgetHidden) {
            proxyWidget->hide();
        } else {
            if (!proxyWidget->isVisible()) {
                proxyWidget->setGeometry(boundingRect().adjusted(25, 25, -25, -25));
                proxyWidget->show();
                proxyWidget->setFocus();
            }
            painter->setBrush(brush);
            painter->setPen(QPen(Qt::black, 1));
            painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
            painter->drawRoundRect(m_rect);
        }
    } else if (widgetHidden) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0, 0, 0, 64));
        painter->drawRoundRect(m_rect.translated(2, 2));

        QLinearGradient gradient(m_rect.topLeft(), m_rect.bottomRight());
        const QColor col = brush.color();
        gradient.setColorAt(0, col);
        gradient.setColorAt(1, col.dark(200));
        painter->setBrush(gradient);
        painter->setPen(QPen(Qt::black, 1));
        painter->drawRoundRect(m_rect);
        if (!pix.isNull()) {
            painter->scale(qreal(1.95), qreal(1.95));
            painter->drawPixmap(-pix.width() / 2, -pix.height() / 2, pix);
        }
    }

}

QRectF RoundRectItem::boundingRect() const
{
    qreal penW = qreal(.5);
    qreal shadowW = 2;
    return m_rect.adjusted(-penW, -penW, penW + shadowW, penW + shadowW);
}

void RoundRectItem::setPixmap(const QPixmap &pixmap)
{
    pix = pixmap;
    if (scene() && isVisible())
        update();
}

void RoundRectItem::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || event->key() != Qt::Key_Return) {
        QGraphicsWidget::keyPressEvent(event);
        return;
    }

    if (!proxyWidget) {
       setXScale(qreal(.9));
       setYScale(qreal(.9));
    }
    emit activated();
}

void RoundRectItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || event->key() != Qt::Key_Return) {
        QGraphicsWidget::keyReleaseEvent(event);
        return;
    }

    if (!proxyWidget) {
        setXScale(1);
        setYScale(1);
    }
}
