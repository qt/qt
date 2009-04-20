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

#ifndef QT_NO_ANIMATION

#include "qvariantanimation.h"
#include "qvariantanimation_p.h"

#include <QtCore/QRect>
#include <QtCore/QLineF>
#include <QtCore/QLine>
#include <QtCore/QReadWriteLock>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QVariantAnimation
    \ingroup animation
    \brief The QVariantAnimation class provides an abstract base class for animations.
    \since 4.5
    \preliminary

    This class is part of {The Animation Framework}. It serves as a base class
    for property and item animations, with functions for shared functionality.

    If you want to create an animation, you should look at QPropertyAnimation instead.

    You can then set start and end values for the property by calling
    setStartValue() and setEndValue(), and finally call start() to
    start the animation. When control goes back to the event loop, QVariantAnimation
    will interpolate the property of the target object and emit the valueChanged
    signal. To react to a change in the current value you have to reimplement the
    updateCurrentValue virtual method.

    There are two ways to affect how QVariantAnimation interpolates the values.
    You can set an easing curve by calling setEasingCurve(), and configure the
    duration by calling setDuration(). You can change how the QVariants are
    interpolated by creating a subclass of QVariantAnimation, and reimplementing the
    virtual interpolated() function.


    \sa QPropertyAnimation, {The Animation Framework}
*/

/*!
    \fn void QVariantAnimation::valueChanged(const QVariant &value)

    QVariantAnimation emits this signal whenever the current \a value changes.

    \sa currentValue, startValue, endValue
*/

static bool animationValueLessThan(const QVariantAnimation::KeyValue &p1, const QVariantAnimation::KeyValue &p2)
{
    return p1.first < p2.first;
}

template<> Q_INLINE_TEMPLATE QRect _q_interpolate(const QRect &f, const QRect &t, qreal progress)
{
    QRect ret;
    ret.setCoords(_q_interpolate(f.left(), t.left(), progress),
                  _q_interpolate(f.top(), t.top(), progress),
                  _q_interpolate(f.right(), t.right(), progress),
                  _q_interpolate(f.bottom(), t.bottom(), progress));
    return ret;
}

template<> Q_INLINE_TEMPLATE QRectF _q_interpolate(const QRectF &f, const QRectF &t, qreal progress)
{
    qreal x1, y1, w1, h1;
    f.getRect(&x1, &y1, &w1, &h1);
    qreal x2, y2, w2, h2;
    t.getRect(&x2, &y2, &w2, &h2);
    return QRectF( _q_interpolate(x1, x2, progress), _q_interpolate(y1, y2, progress),
                  _q_interpolate(w1, w2, progress), _q_interpolate(h1, h2, progress));
}

template<> Q_INLINE_TEMPLATE QLine _q_interpolate(const QLine &f, const QLine &t, qreal progress)
{
    return QLine( _q_interpolate(f.p1(), t.p1(), progress), _q_interpolate(f.p2(), t.p2(), progress));
}

template<> Q_INLINE_TEMPLATE QLineF _q_interpolate(const QLineF &f, const QLineF &t, qreal progress)
{
    return QLineF( _q_interpolate(f.p1(), t.p1(), progress), _q_interpolate(f.p2(), t.p2(), progress));
}

void QVariantAnimationPrivate::convertValues(int t)
{
    //this ensures that all the keyValues are of type t
    for (int i = 0; i < keyValues.count(); ++i) {
        QVariantAnimation::KeyValue &pair = keyValues[i];
        if (pair.second.userType() != t)
            pair.second.convert(static_cast<QVariant::Type>(t));
    }
    currentInterval.start.first = 2; // this will force the refresh
    interpolator = 0;               // if the type changed we need to update the interpolator
}

/*!
    \fn void QVariantAnimation::updateCurrentValue(const QVariant &value) = 0;
    This pure virtual function is called when the animated value is changed.
    \a value is the new value.
*/

void QVariantAnimationPrivate::updateCurrentValue()
{
    Q_Q(QVariantAnimation);
    const qreal progress = easing.valueForProgress(((duration == 0) ? qreal(1) : qreal(currentTime) / qreal(duration)));

    if (progress < currentInterval.start.first || progress > currentInterval.end.first) {
        //let's update currentInterval
        QVariantAnimation::KeyValues::const_iterator itStart = qLowerBound(keyValues.constBegin(), keyValues.constEnd(), qMakePair(progress, QVariant()), animationValueLessThan);
        QVariantAnimation::KeyValues::const_iterator itEnd = itStart;

        // If we are at the end we should continue to use the last keyValues in case of extrapolation (progress > 1.0).
        // This is because the easing function can return a value slightly outside the range [0, 1]
        if (itStart != keyValues.constEnd()) {

            //this can't happen because we always prepend the default start value there
            if (itStart == keyValues.begin()) {
                ++itEnd;
                if (itEnd == keyValues.constEnd())
                    return; //there is no upper bound
            } else {
                --itStart;
            }

            //update all the values of the currentInterval
            currentInterval.start = *itStart;
            currentInterval.end = *itEnd;
        }
    }

    const qreal startProgress = currentInterval.start.first,
                endProgress = currentInterval.end.first;
    const qreal localProgress = (progress - startProgress) / (endProgress - startProgress);

    bool changed = false;
    {
      //we do that here in a limited scope so that ret is dereferenced and frees memory
      //and the call to updateCurrentValue can recreate QVariant of the same type (for ex. in
      //QGraphicsItem::setPos
      QVariant ret = q->interpolated(currentInterval.start.second,
                                    currentInterval.end.second,
                                    localProgress);
      if (currentValue != ret) {
          changed = true;
          qSwap(currentValue, ret);
      }
    }

    if (changed) {
        //the value has changed
        q->updateCurrentValue(currentValue);
#ifndef QT_EXPERIMENTAL_SOLUTION
        if (connectedSignals & changedSignalMask)
#endif
            emit q->valueChanged(currentValue);
    }
}


QVariant QVariantAnimationPrivate::valueAt(qreal step) const
{
    QVariantAnimation::KeyValues::const_iterator result =
        qBinaryFind(keyValues.begin(), keyValues.end(), qMakePair(step, QVariant()), animationValueLessThan);
    if (result != keyValues.constEnd())
        return result->second;

    return QVariant();
}


void QVariantAnimationPrivate::setValueAt(qreal step, const QVariant &value)
{
    if (step < qreal(0.0) || step > qreal(1.0)) {
        qWarning("QVariantAnimation::setValueAt: invalid step = %f", step);
        return;
    }

    QVariantAnimation::KeyValue pair(step, value);

    QVariantAnimation::KeyValues::iterator result = qLowerBound(keyValues.begin(), keyValues.end(), pair, animationValueLessThan);
    if (result == keyValues.end() || result->first != step) {
        keyValues.insert(result, pair);
    } else {
        if (value.isValid())
            result->second = value; //remove the previous value
        else if (step == 0 && !hasStartValue && defaultStartValue.isValid())
            result->second = defaultStartValue; //we reset to the default start value
        else
            keyValues.erase(result); //replace the previous value
    }

    currentInterval.start.first = 2; // this will force the refresh
    updateCurrentValue();
}

void QVariantAnimationPrivate::setDefaultStartValue(const QVariant &value)
{
    defaultStartValue = value;
    if (!hasStartValue)
      setValueAt(0, value);
}

/*!
    Construct a QVariantAnimation object. \a parent is passed to QAbstractAnimation's
    constructor.
*/
QVariantAnimation::QVariantAnimation(QObject *parent) : QAbstractAnimation(*new QVariantAnimationPrivate, parent)
{
   d_func()->init();
}

/*!
    \internal
*/
QVariantAnimation::QVariantAnimation(QVariantAnimationPrivate &dd, QObject *parent) : QAbstractAnimation(dd, parent)
{
   d_func()->init();
}

/*!
    Destroys the animation.
*/
QVariantAnimation::~QVariantAnimation()
{
}

/*!
    \property QVariantAnimation::easingCurve
    \brief the easing curve of the animation

    This property defines the easing curve of the animation. By default, a
    linear easing curve is used, resulting in linear interpolation of the
    end property. For many animations, it's useful to try different easing
    curves, including QEasingCurve::InCirc, which provides a circular entry curve,
    and QEasingCurve::InOutElastic, which provides an elastic effect on the values
    of the interpolated property.

    The easing curve is used with the interpolator, the interpolated() virtual
    function, the animation's duration, and loopCount, to control how the
    current value changes as the animation progresses.
*/
QEasingCurve QVariantAnimation::easingCurve() const
{
    Q_D(const QVariantAnimation);
    return d->easing;
}

void QVariantAnimation::setEasingCurve(const QEasingCurve &easing)
{
    Q_D(QVariantAnimation);
    d->easing = easing;
    d->updateCurrentValue();
}

Q_GLOBAL_STATIC(QVector<QVariantAnimation::Interpolator>, registeredInterpolators)
Q_GLOBAL_STATIC(QReadWriteLock, registeredInterpolatorsLock)

