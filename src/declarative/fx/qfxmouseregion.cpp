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

#include "qfxmouseregion.h"
#include "qfxmouseregion_p.h"
#include "qfxevents_p.h"
#include <QGraphicsSceneMouseEvent>


QT_BEGIN_NAMESPACE
static const qreal DragThreshold = 5;
static const int PressAndHoldDelay = 800;

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Drag,QFxDrag)
QFxDrag::QFxDrag(QObject *parent)
: QObject(parent), _target(0), _xmin(0), _xmax(0), _ymin(0), _ymax(0)
{
}

QFxDrag::~QFxDrag()
{
}

QFxItem *QFxDrag::target() const
{
    return _target;
}

void QFxDrag::setTarget(QFxItem *t)
{
    _target = t;
}

QString QFxDrag::axis() const
{
    return _axis;
}

void QFxDrag::setAxis(const QString &a)
{
    _axis = a;
}

/*!
    \property QFxDrag::xmin
    \brief the minimum x position for the target

    If x-axis dragging is enabled, xmin limits how far to the left the target can be dragged. If x-axis dragging is not enabled, this property has no effect.
*/
qreal QFxDrag::xmin() const
{
    return _xmin;
}

void QFxDrag::setXmin(qreal m)
{
    _xmin = m;
}

/*!
    \property QFxDrag::xmax
    \brief the maximum x position for the target

    If x-axis dragging is enabled, xmax limits how far to the right the target can be dragged. If x-axis dragging is not enabled, this property has no effect.
*/
qreal QFxDrag::xmax() const
{
    return _xmax;
}

void QFxDrag::setXmax(qreal m)
{
    _xmax = m;
}

/*!
    \property QFxDrag::ymin
    \brief the minimum y position for the target

    If y-axis dragging is enabled, ymin limits how far up the target can be dragged. If y-axis dragging is not enabled, this property has no effect.
*/
qreal QFxDrag::ymin() const
{
    return _ymin;
}

void QFxDrag::setYmin(qreal m)
{
    _ymin = m;
}

/*!
    \property QFxDrag::ymax
    \brief the maximum y position for the target

    If y-axis dragging is enabled, ymax limits how far down the target can be dragged. If y-axis dragging is not enabled, this property has no effect.
*/
qreal QFxDrag::ymax() const
{
    return _ymax;
}

void QFxDrag::setYmax(qreal m)
{
    _ymax = m;
}

/*!
    \qmlclass MouseRegion
    \brief The MouseRegion item enables simple mouse handling.
    \inherits Item

    A MouseRegion is typically used in conjunction with a visible item,
    where the MouseRegion effectively 'proxies' mouse handling for that
    item. For example, we can put a MouseRegion in a Rect that changes
    the Rect color to red when clicked:
    \snippet doc/src/snippets/declarative/mouseregion.qml 0

    Many MouseRegion signals pass a \l {MouseEvent}{mouse} parameter that contains
    additional information about the mouse event, such as the position, button,
    and any key modifiers.

    Below we have the previous
    example extended so as to give a different color when you right click.
    \snippet doc/src/snippets/declarative/mouseregion.qml 1

    For basic key handling, see \l KeyActions.

    MouseRegion is an invisible item: it is never painted.

    \sa MouseEvent
*/

/*!
    \qmlsignal MouseRegion::onEntered

    This handler is called when the mouse enters the mouse region.
*/

/*!
    \qmlsignal MouseRegion::onExited

    This handler is called when the mouse exists the mouse region.
*/

/*!
    \qmlsignal MouseRegion::onPositionChanged(mouse)

    This handler is called when the mouse position changes.

    The \l {MouseEvent}{mouse} parameter provides information about the mouse, including the x and y
    position, and any buttons currently pressed.
*/

/*!
    \qmlsignal MouseRegion::onClicked(mouse)

    This handler is called when there is a click. A click is defined as a press followed by a release,
    both inside the MouseRegion (pressing, moving outside the MouseRegion, and then moving back inside and
    releasing is also considered a click).

    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click wasHeld.
*/

/*!
    \qmlsignal MouseRegion::onPressed(mouse)

    This handler is called when there is a press.
    The \l {MouseEvent}{mouse} parameter provides information about the press, including the x and y
    position and which button was pressed.
*/

/*!
    \qmlsignal MouseRegion::onReleased(mouse)

    This handler is called when there is a release.
    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click wasHeld.
*/

