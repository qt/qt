/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlbehavior_p.h"

#include "qmlanimation_p.h"
#include "qmltransition_p.h"

#include <qmlcontext.h>
#include <qmlinfo.h>

#include <QtCore/qparallelanimationgroup.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Behavior,QmlBehavior)

class QmlBehaviorPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlBehavior)
public:
    QmlBehaviorPrivate() : animation(0), enabled(true) {}

    QmlMetaProperty property;
    QVariant currentValue;
    QmlAbstractAnimation *animation;
    bool enabled;
};

/*!
    \qmlclass Behavior QmlBehavior
    \brief The Behavior element allows you to specify a default animation for a property change.

    Behaviors provide one way to specify \l{qmlanimation.html}{animations} in QML.

    In the example below, the rect will use a bounce easing curve over 200 millisecond for any changes to its y property:
    \code
    Rectangle {
        width: 20; height: 20
        color: "#00ff00"
        y: 200  //initial value
        y: Behavior {
            NumberAnimation {
                easing: "easeOutBounce(amplitude:100)"
                duration: 200
            }
        }
    }
    \endcode

    Currently only a single Behavior may be specified for a property;
    this Behavior can be enabled and disabled via the \l{enabled} property.
*/


QmlBehavior::QmlBehavior(QObject *parent)
    : QObject(*(new QmlBehaviorPrivate), parent)
{
}

QmlBehavior::~QmlBehavior()
{
}

/*!
    \qmlproperty Animation Behavior::animation
    \default

    The animation to use when the behavior is triggered.
*/

QmlAbstractAnimation *QmlBehavior::animation()
{
    Q_D(QmlBehavior);
    return d->animation;
}

void QmlBehavior::setAnimation(QmlAbstractAnimation *animation)
{
    Q_D(QmlBehavior);
    if (d->animation) {
        qmlInfo(this) << tr("Can't change the animation assigned to a Behavior.");
        return;
    }

    d->animation = animation;
    if (d->animation)
        d->animation->setTarget(d->property);
}

/*!
    \qmlproperty bool Behavior::enabled
    Whether the Behavior will be triggered when the property it is tracking changes.

    By default a Behavior is enabled.
*/

bool QmlBehavior::enabled() const
{
    Q_D(const QmlBehavior);
    return d->enabled;
}

void QmlBehavior::setEnabled(bool enabled)
{
    Q_D(QmlBehavior);
    if (d->enabled == enabled)
        return;
    d->enabled = enabled;
    emit enabledChanged();
}

void QmlBehavior::write(const QVariant &value)
{
    Q_D(QmlBehavior);
    if (!d->animation || !d->enabled) {
        d->property.write(value, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
        return;
    }

    d->currentValue = d->property.read();

    d->animation->qtAnimation()->stop();

    QmlStateOperation::ActionList actions;
    QmlAction action;
    action.property = d->property;
    action.fromValue = d->currentValue;
    action.toValue = value;
    actions << action;

    QList<QmlMetaProperty> after;
    if (d->animation)
        d->animation->transition(actions, after, QmlAbstractAnimation::Forward);
    d->animation->qtAnimation()->start();
    if (!after.contains(d->property))
        d->property.write(value, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
}

void QmlBehavior::setTarget(const QmlMetaProperty &property)
{
    Q_D(QmlBehavior);
    d->property = property;
    d->currentValue = property.read();
    if (d->animation)
        d->animation->setTarget(property);
}

QT_END_NAMESPACE
