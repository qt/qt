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

#ifndef QMLANIMATION_H
#define QMLANIMATION_H

#include "qmltransition_p.h"
#include "qmlstate_p.h"
#include <QtGui/qvector3d.h>

#include <qmlpropertyvaluesource.h>
#include <qml.h>
#include <qmlscriptstring.h>

#include <QtCore/qvariant.h>
#include <QtCore/QAbstractAnimation>
#include <QtGui/qcolor.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlAbstractAnimationPrivate;
class QmlAnimationGroup;
class Q_AUTOTEST_EXPORT QmlAbstractAnimation : public QObject, public QmlPropertyValueSource, public QmlParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlAbstractAnimation)

    Q_INTERFACES(QmlParserStatus)
    Q_INTERFACES(QmlPropertyValueSource)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool alwaysRunToEnd READ alwaysRunToEnd WRITE setAlwaysRunToEnd NOTIFY alwaysRunToEndChanged())
    Q_PROPERTY(bool repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
    Q_CLASSINFO("DefaultMethod", "start()")

public:
    QmlAbstractAnimation(QObject *parent=0);
    virtual ~QmlAbstractAnimation();

    bool isRunning() const;
    void setRunning(bool);
    bool isPaused() const;
    void setPaused(bool);
    bool alwaysRunToEnd() const;
    void setAlwaysRunToEnd(bool);
    bool repeat() const;
    void setRepeat(bool);

    int currentTime();
    void setCurrentTime(int);

    QmlAnimationGroup *group() const;
    void setGroup(QmlAnimationGroup *);

    virtual void setTarget(const QmlMetaProperty &);

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void started();
    void completed();
    void runningChanged(bool);
    void pausedChanged(bool);
    void repeatChanged(bool);
    void alwaysRunToEndChanged(bool);

public Q_SLOTS:
    void restart();
    void start();
    void pause();
    void resume();
    void stop();
    void complete();

protected:
    QmlAbstractAnimation(QmlAbstractAnimationPrivate &dd, QObject *parent);

public:
    enum TransitionDirection { Forward, Backward };
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual void prepare(QmlMetaProperty &);
    virtual QAbstractAnimation *qtAnimation() = 0;

private Q_SLOTS:
    void timelineComplete();
};

class QmlPauseAnimationPrivate;
class Q_AUTOTEST_EXPORT QmlPauseAnimation : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPauseAnimation)

    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)

public:
    QmlPauseAnimation(QObject *parent=0);
    virtual ~QmlPauseAnimation();

    int duration() const;
    void setDuration(int);

Q_SIGNALS:
    void durationChanged(int);

protected:
    virtual QAbstractAnimation *qtAnimation();
};

class QmlScriptActionPrivate;
class QmlScriptAction : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlScriptAction)

    Q_PROPERTY(QmlScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString stateChangeScriptName READ stateChangeScriptName WRITE setStateChangeScriptName)

public:
    QmlScriptAction(QObject *parent=0);
    virtual ~QmlScriptAction();

    QmlScriptString script() const;
    void setScript(const QmlScriptString &);

    QString stateChangeScriptName() const;
    void setStateChangeScriptName(const QString &);

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QmlPropertyActionPrivate;
class QmlPropertyAction : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPropertyAction)

    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY targetChanged)
    Q_PROPERTY(QString matchProperties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QList<QObject *>* matchTargets READ targets)
    Q_PROPERTY(QList<QObject *>* exclude READ exclude)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    QmlPropertyAction(QObject *parent=0);
    virtual ~QmlPropertyAction();

    QObject *target() const;
    void setTarget(QObject *);

    QString property() const;
    void setProperty(const QString &);

    QString properties() const;
    void setProperties(const QString &);

    QList<QObject *> *targets();
    QList<QObject *> *exclude();

    QVariant value() const;
    void setValue(const QVariant &);

Q_SIGNALS:
    void valueChanged(const QVariant &);
    void propertiesChanged(const QString &);
    void targetChanged(QObject *, const QString &);

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
    virtual void prepare(QmlMetaProperty &);
};

class QmlGraphicsItem;
class QmlParentActionPrivate;
class QmlParentAction : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlParentAction)

    Q_PROPERTY(QmlGraphicsItem *target READ object WRITE setObject)
    Q_PROPERTY(QmlGraphicsItem *matchTarget READ matchTarget WRITE setMatchTarget)
    Q_PROPERTY(QmlGraphicsItem *parent READ parent WRITE setParent)

