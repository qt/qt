/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QSTANDARDGESTURES_P_H
#define QSTANDARDGESTURES_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qevent.h"
#include "qbasictimer.h"
#include "qdebug.h"

#include "qgesture.h"
#include "qgesture_p.h"

#include "qstandardgestures.h"
#include "qbasictimer.h"

QT_BEGIN_NAMESPACE

class QPanGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QPanGesture)

public:
    void setupGestureTarget(QObject *o);

    QSizeF totalOffset;
    QSizeF lastOffset;
    QSizeF offset;
    QPointF lastPosition;

#if defined(QT_MAC_USE_COCOA)
    QBasicTimer singleTouchPanTimer;
    QPointF prevMousePos;
#endif
};

class QPinchGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QPinchGesture)

public:
    QPinchGesturePrivate()
        : changes(0), totalScaleFactor(0.), lastScaleFactor(0.), scaleFactor(0.),
          totalRotationAngle(0.), lastRotationAngle(0.), rotationAngle(0.)
#ifdef Q_WS_WIN
          ,initialDistance(0), lastSequenceId(0)
#endif
    {
    }

    void setupGestureTarget(QObject *o);

    QPinchGesture::WhatChanged changes;

    qreal totalScaleFactor; // total scale factor, excluding the current sequence.
    qreal lastScaleFactor;
    qreal scaleFactor; // scale factor in the current sequence.

    qreal totalRotationAngle; // total rotation angle, excluding the current sequence.
    qreal lastRotationAngle;
    qreal rotationAngle; // rotation angle in the current sequence.

    QPointF startCenterPoint;
    QPointF lastCenterPoint;
    QPointF centerPoint;
#ifdef Q_WS_WIN
    int initialDistance;
    ulong lastSequenceId;
#endif
};

class QSwipeGesturePrivate : public QGesturePrivate
{
    Q_DECLARE_PUBLIC(QSwipeGesture)

public:
    QSwipeGesturePrivate()
        : swipeAngle(-1)
    {
    }

    void setupGestureTarget(QObject *o);
    qreal swipeAngle;
};

QT_END_NAMESPACE

#endif // QSTANDARDGESTURES_P_H
