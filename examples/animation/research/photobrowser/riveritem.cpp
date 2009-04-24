/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "riveritem.h"
#include "river.h"

RiverItem::RiverItem(QGraphicsItem *parent) : QGraphicsPixmapItem(parent), m_fullscreen(false)
{
    setCacheMode(DeviceCoordinateCache);
}

RiverItem::~RiverItem()
{
}

void RiverItem::setPixmap(const QPixmap &pix)
{
    const QSize oldSize = pixmap().size();
    const QSize newSize = pix.size();
    QGraphicsPixmapItem::setPixmap(pix);

    if (newSize != oldSize) {
        setOffset(-newSize.width()/2, -newSize.height()/2);
        const qreal scaleFactor = qreal(River::fixedSize().height())/(qreal(pix.height()*7));
        setTransform(QTransform().scale(scaleFactor, scaleFactor));
        prepareGeometryChange();
    }
}


void RiverItem::setFullScreen(bool b, qreal originScaleFactor)
{
    if (m_fullscreen == b)
        return;

    m_fullscreen = b;

    QPointF newPos;
    qreal rotationZ;
    qreal scaleX, scaleY;

    if (b) {
        const QSizeF basePixmapSize = transform().map(boundingRect()).boundingRect().size();

        newPos = parentItem()->boundingRect().center();
        rotationZ = 90;
        scaleY = qreal(River::fixedSize().width()) / basePixmapSize.height() * yScale();
        scaleX = qreal(River::fixedSize().height()) / basePixmapSize.width() * xScale();

        if (m_nonFSPos.isNull()) {
           m_nonFSPos = pos(); //let's save our current (non fullscreen) position
        }

    } else {
        Q_ASSERT(!m_nonFSPos.isNull());
        rotationZ = 0;
        scaleX = originScaleFactor;
        scaleY = originScaleFactor;
        newPos = m_nonFSPos;
    }

    QAnimationGroup *group = new QParallelAnimationGroup(scene());
    QItemAnimation *anim = new QItemAnimation(this, QItemAnimation::Position);
    anim->setEndValue(newPos);
    group->addAnimation(anim);
    anim = new QItemAnimation(this, QItemAnimation::RotationZ);
    anim->setEndValue(rotationZ);
    group->addAnimation(anim);
    anim = new QItemAnimation(this, QItemAnimation::ScaleFactorX);
    anim->setEndValue(scaleX);
    group->addAnimation(anim);
    anim = new QItemAnimation(this, QItemAnimation::ScaleFactorY);
    anim->setEndValue(scaleY);
    group->addAnimation(anim);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void RiverItem::mousePressEvent(QGraphicsSceneMouseEvent*)
{
    //just let it rotate on itself
    QItemAnimation *anim = new QItemAnimation(this, QItemAnimation::RotationY);
    anim->setEndValue(yRotation() < 180 ? 360 : 0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}


