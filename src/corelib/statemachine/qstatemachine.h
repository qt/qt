/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSTATEMACHINE_H
#define QSTATEMACHINE_H

#include <QtCore/qstate.h>

#include <QtCore/qlist.h>
#include <QtCore/qobject.h>
#include <QtCore/qset.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_STATEMACHINE

class QEvent;

class QStateMachinePrivate;
class QAbstractAnimation;
class Q_CORE_EXPORT QStateMachine : public QState
{
    Q_OBJECT
    Q_PROPERTY(QString errorString READ errorString)
    Q_PROPERTY(RestorePolicy globalRestorePolicy READ globalRestorePolicy WRITE setGlobalRestorePolicy)
    Q_ENUMS(RestorePolicy)
#ifndef QT_NO_ANIMATION
    Q_PROPERTY(bool animationsEnabled READ animationsEnabled WRITE setAnimationsEnabled)
#endif
public:
    enum RestorePolicy {
        DoNotRestoreProperties,
        RestoreProperties
    };

    enum Error {
        NoError, 
        NoInitialStateError,
        NoDefaultStateInHistoryStateError,
        NoCommonAncestorForTransitionError
    };

    QStateMachine(QObject *parent = 0);
    ~QStateMachine();

    void addState(QAbstractState *state);
    void removeState(QAbstractState *state);

    Error error() const;
    QString errorString() const;
    void clearError();

    bool isRunning() const;

#ifndef QT_NO_ANIMATION
    bool animationsEnabled() const;
    void setAnimationsEnabled(bool enabled);

    void addDefaultAnimation(QAbstractAnimation *animation);
    QList<QAbstractAnimation *> defaultAnimations() const;
    void removeDefaultAnimation(QAbstractAnimation *animation);
#endif // QT_NO_ANIMATION

    QStateMachine::RestorePolicy globalRestorePolicy() const;
    void setGlobalRestorePolicy(QStateMachine::RestorePolicy restorePolicy);

    void postEvent(QEvent *event, int delay = 0);

    QSet<QAbstractState*> configuration() const;

#ifndef QT_NO_STATEMACHINE_EVENTFILTER
    bool eventFilter(QObject *watched, QEvent *event);
#endif

public Q_SLOTS:
    void start();
    void stop();

Q_SIGNALS:
    void started();
    void stopped();

protected:
    void onEntry(QEvent *event);
    void onExit(QEvent *event);

    void postInternalEvent(QEvent *event);

    virtual void beginSelectTransitions(QEvent *event);
    virtual void endSelectTransitions(QEvent *event);

    virtual void beginMicrostep(QEvent *event);
    virtual void endMicrostep(QEvent *event);

    bool event(QEvent *e);

protected:
    QStateMachine(QStateMachinePrivate &dd, QObject *parent);

private:
    Q_DISABLE_COPY(QStateMachine)
    Q_DECLARE_SCOPED_PRIVATE(QStateMachine)
    Q_PRIVATE_SLOT(d_func(), void _q_start())
    Q_PRIVATE_SLOT(d_func(), void _q_process())
#ifndef QT_NO_ANIMATION
    Q_PRIVATE_SLOT(d_func(), void _q_animationFinished())
#endif
};

#endif //QT_NO_STATEMACHINE

QT_END_NAMESPACE

QT_END_HEADER

#endif
