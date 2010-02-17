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

#include "qmlanimation_p.h"
#include "qmlanimation_p_p.h"

#include "qmlbehavior_p.h"
#include "qmlstateoperations_p.h"

#include <qmlpropertyvaluesource.h>
#include <qml.h>
#include <qmlinfo.h>
#include <qmlexpression.h>
#include <qmlstringconverters_p.h>
#include <qmlglobal_p.h>

#include <qvariant.h>
#include <qcolor.h>
#include <qfile.h>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QtCore/qset.h>
#include <QtCore/qrect.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>

#include <private/qvariantanimation_p.h>

QT_BEGIN_NAMESPACE

static QEasingCurve stringToCurve(const QString &curve, QObject *obj)
{
    QEasingCurve easingCurve;

    QString normalizedCurve = curve;
    bool hasParams = curve.contains(QLatin1Char('('));
    QStringList props;

    if (hasParams) {
        QString easeName = curve.trimmed();
        if (!easeName.endsWith(QLatin1Char(')'))) {
            qmlInfo(obj) << obj->tr("Unmatched parenthesis in easing function \"%1\"").arg(curve);
            return easingCurve;
        }

        int idx = easeName.indexOf(QLatin1Char('('));
        QString prop_str =
            easeName.mid(idx + 1, easeName.length() - 1 - idx - 1);
        normalizedCurve = easeName.left(idx);
        if (!normalizedCurve.startsWith(QLatin1String("ease"))) {
            qmlInfo(obj) << obj->tr("Easing function \"%1\" must start with \"ease\"").arg(curve);
            return easingCurve;
        }

        props = prop_str.split(QLatin1Char(','));
    }

    if (normalizedCurve.startsWith(QLatin1String("ease")))
        normalizedCurve = normalizedCurve.mid(4);

    static int index = QEasingCurve::staticMetaObject.indexOfEnumerator("Type");
    static QMetaEnum me = QEasingCurve::staticMetaObject.enumerator(index);

    int value = me.keyToValue(normalizedCurve.toUtf8().constData());
    if (value < 0) {
        qmlInfo(obj) << obj->tr("Unknown easing curve \"%1\"").arg(curve);
        return easingCurve;
    }
    easingCurve.setType((QEasingCurve::Type)value);

    if (hasParams) {
        foreach(const QString &str, props) {
            int sep = str.indexOf(QLatin1Char(':'));

            if (sep == -1) {
                qmlInfo(obj) << obj->tr("Improperly specified parameter in easing function \"%1\"").arg(curve);
                continue;
            }

            QString propName = str.left(sep).trimmed();
            bool isOk;
            qreal propValue = str.mid(sep + 1).trimmed().toDouble(&isOk);

            if (propName.isEmpty() || !isOk) {
                qmlInfo(obj) << obj->tr("Improperly specified parameter in easing function \"%1\"").arg(curve);
                continue;
            }

            if (propName == QLatin1String("amplitude")) {
                easingCurve.setAmplitude(propValue);
            } else if (propName == QLatin1String("period")) {
                easingCurve.setPeriod(propValue);
            } else if (propName == QLatin1String("overshoot")) {
                easingCurve.setOvershoot(propValue);
            } else {
                qmlInfo(obj) << obj->tr("Unknown easing parameter \"%1\"").arg(propName);
                continue;
            }
        }
    }
    return easingCurve;
}

QML_DEFINE_NOCREATE_TYPE(QmlAbstractAnimation)

/*!
    \qmlclass Animation QmlAbstractAnimation
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
: QObject(*(new QmlAbstractAnimationPrivate), parent)
{
}

QmlAbstractAnimation::~QmlAbstractAnimation()
{
}

QmlAbstractAnimation::QmlAbstractAnimation(QmlAbstractAnimationPrivate &dd, QObject *parent)
: QObject(dd, parent)
{
}

/*!
    \qmlproperty bool Animation::running
    This property holds whether the animation is currently running.

    The \c running property can be set to declaratively control whether or not
    an animation is running.  The following example will animate a rectangle
    whenever the \l MouseRegion is pressed.

    \code
    Rectangle {
        width: 100; height: 100
        x: NumberAnimation {
            running: myMouse.pressed
            from: 0; to: 100
        }
        MouseRegion { id: myMouse }
    }
    \endcode

    Likewise, the \c running property can be read to determine if the animation
    is running.  In the following example the text element will indicate whether
    or not the animation is running.

    \code
    NumberAnimation { id: myAnimation }
    Text { text: myAnimation.running ? "Animation is running" : "Animation is not running" }
    \endcode

    Animations can also be started and stopped imperatively from JavaScript
    using the \c start() and \c stop() methods.

    By default, animations are not running. Though, when the animations are assigned to properties,
    as property value sources, they are set to running by default.
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

QmlMetaProperty QmlAbstractAnimationPrivate::createProperty(QObject *obj, const QString &str, QObject *infoObj)
{
    QmlMetaProperty prop = QmlMetaProperty::createProperty(obj, str, qmlContext(infoObj));
    if (!prop.isValid()) {
        qmlInfo(infoObj) << QmlAbstractAnimation::tr("Cannot animate non-existant property \"%1\"").arg(str);
        return QmlMetaProperty();
    } else if (!prop.isWritable()) {
        qmlInfo(infoObj) << QmlAbstractAnimation::tr("Cannot animate read-only property \"%1\"").arg(str);
        return QmlMetaProperty();
    }
    return prop;
}

void QmlAbstractAnimation::setRunning(bool r)
{
    Q_D(QmlAbstractAnimation);

    if (r == false)
        d->avoidPropertyValueSourceStart = true;

    if (d->running == r)
        return;

    if (d->group) {
        qWarning("QmlAbstractAnimation: setRunning() cannot be used on non-root animation nodes");
        return;
    }

    d->running = r;
    if (d->running) {
        if (d->alwaysRunToEnd && d->repeat
            && qtAnimation()->state() == QAbstractAnimation::Running) {
            qtAnimation()->setLoopCount(-1);
        }

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
        if (d->alwaysRunToEnd) {
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
    \qmlproperty bool Animation::alwaysRunToEnd
    This property holds whether the animation should run to completion when it is stopped.

    If this true the animation will complete its current iteration when it
    is stopped - either by setting the \c running property to false, or by
    calling the \c stop() method.  The \c complete() method is not effected
    by this value.

    This behavior is most useful when the \c repeat property is set, as the
    animation will finish playing normally but not restart.

    By default, the alwaysRunToEnd property is not set.
*/
bool QmlAbstractAnimation::alwaysRunToEnd() const
{
    Q_D(const QmlAbstractAnimation);
    return d->alwaysRunToEnd;
}

