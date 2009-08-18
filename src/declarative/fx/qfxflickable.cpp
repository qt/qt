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

#include "qfxflickable.h"
#include "qfxflickable_p.h"

#include <QGraphicsSceneMouseEvent>
#include <QPointer>
#include <QTimer>

QT_BEGIN_NAMESPACE

ElasticValue::ElasticValue(QmlTimeLineValue &val)
    : _value(val)
{
    _to = _value.value();
    _myValue = _to;
    _velocity = 0;
}

void ElasticValue::setValue(qreal to)
{
    if (_to != to) {
        _to = to;
        _startTime.start();
        if (state() != Running)
            start();
    }
}

void ElasticValue::clear()
{
    stop();
    _velocity = 0.0;
    _myValue = _value.value();
}

void ElasticValue::updateCurrentTime(int)
{
    const qreal Tension = 0.1;
    int elapsed = _startTime.restart();
    if (!elapsed)
        return;
    qreal dist = _to - _value.value();
    qreal move = Tension * dist * qAbs(dist);
    if (elapsed < 100 && _velocity != 0.0)
        move = (elapsed * move + (100 - elapsed) * _velocity) / 100;
    _myValue += move * elapsed / 1000;
    _value.setValue(qRound(_myValue)); // moving sub-pixel can be ugly.
//    _value.setValue(_myValue);
    _velocity = move;
    if (qAbs(_velocity) < 5.0)
        clear();
    emit updated();
}

QFxFlickablePrivate::QFxFlickablePrivate()
  : _flick(new QFxItem), _moveX(_flick, &QFxItem::setX), _moveY(_flick, &QFxItem::setY)
    , vWidth(-1), vHeight(-1), overShoot(true), flicked(false), moving(false), stealMouse(false)
    , pressed(false), maxVelocity(-1), locked(false), dragMode(QFxFlickable::Hard)
    , elasticY(_moveY), elasticX(_moveX), velocityDecay(100), xVelocity(this), yVelocity(this)
    , vTime(0), atXEnd(false), atXBeginning(true), pageXPosition(0.), pageWidth(0.)
    , atYEnd(false), atYBeginning(true), pageYPosition(0.), pageHeight(0.)
{
    fixupXEvent = QmlTimeLineEvent::timeLineEvent<QFxFlickablePrivate, &QFxFlickablePrivate::fixupX>(&_moveX, this);
    fixupYEvent = QmlTimeLineEvent::timeLineEvent<QFxFlickablePrivate, &QFxFlickablePrivate::fixupY>(&_moveY, this);
}

void QFxFlickablePrivate::init()
{
    Q_Q(QFxFlickable);
    _flick->setParent(q);
    QObject::connect(&_tl, SIGNAL(updated()), q, SLOT(ticked()));
    QObject::connect(&_tl, SIGNAL(completed()), q, SLOT(movementEnding()));
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildEvents(true);
    QObject::connect(_flick, SIGNAL(xChanged()), q, SIGNAL(positionChanged()));
    QObject::connect(_flick, SIGNAL(yChanged()), q, SIGNAL(positionChanged()));
    QObject::connect(&elasticX, SIGNAL(updated()), q, SLOT(ticked()));
    QObject::connect(&elasticY, SIGNAL(updated()), q, SLOT(ticked()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(heightChange()));
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(widthChange()));
}

