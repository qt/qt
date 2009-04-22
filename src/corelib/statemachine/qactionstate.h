/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QACTIONSTATE_H
#define QACTIONSTATE_H

#ifndef QT_STATEMACHINE_SOLUTION
#include <QtCore/qabstractstate.h>
#else
#include "qabstractstate.h"
#endif

#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class QStateAction;

class QActionStatePrivate;
class Q_CORE_EXPORT QActionState : public QAbstractState
{
    Q_OBJECT
public:
    QActionState(QState *parent = 0);
    ~QActionState();

    void invokeMethodOnEntry(QObject *object, const char *method,
                             const QList<QVariant> &args = QList<QVariant>());
    void invokeMethodOnExit(QObject *object, const char *method,
                            const QList<QVariant> &args = QList<QVariant>());

    void addEntryAction(QStateAction *action);
    void addExitAction(QStateAction *action);

    void removeEntryAction(QStateAction *action);
    void removeExitAction(QStateAction *action);

    QList<QStateAction*> entryActions() const;
    QList<QStateAction*> exitActions() const;

protected:
    void onEntry();
    void onExit();

    bool event(QEvent *e);

protected:
    QActionState(QActionStatePrivate &dd, QState *parent);

private:
    Q_DISABLE_COPY(QActionState)
    Q_DECLARE_PRIVATE(QActionState)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
