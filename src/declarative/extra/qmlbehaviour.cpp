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
#include "qmlbehaviour.h"
#include <QtDeclarative/qmlcontext.h>
#include <QtCore/qparallelanimationgroup.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Behavior,QmlBehaviour)

class QmlBehaviourData : public QObject
{
Q_OBJECT
public:
    QmlBehaviourData(QObject *parent)
        : QObject(parent) {}

    Q_PROPERTY(QVariant endValue READ endValue NOTIFY valuesChanged)
    Q_PROPERTY(QVariant startValue READ startValue NOTIFY valuesChanged)
    QVariant endValue() const { return e; }
    QVariant startValue() const { return s; }

    QVariant e;
    QVariant s;

Q_SIGNALS:
    void valuesChanged();

private:
    friend class QmlBehaviour;
};

class QmlBehaviourPrivate : public QObjectPrivate
{
public:
    QmlBehaviourPrivate() : operations(this) {}
    QmlMetaProperty property;
    QVariant currentValue;

    QVariant fromValue;
    QVariant toValue;
    class AnimationList : public QmlConcreteList<QmlAbstractAnimation *>
    {
    public:
        AnimationList(QmlBehaviourPrivate *parent) : _parent(parent) {}
        virtual void append(QmlAbstractAnimation *a)
        {
            QmlConcreteList<QmlAbstractAnimation *>::append(a);
            _parent->group->addAnimation(a->qtAnimation());
            if (_parent->property.isValid()) {
                a->setTarget(_parent->property);
            }
        }
        virtual void clear() {  QmlConcreteList<QmlAbstractAnimation *>::clear(); } //###
    private:
        QmlBehaviourPrivate *_parent;
    };
    AnimationList operations;
    QParallelAnimationGroup *group;
};

/*!
    \qmlclass Behavior QmlBehaviour
    \brief The Behavior element allows you to specify a default animation for a property change.

    In example below, the rect will use a bounce easing curve over 200 millisecond for any changes to its y property:
    \code
    Rect {
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

QmlBehaviour::QmlBehaviour(QObject *parent)
: QmlPropertyValueSource(*(new QmlBehaviourPrivate), parent)
{
    Q_D(QmlBehaviour);
    d->group = new QParallelAnimationGroup(this);
}

/*!
    \qmlproperty QVariant Behavior::fromValue
    This property holds a selector specifying a starting value for the behavior

    If you only want the behavior to apply when the change starts at a
    specific value you can specify fromValue. This selector is used in conjunction
    with the toValue selector.
*/

QVariant QmlBehaviour::fromValue() const
{
    Q_D(const QmlBehaviour);
    return d->fromValue;
}

void QmlBehaviour::setFromValue(const QVariant &v)
{
    Q_D(QmlBehaviour);
    d->fromValue = v;
}

/*!
    \qmlproperty QVariant Behavior::toValue
    This property holds a selector specifying a ending value for the behavior

    If you only want the behavior to apply when the change ends at a
    specific value you can specify toValue. This selector is used in conjunction
    with the fromValue selector.
*/

QVariant QmlBehaviour::toValue() const
{
    Q_D(const QmlBehaviour);
    return d->toValue;
}

void QmlBehaviour::setToValue(const QVariant &v)
{
    Q_D(QmlBehaviour);
    d->toValue = v;
}

QmlList<QmlAbstractAnimation *>* QmlBehaviour::operations()
{
    Q_D(QmlBehaviour);
    return &d->operations;
}

QmlBehaviour::~QmlBehaviour()
{
    //### do we need any other cleanup here?
}

bool QmlBehaviour::_ignore = false;
void QmlBehaviour::propertyValueChanged()
{
    Q_D(QmlBehaviour);
    if (_ignore)
        return;

    QVariant newValue = d->property.read();

    if ((!fromValue().isValid() || fromValue() == d->currentValue) && 
       (!toValue().isValid() || toValue() == newValue)) {

        //### does this clean up everything needed?
        d->group->stop();

        QmlStateOperation::ActionList actions;
        Action action;
        action.property = d->property;
        action.fromValue = d->currentValue;
        action.toValue = newValue;
        actions << action;

        _ignore = true;
        d->property.write(d->currentValue);

        QList<QmlMetaProperty> after;
        for (int ii = 0; ii < d->operations.count(); ++ii) {
            d->operations.at(ii)->transition(actions, after, QmlAbstractAnimation::Forward);
        }
        d->group->start();
        if (!after.contains(d->property))
            d->property.write(newValue);
        _ignore = false;
    }

    d->currentValue = newValue;
}

void QmlBehaviour::setTarget(const QmlMetaProperty &property)
{
    Q_D(QmlBehaviour);
    d->property = property;
    d->currentValue = property.read();
    d->property.connectNotifier(this, SLOT(propertyValueChanged()));
    for (int ii = 0; ii < d->operations.count(); ++ii) {
        d->operations.at(ii)->setTarget(property);
    }
}

QT_END_NAMESPACE

#include "qmlbehaviour.moc"
