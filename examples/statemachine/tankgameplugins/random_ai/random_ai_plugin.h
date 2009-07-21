/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef RANDOM_AI_PLUGIN_H
#define RANDOM_AI_PLUGIN_H

#include <QObject>
#include <QState>

#include <tankgame/plugin.h>

class SelectActionState: public QState
{
    Q_OBJECT
public:
    SelectActionState(QState *parent = 0) : QState(parent)
    {
    }

signals:
    void fireSelected();
    void moveForwardsSelected();
    void moveBackwardsSelected();
    void turnSelected();

protected:
    void onEntry(QEvent *)
    {
        int rand = qrand() % 4;
        switch (rand) {
        case 0: emit fireSelected(); break;
        case 1: emit moveForwardsSelected(); break;
        case 2: emit moveBackwardsSelected(); break;
        case 3: emit turnSelected(); break;
        };
    }
};

class RandomDistanceState: public QState
{
    Q_OBJECT
public:
    RandomDistanceState(QState *parent = 0) : QState(parent)
    {
    }

signals:
    void distanceComputed(qreal distance);

protected:
    void onEntry(QEvent *)
    {
        emit distanceComputed(qreal(qrand() % 180));
    }
};

class RandomAiPlugin: public QObject, public Plugin
{
    Q_OBJECT
    Q_INTERFACES(Plugin)
public:
    RandomAiPlugin() { setObjectName("Random"); }

    virtual QState *create(QState *parentState, QObject *tank);
};

#endif // RANDOM_AI_PLUGIN_H