/*!
    \fn void qRegisterAnimationInterpolator(QVariant (*func)(const T &from, const T &to, qreal progress))
    \relates QVariantAnimation
    \threadsafe

    Registers a custom interpolator \a func for the template type \c{T}.
    The interpolator has to be registered before the animation is constructed.
    To unregister (and use the default interpolator) set \a func to 0.
 */

/*!
    \internal
    \typedef QVariantAnimation::Interpolator

    This is a typedef for a pointer to a function with the following
    signature:
    \code
    QVariant myInterpolator(const QVariant &from, const QVariant &to, qreal progress);
    \endcode

*/

/*! \internal
 * Registers a custom interpolator \a func for the specific \a interpolationType.
 * The interpolator has to be registered before the animation is constructed.
 * To unregister (and use the default interpolator) set \a func to 0.
 */
void QVariantAnimation::registerInterpolator(QVariantAnimation::Interpolator func, int interpolationType)
{
    // will override any existing interpolators
    QWriteLocker locker(registeredInterpolatorsLock());
    if (int(interpolationType) >= registeredInterpolators()->count())
        registeredInterpolators()->resize(int(interpolationType) + 1);
    registeredInterpolators()->replace(interpolationType, func);
}


template<typename T> static inline QVariantAnimation::Interpolator castToInterpolator(QVariant (*func)(const T &from, const T &to, qreal progress))
{
     return reinterpret_cast<QVariantAnimation::Interpolator>(func);
}

static QVariantAnimation::Interpolator getInterpolator(int interpolationType)
{
    QReadLocker locker(registeredInterpolatorsLock());
    QVariantAnimation::Interpolator ret = 0;
    if (interpolationType < registeredInterpolators()->count()) {
        ret = registeredInterpolators()->at(interpolationType);
        if (ret) return ret;
    }

    switch(interpolationType)
    {
    case QMetaType::Int:
        return castToInterpolator(_q_interpolateVariant<int>);
    case QMetaType::Double:
        return castToInterpolator(_q_interpolateVariant<double>);
    case QMetaType::Float:
        return castToInterpolator(_q_interpolateVariant<float>);
    case QMetaType::QLine:
        return castToInterpolator(_q_interpolateVariant<QLine>);
    case QMetaType::QLineF:
        return castToInterpolator(_q_interpolateVariant<QLineF>);
    case QMetaType::QPoint:
        return castToInterpolator(_q_interpolateVariant<QPoint>);
    case QMetaType::QPointF:
        return castToInterpolator(_q_interpolateVariant<QPointF>);
    case QMetaType::QSize:
        return castToInterpolator(_q_interpolateVariant<QSize>);
    case QMetaType::QSizeF:
        return castToInterpolator(_q_interpolateVariant<QSizeF>);
    case QMetaType::QRect:
        return castToInterpolator(_q_interpolateVariant<QRect>);
    case QMetaType::QRectF:
        return castToInterpolator(_q_interpolateVariant<QRectF>);
    default:
        return 0; //this type is not handled
    }
}

/*!
    \property QVariantAnimation::duration
    \brief the duration of the animation

    This property describes the duration of the animation. The default
    duration is 250 milliseconds.

    \sa QAbstractAnimation::duration()
 */
int QVariantAnimation::duration() const
{
    Q_D(const QVariantAnimation);
    return d->duration;
}

void QVariantAnimation::setDuration(int msecs)
{
    Q_D(QVariantAnimation);
    if (msecs < 0) {
        qWarning("QVariantAnimation::setDuration: cannot set a negative duration");
        return;
    }
    if (d->duration == msecs)
        return;
    d->duration = msecs;
    d->updateCurrentValue();
}

/*!
    \property QVariantAnimation::startValue
    \brief the optional start value of the animation

    This property describes the optional start value of the animation. If
    omitted, or if a null QVariant is assigned as the start value, the
    animation will use the current position of the end when the animation
    is started.

    \sa endValue
*/
QVariant QVariantAnimation::startValue() const
{
    return keyValueAt(0);
}

void QVariantAnimation::setStartValue(const QVariant &value)
{
    setKeyValueAt(0, value);
}

/*!
    \property QVariantAnimation::endValue
    \brief the end value of the animation

    This property describes the end value of the animation.

    \sa startValue
 */
QVariant QVariantAnimation::endValue() const
{
    return keyValueAt(1);
}

void QVariantAnimation::setEndValue(const QVariant &value)
{
    setKeyValueAt(1, value);
}


