/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "spin_ai_with_error.h"

#include <QtPlugin>

QState *SpinAiWithError::create(QState *parentState, QObject *tank)
{
    QState *topLevel = new QState(parentState);
    QState *spinState = new SpinState(tank, topLevel);
    topLevel->setInitialState(spinState);

    // When tank is spotted, fire two times and go back to spin state
    // (no initial state set for fireState will lead to run-time error in machine)
    QState *fireState = new QState(topLevel);

    QState *fireOnce = new QState(fireState);
    connect(fireOnce, SIGNAL(entered()), tank, SLOT(fireCannon()));

    QState *fireTwice = new QState(fireState);
    connect(fireTwice, SIGNAL(entered()), tank, SLOT(fireCannon()));

    fireOnce->addTransition(tank, SIGNAL(actionCompleted()), fireTwice);
    fireTwice->addTransition(tank, SIGNAL(actionCompleted()), spinState);

    spinState->addTransition(tank, SIGNAL(tankSpotted(qreal,qreal)), fireState);

    return topLevel;
}

Q_EXPORT_PLUGIN2(spin_ai_with_error, SpinAiWithError)