/*!
    \qmlsignal MouseRegion::onPressAndHold(mouse)

    This handler is called when there is a long press (currently 800ms).
    The \l {MouseEvent}{mouse} parameter provides information about the press, including the x and y
    position of the press, and which button is pressed.
*/

/*!
    \qmlsignal MouseRegion::onDoubleClicked(mouse)

    This handler is called when there is a double-click (a press followed by a release followed by a press).
    The \l {MouseEvent}{mouse} parameter provides information about the click, including the x and y
    position of the release of the click, and whether the click wasHeld.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,MouseRegion,QFxMouseRegion)

/*!
    \internal
    \class QFxMouseRegion
    \brief The QFxMouseRegion class provides a simple mouse handling abstraction for use within Qml.

    \ingroup group_coreitems

    All QFxItem derived classes can do mouse handling but the QFxMouseRegion class exposes mouse
    handling data as properties and tracks flicking and dragging of the mouse.

    A QFxMouseRegion object can be instantiated in Qml using the tag \l MouseRegion.
 */
QFxMouseRegion::QFxMouseRegion(QFxItem *parent)
  : QFxItem(*(new QFxMouseRegionPrivate), parent)
{
    Q_D(QFxMouseRegion);
    d->init();
}

QFxMouseRegion::QFxMouseRegion(QFxMouseRegionPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxMouseRegion);
    d->init();
}

QFxMouseRegion::~QFxMouseRegion()
{
}

/*!
    \qmlproperty real MouseRegion::mouseX
    \qmlproperty real MouseRegion::mouseY

    The coordinates of the mouse while pressed. The coordinates are relative to the item that was pressed.
*/
qreal QFxMouseRegion::mouseX() const
{
    Q_D(const QFxMouseRegion);
    return d->lastPos.x();
}

qreal QFxMouseRegion::mouseY() const
{
    Q_D(const QFxMouseRegion);
    return d->lastPos.y();
}

/*!
    \qmlproperty bool MouseRegion::enabled
    This property holds whether the item accepts mouse events.
*/
bool QFxMouseRegion::isEnabled() const
{
    Q_D(const QFxMouseRegion);
    return d->absorb;
}

void QFxMouseRegion::setEnabled(bool a)
{
    Q_D(QFxMouseRegion);
    d->absorb = a;
}

void QFxMouseRegion::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    d->moved = false;
    if (!d->absorb)
        QFxItem::mousePressEvent(event);
    else {
        d->longPress = false;
        d->saveEvent(event);
        d->dragX = drag()->axis().contains(QLatin1String("x"));
        d->dragY = drag()->axis().contains(QLatin1String("y"));
        d->dragged = false;
        d->start = event->pos();
        d->startScene = event->scenePos();
        // ### we should only start timer if pressAndHold is connected to (but connectNotify doesn't work)
        d->pressAndHoldTimer.start(PressAndHoldDelay, this);
        setKeepMouseGrab(false);
        setPressed(true);
        event->accept();
    }
}

void QFxMouseRegion::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if (!d->absorb) {
        QFxItem::mouseMoveEvent(event);
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

    if (drag()->target()) {
        if (!d->moved) {
            if (d->dragX) d->startX = drag()->target()->x();
            if (d->dragY) d->startY = drag()->target()->y();
        }

        QPointF startLocalPos;
        QPointF curLocalPos;
        if (drag()->target()->parent()) {
            startLocalPos = drag()->target()->parentItem()->mapFromScene(d->startScene);
            curLocalPos = drag()->target()->parentItem()->mapFromScene(event->scenePos());
        } else {
            startLocalPos = d->startScene;
            curLocalPos = event->scenePos();
        }

        qreal dx = qAbs(curLocalPos.x() - startLocalPos.x());
        qreal dy = qAbs(curLocalPos.y() - startLocalPos.y());
        if ((d->dragX && !(dx < DragThreshold)) || (d->dragY && !(dy < DragThreshold)))
            d->dragged = true;
        if (!keepMouseGrab()) {
            if ((!d->dragY && dy < DragThreshold && d->dragX && dx > DragThreshold)
                || (!d->dragX && dx < DragThreshold && d->dragY && dy > DragThreshold)
                || (d->dragX && d->dragY)) {
                setKeepMouseGrab(true);
            }
        }

        if (d->dragX) {
            qreal x = (curLocalPos.x() - startLocalPos.x()) + d->startX;
            if (x < drag()->xmin())
                x = drag()->xmin();
            else if (x > drag()->xmax())
                x = drag()->xmax();
            drag()->target()->setX(x);
        }
        if (d->dragY) {
            qreal y = (curLocalPos.y() - startLocalPos.y()) + d->startY;
            if (y < drag()->ymin())
                y = drag()->ymin();
            else if (y > drag()->ymax())
                y = drag()->ymax();
            drag()->target()->setY(y);
        }
    }
    d->moved = true;
    QFxMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, false, d->longPress);
    emit positionChanged(&me);
    event->accept();
}


