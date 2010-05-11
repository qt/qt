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

#include "private/qdeclarativespringfollow_p.h"

#include "private/qdeclarativeanimation_p_p.h"

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

#include <limits.h>
#include <math.h>

QT_BEGIN_NAMESPACE



class QDeclarativeSpringFollowPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeSpringFollow)
public:
    QDeclarativeSpringFollowPrivate()
        : currentValue(0), to(0), maxVelocity(0), lastTime(0)
        , mass(1.0), spring(0.), damping(0.), velocity(0), epsilon(0.01)
        , modulus(0.0), useMass(false), haveModulus(false), enabled(true), mode(Track), clock(this) {}

    QDeclarativeProperty property;
    qreal currentValue;
    qreal to;
    qreal maxVelocity;
    qreal velocityms;
    int lastTime;
    qreal mass;
    qreal spring;
    qreal damping;
    qreal velocity;
    qreal epsilon;
    qreal modulus;

    bool useMass : 1;
    bool haveModulus : 1;
    bool enabled : 1;

    enum Mode {
        Track,
        Velocity,
        Spring
    };
    Mode mode;

    void tick(int);
    void updateMode();
    void start();
    void stop();

    QTickAnimationProxy<QDeclarativeSpringFollowPrivate, &QDeclarativeSpringFollowPrivate::tick> clock;
};

void QDeclarativeSpringFollowPrivate::tick(int time)
{
    Q_Q(QDeclarativeSpringFollow);

    int elapsed = time - lastTime;
    if (!elapsed)
        return;
    qreal srcVal = to;
    if (haveModulus) {
        currentValue = fmod(currentValue, modulus);
        srcVal = fmod(srcVal, modulus);
    }
    if (mode == Spring) {
        if (elapsed < 16) // capped at 62fps.
            return;
        // Real men solve the spring DEs using RK4.
        // We'll do something much simpler which gives a result that looks fine.
        int count = elapsed / 16;
        for (int i = 0; i < count; ++i) {
            qreal diff = srcVal - currentValue;
            if (haveModulus && qAbs(diff) > modulus / 2) {
                if (diff < 0)
                    diff += modulus;
                else
                    diff -= modulus;
            }
            if (useMass)
                velocity = velocity + (spring * diff - damping * velocity) / mass;
            else
                velocity = velocity + spring * diff - damping * velocity;
            if (maxVelocity > 0.) {
                // limit velocity
                if (velocity > maxVelocity)
                    velocity = maxVelocity;
                else if (velocity < -maxVelocity)
                    velocity = -maxVelocity;
            }
            currentValue += velocity * 16.0 / 1000.0;
            if (haveModulus) {
                currentValue = fmod(currentValue, modulus);
                if (currentValue < 0.0)
                    currentValue += modulus;
            }
        }
        if (qAbs(velocity) < epsilon && qAbs(srcVal - currentValue) < epsilon) {
            velocity = 0.0;
            currentValue = srcVal;
            clock.stop();
        }
        lastTime = time - (elapsed - count * 16);
    } else {
        qreal moveBy = elapsed * velocityms;
        qreal diff = srcVal - currentValue;
        if (haveModulus && qAbs(diff) > modulus / 2) {
            if (diff < 0)
                diff += modulus;
            else
                diff -= modulus;
        }
        if (diff > 0) {
            currentValue += moveBy;
            if (haveModulus)
                currentValue = fmod(currentValue, modulus);
            if (currentValue > to) {
                currentValue = to;
                clock.stop();
            }
        } else {
            currentValue -= moveBy;
            if (haveModulus && currentValue < 0.0)
                currentValue = fmod(currentValue, modulus) + modulus;
            if (currentValue < to) {
                currentValue = to;
                clock.stop();
            }
        }
        lastTime = time;
    }
    property.write(currentValue);
    emit q->valueChanged(currentValue);
    if (clock.state() != QAbstractAnimation::Running)
        emit q->syncChanged();
}

void QDeclarativeSpringFollowPrivate::updateMode()
{
    if (spring == 0. && maxVelocity == 0.)
        mode = Track;
    else if (spring > 0.)
        mode = Spring;
    else
        mode = Velocity;
}

void QDeclarativeSpringFollowPrivate::start()
{
    if (!enabled)
        return;

    Q_Q(QDeclarativeSpringFollow);
    if (mode == QDeclarativeSpringFollowPrivate::Track) {
        currentValue = to;
        property.write(currentValue);
    } else if (to != currentValue && clock.state() != QAbstractAnimation::Running) {
        lastTime = 0;
        currentValue = property.read().toReal();
        clock.start(); // infinity??
        emit q->syncChanged();
    }
}

void QDeclarativeSpringFollowPrivate::stop()
{
    clock.stop();
}

/*!
    \qmlclass SpringFollow QDeclarativeSpringFollow
    \since 4.7
    \brief The SpringFollow element allows a property to track a value.

    In example below, \e rect2 will follow \e rect1 moving with a velocity of up to 200:
    \code
    Rectangle {
        id: rect1
        width: 20; height: 20
        color: "#00ff00"
        y: 200  // initial value
        SequentialAnimation on y {
            loops: Animation.Infinite
            NumberAnimation {
                to: 200
                easing.type: Easing.OutBounce
                easing.amplitude: 100
                duration: 2000
            }
            PauseAnimation { duration: 1000 }
        }
    }
    Rectangle {
        id: rect2
        x: rect1.width
        width: 20; height: 20
        color: "#ff0000"
        SpringFollow on y { to: rect1.y; velocity: 200 }
    }
    \endcode
*/

