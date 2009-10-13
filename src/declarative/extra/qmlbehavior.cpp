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

#include <private/qobject_p.h>
#include "qmlanimation.h"
#include "qmltransition.h"
#include "qmlbehavior.h"
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlinfo.h>
#include <QtCore/qparallelanimationgroup.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Behavior,QmlBehavior)

class QmlBehaviorPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlBehavior)
public:
    QmlBehaviorPrivate() : animation(0) {}

    QmlMetaProperty property;
    QVariant currentValue;
    QmlAbstractAnimation *animation;
};

/*!
    \qmlclass Behavior QmlBehavior
    \brief The Behavior element allows you to specify a default animation for a property change.

    In example below, the rect will use a bounce easing curve over 200 millisecond for any changes to its y property:
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
        qmlInfo(tr("Can't change the animation assigned to a Behavior."),this);
        return;
    }

    d->animation = animation;
    if (d->animation)
        d->animation->setTarget(d->property);
}

void QmlBehavior::write(const QVariant &value)
{
    Q_D(QmlBehavior);
    if (!d->animation) {
        d->property.write(value, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
        return;
    }

    d->currentValue = d->property.read();

    d->animation->qtAnimation()->stop();

    QmlStateOperation::ActionList actions;
    Action action;
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
