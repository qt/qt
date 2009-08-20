/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Anomaly project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
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
    int x = event->pos().x();

    if (x < h) {
        emit menuClicked();
        event->accept();
        return;
    }

    if (x > width() - h) {
        emit forwardClicked();
        event->accept();
        return;
    }

    if ((x < width() - 2 * h) && (x > width() - 3 * h)) {
        emit backClicked();
        event->accept();
        return;
    }
}

void ControlStrip::paintEvent(QPaintEvent *event)
{
    int h = height();
    int s = (h - menuPixmap.height()) / 2;

    QPainter p(this);
    p.fillRect(event->rect(), QColor(32, 32, 32, 192));
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.drawPixmap(s, s, menuPixmap);
    p.drawPixmap(width() - 3 * h + s, s, backPixmap);
    p.drawPixmap(width() - h + s, s, forwardPixmap);
    p.end();
}
