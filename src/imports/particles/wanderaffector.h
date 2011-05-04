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

#ifndef WANDERAFFECTOR_H
#define WANDERAFFECTOR_H
#include <QHash>
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class SpriteEmitter;

struct WanderData{
    qreal x_vel;
    qreal y_vel;
    qreal x_peak;
    qreal x_var;
    qreal y_peak;
    qreal y_var;
};

class WanderAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal xVariance READ xVariance WRITE setXVariance NOTIFY xVarianceChanged)
    Q_PROPERTY(qreal yVariance READ yVariance WRITE setYVariance NOTIFY yVarianceChanged)
    Q_PROPERTY(qreal pace READ pace WRITE setPace NOTIFY paceChanged)

public:
    explicit WanderAffector(QSGItem *parent = 0);
    ~WanderAffector();
    virtual void reset(int systemIdx);

    qreal xVariance() const
    {
        return m_xVariance;
    }

    qreal yVariance() const
    {
        return m_yVariance;
    }

    qreal pace() const
    {
        return m_pace;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void xVarianceChanged(qreal arg);

    void yVarianceChanged(qreal arg);

    void paceChanged(qreal arg);

public slots:
void setXVariance(qreal arg)
{
    if (m_xVariance != arg) {
        m_xVariance = arg;
        emit xVarianceChanged(arg);
    }
}

void setYVariance(qreal arg)
{
    if (m_yVariance != arg) {
        m_yVariance = arg;
        emit yVarianceChanged(arg);
    }
}

void setPace(qreal arg)
{
    if (m_pace != arg) {
        m_pace = arg;
        emit paceChanged(arg);
    }
}

private:
    WanderData* getData(int idx);
    QHash<int, WanderData*> m_wanderData;
    qreal m_xVariance;
    qreal m_yVariance;
    qreal m_pace;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // WANDERAFFECTOR_H
