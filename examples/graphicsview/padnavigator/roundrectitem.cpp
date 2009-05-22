/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "roundrectitem.h"

#include <QtGui/QtGui>

RoundRectItem::RoundRectItem(const QRectF &rect, const QBrush &brush, QWidget *embeddedWidget)
    : QGraphicsWidget(),
      m_rect(rect),
      brush(brush),
      proxyWidget(0)
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

    if (!proxyWidget)
       setScale(qreal(.9), qreal(.9));

    emit activated();
}

void RoundRectItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat() || event->key() != Qt::Key_Return) {
        QGraphicsWidget::keyReleaseEvent(event);
        return;
    }

    if (!proxyWidget)
        setScale(1, 1);
}
