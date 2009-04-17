/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/



#include "qvalueanimation.h"
#include "qvalueanimation_p.h"

QT_BEGIN_NAMESPACE


void QValueAnimationPrivate::initDefaultStartValue()
{
    Q_Q(QValueAnimation);
    if (animProp && !q->startValue().isValid()
        && ((currentTime == 0 && (currentIteration || currentIteration == 0))
        || (currentTime == duration && currentIteration == (iterationCount - 1)))) {
            setDefaultStartValue(animProp->read());
    }
}


QValueAnimation::QValueAnimation(QObject *parent) : QVariantAnimation(*new QValueAnimationPrivate, parent)
{
}

QValueAnimation::~QValueAnimation()
{
}

void QValueAnimation::setProperty(AbstractProperty *animProp)
{
    Q_D(QValueAnimation);
    d->animProp = animProp;
}

/*!
    \reimp
 */
void QValueAnimation::updateCurrentValue(const QVariant &value)
{
    Q_D(QValueAnimation);
    if (state() == QAbstractAnimation::Stopped)
        return;

    d->animProp->write(value);
}


/*!
    \reimp
*/
void QValueAnimation::updateState(QAbstractAnimation::State oldState, QAbstractAnimation::State newState)
{
    Q_D(QValueAnimation);
    // Initialize start value
    if (oldState == QAbstractAnimation::Stopped && newState == QAbstractAnimation::Running)
        d->initDefaultStartValue();
}



#include "moc_qvalueanimation.cpp"

QT_END_NAMESPACE
