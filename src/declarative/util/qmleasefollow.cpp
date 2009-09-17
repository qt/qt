/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmleasefollow.h"
#include <QtDeclarative/qmlmetaproperty.h>
#include <math.h>
#include <QtCore/qdebug.h>
#include <private/qmlanimation_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,EaseFollow,QmlEaseFollow);

class QmlEaseFollowPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlEaseFollow)
public:
    QmlEaseFollowPrivate()
        : source(0), velocity(200), duration(-1), 
          reversingMode(QmlEaseFollow::Eased), initialVelocity(0), 
          initialValue(0), invert(false), enabled(true), trackVelocity(0), clockOffset(0),
          lastTick(0), clock(this)
    {} 

    qreal source;
    qreal velocity;
    qreal duration;
    QmlEaseFollow::ReversingMode reversingMode;

    qreal initialVelocity;
    qreal initialValue;
    bool invert;
    bool enabled;

    qreal trackVelocity;

    QmlMetaProperty target;

    int clockOffset;
    int lastTick;
    void tick(int);
    void clockStart();
    void clockStop();
    QTickAnimationProxy<QmlEaseFollowPrivate, &QmlEaseFollowPrivate::tick> clock;

    void restart();

    // Parameters for use in tick()
    qreal a;  // Acceleration
    qreal tf; // Total time
    qreal tp; // Time at which peak velocity occurs
    qreal vp; // Velocity at tp
    qreal sp; // Displacement at tp
    qreal vi; // "Normalized" initialvelocity
    bool recalc();
};

bool QmlEaseFollowPrivate::recalc()
{
    qreal s = source - initialValue;
    vi = initialVelocity;

    s = (invert?-1.0:1.0) * s;
    vi = (invert?-1.0:1.0) * vi;

    if (duration > 0 && velocity > 0) {
        tf = s / velocity;
        if (tf > (duration / 1000.)) tf = (duration / 1000.);
    } else if (duration > 0) {
        tf = duration / 1000.;
    } else if (velocity > 0) {
        tf = s / velocity;
    } else {
        return false;
    }

    qreal c1 = 0.25 * tf * tf;
    qreal c2 = 0.5 * vi * tf - s;
    qreal c3 = -0.25 * vi * vi;

    qreal a1 = (-c2 + sqrt(c2 * c2 - 4 * c1 * c3)) / (2. * c1);
    // qreal a2 = (-c2 - sqrt(c2 * c2 - 4 * c1 * c3)) / (2. * c1);

    qreal tp1 = 0.5 * tf - 0.5 * vi / a1;
    // qreal tp2 = 0.5 * tf - 0.5 * vi / a2;
    qreal vp1 = a1 * tp1 + vi;
    // qreal vp2 = a2 * tp2 + vi;

    qreal sp1 = 0.5 * a1 * tp1 * tp1 + vi * tp1;
    // qreal sp2 = 0.5 * a2 * tp2 * tp2 + vi * tp2;

    a = a1;
    tp = tp1;
    vp = vp1;
    sp = sp1;

    return true;
}

void QmlEaseFollowPrivate::clockStart()
{
    if (clock.state() == QAbstractAnimation::Running) {
        clockOffset = lastTick;
        return;
    } else {
        clockOffset = 0;
        lastTick = 0;
        clock.start();
    }
}

void QmlEaseFollowPrivate::clockStop()
{
    clockOffset = 0;
    lastTick = 0;
    clock.stop();
}

void QmlEaseFollowPrivate::tick(int t)
{
    lastTick = t;
    t -= clockOffset;

    qreal time_seconds = qreal(t) / 1000.;

    if (time_seconds < tp) {

        trackVelocity = vi + time_seconds * a;
        trackVelocity = (invert?-1.0:1.0) * trackVelocity;

        qreal value = 0.5 * a * time_seconds * time_seconds + vi * time_seconds;
        value = (invert?-1.0:1.0) * value;
        target.write(initialValue + value);

    } else if (time_seconds < tf) {

        time_seconds -= tp;

        trackVelocity = vp - time_seconds * a;
        trackVelocity = (invert?-1.0:1.0) * trackVelocity;

        qreal value = 0.5 * a * tp * tp + vi * tp 
                    - 0.5 * a * time_seconds * time_seconds + vp * time_seconds;
        value = (invert?-1.0:1.0) * value;

        target.write(initialValue + value);

    } else {

        clock.stop();

        trackVelocity = 0;
        target.write(source);
    }
}

