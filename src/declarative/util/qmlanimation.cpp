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

#include "qmlanimation.h"
#include "qvariant.h"
#include "qcolor.h"
#include "qfile.h"
#include "qmlpropertyvaluesource.h"
#include "qml.h"
#include "qmlanimation_p.h"
#include "qmlbehaviour.h"
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QtCore/qset.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtDeclarative/qmlexpression.h>
#include <private/qmlstringconverters_p.h>
#include <private/qvariantanimation_p.h>

/* TODO:
    Check for any memory leaks
    easing should be a QEasingCurve-type property
    All other XXXs
*/

QT_BEGIN_NAMESPACE

QEasingCurve stringToCurve(const QString &curve)
{
    QEasingCurve easingCurve;

    QString normalizedCurve = curve;
    bool hasParams = curve.contains(QLatin1Char('('));
    QStringList props;

    if (hasParams) {
        QString easeName = curve.trimmed();
        if (!easeName.endsWith(QLatin1Char(')'))) {
            qWarning("QEasingCurve: Unmatched perenthesis in easing function '%s'",
                     curve.toLatin1().constData());
            return easingCurve;
        }

        int idx = easeName.indexOf(QLatin1Char('('));
        QString prop_str =
            easeName.mid(idx + 1, easeName.length() - 1 - idx - 1);
        normalizedCurve = easeName.left(idx);

        props = prop_str.split(QLatin1Char(','));
    }

    normalizedCurve = normalizedCurve.mid(4);
    //XXX optimize?
    int index = QEasingCurve::staticMetaObject.indexOfEnumerator("Type");
    QMetaEnum me = QEasingCurve::staticMetaObject.enumerator(index);

    int value = me.keyToValue(normalizedCurve.toLatin1().constData());
    if (value < 0) {
        //XXX print line number
        qWarning("QEasingCurve: Unknown easing curve '%s'",
                 curve.toLatin1().constData());
        value = 0;
    }
    easingCurve.setType((QEasingCurve::Type)value);

    if (hasParams) {
        foreach(const QString &str, props) {
            int sep = str.indexOf(QLatin1Char(':'));

            if (sep == -1) {
                qWarning("QEasingCurve: Improperly specified property in easing function '%s'",
                        curve.toLatin1().constData());
                return easingCurve;
            }

            QString propName = str.left(sep).trimmed();
            bool isOk;
            qreal propValue = str.mid(sep + 1).trimmed().toDouble(&isOk);

            if (propName.isEmpty() || !isOk) {
                qWarning("QEasingCurve: Improperly specified property in easing function '%s'",
                        curve.toLatin1().constData());
                return easingCurve;
            }

            //XXX optimize
            if (propName == QLatin1String("amplitude")) {
                easingCurve.setAmplitude(propValue);
            } else if (propName == QLatin1String("period")) {
                easingCurve.setPeriod(propValue);
            } else if (propName == QLatin1String("overshoot")) {
                easingCurve.setOvershoot(propValue);
            }
        }
    }

    return easingCurve;
}

QML_DEFINE_NOCREATE_TYPE(QmlAbstractAnimation)

/*!
    \qmlclass Animation
    \brief The Animation element is the base of all QML animations.

    The Animation element cannot be used directly in a QML file.  It exists
    to provide a set of common properties and methods, available across all the
    other animation types that inherit from it.  Attempting to use the Animation
    element directly will result in an error.
*/

/*!
    \class QmlAbstractAnimation
    \internal
*/

QmlAbstractAnimation::QmlAbstractAnimation(QObject *parent)
: QmlPropertyValueSource(*(new QmlAbstractAnimationPrivate), parent)
{
}

QmlAbstractAnimation::~QmlAbstractAnimation()
{
}

QmlAbstractAnimation::QmlAbstractAnimation(QmlAbstractAnimationPrivate &dd, QObject *parent)
: QmlPropertyValueSource(dd, parent)
{
}

/*!
    \qmlproperty bool Animation::running
    This property holds whether the animation is currently running.

    The \c running property can be set to declaratively control whether or not
    an animation is running.  The following example will animate a rectangle
    whenever the \l MouseRegion is pressed.

    \code
    Rect {
        width: 100; height: 100
        x: NumericAnimation {
            running: MyMouse.pressed
            from: 0; to: 100
        }
        MouseRegion { id: MyMouse }
    }
    \endcode

    Likewise, the \c running property can be read to determine if the animation
    is running.  In the following example the text element will indicate whether
    or not the animation is running.

    \code
    NumericAnimation { id: MyAnimation }
    Text { text: MyAnimation.running ? "Animation is running" : "Animation is not running" }
    \endcode

    Animations can also be started and stopped imperatively from JavaScript
    using the \c start() and \c stop() methods.

    By default, animations are not running.
*/
bool QmlAbstractAnimation::isRunning() const
{
    Q_D(const QmlAbstractAnimation);
    return d->running;
}

void QmlAbstractAnimationPrivate::commence()
{
    Q_Q(QmlAbstractAnimation);

    q->prepare(userProperty.value);
    q->qtAnimation()->start();
    if (q->qtAnimation()->state() != QAbstractAnimation::Running) {
        running = false;
        emit q->completed();
    }
}

void QmlAbstractAnimation::setRunning(bool r)
{
    Q_D(QmlAbstractAnimation);
    if (d->running == r)
        return;

    if (d->group) {
        qWarning("QmlAbstractAnimation: setRunning() cannot be used on non-root animation nodes");
        return;
    }

    d->running = r;
    if (d->running) {
        if (!d->connectedTimeLine) {
            QObject::connect(qtAnimation(), SIGNAL(finished()),
                             this, SLOT(timelineComplete()));
            d->connectedTimeLine = true;
        }
        if (d->componentComplete)
            d->commence();
        else
            d->startOnCompletion = true;
        emit started();
    } else {
        if (d->finishPlaying) {
            if (d->repeat)
                qtAnimation()->setLoopCount(qtAnimation()->currentLoop()+1);
        } else
            qtAnimation()->stop();

        emit completed();
    }

    emit runningChanged(d->running);
}

/*!
    \qmlproperty bool Animation::paused
    This property holds whether the animation is currently paused.

    The \c paused property can be set to declaratively control whether or not
    an animation is paused.

    Animations can also be paused and resumed imperatively from JavaScript
    using the \c pause() and \c resume() methods.

    By default, animations are not paused.
*/
bool QmlAbstractAnimation::isPaused() const
{
    Q_D(const QmlAbstractAnimation);
    return d->paused;
}

void QmlAbstractAnimation::setPaused(bool p)
{
    Q_D(QmlAbstractAnimation);
    if (d->paused == p)
        return;

    if (d->group) {
        qWarning("QmlAbstractAnimation: setPaused() cannot be used on non-root animation nodes");
        return;
    }

    d->paused = p;
    if (d->paused)
        qtAnimation()->pause();
    else
        qtAnimation()->resume();

    emit pausedChanged(d->running);
}

void QmlAbstractAnimation::classBegin()
{
    Q_D(QmlAbstractAnimation);
    d->componentComplete = false;
}

void QmlAbstractAnimation::componentComplete()
{
    Q_D(QmlAbstractAnimation);
    if (d->startOnCompletion)
        d->commence();
    d->componentComplete = true;
}

/*!
    \qmlproperty bool Animation::finishPlaying
    This property holds whether the animation should finish playing when it is stopped.

    If this true the animation will complete its current iteration when it
    is stopped - either by setting the \c running property to false, or by
    calling the \c stop() method.  The \c complete() method is not effected
    by this value.

    This behaviour is most useful when the \c repeat property is set, as the
    animation will finish playing normally but not restart.

    By default, the finishPlaying property is not set.
*/
bool QmlAbstractAnimation::finishPlaying() const
{
    Q_D(const QmlAbstractAnimation);
    return d->finishPlaying;
}

