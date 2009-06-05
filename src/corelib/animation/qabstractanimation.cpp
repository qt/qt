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

/*!
    \class QAbstractAnimation
    \ingroup animation
    \brief The QAbstractAnimation class is the base of all animations.
    \since 4.6

    The class defines the functions for the functionality shared by
    all animations. By inheriting this class, you can create custom
    animations that plug into the rest of the animation framework.

    The progress of an animation is given by its current time
    (currentTime()), which is measured in milliseconds from the start
    of the animation (0) to its end (duration()). The value is updated
    automatically while the animation is running. It can also be set
    directly with setCurrentTime().

    At any point an animation is in one of three states:
    \l{QAbstractAnimation::}{Running},
    \l{QAbstractAnimation::}{Stopped}, or
    \l{QAbstractAnimation::}{Paused}--as defined by the
    \l{QAbstractAnimation::}{State} enum. The current state can be
    changed by calling start(), stop(), pause(), or resume(). An
    animation will always reset its \l{currentTime()}{current time}
    when it is started. If paused, it will continue with the same
    current time when resumed. When an animation is stopped, it cannot
    be resumed, but will keep its current time (until started again).
    QAbstractAnimation will emit stateChanged() whenever its state
    changes.

    An animation can loop any number of times by setting the loopCount
    property. When an animation's current time reaches its duration(),
    it will reset the current time and keep running. A loop count of 1
    (the default value) means that the animation will run one time.
    Note that a duration of -1 means that the animation will run until
    stopped; the current time will increase indefinitely. When the
    current time equals duration() and the animation is in its
    final loop, the \l{QAbstractAnimation::}{Stopped} state is
    entered, and the finished() signal is emitted.

    QAbstractAnimation provides pure virtual functions used by
    subclasses to track the progress of the animation: duration() and
    updateCurrentTime(). The duration() function lets you report a
    duration for the animation (as discussed above). The current time
    is delivered by the animation framework through calls to
    updateCurrentTime(). By reimplementing this function, you can
    track the animation progress. Note that neither the interval
    between calls nor the number of calls to this function are
    defined; though, it will normally be 60 updates per second.

    By reimplementing updateState(), you can track the animation's
    state changes, which is particularly useful for animations that
    are not driven by time.

    \sa QVariantAnimation, QPropertyAnimation, QAnimationGroup, {The Animation Framework}
*/

/*!
    \enum QAbstractAnimation::DeletionPolicy

    \value KeepWhenStopped The animation will not be deleted when stopped.
    \value DeleteWhenStopped The animation will be automatically deleted when
    stopped.
*/

/*!
    \fn QAbstractAnimation::finished()

    QAbstractAnimation emits this signal after the animation has stopped and
    has reached the end.

    This signal is emitted after stateChanged().

    \sa stateChanged()
*/

/*!
    \fn QAbstractAnimation::stateChanged(QAbstractAnimation::State oldState, QAbstractAnimation::State newState)

    QAbstractAnimation emits this signal whenever the state of the animation has
    changed from \a oldState to \a newState. This signal is emitted after the virtual
    updateState() function is called.

    \sa updateState()
*/

/*!
    \fn QAbstractAnimation::currentLoopChanged(int currentLoop)

    QAbstractAnimation emits this signal whenever the current loop
    changes. \a currentLoop is the current loop.

    \sa currentLoop(), loopCount()
*/

/*!
    \fn QAbstractAnimation::directionChanged(QAbstractAnimation::Direction newDirection);

    QAbstractAnimation emits this signal whenever the direction has been
    changed. \a newDirection is the new direction.

    \sa direction
*/

#include "qabstractanimation.h"
#include "qanimationgroup.h"
#include <QtCore/qdebug.h>

#include "qabstractanimation_p.h"

#include <QtCore/qmath.h>
#include <QtCore/qthreadstorage.h>
#include <QtCore/qcoreevent.h>
#include <QtCore/qpointer.h>

#ifndef QT_NO_ANIMATION

#define DEFAULT_TIMER_INTERVAL 16

QT_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(QThreadStorage<QUnifiedTimer *>, unifiedTimer)