void QmlAbstractAnimation::setAlwaysRunToEnd(bool f)
{
    Q_D(QmlAbstractAnimation);
    if (d->alwaysRunToEnd == f)
        return;

    d->alwaysRunToEnd = f;
    emit alwaysRunToEndChanged(f);
}

/*!
    \qmlproperty bool Animation::repeat
    This property holds whether the animation should repeat.

    If set, the animation will continuously repeat until it is explicitly
    stopped - either by setting the \c running property to false, or by calling
    the \c stop() method.

    In the following example, the rectangle will spin indefinately.

    \code
    Rectangle {
        rotation: NumberAnimation { running: true; repeat: true; from: 0 to: 360 }
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

int QmlAbstractAnimation::currentTime()
{
    return qtAnimation()->currentLoopTime();
}

void QmlAbstractAnimation::setCurrentTime(int time)
{
    qtAnimation()->setCurrentTime(time);
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
        static_cast<QmlAnimationGroupPrivate *>(d->group->d_func())->animations.removeAll(this);

    d->group = g;

    if (d->group && !static_cast<QmlAnimationGroupPrivate *>(d->group->d_func())->animations.contains(this))
        static_cast<QmlAnimationGroupPrivate *>(d->group->d_func())->animations.append(this);

    if (d->group)
        ((QAnimationGroup*)d->group->qtAnimation())->addAnimation(qtAnimation());

    //if (g) //if removed from a group, then the group should no longer be the parent
        setParent(g);
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
    Rectangle {
        x: NumberAnimation { from: 0; to: 100; duration: 500 }
    }
    \endcode
    was stopped at time 250ms, the \c x property will have a value of 50.

    However, if the \c alwaysRunToEnd property is set, the animation will
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
    Rectangle {
        x: NumberAnimation { from: 0; to: 100; duration: 500 }
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

    if (!d->avoidPropertyValueSourceStart)
        setRunning(true);
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
    if (d->alwaysRunToEnd && d->repeat) {
        qtAnimation()->setLoopCount(-1);
    }
}

/*!
    \qmlclass PauseAnimation QmlPauseAnimation
    \inherits Animation
    \brief The PauseAnimation element provides a pause for an animation.

    When used in a SequentialAnimation, PauseAnimation is a step when
    nothing happens, for a specified duration.

    A 500ms animation sequence, with a 100ms pause between two animations:
    \code
    SequentialAnimation {
        NumberAnimation { ... duration: 200 }
        PauseAnimation { duration: 100 }
        NumberAnimation { ... duration: 200 }
    }
    \endcode
*/
/*!
    \internal
    \class QmlPauseAnimation
*/

QML_DEFINE_TYPE(Qt,4,6,PauseAnimation,QmlPauseAnimation)
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
    pa = new QPauseAnimation;
    QmlGraphics_setParent_noEvent(pa, q);
}

/*!
    \qmlproperty int PauseAnimation::duration
    This property holds the duration of the pause in milliseconds

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
        qmlInfo(this) << tr("Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlPauseAnimation);
    if (d->pa->duration() == duration)
        return;
    d->pa->setDuration(duration);
    emit durationChanged(duration);
}

QAbstractAnimation *QmlPauseAnimation::qtAnimation()
{
    Q_D(QmlPauseAnimation);
    return d->pa;
}

/*!
    \qmlclass ColorAnimation QmlColorAnimation
    \inherits PropertyAnimation
    \brief The ColorAnimation element allows you to animate color changes.

    \code
    ColorAnimation { from: "white"; to: "#c0c0c0"; duration: 100 }
    \endcode

    When used in a transition, ColorAnimation will by default animate
    all properties of type color that are changing. If a property or properties
    are explicitly set for the animation, then those will be used instead.
*/
/*!
    \internal
    \class QmlColorAnimation
*/

QmlColorAnimation::QmlColorAnimation(QObject *parent)
: QmlPropertyAnimation(parent)
{
    Q_D(QmlPropertyAnimation);
    d->interpolatorType = QMetaType::QColor;
    d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
    d->defaultToInterpolatorType = true;
}

QmlColorAnimation::~QmlColorAnimation()
{
}

/*!
    \qmlproperty color ColorAnimation::from
    This property holds the starting color.
*/
QColor QmlColorAnimation::from() const
{
    Q_D(const QmlPropertyAnimation);
    return d->from.value<QColor>();
}

void QmlColorAnimation::setFrom(const QColor &f)
{
    QmlPropertyAnimation::setFrom(f);
}

/*!
    \qmlproperty color ColorAnimation::from
    This property holds the ending color.
*/
QColor QmlColorAnimation::to() const
{
    Q_D(const QmlPropertyAnimation);
    return d->to.value<QColor>();
}

void QmlColorAnimation::setTo(const QColor &t)
{
    QmlPropertyAnimation::setTo(t);
}

QML_DEFINE_TYPE(Qt,4,6,ColorAnimation,QmlColorAnimation)

/*!
    \qmlclass ScriptAction QmlScriptAction
    \inherits Animation
    \brief The ScriptAction element allows scripts to be run during an animation.

*/
/*!
    \internal
    \class QmlScriptAction
*/
QmlScriptAction::QmlScriptAction(QObject *parent)
    :QmlAbstractAnimation(*(new QmlScriptActionPrivate), parent)
{
    Q_D(QmlScriptAction);
    d->init();
}

QmlScriptAction::~QmlScriptAction()
{
}

void QmlScriptActionPrivate::init()
{
    Q_Q(QmlScriptAction);
    rsa = new QActionAnimation(&proxy);
    QmlGraphics_setParent_noEvent(rsa, q);
}

/*!
    \qmlproperty script ScriptAction::script
    This property holds the script to run.
*/
QmlScriptString QmlScriptAction::script() const
{
    Q_D(const QmlScriptAction);
    return d->script;
}

void QmlScriptAction::setScript(const QmlScriptString &script)
{
    Q_D(QmlScriptAction);
    d->script = script;
}

/*!
    \qmlproperty QString ScriptAction::stateChangeScriptName
    This property holds the the name of the StateChangeScript to run.

    This property is only valid when ScriptAction is used as part of a transition.
    If both script and stateChangeScriptName are set, stateChangeScriptName will be used.
*/
QString QmlScriptAction::stateChangeScriptName() const
{
    Q_D(const QmlScriptAction);
    return d->name;
}

void QmlScriptAction::setStateChangeScriptName(const QString &name)
{
    Q_D(QmlScriptAction);
    d->name = name;
}

void QmlScriptActionPrivate::execute()
{
    QmlScriptString scriptStr = hasRunScriptScript ? runScriptScript : script;

    const QString &str = scriptStr.script();
    if (!str.isEmpty()) {
        QmlExpression expr(scriptStr.context(), str, scriptStr.scopeObject());
        expr.setTrackChange(false);
        expr.value();
    }
}

