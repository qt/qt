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

#include <private/qobject_p.h>
#include <private/qmlnullablevalue_p.h>
#include <private/qvariantanimation_p.h>
#include <QtCore/QPauseAnimation>
#include <QtCore/QVariantAnimation>
#include <QtCore/QAnimationGroup>
#include <QtGui/QColor>
#include <QtDeclarative/qmlanimation.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmltimelinevalueproxy.h>

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
public:
    QActionAnimation(QObject *parent = 0) : QAbstractAnimation(parent), animAction(0), policy(KeepWhenStopped) {}
    QActionAnimation(QAbstractAnimationAction *action, QObject *parent = 0)
        : QAbstractAnimation(parent), animAction(action), policy(KeepWhenStopped) {}
    virtual int duration() const { return 0; }
    void setAnimAction(QAbstractAnimationAction *action, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
        animAction = action;
        policy = p;
    }
protected:
    virtual void updateCurrentTime(int) {}

    virtual void updateState(State /*oldState*/, State newState)
    {
        if (newState == Running) {
            if (animAction)
                animAction->doAction();
        } else if (newState == Stopped && policy == DeleteWhenStopped) {
            delete animAction;
            animAction = 0;
        }
    }

private:
    QAbstractAnimationAction *animAction;
    DeletionPolicy policy;
};

//animates QmlTimeLineValue (assumes start and end values will be reals or compatible)
class QmlTimeLineValueAnimator : public QVariantAnimation
{
public:
    QmlTimeLineValueAnimator(QObject *parent = 0) : QVariantAnimation(parent), animValue(0), fromSourced(0), policy(KeepWhenStopped) {}
    void setAnimValue(QmlTimeLineValue *value, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
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
            animValue->setValue(value.toDouble());
    }
    virtual void updateState(State oldState, State newState)
    {
        QVariantAnimation::updateState(oldState, newState);
        if (newState == Running) {
            //check for new from every loop
            if (fromSourced)
                *fromSourced = false;
        } else if (newState == Stopped && policy == DeleteWhenStopped) {
            delete animValue;
            animValue = 0;
        }
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
    : running(false), paused(false), finishPlaying(false), repeat(false),
      connectedTimeLine(false), componentComplete(true), startOnCompletion(false),
      target(0), group(0) {}

    bool running;
    bool paused;
    bool finishPlaying;
    bool repeat;
    bool connectedTimeLine;

    bool componentComplete;
    bool startOnCompletion;

    void commence();

    QmlNullableValue<QmlMetaProperty> userProperty;
    QObject *target;
    QString propertyName;

    QmlMetaProperty property;
    QmlAnimationGroup *group;

    QmlMetaProperty createProperty(QObject *obj, const QString &str);
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

class QmlRunScriptActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlRunScriptAction)
public:
    QmlRunScriptActionPrivate()
        : QmlAbstractAnimationPrivate(), proxy(this), rsa(0) {}

    void init();

    QString script;
    QString file;

    void execute();

    QAnimationActionProxy<QmlRunScriptActionPrivate,
                  &QmlRunScriptActionPrivate::execute> proxy;
    QActionAnimation *rsa;
};

class QmlSetPropertyActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlSetPropertyAction)
public:
    QmlSetPropertyActionPrivate()
    : QmlAbstractAnimationPrivate(), proxy(this), spa(0) {}

    void init();

    QString properties;
    QList<QObject *> filter;
    QList<QObject *> exclude;

    QmlNullableValue<QVariant> value;

    void doAction();

    QAnimationActionProxy<QmlSetPropertyActionPrivate,
                  &QmlSetPropertyActionPrivate::doAction> proxy;
    QActionAnimation *spa;
};

class QmlParentChangeActionPrivate : public QmlAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QmlParentChangeAction)
public:
    QmlParentChangeActionPrivate()
    : QmlAbstractAnimationPrivate() {}

    void init();

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
    : QmlAbstractAnimationPrivate(), fromSourced(false), fromIsDefined(false), toIsDefined(false),
      defaultToInterpolatorType(0), interpolatorType(0), interpolator(0), va(0),
      value(this, &QmlPropertyAnimationPrivate::valueChanged) {}

    void init();

    QVariant from;
    QVariant to;

    QString easing;

    QString properties;
    QList<QObject *> filter;
    QList<QObject *> exclude;

    bool fromSourced;
    bool fromIsDefined;
    bool toIsDefined;
    bool defaultToInterpolatorType;
    int interpolatorType;
    QVariantAnimation::Interpolator interpolator;

    QmlTimeLineValueAnimator *va;
    virtual void valueChanged(qreal);

    QmlTimeLineValueProxy<QmlPropertyAnimationPrivate> value;

    static QVariant interpolateVariant(const QVariant &from, const QVariant &to, qreal progress);
    static void convertVariant(QVariant &variant, QVariant::Type type);
};

QT_END_NAMESPACE

#endif // QMLANIMATION_P_H
