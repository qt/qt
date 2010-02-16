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

#ifndef QMLGRAPHICSFLICKABLE_H
#define QMLGRAPHICSFLICKABLE_H

#include "qmlgraphicsitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlGraphicsFlickablePrivate;
class QmlGraphicsFlickableVisibleArea;
class Q_DECLARATIVE_EXPORT QmlGraphicsFlickable : public QmlGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(qreal viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)
    Q_PROPERTY(qreal viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY viewportHeightChanged)
    Q_PROPERTY(qreal viewportX READ viewportX WRITE setViewportX NOTIFY positionXChanged)
    Q_PROPERTY(qreal viewportY READ viewportY WRITE setViewportY NOTIFY positionYChanged)

    Q_PROPERTY(qreal horizontalVelocity READ horizontalVelocity NOTIFY horizontalVelocityChanged)
    Q_PROPERTY(qreal verticalVelocity READ verticalVelocity NOTIFY verticalVelocityChanged)
    Q_PROPERTY(qreal reportedVelocitySmoothing READ reportedVelocitySmoothing WRITE setReportedVelocitySmoothing NOTIFY reportedVelocitySmoothingChanged)

    Q_PROPERTY(bool overShoot READ overShoot WRITE setOverShoot NOTIFY overShootChanged)
    Q_PROPERTY(qreal maximumFlickVelocity READ maximumFlickVelocity WRITE setMaximumFlickVelocity NOTIFY maximumFlickVelocityChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)
    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
    Q_PROPERTY(bool flicking READ isFlicking NOTIFY flickingChanged)
    Q_PROPERTY(FlickDirection flickDirection READ flickDirection WRITE setFlickDirection NOTIFY flickDirectionChanged)

    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(int pressDelay READ pressDelay WRITE setPressDelay NOTIFY pressDelayChanged)

    Q_PROPERTY(bool atXEnd READ isAtXEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYEnd READ isAtYEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atXBeginning READ isAtXBeginning NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYBeginning READ isAtYBeginning NOTIFY isAtBoundaryChanged)

    Q_PROPERTY(QmlGraphicsFlickableVisibleArea *visibleArea READ visibleArea CONSTANT)

    Q_PROPERTY(QmlList<QObject *>* flickableData READ flickableData)
    Q_PROPERTY(QmlList<QmlGraphicsItem *>* flickableChildren READ flickableChildren)
    Q_CLASSINFO("DefaultProperty", "flickableData")

    Q_ENUMS(FlickDirection)

public:
    QmlGraphicsFlickable(QmlGraphicsItem *parent=0);
    ~QmlGraphicsFlickable();

    QmlList<QObject *> *flickableData();
    QmlList<QmlGraphicsItem *> *flickableChildren();

    bool overShoot() const;
    void setOverShoot(bool);

    qreal viewportWidth() const;
    void setViewportWidth(qreal);

    qreal viewportHeight() const;
    void setViewportHeight(qreal);

    qreal viewportX() const;
    void setViewportX(qreal pos);

    qreal viewportY() const;
    void setViewportY(qreal pos);

    bool isMoving() const;
    bool isFlicking() const;

    int pressDelay() const;
    void setPressDelay(int delay);

    qreal reportedVelocitySmoothing() const;
    void setReportedVelocitySmoothing(qreal);

    qreal maximumFlickVelocity() const;
    void setMaximumFlickVelocity(qreal);

    qreal flickDeceleration() const;
    void setFlickDeceleration(qreal);

    bool isInteractive() const;
    void setInteractive(bool);

    qreal horizontalVelocity() const;
    qreal verticalVelocity() const;

    bool isAtXEnd() const;
    bool isAtXBeginning() const;
    bool isAtYEnd() const;
    bool isAtYBeginning() const;

    QmlGraphicsItem *viewport();

    enum FlickDirection { AutoFlickDirection=0x00, HorizontalFlick=0x01, VerticalFlick=0x02, HorizontalAndVerticalFlick=0x03 };
    FlickDirection flickDirection() const;
    void setFlickDirection(FlickDirection);

Q_SIGNALS:
    void viewportWidthChanged();
    void viewportHeightChanged();
    void positionXChanged();
    void positionYChanged();
    void movingChanged();
    void flickingChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();
    void reportedVelocitySmoothingChanged(int);
    void horizontalVelocityChanged();
    void verticalVelocityChanged();
    void isAtBoundaryChanged();
    void pageChanged();
    void flickDirectionChanged();
    void interactiveChanged();
    void overShootChanged();
    void maximumFlickVelocityChanged();
    void flickDecelerationChanged();
    void pressDelayChanged();

protected:
    virtual bool sceneEventFilter(QGraphicsItem *, QEvent *);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void timerEvent(QTimerEvent *event);

    QmlGraphicsFlickableVisibleArea *visibleArea();

protected Q_SLOTS:
    virtual void ticked();
    void movementStarting();
    void movementEnding();
    void heightChange();
    void widthChange();

protected:
    virtual qreal minXExtent() const;
    virtual qreal minYExtent() const;
    virtual qreal maxXExtent() const;
    virtual qreal maxYExtent() const;
    qreal vWidth() const;
    qreal vHeight() const;
    virtual void viewportMoved();
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);

    bool xflick() const;
    bool yflick() const;
    void cancelFlick();

protected:
    QmlGraphicsFlickable(QmlGraphicsFlickablePrivate &dd, QmlGraphicsItem *parent);

private:
    Q_DISABLE_COPY(QmlGraphicsFlickable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsFlickable)
    friend class QmlGraphicsFlickableVisibleArea;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsFlickable)

QT_END_HEADER

#endif
