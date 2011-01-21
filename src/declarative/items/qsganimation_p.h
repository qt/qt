// Commit: e39a2e39451bf106a9845f8a60fc571faaa4dde5
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

#ifndef QSGANIMATION_H
#define QSGANIMATION_H

#include "qsgitem.h"

#include <private/qdeclarativeanimation_p.h>

#include <QtCore/qabstractanimation.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGParentAnimationPrivate;
class QSGParentAnimation : public QDeclarativeAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGParentAnimation)

    Q_PROPERTY(QSGItem *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QSGItem *newParent READ newParent WRITE setNewParent NOTIFY newParentChanged)
    Q_PROPERTY(QSGItem *via READ via WRITE setVia NOTIFY viaChanged)

public:
    QSGParentAnimation(QObject *parent=0);
    virtual ~QSGParentAnimation();

    QSGItem *target() const;
    void setTarget(QSGItem *);

    QSGItem *newParent() const;
    void setNewParent(QSGItem *);

    QSGItem *via() const;
    void setVia(QSGItem *);

Q_SIGNALS:
    void targetChanged();
    void newParentChanged();
    void viaChanged();

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QSGAnchorAnimationPrivate;
class QSGAnchorAnimation : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSGAnchorAnimation)
    Q_PROPERTY(QDeclarativeListProperty<QSGItem> targets READ targets)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)

public:
    QSGAnchorAnimation(QObject *parent=0);
    virtual ~QSGAnchorAnimation();

    QDeclarativeListProperty<QSGItem> targets();

    int duration() const;
    void setDuration(int);

    QEasingCurve easing() const;
    void setEasing(const QEasingCurve &);

Q_SIGNALS:
    void durationChanged(int);
    void easingChanged(const QEasingCurve&);

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGParentAnimation)
QML_DECLARE_TYPE(QSGAnchorAnimation)

QT_END_HEADER

#endif // QSGANIMATION_H
