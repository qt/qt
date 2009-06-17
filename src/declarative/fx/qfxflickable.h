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

#ifndef QFXFLICKABLE_H
#define QFXFLICKABLE_H

#include <QtDeclarative/qfxitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QFxFlickablePrivate;
class Q_DECLARATIVE_EXPORT QFxFlickable : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(bool overShoot READ overShoot WRITE setOverShoot)
    Q_PROPERTY(int viewportWidth READ viewportWidth WRITE setViewportWidth NOTIFY viewportWidthChanged)
    Q_PROPERTY(int viewportHeight READ viewportHeight WRITE setViewportHeight NOTIFY viewportHeightChanged)
    Q_PROPERTY(qreal xPosition READ xPosition WRITE setXPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal yPosition READ yPosition WRITE setYPosition NOTIFY positionChanged)
    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
    Q_PROPERTY(bool flicking READ isFlicking NOTIFY flickingChanged)
    Q_PROPERTY(int velocityDecay READ velocityDecay WRITE setVelocityDecay NOTIFY velocityDecayChanged)
    Q_PROPERTY(int maximumFlickVelocity READ maximumFlickVelocity WRITE setMaximumFlickVelocity)
    Q_PROPERTY(bool locked READ isLocked WRITE setLocked)
    Q_PROPERTY(DragMode dragMode READ dragMode WRITE setDragMode)
    Q_PROPERTY(qreal xVelocity READ xVelocity NOTIFY velocityChanged)
    Q_PROPERTY(qreal yVelocity READ yVelocity NOTIFY velocityChanged)
    Q_PROPERTY(bool atXEnd READ isAtXEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYEnd READ isAtYEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atXBeginning READ isAtXBeginning NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYBeginning READ isAtYBeginning NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(qreal pageXPosition READ pageXPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal pageYPosition READ pageYPosition NOTIFY pageChanged)
    Q_PROPERTY(qreal pageWidth READ pageWidth NOTIFY pageChanged)
    Q_PROPERTY(qreal pageHeight READ pageHeight NOTIFY pageChanged)

    Q_PROPERTY(QmlList<QObject *>* flickableData READ flickableData)
    Q_PROPERTY(QmlList<QFxItem *>* flickableChildren READ flickableChildren)
    Q_CLASSINFO("DefaultProperty", "flickableData")

public:
    QFxFlickable(QFxItem *parent=0);
    ~QFxFlickable();

    QmlList<QObject *> *flickableData();
    QmlList<QFxItem *> *flickableChildren();

    bool overShoot() const;
    void setOverShoot(bool);

    int viewportWidth() const;
    void setViewportWidth(int);

    int viewportHeight() const;
    void setViewportHeight(int);

    qreal xPosition() const;
    void setXPosition(qreal pos);

    qreal yPosition() const;
    void setYPosition(qreal pos);

    bool isMoving() const;
    bool isFlicking() const;

    int velocityDecay() const;
    void setVelocityDecay(int);

    int maximumFlickVelocity() const;
    void setMaximumFlickVelocity(int);

    bool isLocked() const;
    void setLocked(bool);

    Q_ENUMS(DragMode);
    enum DragMode { Hard, Elastic };
    DragMode dragMode() const;
    void setDragMode(DragMode mode);

    qreal xVelocity() const;
    qreal yVelocity() const;

    bool isAtXEnd() const;
    bool isAtXBeginning() const;
    qreal pageXPosition() const;
    qreal pageWidth() const;

    bool isAtYEnd() const;
    bool isAtYBeginning() const;
    qreal pageYPosition() const;
    qreal pageHeight() const;

    QFxItem *viewport();

Q_SIGNALS:
    void viewportWidthChanged();
    void viewportHeightChanged();
    void positionChanged();
    void movingChanged();
    void flickingChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();
    void velocityDecayChanged(int);
    void velocityChanged(qreal, qreal);
    void isAtBoundaryChanged();
    void pageChanged();

protected:
    virtual bool mouseFilter(QGraphicsSceneMouseEvent *);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    qreal visibleX() const;
    qreal visibleY() const;

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
    int vWidth() const;
    int vHeight() const;
    virtual void viewportMoved();
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);

    bool xflick() const;
    bool yflick() const;

protected:
    QFxFlickable(QFxFlickablePrivate &dd, QFxItem *parent);

private:
    Q_DISABLE_COPY(QFxFlickable)
    Q_DECLARE_PRIVATE(QFxFlickable)
};
QML_DECLARE_TYPE(QFxFlickable)


QT_END_NAMESPACE

QT_END_HEADER
#endif
