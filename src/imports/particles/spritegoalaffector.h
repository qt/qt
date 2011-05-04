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

#ifndef SPRITEGOALAFFECTOR_H
#define SPRITEGOALAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class SpriteEngine;

class SpriteGoalAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(QString goalState READ goalState WRITE setGoalState NOTIFY goalStateChanged)
    Q_PROPERTY(bool jump READ jump WRITE setJump NOTIFY jumpChanged)
public:
    explicit SpriteGoalAffector(QSGItem *parent = 0);

    QString goalState() const
    {
        return m_goalState;
    }

    bool jump() const
    {
        return m_jump;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void goalStateChanged(QString arg);

    void jumpChanged(bool arg);

    void affected(const QPointF &pos);
public slots:

void setGoalState(QString arg);

void setJump(bool arg)
{
    if (m_jump != arg) {
        m_jump = arg;
        emit jumpChanged(arg);
    }
}

private:
    void updateStateIndex(SpriteEngine* e);
    QString m_goalState;
    int m_goalIdx;
    SpriteEngine* m_lastEngine;
    bool m_jump;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SPRITEGOALAFFECTOR_H
