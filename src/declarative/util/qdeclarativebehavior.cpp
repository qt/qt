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

#include "private/qdeclarativebehavior_p.h"

#include "private/qdeclarativeanimation_p.h"
#include "private/qdeclarativetransition_p.h"

#include <qdeclarativecontext.h>
#include <qdeclarativeinfo.h>
#include <qdeclarativeproperty_p.h>
#include <qdeclarativeguard_p.h>
#include <qdeclarativeengine_p.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeBehaviorPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeBehavior)
public:
    QDeclarativeBehaviorPrivate() : animation(0), enabled(true), finalized(false)
      , blockRunningChanged(false) {}

    QDeclarativeProperty property;
    QVariant currentValue;
    QVariant targetValue;
    QDeclarativeGuard<QDeclarativeAbstractAnimation> animation;
    bool enabled;
    bool finalized;
    bool blockRunningChanged;
};

/*!
    \qmlclass Behavior QDeclarativeBehavior
    \since 4.7
    \brief The Behavior element allows you to specify a default animation for a property change.

    Behaviors provide one way to specify \l{qdeclarativeanimation.html}{animations} in QML.

    In the example below, the rectangle will use a bounce easing curve over 200 millisecond for any changes to its y property:
    \code
    Rectangle {
        width: 20; height: 20
        color: "#00ff00"
        y: 200  // initial value
        Behavior on y {
            NumberAnimation {
                easing.type: Easing.OutBounce
                easing.amplitude: 100
                duration: 200
            }
        }
    }
    \endcode

    Currently only a single Behavior may be specified for a property;
    this Behavior can be enabled and disabled via the \l{enabled} property.

    \sa {declarative/animation/behaviors}{Behavior example}, QtDeclarative
*/


QDeclarativeBehavior::QDeclarativeBehavior(QObject *parent)
    : QObject(*(new QDeclarativeBehaviorPrivate), parent)
{
}

QDeclarativeBehavior::~QDeclarativeBehavior()
{
}

/*!
    \qmlproperty Animation Behavior::animation
    \default

    The animation to use when the behavior is triggered.
*/

QDeclarativeAbstractAnimation *QDeclarativeBehavior::animation()
{
    Q_D(QDeclarativeBehavior);
    return d->animation;
}

void QDeclarativeBehavior::setAnimation(QDeclarativeAbstractAnimation *animation)
{
    Q_D(QDeclarativeBehavior);
    if (d->animation) {
        qmlInfo(this) << tr("Cannot change the animation assigned to a Behavior.");
        return;
    }

    d->animation = animation;
    if (d->animation) {
        d->animation->setDefaultTarget(d->property);
        d->animation->setDisableUserControl();
        connect(d->animation->qtAnimation(),
                SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
                this,
                SLOT(qtAnimationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
        connect(this,
                SIGNAL(qtAnimationRunningChanged(bool)),
                d->animation,
                SLOT(behaviorControlRunningChanged(bool)));
    }
}


void QDeclarativeBehavior::qtAnimationStateChanged(QAbstractAnimation::State newState,QAbstractAnimation::State)
{
    Q_D(QDeclarativeBehavior);
    if (!d->blockRunningChanged)
        emit qtAnimationRunningChanged(newState == QAbstractAnimation::Running);
}


/*!
    \qmlproperty bool Behavior::enabled
    Whether the Behavior will be triggered when the property it is tracking changes.

    By default a Behavior is enabled.
*/

bool QDeclarativeBehavior::enabled() const
{
    Q_D(const QDeclarativeBehavior);
    return d->enabled;
}

void QDeclarativeBehavior::setEnabled(bool enabled)
{
    Q_D(QDeclarativeBehavior);
    if (d->enabled == enabled)
        return;
    d->enabled = enabled;
    emit enabledChanged();
}

void QDeclarativeBehavior::write(const QVariant &value)
{
    Q_D(QDeclarativeBehavior);
    qmlExecuteDeferred(this);
    if (!d->animation || !d->enabled || !d->finalized) {
        QDeclarativePropertyPrivate::write(d->property, value, QDeclarativePropertyPrivate::BypassInterceptor | QDeclarativePropertyPrivate::DontRemoveBinding);
        d->targetValue = value;
        return;
    }

    if (value == d->targetValue)
        return;

    d->currentValue = d->property.read();
    d->targetValue = value;

    if (d->animation->qtAnimation()->duration() != -1
            && d->animation->qtAnimation()->state() != QAbstractAnimation::Stopped) {
        d->blockRunningChanged = true;
        d->animation->qtAnimation()->stop();
    }

    QDeclarativeStateOperation::ActionList actions;
    QDeclarativeAction action;
    action.property = d->property;
    action.fromValue = d->currentValue;
    action.toValue = value;
    actions << action;

    QList<QDeclarativeProperty> after;
    d->animation->transition(actions, after, QDeclarativeAbstractAnimation::Forward);
    d->animation->qtAnimation()->start();
    d->blockRunningChanged = false;
    if (!after.contains(d->property))
        QDeclarativePropertyPrivate::write(d->property, value, QDeclarativePropertyPrivate::BypassInterceptor | QDeclarativePropertyPrivate::DontRemoveBinding);    
}

void QDeclarativeBehavior::setTarget(const QDeclarativeProperty &property)
{
    Q_D(QDeclarativeBehavior);
    d->property = property;
    d->currentValue = property.read();
    if (d->animation)
        d->animation->setDefaultTarget(property);

    QDeclarativeEnginePrivate *engPriv = QDeclarativeEnginePrivate::get(qmlEngine(this));
    engPriv->registerFinalizedParserStatusObject(this, this->metaObject()->indexOfSlot("componentFinalized()"));
}

void QDeclarativeBehavior::componentFinalized()
{
    Q_D(QDeclarativeBehavior);
    d->finalized = true;
}

QT_END_NAMESPACE