public:
    QmlParentAction(QObject *parent=0);
    virtual ~QmlParentAction();

    QmlGraphicsItem *object() const;
    void setObject(QmlGraphicsItem *);

    QmlGraphicsItem *matchTarget() const;
    void setMatchTarget(QmlGraphicsItem *);

    QmlGraphicsItem *parent() const;
    void setParent(QmlGraphicsItem *);

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QmlPropertyAnimationPrivate;
class Q_AUTOTEST_EXPORT QmlPropertyAnimation : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPropertyAnimation)

    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QVariant from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QVariant to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QString easing READ easing WRITE setEasing NOTIFY easingChanged)
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY targetChanged)
    Q_PROPERTY(QString matchProperties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QList<QObject *>* matchTargets READ targets)
    Q_PROPERTY(QList<QObject *>* exclude READ exclude)

public:
    QmlPropertyAnimation(QObject *parent=0);
    virtual ~QmlPropertyAnimation();

    int duration() const;
    void setDuration(int);

    QVariant from() const;
    void setFrom(const QVariant &);

    QVariant to() const;
    void setTo(const QVariant &);

    QString easing() const;
    void setEasing(const QString &);

    QObject *target() const;
    void setTarget(QObject *);

    QString property() const;
    void setProperty(const QString &);

    QString properties() const;
    void setProperties(const QString &);

    QList<QObject *> *targets();
    QList<QObject *> *exclude();

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
    virtual void prepare(QmlMetaProperty &);

Q_SIGNALS:
    void durationChanged(int);
    void fromChanged(QVariant);
    void toChanged(QVariant);
    void easingChanged(const QString &);
    void propertiesChanged(const QString &);
    void targetChanged(QObject *, const QString &);
};

class Q_AUTOTEST_EXPORT QmlColorAnimation : public QmlPropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPropertyAnimation)
    Q_PROPERTY(QColor from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QColor to READ to WRITE setTo NOTIFY toChanged)

public:
    QmlColorAnimation(QObject *parent=0);
    virtual ~QmlColorAnimation();

    QColor from() const;
    void setFrom(const QColor &);

    QColor to() const;
    void setTo(const QColor &);
};

class Q_AUTOTEST_EXPORT QmlNumberAnimation : public QmlPropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPropertyAnimation)

    Q_PROPERTY(qreal from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(qreal to READ to WRITE setTo NOTIFY toChanged)

public:
    QmlNumberAnimation(QObject *parent=0);
    virtual ~QmlNumberAnimation();

    qreal from() const;
    void setFrom(qreal);

    qreal to() const;
    void setTo(qreal);
};

class Q_AUTOTEST_EXPORT QmlVector3dAnimation : public QmlPropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlPropertyAnimation)

    Q_PROPERTY(QVector3D from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QVector3D to READ to WRITE setTo NOTIFY toChanged)

public:
    QmlVector3dAnimation(QObject *parent=0);
    virtual ~QmlVector3dAnimation();

    QVector3D from() const;
    void setFrom(QVector3D);

    QVector3D to() const;
    void setTo(QVector3D);
};

class QmlAnimationGroupPrivate;
class QmlAnimationGroup : public QmlAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlAnimationGroup)

    Q_CLASSINFO("DefaultProperty", "animations")
    Q_PROPERTY(QmlList<QmlAbstractAnimation *> *animations READ animations)

public:
    QmlAnimationGroup(QObject *parent);
    virtual ~QmlAnimationGroup();

    QmlList<QmlAbstractAnimation *>* animations();
    friend class QmlAbstractAnimation;
};

class QmlSequentialAnimation : public QmlAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlAnimationGroup)

public:
    QmlSequentialAnimation(QObject *parent=0);
    virtual ~QmlSequentialAnimation();

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
    virtual void prepare(QmlMetaProperty &);
};

class QmlParallelAnimation : public QmlAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlAnimationGroup)

public:
    QmlParallelAnimation(QObject *parent=0);
    virtual ~QmlParallelAnimation();

protected:
    virtual void transition(QmlStateActions &actions,
                            QmlMetaProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
    virtual void prepare(QmlMetaProperty &);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlAbstractAnimation)
QML_DECLARE_TYPE(QmlPauseAnimation)
QML_DECLARE_TYPE(QmlScriptAction)
QML_DECLARE_TYPE(QmlPropertyAction)
QML_DECLARE_TYPE(QmlParentAction)
QML_DECLARE_TYPE(QmlPropertyAnimation)
QML_DECLARE_TYPE(QmlColorAnimation)
QML_DECLARE_TYPE(QmlNumberAnimation)
QML_DECLARE_TYPE(QmlSequentialAnimation)
QML_DECLARE_TYPE(QmlParallelAnimation)
QML_DECLARE_TYPE(QmlVector3dAnimation)

QT_END_HEADER

#endif // QMLANIMATION_H
