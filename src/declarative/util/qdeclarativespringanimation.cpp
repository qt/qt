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

#include "private/qdeclarativespringanimation_p.h"

#include "private/qdeclarativeanimation_p_p.h"
#include <qdeclarativeproperty_p.h>

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

#include <limits.h>
#include <math.h>

QT_BEGIN_NAMESPACE



class QDeclarativeSpringAnimationPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeSpringAnimation)
public:
    QDeclarativeSpringAnimationPrivate()
        : currentValue(0), to(0), from(0), maxVelocity(0), lastTime(0)
        , mass(1.0), spring(0.), damping(0.), velocity(0), epsilon(0.01)
        , modulus(0.0), useMass(false), haveModulus(false), enabled(true)
        , fromDefined(false), toDefined(false)
        , mode(Track), clock(this) {}

    qreal currentValue;
    qreal to;
    qreal from;
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
    bool fromDefined : 1;
    bool toDefined : 1;

    enum Mode {
        Track,
        Velocity,
        Spring
    };
    Mode mode;

    void tick(int);
    void updateMode();

    QTickAnimationProxy<QDeclarativeSpringAnimationPrivate, &QDeclarativeSpringAnimationPrivate::tick> clock;
};

void QDeclarativeSpringAnimationPrivate::tick(int time)
{
    if (mode == Track) {
        clock.stop();
        return;
    }

    int elapsed = time - lastTime;
    if (!elapsed)
        return;
    qreal srcVal = to;

    bool stop = false;

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
            stop = true;
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
                stop = true;
            }
        } else {
            currentValue -= moveBy;
            if (haveModulus && currentValue < 0.0)
                currentValue = fmod(currentValue, modulus) + modulus;
            if (currentValue < to) {
                currentValue = to;
                stop = true;
            }
        }
        lastTime = time;
    }

    QDeclarativePropertyPrivate::write(defaultProperty, currentValue,
                                       QDeclarativePropertyPrivate::BypassInterceptor |
                                       QDeclarativePropertyPrivate::DontRemoveBinding);

    if (stop)
        clock.stop();
}

void QDeclarativeSpringAnimationPrivate::updateMode()
{
    if (spring == 0. && maxVelocity == 0.)
        mode = Track;
    else if (spring > 0.)
        mode = Spring;
    else
        mode = Velocity;
}

/*!
    \qmlclass SpringAnimation QDeclarativeSpringAnimation
    \since 4.7
*/

QDeclarativeSpringAnimation::QDeclarativeSpringAnimation(QObject *parent)
: QDeclarativeAbstractAnimation(*(new QDeclarativeSpringAnimationPrivate),parent)
{
}

QDeclarativeSpringAnimation::~QDeclarativeSpringAnimation()
{
}

void QDeclarativeSpringAnimation::setTarget(const QDeclarativeProperty &property)
{
    Q_D(QDeclarativeSpringAnimation);
    d->defaultProperty = property;
    d->currentValue = property.read().toReal();
    if (!d->avoidPropertyValueSourceStart) {
        setRunning(true);
    }
}

qreal QDeclarativeSpringAnimation::to() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->toDefined ? d->to : 0;
}

/*!
    \qmlproperty real SpringFollow::to
*/

void QDeclarativeSpringAnimation::setTo(qreal value)
{
    Q_D(QDeclarativeSpringAnimation);
    if (d->to == value)
        return;

    d->to = value;
    d->toDefined = true;
    d->lastTime = 0;
    emit toChanged(value);
}

qreal QDeclarativeSpringAnimation::from() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->fromDefined ? d->from : 0;
}

/*!
    \qmlproperty real SpringFollow::from
*/

void QDeclarativeSpringAnimation::setFrom(qreal value)
{
    Q_D(QDeclarativeSpringAnimation);
    if (d->from == value)
        return;

    d->currentValue = d->from = value;
    d->fromDefined = true;
    d->lastTime = 0;
    emit fromChanged(value);
}


/*!
    \qmlproperty real SpringAnimation::velocity
    This property holds the maximum velocity allowed when tracking the source.
*/

qreal QDeclarativeSpringAnimation::velocity() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->maxVelocity;
}