/*!
    \qmlclass EaseFollow QmlEaseFollow
    \brief The EaseFollow element allows a property to smoothly track a value.

    The EaseFollow smoothly animates a property's value to a set target value 
    using an ease in/out quad easing curve.  If the target value changes while
    the animation is in progress, the easing curves used to animate to the old 
    and the new target values are spliced together to avoid any obvious visual
    glitches.

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
import Qt 4.6

Rectangle {
    width: 800; height: 600; color: "blue"

    Rectangle {
        color: "green"
        width: 60; height: 60;
        x: -5; y: -5;
        x: EaseFollow { source: Rect1.x - 5; velocity: 200 }
        y: EaseFollow { source: Rect1.y - 5; velocity: 200 }
    }

    Rectangle {
        id: Rect1
        color: "red"
        width: 50; height: 50;
    }

    focus: true
    Keys.onRightPressed: Rect1.x = Rect1.x + 100
    Keys.onLeftPressed: Rect1.x = Rect1.x - 100
    Keys.onUpPressed: Rect1.y = Rect1.y - 100
    Keys.onDownPressed: Rect1.y = Rect1.y + 100
}
\endcode

    \sa SpringFollow
*/

QmlEaseFollow::QmlEaseFollow(QObject *parent)
: QObject(*(new QmlEaseFollowPrivate), parent)
{
}

QmlEaseFollow::~QmlEaseFollow()
{
}

/*!
    \qmlproperty qreal EaseFollow::source
    This property holds the source value which will be tracked.

    Bind to a property in order to track its changes.
*/
qreal QmlEaseFollow::sourceValue() const
{
    Q_D(const QmlEaseFollow);
    return d->source;
}

/*!
    \qmlproperty enumeration EaseFollow::reversingMode

    Sets how the EaseFollow behaves if an animation direction is reversed.

    If reversing mode is \c Eased, the animation will smoothly decelerate, and
    then reverse direction.  If the reversing mode is \c Immediate, the 
    animation will immediately begin accelerating in the reverse direction, 
    begining with a velocity of 0.  If the reversing mode is \c Sync, the
    property is immediately set to the target value.
*/
QmlEaseFollow::ReversingMode QmlEaseFollow::reversingMode() const
{
    Q_D(const QmlEaseFollow);
    return d->reversingMode;
}

void QmlEaseFollow::setReversingMode(ReversingMode m)
{
    Q_D(QmlEaseFollow);
    d->reversingMode = m;
}

void QmlEaseFollowPrivate::restart()
{
    if (!enabled)
        return;

    initialValue = target.read().toReal();

    if (source == initialValue) {
        clockStop();
        return;
    }

    bool hasReversed = trackVelocity != 0. && 
                      ((trackVelocity > 0) == ((initialValue - source) > 0));

    if (hasReversed) {
        switch (reversingMode) {
            default:
            case QmlEaseFollow::Eased:
                break;
            case QmlEaseFollow::Sync:
                target.write(source);
                return;
            case QmlEaseFollow::Immediate:
                initialVelocity = 0;
                clockStop();
                break;
        }
    }

    trackVelocity = initialVelocity;

    invert = (source < initialValue);

    if (!recalc()) {
        target.write(source);
        clockStop();
        return;
    }

    clockStart();
}

void QmlEaseFollow::setSourceValue(qreal s)
{
    Q_D(QmlEaseFollow);

    d->source = s;
    d->initialVelocity = d->trackVelocity;
    d->restart();
}

/*!
    \qmlproperty qreal EaseFollow::duration

    This property holds the animation duration used when tracking the source.

    Setting this to -1 disables the duration value.
*/
qreal QmlEaseFollow::duration() const
{
    Q_D(const QmlEaseFollow);
    return d->duration;
}

void QmlEaseFollow::setDuration(qreal v)
{
    Q_D(QmlEaseFollow);
    d->duration = v;
    d->trackVelocity = 0;

    if (d->clock.state() == QAbstractAnimation::Running) 
        d->restart();
}

qreal QmlEaseFollow::velocity() const
{
    Q_D(const QmlEaseFollow);
    return d->velocity;
}

/*!
    \qmlproperty qreal EaseFollow::velocity

    This property holds the average velocity allowed when tracking the source.

    Setting this to -1 disables the velocity value.
*/
void QmlEaseFollow::setVelocity(qreal v)
{
    Q_D(QmlEaseFollow);
    d->velocity = v;
    d->trackVelocity = 0;

    if (d->clock.state() == QAbstractAnimation::Running) 
        d->restart();
}

/*!
    \qmlproperty bool EaseFollow::enabled
    This property holds whether the target will track the source.
*/
bool QmlEaseFollow::enabled() const
{
    Q_D(const QmlEaseFollow);
    return d->enabled;
}

void QmlEaseFollow::setEnabled(bool enabled)
{
    Q_D(QmlEaseFollow);
    d->enabled = enabled;
    if (enabled)
        d->restart();
    else
        d->clockStop();
}

void QmlEaseFollow::setTarget(const QmlMetaProperty &t)
{
    Q_D(QmlEaseFollow);
    d->target = t;
}

QT_END_NAMESPACE