QDeclarativeSpringFollow::QDeclarativeSpringFollow(QObject *parent)
: QObject(*(new QDeclarativeSpringFollowPrivate),parent)
{
}

QDeclarativeSpringFollow::~QDeclarativeSpringFollow()
{
}

void QDeclarativeSpringFollow::setTarget(const QDeclarativeProperty &property)
{
    Q_D(QDeclarativeSpringFollow);
    d->property = property;
    d->currentValue = property.read().toReal();
}

qreal QDeclarativeSpringFollow::to() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->to;
}

/*!
    \qmlproperty real SpringFollow::to
    This property holds the target value which will be tracked.

    Bind to a property in order to track its changes.
*/

void QDeclarativeSpringFollow::setTo(qreal value)
{
    Q_D(QDeclarativeSpringFollow);
    if (d->clock.state() == QAbstractAnimation::Running && d->to == value)
        return;

    d->to = value;
    d->start();
}

/*!
    \qmlproperty real SpringFollow::velocity
    This property holds the maximum velocity allowed when tracking the source.
*/

qreal QDeclarativeSpringFollow::velocity() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->maxVelocity;
}

void QDeclarativeSpringFollow::setVelocity(qreal velocity)
{
    Q_D(QDeclarativeSpringFollow);
    d->maxVelocity = velocity;
    d->velocityms = velocity / 1000.0;
    d->updateMode();
}

/*!
    \qmlproperty real SpringFollow::spring
    This property holds the spring constant

    The spring constant describes how strongly the target is pulled towards the
    source. Setting spring to 0 turns off spring tracking.  Useful values 0 - 5.0

    When a spring constant is set and the velocity property is greater than 0,
    velocity limits the maximum speed.
*/
qreal QDeclarativeSpringFollow::spring() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->spring;
}

void QDeclarativeSpringFollow::setSpring(qreal spring)
{
    Q_D(QDeclarativeSpringFollow);
    d->spring = spring;
    d->updateMode();
}

/*!
    \qmlproperty real SpringFollow::damping
    This property holds the spring damping constant

    The damping constant describes how quickly a sprung follower comes to rest.
    Useful range is 0 - 1.0
*/
qreal QDeclarativeSpringFollow::damping() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->damping;
}

void QDeclarativeSpringFollow::setDamping(qreal damping)
{
    Q_D(QDeclarativeSpringFollow);
    if (damping > 1.)
        damping = 1.;

    d->damping = damping;
}


/*!
    \qmlproperty real SpringFollow::epsilon
    This property holds the spring epsilon

    The epsilon is the rate and amount of change in the value which is close enough
    to 0 to be considered equal to zero. This will depend on the usage of the value.
    For pixel positions, 0.25 would suffice. For scale, 0.005 will suffice.

    The default is 0.01. Tuning this value can provide small performance improvements.
*/
qreal QDeclarativeSpringFollow::epsilon() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->epsilon;
}

void QDeclarativeSpringFollow::setEpsilon(qreal epsilon)
{
    Q_D(QDeclarativeSpringFollow);
    d->epsilon = epsilon;
}

/*!
    \qmlproperty real SpringFollow::modulus
    This property holds the modulus value.

    Setting a \a modulus forces the target value to "wrap around" at the modulus.
    For example, setting the modulus to 360 will cause a value of 370 to wrap around to 10.
*/
qreal QDeclarativeSpringFollow::modulus() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->modulus;
}

void QDeclarativeSpringFollow::setModulus(qreal modulus)
{
    Q_D(QDeclarativeSpringFollow);
    if (d->modulus != modulus) {
        d->haveModulus = modulus != 0.0;
        d->modulus = modulus;
        emit modulusChanged();
    }
}

/*!
    \qmlproperty real SpringFollow::mass
    This property holds the "mass" of the property being moved.

    mass is 1.0 by default.  Setting a different mass changes the dynamics of
    a \l spring follow.
*/
qreal QDeclarativeSpringFollow::mass() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->mass;
}

void QDeclarativeSpringFollow::setMass(qreal mass)
{
    Q_D(QDeclarativeSpringFollow);
    if (d->mass != mass && mass > 0.0) {
        d->useMass = mass != 1.0;
        d->mass = mass;
        emit massChanged();
    }
}

/*!
    \qmlproperty bool SpringFollow::enabled
    This property holds whether the target will track the source.

    The default value of this property is 'true'.
*/
bool QDeclarativeSpringFollow::enabled() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->enabled;
}

void QDeclarativeSpringFollow::setEnabled(bool enabled)
{
    Q_D(QDeclarativeSpringFollow);
    d->enabled = enabled;
    if (enabled)
        d->start();
    else
        d->stop();
}

/*!
    \qmlproperty bool SpringFollow::inSync
    This property is true when target is equal to the source; otherwise
    false.  If inSync is true the target is not being animated.

    If \l enabled is false then inSync will also be false.
*/
bool QDeclarativeSpringFollow::inSync() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->enabled && d->clock.state() != QAbstractAnimation::Running;
}

/*!
    \qmlproperty real SpringFollow::value
    The current value.
*/
qreal QDeclarativeSpringFollow::value() const
{
    Q_D(const QDeclarativeSpringFollow);
    return d->currentValue;
}

QT_END_NAMESPACE
