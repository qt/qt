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

#ifndef GRAVITYAFFECTOR_H
#define GRAVITYAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class GravityAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
public:
    explicit GravityAffector(QSGItem *parent = 0);
    qreal acceleration() const
    {
        return m_acceleration;
    }

    qreal angle() const
    {
        return m_angle;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void accelerationChanged(qreal arg);

    void angleChanged(qreal arg);

public slots:
void setAcceleration(qreal arg)
{
    if (m_acceleration != arg) {
        m_acceleration = arg;
        emit accelerationChanged(arg);
    }
}

void setAngle(qreal arg)
{
    if (m_angle != arg) {
        m_angle = arg;
        emit angleChanged(arg);
    }
}

private slots:
    void recalc();
private:
    qreal m_acceleration;
    qreal m_angle;

    qreal m_xAcc;
    qreal m_yAcc;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // GRAVITYAFFECTOR_H
