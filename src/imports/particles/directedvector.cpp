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

#include "directedvector.h"
#include <cmath>

QT_BEGIN_NAMESPACE
DirectedVector::DirectedVector(QObject *parent) :
    VaryingVector(parent)
  , m_targetX(0)
  , m_targetY(0)
  , m_targetVariation(0)
  , m_proportionalMagnitude(false)
  , m_magnitude(0)
  , m_magnitudeVariation(0)
{
}

const QPointF &DirectedVector::sample(const QPointF &from)
{
    //###This approach loses interpolating the last position of the target (like we could with the emitter) is it worthwhile?
    qreal targetX = m_targetX - from.x() - m_targetVariation + rand()/(float)RAND_MAX * m_targetVariation*2;
    qreal targetY = m_targetY - from.y() - m_targetVariation + rand()/(float)RAND_MAX * m_targetVariation*2;
    qreal theta = atan2(targetY, targetX);
    qreal mag = m_magnitude + rand()/(float)RAND_MAX * m_magnitudeVariation * 2 - m_magnitudeVariation;
    if(m_proportionalMagnitude)
        mag *= sqrt(targetX * targetX + targetY * targetY);
    m_ret.setX(mag * cos(theta));
    m_ret.setY(mag * sin(theta));
    return m_ret;
}

QT_END_NAMESPACE