void QFxFlickablePrivate::fixupX()
{
    Q_Q(QFxFlickable);
    if (!q->xflick() || _moveX.timeLine())
        return;

    vTime = _tl.time();

    if (_moveX.value() > q->minXExtent() || (q->maxXExtent() > q->maxXExtent())) {
        _tl.clear();
        if (_moveX.value() != q->minXExtent())
            _tl.move(_moveX, q->minXExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else if (_moveX.value() < q->maxXExtent()) {
        _tl.clear();
        _tl.move(_moveX,  q->maxXExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else {
        flicked = false;
    }
}

void QFxFlickablePrivate::fixupY()
{
    Q_Q(QFxFlickable);
    if (!q->yflick() || _moveY.timeLine())
        return;

    vTime = _tl.time();

    if (_moveY.value() > q->minYExtent() || (q->maxYExtent() > q->minYExtent())) {
        _tl.clear();
        if (_moveY.value() != q->minYExtent())
            _tl.move(_moveY, q->minYExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else if (_moveY.value() < q->maxYExtent()) {
        _tl.clear();
        _tl.move(_moveY,  q->maxYExtent(), QEasingCurve(QEasingCurve::InOutQuad), 200);
        //emit flickingChanged();
    } else {
        flicked = false;
    }
}

void QFxFlickablePrivate::updateBeginningEnd()
{
    Q_Q(QFxFlickable);
    bool pageChange = false;
    bool atBoundaryChange = false;

    // Vertical
    const int viewheight = q->height();
    const int maxyextent = int(-q->maxYExtent());
    const qreal ypos = -_moveY.value();
    qreal pagePos = ((ypos * 100.0) / (maxyextent + viewheight)) / 100.0;
    qreal pageSize = ((viewheight * 100.0) / (maxyextent + viewheight)) / 100.0;
    bool atBeginning = (ypos <= 0.0);
    bool atEnd = (maxyextent <= ypos);

    if (pageSize != pageHeight) {
        pageHeight = pageSize;
        pageChange = true;
    }
    if (pagePos != pageYPosition) {
        pageYPosition = pagePos;
        pageChange = true;
    }
    if (atBeginning != atYBeginning) {
        atYBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != atYEnd) {
        atYEnd = atEnd;
        atBoundaryChange = true;
    }

    // Horizontal
    const int viewwidth = q->width();
    const int maxxextent = int(-q->maxXExtent());
    const qreal xpos = -_moveX.value();
    pagePos = ((xpos * 100.0) / (maxxextent + viewwidth)) / 100.0;
    pageSize = ((viewwidth * 100.0) / (maxxextent + viewwidth)) / 100.0;
    atBeginning = (xpos <= 0.0);
    atEnd = (maxxextent <= xpos);

    if (pageSize != pageWidth) {
        pageWidth = pageSize;
        pageChange = true;
    }
    if (pagePos != pageXPosition) {
        pageXPosition = pagePos;
        pageChange = true;
    }
    if (atBeginning != atXBeginning) {
        atXBeginning = atBeginning;
        atBoundaryChange = true;
    }
    if (atEnd != atXEnd) {
        atXEnd = atEnd;
        atBoundaryChange = true;
    }

    if (pageChange)
        emit q->pageChanged();
    if (atBoundaryChange)
        emit q->isAtBoundaryChanged();
}

static const int FlickThreshold = 5;

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Flickable,QFxFlickable)

/*!
    \qmlclass Flickable
    \brief The Flickable item provides a surface that can be "flicked".
    \inherits Item

    Flickable places its children on a surface that can be dragged and flicked.

    \code
    Flickable {
        width: 200; height: 200; viewportWidth: image.width; viewportHeight: image.height
        Image { id: image; source: "bigimage.png" }
    }
    \endcode

    \image flickable.gif

    \note Flickable does not automatically clip its contents. If
    it is not full-screen it is likely that \c clip should be set
    to true.

    \note Due to an implementation detail items placed inside a flickable cannot anchor to it by
    id, use 'parent' instead.
*/

/*!
    \internal
    \class QFxFlickable
    \brief The QFxFlickable class provides a view that can be "flicked".

    \ingroup group_widgets

    QFxFlickable allows its children to be dragged and flicked.

\code
Flickable {
    width: 320; height: 480; viewportWidth: image.width; viewportHeight: image.height
    Image { id: image; source: "bigimage.png" }
}
\endcode

    Note that QFxFlickable does not automatically clip its contents. If
    it is not full-screen it is likely that QFxItem::clip should be set
    to true.

*/

QFxFlickable::QFxFlickable(QFxItem *parent)
  : QFxItem(*(new QFxFlickablePrivate), parent)
{
    Q_D(QFxFlickable);
    d->init();
}

QFxFlickable::QFxFlickable(QFxFlickablePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxFlickable);
    d->init();
}

QFxFlickable::~QFxFlickable()
{
}

/*!
    \qmlproperty int Flickable::xPosition
    \qmlproperty int Flickable::yPosition

    These properties hold the surface coordinate currently at the top-left
    corner of the Flickable. For example, if you flick an image up 100 pixels,
    \c yPosition will be 100.
*/
qreal QFxFlickable::xPosition() const
{
    Q_D(const QFxFlickable);
    return -d->_moveX.value();
}

void QFxFlickable::setXPosition(qreal pos)
{
    Q_D(QFxFlickable);
    pos = qRound(pos);
    if (-pos != d->_moveX.value()) {
        d->_tl.reset(d->_moveX);
        d->_moveX.setValue(-pos);
        viewportMoved();
    }
}

qreal QFxFlickable::yPosition() const
{
    Q_D(const QFxFlickable);
    return -d->_moveY.value();
}

void QFxFlickable::setYPosition(qreal pos)
{
    Q_D(QFxFlickable);
    pos = qRound(pos);
    if (-pos != d->_moveY.value()) {
        d->_tl.reset(d->_moveY);
        d->_moveY.setValue(-pos);
        viewportMoved();
    }
}

/*!
    \qmlproperty bool Flickable::locked

    A user cannot drag or flick a Flickable that is locked.

    This property is useful for temporarily disabling flicking. This allows
    special interaction with Flickable's children: for example, you might want to
    freeze a flickable map while viewing detailed information on a location popup that is a child of the Flickable.
*/
bool QFxFlickable::isLocked() const
{
    Q_D(const QFxFlickable);
    return d->locked;
}

void QFxFlickable::setLocked(bool lock)
{
    Q_D(QFxFlickable);
    d->locked = lock;
}

/*!
    \qmlproperty enumeration Flickable::dragMode
    This property contains the kind of 'physics' applied when dragging the surface.

    Two modes are supported:
    \list
    \i Hard - the view follows the user's input exactly.
    \i Elastic - the view moves elastically in response to the user's input.
    \endlist
*/
QFxFlickable::DragMode QFxFlickable::dragMode() const
{
    Q_D(const QFxFlickable);
    return d->dragMode;
}

void QFxFlickable::setDragMode(DragMode mode)
{
    Q_D(QFxFlickable);
    d->dragMode = mode;
}

/*!
    \qmlproperty real Flickable::xVelocity
    \qmlproperty real Flickable::yVelocity

    The instantaneous velocity of movement along the x and y axes, in pixels/sec.
*/
qreal QFxFlickable::xVelocity() const
{
    Q_D(const QFxFlickable);
    return d->xVelocity.value();
}

qreal QFxFlickable::yVelocity() const
{
    Q_D(const QFxFlickable);
    return d->yVelocity.value();
}

/*!
    \qmlproperty bool Flickable::atXBeginning
    \qmlproperty bool Flickable::atXEnd
    \qmlproperty bool Flickable::atYBeginning
    \qmlproperty bool Flickable::atYEnd

    These properties are true if the flickable view is positioned at the beginning,
    or end respecively.
*/
bool QFxFlickable::isAtXEnd() const
{
    Q_D(const QFxFlickable);
    return d->atXEnd;
}

bool QFxFlickable::isAtXBeginning() const
{
    Q_D(const QFxFlickable);
    return d->atXBeginning;
}

bool QFxFlickable::isAtYEnd() const
{
    Q_D(const QFxFlickable);
    return d->atYEnd;
}

bool QFxFlickable::isAtYBeginning() const
{
    Q_D(const QFxFlickable);
    return d->atYBeginning;
}

/*!
    \qmlproperty real Flickable::pageXPosition
    \qmlproperty real Flickable::pageWidth
    \qmlproperty real Flickable::pageYPosition
    \qmlproperty real Flickable::pageHeight

    These properties describe the position and size of the currently viewed page.
    The page size is defined as the percentage of the full view currently visible,
    scaled to 0.0 - 1.0.  The page position is also in the range 0.0 (beginning) to
    1.0 (end).

    These properties are typically used to draw a scrollbar, for example:
    \code
    Rect {
        opacity: 0.5; anchors.right: MyListView.right-2; width: 6
        y: MyListView.pageYPosition * MyListView.height
        height: MyListView.pageHeight * MyListView.height
    }
    \endcode
*/
qreal QFxFlickable::pageWidth() const
{
    Q_D(const QFxFlickable);
    return d->pageWidth;
}

qreal QFxFlickable::pageXPosition() const
{
    Q_D(const QFxFlickable);
    return d->pageXPosition;
}

qreal QFxFlickable::pageHeight() const
{
    Q_D(const QFxFlickable);
    return d->pageHeight;
}

qreal QFxFlickable::pageYPosition() const
{
    Q_D(const QFxFlickable);
    return d->pageYPosition;
}

void QFxFlickable::ticked()
{
    viewportMoved();
}

QFxItem *QFxFlickable::viewport()
{
    Q_D(QFxFlickable);
    return d->_flick;
}

qreal QFxFlickable::visibleX() const
{
    Q_D(const QFxFlickable);
    return -d->_moveX.value();
}

qreal QFxFlickable::visibleY() const
{
    Q_D(const QFxFlickable);
    return -d->_moveY.value();
}

void QFxFlickablePrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!locked && _tl.isActive() && (qAbs(velocityX) > 10 || qAbs(velocityY) > 10))
        stealMouse = true; // If we've been flicked then steal the click.
    else
        stealMouse = false;
    pressed = true;
    _tl.clear();
    velocityX = -1;
    velocityY = -1;
    lastPos = QPoint();
    lastPosTime.start();
    pressPos = event->pos();
    pressX = _moveX.value();
    pressY = _moveY.value();
    flicked = false;
    pressTime.start();
    if (dragMode == QFxFlickable::Elastic) {
        elasticX.clear();
        elasticY.clear();
    }
    velocityTime.start();
}

void QFxFlickablePrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QFxFlickable);
    if (locked || lastPosTime.isNull())
        return;
    bool rejectY = false;
    bool rejectX = false;
    bool moved = false;

    if (q->yflick()) {
        int dy = int(event->pos().y() - pressPos.y());
        if (qAbs(dy) > FlickThreshold || pressTime.elapsed() > 200) {
            qreal newY = dy + pressY;
            const qreal minY = q->minYExtent();
            const qreal maxY = q->maxYExtent();
            if (newY > minY)
                newY = minY + (newY - minY) / 2;
            if (newY < maxY && maxY - minY < 0)
                newY = maxY + (newY - maxY) / 2;
            if (q->overShoot() || (newY <= minY && newY >= maxY)) {
                if (dragMode == QFxFlickable::Hard)
                    _moveY.setValue(newY);
                else
                    elasticY.setValue(newY);
                moved = true;
            } else if (!q->overShoot())
                rejectY = true;
            if (qAbs(dy) > FlickThreshold)
                stealMouse = true;
        }
    }

    if (q->xflick()) {
        int dx = int(event->pos().x() - pressPos.x());
        if (qAbs(dx) > FlickThreshold || pressTime.elapsed() > 200) {
            qreal newX = dx + pressX;
            const qreal minX = q->minXExtent();
            const qreal maxX = q->maxXExtent();
            if (newX > minX)
                newX = minX + (newX - minX) / 2;
            if (newX < maxX && maxX - minX < 0)
                newX = maxX + (newX - maxX) / 2;
            if (q->overShoot() || (newX <= minX && newX >= maxX)) {
                if (dragMode == QFxFlickable::Hard)
                    _moveX.setValue(newX);
                else
                    elasticX.setValue(newX);
                moved = true;
            } else if (!q->overShoot())
                rejectX = true;
            if (qAbs(dx) > FlickThreshold)
                stealMouse = true;
        }
    }

    if (!lastPos.isNull()) {
        qreal elapsed = qreal(lastPosTime.restart()) / 1000.;
        if (elapsed <= 0)
            elapsed = 1;
        if (q->yflick()) {
            qreal diff = event->pos().y() - lastPos.y();
            velocityY = diff / elapsed;
        }

        if (q->xflick()) {
            qreal diff = event->pos().x() - lastPos.x();
            velocityX = diff / elapsed;
        }
    }

    if (rejectY) velocityY = 0;
    if (rejectX) velocityX = 0;

    if (moved) {
        q->viewportMoved();
        q->movementStarting();
    }

    lastPos = event->pos();
}

void QFxFlickablePrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    Q_Q(QFxFlickable);

    pressed = false;
    if (lastPosTime.isNull())
        return;

    if (dragMode == QFxFlickable::Elastic) {
        elasticY.clear();
        elasticX.clear();
    }

    vTime = _tl.time();
    if (qAbs(velocityY) > 10) {
        qreal maxDistance = -1;
        // -ve velocity means list is moving up
        if (velocityY > 0) {
            if (_moveY.value() < q->minYExtent())
                maxDistance = qAbs(q->minYExtent() -_moveY.value() + (overShoot?30:0));
        } else {
            if (_moveY.value() > q->maxYExtent())
                maxDistance = qAbs(q->maxYExtent() - _moveY.value()) + (overShoot?30:0);
        }
        if (maxDistance > 0) {
            qreal v = velocityY;
            if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
                if (v < 0)
                    v = -maxVelocity;
                else
                    v = maxVelocity;
            }
            _tl.accel(_moveY, v, 500, maxDistance);
            _tl.execute(fixupYEvent);
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
        } else {
            fixupY();
        }
    } else {
        fixupY();
    }
    if (qAbs(velocityX) > 10) {
        qreal maxDistance = -1;
        // -ve velocity means list is moving up
        if (velocityX > 0) {
            if (_moveX.value() < q->minXExtent())
                maxDistance = qAbs(q->minXExtent()) -_moveX.value() + (overShoot?30:0);
        } else {
            if (_moveX.value() > q->maxXExtent())
                maxDistance = qAbs(q->maxXExtent() - _moveX.value()) + (overShoot?30:0);
        }
        if (maxDistance > 0) {
            qreal v = velocityX;
            if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
                if (v < 0)
                    v = -maxVelocity;
                else
                    v = maxVelocity;
            }
            _tl.accel(_moveX, v, 500, maxDistance);
            _tl.execute(fixupXEvent);
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
        } else {
            fixupX();
        }
    } else {
        fixupX();
    }
    stealMouse = false;
    lastPosTime = QTime();

    if (!_tl.isActive())
        q->movementEnding();
}

