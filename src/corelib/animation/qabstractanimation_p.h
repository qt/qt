/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QABSTRACTANIMATION_P_H
#define QABSTRACTANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of QIODevice. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qbasictimer.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qtimer.h>
#include <private/qobject_p.h>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

#ifndef QT_NO_ANIMATION

QT_BEGIN_NAMESPACE

class QAnimationGroup;
class QAbstractAnimation;
class QAbstractAnimationPrivate : public QObjectPrivate
{
public:
    QAbstractAnimationPrivate()
        : state(QAbstractAnimation::Stopped),
          direction(QAbstractAnimation::Forward),
          totalCurrentTime(0),
          currentTime(0),
          loopCount(1),
          currentLoop(0),
          deleteWhenStopped(false),
          hasRegisteredTimer(false),
          isPause(false),
          isGroup(false),
          group(0)
    {
    }

    virtual ~QAbstractAnimationPrivate() {}

    static QAbstractAnimationPrivate *get(QAbstractAnimation *q)
    {
        return q->d_func();
    }

    QAbstractAnimation::State state;
    QAbstractAnimation::Direction direction;
    void setState(QAbstractAnimation::State state);

    int totalCurrentTime;
    int currentTime;
    int loopCount;
    int currentLoop;

    bool deleteWhenStopped;
    bool hasRegisteredTimer;
    bool isPause;
    bool isGroup;

    QAnimationGroup *group;

private:
    Q_DECLARE_PUBLIC(QAbstractAnimation)
};

class ElapsedTimer
{
public:
    ElapsedTimer() {
        invalidate();
    }

    void invalidate() {
        m_started = -1;
    }

    bool isValid() const {
        return m_started >= 0;
    }

    void start() {
        m_started = getTickCount_sys();
    }

    qint64 elapsed() const {
        qint64 current = getTickCount_sys();
        qint64 delta = current - m_started;
        if (delta < 0)
            delta += getPeriod_sys();   //we wrapped around
        return delta;
    }

private:
    enum {
        MSECS_PER_HOUR = 3600000,
        MSECS_PER_MIN = 60000
    };

    qint64 m_started;

    quint64 getPeriod_sys() const {
#ifdef Q_OS_WIN
        return Q_UINT64_C(0x100000000);
#else
        // fallback
        return 86400 * 1000;
#endif
    }

    qint64 getTickCount_sys() const {
#ifdef Q_OS_WIN
        return ::GetTickCount();
#else
        // fallback
        const QTime t = QTime::currentTime();
        return MSECS_PER_HOUR * t.hour() + MSECS_PER_MIN * t.minute() + 1000 * t.second() + t.msec();
#endif
    }
};


class QUnifiedTimer : public QObject
{
private:
    QUnifiedTimer();

public:
    //XXX this is needed by dui
    static Q_CORE_EXPORT QUnifiedTimer *instance();

    void registerAnimation(QAbstractAnimation *animation, bool isTopLevel);
    void unregisterAnimation(QAbstractAnimation *animation);

    //defines the timing interval. Default is DEFAULT_TIMER_INTERVAL
    void setTimingInterval(int interval)
    {
        timingInterval = interval;
        if (animationTimer.isActive() && !isPauseTimerActive) {
            //we changed the timing interval
            animationTimer.start(timingInterval, this);
        }
    }

    /*
       this allows to have a consistent timer interval at each tick from the timer
       not taking the real time that passed into account.
    */
    void setConsistentTiming(bool consistent) { consistentTiming = consistent; }

    //this facilitates fine-tuning of complex animations
    void setSlowModeEnabled(bool enabled) { slowMode = enabled; }

    /*
        this is used for updating the currentTime of all animations in case the pause
        timer is active or, otherwise, only of the animation passed as parameter.
    */
    void ensureTimerUpdate();

    /*
        this will evaluate the need of restarting the pause timer in case there is still
        some pause animations running.
    */
    void restartAnimationTimer();

protected:
    void timerEvent(QTimerEvent *);

private:
    // timer used for all active (running) animations
    QBasicTimer animationTimer;
    // timer used to delay the check if we should start/stop the animation timer
    QBasicTimer startStopAnimationTimer;

    ElapsedTimer time;

    int lastTick;
    int timingInterval;
    int currentAnimationIdx;
    bool consistentTiming;
    bool slowMode;
    // bool to indicate that only pause animations are active
    bool isPauseTimerActive;

    QList<QAbstractAnimation*> animations, animationsToStart;

    // this is the count of running animations that are not a group neither a pause animation
    int runningLeafAnimations;
    QList<QAbstractAnimation*> runningPauseAnimations;

    void registerRunningAnimation(QAbstractAnimation *animation);
    void unregisterRunningAnimation(QAbstractAnimation *animation);

    void updateAnimationsTime();
    int closestPauseAnimationTimeToFinish();
};

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION

#endif //QABSTRACTANIMATION_P_H
