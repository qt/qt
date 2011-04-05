// Commit: 0ade09152067324f74678f2de4d447b6e0280600
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

#ifndef QSGANIMATION_P_H
#define QSGANIMATION_P_H

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

#include "qsganimation_p.h"

#include <private/qdeclarativeanimation_p_p.h>

QT_BEGIN_NAMESPACE

class QSGParentAnimationPrivate : public QDeclarativeAnimationGroupPrivate
{
    Q_DECLARE_PUBLIC(QSGParentAnimation)
public:
    QSGParentAnimationPrivate()
    : QDeclarativeAnimationGroupPrivate(), target(0), newParent(0),
       via(0), topLevelGroup(0), startAction(0), endAction(0) {}

    QSGItem *target;
    QSGItem *newParent;
    QSGItem *via;

    QSequentialAnimationGroup *topLevelGroup;
    QActionAnimation *startAction;
    QActionAnimation *endAction;

    QPointF computeTransformOrigin(QSGItem::TransformOrigin origin, qreal width, qreal height) const;
};

class QSGAnchorAnimationPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QSGAnchorAnimation)
public:
    QSGAnchorAnimationPrivate() : rangeIsSet(false), va(0),
        interpolator(QVariantAnimationPrivate::getInterpolator(QMetaType::QReal)) {}

    bool rangeIsSet;
    QDeclarativeBulkValueAnimator *va;
    QVariantAnimation::Interpolator interpolator;
    QList<QSGItem*> targets;
};

QT_END_NAMESPACE

#endif // QSGANIMATION_P_H
