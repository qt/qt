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

#include "zoneaffector.h"
#include <QDebug>

QT_BEGIN_NAMESPACE

ZoneAffector::ZoneAffector(QObject *parent) :
    ParticleAffector(parent), m_x(0), m_y(0), m_width(0), m_height(0), m_affector(0)
{
}

bool ZoneAffector::affect(ParticleData *d, qreal dt)
{
    if(!m_affector)
        return false;
    qreal x = d->curX();
    qreal y = d->curY();
    if(x >= m_x && x <= m_x+m_width && y >= m_y && y <= m_y+m_height)
        return m_affector->affect(d, dt);
    return false;
}

void ZoneAffector::reset(int systemIdx)
{
    if(m_affector)
        m_affector->reset(systemIdx);
}
QT_END_NAMESPACE
