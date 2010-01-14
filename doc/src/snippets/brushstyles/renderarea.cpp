/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "renderarea.h"

RenderArea::RenderArea(QBrush *brush, QWidget *parent)
        : QWidget(parent)
{
    currentBrush = brush;
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(120, 60);
}

void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);


    if(currentBrush->style() == Qt::LinearGradientPattern) {
        currentBrush = new QBrush(QLinearGradient(0, 0, width(), 60));
    } else if(currentBrush->style() == Qt::RadialGradientPattern) {
        QRadialGradient radial(width() / 2, 30, width() / 2, width() / 2, 30);
        radial.setColorAt(0, Qt::white);
        radial.setColorAt(1, Qt::black);
        currentBrush = new QBrush(radial);
    } else if(currentBrush->style() == Qt::ConicalGradientPattern) {
        currentBrush = new QBrush(QConicalGradient(width() / 2, 30, 90));
    }
    painter.setBrush(*currentBrush);

    QPainterPath path;
    path.addRect(0, 0, parentWidget()->width(), 60);
    painter.drawPath(path);
}