void QmlScriptAction::transition(QmlStateActions &actions,
                                    QmlMetaProperties &modified,
                                    TransitionDirection direction)
{
    Q_D(QmlScriptAction);
    Q_UNUSED(modified);
    Q_UNUSED(direction);

    d->hasRunScriptScript = false;
    for (int ii = 0; ii < actions.count(); ++ii) {
        QmlAction &action = actions[ii];

        if (action.event && action.event->typeName() == QLatin1String("StateChangeScript")
            && static_cast<QmlStateChangeScript*>(action.event)->name() == d->name) {
            //### how should we handle reverse direction?
            d->runScriptScript = static_cast<QmlStateChangeScript*>(action.event)->script();
            d->hasRunScriptScript = true;
            action.actionDone = true;
            break;  //assumes names are unique
        }
    }
}

QAbstractAnimation *QmlScriptAction::qtAnimation()
{
    Q_D(QmlScriptAction);
    return d->rsa;
}

QML_DEFINE_TYPE(Qt,4,6,ScriptAction,QmlScriptAction)

/*!
    \qmlclass PropertyAction QmlPropertyAction
    \inherits Animation
    \brief The PropertyAction element allows immediate property changes during animation.

    Explicitly set \c theimage.smooth=true during a transition:
    \code
    PropertyAction { target: theimage; property: "smooth"; value: true }
    \endcode

    Set \c thewebview.url to the value set for the destination state:
    \code
    PropertyAction { matchTargets: thewebview; matchProperties: "url" }
    \endcode

    The PropertyAction is immediate -
    the target property is not animated to the selected value in any way.
*/
/*!
    \internal
    \class QmlPropertyAction
*/
QmlPropertyAction::QmlPropertyAction(QObject *parent)
: QmlAbstractAnimation(*(new QmlPropertyActionPrivate), parent)
{
    Q_D(QmlPropertyAction);
    d->init();
}

QmlPropertyAction::~QmlPropertyAction()
{
}

void QmlPropertyActionPrivate::init()
{
    Q_Q(QmlPropertyAction);
    spa = new QActionAnimation;
    QmlGraphics_setParent_noEvent(spa, q);
}

/*!
    \qmlproperty Object PropertyAction::target
    This property holds an explicit target object to animate.

    The exact effect of the \c target property depends on how the animation
    is being used.  Refer to the \l animation documentation for details.
*/

QObject *QmlPropertyAction::target() const
{
    Q_D(const QmlPropertyAction);
    return d->target;
}

