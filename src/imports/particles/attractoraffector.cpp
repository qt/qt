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

#include "attractoraffector.h"
#include <cmath>
#include <QDebug>
QT_BEGIN_NAMESPACE
AttractorAffector::AttractorAffector(QSGItem *parent) :
    ParticleAffector(parent), m_strength(0.0), m_x(0), m_y(0)
{
}

bool AttractorAffector::affectParticle(ParticleData *d, qreal dt)
{
    if(m_strength == 0.0)
        return false;
    qreal dx = m_x - d->curX();
    qreal dy = m_y - d->curY();
    qreal r = sqrt((dx*dx) + (dy*dy));
    qreal theta = atan2(dy,dx);
    qreal ds = (m_strength / r) * dt;
    dx = ds * cos(theta);
    dy = ds * sin(theta);
    d->setInstantaneousSX(d->pv.sx + dx);
    d->setInstantaneousSY(d->pv.sy + dy);
    return true;
}
QT_END_NAMESPACE
