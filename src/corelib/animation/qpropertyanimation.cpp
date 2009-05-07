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
    \class QPropertyAnimation
    \brief The QPropertyAnimation class animates Qt properties
    \ingroup animation
    \preliminary

    QPropertyAnimation interpolates over \l{Qt's Property System}{Qt
    properties}. As property values are stored in \l{QVariant}s, the
    class inherits QVariantAnimation, and supports animation of the
    same \l{QVariant::Type}{variant types} as its super class. 

    A class declaring properties must be a QObject. To make it
    possible to animate a property, it must provide a setter (so that
    QPropertyAnimation can set the property's value). Note that this
    makes it possible to animate many of Qt's widgets. Let's look at
    an example:
    
    \code
        QPropertyAnimation animation(myWidget, "geometry");
        animation.setDuration(10000);
        animation.setStartValue(QRect(0, 0, 100, 30));
        animation.setEndValue(QRect(250, 250, 100, 30));

        animation.start();
    \endcode

    The property name and the QObject instance of which property
    should be animated are passed to the constructor. You can then
    specify the start and end value of the property. The procedure is
    equal for properties in classes you have implemented
    yourself--just check with QVariantAnimation that your QVariant
    type is supported.

    The QVariantAnimation class description explains how to set up the
    animation in detail. Note, however, that if a start value is not
    set, the property will start at the value it had when the
    QPropertyAnimation instance was created.

    QPropertyAnimation works like a charm on its own. For complex
    animations that, for instance, contain several objects,
    QAnimationGroup is provided. An animation group is an animation
    that can contain other animations, and that can manage when its
    animations are played. Look at QParallelAnimationGroup for an
    example.

    \sa QVariantAnimation, QAnimationGroup, {The Animation Framework}
*/

#ifndef QT_NO_ANIMATION

#include "qpropertyanimation.h"
#include "qanimationgroup.h"
#include <QtCore/qdebug.h>

#include "qpropertyanimation_p.h"

#include <QtCore/qmath.h>
#include <QtCore/qmutex.h>

QT_BEGIN_NAMESPACE

typedef QPair<QObject *, QByteArray> QPropertyAnimationPair;
typedef QHash<QPropertyAnimationPair, QPropertyAnimation*> QPropertyAnimationHash;
Q_GLOBAL_STATIC(QPropertyAnimationHash, _q_runningAnimations);
Q_GLOBAL_STATIC_WITH_ARGS(QMutex, guardHashLock, (QMutex::Recursive) )

void QPropertyAnimationPrivate::updateMetaProperty()
{
    if (!target || propertyName.isEmpty())
        return;

    if (hasMetaProperty == 0 && !property.isValid()) {
        const QMetaObject *mo = target->metaObject();
        propertyIndex = mo->indexOfProperty(propertyName);
        if (propertyIndex != -1) {
            hasMetaProperty = 1;
            property = mo->property(propertyIndex);
            propertyType = property.userType();
        } else {
            hasMetaProperty = 2;
        }
    }

    if (property.isValid())
        convertValues(propertyType);
}

void QPropertyAnimationPrivate::updateProperty(const QVariant &newValue)
{
    if (!target || state == QAbstractAnimation::Stopped)
        return;

    if (hasMetaProperty == 1) {
        if (newValue.userType() == propertyType) {
          //no conversion is needed, we directly call the QObject::qt_metacall
          void *data = const_cast<void*>(newValue.constData());
          target->qt_metacall(QMetaObject::WriteProperty, propertyIndex, &data);
        } else {
          property.write(target, newValue);
        }
    } else {
        target->setProperty(propertyName.constData(), newValue);
    }
}

/*!
    Construct a QPropertyAnimation object. \a parent is passed to QObject's
    constructor.
*/
QPropertyAnimation::QPropertyAnimation(QObject *parent)
    : QVariantAnimation(*new QPropertyAnimationPrivate, parent)
{
}

/*!
    Construct a QPropertyAnimation object. \a parent is passed to QObject's
    constructor. The animation changes the property \a propertyName on \a
    target. The default duration is 250ms.

    \sa targetObject, propertyName
*/
QPropertyAnimation::QPropertyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QVariantAnimation(*new QPropertyAnimationPrivate, parent)
{
    setTargetObject(target);
    setPropertyName(propertyName);
}

/*!
    Destroys the QPropertyAnimation instance.
 */
QPropertyAnimation::~QPropertyAnimation()
{
    stop();
}

/*!
    \property QPropertyAnimation::targetObject
    \brief the target QObject for this animation.

    This property defines the target QObject for this animation.
 */
QObject *QPropertyAnimation::targetObject() const
{
    Q_D(const QPropertyAnimation);
    return d->target;
}
void QPropertyAnimation::setTargetObject(QObject *target)
{
    Q_D(QPropertyAnimation);
    if (d->target == target)
        return;

    d->target = target;
    d->hasMetaProperty = 0;
    d->updateMetaProperty();
}

/*!
    \property QPropertyAnimation::propertyName
    \brief the target property name for this animation

    This property defines the target property name for this animation. The
    property name is required for the animation to operate.
 */
QByteArray QPropertyAnimation::propertyName() const
{
    Q_D(const QPropertyAnimation);
    return d->propertyName;
}
void QPropertyAnimation::setPropertyName(const QByteArray &propertyName)
{
    Q_D(QPropertyAnimation);
    d->propertyName = propertyName;
    d->hasMetaProperty = 0;
    d->updateMetaProperty();
}


/*!
    \reimp
 */
bool QPropertyAnimation::event(QEvent *event)
{
    return QVariantAnimation::event(event);
}

/*!
    This virtual function is called by QVariantAnimation whenever the current value
    changes. \a value is the new, updated value. It updates the current value
    of the property on the target object.

    \sa currentValue, currentTime
 */
void QPropertyAnimation::updateCurrentValue(const QVariant &value)
{
    Q_D(QPropertyAnimation);
    d->updateProperty(value);
}

/*!
    \reimp

    If the startValue is not defined when the state of the animation changes from Stopped to Running,
    the current property value is used as the initial value for the animation.
*/
void QPropertyAnimation::updateState(QAbstractAnimation::State oldState,
                                     QAbstractAnimation::State newState)
{
    Q_D(QPropertyAnimation);

    if (!d->target) {
        qWarning("QPropertyAnimation::updateState: Changing state of an animation without target");
        return;
    }

    QVariantAnimation::updateState(oldState, newState);
    QMutexLocker locker(guardHashLock());
    QPropertyAnimationHash * hash = _q_runningAnimations();
    QPropertyAnimationPair key(d->target, d->propertyName);
    if (newState == Running) {
        d->updateMetaProperty();
        QPropertyAnimation *oldAnim = hash->value(key, 0);
        if (oldAnim) {
            // try to stop the top level group
            QAbstractAnimation *current = oldAnim;
            while (current->group() && current->state() != Stopped)
                current = current->group();
            current->stop();
        }
        hash->insert(key, this);

        // update the default start value
        if (oldState == Stopped) {
            d->setDefaultStartValue(d->target->property(d->propertyName.constData()));
        }
    } else if (hash->value(key) == this) {
        hash->remove(key);
    }
}

#include "moc_qpropertyanimation.cpp"

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION
