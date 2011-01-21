// Commit: 91501cc9b542de644cd70098a6bc5ff738cdeb49
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

#include "qsganimation_p.h"
#include "qsganimation_p_p.h"
#include "qsgstateoperations_p.h"

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtCore/qmath.h>
#include <QtCore/qsequentialanimationgroup.h>
#include <QtCore/qparallelanimationgroup.h>

QT_BEGIN_NAMESPACE

QSGParentAnimation::QSGParentAnimation(QObject *parent)
    : QDeclarativeAnimationGroup(*(new QSGParentAnimationPrivate), parent)
{
    Q_D(QSGParentAnimation);
    d->topLevelGroup = new QSequentialAnimationGroup;
    QDeclarative_setParent_noEvent(d->topLevelGroup, this);

    d->startAction = new QActionAnimation;
    QDeclarative_setParent_noEvent(d->startAction, d->topLevelGroup);
    d->topLevelGroup->addAnimation(d->startAction);

    d->ag = new QParallelAnimationGroup;
    QDeclarative_setParent_noEvent(d->ag, d->topLevelGroup);
    d->topLevelGroup->addAnimation(d->ag);

    d->endAction = new QActionAnimation;
    QDeclarative_setParent_noEvent(d->endAction, d->topLevelGroup);
    d->topLevelGroup->addAnimation(d->endAction);
}

QSGParentAnimation::~QSGParentAnimation()
{
}

QSGItem *QSGParentAnimation::target() const
{
    Q_D(const QSGParentAnimation);
    return d->target;
}

void QSGParentAnimation::setTarget(QSGItem *target)
{
    Q_D(QSGParentAnimation);
    if (target == d->target)
        return;

    d->target = target;
    emit targetChanged();
}

QSGItem *QSGParentAnimation::newParent() const
{
    Q_D(const QSGParentAnimation);
    return d->newParent;
}

void QSGParentAnimation::setNewParent(QSGItem *newParent)
{
    Q_D(QSGParentAnimation);
    if (newParent == d->newParent)
        return;

    d->newParent = newParent;
    emit newParentChanged();
}

QSGItem *QSGParentAnimation::via() const
{
    Q_D(const QSGParentAnimation);
    return d->via;
}

void QSGParentAnimation::setVia(QSGItem *via)
{
    Q_D(QSGParentAnimation);
    if (via == d->via)
        return;

    d->via = via;
    emit viaChanged();
}

//### mirrors same-named function in QSGItem
QPointF QSGParentAnimationPrivate::computeTransformOrigin(QSGItem::TransformOrigin origin, qreal width, qreal height) const
{
    switch(origin) {
    default:
    case QSGItem::TopLeft:
        return QPointF(0, 0);
    case QSGItem::Top:
        return QPointF(width / 2., 0);
    case QSGItem::TopRight:
        return QPointF(width, 0);
    case QSGItem::Left:
        return QPointF(0, height / 2.);
    case QSGItem::Center:
        return QPointF(width / 2., height / 2.);
    case QSGItem::Right:
        return QPointF(width, height / 2.);
    case QSGItem::BottomLeft:
        return QPointF(0, height);
    case QSGItem::Bottom:
        return QPointF(width / 2., height);
    case QSGItem::BottomRight:
        return QPointF(width, height);
    }
}

