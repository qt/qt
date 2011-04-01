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

#ifndef ZONEAFFECTOR_H
#define ZONEAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ZoneAffector : public ParticleAffector
{
    Q_OBJECT
    //TODO: Can we get anchors in here? consider becoming an un-parented QSGItem?
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged);
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged);
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged);
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged);
    Q_PROPERTY(ParticleAffector* affector READ affector WRITE affector NOTIFY affectorChanged)
    Q_CLASSINFO("DefaultProperty", "affector")
public:
    explicit ZoneAffector(QObject *parent = 0);

    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);

    ParticleAffector* affector() const
    {
        return m_affector;
    }

    qreal x() const
    {
        return m_x;
    }

    qreal y() const
    {
        return m_y;
    }

    qreal width() const
    {
        return m_width;
    }

    qreal height() const
    {
        return m_height;
    }

signals:


    void affectorChanged(ParticleAffector* arg);

    void xChanged(qreal arg);

    void yChanged(qreal arg);

    void widthChanged(qreal arg);

    void heightChanged(qreal arg);

public slots:


void affector(ParticleAffector* arg)
{
    if (m_affector != arg) {
        m_affector = arg;
        emit affectorChanged(arg);
    }
}

void setX(qreal arg)
{
    if (m_x != arg) {
        m_x = arg;
        emit xChanged(arg);
    }
}

void setY(qreal arg)
{
    if (m_y != arg) {
        m_y = arg;
        emit yChanged(arg);
    }
}

void setWidth(qreal arg)
{
    if (m_width != arg) {
        m_width = arg;
        emit widthChanged(arg);
    }
}

void setHeight(qreal arg)
{
    if (m_height != arg) {
        m_height = arg;
        emit heightChanged(arg);
    }
}

private:
qreal m_x;
qreal m_y;
qreal m_width;
qreal m_height;
ParticleAffector* m_affector;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // ZONEAFFECTOR_H
