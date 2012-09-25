/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "animateditem.h"

#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QGraphicsScene>

AnimatedPixmapItem::AnimatedPixmapItem(const QList<QPixmap> &animation,
                                       QGraphicsScene *scene)
    : QGraphicsItem(0), currentFrame(0), vx(0), vy(0)
{
    for (int i = 0; i < animation.size(); ++i) {
        QPixmap pixmap = animation.at(i);
        Frame frame;
        frame.pixmap = pixmap;
        frame.shape = QPainterPath();
        frame.boundingRect = pixmap.rect();
        frames << frame;
    }

    scene->addItem(this);
}

void AnimatedPixmapItem::setFrame(int frame)
{
    if (!frames.isEmpty()) {
        prepareGeometryChange();
        currentFrame = frame % frames.size();
    }

    //scene->addItem(this);
}

void AnimatedPixmapItem::advance(int phase)
{
    if (phase == 1)
        moveBy(vx, vy);
}

QRectF AnimatedPixmapItem::boundingRect() const
{
    return frames.at(currentFrame).boundingRect;
}

QPainterPath AnimatedPixmapItem::shape() const
{
    const Frame &f = frames.at(currentFrame);
    if (f.shape.isEmpty()) {
        QPainterPath path;
        path.addRegion(f.pixmap.createHeuristicMask());
        const_cast<Frame &>(f).shape = path;
    }
    return f.shape;
}

void AnimatedPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/,
                               QWidget * /*widget*/)
{
    painter->drawPixmap(0, 0, frames.at(currentFrame).pixmap);
}
