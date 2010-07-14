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

#include "private/qdeclarativemousearea_p.h"
#include "private/qdeclarativemousearea_p_p.h"

#include "private/qdeclarativeevents_p_p.h"

#include <QGraphicsSceneMouseEvent>

QT_BEGIN_NAMESPACE
static const int PressAndHoldDelay = 800;

QDeclarativeDrag::QDeclarativeDrag(QObject *parent)
: QObject(parent), _target(0), _axis(XandYAxis), _xmin(0), _xmax(0), _ymin(0), _ymax(0),
_active(false)
{
}

QDeclarativeDrag::~QDeclarativeDrag()
{
}

QGraphicsObject *QDeclarativeDrag::target() const
{
    return _target;
}

void QDeclarativeDrag::setTarget(QGraphicsObject *t)
{
    if (_target == t)
        return;
    _target = t;
    emit targetChanged();
}

void QDeclarativeDrag::resetTarget()
{
    if (!_target)
        return;
    _target = 0;
    emit targetChanged();
}

QDeclarativeDrag::Axis QDeclarativeDrag::axis() const
{
    return _axis;
}

void QDeclarativeDrag::setAxis(QDeclarativeDrag::Axis a)
{
    if (_axis == a)
        return;
    _axis = a;
    emit axisChanged();
}

qreal QDeclarativeDrag::xmin() const
{
    return _xmin;
}

void QDeclarativeDrag::setXmin(qreal m)
{
    if (_xmin == m)
        return;
    _xmin = m;
    emit minimumXChanged();
}

qreal QDeclarativeDrag::xmax() const
{
    return _xmax;
}

void QDeclarativeDrag::setXmax(qreal m)
{
    if (_xmax == m)
        return;
    _xmax = m;
    emit maximumXChanged();
}

qreal QDeclarativeDrag::ymin() const
{
    return _ymin;
}

void QDeclarativeDrag::setYmin(qreal m)
{
    if (_ymin == m)
        return;
    _ymin = m;
    emit minimumYChanged();
}

qreal QDeclarativeDrag::ymax() const
{
    return _ymax;
}

void QDeclarativeDrag::setYmax(qreal m)
{
    if (_ymax == m)
        return;
    _ymax = m;
    emit maximumYChanged();
}

bool QDeclarativeDrag::active() const
{
    return _active;
}

void QDeclarativeDrag::setActive(bool drag)
{
    if (_active == drag)
        return;
    _active = drag;
    emit activeChanged();
}

QDeclarativeMouseAreaPrivate::~QDeclarativeMouseAreaPrivate()
{
    delete drag;
}


/*!
    \qmlclass MouseArea QDeclarativeMouseArea
    \since 4.7
    \brief The MouseArea item enables simple mouse handling.
    \inherits Item

    A MouseArea is typically used in conjunction with a visible item,
    where the MouseArea effectively 'proxies' mouse handling for that
    item. For example, we can put a MouseArea in a \l Rectangle that changes
    the \l Rectangle color to red when clicked:

    \snippet doc/src/snippets/declarative/mousearea.qml import
    \codeline
    \snippet doc/src/snippets/declarative/mousearea.qml intro

    Many MouseArea signals pass a \l {MouseEvent}{mouse} parameter that contains
    additional information about the mouse event, such as the position, button,
    and any key modifiers.

    Here is an extension of the previous example that produces a different
    color when the area is right clicked:

    \snippet doc/src/snippets/declarative/mousearea.qml intro-extended

    For basic key handling, see the \l {Keys}{Keys attached property}.

    MouseArea is an invisible item: it is never painted.

    \sa MouseEvent, {declarative/touchinteraction/mousearea}{MouseArea example}
*/

/*!
    \qmlsignal MouseArea::onEntered()

    This handler is called when the mouse enters the mouse area.

    By default the onEntered handler is only called while a button is
    pressed. Setting hoverEnabled to true enables handling of
    onEntered when no mouse button is pressed.

    \sa hoverEnabled
*/

/*!
    \qmlsignal MouseArea::onExited()

    This handler is called when the mouse exists the mouse area.

    By default the onExited handler is only called while a button is
    pressed. Setting hoverEnabled to true enables handling of
    onExited when no mouse button is pressed.

    \sa hoverEnabled
*/