void QFxFlickable::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->handleMousePressEvent(event);
    event->accept();
}

void QFxFlickable::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->handleMouseMoveEvent(event);
    event->accept();
}

void QFxFlickable::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    d->handleMouseReleaseEvent(event);
    event->accept();
    ungrabMouse();
}

qreal QFxFlickable::minYExtent() const
{
    return 0.0;
}

qreal QFxFlickable::minXExtent() const
{
    return 0.0;
}

/* returns -ve */
qreal QFxFlickable::maxXExtent() const
{
    return width() - vWidth();
}
/* returns -ve */
qreal QFxFlickable::maxYExtent() const
{
    return height() - vHeight();
}

void QFxFlickable::viewportMoved()
{
    Q_D(QFxFlickable);
    //XXX should look at moveX here as well
    if (d->flicked && (d->_moveY.value() > minYExtent() + (d->overShoot?30:0)
                || d->_moveY.value() < maxYExtent() - (d->overShoot?30:0))){
        d->flicked = false;
        emit flickingChanged();
        emit flickEnded();
        d->_tl.reset(d->_moveY);
        d->fixupY();
    }

    int elapsed = d->velocityTime.elapsed();

    if (elapsed) {
        qreal prevY = d->lastFlickablePosition.x();
        qreal prevX = d->lastFlickablePosition.y();
        d->velocityTimeline.clear();
        if (d->pressed) {
            qreal xVelocity = (prevX - d->_moveX.value()) * 1000 / elapsed;
            qreal yVelocity = (prevY - d->_moveY.value()) * 1000 / elapsed;
            d->velocityTimeline.move(d->xVelocity, xVelocity, d->velocityDecay);
            d->velocityTimeline.move(d->xVelocity, 0, d->velocityDecay);
            d->velocityTimeline.move(d->yVelocity, yVelocity, d->velocityDecay);
            d->velocityTimeline.move(d->yVelocity, 0, d->velocityDecay);
        } else {
            if (d->_tl.time() != d->vTime) {
                qreal xVelocity = (prevX - d->_moveX.value()) * 1000 / (d->_tl.time() - d->vTime);
                qreal yVelocity = (prevY - d->_moveY.value()) * 1000 / (d->_tl.time() - d->vTime);
                d->xVelocity.setValue(xVelocity);
                d->yVelocity.setValue(yVelocity);
            }
            d->vTime = d->_tl.time();
        }
    }

    d->lastFlickablePosition = QPointF(d->_moveY.value(), d->_moveX.value());
    d->velocityTime.restart();
    d->updateBeginningEnd();
}