QUnifiedTimer::QUnifiedTimer() : QObject(), lastTick(0), timingInterval(DEFAULT_TIMER_INTERVAL), consistentTiming(false)
{
}

QUnifiedTimer *QUnifiedTimer::instance()
{
    QUnifiedTimer *inst;
    if (!unifiedTimer()->hasLocalData()) {
        inst = new QUnifiedTimer;
        unifiedTimer()->setLocalData(inst);
    } else {
        inst = unifiedTimer()->localData();
    }
    return inst;
}

void QUnifiedTimer::updateRecentlyStartedAnimations()
{
    if (animationsToStart.isEmpty())
        return;

    animations += animationsToStart;
    updateTimer(); //we make sure we start the timer there

    animationsToStart.clear();
}

void QUnifiedTimer::timerEvent(QTimerEvent *event)
{
    //this is simply the time we last received a tick
    const int oldLastTick = lastTick;
    if (time.isValid())
        lastTick = consistentTiming ? oldLastTick + timingInterval : time.elapsed();

    //we transfer the waiting animations into the "really running" state
    updateRecentlyStartedAnimations();

    if (event->timerId() == startStopAnimationTimer.timerId()) {
        startStopAnimationTimer.stop();
        if (animations.isEmpty()) {
            animationTimer.stop();
            time = QTime();
        } else {
            animationTimer.start(timingInterval, this);
            lastTick = 0;
            time.start();
        }
    } else if (event->timerId() == animationTimer.timerId()) {
        const int delta = lastTick - oldLastTick;
        for (int i = 0; i < animations.count(); ++i) {
            QAbstractAnimation *animation = animations.at(i);
            int elapsed = QAbstractAnimationPrivate::get(animation)->totalCurrentTime
                + (animation->direction() == QAbstractAnimation::Forward ? delta : -delta);
            animation->setCurrentTime(elapsed);
        }
    }
}

void QUnifiedTimer::updateTimer()
{
    //we delay the call to start and stop for the animation timer so that if you
    //stop and start animations in batch you don't stop/start the timer too often.
    if (!startStopAnimationTimer.isActive())
        startStopAnimationTimer.start(0, this); // we delay the actual start of the animation
}

void QUnifiedTimer::registerAnimation(QAbstractAnimation *animation)
{
    if (animations.contains(animation) ||animationsToStart.contains(animation))
        return;
    animationsToStart << animation;
    updateTimer();
}

void QUnifiedTimer::unregisterAnimation(QAbstractAnimation *animation)
{
    animations.removeAll(animation);
    animationsToStart.removeAll(animation);
    updateTimer();
}


void QAbstractAnimationPrivate::setState(QAbstractAnimation::State newState)
{
    Q_Q(QAbstractAnimation);
    if (state == newState)
        return;

    QAbstractAnimation::State oldState = state;
    int oldCurrentTime = currentTime;
    int oldCurrentLoop = currentLoop;
    QAbstractAnimation::Direction oldDirection = direction;

    state = newState;

    QPointer<QAbstractAnimation> guard(q);

    guard->updateState(oldState, newState);

    //this is to be safe if updateState changes the state
    if (state == oldState)
        return;

    // Notify state change
    if (guard)
        emit guard->stateChanged(oldState, newState);

    // Enter running state.
    switch (state)
    {
    case QAbstractAnimation::Paused:
    case QAbstractAnimation::Running:
        {
            // Rewind
            if (oldState == QAbstractAnimation::Stopped) {
                if (guard) {
                    if (direction == QAbstractAnimation::Forward)
                        q->setCurrentTime(0);
                    else
                        q->setCurrentTime(loopCount == -1 ? q->duration() : q->totalDuration());
                }

                // Check if the setCurrentTime() function called stop().
                // This can happen for a 0-duration animation
                if (state == QAbstractAnimation::Stopped)
                    return;
            }

            // Register timer if our parent is not running.
            if (state == QAbstractAnimation::Running && guard) {
                if (!group || group->state() == QAbstractAnimation::Stopped) {
                    QUnifiedTimer::instance()->registerAnimation(q);
                }
            } else {
                //new state is paused
                QUnifiedTimer::instance()->unregisterAnimation(q);
            }
        }
        break;
    case QAbstractAnimation::Stopped:
        // Leave running state.
        int dura = q->duration();
        if (deleteWhenStopped && guard)
            q->deleteLater();

        QUnifiedTimer::instance()->unregisterAnimation(q);

        if (dura == -1 || loopCount < 0
            || (oldDirection == QAbstractAnimation::Forward && (oldCurrentTime * (oldCurrentLoop + 1)) == (dura * loopCount))
            || (oldDirection == QAbstractAnimation::Backward && oldCurrentTime == 0)) {
                if (guard)
                    emit q->finished();
        }
        break;
    }

}