/*!
    \qmlsignal MouseArea::onPositionChanged(MouseEvent mouse)

    This handler is called when the mouse position changes.

    The \l {MouseEvent}{mouse} parameter provides information about the mouse, including the x and y
    position, and any buttons currently pressed.

    The \e accepted property of the MouseEvent parameter is ignored in this handler.

    By default the onPositionChanged handler is only called while a button is
    pressed.  Setting hoverEnabled to true enables handling of
    onPositionChanged when no mouse button is pressed.
*/

/*!
    \qmlsignal MouseArea::onClicked(mouse)

    This handler is called when there is a click. A click is defined as a press followed by a release,
    both inside the MouseArea (pressing, moving outside the MouseArea, and then moving back inside and
    releasing is also considered a click).

    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click was held.

    The \e accepted property of the MouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal MouseArea::onPressed(mouse)

    This handler is called when there is a press.
    The \l {MouseEvent}{mouse} parameter provides information about the press, including the x and y
    position and which button was pressed.

    The \e accepted property of the MouseEvent parameter determines whether this MouseArea
    will handle the press and all future mouse events until release.  The default is to accept
    the event and not allow other MouseArea beneath this one to handle the event.  If \e accepted
    is set to false, no further events will be sent to this MouseArea until the button is next
    pressed.
*/

/*!
    \qmlsignal MouseArea::onReleased(mouse)

    This handler is called when there is a release.
    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click was held.

    The \e accepted property of the MouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal MouseArea::onPressAndHold(mouse)

    This handler is called when there is a long press (currently 800ms).
    The \l {MouseEvent}{mouse} parameter provides information about the press, including the x and y
    position of the press, and which button is pressed.

    The \e accepted property of the MouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal MouseArea::onDoubleClicked(mouse)

    This handler is called when there is a double-click (a press followed by a release followed by a press).
    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click was held.

    The \e accepted property of the MouseEvent parameter is ignored in this handler.
*/

/*!
    \qmlsignal MouseArea::onCanceled()

    This handler is called when mouse events have been canceled, either because an event was not accepted, or
    because another element stole the mouse event handling. This signal is for advanced use: it is useful when
    there is more than one MouseArea that is handling input, or when there is a MouseArea inside a \l Flickable. In the latter
    case, if you execute some logic on the pressed signal and then start dragging, the \l Flickable will steal the mouse handling
    from the MouseArea. In these cases, to reset the logic when the MouseArea has lost the mouse handling to the
    \l Flickable, \c onCanceled should be used in addition to onReleased.
*/

/*!
    \internal
    \class QDeclarativeMouseArea
    \brief The QDeclarativeMouseArea class provides a simple mouse handling abstraction for use within QML.

    All QDeclarativeItem derived classes can do mouse handling but the QDeclarativeMouseArea class exposes mouse
    handling data as properties and tracks flicking and dragging of the mouse.

    A QDeclarativeMouseArea object can be instantiated in QML using the tag \l MouseArea.
 */
QDeclarativeMouseArea::QDeclarativeMouseArea(QDeclarativeItem *parent)
  : QDeclarativeItem(*(new QDeclarativeMouseAreaPrivate), parent)
{
    Q_D(QDeclarativeMouseArea);
    d->init();
}

QDeclarativeMouseArea::~QDeclarativeMouseArea()
{
}

/*!
    \qmlproperty real MouseArea::mouseX
    \qmlproperty real MouseArea::mouseY
    These properties hold the coordinates of the mouse.

    If the hoverEnabled property is false then these properties will only be valid
    while a button is pressed, and will remain valid as long as the button is held
    even if the mouse is moved outside the area.

    If hoverEnabled is true then these properties will be valid when:
    \list
        \i no button is pressed, but the mouse is within the MouseArea (containsMouse is true).
        \i a button is pressed and held, even if it has since moved out of the area.
    \endlist

    The coordinates are relative to the MouseArea.
*/
qreal QDeclarativeMouseArea::mouseX() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->lastPos.x();
}

qreal QDeclarativeMouseArea::mouseY() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->lastPos.y();
}