void QmlAbstractAnimation::setFinishPlaying(bool f)
{
    Q_D(QmlAbstractAnimation);
    if (d->finishPlaying == f)
        return;

    d->finishPlaying = f;
    emit finishPlayingChanged(f);
}

/*!
    \qmlproperty bool Animation::repeat
    This property holds whether the animation should repeat.

    If set, the animation will continuously repeat until it is explicitly
    stopped - either by setting the \c running property to false, or by calling
    the \c stop() method.

    In the following example, the rectangle will spin indefinately.

    \code
    Rect {
        rotation: NumericAnimation { running: true; repeat: true; from: 0 to: 360 }
    }
    \endcode
*/
bool QmlAbstractAnimation::repeat() const
{
    Q_D(const QmlAbstractAnimation);
    return d->repeat;
}

void QmlAbstractAnimation::setRepeat(bool r)
{
    Q_D(QmlAbstractAnimation);
    if (r == d->repeat)
        return;

    d->repeat = r;
    int lc = r ? -1 : 1;
    qtAnimation()->setLoopCount(lc);
    emit repeatChanged(r);
}

QmlAnimationGroup *QmlAbstractAnimation::group() const
{
    Q_D(const QmlAbstractAnimation);
    return d->group;
}

void QmlAbstractAnimation::setGroup(QmlAnimationGroup *g)
{
    Q_D(QmlAbstractAnimation);
    if (d->group == g)
        return;
    if (d->group)
        static_cast<QmlAnimationGroupPrivate *>(d->group->d_ptr)->animations.removeAll(this);

    d->group = g;

    if (d->group && !static_cast<QmlAnimationGroupPrivate *>(d->group->d_ptr)->animations.contains(this))
        static_cast<QmlAnimationGroupPrivate *>(d->group->d_ptr)->animations.append(this);

    if (d->group)
        ((QAnimationGroup*)d->group->qtAnimation())->addAnimation(qtAnimation());

    //if (g) //if removed from a group, then the group should no longer be the parent
        setParent(g);
}

/*!
    \qmlproperty Object Animation::target
    This property holds an explicit target object to animate.

    The exact effect of the \c target property depends on how the animation
    is being used.  Refer to the \l animation documentation for details.
*/
QObject *QmlAbstractAnimation::target() const
{
    Q_D(const QmlAbstractAnimation);
    return d->target;
}

void QmlAbstractAnimation::setTarget(QObject *o)
{
    Q_D(QmlAbstractAnimation);
    if (d->target == o)
        return;

    d->target = o;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = QmlMetaProperty(d->target, d->propertyName);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlproperty string Animation::property
    This property holds an explicit property to animated.

    The exact effect of the \c property property depends on how the animation
    is being used.  Refer to the \l animation documentation for details.
*/
QString QmlAbstractAnimation::property() const
{
    Q_D(const QmlAbstractAnimation);
    return d->propertyName;
}

void QmlAbstractAnimation::setProperty(const QString &n)
{
    Q_D(QmlAbstractAnimation);
    if (d->propertyName == n)
        return;

    d->propertyName = n;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = QmlMetaProperty(d->target, d->propertyName);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlmethod Animation::start()
    \brief Starts the animation.

    If the animation is already running, calling this method has no effect.  The
    \c running property will be true following a call to \c start().
*/
void QmlAbstractAnimation::start()
{
    setRunning(true);
}

/*!
    \qmlmethod Animation::pause()
    \brief Pauses the animation.

    If the animation is already paused, calling this method has no effect.  The
    \c paused property will be true following a call to \c pause().
*/
void QmlAbstractAnimation::pause()
{
    setPaused(true);
}

/*!
    \qmlmethod Animation::resume()
    \brief Resumes a paused animation.

    If the animation is not paused, calling this method has no effect.  The
    \c paused property will be false following a call to \c resume().
*/
void QmlAbstractAnimation::resume()
{
    setPaused(false);
}

/*!
    \qmlmethod Animation::stop()
    \brief Stops the animation.

    If the animation is not running, calling this method has no effect.  The
    \c running property will be false following a call to \c stop().

    Normally \c stop() stops the animation immediately, and the animation has
    no further influence on property values.  In this example animation
    \code
    Rect {
        x: NumericAnimation { from: 0; to: 100; duration: 500 }
    }
    \endcode
    was stopped at time 250ms, the \c x property will have a value of 50.

    However, if the \c finishPlaying property is set, the animation will
    continue running until it completes and then stop.  The \c running property
    will still become false immediately.
*/
void QmlAbstractAnimation::stop()
{
    setRunning(false);
}

/*!
    \qmlmethod Animation::restart()
    \brief Restarts the animation.

    This is a convenience method, and is equivalent to calling \c stop() and
    then \c start().
*/
void QmlAbstractAnimation::restart()
{
    stop();
    start();
}

/*!
    \qmlmethod Animation::complete()
    \brief Stops the animation, jumping to the final property values.

    If the animation is not running, calling this method has no effect.  The
    \c running property will be false following a call to \c complete().

    Unlike \c stop(), \c complete() immediately fast-forwards the animation to
    its end.  In the following example,
    \code
    Rect {
        x: NumericAnimation { from: 0; to: 100; duration: 500 }
    }
    \endcode
    calling \c stop() at time 250ms will result in the \c x property having
    a value of 50, while calling \c complete() will set the \c x property to
    100, exactly as though the animation had played the whole way through.
*/
void QmlAbstractAnimation::complete()
{
    if (isRunning()) {
         qtAnimation()->setCurrentTime(qtAnimation()->duration());
    }
}

void QmlAbstractAnimation::setTarget(const QmlMetaProperty &p)
{
    Q_D(QmlAbstractAnimation);
    if (d->userProperty.isNull)
        d->userProperty = p;
}

//prepare is called before an animation begins
//(when an animation is used as a simple animation, and not as part of a transition)
void QmlAbstractAnimation::prepare(QmlMetaProperty &)
{
}

void QmlAbstractAnimation::transition(QmlStateActions &actions,
                                      QmlMetaProperties &modified,
                                      TransitionDirection direction)
{
    Q_UNUSED(actions);
    Q_UNUSED(modified);
    Q_UNUSED(direction);
}

void QmlAbstractAnimation::timelineComplete()
{
    Q_D(QmlAbstractAnimation);
    setRunning(false);
    if (d->finishPlaying && d->repeat) {
        qtAnimation()->setLoopCount(-1);
    }
}

/*!
    \qmlclass PauseAnimation QmlPauseAnimation
    \inherits Animation
    \brief The PauseAnimation provides a pause for an animation.

    When used in a SequentialAnimation, PauseAnimation is a step when
    nothing happens, for a specified duration.

    A 500ms animation sequence, with a 100ms pause between two animations:
    \code
    SequentialAnimation {
        NumericAnimation { ... duration: 200 }
        PauseAnimation { duration: 100 }
        NumericAnimation { ... duration: 200 }
    }
    \endcode
*/
/*!
    \internal
    \class QmlPauseAnimation
    \ingroup group_animation
    \ingroup group_states
    \brief The QmlPauseAnimation class provides a pause for an animation.

    When used in a QmlSequentialAnimation, QmlPauseAnimation is a step when
    nothing happens, for a specified duration.

    A QmlPauseAnimation object can be instantiated in Qml using the tag
    \l{xmlPauseAnimation} {&lt;PauseAnimation&gt;}.
*/

QML_DEFINE_TYPE(QmlPauseAnimation,PauseAnimation)
QmlPauseAnimation::QmlPauseAnimation(QObject *parent)
: QmlAbstractAnimation(*(new QmlPauseAnimationPrivate), parent)
{
    Q_D(QmlPauseAnimation);
    d->init();
}

QmlPauseAnimation::~QmlPauseAnimation()
{
}

void QmlPauseAnimationPrivate::init()
{
    Q_Q(QmlPauseAnimation);
    pa = new QPauseAnimation(q);
}

/*!
    \qmlproperty int PauseAnimation::duration
    This property holds the duration of the pause in milliseconds

    The default value is 250.
*/
/*!
    \property QmlPauseAnimation::duration
    \brief the duration of the pause in milliseconds

    The default value is 250.
*/
int QmlPauseAnimation::duration() const
{
    Q_D(const QmlPauseAnimation);
    return d->pa->duration();
}

void QmlPauseAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qWarning("QmlPauseAnimation: Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlPauseAnimation);
    if (d->pa->duration() == duration)
        return;
    d->pa->setDuration(duration);
    emit durationChanged(duration);
}

void QmlPauseAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlPauseAnimation);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;
}

