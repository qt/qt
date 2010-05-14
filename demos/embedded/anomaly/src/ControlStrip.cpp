/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demos of the Qt Toolkit.
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

#include "ControlStrip.h"

#include <QtCore>
#include <QtGui>

ControlStrip::ControlStrip(QWidget *parent)
    : QWidget(parent)
{
    menuPixmap.load(":/images/edit-find.png");
    backPixmap.load(":/images/go-previous.png");
    forwardPixmap.load(":/images/go-next.png");
    closePixmap.load(":/images/button-close.png");
}

QSize ControlStrip::sizeHint() const
{
    return minimumSizeHint();
}

QSize ControlStrip::minimumSizeHint() const
{
    return QSize(320, 48);
}

void ControlStrip::mousePressEvent(QMouseEvent *event)
{
    int h = height();
    int spacing = qMin(h, (width() - h * 4) / 3);
    int x = event->pos().x();

    if (x < h) {
        emit menuClicked();
        event->accept();
        return;
    }

    if (x > width() - h) {
        emit closeClicked();
        event->accept();
        return;
    }

    if ((x < width() - (h + spacing)) && (x > width() - (h * 2 + spacing))) {
        emit forwardClicked();
        event->accept();
        return;
    }

    if ((x < width() - (h * 2 + spacing * 2)) && (x > width() - (h * 3 + spacing * 2))) {
        emit backClicked();
        event->accept();
        return;
    }
}

void ControlStrip::paintEvent(QPaintEvent *event)
{
    int h = height();
    int spacing = qMin(h, (width() - h * 4) / 3);
    int s = (height() - menuPixmap.height()) / 2;

    QPainter p(this);
    p.fillRect(event->rect(), QColor(32, 32, 32, 192));
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawPixmap(s, s, menuPixmap);
    p.drawPixmap(width() - h + s, s, closePixmap);
    p.drawPixmap(width() - (h * 2 + spacing) + s, s, forwardPixmap);
    p.drawPixmap(width() - (h * 3 + spacing * 2) + s, s, backPixmap);

    p.end();
}