/*!
    \qmlproperty bool MouseArea::enabled
    This property holds whether the item accepts mouse events.
*/
bool QDeclarativeMouseArea::isEnabled() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->absorb;
}

void QDeclarativeMouseArea::setEnabled(bool a)
{
    Q_D(QDeclarativeMouseArea);
    if (a != d->absorb) {
        d->absorb = a;
        emit enabledChanged();
    }
}
/*!
    \qmlproperty MouseButtons MouseArea::pressedButtons
    This property holds the mouse buttons currently pressed.

    It contains a bitwise combination of:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist

    The code below displays "right" when the right mouse buttons is pressed:
    \snippet doc/src/snippets/declarative/mousearea.qml mousebuttons

    \sa acceptedButtons
*/
Qt::MouseButtons QDeclarativeMouseArea::pressedButtons() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->lastButtons;
}

void QDeclarativeMouseArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    d->moved = false;
    if (!d->absorb)
        QDeclarativeItem::mousePressEvent(event);
    else {
        d->longPress = false;
        d->saveEvent(event);
        if (d->drag) {
            d->dragX = drag()->axis() & QDeclarativeDrag::XAxis;
            d->dragY = drag()->axis() & QDeclarativeDrag::YAxis;
        }
        if (d->drag)
            d->drag->setActive(false);
        setHovered(true);
        d->startScene = event->scenePos();
        // we should only start timer if pressAndHold is connected to.
        if (d->isPressAndHoldConnected())
            d->pressAndHoldTimer.start(PressAndHoldDelay, this);
        setKeepMouseGrab(false);
        event->setAccepted(setPressed(true));
    }
}

void QDeclarativeMouseArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb) {
        QDeclarativeItem::mouseMoveEvent(event);
        return;
    }

    d->saveEvent(event);

    // ### we should skip this if these signals aren't used
    // ### can GV handle this for us?
    bool contains = boundingRect().contains(d->lastPos);
    if (d->hovered && !contains)
        setHovered(false);
    else if (!d->hovered && contains)
        setHovered(true);

    if (d->drag && d->drag->target()) {
        if (!d->moved) {
            d->startX = drag()->target()->x();
            d->startY = drag()->target()->y();
        }

        QPointF startLocalPos;
        QPointF curLocalPos;
        if (drag()->target()->parentItem()) {
            startLocalPos = drag()->target()->parentItem()->mapFromScene(d->startScene);
            curLocalPos = drag()->target()->parentItem()->mapFromScene(event->scenePos());
        } else {
            startLocalPos = d->startScene;
            curLocalPos = event->scenePos();
        }

        const int dragThreshold = QApplication::startDragDistance();
        qreal dx = qAbs(curLocalPos.x() - startLocalPos.x());
        qreal dy = qAbs(curLocalPos.y() - startLocalPos.y());
        if ((d->dragX && !(dx < dragThreshold)) || (d->dragY && !(dy < dragThreshold)))
            d->drag->setActive(true);
        if (!keepMouseGrab()) {
            if ((!d->dragY && dy < dragThreshold && d->dragX && dx > dragThreshold)
                || (!d->dragX && dx < dragThreshold && d->dragY && dy > dragThreshold)
                || (d->dragX && d->dragY)) {
                setKeepMouseGrab(true);
            }
        }

        if (d->dragX && d->drag->active()) {
            qreal x = (curLocalPos.x() - startLocalPos.x()) + d->startX;
            if (x < drag()->xmin())
                x = drag()->xmin();
            else if (x > drag()->xmax())
                x = drag()->xmax();
            drag()->target()->setX(x);
        }
        if (d->dragY && d->drag->active()) {
            qreal y = (curLocalPos.y() - startLocalPos.y()) + d->startY;
            if (y < drag()->ymin())
                y = drag()->ymin();
            else if (y > drag()->ymax())
                y = drag()->ymax();
            drag()->target()->setY(y);
        }
        d->moved = true;
    }
    QDeclarativeMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, false, d->longPress);
    emit mousePositionChanged(&me);
    me.setX(d->lastPos.x());
    me.setY(d->lastPos.y());
    emit positionChanged(&me);
}


void QDeclarativeMouseArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb) {
        QDeclarativeItem::mouseReleaseEvent(event);
    } else {
        d->saveEvent(event);
        setPressed(false);
        if (d->drag)
            d->drag->setActive(false);
        // If we don't accept hover, we need to reset containsMouse.
        if (!acceptHoverEvents())
            setHovered(false);
        setKeepMouseGrab(false);
    }
}

void QDeclarativeMouseArea::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb) {
        QDeclarativeItem::mouseDoubleClickEvent(event);
    } else {
        QDeclarativeItem::mouseDoubleClickEvent(event);
        if (event->isAccepted()) {
            // Only deliver the event if we have accepted the press.
            d->saveEvent(event);
            QDeclarativeMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, true, false);
            emit this->doubleClicked(&me);
        }
    }
}

void QDeclarativeMouseArea::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb)
        QDeclarativeItem::hoverEnterEvent(event);
    else
        setHovered(true);
}

void QDeclarativeMouseArea::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb) {
        QDeclarativeItem::hoverEnterEvent(event);
    } else {
        d->lastPos = event->pos();
        QDeclarativeMouseEvent me(d->lastPos.x(), d->lastPos.y(), Qt::NoButton, d->lastButtons, d->lastModifiers, false, d->longPress);
        emit mousePositionChanged(&me);
        me.setX(d->lastPos.x());
        me.setY(d->lastPos.y());
        emit positionChanged(&me);
    }
}

void QDeclarativeMouseArea::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (!d->absorb)
        QDeclarativeItem::hoverLeaveEvent(event);
    else
        setHovered(false);
}

bool QDeclarativeMouseArea::sceneEvent(QEvent *event)
{
    bool rv = QDeclarativeItem::sceneEvent(event);
    if (event->type() == QEvent::UngrabMouse) {
        Q_D(QDeclarativeMouseArea);
        if (d->pressed) {
            // if our mouse grab has been removed (probably by Flickable), fix our
            // state
            d->pressed = false;
            setKeepMouseGrab(false);
            emit canceled();
            emit pressedChanged();
            if (d->hovered) {
                d->hovered = false;
                emit hoveredChanged();
            }
        }
    }
    return rv;
}

void QDeclarativeMouseArea::timerEvent(QTimerEvent *event)
{
    Q_D(QDeclarativeMouseArea);
    if (event->timerId() == d->pressAndHoldTimer.timerId()) {
        d->pressAndHoldTimer.stop();
        bool dragged = d->drag && d->drag->active();
        if (d->pressed && dragged == false && d->hovered == true) {
            d->longPress = true;
            QDeclarativeMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, false, d->longPress);
            emit pressAndHold(&me);
        }
    }
}

void QDeclarativeMouseArea::geometryChanged(const QRectF &newGeometry,
                                            const QRectF &oldGeometry)
{
    Q_D(QDeclarativeMouseArea);
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);

    if (d->lastScenePos.isNull)
        d->lastScenePos = mapToScene(d->lastPos);
    else if (newGeometry.x() != oldGeometry.x() || newGeometry.y() != oldGeometry.y())
        d->lastPos = mapFromScene(d->lastScenePos);
}

/*! \internal */
QVariant QDeclarativeMouseArea::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    Q_D(QDeclarativeMouseArea);
    switch (change) {
    case ItemVisibleHasChanged:
        if (acceptHoverEvents() && d->hovered != (isVisible() && isUnderMouse()))
            setHovered(!d->hovered);
        break;
    default:
        break;
    }

    return QDeclarativeItem::itemChange(change, value);
}

/*!
    \qmlproperty bool MouseArea::hoverEnabled
    This property holds whether hover events are handled.

    By default, mouse events are only handled in response to a button event, or when a button is
    pressed.  Hover enables handling of all mouse events even when no mouse button is
    pressed.

    This property affects the containsMouse property and the onEntered, onExited and onPositionChanged signals.
*/
bool QDeclarativeMouseArea::hoverEnabled() const
{
    return acceptHoverEvents();
}