QAbstractAnimation *QmlPauseAnimation::qtAnimation()
{
    Q_D(QmlPauseAnimation);
    return d->pa;
}

/*!
    \qmlclass ColorAnimation QmlColorAnimation
    \inherits Animation
    \brief The ColorAnimation allows you to animate color changes.

    \code
    ColorAnimation { from: "white"; to: "#c0c0c0"; duration: 100 }
    \endcode

    The default property animated is \c color, but like other animations,
    this can be changed by setting \c property. The \c color property will
    still animate. XXX is this a bug?
*/
/*!
    \internal
    \class QmlColorAnimation
    \ingroup group_animation
    \ingroup group_states
    \brief The QmlColorAnimation class allows you to animate color changes.

    A QmlColorAnimation object can be instantiated in Qml using the tag
    \l{xmlColorAnimation} {&lt;ColorAnimation&gt;}.
*/

QmlColorAnimation::QmlColorAnimation(QObject *parent)
: QmlAbstractAnimation(*(new QmlColorAnimationPrivate), parent)
{
    Q_D(QmlColorAnimation);
    d->init();
}

QmlColorAnimation::~QmlColorAnimation()
{
}

void QmlColorAnimationPrivate::init()
{
    Q_Q(QmlColorAnimation);
    ca = new QmlTimeLineValueAnimator(q);
    ca->setStartValue(QVariant(0.0f));
    ca->setEndValue(QVariant(1.0f));
}

/*!
    \qmlproperty int ColorAnimation::duration
    This property holds the duration of the color transition, in milliseconds.

    The default value is 250.
*/
/*!
    \property QmlColorAnimation::duration
    \brief the duration of the transition, in milliseconds.

    The default value is 250.
*/
int QmlColorAnimation::duration() const
{
    Q_D(const QmlColorAnimation);
    return d->ca->duration();
}

void QmlColorAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qWarning("QmlColorAnimation: Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlColorAnimation);
    if (d->ca->duration() == duration)
        return;
    d->ca->setDuration(duration);
    emit durationChanged(duration);
}

/*!
    \qmlproperty color ColorAnimation::from
    This property holds the starting color.
*/
/*!
    \property QmlColorAnimation::from
    \brief the starting color.
*/
QColor QmlColorAnimation::from() const
{
    Q_D(const QmlColorAnimation);
    return d->fromValue;
}

void QmlColorAnimation::setFrom(const QColor &f)
{
    Q_D(QmlColorAnimation);
    if (d->fromIsDefined && f == d->fromValue)
        return;
    d->fromValue = f;
    d->fromIsDefined = f.isValid();
    emit fromChanged(f);
}

/*!
    \qmlproperty color ColorAnimation::from
    This property holds the ending color.
*/
/*!
    \property QmlColorAnimation::to
    \brief the ending color.
*/
QColor QmlColorAnimation::to() const
{
    Q_D(const QmlColorAnimation);
    return d->toValue;
}

void QmlColorAnimation::setTo(const QColor &t)
{
    Q_D(QmlColorAnimation);
    if (d->toIsDefined && t == d->toValue)
        return;
    d->toValue = t;
    d->toIsDefined = t.isValid();
    emit toChanged(t);
}

/*!
    \qmlproperty string ColorAnimation::easing
    This property holds the easing curve used for the transition.

    Each channel of the color is eased using the same easing curve.
    See NumericAnimation::easing for a full discussion of easing,
    and a list of available curves.
*/
QString QmlColorAnimation::easing() const
{
    Q_D(const QmlColorAnimation);
    return d->easing;
}

void QmlColorAnimation::setEasing(const QString &e)
{
    Q_D(QmlColorAnimation);
    if (d->easing == e)
        return;

    d->easing = e;
    d->ca->setEasingCurve(stringToCurve(d->easing));
    emit easingChanged(e);
}

/*!
    \qmlproperty list<Item> ColorAnimation::filter
    This property holds the items selected to be affected by this animation (all if not set).
    \sa exclude
*/
QList<QObject *> *QmlColorAnimation::filter()
{
    Q_D(QmlColorAnimation);
    return &d->filter;
}

/*!
    \qmlproperty list<Item> ColorAnimation::exclude
    This property holds the items not to be affected by this animation.
    \sa filter
*/
QList<QObject *> *QmlColorAnimation::exclude()
{
    Q_D(QmlColorAnimation);
    return &d->exclude;
}

void QmlColorAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlColorAnimation);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;
    d->fromSourced = false;
    d->value.QmlTimeLineValue::setValue(0.);
    d->ca->setAnimValue(&d->value, QAbstractAnimation::KeepWhenStopped);
    d->ca->setFromSourcedValue(&d->fromSourced);
}

QAbstractAnimation *QmlColorAnimation::qtAnimation()
{
    Q_D(QmlColorAnimation);
    return d->ca;
}

void QmlColorAnimation::transition(QmlStateActions &actions,
                                   QmlMetaProperties &modified,
                                   TransitionDirection direction)
{
    Q_D(QmlColorAnimation);
    Q_UNUSED(direction);

    struct NTransitionData : public QmlTimeLineValue
    {
        QmlStateActions actions;
        void write(QmlMetaProperty &property, const QVariant &color)
        {
            if (property.propertyType() == QVariant::Color) {
                property.write(color);
            }
        }

        void setValue(qreal v)
        {
            QmlTimeLineValue::setValue(v);
            for (int ii = 0; ii < actions.count(); ++ii) {
                Action &action = actions[ii];

                QColor to(action.toValue.value<QColor>());

                if (v == 1.) {
                    write(action.property, to);
                } else {
                    if (action.fromValue.isNull()) {
                        action.fromValue = action.property.read();
                        if (action.fromValue.isNull())
                            action.fromValue = QVariant(QColor());
                    }

                    QColor from(action.fromValue.value<QColor>());

                    QVariant newColor = QmlColorAnimationPrivate::colorInterpolator(&from, &to, v);
                    write(action.property, newColor);
                }
            }
        }
    };

    //XXX should we get rid of this?
    QStringList props;
    props << QLatin1String("color");
    if (!d->propertyName.isEmpty() && !props.contains(d->propertyName))
        props.append(d->propertyName);

    NTransitionData *data = new NTransitionData;

    QSet<QObject *> objs;
    for (int ii = 0; ii < actions.count(); ++ii) {
        Action &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();

        if ((d->filter.isEmpty() || d->filter.contains(obj)) &&
           (!d->exclude.contains(obj)) && props.contains(propertyName) &&
           (!target() || target() == obj)) {
            objs.insert(obj);
            Action myAction = action;

            if (d->fromIsDefined) {
                myAction.fromValue = QVariant(d->fromValue);
            } else {
                myAction.fromValue = QVariant();
            }
            if (d->toIsDefined)
                myAction.toValue = QVariant(d->toValue);

            modified << action.property;
            data->actions << myAction;
            action.fromValue = myAction.toValue;
        }
    }

    if (d->toValue.isValid() && target() && !objs.contains(target())) {
        QObject *obj = target();
        for (int jj = 0; jj < props.count(); ++jj) {
            Action myAction;
            myAction.property = QmlMetaProperty(obj, props.at(jj));

            if (d->fromIsDefined)
                myAction.fromValue = QVariant(d->fromValue);

            myAction.toValue = QVariant(d->toValue);
            data->actions << myAction;
        }
    }

    if (data->actions.count())
        d->ca->setAnimValue(data, QAbstractAnimation::DeleteWhenStopped);
    else
        delete data;
}