void QmlPropertyAction::setTarget(QObject *o)
{
    Q_D(QmlPropertyAction);
    if (d->target == o)
        return;

    d->target = o;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = d->createProperty(d->target, d->propertyName, this);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlproperty string PropertyAction::property
    This property holds an explicit property to animated.

    The exact effect of the \c property property depends on how the animation
    is being used.  Refer to the \l animation documentation for details.
*/

QString QmlPropertyAction::property() const
{
    Q_D(const QmlPropertyAction);
    return d->propertyName;
}

void QmlPropertyAction::setProperty(const QString &n)
{
    Q_D(QmlPropertyAction);
    if (d->propertyName == n)
        return;

    d->propertyName = n;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = d->createProperty(d->target, d->propertyName, this);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlproperty string PropertyAction::matchProperties
    This property holds a comma-separated list of property names this action
    will match against. These names are used in conjunction with matchTargets
    to create a list of properties that the action will set, assuming those
    properties have changed.

    This property is typically used for an action appearing as part of a Transition.

    By default, no property names will be matched.

    \sa matchTargets PropertyAnimation::matchProperties
*/
QString QmlPropertyAction::properties() const
{
    Q_D(const QmlPropertyAction);
    return d->properties;
}

void QmlPropertyAction::setProperties(const QString &p)
{
    Q_D(QmlPropertyAction);
    if (d->properties == p)
        return;
    d->properties = p;
    emit propertiesChanged(p);
}

/*!
    \qmlproperty list<Object> PropertyAction::matchTargets
    This property holds a list of objects this action will match against.
    These objects are used in conjunction with matchProperties to create a list of properties
    that the action will set, assuming those properties have changed.

    This property is typically used for an action appearing as part of a Transition.

    By default, all changing targets will be matched.

    \sa exclude matchProperties PropertyAnimation::matchTargets
*/
QList<QObject *> *QmlPropertyAction::targets()
{
    Q_D(QmlPropertyAction);
    return &d->targets;
}

/*!
    \qmlproperty list<Object> PropertyAction::exclude
    This property holds the objects not to be affected by this animation.
    \sa matchTargets
*/
QList<QObject *> *QmlPropertyAction::exclude()
{
    Q_D(QmlPropertyAction);
    return &d->exclude;
}

/*!
    \qmlproperty any PropertyAction::value
    This property holds the value to be set on the property.
    If not set, then the value defined for the end state of the transition.
*/
QVariant QmlPropertyAction::value() const
{
    Q_D(const QmlPropertyAction);
    return d->value;
}

void QmlPropertyAction::setValue(const QVariant &v)
{
    Q_D(QmlPropertyAction);
    if (d->value.isNull || d->value != v) {
        d->value = v;
        emit valueChanged(v);
    }
}

void QmlPropertyActionPrivate::doAction()
{
    property.write(value, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
}

QAbstractAnimation *QmlPropertyAction::qtAnimation()
{
    Q_D(QmlPropertyAction);
    return d->spa;
}

void QmlPropertyAction::prepare(QmlMetaProperty &p)
{
    Q_D(QmlPropertyAction);

    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    d->spa->setAnimAction(&d->proxy, QAbstractAnimation::KeepWhenStopped);
}

void QmlPropertyAction::transition(QmlStateActions &actions,
                                      QmlMetaProperties &modified,
                                      TransitionDirection direction)
{
    Q_D(QmlPropertyAction);
    Q_UNUSED(direction);

    struct QmlSetPropertyAnimationAction : public QAbstractAnimationAction
    {
        QmlStateActions actions;
        virtual void doAction()
        {
            for (int ii = 0; ii < actions.count(); ++ii) {
                const QmlAction &action = actions.at(ii);
                action.property.write(action.toValue, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
            }
        }
    };

    QStringList props = d->properties.isEmpty() ? QStringList() : d->properties.split(QLatin1Char(','));
    for (int ii = 0; ii < props.count(); ++ii)
        props[ii] = props.at(ii).trimmed();

    bool hasSelectors = !props.isEmpty() || !d->targets.isEmpty() || !d->exclude.isEmpty();
    bool hasTarget = !d->propertyName.isEmpty() || d->target;

    if (hasSelectors && hasTarget) {
        qmlInfo(this) << tr("matchTargets/matchProperties/exclude and target/property are mutually exclusive.");
        return;
    }

    QmlSetPropertyAnimationAction *data = new QmlSetPropertyAnimationAction;

    bool hasExplicit = false;
    if (hasTarget && d->value.isValid()) {
        QmlAction myAction;
        myAction.property = d->createProperty(target(), d->propertyName, this);
        if (myAction.property.isValid()) {
            myAction.toValue = d->value;
            data->actions << myAction;
            hasExplicit = true;
            for (int ii = 0; ii < actions.count(); ++ii) {
                QmlAction &action = actions[ii];
                if (action.property.object() == myAction.property.object() &&
                    myAction.property.name() == action.property.name()) {
                    modified << action.property;
                    break;  //### any chance there could be multiples?
                }
            }
        }
    }

    if (!hasExplicit)
    for (int ii = 0; ii < actions.count(); ++ii) {
        QmlAction &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();
        QObject *sObj = action.specifiedObject;
        QString sPropertyName = action.specifiedProperty;
        bool same = (obj == sObj);

        if ((d->targets.isEmpty() || d->targets.contains(obj) || (!same && d->targets.contains(sObj))) &&
           (!d->exclude.contains(obj)) && (same || (!d->exclude.contains(sObj))) &&
           (props.contains(propertyName) || (!same && props.contains(sPropertyName)))) {
            QmlAction myAction = action;

            if (d->value.isValid())
                myAction.toValue = d->value;

            modified << action.property;
            data->actions << myAction;
            action.fromValue = myAction.toValue;
        } else if (d->userProperty.isValid() &&
            !hasSelectors && !hasTarget) {
            if ((d->userProperty.value.object() == obj || (!same && d->userProperty.value.object() == sObj)) &&
               (d->userProperty.value.name() == propertyName || (!same && d->userProperty.value.name() == sPropertyName))) {
                //### same as above. merge
                QmlAction myAction = action;

                if (d->value.isValid())
                    myAction.toValue = d->value;

                modified << action.property;
                data->actions << myAction;
                action.fromValue = myAction.toValue;
            }
        }
    }

    if (data->actions.count()) {
        d->spa->setAnimAction(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(Qt,4,6,PropertyAction,QmlPropertyAction)

/*!
    \qmlclass ParentAction QmlParentAction
    \inherits Animation
    \brief The ParentAction element allows parent changes during animation.

    ParentAction provides a way to specify at what point in a Transition a ParentChange should
    occur.
    \qml
    State {
        ParentChange {
            target: myItem
            parent: newParent
        }
    }
    Transition {
        SequentialAnimation {
            PropertyAnimation { ... }
            ParentAction {}   //reparent myItem now
            PropertyAnimation { ... }
        }
    }
    \endqml

    It also provides a way to explicitly reparent an item during an animation.
    \qml
    SequentialAnimation {
        ParentAction { target: myItem; parent: newParent }
        PropertyAnimation {}
    }
    \endqml

    The ParentAction is immediate - it is not animated in any way.
*/

QmlParentAction::QmlParentAction(QObject *parent)
: QmlAbstractAnimation(*(new QmlParentActionPrivate), parent)
{
    Q_D(QmlParentAction);
    d->init();
}

QmlParentAction::~QmlParentAction()
{
}

void QmlParentActionPrivate::init()
{
    Q_Q(QmlParentAction);
    cpa = new QActionAnimation;
    QmlGraphics_setParent_noEvent(cpa, q);
}

/*!
    \qmlproperty Item ParentAction::target

    This property holds an explicit target item to reparent.
 */
QmlGraphicsItem *QmlParentAction::object() const
{
    Q_D(const QmlParentAction);
    return d->pcTarget;
}

void QmlParentAction::setObject(QmlGraphicsItem *target)
{
    Q_D(QmlParentAction);
    d->pcTarget = target;
}

/*!
    \qmlproperty Item ParentAction::matchTarget
    This property holds the item this action will match against -- the item
    that the action will reparent, assuming its parent has changed.

    In the following example, \c myItem will be reparented by the ParentAction, while
    \c myOtherItem will not.
    \qml
    State {
        ParentChange {
            target: myItem
            parent: newParent
        }
        ParentChange {
            target: myOtherItem
            parent: otherNewParent
        }
    }
    Transition {
        SequentialAnimation {
            PropertyAnimation { ... }
            ParentAction { matchTargets: myItem }
            PropertyAnimation { ... }
        }
    }
    \endqml

    This property is typically used for an action appearing as part of a Transition.
 */
QmlGraphicsItem *QmlParentAction::matchTarget() const
{
    Q_D(const QmlParentAction);
    return d->pcTarget;
}

void QmlParentAction::setMatchTarget(QmlGraphicsItem *target)
{
    Q_D(QmlParentAction);
    d->pcMatchTarget = target;
}

/*!
    \qmlproperty Item ParentAction::parent

    The item to reparent to (i.e. the new parent).
 */
QmlGraphicsItem *QmlParentAction::parent() const
{
    Q_D(const QmlParentAction);
    return d->pcParent;
}

void QmlParentAction::setParent(QmlGraphicsItem *parent)
{
    Q_D(QmlParentAction);
    d->pcParent = parent;
}

void QmlParentActionPrivate::doAction()
{
    QmlParentChange pc;
    pc.setObject(pcTarget);
    pc.setParent(pcParent);
    pc.execute();
}

QAbstractAnimation *QmlParentAction::qtAnimation()
{
    Q_D(QmlParentAction);
    return d->cpa;
}

void QmlParentAction::transition(QmlStateActions &actions,
                                       QmlMetaProperties &modified,
                                       TransitionDirection direction)
{
    Q_D(QmlParentAction);
    Q_UNUSED(modified);
    Q_UNUSED(direction);

    struct QmlParentActionData : public QAbstractAnimationAction
    {
        QmlParentActionData(): pc(0) {}
        ~QmlParentActionData() { delete pc; }

        QmlStateActions actions;
        bool reverse;
        QmlParentChange *pc;
        virtual void doAction()
        {
            for (int ii = 0; ii < actions.count(); ++ii) {
                const QmlAction &action = actions.at(ii);
                if (reverse)
                    action.event->reverse();
                else
                    action.event->execute();
            }
        }
    };

    QmlParentActionData *data = new QmlParentActionData;

    if (d->pcTarget && d->pcMatchTarget) {
        qmlInfo(this) << tr("matchTarget and target are mutually exclusive.");
        return;
    }

    for (int ii = 0; ii < actions.count(); ++ii) {
        QmlAction &action = actions[ii];

        if (action.event && action.event->typeName() == QLatin1String("ParentChange")
            && !d->pcTarget
            && (!d->pcMatchTarget || static_cast<QmlParentChange*>(action.event)->object() == d->pcMatchTarget)) {
            QmlAction myAction = action;
            data->reverse = action.reverseEvent;
            //### this logic differs from PropertyAnimation
            //    (probably a result of modified vs. done)
            if (d->pcParent) {
                //### should we disallow this case?
                QmlParentChange *pc = new QmlParentChange;
                pc->setObject(d->pcTarget);
                pc->setParent(static_cast<QmlParentChange*>(action.event)->parent());
                myAction.event = pc;
                data->pc = pc;
                data->actions << myAction;
                break;  //only match one
            } else {
                action.actionDone = true;
                data->actions << myAction;
            }
        }
    }

    if (d->pcTarget && d->pcParent) {
        data->reverse = false;
        QmlAction myAction;
        QmlParentChange *pc = new QmlParentChange;
        pc->setObject(d->pcTarget);
        pc->setParent(d->pcParent);
        myAction.event = pc;
        data->pc = pc;
        data->actions << myAction;
    }

    if (data->actions.count()) {
        d->cpa->setAnimAction(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(Qt,4,6,ParentAction,QmlParentAction)

/*!
    \qmlclass NumberAnimation QmlNumberAnimation
    \inherits PropertyAnimation
    \brief The NumberAnimation element allows you to animate changes in properties of type qreal.

    Animate a set of properties over 200ms, from their values in the start state to
    their values in the end state of the transition:
    \code
    NumberAnimation { matchProperties: "x,y,scale"; duration: 200 }
    \endcode
*/

/*!
    \internal
    \class QmlNumberAnimation
*/

QmlNumberAnimation::QmlNumberAnimation(QObject *parent)
: QmlPropertyAnimation(parent)
{
    Q_D(QmlPropertyAnimation);
    d->interpolatorType = QMetaType::QReal;
    d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
}

QmlNumberAnimation::~QmlNumberAnimation()
{
}

/*!
    \qmlproperty real NumberAnimation::from
    This property holds the starting value.
    If not set, then the value defined in the start state of the transition.
*/
qreal QmlNumberAnimation::from() const
{
    Q_D(const QmlPropertyAnimation);
    return d->from.toReal();
}

void QmlNumberAnimation::setFrom(qreal f)
{
    QmlPropertyAnimation::setFrom(f);
}

/*!
    \qmlproperty real NumberAnimation::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition.
*/
qreal QmlNumberAnimation::to() const
{
    Q_D(const QmlPropertyAnimation);
    return d->to.toReal();
}

void QmlNumberAnimation::setTo(qreal t)
{
    QmlPropertyAnimation::setTo(t);
}

QML_DEFINE_TYPE(Qt,4,6,NumberAnimation,QmlNumberAnimation)

/*!
    \qmlclass Vector3dAnimation QmlVector3dAnimation
    \inherits PropertyAnimation
    \brief The Vector3dAnimation element allows you to animate changes in properties of type QVector3d.
*/

/*!
    \internal
    \class QmlVector3dAnimation
*/

QmlVector3dAnimation::QmlVector3dAnimation(QObject *parent)
: QmlPropertyAnimation(parent)
{
    Q_D(QmlPropertyAnimation);
    d->interpolatorType = QMetaType::QVector3D;
    d->interpolator = QVariantAnimationPrivate::getInterpolator(d->interpolatorType);
	d->defaultToInterpolatorType = true;
}

QmlVector3dAnimation::~QmlVector3dAnimation()
{
}

/*!
    \qmlproperty real Vector3dAnimation::from
    This property holds the starting value.
    If not set, then the value defined in the start state of the transition.
*/
QVector3D QmlVector3dAnimation::from() const
{
    Q_D(const QmlPropertyAnimation);
    return d->from.value<QVector3D>();
}

void QmlVector3dAnimation::setFrom(QVector3D f)
{
    QmlPropertyAnimation::setFrom(f);
}

/*!
    \qmlproperty real Vector3dAnimation::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition.
*/
QVector3D QmlVector3dAnimation::to() const
{
    Q_D(const QmlPropertyAnimation);
    return d->to.value<QVector3D>();
}

void QmlVector3dAnimation::setTo(QVector3D t)
{
    QmlPropertyAnimation::setTo(t);
}

QML_DEFINE_TYPE(Qt,4,6,Vector3dAnimation,QmlVector3dAnimation)

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
    \brief The SequentialAnimation element allows you to run animations sequentially.

    Animations controlled in SequentialAnimation will be run one after the other.

    The following example chains two numeric animations together.  The \c MyItem
    object will animate from its current x position to 100, and then back to 0.

    \code
    SequentialAnimation {
        NumberAnimation { target: MyItem; property: "x"; to: 100 }
        NumberAnimation { target: MyItem; property: "x"; to: 0 }
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

    //needed for Behavior
    if (d->userProperty.isValid()) {
        for (int i = 0; i < d->animations.count(); ++i)
            d->animations.at(i)->setTarget(d->userProperty);
    }

    for (int ii = from; ii < d->animations.count() && ii >= 0; ii += inc) {
        d->animations.at(ii)->transition(actions, modified, direction);
    }
}

QML_DEFINE_TYPE(Qt,4,6,SequentialAnimation,QmlSequentialAnimation)

/*!
    \qmlclass ParallelAnimation QmlParallelAnimation
    \inherits Animation
    \brief The ParallelAnimation element allows you to run animations in parallel.

    Animations contained in ParallelAnimation will be run at the same time.

    The following animation demonstrates animating the \c MyItem item
    to (100,100) by animating the x and y properties in parallel.

    \code
    ParallelAnimation {
        NumberAnimation { target: MyItem; property: "x"; to: 100 }
        NumberAnimation { target: MyItem; property: "y"; to: 100 }
    }
    \endcode

    \sa SequentialAnimation
*/
/*!
    \internal
    \class QmlParallelAnimation
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

    //needed for Behavior
    if (d->userProperty.isValid()) {
        for (int i = 0; i < d->animations.count(); ++i)
            d->animations.at(i)->setTarget(d->userProperty);
    }

    for (int ii = 0; ii < d->animations.count(); ++ii) {
        d->animations.at(ii)->transition(actions, modified, direction);
    }
}

QML_DEFINE_TYPE(Qt,4,6,ParallelAnimation,QmlParallelAnimation)

//convert a variant from string type to another animatable type
void QmlPropertyAnimationPrivate::convertVariant(QVariant &variant, int type)
{
    if (variant.userType() != QVariant::String) {
        variant.convert((QVariant::Type)type);
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
    case QVariant::Vector3D: {
        variant.setValue(QmlStringConverters::vector3DFromString(variant.toString()));
        break;
    }
    default:
        if ((uint)type >= QVariant::UserType) {
            QmlMetaType::StringConverter converter = QmlMetaType::customStringConverter(type);
            if (converter)
                variant = converter(variant.toString());
        } else
            variant.convert((QVariant::Type)type);
        break;
    }
}

/*!
    \qmlclass PropertyAnimation QmlPropertyAnimation
    \inherits Animation
    \brief The PropertyAnimation element allows you to animate property changes.

    Animate theObject's size property over 200ms, from its current size to 20-by-20:
    \code
    PropertyAnimation { target: theObject; property: "size"; to: "20x20"; duration: 200 }
    \endcode

    For an introduction to animation in QML, see \l{QML Animation}.
*/

QmlPropertyAnimation::QmlPropertyAnimation(QObject *parent)
: QmlAbstractAnimation(*(new QmlPropertyAnimationPrivate), parent)
{
    Q_D(QmlPropertyAnimation);
    d->init();
}

QmlPropertyAnimation::~QmlPropertyAnimation()
{
}

void QmlPropertyAnimationPrivate::init()
{
    Q_Q(QmlPropertyAnimation);
    va = new QmlTimeLineValueAnimator;
    QmlGraphics_setParent_noEvent(va, q);
}

/*!
    \qmlproperty int PropertyAnimation::duration
    This property holds the duration of the transition, in milliseconds.

    The default value is 250.
*/
int QmlPropertyAnimation::duration() const
{
    Q_D(const QmlPropertyAnimation);
    return d->va->duration();
}

void QmlPropertyAnimation::setDuration(int duration)
{
    if (duration < 0) {
        qmlInfo(this) << tr("Cannot set a duration of < 0");
        return;
    }

    Q_D(QmlPropertyAnimation);
    if (d->va->duration() == duration)
        return;
    d->va->setDuration(duration);
    emit durationChanged(duration);
}

/*!
    \qmlproperty real PropertyAnimation::from
    This property holds the starting value.
    If not set, then the value defined in the start state of the transition.
*/
QVariant QmlPropertyAnimation::from() const
{
    Q_D(const QmlPropertyAnimation);
    return d->from;
}

void QmlPropertyAnimation::setFrom(const QVariant &f)
{
    Q_D(QmlPropertyAnimation);
    if (d->fromIsDefined && f == d->from)
        return;
    d->from = f;
    d->fromIsDefined = f.isValid();
    emit fromChanged(f);
}

/*!
    \qmlproperty real PropertyAnimation::to
    This property holds the ending value.
    If not set, then the value defined in the end state of the transition.
*/
QVariant QmlPropertyAnimation::to() const
{
    Q_D(const QmlPropertyAnimation);
    return d->to;
}

void QmlPropertyAnimation::setTo(const QVariant &t)
{
    Q_D(QmlPropertyAnimation);
    if (d->toIsDefined && t == d->to)
        return;
    d->to = t;
    d->toIsDefined = t.isValid();
    emit toChanged(t);
}

/*!
    \qmlproperty string PropertyAnimation::easing
    \brief the easing curve used for the transition.

    Available values are:

    \table
    \row
        \o \c easeLinear
        \o Easing curve for a linear (t) function: velocity is constant.
        \o \inlineimage qeasingcurve-linear.png
    \row
        \o \c easeInQuad
        \o Easing curve for a quadratic (t^2) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inquad.png
    \row
        \o \c easeOutQuad
        \o Easing curve for a quadratic (t^2) function: decelerating to zero velocity.
        \o \inlineimage qeasingcurve-outquad.png
    \row
        \o \c easeInOutQuad
        \o Easing curve for a quadratic (t^2) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutquad.png
    \row
        \o \c easeOutInQuad
        \o Easing curve for a quadratic (t^2) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinquad.png
    \row
        \o \c easeInCubic
        \o Easing curve for a cubic (t^3) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-incubic.png
    \row
        \o \c easeOutCubic
        \o Easing curve for a cubic (t^3) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outcubic.png
    \row
        \o \c easeInOutCubic
        \o Easing curve for a cubic (t^3) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutcubic.png
    \row
        \o \c easeOutInCubic
        \o Easing curve for a cubic (t^3) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outincubic.png
    \row
        \o \c easeInQuart
        \o Easing curve for a quartic (t^4) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inquart.png
    \row
        \o \c easeOutQuart
        \o Easing curve for a cubic (t^4) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outquart.png
    \row
        \o \c easeInOutQuart
        \o Easing curve for a cubic (t^4) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutquart.png
    \row
        \o \c easeOutInQuart
        \o Easing curve for a cubic (t^4) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinquart.png
    \row
        \o \c easeInQuint
        \o Easing curve for a quintic (t^5) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inquint.png
    \row
        \o \c easeOutQuint
        \o Easing curve for a cubic (t^5) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outquint.png
    \row
        \o \c easeInOutQuint
        \o Easing curve for a cubic (t^5) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutquint.png
    \row
        \o \c easeOutInQuint
        \o Easing curve for a cubic (t^5) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinquint.png
    \row
        \o \c easeInSine
        \o Easing curve for a sinusoidal (sin(t)) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-insine.png
    \row
        \o \c easeOutSine
        \o Easing curve for a sinusoidal (sin(t)) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outsine.png
    \row
        \o \c easeInOutSine
        \o Easing curve for a sinusoidal (sin(t)) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutsine.png
    \row
        \o \c easeOutInSine
        \o Easing curve for a sinusoidal (sin(t)) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinsine.png
    \row
        \o \c easeInExpo
        \o Easing curve for an exponential (2^t) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inexpo.png
    \row
        \o \c easeOutExpo
        \o Easing curve for an exponential (2^t) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outexpo.png
    \row
        \o \c easeInOutExpo
        \o Easing curve for an exponential (2^t) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutexpo.png
    \row
        \o \c easeOutInExpo
        \o Easing curve for an exponential (2^t) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinexpo.png
    \row
        \o \c easeInCirc
        \o Easing curve for a circular (sqrt(1-t^2)) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-incirc.png
    \row
        \o \c easeOutCirc
        \o Easing curve for a circular (sqrt(1-t^2)) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outcirc.png
    \row
        \o \c easeInOutCirc
        \o Easing curve for a circular (sqrt(1-t^2)) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutcirc.png
    \row
        \o \c easeOutInCirc
        \o Easing curve for a circular (sqrt(1-t^2)) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outincirc.png
    \row
        \o \c easeInElastic
        \o Easing curve for an elastic (exponentially decaying sine wave) function: accelerating from zero velocity.
        \br The peak amplitude can be set with the \e amplitude parameter, and the period of decay by the \e period parameter.
        \o \inlineimage qeasingcurve-inelastic.png
    \row
        \o \c easeOutElastic
        \o Easing curve for an elastic (exponentially decaying sine wave) function: decelerating from zero velocity.
        \br The peak amplitude can be set with the \e amplitude parameter, and the period of decay by the \e period parameter.
        \o \inlineimage qeasingcurve-outelastic.png
    \row
        \o \c easeInOutElastic
        \o Easing curve for an elastic (exponentially decaying sine wave) function: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutelastic.png
    \row
        \o \c easeOutInElastic
        \o Easing curve for an elastic (exponentially decaying sine wave) function: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinelastic.png
    \row
        \o \c easeInBack
        \o Easing curve for a back (overshooting cubic function: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inback.png
    \row
        \o \c easeOutBack
        \o Easing curve for a back (overshooting cubic function: (s+1)*t^3 - s*t^2) easing out: decelerating to zero velocity.
        \o \inlineimage qeasingcurve-outback.png
    \row
        \o \c easeInOutBack
        \o Easing curve for a back (overshooting cubic function: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutback.png
    \row
        \o \c easeOutInBack
        \o Easing curve for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinback.png
    \row
        \o \c easeInBounce
        \o Easing curve for a bounce (exponentially decaying parabolic bounce) function: accelerating from zero velocity.
        \o \inlineimage qeasingcurve-inbounce.png
    \row
        \o \c easeOutBounce
        \o Easing curve for a bounce (exponentially decaying parabolic bounce) function: decelerating from zero velocity.
        \o \inlineimage qeasingcurve-outbounce.png
    \row
        \o \c easeInOutBounce
        \o Easing curve for a bounce (exponentially decaying parabolic bounce) function easing in/out: acceleration until halfway, then deceleration.
        \o \inlineimage qeasingcurve-inoutbounce.png
    \row
        \o \c easeOutInBounce
        \o Easing curve for a bounce (exponentially decaying parabolic bounce) function easing out/in: deceleration until halfway, then acceleration.
        \o \inlineimage qeasingcurve-outinbounce.png
    \endtable

*/
QString QmlPropertyAnimation::easing() const
{
    Q_D(const QmlPropertyAnimation);
    return d->easing;
}

void QmlPropertyAnimation::setEasing(const QString &e)
{
    Q_D(QmlPropertyAnimation);
    if (d->easing == e)
        return;

    d->easing = e;
    d->va->setEasingCurve(stringToCurve(d->easing, this));
    emit easingChanged(e);
}

/*!
    \qmlproperty Object PropertyAnimation::target
    This property holds an explicit target object to animate.

    target is used in conjunction with property to determine
    what property should be animated.

    \sa property matchTargets
*/

QObject *QmlPropertyAnimation::target() const
{
    Q_D(const QmlPropertyAnimation);
    return d->target;
}

void QmlPropertyAnimation::setTarget(QObject *o)
{
    Q_D(QmlPropertyAnimation);
    if (d->target == o)
        return;

    d->target = o;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = d->createProperty(d->target, d->propertyName, this);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlproperty string PropertyAnimation::property
    This property holds an explicit property name to animate.

    property is used in conjunction with target to determine
    what property should be animated.

    \sa target matchProperties
*/

QString QmlPropertyAnimation::property() const
{
    Q_D(const QmlPropertyAnimation);
    return d->propertyName;
}

void QmlPropertyAnimation::setProperty(const QString &n)
{
    Q_D(QmlPropertyAnimation);
    if (d->propertyName == n)
        return;

    d->propertyName = n;
    if (d->target && !d->propertyName.isEmpty()) {
        d->userProperty = d->createProperty(d->target, d->propertyName, this);
    } else {
        d->userProperty.invalidate();
    }

    emit targetChanged(d->target, d->propertyName);
}

/*!
    \qmlproperty string PropertyAnimation::matchProperties
    This property holds a comma-separated list of property names this animation
    will match against. These names are used in conjunction with matchTargets
    to create a list of properties that the animation will animate, assuming those
    properties have changed.

    In the following example, the change in 'x' will be animated by the transition, while
    the change in 'y' will not.
    \qml
    State {
        PropertyChanges {
            target: myItem
            x: 15; y: 15
        }
    }
    Transition {
        PropertyAnimation {
            matchProperties: "x"
        }
    }
    \endqml

    This property is typically used for an animation appearing as part of a Transition.

    By default, no property names will be matched.

    \sa matchTargets PropertyAction::matchTargets
*/
QString QmlPropertyAnimation::properties() const
{
    Q_D(const QmlPropertyAnimation);
    return d->properties;
}

void QmlPropertyAnimation::setProperties(const QString &prop)
{
    Q_D(QmlPropertyAnimation);
    if (d->properties == prop)
        return;

    d->properties = prop;
    emit propertiesChanged(prop);
}

/*!
    \qmlproperty list<Object> PropertyAnimation::matchTargets
    This property holds a list of objects this animation will match against.
    These objects are used in conjunction with matchProperties to create a list of properties
    that the animation will animate, assuming those properties have changed.

    In the following example, the changes to \c myItem will be animated by the transition, while
    the changes to \c myOtherItem will not.
    \qml
    State {
        PropertyChanges {
            target: myItem
            x: 15; y: 15
        }
        PropertyChanges {
            target: myOtherItem
            x: 30; y: 30
        }
    }
    Transition {
        PropertyAnimation {
            matchTargets: myItem
            matchProperties: "x,y"
        }
    }
    \endqml

    This property is typically used for an animation appearing as part of a Transition.

    By default, all changing targets will be matched.
    
    \sa exclude matchProperties
*/
QList<QObject *> *QmlPropertyAnimation::targets()
{
    Q_D(QmlPropertyAnimation);
    return &d->targets;
}

/*!
    \qmlproperty list<Object> PropertyAnimation::exclude
    This property holds the items not to be affected by this animation.
    \sa matchTargets
*/
QList<QObject *> *QmlPropertyAnimation::exclude()
{
    Q_D(QmlPropertyAnimation);
    return &d->exclude;
}

void QmlPropertyAnimationPrivate::valueChanged(qreal r)
{
    if (!fromSourced) {
        if (!fromIsDefined) {
            from = property.read();
            convertVariant(from, interpolatorType ? interpolatorType : property.propertyType());
            //### check for invalid variant if using property type
        }
        fromSourced = true;
    }

    if (r == 1.) {
        property.write(to, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
    } else {
        if (interpolator)
            property.write(interpolator(from.constData(), to.constData(), r), QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
    }
}

QAbstractAnimation *QmlPropertyAnimation::qtAnimation()
{
    Q_D(QmlPropertyAnimation);
    return d->va;
}

void QmlPropertyAnimation::prepare(QmlMetaProperty &p)
{
    Q_D(QmlPropertyAnimation);
    if (d->userProperty.isNull)
        d->property = p;
    else
        d->property = d->userProperty;

    if (!d->rangeIsSet) {
        d->va->setStartValue(qreal(0));
        d->va->setEndValue(qreal(1));
        d->rangeIsSet = true;
    }

    int propType = d->property.propertyType();
    d->convertVariant(d->to, d->interpolatorType ? d->interpolatorType : propType);
    if (d->fromIsDefined)
        d->convertVariant(d->from, d->interpolatorType ? d->interpolatorType : propType);

    if (!d->interpolatorType) {
        //### check for invalid variants
        d->interpolator = QVariantAnimationPrivate::getInterpolator(propType);
    }

    d->fromSourced = false;
    d->value.QmlTimeLineValue::setValue(0.);
    d->va->setAnimValue(&d->value, QAbstractAnimation::KeepWhenStopped);
    d->va->setFromSourcedValue(&d->fromSourced);
}

void QmlPropertyAnimation::transition(QmlStateActions &actions,
                                     QmlMetaProperties &modified,
                                     TransitionDirection direction)
{
    Q_D(QmlPropertyAnimation);
    Q_UNUSED(direction);

    struct PropertyUpdater : public QmlTimeLineValue
    {
        QmlStateActions actions;
        int interpolatorType;       //for Number/ColorAnimation
        int prevInterpolatorType;   //for generic
        QVariantAnimation::Interpolator interpolator;
        bool reverse;
        bool *wasDeleted;
        PropertyUpdater() : wasDeleted(0) {}
        ~PropertyUpdater() { if (wasDeleted) *wasDeleted = true; }
        void setValue(qreal v)
        {
            bool deleted = false;
            wasDeleted = &deleted;
            if (reverse)    //QVariantAnimation sends us 1->0 when reversed, but we are expecting 0->1
                v = 1 - v;
            QmlTimeLineValue::setValue(v);
            for (int ii = 0; ii < actions.count(); ++ii) {
                QmlAction &action = actions[ii];

                if (v == 1.)
                    action.property.write(action.toValue, QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
                else {
                    if (action.fromValue.isNull()) {
                        action.fromValue = action.property.read();
                        if (interpolatorType)
                            QmlPropertyAnimationPrivate::convertVariant(action.fromValue, interpolatorType);
                    }
                    if (!interpolatorType) {
                        int propType = action.property.propertyType();
                        if (!prevInterpolatorType || prevInterpolatorType != propType) {
                            prevInterpolatorType = propType;
                            interpolator = QVariantAnimationPrivate::getInterpolator(prevInterpolatorType);
                        }
                    }
                    if (interpolator)
                        action.property.write(interpolator(action.fromValue.constData(), action.toValue.constData(), v), QmlMetaProperty::BypassInterceptor | QmlMetaProperty::DontRemoveBinding);
                }
                if (deleted)
                    return;
            }
            wasDeleted = 0;
        }
    };

    QStringList props = d->properties.isEmpty() ? QStringList() : d->properties.split(QLatin1Char(','));
    for (int ii = 0; ii < props.count(); ++ii)
        props[ii] = props.at(ii).trimmed();

    bool hasSelectors = !props.isEmpty() || !d->targets.isEmpty() || !d->exclude.isEmpty();
    bool hasTarget = !d->propertyName.isEmpty() || d->target;

    if (hasSelectors && hasTarget) {
        qmlInfo(this) << tr("matchTargets/matchProperties/exclude and target/property are mutually exclusive.");
        return;
    }

    bool useType = (props.isEmpty() && d->propertyName.isEmpty() && d->defaultToInterpolatorType) ? true : false;

    PropertyUpdater *data = new PropertyUpdater;
    data->interpolatorType = d->interpolatorType;
    data->interpolator = d->interpolator;
    data->reverse = direction == Backward ? true : false;

    bool hasExplicit = false;
    //an explicit animation has been specified
    if (hasTarget && d->toIsDefined) {
        QmlAction myAction;
        myAction.property = d->createProperty(target(), d->propertyName, this);
        if (myAction.property.isValid()) {
            if (d->fromIsDefined) {
                d->convertVariant(d->from, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());
                myAction.fromValue = d->from;
            }
            d->convertVariant(d->to, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());
            myAction.toValue = d->to;
            data->actions << myAction;
            hasExplicit = true;
            for (int ii = 0; ii < actions.count(); ++ii) {
                QmlAction &action = actions[ii];
                if (action.property.object() == myAction.property.object() &&
                    myAction.property.name() == action.property.name()) {
                    modified << action.property;
                    break;  //### any chance there could be multiples?
                }
            }
        }
    }

    if (!hasExplicit)
    for (int ii = 0; ii < actions.count(); ++ii) {
        QmlAction &action = actions[ii];

        QObject *obj = action.property.object();
        QString propertyName = action.property.name();
        QObject *sObj = action.specifiedObject;
        QString sPropertyName = action.specifiedProperty;
        bool same = (obj == sObj);

        if ((d->targets.isEmpty() || d->targets.contains(obj) || (!same && d->targets.contains(sObj))) &&
           (!d->exclude.contains(obj)) && (same || (!d->exclude.contains(sObj))) &&
           (props.contains(propertyName) || (!same && props.contains(sPropertyName))
               || (useType && action.property.propertyType() == d->interpolatorType))) {
            QmlAction myAction = action;

            if (d->fromIsDefined)
                myAction.fromValue = d->from;
            else
                myAction.fromValue = QVariant();
            if (d->toIsDefined)
                myAction.toValue = d->to;

            d->convertVariant(myAction.fromValue, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());
            d->convertVariant(myAction.toValue, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());

            modified << action.property;

            data->actions << myAction;
            action.fromValue = myAction.toValue;
        } else if (d->userProperty.isValid() &&
                   !hasSelectors && !hasTarget) {
           if ((d->userProperty.value.object() == obj || (!same && d->userProperty.value.object() == sObj)) &&
              (d->userProperty.value.name() == propertyName || (!same && d->userProperty.value.name() == sPropertyName))) {
               //### same as above. merge
               QmlAction myAction = action;

               if (d->fromIsDefined)
                   myAction.fromValue = d->from;
               else
                   myAction.fromValue = QVariant();
               if (d->toIsDefined)
                   myAction.toValue = d->to;

               d->convertVariant(myAction.fromValue, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());
               d->convertVariant(myAction.toValue, d->interpolatorType ? d->interpolatorType : myAction.property.propertyType());

               modified << action.property;

               data->actions << myAction;
               action.fromValue = myAction.toValue;
           }
       }
    }

    if (data->actions.count()) {
        if (!d->rangeIsSet) {
            d->va->setStartValue(qreal(0));
            d->va->setEndValue(qreal(1));
            d->rangeIsSet = true;
        }
        d->va->setAnimValue(data, QAbstractAnimation::DeleteWhenStopped);
    } else {
        delete data;
    }
}

QML_DEFINE_TYPE(Qt,4,6,PropertyAnimation,QmlPropertyAnimation)

QT_END_NAMESPACE
