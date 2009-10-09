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

#ifndef QGESTURE_H
#define QGESTURE_H

#include <QtCore/qobject.h>
#include <QtCore/qlist.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

Q_DECLARE_METATYPE(Qt::GestureState)

class QGesturePrivate;
class Q_GUI_EXPORT QGesture : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGesture)

    Q_PROPERTY(Qt::GestureState state READ state)
    Q_PROPERTY(Qt::GestureType gestureType READ gestureType)
    Q_PROPERTY(QPointF hotSpot READ hotSpot WRITE setHotSpot RESET unsetHotSpot)
    Q_PROPERTY(bool hasHotSpot READ hasHotSpot)
    Q_PROPERTY(QObject* targetObject READ targetObject WRITE setTargetObject)

public:
    explicit QGesture(Qt::GestureType type = Qt::CustomGesture, QObject *parent = 0);
    explicit QGesture(QObject *parent);
    ~QGesture();

    Qt::GestureType gestureType() const;

    Qt::GestureState state() const;

    QObject *targetObject() const;
    void setTargetObject(QObject *value);

    QPointF hotSpot() const;
    void setHotSpot(const QPointF &value);
    bool hasHotSpot() const;
    void unsetHotSpot();

protected:
    QGesture(QGesturePrivate &dd, Qt::GestureType type, QObject *parent);

private:
    friend class QGestureEvent;
    friend class QGestureRecognizer;
    friend class QGestureManager;
};

class QPanGesturePrivate;
class Q_GUI_EXPORT QPanGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPanGesture)

    Q_PROPERTY(QSizeF totalOffset READ totalOffset WRITE setTotalOffset)
    Q_PROPERTY(QSizeF lastOffset READ lastOffset WRITE setLastOffset)
    Q_PROPERTY(QSizeF offset READ offset WRITE setOffset)
    Q_PROPERTY(qreal acceleration READ acceleration WRITE setAcceleration)

public:
    QPanGesture(QObject *parent = 0);

    QSizeF totalOffset() const;
    QSizeF lastOffset() const;
    QSizeF offset() const;
    qreal acceleration() const;

    void setTotalOffset(const QSizeF &value);
    void setLastOffset(const QSizeF &value);
    void setOffset(const QSizeF &value);
    void setAcceleration(qreal value);

    friend class QPanGestureRecognizer;
    friend class QWinNativePanGestureRecognizer;
};

class QPinchGesturePrivate;
class Q_GUI_EXPORT QPinchGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPinchGesture)

public:
    enum WhatChange {
        ScaleFactorChanged = 0x1,
        RotationAngleChanged = 0x2,
        CenterPointChanged = 0x4
    };
    Q_DECLARE_FLAGS(WhatChanged, WhatChange)

    Q_PROPERTY(WhatChanged whatChanged READ whatChanged WRITE setWhatChanged)

    Q_PROPERTY(qreal totalScaleFactor READ totalScaleFactor WRITE setTotalScaleFactor)
    Q_PROPERTY(qreal lastScaleFactor READ lastScaleFactor WRITE setLastScaleFactor)
    Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor)

    Q_PROPERTY(qreal totalRotationAngle READ totalRotationAngle WRITE setTotalRotationAngle)
    Q_PROPERTY(qreal lastRotationAngle READ lastRotationAngle WRITE setLastRotationAngle)
    Q_PROPERTY(qreal rotationAngle READ rotationAngle WRITE setRotationAngle)

    Q_PROPERTY(QPointF startCenterPoint READ startCenterPoint WRITE setStartCenterPoint)
    Q_PROPERTY(QPointF lastCenterPoint READ lastCenterPoint WRITE setLastCenterPoint)
    Q_PROPERTY(QPointF centerPoint READ centerPoint WRITE setCenterPoint)

public:
    QPinchGesture(QObject *parent = 0);

    WhatChanged whatChanged() const;
    void setWhatChanged(WhatChanged value);

    QPointF startCenterPoint() const;
    QPointF lastCenterPoint() const;
    QPointF centerPoint() const;
    void setStartCenterPoint(const QPointF &value);
    void setLastCenterPoint(const QPointF &value);
    void setCenterPoint(const QPointF &value);

    qreal totalScaleFactor() const;
    qreal lastScaleFactor() const;
    qreal scaleFactor() const;
    void setTotalScaleFactor(qreal value);
    void setLastScaleFactor(qreal value);
    void setScaleFactor(qreal value);

    qreal totalRotationAngle() const;
    qreal lastRotationAngle() const;
    qreal rotationAngle() const;
    void setTotalRotationAngle(qreal value);
    void setLastRotationAngle(qreal value);
    void setRotationAngle(qreal value);

    friend class QPinchGestureRecognizer;
};

Q_DECLARE_METATYPE(QPinchGesture::WhatChanged)

class QSwipeGesturePrivate;
class Q_GUI_EXPORT QSwipeGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSwipeGesture)
    Q_ENUMS(SwipeDirection)

    Q_PROPERTY(SwipeDirection horizontalDirection READ horizontalDirection WRITE setHorizontalDirection)
    Q_PROPERTY(SwipeDirection verticalDirection READ verticalDirection WRITE setVerticalDirection)
    Q_PROPERTY(qreal swipeAngle READ swipeAngle WRITE setSwipeAngle)

public:
    enum SwipeDirection { NoDirection, Left, Right, Up, Down };
    QSwipeGesture(QObject *parent = 0);

    SwipeDirection horizontalDirection() const;
    SwipeDirection verticalDirection() const;
    void setHorizontalDirection(SwipeDirection value);
    void setVerticalDirection(SwipeDirection value);

    qreal swipeAngle() const;
    void setSwipeAngle(qreal value);

    friend class QSwipeGestureRecognizer;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGESTURE_H
