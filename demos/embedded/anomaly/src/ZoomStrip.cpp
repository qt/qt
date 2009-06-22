/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ZoomStrip.h"

#include <QtCore>
#include <QtGui>

ZoomStrip::ZoomStrip(QWidget *parent)
    : QWidget(parent)
{
    zoomInPixmap.load(":/images/list-add.png");
    zoomOutPixmap.load(":/images/list-remove.png");
}

QSize ZoomStrip::sizeHint() const
{
    return minimumSizeHint();
}

QSize ZoomStrip::minimumSizeHint() const
{
    return QSize(48, 96);
}

void ZoomStrip::mousePressEvent(QMouseEvent *event)
{
    if (event->pos().y() < height() / 2)
        emit zoomInClicked();
    else
        emit zoomOutClicked();
}

void ZoomStrip::paintEvent(QPaintEvent *event)
{
    int w = width();
    int s = (w - zoomInPixmap.width()) / 2;

    QPainter p(this);
    p.fillRect(event->rect(), QColor(128, 128, 128, 128));
    p.drawPixmap(s, s, zoomInPixmap);
    p.drawPixmap(s, s + w, zoomOutPixmap);
    p.end();
}
