/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
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

#include "particleextruder.h"

QT_BEGIN_NAMESPACE

ParticleExtruder::ParticleExtruder(QObject *parent) :
    QObject(parent), m_fill(true)
{
}

QPointF ParticleExtruder::extrude(const QRectF &rect)
{
    if(m_fill)
        return QPointF(((qreal)rand() / RAND_MAX) * rect.width() + rect.x(),
                       ((qreal)rand() / RAND_MAX) * rect.height() + rect.y());
    int side = rand() % 4;
    switch(side){//TODO: Doesn't this overlap the corners?
    case 0:
        return QPointF(rect.x(),
                       ((qreal)rand() / RAND_MAX) * rect.height() + rect.y());
    case 1:
        return QPointF(rect.width() + rect.x(),
                       ((qreal)rand() / RAND_MAX) * rect.height() + rect.y());
    case 2:
        return QPointF(((qreal)rand() / RAND_MAX) * rect.width() + rect.x(),
                       rect.y());
    default:
        return QPointF(((qreal)rand() / RAND_MAX) * rect.width() + rect.x(),
                       rect.height() + rect.y());
    }
}

bool ParticleExtruder::contains(const QRectF &bounds, const QPointF &point)
{
    return bounds.contains(point);
}

QT_END_NAMESPACE