void QFxFlickablePrivate::data_removeAt(int)
{
    // ###
}

int QFxFlickablePrivate::data_count() const
{
    // ###
    return 0;
}

void QFxFlickablePrivate::data_append(QObject *o)
{
    Q_Q(QFxFlickable);
    QFxItem *i = qobject_cast<QFxItem *>(o);
    if (i)
        _flick->children()->append(i);
    else
        o->setParent(q);
}

void QFxFlickablePrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QFxFlickablePrivate::data_at(int) const
{
    // ###
    return 0;
}

void QFxFlickablePrivate::data_clear()
{
    // ###
}


QmlList<QObject *> *QFxFlickable::flickableData()
{
    Q_D(QFxFlickable);
    return &d->data;
}

QmlList<QFxItem *> *QFxFlickable::flickableChildren()
{
    Q_D(QFxFlickable);
    return d->_flick->children();
}

/*!
    \qmlproperty bool Flickable::overShoot
    This property holds the number of pixels the surface may overshoot the
    Flickable's boundaries when flicked.

    If overShoot is non-zero the contents can be flicked beyond the boundary
    of the Flickable before being moved back to the boundary.  This provides
    the feeling that the edges of the view are soft, rather than a hard
    physical boundary.
*/
bool QFxFlickable::overShoot() const
{
    Q_D(const QFxFlickable);
    return d->overShoot;
}