void QSGParentAnimation::transition(QDeclarativeStateActions &actions,
                        QDeclarativeProperties &modified,
                        TransitionDirection direction)
{
    Q_D(QSGParentAnimation);

    struct QSGParentAnimationData : public QAbstractAnimationAction
    {
        QSGParentAnimationData() {}
        ~QSGParentAnimationData() { qDeleteAll(pc); }

        QDeclarativeStateActions actions;
        //### reverse should probably apply on a per-action basis
        bool reverse;
        QList<QSGParentChange *> pc;
        virtual void doAction()
        {
            for (int ii = 0; ii < actions.count(); ++ii) {
                const QDeclarativeAction &action = actions.at(ii);
                if (reverse)
                    action.event->reverse();
                else
                    action.event->execute();
            }
        }
    };

    QSGParentAnimationData *data = new QSGParentAnimationData;
    QSGParentAnimationData *viaData = new QSGParentAnimationData;

    bool hasExplicit = false;
    if (d->target && d->newParent) {
        data->reverse = false;
        QDeclarativeAction myAction;
        QSGParentChange *pc = new QSGParentChange;
        pc->setObject(d->target);
        pc->setParent(d->newParent);
        myAction.event = pc;
        data->pc << pc;
        data->actions << myAction;
        hasExplicit = true;
        if (d->via) {
            viaData->reverse = false;
            QDeclarativeAction myVAction;
            QSGParentChange *vpc = new QSGParentChange;
            vpc->setObject(d->target);
            vpc->setParent(d->via);
            myVAction.event = vpc;
            viaData->pc << vpc;
            viaData->actions << myVAction;
        }
        //### once actions have concept of modified,
        //    loop to match appropriate ParentChanges and mark as modified
    }

    if (!hasExplicit)
    for (int i = 0; i < actions.size(); ++i) {
        QDeclarativeAction &action = actions[i];
        if (action.event && action.event->typeName() == QLatin1String("ParentChange")
            && (!d->target || static_cast<QSGParentChange*>(action.event)->object() == d->target)) {

            QSGParentChange *pc = static_cast<QSGParentChange*>(action.event);
            QDeclarativeAction myAction = action;
            data->reverse = action.reverseEvent;

            //### this logic differs from PropertyAnimation
            //    (probably a result of modified vs. done)
            if (d->newParent) {
                QSGParentChange *epc = new QSGParentChange;
                epc->setObject(static_cast<QSGParentChange*>(action.event)->object());
                epc->setParent(d->newParent);
                myAction.event = epc;
                data->pc << epc;
                data->actions << myAction;
                pc = epc;
            } else {
                action.actionDone = true;
                data->actions << myAction;
            }

            if (d->via) {
                viaData->reverse = false;
                QDeclarativeAction myAction;
                QSGParentChange *vpc = new QSGParentChange;
                vpc->setObject(pc->object());
                vpc->setParent(d->via);
                myAction.event = vpc;
                viaData->pc << vpc;
                viaData->actions << myAction;
                QDeclarativeAction dummyAction;
                QDeclarativeAction &xAction = pc->xIsSet() && i < actions.size()-1 ? actions[++i] : dummyAction;
                QDeclarativeAction &yAction = pc->yIsSet() && i < actions.size()-1 ? actions[++i] : dummyAction;
                QDeclarativeAction &sAction = pc->scaleIsSet() && i < actions.size()-1 ? actions[++i] : dummyAction;
                QDeclarativeAction &rAction = pc->rotationIsSet() && i < actions.size()-1 ? actions[++i] : dummyAction;
                QSGItem *target = pc->object();
                QSGItem *targetParent = action.reverseEvent ? pc->originalParent() : pc->parent();

                //### this mirrors the logic in QSGParentChange.
                bool ok;
                const QTransform &transform = targetParent->itemTransform(d->via, &ok);
                if (transform.type() >= QTransform::TxShear || !ok) {
                    qmlInfo(this) << QSGParentAnimation::tr("Unable to preserve appearance under complex transform");
                    ok = false;
                }

                qreal scale = 1;
                qreal rotation = 0;
                bool isRotate = (transform.type() == QTransform::TxRotate) || (transform.m11() < 0);
                if (ok && !isRotate) {
                    if (transform.m11() == transform.m22())
                        scale = transform.m11();
                    else {
                        qmlInfo(this) << QSGParentAnimation::tr("Unable to preserve appearance under non-uniform scale");
                        ok = false;
                    }
                } else if (ok && isRotate) {
                    if (transform.m11() == transform.m22())
                        scale = qSqrt(transform.m11()*transform.m11() + transform.m12()*transform.m12());
                    else {
                        qmlInfo(this) << QSGParentAnimation::tr("Unable to preserve appearance under non-uniform scale");
                        ok = false;
                    }

                    if (scale != 0)
                        rotation = atan2(transform.m12()/scale, transform.m11()/scale) * 180/M_PI;
                    else {
                        qmlInfo(this) << QSGParentAnimation::tr("Unable to preserve appearance under scale of 0");
                        ok = false;
                    }
                }

                const QPointF &point = transform.map(QPointF(xAction.toValue.toReal(),yAction.toValue.toReal()));
                qreal x = point.x();
                qreal y = point.y();
                if (ok && target->transformOrigin() != QSGItem::TopLeft) {
                    qreal w = target->width();
                    qreal h = target->height();
                    if (pc->widthIsSet() && i < actions.size() - 1)
                        w = actions[++i].toValue.toReal();
                    if (pc->heightIsSet() && i < actions.size() - 1)
                        h = actions[++i].toValue.toReal();
                    const QPointF &transformOrigin
                            = d->computeTransformOrigin(target->transformOrigin(), w,h);
                    qreal tempxt = transformOrigin.x();
                    qreal tempyt = transformOrigin.y();
                    QTransform t;
                    t.translate(-tempxt, -tempyt);
                    t.rotate(rotation);
                    t.scale(scale, scale);
                    t.translate(tempxt, tempyt);
                    const QPointF &offset = t.map(QPointF(0,0));
                    x += offset.x();
                    y += offset.y();
                }

                if (ok) {
                    //qDebug() << x << y << rotation << scale;
                    xAction.toValue = x;
                    yAction.toValue = y;
                    sAction.toValue = sAction.toValue.toReal() * scale;
                    rAction.toValue = rAction.toValue.toReal() + rotation;
                }
            }
        }
    }

    if (data->actions.count()) {
        if (direction == QDeclarativeAbstractAnimation::Forward) {
            d->startAction->setAnimAction(d->via ? viaData : data, QActionAnimation::DeleteWhenStopped);
            d->endAction->setAnimAction(d->via ? data : 0, QActionAnimation::DeleteWhenStopped);
        } else {
            d->endAction->setAnimAction(d->via ? viaData : data, QActionAnimation::DeleteWhenStopped);
            d->startAction->setAnimAction(d->via ? data : 0, QActionAnimation::DeleteWhenStopped);
        }
    } else {
        delete data;
        delete viaData;
    }

    //take care of any child animations
    bool valid = d->defaultProperty.isValid();
    for (int ii = 0; ii < d->animations.count(); ++ii) {
        if (valid)
            d->animations.at(ii)->setDefaultTarget(d->defaultProperty);
        d->animations.at(ii)->transition(actions, modified, direction);
    }

}

