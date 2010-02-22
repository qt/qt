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

#ifndef QMLANIMATION_P_H
#define QMLANIMATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmlanimation_p.h"

#include "qmlnullablevalue_p_p.h"
#include "qmltimeline_p_p.h"

#include <qml.h>
#include <qmlcontext.h>

#include <QtCore/QPauseAnimation>
#include <QtCore/QVariantAnimation>
#include <QtCore/QAnimationGroup>
#include <QtGui/QColor>
#include <QDebug>

#include <private/qobject_p.h>
#include <private/qvariantanimation_p.h>

QT_BEGIN_NAMESPACE

//interface for classes that provide animation actions for QActionAnimation
class QAbstractAnimationAction
{
public:
    virtual ~QAbstractAnimationAction() {}
    virtual void doAction() = 0;
};

//templated animation action
//allows us to specify an action that calls a function of a class.
//(so that class doesn't have to inherit QmlAbstractAnimationAction)
template<class T, void (T::*method)()>
class QAnimationActionProxy : public QAbstractAnimationAction
{
public:
    QAnimationActionProxy(T *p) : m_p(p) {}
    virtual void doAction() { (m_p->*method)(); }

private:
    T *m_p;
};

//performs an action of type QAbstractAnimationAction
class QActionAnimation : public QAbstractAnimation
{
    Q_OBJECT
public:
    QActionAnimation(QObject *parent = 0) : QAbstractAnimation(parent), animAction(0), policy(KeepWhenStopped), running(false) {}
    QActionAnimation(QAbstractAnimationAction *action, QObject *parent = 0)
        : QAbstractAnimation(parent), animAction(action), policy(KeepWhenStopped), running(false) {}
    ~QActionAnimation() { if (policy == DeleteWhenStopped) { delete animAction; animAction = 0; } }
    virtual int duration() const { return 0; }
    void setAnimAction(QAbstractAnimationAction *action, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
        if (policy == DeleteWhenStopped)
            delete animAction;
        animAction = action;
        policy = p;
    }
protected:
    virtual void updateCurrentTime(int) {}

    virtual void updateState(State newState, State /*oldState*/)
    {
        if (newState == Running) {
            if (animAction) {
                running = true;
                animAction->doAction();
                running = false;
                if (state() == Stopped && policy == DeleteWhenStopped) {
                    delete animAction;
                    animAction = 0;
                }
            }
        } /*else if (newState == Stopped && policy == DeleteWhenStopped) {
            if (!running) {
                delete animAction;
                animAction = 0;
            }
        }*/
    }

private:
    QAbstractAnimationAction *animAction;
    DeletionPolicy policy;
    bool running;
};

//animates QmlTimeLineValue (assumes start and end values will be reals or compatible)
class QmlTimeLineValueAnimator : public QVariantAnimation
{
    Q_OBJECT
public:
    QmlTimeLineValueAnimator(QObject *parent = 0) : QVariantAnimation(parent), animValue(0), fromSourced(0), policy(KeepWhenStopped) {}
    ~QmlTimeLineValueAnimator() { if (policy == DeleteWhenStopped) { delete animValue; animValue = 0; } }
    void setAnimValue(QmlTimeLineValue *value, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
        if (policy == DeleteWhenStopped)
            delete animValue;
        animValue = value;
        policy = p;
    }
    void setFromSourcedValue(bool *value)
    {
        fromSourced = value;
    }
protected:
    virtual void updateCurrentValue(const QVariant &value)
    {
        if (animValue)
            animValue->setValue(value.toReal());
    }
    virtual void updateState(State newState, State oldState)
    {   
        QVariantAnimation::updateState(newState, oldState);
        if (newState == Running) {
            //check for new from every loop
            if (fromSourced)
                *fromSourced = false;
        } /*else if (newState == Stopped && policy == DeleteWhenStopped) {
            delete animValue;
            animValue = 0;
        }*/ //### we get a stop each loop if we are in a group
        //### top-level animation is the only reliable one for this
    }

private:
    QmlTimeLineValue *animValue;
    bool *fromSourced;
    DeletionPolicy policy;
};

//an animation that just gives a tick
template<class T, void (T::*method)(int)>
class QTickAnimationProxy : public QAbstractAnimation
{
    //Q_OBJECT //doesn't work with templating
public:
    QTickAnimationProxy(T *p, QObject *parent = 0) : QAbstractAnimation(parent), m_p(p) {}
    virtual int duration() const { return -1; }
protected:
    virtual void updateCurrentTime(int msec) { (m_p->*method)(msec); }

private:
    T *m_p;
};

class QmlAbstractAnimationPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlAbstractAnimation)
public:
    QmlAbstractAnimationPrivate()
    : running(false), paused(false), alwaysRunToEnd(false), repeat(false),
      connectedTimeLine(false), componentComplete(true),
      avoidPropertyValueSourceStart(false), disableUserControl(false), group(0) {}

    bool running:1;
    bool paused:1;
    bool alwaysRunToEnd:1;
    bool repeat:1;
    bool connectedTimeLine:1;
    bool componentComplete:1;
    bool avoidPropertyValueSourceStart:1;
    bool disableUserControl:1;

    void commence();

    QmlMetaProperty defaultProperty;

    QmlAnimationGroup *group;

    static QmlMetaProperty createProperty(QObject *obj, const QString &str, QObject *infoObj);
};

class QmlPauseAnimationPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlPauseAnimation)
public:
    QmlPauseAnimationPrivate()
    : QmlAbstractAnimationPrivate(), pa(0) {}

    void init();

    QPauseAnimation *pa;
};

class QmlScriptActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlScriptAction)
public:
    QmlScriptActionPrivate()
        : QmlAbstractAnimationPrivate(), hasRunScriptScript(false), proxy(this), rsa(0) {}

    void init();

    QmlScriptString script;
    QString name;
    QmlScriptString runScriptScript;
    bool hasRunScriptScript;

    void execute();

    QAnimationActionProxy<QmlScriptActionPrivate,
                  &QmlScriptActionPrivate::execute> proxy;
    QActionAnimation *rsa;
};

class QmlPropertyActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlPropertyAction)
public:
    QmlPropertyActionPrivate()
    : QmlAbstractAnimationPrivate(), target(0), spa(0) {}

    void init();

    QObject *target;
    QString propertyName;
    QString properties;
    QList<QObject *> targets;
    QList<QObject *> exclude;

    QmlNullableValue<QVariant> value;

    QActionAnimation *spa;
};

class QmlParentActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlParentAction)
public:
    QmlParentActionPrivate()
    : QmlAbstractAnimationPrivate(), pcTarget(0), pcParent(0) {}

    void init();

    QmlGraphicsItem *pcTarget;
    QmlGraphicsItem *pcParent;

    void doAction();
    QActionAnimation *cpa;
};

class QmlAnimationGroupPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlAnimationGroup)
public:
    QmlAnimationGroupPrivate()
    : QmlAbstractAnimationPrivate(), animations(this), ag(0) {}

    struct AnimationList : public QmlConcreteList<QmlAbstractAnimation *>
    {
        AnimationList(QmlAnimationGroupPrivate *p)
        : anim(p) {}
        virtual void append(QmlAbstractAnimation *a) {
            QmlConcreteList<QmlAbstractAnimation *>::append(a);
            a->setGroup(anim->q_func());
        }
        virtual void clear()
        {
            for (int i = 0; i < count(); ++i)
                at(i)->setGroup(0);
            QmlConcreteList<QmlAbstractAnimation *>::clear();
        }
        virtual void removeAt(int i)
        {
            at(i)->setGroup(0);
            QmlConcreteList<QmlAbstractAnimation *>::removeAt(i);
        }
        virtual void insert(int i, QmlAbstractAnimation *a)
        {
            QmlConcreteList<QmlAbstractAnimation *>::insert(i, a);
            a->setGroup(anim->q_func());
        }

        QmlAnimationGroupPrivate *anim;
    };

    AnimationList animations;
    QAnimationGroup *ag;
};

class QmlPropertyAnimationPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlPropertyAnimation)
public:
    QmlPropertyAnimationPrivate()
    : QmlAbstractAnimationPrivate(), target(0), fromSourced(false), fromIsDefined(false), toIsDefined(false),
      rangeIsSet(false), defaultToInterpolatorType(0), interpolatorType(0), interpolator(0), va(0) {}

    void init();

    QVariant from;
    QVariant to;

    QString easing;

    QObject *target;
    QString propertyName;
    QString properties;
    QList<QObject *> targets;
    QList<QObject *> exclude;
    QString defaultProperties;

    bool fromSourced;
    bool fromIsDefined:1;
    bool toIsDefined:1;
    bool rangeIsSet:1;
    bool defaultToInterpolatorType:1;
    int interpolatorType;
    QVariantAnimation::Interpolator interpolator;

    QmlTimeLineValueAnimator *va;

    static QVariant interpolateVariant(const QVariant &from, const QVariant &to, qreal progress);
    static void convertVariant(QVariant &variant, int type);
};

class QmlRotationAnimationPrivate : public QmlPropertyAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlRotationAnimation)
public:
    QmlRotationAnimationPrivate() : direction(QmlRotationAnimation::Shortest) {}

    QmlRotationAnimation::RotationDirection direction;
};

QT_END_NAMESPACE

#endif // QMLANIMATION_P_H