void QDeclarativeSpringAnimation::setVelocity(qreal velocity)
{
    Q_D(QDeclarativeSpringAnimation);
    d->maxVelocity = velocity;
    d->velocityms = velocity / 1000.0;
    d->updateMode();
}

/*!
    \qmlproperty real SpringAnimation::spring
    This property holds the spring constant

    The spring constant describes how strongly the target is pulled towards the
    source. Setting spring to 0 turns off spring tracking.  Useful values 0 - 5.0

    When a spring constant is set and the velocity property is greater than 0,
    velocity limits the maximum speed.
*/
qreal QDeclarativeSpringAnimation::spring() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->spring;
}

void QDeclarativeSpringAnimation::setSpring(qreal spring)
{
    Q_D(QDeclarativeSpringAnimation);
    d->spring = spring;
    d->updateMode();
}

/*!
    \qmlproperty real SpringAnimation::damping
    This property holds the spring damping constant

    The damping constant describes how quickly a sprung follower comes to rest.
    Useful range is 0 - 1.0
*/
qreal QDeclarativeSpringAnimation::damping() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->damping;
}

void QDeclarativeSpringAnimation::setDamping(qreal damping)
{
    Q_D(QDeclarativeSpringAnimation);
    if (damping > 1.)
        damping = 1.;

    d->damping = damping;
}


/*!
    \qmlproperty real SpringAnimation::epsilon
    This property holds the spring epsilon

    The epsilon is the rate and amount of change in the value which is close enough
    to 0 to be considered equal to zero. This will depend on the usage of the value.
    For pixel positions, 0.25 would suffice. For scale, 0.005 will suffice.

    The default is 0.01. Tuning this value can provide small performance improvements.
*/
qreal QDeclarativeSpringAnimation::epsilon() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->epsilon;
}

void QDeclarativeSpringAnimation::setEpsilon(qreal epsilon)
{
    Q_D(QDeclarativeSpringAnimation);
    d->epsilon = epsilon;
}

/*!
    \qmlproperty real SpringAnimation::modulus
    This property holds the modulus value.

    Setting a \a modulus forces the target value to "wrap around" at the modulus.
    For example, setting the modulus to 360 will cause a value of 370 to wrap around to 10.
*/
qreal QDeclarativeSpringAnimation::modulus() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->modulus;
}

void QDeclarativeSpringAnimation::setModulus(qreal modulus)
{
    Q_D(QDeclarativeSpringAnimation);
    if (d->modulus != modulus) {
        d->haveModulus = modulus != 0.0;
        d->modulus = modulus;
        emit modulusChanged();
    }
}

/*!
    \qmlproperty real SpringAnimation::mass
    This property holds the "mass" of the property being moved.

    mass is 1.0 by default.  Setting a different mass changes the dynamics of
    a \l spring follow.
*/
qreal QDeclarativeSpringAnimation::mass() const
{
    Q_D(const QDeclarativeSpringAnimation);
    return d->mass;
}

void QDeclarativeSpringAnimation::setMass(qreal mass)
{
    Q_D(QDeclarativeSpringAnimation);
    if (d->mass != mass && mass > 0.0) {
        d->useMass = mass != 1.0;
        d->mass = mass;
        emit massChanged();
    }
}

void QDeclarativeSpringAnimation::transition(QDeclarativeStateActions &actions,
                                             QDeclarativeProperties &modified,
                                             TransitionDirection direction)
{
    Q_D(QDeclarativeSpringAnimation);
    Q_UNUSED(direction);

    if (d->clock.state() != QAbstractAnimation::Running)
        d->lastTime = 0;

    if (!actions.isEmpty()) {
        for (int i = 0; i < actions.size(); ++i) {
            if (!d->toDefined)
                d->to = actions.at(i).toValue.toReal();
            if (!d->fromDefined)
                d->currentValue = actions.at(i).fromValue.toReal();
            if (d->mode != QDeclarativeSpringAnimationPrivate::Track)
                modified << d->defaultProperty;
        }
    }
}


QAbstractAnimation *QDeclarativeSpringAnimation::qtAnimation()
{
    Q_D(QDeclarativeSpringAnimation);
    return &d->clock;
}

QT_END_NAMESPACE
