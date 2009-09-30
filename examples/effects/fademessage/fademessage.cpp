/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "fademessage.h"

#include <QtGui>

FadeMessage::FadeMessage(QWidget *parent): QGraphicsView(parent), m_index(0.0)
{
    setScene(&m_scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setupScene();

    m_timeLine = new QTimeLine(500, this);
    m_timeLine->setCurveShape(QTimeLine::EaseInOutCurve);
    connect(m_timeLine, SIGNAL(valueChanged(qreal)), m_effect, SLOT(setStrength(qreal)));

    setRenderHint(QPainter::Antialiasing, true);
    setFrameStyle(QFrame::NoFrame);
}

void FadeMessage::togglePopup()
{
    if (m_message->isVisible()) {
        m_message->setVisible(false);
        m_timeLine->setDirection(QTimeLine::Backward);
        m_timeLine->start();
    } else {
        m_message->setVisible(true);
        m_timeLine->setDirection(QTimeLine::Forward);
        m_timeLine->start();
    }
}

void FadeMessage::setupScene()
{
    QGraphicsRectItem *parent = m_scene.addRect(0, 0, 400, 600);
    parent->setPen(Qt::NoPen);
    parent->setZValue(0);

    QGraphicsPixmapItem *bg = m_scene.addPixmap(QPixmap(":/background.jpg"));
    bg->setParentItem(parent);
    bg->setZValue(-1);

    for (int i = 1; i < 5; ++i)
        for (int j = 2; j < 5; ++j) {
            QGraphicsRectItem *item = m_scene.addRect(i * 50, j * 50, 38, 38);
            item->setParentItem(parent);
            item->setZValue(1);
            int hue = 12 * (i * 5 + j);
            item->setBrush(QColor::fromHsv(hue, 128, 128));
        }

    QFont font;
    font.setPointSize(font.pointSize() * 2);
    font.setBold(true);
    int fh = QFontMetrics(font).height();

    QGraphicsRectItem *block = m_scene.addRect(50, 300, 300, fh + 3);
    block->setPen(Qt::NoPen);
    block->setBrush(QColor(102, 153, 51));

    QGraphicsTextItem *text = m_scene.addText("Qt Everywhere!", font);
    text->setDefaultTextColor(Qt::white);
    text->setPos(50, 300);
    block->setZValue(2);
    block->hide();

    text->setParentItem(block);
    m_message = block;

    m_effect = new QGraphicsColorizeEffect;
    m_effect->setColor(QColor(122, 193, 66));
    m_effect->setStrength(0);
    m_effect->setEnabled(true);
    parent->setGraphicsEffect(m_effect);

    QPushButton *press = new QPushButton;
    press->setText(tr("Press me"));
    connect(press, SIGNAL(clicked()), SLOT(togglePopup()));
    m_scene.addWidget(press);
    press->move(300, 500);
}