void QFxFlickable::setOverShoot(bool o)
{
    Q_D(QFxFlickable);
    d->overShoot = o;
}

/*!
    \qmlproperty int Flickable::viewportWidth
    \qmlproperty int Flickable::viewportHeight

    The dimensions of the viewport (the surface controlled by Flickable). Typically this
    should be set to the combined size of the items placed in the Flickable.

    \code
    Flickable {
        width: 320; height: 480; viewportWidth: image.width; viewportHeight: image.height
        Image { id: image; source: "bigimage.png" }
    }
    \endcode
*/
int QFxFlickable::viewportWidth() const
{
    Q_D(const QFxFlickable);
    return d->vWidth;
}

void QFxFlickable::setViewportWidth(int w)
{
    Q_D(QFxFlickable);
    if (d->vWidth == w)
        return;
    d->vWidth = w;
    if (w < 0)
        d->_flick->setWidth(width());
    else
        d->_flick->setWidth(w);
    // Make sure that we're entirely in view.
    if (d->_moveX.value() > minXExtent() || maxXExtent() > 0) {
        d->_tl.clear();
        d->_moveX.setValue(minXExtent());
    } else if (d->_moveX.value() < maxXExtent()) {
        d->_tl.clear();
        d->_moveX.setValue(maxXExtent());
    }
    emit viewportWidthChanged();
    d->updateBeginningEnd();
}

void QFxFlickable::widthChange()
{
    Q_D(QFxFlickable);
    if (d->vWidth < 0) {
        d->_flick->setWidth(width());
        emit viewportWidthChanged();
        d->updateBeginningEnd();
    }
}

void QFxFlickable::heightChange()
{
    Q_D(QFxFlickable);
    if (d->vHeight < 0) {
        d->_flick->setHeight(height());
        emit viewportHeightChanged();
        d->updateBeginningEnd();
    }
}

int QFxFlickable::viewportHeight() const
{
    Q_D(const QFxFlickable);
    return d->vHeight;
}

void QFxFlickable::setViewportHeight(int h)
{
    Q_D(QFxFlickable);
    if (d->vHeight == h)
        return;
    d->vHeight = h;
    if (h < 0)
        d->_flick->setHeight(height());
    else
        d->_flick->setHeight(h);
    // Make sure that we're entirely in view.
    if (d->_moveY.value() > minYExtent() || maxYExtent() > 0) {
        d->_tl.clear();
        d->_moveY.setValue(minYExtent());
    } else if (d->_moveY.value() < maxYExtent()) {
        d->_tl.clear();
        d->_moveY.setValue(maxYExtent());
    }
    emit viewportHeightChanged();
    d->updateBeginningEnd();
}

