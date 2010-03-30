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

#ifndef QDECLARATIVEFLICKABLE_H
#define QDECLARATIVEFLICKABLE_H

#include "qdeclarativeitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeFlickablePrivate;
class QDeclarativeFlickableVisibleArea;
class Q_DECLARATIVE_EXPORT QDeclarativeFlickable : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(qreal contentWidth READ contentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight WRITE setContentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentY READ contentY WRITE setContentY NOTIFY contentYChanged)

    Q_PROPERTY(qreal horizontalVelocity READ horizontalVelocity NOTIFY horizontalVelocityChanged)
    Q_PROPERTY(qreal verticalVelocity READ verticalVelocity NOTIFY verticalVelocityChanged)

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

    Q_PROPERTY(QDeclarativeFlickableVisibleArea *visibleArea READ visibleArea CONSTANT)

    Q_PROPERTY(QDeclarativeListProperty<QObject> flickableData READ flickableData)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsObject> flickableChildren READ flickableChildren)
    Q_CLASSINFO("DefaultProperty", "flickableData")

    Q_ENUMS(FlickDirection)

public:
    QDeclarativeFlickable(QDeclarativeItem *parent=0);
    ~QDeclarativeFlickable();

    QDeclarativeListProperty<QObject> flickableData();
    QDeclarativeListProperty<QGraphicsObject> flickableChildren();

    bool overShoot() const;
    void setOverShoot(bool);

    qreal contentWidth() const;
    void setContentWidth(qreal);

    qreal contentHeight() const;
    void setContentHeight(qreal);

    qreal contentX() const;
    void setContentX(qreal pos);

    qreal contentY() const;
    void setContentY(qreal pos);

    bool isMoving() const;
    bool isFlicking() const;

    int pressDelay() const;
    void setPressDelay(int delay);

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

    QDeclarativeItem *viewport();

    enum FlickDirection { AutoFlickDirection=0x00, HorizontalFlick=0x01, VerticalFlick=0x02, HorizontalAndVerticalFlick=0x03 };
    FlickDirection flickDirection() const;
    void setFlickDirection(FlickDirection);

Q_SIGNALS:
    void contentWidthChanged();
    void contentHeightChanged();
    void contentXChanged();
    void contentYChanged();
    void movingChanged();
    void flickingChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();
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

    QDeclarativeFlickableVisibleArea *visibleArea();

protected Q_SLOTS:
    virtual void ticked();
    void movementStarting();
    void movementEnding();

protected:
    virtual qreal minXExtent() const;
    virtual qreal minYExtent() const;
    virtual qreal maxXExtent() const;
    virtual qreal maxYExtent() const;
    qreal vWidth() const;
    qreal vHeight() const;
    virtual void viewportMoved();
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);

    bool xflick() const;
    bool yflick() const;
    void cancelFlick();

protected:
    QDeclarativeFlickable(QDeclarativeFlickablePrivate &dd, QDeclarativeItem *parent);

private:
    Q_DISABLE_COPY(QDeclarativeFlickable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeFlickable)
    friend class QDeclarativeFlickableVisibleArea;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeFlickable)

QT_END_HEADER

#endif
