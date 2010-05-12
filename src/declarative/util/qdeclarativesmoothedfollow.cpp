/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativesmoothedfollow_p.h"
#include "qdeclarativesmoothedanimation_p_p.h"

#include <private/qobject_p.h>
#include <QtCore/qnumeric.h>

#include "qdeclarativeglobal_p.h"


QT_BEGIN_NAMESPACE

class QDeclarativeSmoothedFollowPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeSmoothedFollow)
public:
    QDeclarativeSmoothedFollowPrivate();

    bool enabled;
    QSmoothedAnimation *anim;
};

/*!
    \qmlclass SmoothedFollow QDeclarativeSmoothedFollow
    \since 4.7
    \inherits NumberAnimation
    \brief The SmoothedFollow element allows a property to smoothly track a value.

    The SmoothedFollow animates a property's value to a set target value
    using an ease in/out quad easing curve.  If the animation is restarted
    with a different target value, the easing curves used to animate to the old
    and the new target values are smoothly spliced together to avoid any obvious
    visual glitches by maintaining the current velocity.

    The property animation is configured by setting the velocity at which the
    animation should occur, or the duration that the animation should take.
    If both a velocity and a duration are specified, the one that results in
    the quickest animation is chosen for each change in the target value.

    For example, animating from 0 to 800 will take 4 seconds if a velocity
    of 200 is set, will take 8 seconds with a duration of 8000 set, and will
    take 4 seconds with both a velocity of 200 and a duration of 8000 set.
    Animating from 0 to 20000 will take 10 seconds if a velocity of 200 is set,
    will take 8 seconds with a duration of 8000 set, and will take 8 seconds
    with both a velocity of 200 and a duration of 8000 set.

    The follow example shows one rectangle tracking the position of another.
\code
import Qt 4.7

Rectangle {
    width: 800; height: 600; color: "blue"

    Rectangle {
        color: "green"
        width: 60; height: 60;
        SmoothedFollow on x { to: rect1.x - 5; velocity: 200 }
        SmoothedFollow on y { to: rect1.y - 5; velocity: 200 }
    }

    Rectangle {
        id: rect1
        color: "red"
        width: 50; height: 50;
    }

    focus: true
    Keys.onRightPressed: rect1.x = rect1.x + 100
    Keys.onLeftPressed: rect1.x = rect1.x - 100
    Keys.onUpPressed: rect1.y = rect1.y - 100
    Keys.onDownPressed: rect1.y = rect1.y + 100
}
\endcode

    The default velocity of SmoothedFollow is 200 units/second.  Note that if the range of the
    value being animated is small, then the velocity will need to be adjusted
    appropriately.  For example, the opacity of an item ranges from 0 - 1.0.
    To enable a smooth animation in this range the velocity will need to be
    set to a value such as 0.5 units/second.  Animating from 0 to 1.0 with a velocity
    of 0.5 will take 2000 ms to complete.

    \sa SpringFollow
*/

QDeclarativeSmoothedFollow::QDeclarativeSmoothedFollow(QObject *parent)
    : QObject(*(new QDeclarativeSmoothedFollowPrivate), parent)
{
}

QDeclarativeSmoothedFollow::~QDeclarativeSmoothedFollow()
{
}

QDeclarativeSmoothedFollowPrivate::QDeclarativeSmoothedFollowPrivate()
    : enabled(true), anim(new QSmoothedAnimation)
{
    Q_Q(QDeclarativeSmoothedFollow);
    QDeclarative_setParent_noEvent(anim, q);
}

/*!
    \qmlproperty enumeration SmoothedFollow::reversingMode

    Sets how the SmoothedFollow behaves if an animation direction is reversed.

    If reversing mode is \c SmoothedFollow.Eased, the animation will smoothly decelerate, and
    then reverse direction.  If the reversing mode is \c SmoothedFollow.Immediate, the
    animation will immediately begin accelerating in the reverse direction,
    begining with a velocity of 0.  If the reversing mode is \c SmoothedFollow.Sync, the
    property is immediately set to the target value.
*/
QDeclarativeSmoothedFollow::ReversingMode QDeclarativeSmoothedFollow::reversingMode() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return (ReversingMode) d->anim->reversingMode;
}

void QDeclarativeSmoothedFollow::setReversingMode(ReversingMode m)
{
    Q_D(QDeclarativeSmoothedFollow);
    if (d->anim->reversingMode == (QDeclarativeSmoothedAnimation::ReversingMode) m)
        return;

    d->anim->reversingMode = (QDeclarativeSmoothedAnimation::ReversingMode) m;
    emit reversingModeChanged();
}

/*!
    \qmlproperty int SmoothedFollow::duration

    This property holds the animation duration, in msecs, used when tracking the source.

    Setting this to -1 (the default) disables the duration value.
*/
int QDeclarativeSmoothedFollow::duration() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return d->anim->userDuration;
}

void QDeclarativeSmoothedFollow::setDuration(int duration)
{
    Q_D(QDeclarativeSmoothedFollow);
    if (duration == d->anim->duration())
        return;

    d->anim->userDuration = duration;
    emit durationChanged();
}

qreal QDeclarativeSmoothedFollow::velocity() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return d->anim->velocity;
}

/*!
    \qmlproperty real SmoothedFollow::velocity

    This property holds the average velocity allowed when tracking the 'to' value.

    The default velocity of SmoothedFollow is 200 units/second.

    Setting this to -1 disables the velocity value.
*/
void QDeclarativeSmoothedFollow::setVelocity(qreal v)
{
    Q_D(QDeclarativeSmoothedFollow);
    if (d->anim->velocity == v)
        return;

    d->anim->velocity = v;
    emit velocityChanged();
}

/*!
    \qmlproperty int SmoothedFollow::maximumEasingTime

    This property specifies the maximum time, in msecs, an "eases" during the follow should take.
    Setting this property causes the velocity to "level out" after at a time.  Setting
    a negative value reverts to the normal mode of easing over the entire animation
    duration.

    The default value is -1.
*/
int QDeclarativeSmoothedFollow::maximumEasingTime() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return d->anim->maximumEasingTime;
}

void QDeclarativeSmoothedFollow::setMaximumEasingTime(int v)
{
    Q_D(QDeclarativeSmoothedFollow);
    d->anim->maximumEasingTime = v;
    emit maximumEasingTimeChanged();
}

/*!
    \qmlproperty real SmoothedFollow::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition or Behavior.
*/
qreal QDeclarativeSmoothedFollow::to() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return d->anim->to;
}

void QDeclarativeSmoothedFollow::setTo(qreal t)
{
    Q_D(QDeclarativeSmoothedFollow);

    if (qIsNaN(t))
        return;

    if (d->anim->to == t)
        return;

    d->anim->to = t;

    if (d->enabled)
        d->anim->restart();
}

/*!
    \qmlproperty bool SmoothedFollow::enabled
    This property whether this animation should automatically restart when
    the 'to' property is upated.

    The default value of this property is 'true'.
*/
bool QDeclarativeSmoothedFollow::enabled() const
{
    Q_D(const QDeclarativeSmoothedFollow);
    return d->enabled;
}

void QDeclarativeSmoothedFollow::setEnabled(bool e)
{
    Q_D(QDeclarativeSmoothedFollow);
    if (d->enabled == e)
        return;
    d->enabled = e;

    if (d->enabled)
        d->anim->restart();
    else
        d->anim->stop();
    emit enabledChanged();
}

void QDeclarativeSmoothedFollow::setTarget(const QDeclarativeProperty &t)
{
    Q_D(QDeclarativeSmoothedFollow);
    d->anim->target = t;
}

QT_END_NAMESPACE