int QFxFlickable::vWidth() const
{
    Q_D(const QFxFlickable);
    if (d->vWidth < 0)
        return width();
    else
        return d->vWidth;
}

int QFxFlickable::vHeight() const
{
    Q_D(const QFxFlickable);
    if (d->vHeight < 0)
        return height();
    else
        return d->vHeight;
}

bool QFxFlickable::xflick() const
{
    return vWidth() != width();
}

bool QFxFlickable::yflick() const
{
    return vHeight() !=  height();
}

bool QFxFlickable::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxFlickable);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();

    QGraphicsScene *s = scene();
    QFxItem *grabber = s ? qobject_cast<QFxItem*>(s->mouseGrabberItem()) : 0;
    if ((d->stealMouse || myRect.contains(event->scenePos().toPoint())) && (!grabber || !grabber->keepMouseGrab())) {
        mouseEvent.setAccepted(false);
        for (int i = 0x1; i <= 0x10; i <<= 1) {
            if (event->buttons() & i) {
                Qt::MouseButton button = Qt::MouseButton(i);
                mouseEvent.setButtonDownPos(button, mapFromScene(event->buttonDownPos(button)));
            }
        }
        mouseEvent.setScenePos(event->scenePos());
        mouseEvent.setLastScenePos(event->lastScenePos());
        mouseEvent.setPos(mapFromScene(event->scenePos()));
        mouseEvent.setLastPos(mapFromScene(event->lastScenePos()));

        switch(mouseEvent.type()) {
        case QEvent::GraphicsSceneMouseMove:
            d->handleMouseMoveEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMousePress:
            d->handleMousePressEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMouseRelease:
            d->handleMouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = qobject_cast<QFxItem*>(s->mouseGrabberItem());
        if (grabber && d->stealMouse && !grabber->keepMouseGrab() && grabber != this)
            grabMouse();

        return d->stealMouse;
    } else if (!d->lastPosTime.isNull()) {
        d->lastPosTime = QTime();
    }
    return false;
}

bool QFxFlickable::sceneEventFilter(QGraphicsItem *i, QEvent *e)
{
    if (!isVisible())
        return QFxItem::sceneEventFilter(i, e);
    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QFxItem::sceneEventFilter(i, e);
}

/*!
    \qmlproperty int Flickable::maximumFlickVelocity
    This property holds the maximum velocity that the user can flick the view.
*/
int QFxFlickable::maximumFlickVelocity() const
{
    Q_D(const QFxFlickable);
    return d->maxVelocity;
}

void QFxFlickable::setMaximumFlickVelocity(int v)
{
    Q_D(QFxFlickable);
    if (v == d->maxVelocity)
        return;
    d->maxVelocity = v;
}

bool QFxFlickable::isFlicking() const
{
    Q_D(const QFxFlickable);
    return d->flicked;
}

int QFxFlickable::velocityDecay() const
{
    Q_D(const QFxFlickable);
    return d->velocityDecay;
}

void QFxFlickable::setVelocityDecay(int decay)
{
    Q_D(QFxFlickable);
    Q_ASSERT(decay >= 0);
    if (decay == d->velocityDecay)
        return;
    d->velocityDecay = decay;
    emit velocityDecayChanged(decay);
}

bool QFxFlickable::isMoving() const
{
    Q_D(const QFxFlickable);
    return d->moving;
}

void QFxFlickable::movementStarting()
{
    Q_D(QFxFlickable);
    if (!d->moving) {
        d->moving = true;
        emit movingChanged();
        emit movementStarted();
    }
}

void QFxFlickable::movementEnding()
{
    Q_D(QFxFlickable);
    if (d->moving) {
        d->moving = false;
        emit movingChanged();
        emit movementEnded();
    }
    if (d->flicked) {
        d->flicked = false;
        emit flickingChanged();
        emit flickEnded();
    }
    d->xVelocity.setValue(0);
}

void QFxFlickablePrivate::updateVelocity()
{
    Q_Q(QFxFlickable);
    emit q->velocityChanged(q->xVelocity(), q->yVelocity());
}

QT_END_NAMESPACE
