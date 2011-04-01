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

#ifndef SWARMAFFECTOR_H
#define SWARMAFFECTOR_H
#include "particleaffector.h"
#include <QDeclarativeListProperty>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleType;

class SwarmAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(QStringList leaders READ leaders WRITE setLeaders NOTIFY leadersChanged)
public:
    explicit SwarmAffector(QSGItem *parent = 0);
    virtual bool affectParticle(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);

    qreal strength() const
    {
        return m_strength;
    }

    QStringList leaders() const
    {
        return m_leaders;
    }

signals:

    void strengthChanged(qreal arg);

    void leadersChanged(QStringList arg);

public slots:

void setStrength(qreal arg)
{
    if (m_strength != arg) {
        m_strength = arg;
        emit strengthChanged(arg);
    }
}

void setLeaders(QStringList arg)
{
    if (m_leaders != arg) {
        m_leaders = arg;
        emit leadersChanged(arg);
    }
}

private:
    void ensureInit();
    void mapUpdate(int idx, qreal strength);
    QVector<QPointF> m_lastPos;
    qreal m_strength;
    bool m_inited;
    QStringList m_leaders;
    QSet<int> m_leadGroups;
private slots:
    void updateGroupList();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SWARMAFFECTOR_H
