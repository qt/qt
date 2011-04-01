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

#ifndef TOGGLEAFFECTOR_H
#define TOGGLEAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ToggleAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(bool affecting READ affecting WRITE setAffecting NOTIFY affectingChanged)
    Q_PROPERTY(ParticleAffector* affector READ affector WRITE affector NOTIFY affectorChanged)
    Q_CLASSINFO("DefaultProperty", "affector")

public:
    explicit ToggleAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    bool affecting() const
    {
        return m_affecting;
    }

    ParticleAffector* affector() const
    {
        return m_affector;
    }

signals:

    void affectingChanged(bool arg);

    void affectorChanged(ParticleAffector* arg);

public slots:
void setAffecting(bool arg)
{
    if (m_affecting != arg) {
        m_affecting = arg;
        emit affectingChanged(arg);
    }
}

void affector(ParticleAffector* arg)
{
    if (m_affector != arg) {
        m_affector = arg;
        emit affectorChanged(arg);
    }
}

private:
bool m_affecting;
ParticleAffector* m_affector;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // TOGGLEAFFECTOR_H
