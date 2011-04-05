// Commit: cb0a6844705802564c81b581f24a76c5d5adf6d1
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QSGFLICKABLE_P_P_H
#define QSGFLICKABLE_P_P_H

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

#include "qsgflickable_p.h"
#include "qsgitem_p.h"
#include "qsgitemchangelistener_p.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtCore/qdatetime.h>

#include <private/qdeclarativetimeline_p_p.h>
#include <private/qdeclarativeanimation_p_p.h>

QT_BEGIN_NAMESPACE

// Really slow flicks can be annoying.
const qreal MinimumFlickVelocity = 75.0;

class QSGFlickableVisibleArea;
class QSGFlickablePrivate : public QSGItemPrivate, public QSGItemChangeListener
{
    Q_DECLARE_PUBLIC(QSGFlickable)

public:
    static inline QSGFlickablePrivate *get(QSGFlickable *o) { return o->d_func(); }

    QSGFlickablePrivate();
    void init();

    struct Velocity : public QDeclarativeTimeLineValue
    {
        Velocity(QSGFlickablePrivate *p)
            : parent(p) {}
        virtual void setValue(qreal v) {
            if (v != value()) {
                QDeclarativeTimeLineValue::setValue(v);
                parent->updateVelocity();
            }
        }
        QSGFlickablePrivate *parent;
    };

    struct AxisData {
        AxisData(QSGFlickablePrivate *fp, void (QSGFlickablePrivate::*func)(qreal))
            : move(fp, func), viewSize(-1), smoothVelocity(fp), atEnd(false), atBeginning(true)
            , fixingUp(false)
        {}

        QDeclarativeTimeLineValueProxy<QSGFlickablePrivate> move;
        qreal viewSize;
        qreal pressPos;
        qreal dragStartOffset;
        qreal dragMinBound;
        qreal dragMaxBound;
        qreal velocity;
        qreal flickTarget;
        QSGFlickablePrivate::Velocity smoothVelocity;
        bool atEnd : 1;
        bool atBeginning : 1;
        bool fixingUp : 1;
    };

    void flickX(qreal velocity);
    void flickY(qreal velocity);
    virtual void flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity);

    void fixupX();
    void fixupY();
    virtual void fixup(AxisData &data, qreal minExtent, qreal maxExtent);

    void updateBeginningEnd();

    bool isOutermostPressDelay() const;
    void captureDelayedPress(QGraphicsSceneMouseEvent *event);
    void clearDelayedPress();

    void setRoundedViewportX(qreal x);
    void setRoundedViewportY(qreal y);

    qreal overShootDistance(qreal velocity, qreal size);

    void itemGeometryChanged(QSGItem *, const QRectF &, const QRectF &);

public:
    QSGItem *contentItem;

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
    bool calcVelocity : 1;
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
    QSGItem *delayedPressTarget;
    QBasicTimer delayedPressTimer;
    int pressDelay;
    int fixupDuration;

    enum FixupMode { Normal, Immediate, ExtentChanged };
    FixupMode fixupMode;

    static void fixupY_callback(void *);
    static void fixupX_callback(void *);

    void updateVelocity();
    int vTime;
    QDeclarativeTimeLine velocityTimeline;
    QSGFlickableVisibleArea *visibleArea;
    QSGFlickable::FlickableDirection flickableDirection;
    QSGFlickable::BoundsBehavior boundsBehavior;

    void handleMousePressEvent(QGraphicsSceneMouseEvent *);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    // flickableData property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int data_count(QDeclarativeListProperty<QObject> *);
    static QObject *data_at(QDeclarativeListProperty<QObject> *, int);
    static void data_clear(QDeclarativeListProperty<QObject> *);
};

class QSGFlickableVisibleArea : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal xPosition READ xPosition NOTIFY xPositionChanged)
    Q_PROPERTY(qreal yPosition READ yPosition NOTIFY yPositionChanged)
    Q_PROPERTY(qreal widthRatio READ widthRatio NOTIFY widthRatioChanged)
    Q_PROPERTY(qreal heightRatio READ heightRatio NOTIFY heightRatioChanged)

public:
    QSGFlickableVisibleArea(QSGFlickable *parent=0);

    qreal xPosition() const;
    qreal widthRatio() const;
    qreal yPosition() const;
    qreal heightRatio() const;

    void updateVisible();

signals:
    void xPositionChanged(qreal xPosition);
    void yPositionChanged(qreal yPosition);
    void widthRatioChanged(qreal widthRatio);
    void heightRatioChanged(qreal heightRatio);

private:
    QSGFlickable *flickable;
    qreal m_xPosition;
    qreal m_widthRatio;
    qreal m_yPosition;
    qreal m_heightRatio;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QSGFlickableVisibleArea)

#endif // QSGFLICKABLE_P_P_H
