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

#include "blurpicker.h"

#include <QtGui>

#include "blureffect.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BlurPicker::BlurPicker(QWidget *parent): QGraphicsView(parent), m_index(0.0)
{
    setBackgroundBrush(QPixmap(":/images/background.jpg"));
    setScene(&m_scene);

    setupScene();
    updateIconPositions();

    connect(&m_timeLine, SIGNAL(valueChanged(qreal)), SLOT(updateIconPositions()));
    m_timeLine.setDuration(400);

    setRenderHint(QPainter::Antialiasing, true);
    setFrameStyle(QFrame::NoFrame);
}

void BlurPicker::updateIconPositions()
{
    m_index = m_timeLine.currentFrame() / 1000.0;

    qreal baseline = 0;
    for (int i = 0; i < m_icons.count(); ++i) {
        QGraphicsItem *icon = m_icons[i];
        qreal a = ((i + m_index) * 2 * M_PI) / m_icons.count();
        qreal xs = 170 * sin(a);
        qreal ys = 100 * cos(a);
        QPointF pos(xs, ys);
        pos = QTransform().rotate(-20).map(pos);
        pos -= QPointF(40, 40);
        icon->setPos(pos);
        baseline = qMax(baseline, ys);
        static_cast<BlurEffect *>(icon->graphicsEffect())->setBaseLine(baseline);
    }

    m_scene.update();
}

void BlurPicker::setupScene()
{
    m_scene.setSceneRect(-200, -120, 400, 240);

    QStringList names;
    names << ":/images/accessories-calculator.png";
    names << ":/images/accessories-text-editor.png";
    names << ":/images/help-browser.png";
    names << ":/images/internet-group-chat.png";
    names << ":/images/internet-mail.png";
    names << ":/images/internet-web-browser.png";
    names << ":/images/office-calendar.png";
    names << ":/images/system-users.png";

    for (int i = 0; i < names.count(); i++) {
        QPixmap pixmap(names[i]);
        QGraphicsPixmapItem *icon = m_scene.addPixmap(pixmap);
        icon->setZValue(1);
        icon->setGraphicsEffect(new BlurEffect(icon));
        m_icons << icon;
    }

    QGraphicsPixmapItem *bg = m_scene.addPixmap(QPixmap(":/images/background.jpg"));
    bg->setZValue(0);
    bg->setPos(-200, -150);
}

void BlurPicker::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        if (m_timeLine.state() == QTimeLine::NotRunning) {
            m_timeLine.setFrameRange(m_index * 1000, m_index * 1000 - 1000);
            m_timeLine.start();
            event->accept();
        }
    }

    if (event->key() == Qt::Key_Right) {
        if (m_timeLine.state() == QTimeLine::NotRunning) {
            m_timeLine.setFrameRange(m_index * 1000, m_index * 1000 + 1000);
            m_timeLine.start();
            event->accept();
        }
    }
}