QVariantAnimation::Interpolator QmlColorAnimationPrivate::colorInterpolator = 0;

void QmlColorAnimationPrivate::valueChanged(qreal v)
{
    if (!fromSourced) {
        if (!fromIsDefined) {
            fromValue = qvariant_cast<QColor>(property.read());
        }
        fromSourced = true;
    }

    if (property.propertyType() == QVariant::Color) {
        QVariant newColor = colorInterpolator(&fromValue, &toValue, v);
        property.write(newColor);
    }
}
QML_DEFINE_TYPE(QmlColorAnimation,ColorAnimation)

/*!
    \qmlclass RunScriptAction QmlRunScriptAction
    \inherits Animation
    \brief The RunScripAction allows scripts to be run during transitions.

*/
/*!
    \internal
    \class QmlRunScriptAction
    \brief The QmlRunScriptAction class allows scripts to be run during transitions

    \sa xmlRunScriptAction
*/
QmlRunScriptAction::QmlRunScriptAction(QObject *parent)
    :QmlAbstractAnimation(*(new QmlRunScriptActionPrivate), parent)
{
    Q_D(QmlRunScriptAction);
    d->init();
}

QmlRunScriptAction::~QmlRunScriptAction()
{
}

void QmlRunScriptActionPrivate::init()
{
    Q_Q(QmlRunScriptAction);
    rsa = new QActionAnimation(&proxy, q);
}

/*!
    \qmlproperty QString RunScript::script
    This property holds the script to run.
*/
QString QmlRunScriptAction::script() const
{
    Q_D(const QmlRunScriptAction);
    return d->script;
}

void QmlRunScriptAction::setScript(const QString &script)
{
    Q_D(QmlRunScriptAction);
    if (script == d->script)
        return;
    d->script = script;
    emit scriptChanged(script);
}

/*!
    \qmlproperty QString RunScript::script
    This property holds the file containing the script to run.
*/
QString QmlRunScriptAction::file() const
{
    Q_D(const QmlRunScriptAction);
    return d->file;
}

void QmlRunScriptAction::setFile(const QString &file)
{
    Q_D(QmlRunScriptAction);
    if (file == d->file)
        return;
    d->file = file;
    emit fileChanged(file);
}

void QmlRunScriptActionPrivate::execute()
{
    Q_Q(QmlRunScriptAction);
    QString scriptStr = script;
    if (!file.isEmpty()){
        QFile scriptFile(file);
        if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)){
            scriptStr = QString::fromUtf8(scriptFile.readAll());
        }
    }

    if (!scriptStr.isEmpty()) {
        QmlExpression expr(qmlContext(q), scriptStr, q);
        expr.setTrackChange(false);
        expr.value();
    }
}

QAbstractAnimation *QmlRunScriptAction::qtAnimation()
{
    Q_D(QmlRunScriptAction);
    return d->rsa;
}

QML_DEFINE_TYPE(QmlRunScriptAction, RunScriptAction)

/*!
    \qmlclass SetPropertyAction QmlSetPropertyAction
    \inherits Animation
    \brief The SetPropertyAction allows property changes during transitions.

    Explicitly set \c theimage.smooth=true during a transition:
    \code
    SetPropertyAction { target: theimage; property: "smooth"; value: true }
    \endcode

    Set \c thewebview.url to the value set for the destination state:
    \code
    SetPropertyAction { target: thewebview; property: "url" }
    \endcode

    The SetPropertyAction is immediate -
    the target property is not animated to the selected value in any way.
*/
/*!
    \internal
    \class QmlSetPropertyAction
    \brief The QmlSetPropertyAction class allows property changes during transitions.

    A QmlSetPropertyAction object can be instantiated in Qml using the tag
    \l{xmlSetPropertyAction} {&lt;SetPropertyAction&gt;}.
*/
QmlSetPropertyAction::QmlSetPropertyAction(QObject *parent)
: QmlAbstractAnimation(*(new QmlSetPropertyActionPrivate), parent)
{
    Q_D(QmlSetPropertyAction);
    d->init();
}

QmlSetPropertyAction::~QmlSetPropertyAction()
{
}

void QmlSetPropertyActionPrivate::init()
{
    Q_Q(QmlSetPropertyAction);
    spa = new QActionAnimation(q);
}

/*!
    \qmlproperty string SetPropertyAction::properties
    This property holds the properties to be immediately set, comma-separated.
*/
QString QmlSetPropertyAction::properties() const
{
    Q_D(const QmlSetPropertyAction);
    return d->properties;
}

void QmlSetPropertyAction::setProperties(const QString &p)
{
    Q_D(QmlSetPropertyAction);
    if (d->properties == p)
        return;
    d->properties = p;
    emit propertiesChanged(p);
}

/*!
    \qmlproperty list<Item> SetPropertyAction::filter
    This property holds the items selected to be affected by this animation (all if not set).
    \sa exclude
*/
QList<QObject *> *QmlSetPropertyAction::filter()
{
    Q_D(QmlSetPropertyAction);
    return &d->filter;
}

/*!
    \qmlproperty list<Item> SetPropertyAction::exclude
    This property holds the items not to be affected by this animation.
    \sa filter
*/
QList<QObject *> *QmlSetPropertyAction::exclude()
{
    Q_D(QmlSetPropertyAction);
    return &d->exclude;
}

/*!
    \qmlproperty any SetPropertyAction::value
    This property holds the value to be set on the property.
    If not set, then the value defined for the end state of the transition.
*/
QVariant QmlSetPropertyAction::value() const
{
    Q_D(const QmlSetPropertyAction);
    return d->value;
}

void QmlSetPropertyAction::setValue(const QVariant &v)
{
    Q_D(QmlSetPropertyAction);
    if (d->value.isNull || d->value != v) {
        d->value = v;
        emit valueChanged(v);
    }
}

void QmlSetPropertyActionPrivate::doAction()
{
    property.write(value);
}

QAbstractAnimation *QmlSetPropertyAction::qtAnimation()
{
    Q_D(QmlSetPropertyAction);
    return d->spa;
}

void QmlSetPropertyAction::prepare(QmlMetaProperty &p)
{
    Q_D(QmlSetPropertyAction);

    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    d->spa->setAnimAction(&d->proxy, QAbstractAnimation::KeepWhenStopped);
}