void QFxMouseRegion::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if (!d->absorb)
        QFxItem::mouseReleaseEvent(event);
    else {
        d->saveEvent(event);
        setPressed(false);
        event->accept();
    }
}

void QFxMouseRegion::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if (!d->absorb)
        QFxItem::mouseDoubleClickEvent(event);
    else {
        d->saveEvent(event);
        setPressed(true);
        QFxMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, true, false);
        emit this->doubleClicked(&me);
        event->accept();
    }
}

void QFxMouseRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QFxMouseRegion);
    if (!d->absorb)
        QFxItem::hoverEnterEvent(event);
    else {
        setHovered(true);
    }
}

void QFxMouseRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QFxMouseRegion);
    if (!d->absorb)
        QFxItem::hoverLeaveEvent(event);
    else {
        setHovered(false);
    }
}

void QFxMouseRegion::mouseUngrabEvent()
{
    Q_D(QFxMouseRegion);
    if (d->pressed) {
        // if our mouse grab has been removed (probably by Flickable), fix our 
        // state
        d->pressed = false;
        setKeepMouseGrab(false);
        emit pressedChanged();
        //emit hoveredChanged();
    }
}

void QFxMouseRegion::timerEvent(QTimerEvent *event)
{
    Q_D(QFxMouseRegion);
    if (event->timerId() == d->pressAndHoldTimer.timerId()) {
        d->pressAndHoldTimer.stop();
        if (d->pressed && d->dragged == false && d->hovered == true) {
            d->longPress = true;
            QFxMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, false, d->longPress);
            emit pressAndHold(&me);
        }
    }
}

/*!
    \qmlproperty bool MouseRegion::containsMouse
    This property holds whether the mouse is currently inside the mouse region.

    \warning This property is only partially implemented -- it is only valid when the mouse is pressed, and not for hover events.
*/
bool QFxMouseRegion::hovered()
{
    Q_D(QFxMouseRegion);
    return d->hovered;
}

/*!
    \qmlproperty bool MouseRegion::pressed
    This property holds whether the mouse region is currently pressed.
*/
bool QFxMouseRegion::pressed()
{
    Q_D(QFxMouseRegion);
    return d->pressed;
}

void QFxMouseRegion::setHovered(bool h)
{
    Q_D(QFxMouseRegion);
    if (d->hovered != h) {
        d->hovered = h;
        emit hoveredChanged();
        d->hovered ? emit entered() : emit exited();
    }
}

void QFxMouseRegion::setPressed(bool p)
{
    Q_D(QFxMouseRegion);
    bool isclick = d->pressed == true && p == false && d->dragged == false && d->hovered == true;

    if (d->pressed != p) {
        d->pressed = p;
        QFxMouseEvent me(d->lastPos.x(), d->lastPos.y(), d->lastButton, d->lastButtons, d->lastModifiers, isclick, d->longPress);
        if (d->pressed) {
            emit positionChanged(&me);
            emit pressed(&me);
        } else {
            emit released(&me);
            if (isclick)
                emit clicked(&me);
        }

        emit pressedChanged();
    }
}

/*!
    \property QFxMouseRegion::drag
    \brief The current drag being performed on the Mouse Region.
*/
QFxDrag *QFxMouseRegion::drag()
{
    Q_D(QFxMouseRegion);
    return &(d->drag);
}

/*!
    \qmlproperty Item MouseRegion::drag.target
    \qmlproperty string MouseRegion::drag.axis
    \qmlproperty real MouseRegion::drag.xmin
    \qmlproperty real MouseRegion::drag.xmax
    \qmlproperty real MouseRegion::drag.ymin
    \qmlproperty real MouseRegion::drag.ymax

    drag provides a convenient way to make an item draggable.

    \list
    \i \c target specifies the item to drag.
    \i \c axis specifies whether dragging can be done horizontally (x), vertically (y), or both (x,y)
    \i the min and max properties limit how far the target can be dragged along the corresponding axes.
    \endlist

    The following example uses drag to reduce the opacity of an image as it moves to the right:
    \snippet doc/src/snippets/declarative/drag.qml 0
*/

QT_END_NAMESPACE
