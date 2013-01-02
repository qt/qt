/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "guideline.h"
#include <cmath>

GuideLine::GuideLine(const QLineF &line, Guide *follows) : Guide(follows)
{
    this->line = line;
}

GuideLine::GuideLine(const QPointF &end, Guide *follows) : Guide(follows)
{
    if (follows)
        this->line = QLineF(prevGuide->endPos(), end);
    else
        this->line = QLineF(QPointF(0, 0), end);
}

float GuideLine::length()
{
    return line.length();
}

QPointF GuideLine::startPos()
{
    return QPointF(this->line.p1().x() * scaleX, this->line.p1().y() * scaleY);
}

QPointF GuideLine::endPos()
{
    return QPointF(this->line.p2().x() * scaleX, this->line.p2().y() * scaleY);
}

void GuideLine::guide(DemoItem *item, float moveSpeed)
{
    float frame = item->guideFrame - this->startLength;
    float endX = (this->line.p1().x() + (frame * this->line.dx() / this->length())) * scaleX;
    float endY = (this->line.p1().y() + (frame * this->line.dy() / this->length())) * scaleY;
    QPointF pos(endX, endY);
    this->move(item, pos, moveSpeed);
}