/*!
    Constructs the QAbstractAnimation base class, and passes \a parent to
    QObject's constructor.

    \sa QVariantAnimation, QAnimationGroup
*/
QAbstractAnimation::QAbstractAnimation(QObject *parent)
    : QObject(*new QAbstractAnimationPrivate, 0)
{
    // Allow auto-add on reparent
    setParent(parent);
}

/*!
    \internal
*/
QAbstractAnimation::QAbstractAnimation(QAbstractAnimationPrivate &dd, QObject *parent)
    : QObject(dd, 0)
{
    // Allow auto-add on reparent
   setParent(parent);
}

/*!
    Stops the animation if it's running, then destroys the
    QAbstractAnimation. If the animation is part of a QAnimationGroup, it is
    automatically removed before it's destroyed.
*/
QAbstractAnimation::~QAbstractAnimation()
{
    Q_D(QAbstractAnimation);
    //we can't call stop here. Otherwise we get pure virtual calls
    if (d->state != Stopped) {
        QAbstractAnimation::State oldState = d->state;
        d->state = Stopped;
        emit stateChanged(oldState, d->state);
        QUnifiedTimer::instance()->unregisterAnimation(this);
    }
}

/*!
    \property QAbstractAnimation::state
    \brief state of the animation.

    This property describes the current state of the animation. When the
    animation state changes, QAbstractAnimation emits the stateChanged()
    signal.
*/
QAbstractAnimation::State QAbstractAnimation::state() const
{
    Q_D(const QAbstractAnimation);
    return d->state;
}

/*!
    If this animation is part of a QAnimationGroup, this function returns a
    pointer to the group; otherwise, it returns 0.

    \sa QAnimationGroup::addAnimation()
*/
QAnimationGroup *QAbstractAnimation::group() const
{
    Q_D(const QAbstractAnimation);
    return d->group;
}

/*!
    \enum QAbstractAnimation::State

    This enum describes the state of the animation.

    \value Stopped The animation is not running. This is the initial state
    of QAbstractAnimation, and the state QAbstractAnimation reenters when finished. The current
    time remain unchanged until either setCurrentTime() is
    called, or the animation is started by calling start().

    \value Paused The animation is paused (i.e., temporarily
    suspended). Calling resume() will resume animation activity.

    \value Running The animation is running. While control is in the event
    loop, QAbstractAnimation will update its current time at regular intervals,
    calling updateCurrentTime() when appropriate.

    \sa state(), stateChanged()
*/

/*!
    \enum QAbstractAnimation::Direction

    This enum describes the direction of the animation when in \l Running state.

    \value Forward The current time of the animation increases with time (i.e.,
    moves from 0 and towards the end / duration).

    \value Backward The current time of the animation decreases with time (i.e.,
    moves from the end / duration and towards 0).

    \sa direction
*/

/*!
    \property QAbstractAnimation::direction
    \brief the direction of the animation when it is in \l Running
    state.

    This direction indicates whether the time moves from 0 towards the
    animation duration, or from the value of the duration and towards 0 after
    start() has been called.

    By default, this property is set to \l Forward.
*/
QAbstractAnimation::Direction QAbstractAnimation::direction() const
{
    Q_D(const QAbstractAnimation);
    return d->direction;
}
void QAbstractAnimation::setDirection(Direction direction)
{
    Q_D(QAbstractAnimation);
    if (d->direction == direction)
        return;

    d->direction = direction;
    if (state() == Stopped) {
        if (direction == Backward) {
            d->currentTime = duration();
            d->currentLoop = d->loopCount - 1;
        } else {
            d->currentTime = 0;
            d->currentLoop = 0;
        }
    }
    updateDirection(direction);
    emit directionChanged(direction);
}

