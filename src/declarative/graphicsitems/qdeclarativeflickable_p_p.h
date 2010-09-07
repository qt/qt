/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEFLICKABLE_P_H
#define QDECLARATIVEFLICKABLE_P_H

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

#include "private/qdeclarativeflickable_p.h"

#include "private/qdeclarativeitem_p.h"
#include "private/qdeclarativeitemchangelistener_p.h"

#include <qdeclarative.h>
#include <qdeclarativetimeline_p_p.h>
#include <qdeclarativeanimation_p_p.h>

#include <qdatetime.h>

QT_BEGIN_NAMESPACE

class QDeclarativeFlickableVisibleArea;
class QDeclarativeFlickablePrivate : public QDeclarativeItemPrivate, public QDeclarativeItemChangeListener
{
    Q_DECLARE_PUBLIC(QDeclarativeFlickable)

public:
    QDeclarativeFlickablePrivate();
    void init();

    struct Velocity : public QDeclarativeTimeLineValue
    {
        Velocity(QDeclarativeFlickablePrivate *p)
            : parent(p) {}
        virtual void setValue(qreal v) {
            if (v != value()) {
                QDeclarativeTimeLineValue::setValue(v);
                parent->updateVelocity();
            }
        }
        QDeclarativeFlickablePrivate *parent;
    };

    struct AxisData {
        AxisData(QDeclarativeFlickablePrivate *fp, void (QDeclarativeFlickablePrivate::*func)(qreal))
            : move(fp, func), viewSize(-1), smoothVelocity(fp), atEnd(false), atBeginning(true)
        {}

        QDeclarativeTimeLineValueProxy<QDeclarativeFlickablePrivate> move;
        qreal viewSize;
        qreal pressPos;
        qreal velocity;
        qreal flickTarget;
        QDeclarativeFlickablePrivate::Velocity smoothVelocity;
        bool atEnd : 1;
        bool atBeginning : 1;
    };

    void flickX(qreal velocity);
    void flickY(qreal velocity);
    virtual void flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity);

    void fixupX();
    void fixupY();
    virtual void fixup(AxisData &data, qreal minExtent, qreal maxExtent);

    void updateBeginningEnd();

    void captureDelayedPress(QGraphicsSceneMouseEvent *event);
    void clearDelayedPress();

    void setRoundedViewportX(qreal x);
    void setRoundedViewportY(qreal y);

    qreal overShootDistance(qreal velocity, qreal size);

    void itemGeometryChanged(QDeclarativeItem *, const QRectF &, const QRectF &);

public:
    QDeclarativeItem *contentItem;

    AxisData hData;
    AxisData vData;

    QDeclarativeTimeLine timeline;
    bool flickingHorizontally : 1;
    bool flickingVertically : 1;
    bool hMoved : 1;
    bool vMoved : 1;
    bool movingHorizontally : 1;
    bool movingVertically : 1;
    bool stealMouse : 1;
    bool pressed : 1;
    bool interactive : 1;
    QElapsedTimer lastPosTime;
    QPointF lastPos;
    QPointF pressPos;
    QElapsedTimer pressTime;
    qreal deceleration;
    qreal maxVelocity;
    QElapsedTimer velocityTime;
    QPointF lastFlickablePosition;
    qreal reportedVelocitySmoothing;
    QGraphicsSceneMouseEvent *delayedPressEvent;
    QGraphicsItem *delayedPressTarget;
    QBasicTimer delayedPressTimer;
    int pressDelay;
    int fixupDuration;

    static void fixupY_callback(void *);
    static void fixupX_callback(void *);

    void updateVelocity();
    int vTime;
    QDeclarativeTimeLine velocityTimeline;
    QDeclarativeFlickableVisibleArea *visibleArea;
    QDeclarativeFlickable::FlickableDirection flickableDirection;
    QDeclarativeFlickable::BoundsBehavior boundsBehavior;

    void handleMousePressEvent(QGraphicsSceneMouseEvent *);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    // flickableData property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);
};

class QDeclarativeFlickableVisibleArea : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal xPosition READ xPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal yPosition READ yPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal widthRatio READ widthRatio NOTIFY pageChanged)
    Q_PROPERTY(qreal heightRatio READ heightRatio NOTIFY pageChanged)

public:
    QDeclarativeFlickableVisibleArea(QDeclarativeFlickable *parent=0);

    qreal xPosition() const;
    qreal widthRatio() const;
    qreal yPosition() const;
    qreal heightRatio() const;

    void updateVisible();

signals:
    void pageChanged();

private:
    QDeclarativeFlickable *flickable;
    qreal m_xPosition;
    qreal m_widthRatio;
    qreal m_yPosition;
    qreal m_heightRatio;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeFlickableVisibleArea)

#endif
