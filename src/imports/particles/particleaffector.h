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

#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
#include "particlesystem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleAffector : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(QStringList particles READ particles WRITE setParticles NOTIFY particlesChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool onceOff READ onceOff WRITE setOnceOff NOTIFY onceOffChanged)

public:
    explicit ParticleAffector(QSGItem *parent = 0);
    virtual void affectSystem(qreal dt);
    virtual void reset(int systemIdx);//As some store their own data per idx?
    ParticleSystem* system() const
    {
        return m_system;
    }

    QStringList particles() const
    {
        return m_particles;
    }

    bool active() const
    {
        return m_active;
    }

    bool onceOff() const
    {
        return m_onceOff;
    }

signals:

    void systemChanged(ParticleSystem* arg);

    void particlesChanged(QStringList arg);

    void activeChanged(bool arg);

    void onceOffChanged(bool arg);

public slots:
void setSystem(ParticleSystem* arg)
{
    if (m_system != arg) {
        m_system = arg;
        m_system->registerParticleAffector(this);
        emit systemChanged(arg);
    }
}

void setParticles(QStringList arg)
{
    if (m_particles != arg) {
        m_particles = arg;
        m_updateIntSet = true;
        emit particlesChanged(arg);
    }
}

void setActive(bool arg)
{
    if (m_active != arg) {
        m_active = arg;
        emit activeChanged(arg);
    }
}

void setOnceOff(bool arg)
{
    if (m_onceOff != arg) {
        m_onceOff = arg;
        emit onceOffChanged(arg);
    }
}

protected:
    friend class ParticleSystem;
    virtual bool affectParticle(ParticleData *d, qreal dt);
    bool m_needsReset;//### What is this really saving?
    ParticleSystem* m_system;
    QStringList m_particles;
    bool activeGroup(int g) {return m_groups.isEmpty() || m_groups.contains(g);}
    bool m_active;
    virtual void componentComplete();
    QPointF m_offset;
private:
    QSet<int> m_groups;
    QSet<int> m_onceOffed;
    bool m_updateIntSet;

    bool m_onceOff;

private slots:
    void updateOffsets();
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // PARTICLEAFFECTOR_H