QAbstractAnimation *QSGParentAnimation::qtAnimation()
{
    Q_D(QSGParentAnimation);
    return d->topLevelGroup;
}

QSGAnchorAnimation::QSGAnchorAnimation(QObject *parent)
: QDeclarativeAbstractAnimation(*(new QSGAnchorAnimationPrivate), parent)
{
    Q_D(QSGAnchorAnimation);
    d->va = new QDeclarativeBulkValueAnimator;
    QDeclarative_setParent_noEvent(d->va, this);
}

QSGAnchorAnimation::~QSGAnchorAnimation()
{
}

QAbstractAnimation *QSGAnchorAnimation::qtAnimation()
{
    Q_D(QSGAnchorAnimation);
    return d->va;
}

QDeclarativeListProperty<QSGItem> QSGAnchorAnimation::targets()
{
    Q_D(QSGAnchorAnimation);
    return QDeclarativeListProperty<QSGItem>(this, d->targets);
}

int QSGAnchorAnimation::duration() const
{
    Q_D(const QSGAnchorAnimation);
    return d->va->duration();
}

void QSGAnchorAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qmlInfo(this) << tr("Cannot set a duration of < 0");
        return;
    }

    Q_D(QSGAnchorAnimation);
    if (d->va->duration() == duration)
        return;
    d->va->setDuration(duration);
    emit durationChanged(duration);
}

QEasingCurve QSGAnchorAnimation::easing() const
{
    Q_D(const QSGAnchorAnimation);
    return d->va->easingCurve();
}

void QSGAnchorAnimation::setEasing(const QEasingCurve &e)
{
    Q_D(QSGAnchorAnimation);
    if (d->va->easingCurve() == e)
        return;

    d->va->setEasingCurve(e);
    emit easingChanged(e);
}

void QSGAnchorAnimation::transition(QDeclarativeStateActions &actions,
                        QDeclarativeProperties &modified,
                        TransitionDirection direction)
{
    Q_UNUSED(modified);
    Q_D(QSGAnchorAnimation);
    QDeclarativeAnimationPropertyUpdater *data = new QDeclarativeAnimationPropertyUpdater;
    data->interpolatorType = QMetaType::QReal;
    data->interpolator = d->interpolator;

    data->reverse = direction == Backward ? true : false;
    data->fromSourced = false;
    data->fromDefined = false;

    for (int ii = 0; ii < actions.count(); ++ii) {
        QDeclarativeAction &action = actions[ii];
        if (action.event && action.event->typeName() == QLatin1String("AnchorChanges")
            && (d->targets.isEmpty() || d->targets.contains(static_cast<QSGAnchorChanges*>(action.event)->object()))) {
            data->actions << static_cast<QSGAnchorChanges*>(action.event)->additionalActions();
        }
    }

    if (data->actions.count()) {
        if (!d->rangeIsSet) {
            d->va->setStartValue(qreal(0));
            d->va->setEndValue(qreal(1));
            d->rangeIsSet = true;
        }
        d->va->setAnimValue(data, QAbstractAnimation::DeleteWhenStopped);
        d->va->setFromSourcedValue(&data->fromSourced);
    } else {
        delete data;
    }
}

QT_END_NAMESPACE
