/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
    \class QItemAnimation
    \brief The QItemAnimation class animates properties for QGraphicsItem
    \since 4.5
    \ingroup animation
    \preliminary

    This class is part of {The Animation Framework}.  You can use QItemAnimation
    by itself as a simple animation class, or as part of more complex
    animations through QAnimationGroup.

    The most common way to use QItemAnimation is to construct an instance
    of it by passing a pointer to a QGraphicsItem and the property you
    would like to animate to QItemAnimation's constructor.

    The start value of the animation is optional. If you do not set any start
    value, the animation will operate on the target's current property value
    at the point when the animation was started. You can call setStartValue()
    to set the start value, and setEndValue() to set the target value for
    the animated property.

    You can choose to assign a target item by either calling setTargetItem()
    or by passing a QGraphicsItem pointer to QVariantAnimation's constructor.

    \sa QVariantAnimation, QAnimationGroup, {The Animation Framework}
*/


#ifndef QT_NO_ANIMATION

#include "qitemanimation.h"
#include "qitemanimation_p.h"

#include <QtCore/QMutex>
#ifdef QT_EXPERIMENTAL_SOLUTION
#include "qanimationgroup.h"
#else
#include <QtCore/QAnimationGroup>
#endif
#include <QtGui/QGraphicsItem>


QT_BEGIN_NAMESPACE

typedef QPair<QGraphicsItem *, QItemAnimation::PropertyName> QItemAnimationPair;
typedef QHash<QItemAnimationPair, QItemAnimation*> QItemAnimationHash;
Q_GLOBAL_STATIC(QItemAnimationHash, _q_runningAnimations)
Q_GLOBAL_STATIC_WITH_ARGS(QMutex, guardHashLock, (QMutex::Recursive) )

void QItemAnimationPrivate::initDefaultStartValue()
{
    if (target && !defaultStartValue.isValid() && (atBeginning() || atEnd())) {
        switch (propertyName)
        {
        case QItemAnimation::Position:
            setDefaultStartValue(target->pos());
            break;
        case QItemAnimation::Opacity:
            setDefaultStartValue(target->opacity());
            break;
        case QItemAnimation::RotationX:
            setDefaultStartValue(target->xRotation());
            break;
        case QItemAnimation::RotationY:
            setDefaultStartValue(target->yRotation());
            break;
        case QItemAnimation::RotationZ:
            setDefaultStartValue(target->zRotation());
            break;
        case QItemAnimation::ScaleFactorX:
            setDefaultStartValue(target->xScale());
            break;
        case QItemAnimation::ScaleFactorY:
            setDefaultStartValue(target->yScale());
            break;
        default:
            break;
        }
    }
}


/*!
    Construct a QItemAnimation object. \a parent is passed to QObject's
    constructor.
*/

QItemAnimation::QItemAnimation(QObject *parent) : QVariantAnimation(*new QItemAnimationPrivate, parent)
{
}

/*!
    Construct a QItemAnimation object. \a parent is passed to QObject's
    constructor. The animation changes the property \a propertyName on \a
    target. The default duration is 250ms.

    \sa targetItem, propertyName
*/

QItemAnimation::QItemAnimation(QGraphicsItem *target, PropertyName p, QObject *parent) : QVariantAnimation(*new QItemAnimationPrivate, parent)
{
    Q_D(QItemAnimation);
    d->target = target;
    d->propertyName = p;
}

/*!
    Destroys the QPropertyAnimation instance.
 */
QItemAnimation::~QItemAnimation()
{
    stop();
}

/*!
    \property QItemAnimation::targetItem
    \brief the target Graphics Item for this animation.

    This property defines the target item for this animation.

    \sa targetItem
 */

QGraphicsItem *QItemAnimation::targetItem() const
{
   Q_D(const QItemAnimation);
   return d->target;
}

void QItemAnimation::setTargetItem(QGraphicsItem *item)
{
   Q_D(QItemAnimation);
   d->target = item;
}

/*!
    \property QItemAnimation::propertyName
    \brief the target property for this animation

    This property defines the target property for this animation. The
    property is required for the animation to operate.
 */
QItemAnimation::PropertyName QItemAnimation::propertyName() const
{
    Q_D(const QItemAnimation);
    return d->propertyName;
}

void QItemAnimation::setPropertyName(PropertyName p)
{
    Q_D(QItemAnimation);
    d->propertyName = p;
}

/*!
    This static function returns the list of running animations on \a item.
    If item is 0, then it returns all QItemAnimations running on all QGraphicsItem.
 */