/*!
    Returns the key frame value for the given \a step. The given \a step
    must be in the range 0 to 1. If there is no KeyValue for \a step,
    it returns an invalid QVariant.

    \sa keyValues(), setKeyValueAt()
*/
QVariant QVariantAnimation::keyValueAt(qreal step) const
{
    Q_D(const QVariantAnimation);
    if (step == 0 && !d->hasStartValue)
        return QVariant(); //special case where we don't have an explicit startValue

    return d->valueAt(step);
}

/*!
    \typedef QVariantAnimation::KeyValue

    This is a typedef for QPair<qreal, QVariant>.
*/
/*!
    \typedef QVariantAnimation::KeyValues

    This is a typedef for QVector<KeyValue>
*/

/*!
    Creates a key frame at the given \a step with the given \a value.
    The given \a step must be in the range 0 to 1.

    \sa setKeyValues(), keyValueAt()
*/
void QVariantAnimation::setKeyValueAt(qreal step, const QVariant &value)
{
    Q_D(QVariantAnimation);
    if (step == 0)
        d->hasStartValue = value.isValid();
    d->setValueAt(step, value);
}

/*!
    Returns the key frames of this animation.

    \sa keyValueAt(), setKeyValues()
*/
QVariantAnimation::KeyValues QVariantAnimation::keyValues() const
{
    Q_D(const QVariantAnimation);
    QVariantAnimation::KeyValues ret = d->keyValues;
    //in case we added the default start value, we remove it
    if (!d->hasStartValue && !ret.isEmpty() && ret.at(0).first == 0)
        ret.remove(0);
    return ret;
}

/*!
    Replaces the current set of key frames with the given \a keyValues.
    the step of the key frames must be in the range 0 to 1.

    \sa keyValues(), keyValueAt()
*/
void QVariantAnimation::setKeyValues(const KeyValues &keyValues)
{
    Q_D(QVariantAnimation);
    d->keyValues = keyValues;
    qSort(d->keyValues.begin(), d->keyValues.end(), animationValueLessThan);
    d->currentInterval.start.first = 2; // this will force the refresh
    d->hasStartValue = !d->keyValues.isEmpty() && d->keyValues.at(0).first == 0;
}

/*!
    \property QVariantAnimation::currentValue
    \brief the current value of the animation

    This property describes the current value; an interpolation between the
    start value and the end value, using the current time for progress.

    QVariantAnimation calls the virtual updateCurrentValue() function when the
    current value changes. This is particularily useful for subclasses that
    need to track updates.

    \sa startValue, endValue
 */
QVariant QVariantAnimation::currentValue() const
{
    Q_D(const QVariantAnimation);
    if (!d->currentValue.isValid())
        const_cast<QVariantAnimationPrivate*>(d)->updateCurrentValue();
    return d->currentValue;
}

/*!
    \reimp
 */
bool QVariantAnimation::event(QEvent *event)
{
    return QAbstractAnimation::event(event);
}

/*!
    \reimp
*/
void QVariantAnimation::updateState(QAbstractAnimation::State oldState,
                             QAbstractAnimation::State newState)
{
    Q_UNUSED(oldState);
    Q_UNUSED(newState);
    Q_D(QVariantAnimation);
    d->currentValue = QVariant(); // this will force the refresh
}

/*!
    This virtual function returns the linear interpolation between variants \a
    from and \a to, at \a progress, usually a value between 0 and 1. You can reimplement
    this function in a subclass of QVariantAnimation to provide your own interpolation
    algorithm. Note that in order for the interpolation to work with a QEasingCurve
    that return a value smaller than 0 or larger than 1 (such as QEasingCurve::InBack)
    you should make sure that it can extrapolate. If the semantic of the datatype
    does not allow extrapolation this function should handle that gracefully.

    \sa QEasingCurve
 */
QVariant QVariantAnimation::interpolated(const QVariant &from, const QVariant &to, qreal progress) const
{
    Q_D(const QVariantAnimation);
    if (d->interpolator == 0) {
        if (from.userType() == to.userType())
            d->interpolator = getInterpolator(from.userType());
        if (d->interpolator == 0) //no interpolator found
            return QVariant();
    }

    return d->interpolator(from.constData(), to.constData(), progress);
}

/*!
    \reimp
 */
void QVariantAnimation::updateCurrentTime(int msecs)
{
    Q_D(QVariantAnimation);
    Q_UNUSED(msecs);
    d->updateCurrentValue();
}

QT_END_NAMESPACE

#include "moc_qvariantanimation.cpp"

#endif //QT_NO_ANIMATION