void QmlSetPropertyAction::transition(QmlStateActions &actions,
                                      QmlMetaProperties &modified,
                                      TransitionDirection direction)
{
    Q_D(QmlSetPropertyAction);
    Q_UNUSED(direction);

    struct QmlSetPropertyAnimationAction : public QAbstractAnimationAction
    {
        QmlStateActions actions;
        virtual void doAction()
        {
            for (int ii = 0; ii < actions.count(); ++ii) {
                const Action &action = actions.at(ii);
                QmlBehaviour::_ignore = true;
                action.property.write(action.toValue);
                QmlBehaviour::_ignore = false;
            }
        }
    };

    QStringList props = d->properties.isEmpty() ? QStringList() : d->properties.split(QLatin1Char(','));
    for (int ii = 0; ii < props.count(); ++ii)
        props[ii] = props.at(ii).trimmed();
    if (!d->propertyName.isEmpty() && !props.contains(d->propertyName))
        props.append(d->propertyName);

    if (d->userProperty.isValid() && props.isEmpty() && !target()) {
        props.append(d->userProperty.value.name());
        d->target = d->userProperty.value.object();
   }

    QmlSetPropertyAnimationAction *data = new QmlSetPropertyAnimationAction;

    QSet<QObject *> objs;
    for (int ii = 0; ii < actions.count(); ++ii) {
        Action &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();

        if ((d->filter.isEmpty() || d->filter.contains(obj)) &&
           (!d->exclude.contains(obj)) && props.contains(propertyName) &&
           (!target() || target() == obj)) {
            objs.insert(obj);
            Action myAction = action;

            if (d->value.isValid())
                myAction.toValue = d->value;

            modified << action.property;
            data->actions << myAction;
            action.fromValue = myAction.toValue;
        }
    }

    if (d->value.isValid() && target() && !objs.contains(target())) {
        QObject *obj = target();
        for (int jj = 0; jj < props.count(); ++jj) {
            Action myAction;
            myAction.property = QmlMetaProperty(obj, props.at(jj));
            myAction.toValue = d->value;
            data->actions << myAction;
        }
    }

    if (data->actions.count()) {
        d->spa->setAnimAction(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(QmlSetPropertyAction,SetPropertyAction)

/*!
    \qmlclass ParentChangeAction QmlParentChangeAction
    \inherits Animation
    \brief The ParentChangeAction allows parent changes during transitions.

    The ParentChangeAction is immediate - it is not animated in any way.
*/

QmlParentChangeAction::QmlParentChangeAction(QObject *parent)
: QmlAbstractAnimation(*(new QmlParentChangeActionPrivate), parent)
{
    Q_D(QmlParentChangeAction);
    d->init();
}

QmlParentChangeAction::~QmlParentChangeAction()
{
}

void QmlParentChangeActionPrivate::init()
{
    Q_Q(QmlParentChangeAction);
    cpa = new QActionAnimation(q);
}

void QmlParentChangeActionPrivate::doAction()
{
    //XXX property.write(value);
}

void QmlParentChangeAction::prepare(QmlMetaProperty &p)
{
    Q_D(QmlParentChangeAction);

    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    //XXX
}

QAbstractAnimation *QmlParentChangeAction::qtAnimation()
{
    Q_D(QmlParentChangeAction);
    return d->cpa;
}

void QmlParentChangeAction::transition(QmlStateActions &actions,
                                       QmlMetaProperties &modified,
                                       TransitionDirection direction)
{
    Q_D(QmlParentChangeAction);
    Q_UNUSED(direction);

    struct QmlParentChangeActionData : public QAbstractAnimationAction
    {
        QmlStateActions actions;
        virtual void doAction()
        {
            for (int ii = 0; ii < actions.count(); ++ii) {
                const Action &action = actions.at(ii);
                QmlBehaviour::_ignore = true;
                action.property.write(action.toValue);
                QmlBehaviour::_ignore = false;
            }
        }
    };

    QmlParentChangeActionData *data = new QmlParentChangeActionData;

    QSet<QObject *> objs;
    for (int ii = 0; ii < actions.count(); ++ii) {
        Action &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();

        if ((!target() || target() == obj) && propertyName == QString(QLatin1String("moveToParent"))) {
            objs.insert(obj);
            Action myAction = action;

            /*if (d->value.isValid())
                myAction.toValue = d->value;*/

            modified << action.property;
            data->actions << myAction;
            action.fromValue = myAction.toValue;
        }
    }

    /*if (d->value.isValid() && target() && !objs.contains(target())) {
        QObject *obj = target();
        for (int jj = 0; jj < props.count(); ++jj) {
            Action myAction;
            myAction.property = QmlMetaProperty(obj, props.at(jj));
            myAction.toValue = d->value;
            data->actions << myAction;
        }
    }*/

    if (data->actions.count()) {
        d->cpa->setAnimAction(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(QmlParentChangeAction,ParentChangeAction)

/*!
    \qmlclass NumericAnimation QmlNumericAnimation
    \inherits Animation
    \brief The NumericAnimation allows you to animate changes in properties of type qreal.

    Animate a set of properties over 200ms, from their values in the start state to
    their values in the end state of the transition:
    \code
    NumericAnimation { properties: "x,y,scale"; duration: 200 }
    \endcode
*/

/*!
    \internal
    \class QmlNumericAnimation
    \ingroup group_animation
    \ingroup group_states
    \brief The QmlNumericAnimation class allows you to animate changes in properties of type qreal.

    A QmlNumericAnimation object can be instantiated in Qml using the tag
    \l{xmlNumericAnimation} {&lt;NumericAnimation&gt;}.
*/

QmlNumericAnimation::QmlNumericAnimation(QObject *parent)
: QmlAbstractAnimation(*(new QmlNumericAnimationPrivate), parent)
{
    Q_D(QmlNumericAnimation);
    d->init();
}

QmlNumericAnimation::~QmlNumericAnimation()
{
}

void QmlNumericAnimationPrivate::init()
{
    Q_Q(QmlNumericAnimation);
    na = new QmlTimeLineValueAnimator(q);
    na->setStartValue(QVariant(0.0f));
    na->setEndValue(QVariant(1.0f));
}

/*!
    \qmlproperty int NumericAnimation::duration
    This property holds the duration of the transition, in milliseconds.

    The default value is 250.
*/
/*!
    \property QmlNumericAnimation::duration
    \brief the duration of the transition, in milliseconds.

    The default value is 250.
*/
int QmlNumericAnimation::duration() const
{
    Q_D(const QmlNumericAnimation);
    return d->na->duration();
}

void QmlNumericAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qWarning("QmlNumericAnimation: Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlNumericAnimation);
    if (d->na->duration() == duration)
        return;
    d->na->setDuration(duration);
    emit durationChanged(duration);
}

/*!
    \qmlproperty real NumericAnimation::from
    This property holds the starting value.
    If not set, then the value defined in the start state of the transition.
*/
/*!
    \property QmlNumericAnimation::from
    \brief the starting value.
*/
qreal QmlNumericAnimation::from() const
{
    Q_D(const QmlNumericAnimation);
    return d->from;
}

void QmlNumericAnimation::setFrom(qreal f)
{
    Q_D(QmlNumericAnimation);
    if (!d->from.isNull && f == d->from)
        return;
    d->from = f;
    emit fromChanged(f);
}

/*!
    \qmlproperty real NumericAnimation::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition.
*/
/*!
    \property QmlNumericAnimation::to
    \brief the ending value.
*/
qreal QmlNumericAnimation::to() const
{
    Q_D(const QmlNumericAnimation);
    return d->to;
}

void QmlNumericAnimation::setTo(qreal t)
{
    Q_D(QmlNumericAnimation);
    if (!d->to.isNull && t == d->to)
        return;
    d->to = t;
    emit toChanged(t);
}

/*!
    \qmlproperty string NumericAnimation::easing
    \brief the easing curve used for the transition.

    Available values are:

    \list
    \i \e easeNone - Easing equation function for a simple linear tweening, with no easing.
    \i \e easeInQuad - Easing equation function for a quadratic (t^2) easing in: accelerating from zero velocity.
    \i \e easeOutQuad - Easing equation function for a quadratic (t^2) easing out: decelerating to zero velocity.
    \i \e easeInOutQuad - Easing equation function for a quadratic (t^2) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInQuad - Easing equation function for a quadratic (t^2) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInCubic - Easing equation function for a cubic (t^3) easing in: accelerating from zero velocity.
    \i \e easeOutCubic - Easing equation function for a cubic (t^3) easing out: decelerating from zero velocity.
    \i \e easeInOutCubic - Easing equation function for a cubic (t^3) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInCubic - Easing equation function for a cubic (t^3) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInQuart - Easing equation function for a quartic (t^4) easing in: accelerating from zero velocity.
    \i \e easeOutQuart - Easing equation function for a quartic (t^4) easing out: decelerating from zero velocity.
    \i \e easeInOutQuart - Easing equation function for a quartic (t^4) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInQuart - Easing equation function for a quartic (t^4) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInQuint - Easing equation function for a quintic (t^5) easing in: accelerating from zero velocity.
    \i \e easeOutQuint - Easing equation function for a quintic (t^5) easing out: decelerating from zero velocity.
    \i \e easeInOutQuint - Easing equation function for a quintic (t^5) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInQuint - Easing equation function for a quintic (t^5) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInSine - Easing equation function for a sinusoidal (sin(t)) easing in: accelerating from zero velocity.
    \i \e easeOutSine - Easing equation function for a sinusoidal (sin(t)) easing out: decelerating from zero velocity.
    \i \e easeInOutSine - Easing equation function for a sinusoidal (sin(t)) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInSine - Easing equation function for a sinusoidal (sin(t)) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInExpo - Easing equation function for an exponential (2^t) easing in: accelerating from zero velocity.
    \i \e easeOutExpo - Easing equation function for an exponential (2^t) easing out: decelerating from zero velocity.
    \i \e easeInOutExpo - Easing equation function for an exponential (2^t) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInExpo - Easing equation function for an exponential (2^t) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInCirc - Easing equation function for a circular (sqrt(1-t^2)) easing in: accelerating from zero velocity.
    \i \e easeOutCirc - Easing equation function for a circular (sqrt(1-t^2)) easing out: decelerating from zero velocity.
    \i \e easeInOutCirc - Easing equation function for a circular (sqrt(1-t^2)) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInCirc - Easing equation function for a circular (sqrt(1-t^2)) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInElastic - Easing equation function for an elastic (exponentially decaying sine wave) easing in: accelerating from zero velocity.  The peak amplitude can be set with the \e amplitude parameter, and the period of decay by the \e period parameter.
    \i \e easeOutElastic - Easing equation function for an elastic (exponentially decaying sine wave) easing out: decelerating from zero velocity.  The peak amplitude can be set with the \e amplitude parameter, and the period of decay by the \e period parameter.
    \i \e easeInOutElastic - Easing equation function for an elastic (exponentially decaying sine wave) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInElastic - Easing equation function for an elastic (exponentially decaying sine wave) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeInBack - Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
    \i \e easeOutBack - Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out: decelerating from zero velocity.
    \i \e easeInOutBack - Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInBack - Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
    \i \e easeOutBounce - Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out: decelerating from zero velocity.
    \i \e easeInBounce - Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in: accelerating from zero velocity.
    \i \e easeInOutBounce - Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in/out: acceleration until halfway, then deceleration.
    \i \e easeOutInBounce - Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out/in: deceleration until halfway, then acceleration.
    \endlist
*/

/*!
    \property QmlNumericAnimation::easing
    This property holds the easing curve to use.

    \sa QEasingCurve
*/
QString QmlNumericAnimation::easing() const
{
    Q_D(const QmlNumericAnimation);
    return d->easing;
}

void QmlNumericAnimation::setEasing(const QString &e)
{
    Q_D(QmlNumericAnimation);
    if (d->easing == e)
        return;

    d->easing = e;
    d->na->setEasingCurve(stringToCurve(d->easing));
    emit easingChanged(e);
}

/*!
    \qmlproperty string NumericAnimation::properties
    This property holds the properties this animation should be applied to.

    This is a comma-separated list of properties that should use
    this animation when they change.
*/
/*!
    \property QmlNumericAnimation::properties
    \brief the properties this animation should be applied to.

    properties holds a comma-separated list of properties that should use
    this animation when they change.
*/
QString QmlNumericAnimation::properties() const
{
    Q_D(const QmlNumericAnimation);
    return d->properties;
}

void QmlNumericAnimation::setProperties(const QString &prop)
{
    Q_D(QmlNumericAnimation);
    if (d->properties == prop)
        return;

    d->properties = prop;
    emit propertiesChanged(prop);
}

/*!
    \qmlproperty list<Item> NumericAnimation::filter
    This property holds the items selected to be affected by this animation (all if not set).
    \sa exclude
*/
QList<QObject *> *QmlNumericAnimation::filter()
{
    Q_D(QmlNumericAnimation);
    return &d->filter;
}

/*!
    \qmlproperty list<Item> NumericAnimation::exclude
    This property holds the items not to be affected by this animation.
    \sa filter
*/
QList<QObject *> *QmlNumericAnimation::exclude()
{
    Q_D(QmlNumericAnimation);
    return &d->exclude;
}

void QmlNumericAnimationPrivate::valueChanged(qreal r)
{
    if (!fromSourced) {
        if (from.isNull) {
            fromValue = qvariant_cast<qreal>(property.read());
        } else {
            fromValue = from;
        }
        fromSourced = true;
    }

    if (r == 1.) {
        property.write(to.value);
    } else {
        qreal val = fromValue + (to-fromValue) * r;
        property.write(val);
    }
}

void QmlNumericAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlNumericAnimation);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;
    d->fromSourced = false;
    d->value.QmlTimeLineValue::setValue(0.);
    d->na->setAnimValue(&d->value, QAbstractAnimation::KeepWhenStopped);
    d->na->setFromSourcedValue(&d->fromSourced);
}

QAbstractAnimation *QmlNumericAnimation::qtAnimation()
{
    Q_D(QmlNumericAnimation);
    return d->na;
}

void QmlNumericAnimation::transition(QmlStateActions &actions,
                                     QmlMetaProperties &modified,
                                     TransitionDirection direction)
{
    Q_D(QmlNumericAnimation);
    Q_UNUSED(direction);

    struct NTransitionData : public QmlTimeLineValue
    {
        QmlStateActions actions;
        void setValue(qreal v)
        {
            QmlTimeLineValue::setValue(v);
            for (int ii = 0; ii < actions.count(); ++ii) {
                Action &action = actions[ii];

                QmlBehaviour::_ignore = true;
                if (v == 1.)
                    action.property.write(action.toValue.toDouble());
                else {
                    if (action.fromValue.isNull()) {
                        action.fromValue = action.property.read();
                        if (action.fromValue.isNull()) {
                            action.fromValue = QVariant(0.);
                        }
                    }
                    qreal start = action.fromValue.toDouble();
                    qreal end = action.toValue.toDouble();
                    qreal val = start + (end-start) * v;
                    action.property.write(val);
                }
                QmlBehaviour::_ignore = false;
            }
        }
    };

    QStringList props = d->properties.isEmpty() ? QStringList() : d->properties.split(QLatin1Char(','));
    for (int ii = 0; ii < props.count(); ++ii)
        props[ii] = props.at(ii).trimmed();
    if (!d->propertyName.isEmpty() && !props.contains(d->propertyName))
        props.append(d->propertyName);

   if (d->userProperty.isValid() && props.isEmpty() && !target()) {
        props.append(d->userProperty.value.name());
        d->target = d->userProperty.value.object();
   }

    NTransitionData *data = new NTransitionData;

    QSet<QObject *> objs;
    for (int ii = 0; ii < actions.count(); ++ii) {
        Action &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();

        if ((d->filter.isEmpty() || d->filter.contains(obj)) &&
           (!d->exclude.contains(obj)) && props.contains(propertyName) &&
           (!target() || target() == obj)) {
            objs.insert(obj);
            Action myAction = action;
            if (d->from.isValid()) {
                myAction.fromValue = QVariant(d->from);
            } else {
                myAction.fromValue = QVariant();
            }
            if (d->to.isValid())
                myAction.toValue = QVariant(d->to);

            modified << action.property;

            data->actions << myAction;
            action.fromValue = myAction.toValue;
        }
    }

    if (d->to.isValid() && target() && !objs.contains(target())) {
        QObject *obj = target();
        for (int jj = 0; jj < props.count(); ++jj) {
            Action myAction;
            myAction.property = QmlMetaProperty(obj, props.at(jj));

            if (d->from.isValid())
                myAction.fromValue = QVariant(d->from);

            myAction.toValue = QVariant(d->to);
            data->actions << myAction;
        }
    }

    if (data->actions.count()) {
        d->na->setAnimValue(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(QmlNumericAnimation,NumericAnimation)

QmlAnimationGroup::QmlAnimationGroup(QObject *parent)
: QmlAbstractAnimation(*(new QmlAnimationGroupPrivate), parent)
{
}

QmlAnimationGroup::~QmlAnimationGroup()
{
}

QmlList<QmlAbstractAnimation *> *QmlAnimationGroup::animations()
{
    Q_D(QmlAnimationGroup);
    return &d->animations;
}

/*!
    \qmlclass SequentialAnimation QmlSequentialAnimation
    \inherits Animation
    \brief The SequentialAnimation allows you to run animations sequentially.

    Animations controlled in SequentialAnimation will be run one after the other.

    The following example chains two numeric animations together.  The \c MyItem
    object will animate from its current x position to 100, and then back to 0.

    \code
    SequentialAnimation {
        NumericAnimation { target: MyItem; property: "x"; to: 100 }
        NumericAnimation { target: MyItem; property: "x"; to: 0 }
    }
    \endcode

    \sa ParallelAnimation
*/

QmlSequentialAnimation::QmlSequentialAnimation(QObject *parent) :
    QmlAnimationGroup(parent)
{
    Q_D(QmlAnimationGroup);
    d->ag = new QSequentialAnimationGroup(this);
}

QmlSequentialAnimation::~QmlSequentialAnimation()
{
}

void QmlSequentialAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlAnimationGroup);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    for (int i = 0; i < d->animations.size(); ++i)
        d->animations.at(i)->prepare(d->property);
}

QAbstractAnimation *QmlSequentialAnimation::qtAnimation()
{
    Q_D(QmlAnimationGroup);
    return d->ag;
}

void QmlSequentialAnimation::transition(QmlStateActions &actions,
                                    QmlMetaProperties &modified,
                                    TransitionDirection direction)
{
    Q_D(QmlAnimationGroup);

    int inc = 1;
    int from = 0;
    if (direction == Backward) {
        inc = -1;
        from = d->animations.count() - 1;
    }
    
    //### needed for Behavior
    if (d->userProperty.isValid() && d->propertyName.isEmpty() && !target()) {
        for (int i = 0; i < d->animations.count(); ++i)
            d->animations.at(i)->setTarget(d->userProperty);
   }

    //XXX removing and readding isn't ideal; we do it to get around the problem mentioned below.
    for (int i = d->ag->animationCount()-1; i >= 0; --i)
        d->ag->takeAnimationAt(i);

    for (int ii = from; ii < d->animations.count() && ii >= 0; ii += inc) {
        d->animations.at(ii)->transition(actions, modified, direction);
        d->ag->addAnimation(d->animations.at(ii)->qtAnimation());
    }

    //XXX changing direction means all the animations play in reverse, while we only want the ordering reversed.
    //d->ag->setDirection(direction == Backward ? QAbstractAnimation::Backward : QAbstractAnimation::Forward);
}

QML_DEFINE_TYPE(QmlSequentialAnimation,SequentialAnimation)

/*!
    \qmlclass ParallelAnimation QmlParallelAnimation
    \inherits Animation
    \brief The ParallelAnimation allows you to run animations in parallel.

    Animations contained in ParallelAnimation will be run at the same time.

    The following animation demonstrates animating the \c MyItem item
    to (100,100) by animating the x and y properties in parallel.

    \code
    ParallelAnimation {
        NumericAnimation { target: MyItem; property: "x"; to: 100 }
        NumericAnimation { target: MyItem; property: "y"; to: 100 }
    }
    \endcode

    \sa SequentialAnimation
*/
/*!
    \internal
    \class QmlParallelAnimation
    \ingroup group_animation
    \ingroup group_states
    \brief The QmlParallelAnimation class allows you to run animations in parallel.

    Animations controlled by QmlParallelAnimation will be run at the same time.

    \sa QmlSequentialAnimation

    A QmlParallelAnimation object can be instantiated in Qml using the tag
    \l{xmlParallelAnimation} {&lt;ParallelAnimation&gt;}.
*/

QmlParallelAnimation::QmlParallelAnimation(QObject *parent) :
    QmlAnimationGroup(parent)
{
    Q_D(QmlAnimationGroup);
    d->ag = new QParallelAnimationGroup(this);
}

QmlParallelAnimation::~QmlParallelAnimation()
{
}

void QmlParallelAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlAnimationGroup);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    for (int i = 0; i < d->animations.size(); ++i)
        d->animations.at(i)->prepare(d->property);
}

QAbstractAnimation *QmlParallelAnimation::qtAnimation()
{
    Q_D(QmlAnimationGroup);
    return d->ag;
}

void QmlParallelAnimation::transition(QmlStateActions &actions,
                                      QmlMetaProperties &modified,
                                      TransitionDirection direction)
{
    Q_D(QmlAnimationGroup);

     //### needed for Behavior
    if (d->userProperty.isValid() && d->propertyName.isEmpty() && !target()) {
        for (int i = 0; i < d->animations.count(); ++i)
            d->animations.at(i)->setTarget(d->userProperty);
   }

    for (int ii = 0; ii < d->animations.count(); ++ii) {
        d->animations.at(ii)->transition(actions, modified, direction);
    }
}

QML_DEFINE_TYPE(QmlParallelAnimation,ParallelAnimation)

QVariant QmlVariantAnimationPrivate::interpolateVariant(const QVariant &from, const QVariant &to, qreal progress)
{
    if (from.userType() != to.userType())
        return QVariant();

    QVariantAnimation::Interpolator interpolator = QVariantAnimationPrivate::getInterpolator(from.userType());
    return interpolator(from.constData(), to.constData(), progress);
}

//convert a variant from string type to another animatable type
void QmlVariantAnimationPrivate::convertVariant(QVariant &variant, QVariant::Type type)
{
    if (variant.type() != QVariant::String) {
        variant.convert(type);
        return;
    }

    switch (type) {
    case QVariant::Rect: {
        variant.setValue(QmlStringConverters::rectFFromString(variant.toString()).toRect());
        break;
    }
    case QVariant::RectF: {
        variant.setValue(QmlStringConverters::rectFFromString(variant.toString()));
        break;
    }
    case QVariant::Point: {
        variant.setValue(QmlStringConverters::pointFFromString(variant.toString()).toPoint());
        break;
    }
    case QVariant::PointF: {
        variant.setValue(QmlStringConverters::pointFFromString(variant.toString()));
        break;
    }
    case QVariant::Size: {
        variant.setValue(QmlStringConverters::sizeFFromString(variant.toString()).toSize());
        break;
    }
    case QVariant::SizeF: {
        variant.setValue(QmlStringConverters::sizeFFromString(variant.toString()));
        break;
    }
    case QVariant::Color: {
        variant.setValue(QmlStringConverters::colorFromString(variant.toString()));
        break;
    }
    default:
        variant.convert(type);
        break;
    }
}

/*!
    \qmlclass VariantAnimation QmlVariantAnimation
    \inherits Animation
    \brief The VariantAnimation allows you to animate changes in properties of type QVariant.

    Animate a size property over 200ms, from its current size to 20-by-20:
    \code
    VariantAnimation { property: "size"; to: "20x20"; duration: 200 }
    \endcode
*/

QmlVariantAnimation::QmlVariantAnimation(QObject *parent)
: QmlAbstractAnimation(*(new QmlVariantAnimationPrivate), parent)
{
    Q_D(QmlVariantAnimation);
    d->init();
}

QmlVariantAnimation::~QmlVariantAnimation()
{
}

void QmlVariantAnimationPrivate::init()
{
    Q_Q(QmlVariantAnimation);
    va = new QmlTimeLineValueAnimator(q);
    va->setStartValue(QVariant(0.0f));
    va->setEndValue(QVariant(1.0f));
}

/*!
    \qmlproperty int VariantAnimation::duration
    This property holds the duration of the transition, in milliseconds.

    The default value is 250.
*/
/*!
    \property QmlVariantAnimation::duration
    \brief the duration of the transition, in milliseconds.

    The default value is 250.
*/
int QmlVariantAnimation::duration() const
{
    Q_D(const QmlVariantAnimation);
    return d->va->duration();
}

void QmlVariantAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qWarning("QmlVariantAnimation: Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlVariantAnimation);
    if (d->va->duration() == duration)
        return;
    d->va->setDuration(duration);
    emit durationChanged(duration);
}

/*!
    \qmlproperty real VariantAnimation::from
    This property holds the starting value.
    If not set, then the value defined in the start state of the transition.
*/
/*!
    \property QmlVariantAnimation::from
    \brief the starting value.
*/
QVariant QmlVariantAnimation::from() const
{
    Q_D(const QmlVariantAnimation);
    return d->from;
}

void QmlVariantAnimation::setFrom(const QVariant &f)
{
    Q_D(QmlVariantAnimation);
    if (d->fromIsDefined && f == d->from)
        return;
    d->from = f;
    d->fromIsDefined = f.isValid();
    emit fromChanged(f);
}

/*!
    \qmlproperty real VariantAnimation::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition.
*/
/*!
    \property QmlVariantAnimation::to
    \brief the ending value.
*/
QVariant QmlVariantAnimation::to() const
{
    Q_D(const QmlVariantAnimation);
    return d->to;
}

void QmlVariantAnimation::setTo(const QVariant &t)
{
    Q_D(QmlVariantAnimation);
    if (d->toIsDefined && t == d->to)
        return;
    d->to = t;
    d->toIsDefined = t.isValid();
    emit toChanged(t);
}

/*!
    \qmlproperty string VariantAnimation::easing
    This property holds the easing curve used for the transition.

    See NumericAnimation::easing for a full discussion of easing,
    and a list of available curves.
*/

/*!
    \property QmlVariantAnimation::easing
    \brief the easing curve to use.

    \sa QEasingCurve
*/
QString QmlVariantAnimation::easing() const
{
    Q_D(const QmlVariantAnimation);
    return d->easing;
}

void QmlVariantAnimation::setEasing(const QString &e)
{
    Q_D(QmlVariantAnimation);
    if (d->easing == e)
        return;

    d->easing = e;
    d->va->setEasingCurve(stringToCurve(d->easing));
    emit easingChanged(e);
}

/*!
    \qmlproperty string VariantAnimation::properties
    This property holds the properties this animation should be applied to.

    This is a comma-separated list of properties that should use
    this animation when they change.
*/
/*!
    \property QmlVariantAnimation::properties
    \brief the properties this animation should be applied to

    properties holds a copy separated list of properties that should use
    this animation when they change.
*/
QString QmlVariantAnimation::properties() const
{
    Q_D(const QmlVariantAnimation);
    return d->properties;
}

void QmlVariantAnimation::setProperties(const QString &prop)
{
    Q_D(QmlVariantAnimation);
    if (d->properties == prop)
        return;

    d->properties = prop;
    emit propertiesChanged(prop);
}

/*!
    \qmlproperty list<Item> VariantAnimation::filter
    This property holds the items selected to be affected by this animation (all if not set).
    \sa exclude
*/
QList<QObject *> *QmlVariantAnimation::filter()
{
    Q_D(QmlVariantAnimation);
    return &d->filter;
}

/*!
    \qmlproperty list<Item> VariantAnimation::exclude
    This property holds the items not to be affected by this animation.
    \sa filter
*/
QList<QObject *> *QmlVariantAnimation::exclude()
{
    Q_D(QmlVariantAnimation);
    return &d->exclude;
}

void QmlVariantAnimationPrivate::valueChanged(qreal r)
{
    if (!fromSourced) {
        if (!fromIsDefined) {
            from = property.read();
        }
        fromSourced = true;
    }

    if (r == 1.) {
        property.write(to);
    } else {
        QVariant val = interpolateVariant(from, to, r);
        property.write(val);
    }
}

QAbstractAnimation *QmlVariantAnimation::qtAnimation()
{
    Q_D(QmlVariantAnimation);
    return d->va;
}

void QmlVariantAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlVariantAnimation);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    d->convertVariant(d->to, (QVariant::Type)d->property.propertyType());
    if (d->fromIsDefined)
        d->convertVariant(d->from, (QVariant::Type)d->property.propertyType());

    d->fromSourced = false;
    d->value.QmlTimeLineValue::setValue(0.);
    d->va->setAnimValue(&d->value, QAbstractAnimation::KeepWhenStopped);
    d->va->setFromSourcedValue(&d->fromSourced);
}

void QmlVariantAnimation::transition(QmlStateActions &actions,
                                     QmlMetaProperties &modified,
                                     TransitionDirection direction)
{
    Q_D(QmlVariantAnimation);
    Q_UNUSED(direction);

    struct NTransitionData : public QmlTimeLineValue
    {
        QmlStateActions actions;
        void setValue(qreal v)
        {
            QmlTimeLineValue::setValue(v);
            for (int ii = 0; ii < actions.count(); ++ii) {
                Action &action = actions[ii];

                if (v == 1.)
                    action.property.write(action.toValue);
                else {
                    if (action.fromValue.isNull()) {
                        action.fromValue = action.property.read();
                        /*if (action.fromValue.isNull())
                            action.fromValue = QVariant(0.);*/    //XXX can we give a default value for any type?
                    }
                    QVariant val = QmlVariantAnimationPrivate::interpolateVariant(action.fromValue, action.toValue, v);
                    action.property.write(val);
                }
            }
        }
    };

    QStringList props = d->properties.split(QLatin1Char(','));
    for (int ii = 0; ii < props.count(); ++ii)
        props[ii] = props.at(ii).trimmed();
    if (!d->propertyName.isEmpty() && !props.contains(d->propertyName))
        props.append(d->propertyName);

    NTransitionData *data = new NTransitionData;

    QSet<QObject *> objs;
    for (int ii = 0; ii < actions.count(); ++ii) {
        Action &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();

        if ((d->filter.isEmpty() || d->filter.contains(obj)) &&
           (!d->exclude.contains(obj)) && props.contains(propertyName) &&
           (!target() || target() == obj)) {
            objs.insert(obj);
            Action myAction = action;

            if (d->fromIsDefined) {
                myAction.fromValue = d->from;
            } else {
                myAction.fromValue = QVariant();
            }
            if (d->toIsDefined)
                myAction.toValue = d->to;

            d->convertVariant(myAction.fromValue, (QVariant::Type)myAction.property.propertyType());
            d->convertVariant(myAction.toValue, (QVariant::Type)myAction.property.propertyType());

            modified << action.property;

            data->actions << myAction;
            action.fromValue = myAction.toValue;
        }
    }

    if (d->toIsDefined && target() && !objs.contains(target())) {
        QObject *obj = target();
        for (int jj = 0; jj < props.count(); ++jj) {
            Action myAction;
            myAction.property = QmlMetaProperty(obj, props.at(jj));

            if (d->fromIsDefined) {
                d->convertVariant(d->from, (QVariant::Type)myAction.property.propertyType());
                myAction.fromValue = d->from;
            }

            d->convertVariant(d->to, (QVariant::Type)myAction.property.propertyType());
            myAction.toValue = d->to;
            data->actions << myAction;
        }
    }

    if (data->actions.count()) {
        d->va->setAnimValue(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

//XXX whats the best name for this? (just Animation?)
QML_DEFINE_TYPE(QmlVariantAnimation,VariantAnimation)

QT_END_NAMESPACE