/*!
    \property QAbstractAnimation::duration
    \brief the duration of the animation.

    If the duration is -1, it means that the duration is undefined.
    In this case, loopCount is ignored.
*/

/*!
    \property QAbstractAnimation::loopCount
    \brief the loop count of the animation

    This property describes the loop count of the animation as an integer.
    By default this value is 1, indicating that the animation
    should run once only, and then stop. By changing it you can let the
    animation loop several times. With a value of 0, the animation will not
    run at all, and with a value of -1, the animation will loop forever
    until stopped.
    It is not supported to have loop on an animation that has an undefined
    duration. It will only run once.
*/
int QAbstractAnimation::loopCount() const
{
    Q_D(const QAbstractAnimation);
    return d->loopCount;
}
void QAbstractAnimation::setLoopCount(int loopCount)
{
    Q_D(QAbstractAnimation);
    d->loopCount = loopCount;
}

/*!
    \property QAbstractAnimation::currentLoop
    \brief the current loop of the animation

    This property describes the current loop of the animation. By default,
    the animation's loop count is 1, and so the current loop will
    always be 0. If the loop count is 2 and the animation runs past its
    duration, it will automatically rewind and restart at current time 0, and
    current loop 1, and so on.

    When the current loop changes, QAbstractAnimation emits the
    currentLoopChanged() signal.
*/
int QAbstractAnimation::currentLoop() const
{
    Q_D(const QAbstractAnimation);
    return d->currentLoop;
}

/*!
    \fn virtual int QAbstractAnimation::duration() const = 0

    This pure virtual function returns the duration of the animation, and
    defines for how long QAbstractAnimation should update the current
    time. This duration is local, and does not include the loop count.

    A return value of -1 indicates that the animation has no defined duration;
    the animation should run forever until stopped. This is useful for
    animations that are not time driven, or where you cannot easily predict
    its duration (e.g., event driven audio playback in a game).

    If the animation is a parallel QAnimationGroup, the duration will be the longest
    duration of all its animations. If the animation is a sequential QAnimationGroup,
    the duration will be the sum of the duration of all its animations.
    \sa loopCount
*/

/*!
    Returns the total and effective duration of the animation, including the
    loop count.

    \sa duration(), currentTime
*/
int QAbstractAnimation::totalDuration() const
{
    Q_D(const QAbstractAnimation);
    if (d->loopCount < 0)
        return -1;
    int dura = duration();
    if (dura == -1)
        return -1;
    return dura * d->loopCount;
}

/*!
    \property QAbstractAnimation::currentTime
    \brief the current time and progress of the animation

    This property describes the animation's current time. You can change the
    current time by calling setCurrentTime, or you can call start() and let
    the animation run, setting the current time automatically as the animation
    progresses.

    The animation's current time starts at 0, and ends at duration(). If the
    animation's loopCount is larger than 1, the current time will rewind and
    start at 0 again for the consecutive loops. If the animation has a pause.
    currentTime will also include the duration of the pause.

    \sa loopCount
 */
int QAbstractAnimation::currentTime() const
{
    Q_D(const QAbstractAnimation);
    return d->currentTime;
}
void QAbstractAnimation::setCurrentTime(int msecs)
{
    Q_D(QAbstractAnimation);
    msecs = qMax(msecs, 0);

    // Calculate new time and loop.
    int dura = duration();
    int totalDura = (d->loopCount < 0 || dura == -1) ? -1 : dura * d->loopCount;
    if (totalDura != -1)
        msecs = qMin(totalDura, msecs);
    d->totalCurrentTime = msecs;

    // Update new values.
    int oldLoop = d->currentLoop;
    d->currentLoop = ((dura <= 0) ? 0 : (msecs / dura));
    if (d->currentLoop == d->loopCount) {
        //we're at the end
        d->currentTime = qMax(0, dura);
        d->currentLoop = qMax(0, d->loopCount - 1);
    } else {
        if (d->direction == Forward) {
            d->currentTime = (dura <= 0) ? msecs : (msecs % dura);
        } else {
            d->currentTime = (dura <= 0) ? msecs : ((msecs - 1) % dura) + 1;
            if (d->currentTime == dura)
                --d->currentLoop;
        }
    }

    updateCurrentTime(msecs);
    if (d->currentLoop != oldLoop)
        emit currentLoopChanged(d->currentLoop);

    // All animations are responsible for stopping the animation when their
    // own end state is reached; in this case the animation is time driven,
    // and has reached the end.
    if ((d->direction == Forward && d->totalCurrentTime == totalDura)
        || (d->direction == Backward && d->totalCurrentTime == 0)) {
        stop();
    }
}