QList<QItemAnimation*> QItemAnimation::runningAnimations(QGraphicsItem *item)
{
    QMutexLocker locker(guardHashLock());
    QList<QItemAnimation*> animList = _q_runningAnimations()->values();
    if (item == 0)
        return animList;

    QList<QItemAnimation*> ret;

    for (QList<QItemAnimation*>::const_iterator it = animList.constBegin(); it != animList.constEnd(); ++it) {
        if ((*it)->targetItem() == item)
            ret += *it;
    }

    return ret;
}

/*!
    This static function returns the running animations on \a item and on \a property.
    \a prop.
 */
QItemAnimation* QItemAnimation::runningAnimation(QGraphicsItem *item, PropertyName prop)
{
    QMutexLocker locker(guardHashLock());
    return _q_runningAnimations()->value(qMakePair(item, prop), 0 /*default value*/);
}

/*!
    \reimp
 */
bool QItemAnimation::event(QEvent *event)
{
    return QVariantAnimation::event(event);
}

/*!
    \reimp
 */
void QItemAnimation::updateCurrentValue(const QVariant &value)
{
    Q_D(QItemAnimation);
    if (!d->target || d->state == Stopped)
        return;

    switch (d->propertyName)
    {
    case Position:
        d->target->setPos(qVariantValue<QPointF>(value));
        break;
    case Opacity:
        d->target->setOpacity(qVariantValue<qreal>(value));
        break;
    case RotationX:
        d->target->setXRotation(qVariantValue<qreal>(value));
        break;
    case RotationY:
        d->target->setYRotation(qVariantValue<qreal>(value));
        break;
    case RotationZ:
        d->target->setZRotation(qVariantValue<qreal>(value));
        break;
    case ScaleFactorX:
        d->target->setXScale(qVariantValue<qreal>(value));
        break;
    case ScaleFactorY:
        d->target->setYScale(qVariantValue<qreal>(value));
        break;
    default:
        qWarning("The property you're trying to animate is not managed by the item");
        break;
    }
}


/*!
    \reimp
*/
void QItemAnimation::updateState(QAbstractAnimation::State oldState,
                                 QAbstractAnimation::State newState)
{
    Q_D(QItemAnimation);
    QVariantAnimation::updateState(oldState, newState);
    QMutexLocker locker(guardHashLock());
    QItemAnimationHash *hash = _q_runningAnimations();
    QItemAnimationPair key(d->target, d->propertyName);

    //let's try to convert start and target values according to the type of the proerty
    //we're animating
    if (newState != Stopped) {
        int type = QVariant::Invalid;
        switch (d->propertyName)
        {
        case Position:
            type = QVariant::PointF;
            break;
        case Opacity:
        case RotationX:
        case RotationY:
        case RotationZ:
        case ScaleFactorX:
        case ScaleFactorY:
            type = qMetaTypeId<qreal>();
            break;
        case None:
        default:
            break;

        }
        if (type != QVariant::Invalid) {
            d->convertValues(type);
        }
    }

    if (newState == Running) {
        if (hash->contains(key)) {
            QItemAnimation *oldAnim = hash->value(key);
            if (oldAnim != this) {
                //we try to stop the top level group
                QAbstractAnimation *current = oldAnim;
                while(current->group() && current->state() != Stopped) current = current->group();
                current->stop();
            }
        }
        hash->insert(key, this);
        // Initialize start value
        d->initDefaultStartValue();

    } else if (hash->value(key) == this) {
        hash->remove(key);
    }
}

///TODO: should be placed somewhere else (in its own file)
template<> Q_INLINE_TEMPLATE QColor _q_interpolate(const QColor &f,const QColor &t, qreal progress)
{
    return QColor(_q_interpolate(f.red(), t.red(), progress),
                  _q_interpolate(f.green(), t.green(), progress),
                  _q_interpolate(f.blue(), t.blue(), progress),
                  _q_interpolate(f.alpha(), t.alpha(), progress));
}



static int qRegisterGuiGetInterpolator()
{
    qRegisterAnimationInterpolator<QColor>(_q_interpolateVariant<QColor>);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(qRegisterGuiGetInterpolator)

static int qUnregisterGuiGetInterpolator()
{
    qRegisterAnimationInterpolator<QColor>(0);
    return 1;
}
Q_DESTRUCTOR_FUNCTION(qUnregisterGuiGetInterpolator)

QT_END_NAMESPACE

#include "moc_qitemanimation.cpp"

#endif //QT_NO_ANIMATION