void QDeclarativeMouseArea::setHoverEnabled(bool h)
{
    Q_D(QDeclarativeMouseArea);
    if (h == acceptHoverEvents())
        return;

    setAcceptHoverEvents(h);
    emit hoverEnabledChanged();
    if (d->hovered != isUnderMouse())
        setHovered(!d->hovered);
}

/*!
    \qmlproperty bool MouseArea::containsMouse
    This property holds whether the mouse is currently inside the mouse area.

    \warning This property is not updated if the area moves under the mouse: \e containsMouse will not change.
    In addition, if hoverEnabled is false, containsMouse will only be valid when the mouse is pressed.
*/
bool QDeclarativeMouseArea::hovered() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->hovered;
}

/*!
    \qmlproperty bool MouseArea::pressed
    This property holds whether the mouse area is currently pressed.
*/
bool QDeclarativeMouseArea::pressed() const
{
    Q_D(const QDeclarativeMouseArea);
    return d->pressed;
}

void QDeclarativeMouseArea::setHovered(bool h)
{
    Q_D(QDeclarativeMouseArea);
    if (d->hovered != h) {
        d->hovered = h;
        emit hoveredChanged();
        d->hovered ? emit entered() : emit exited();
    }
}

/*!
    \qmlproperty Qt::MouseButtons MouseArea::acceptedButtons
    This property holds the mouse buttons that the mouse area reacts to.

    The available buttons are:
    \list
    \o Qt.LeftButton
    \o Qt.RightButton
    \o Qt.MiddleButton
    \endlist

    To accept more than one button the flags can be combined with the
    "|" (or) operator:

    \code
    MouseArea { acceptedButtons: Qt.LeftButton | Qt.RightButton }
    \endcode

    The default value is \c Qt.LeftButton.
*/
Qt::MouseButtons QDeclarativeMouseArea::acceptedButtons() const
{
    return acceptedMouseButtons();
}

void QDeclarativeMouseArea::setAcceptedButtons(Qt::MouseButtons buttons)
{
    if (buttons != acceptedMouseButtons()) {
        setAcceptedMouseButtons(buttons);
        emit acceptedButtonsChanged();
    }
}

bool QDeclarativeMouseArea::setPressed(bool p)
{
    Q_D(QDeclarativeMouseArea);
    bool dragged = d->drag && d->drag->active();
    bool isclick = d->pressed == true && p == false && dragged == false && d->hovered == true;

    if (d->pressed != p) {
        d->pressed = p;
        QDeclarativeMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, isclick, d->longPress);
        if (d->pressed) {
            emit pressed(&me);
            me.setX(d->lastPos.x());
            me.setY(d->lastPos.y());
            emit mousePositionChanged(&me);
        } else {
            emit released(&me);
            me.setX(d->lastPos.x());
            me.setY(d->lastPos.y());
            if (isclick && !d->longPress)
                emit clicked(&me);
        }

        emit pressedChanged();
        return me.isAccepted();
    }
    return false;
}

QDeclarativeDrag *QDeclarativeMouseArea::drag()
{
    Q_D(QDeclarativeMouseArea);
    if (!d->drag)
        d->drag = new QDeclarativeDrag;
    return d->drag;
}

/*!
    \qmlproperty Item MouseArea::drag.target
    \qmlproperty bool MouseArea::drag.active
    \qmlproperty enumeration MouseArea::drag.axis
    \qmlproperty real MouseArea::drag.minimumX
    \qmlproperty real MouseArea::drag.maximumX
    \qmlproperty real MouseArea::drag.minimumY
    \qmlproperty real MouseArea::drag.maximumY

    \c drag provides a convenient way to make an item draggable.

    \list
    \i \c drag.target specifies the id of the item to drag.
    \i \c drag.active specifies if the target item is currently being dragged.
    \i \c drag.axis specifies whether dragging can be done horizontally (\c Drag.XAxis), vertically (\c Drag.YAxis), or both (\c Drag.XandYAxis)
    \i \c drag.minimum and \c drag.maximum limit how far the target can be dragged along the corresponding axes.
    \endlist

    The following example displays an image that can be dragged along the X-axis. The opacity
    of the image is reduced when it is dragged to the right.

    \snippet doc/src/snippets/declarative/mousearea.qml drag
*/

QT_END_NAMESPACE