/*!
    Starts the animation. The \a policy argument says whether or not the
    animation should be deleted when it's done. When the animation starts, the
    stateChanged() signal is emitted, and state() returns Running. When control
    reaches the event loop, the animation will run by itself, periodically
    calling updateCurrentTime() as the animation progresses.

    If the animation is currently stopped or has already reached the end,
    calling start() will rewind the animation and start again from the beginning.
    When the animation reaches the end, the animation will either stop, or
    if the loop level is more than 1, it will rewind and continue from the beginning.

    If the animation is already running, this function does nothing.

    \sa stop(), state()
*/
void QAbstractAnimation::start(DeletionPolicy policy)
{
    Q_D(QAbstractAnimation);
    if (d->state == Running)
        return;
    d->setState(Running);
    d->deleteWhenStopped = policy;
}

/*!
    Stops the animation. When the animation is stopped, it emits the stateChanged()
    signal, and state() returns Stopped. The current time is not changed.

    If the animation stops by itself after reaching the end (i.e.,
    currentTime() == duration() and currentLoop() > loopCount() - 1), the
    finished() signal is emitted.

    \sa start(), state()
 */
void QAbstractAnimation::stop()
{
    Q_D(QAbstractAnimation);

    d->setState(Stopped);
}

/*!
    Pauses the animation. When the animation is paused, state() returns Paused.
    The currenttime will remain unchanged until resume() or start() is called.
    If you want to continue from the current time, call resume().


    \sa start(), state(), resume()
 */
void QAbstractAnimation::pause()
{
    Q_D(QAbstractAnimation);
    if (d->state == Stopped) {
        qWarning("QAbstractAnimation::pause: Cannot pause a stopped animation");
        return;
    }

    d->setState(Paused);
}

/*!
    Resumes the animation after it was paused. When the animation is resumed,
    it emits the resumed() and stateChanged() signals. The currenttime is not
    changed.

    \sa start(), pause(), state()
 */
void QAbstractAnimation::resume()
{
    Q_D(QAbstractAnimation);
    if (d->state != Paused) {
        qWarning("QAbstractAnimation::resume: "
                 "Cannot resume an animation that is not paused");
        return;
    }

    d->setState(Running);
}

/*!
    \reimp
*/
bool QAbstractAnimation::event(QEvent *event)
{
    return QObject::event(event);
}

/*!
    \fn virtual void QAbstractAnimation::updateCurrentTime(int msecs) = 0;

    This pure virtual function is called every time the animation's current
    time changes. The \a msecs argument is the current time.

    \sa updateState()
*/

/*!
    This virtual function is called by QAbstractAnimation when the state
    of the animation is changed from \a oldState to \a newState.

    \sa start(), stop(), pause(), resume()
*/
void QAbstractAnimation::updateState(QAbstractAnimation::State oldState,
                                     QAbstractAnimation::State newState)
{
    Q_UNUSED(oldState);
    Q_UNUSED(newState);
}

/*!
    This virtual function is called by QAbstractAnimation when the direction
    of the animation is changed. The \a direction argument is the new direction.

    \sa setDirection(), direction()
*/
void QAbstractAnimation::updateDirection(QAbstractAnimation::Direction direction)
{
    Q_UNUSED(direction);
}


QT_END_NAMESPACE

#include "moc_qabstractanimation.cpp"

#endif //QT_NO_ANIMATION
