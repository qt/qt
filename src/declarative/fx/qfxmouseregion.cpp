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
#include <QGraphicsSceneMouseEvent>


QT_BEGIN_NAMESPACE
static const int DragThreshold = 5;
static const int PressAndHoldDelay = 800;

QML_DEFINE_TYPE(QFxDrag,Drag);
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
int QFxDrag::xmin() const
{
    return _xmin;
}

void QFxDrag::setXmin(int m)
{
    _xmin = m;
}

/*!
    \property QFxDrag::xmax
    \brief the maximum x position for the target

    If x-axis dragging is enabled, xmax limits how far to the right the target can be dragged. If x-axis dragging is not enabled, this property has no effect.
*/
int QFxDrag::xmax() const
{
    return _xmax;
}

void QFxDrag::setXmax(int m)
{
    _xmax = m;
}

/*!
    \property QFxDrag::ymin
    \brief the minimum y position for the target

    If y-axis dragging is enabled, ymin limits how far up the target can be dragged. If y-axis dragging is not enabled, this property has no effect.
*/
int QFxDrag::ymin() const
{
    return _ymin;
}

void QFxDrag::setYmin(int m)
{
    _ymin = m;
}

/*!
    \property QFxDrag::ymax
    \brief the maximum y position for the target

    If y-axis dragging is enabled, ymax limits how far down the target can be dragged. If y-axis dragging is not enabled, this property has no effect.
*/
int QFxDrag::ymax() const
{
    return _ymax;
}

void QFxDrag::setYmax(int m)
{
    _ymax = m;
}

/*!
    \qmlclass MouseRegion
    \brief The MouseRegion element enables simple mouse handling.
    \inherits Item

    A MouseRegion is typically used in conjunction with a visible element, where the MouseRegion effectively 'proxies' mouse handling for that element. For example, we can put a MouseRegion in a Rect that changes the Rect color to red when clicked:
    \code
    <Rect width="100" height="100">
        <MouseRegion anchors.fill="{parent}" onClick="parent.color = 'red';"/>
    </Rect>
    \endcode

    For the mouse handlers the variable mouseButton is set to be one of 'Left', 'Right', 'Middle',
    or 'None'. This allows you to distinguish left and right clicking. Below we have the previous 
    example extended so as to give a different color when you right click.
    \code
    <Rect width="100" height="100">
        <MouseRegion anchors.fill="{parent}" onClick="if(mouseButton=='Right') { parent.color='blue';} else { parent.color = 'red';}"/>
    </Rect>
    \endcode

    For basic key handling, see \l KeyActions.

    MouseRegion is an invisible element: it is never painted.
*/

/*!
    \qmlsignal MouseRegion::onEntered

    This handler is called when the mouse enters the mouse region.

    \warning This handler is not yet implemented.
*/

/*!
    \qmlsignal MouseRegion::onExited

    This handler is called when the mouse exists the mouse region.

    \warning This handler is not yet implemented.
*/

/*!
    \qmlsignal MouseRegion::onReenteredWhilePressed

    This handler is called when the mouse reenters the mouse region while pressed.
*/

/*!
    \qmlsignal MouseRegion::onExitedWhilePressed

    This handler is called when the mouse exists the mouse region while pressed.
*/

/*!
    \qmlsignal MouseRegion::onClicked

    This handler is called when there is a click. A click is defined as a press followed by a release,
    both inside the MouseRegion (pressing, moving outside the MouseRegion, and then moving back inside and
    releasing is also considered a click).
    The x and y parameters tell you the position of the release of the click. The followsPressAndHold parameter tells
    you whether or not the release portion of the click followed a long press.
*/

/*!
    \qmlsignal MouseRegion::onPressed

    This handler is called when there is a press.
    The x and y parameters tell you the position of the press.
*/

/*!
    \qmlsignal MouseRegion::onReleased

    This handler is called when there is a release.
    The x and y parameters tell you the position of the release. The isClick parameter tells you whether
    or not the release is part of a click. The followsPressAndHold parameter tells you whether or not the
    release followed a long press.
*/

/*!
    \qmlsignal MouseRegion::onPressAndHold

    This handler is called when there is a long press (currently 800ms).
    The x and y parameters tell you the position of the long press.
*/

/*!
    \qmlsignal MouseRegion::onDoubleClicked

    This handler is called when there is a double-click (a press followed by a release followed by a press).
    The x and y parameters tell you the position of the double-click.
*/

QML_DEFINE_TYPE(QFxMouseRegion,MouseRegion);
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
    \qmlproperty int MouseRegion::mouseX
    \qmlproperty int MouseRegion::mouseY

    The coordinates of the mouse while pressed. The coordinates are relative to the item that was pressed.
*/
int QFxMouseRegion::mouseX() const
{
    Q_D(const QFxMouseRegion);
    return int(d->lastPos.x());
}

int QFxMouseRegion::mouseY() const
{
    Q_D(const QFxMouseRegion);
    return int(d->lastPos.y());
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

void QFxMouseRegionPrivate::bindButtonValue(Qt::MouseButton b)
{
    Q_Q(QFxMouseRegion);
    QString bString;
    switch(b){
        case Qt::LeftButton:
            bString = QLatin1String("Left"); break;
        case Qt::RightButton:
            bString = QLatin1String("Right"); break;
        case Qt::MidButton:
            bString = QLatin1String("Middle"); break;
        default:
            bString = QLatin1String("None"); break;
    }
    // ### is this needed anymore?
    qmlContext(q)->setContextProperty(QLatin1String("mouseButton"), bString);
}

void QFxMouseRegion::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    d->moved = false;
    if(!d->absorb)
        QFxItem::mousePressEvent(event);
    else {
        if (!d->inside) {
            d->inside = true;
            emit hoveredChanged();
        }
        d->longPress = false;
        d->lastPos = event->pos();
        d->dragX = drag()->axis().contains(QLatin1String("x"));
        d->dragY = drag()->axis().contains(QLatin1String("y"));
        d->dragged = false;
        d->start = event->pos();
        d->startScene = event->scenePos();
        // ### we should only start timer if pressAndHold is connected to (but connectNotify doesn't work)
        d->pressAndHoldTimer.start(PressAndHoldDelay, this);
        setKeepMouseGrab(false);
        d->bindButtonValue(event->button());
        setPressed(true);
        emit positionChanged();
        event->accept();
    }
}

void QFxMouseRegion::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if(!d->absorb) {
        QFxItem::mouseMoveEvent(event);
        return;
    }

    d->lastPos = event->pos();

    // ### we should skip this if these signals aren't used
    const QRect &bounds = itemBoundingRect();
    bool contains = bounds.contains(d->lastPos.toPoint());
    if (d->inside && !contains) {
        d->inside = false;
        emit hoveredChanged();
        emit exitedWhilePressed();
    } else if (!d->inside && contains) {
        d->inside = true;
        emit hoveredChanged();
        emit reenteredWhilePressed();
    }

    if(drag()->target()) {
        if(!d->moved) {
            if(d->dragX) d->startX = int(drag()->target()->x());   //### change startX and startY to qreal?
            if(d->dragY) d->startY = int(drag()->target()->y());
        }

        QPointF startLocalPos;
        QPointF curLocalPos;
        if (drag()->target()->parent()) {
            startLocalPos = drag()->target()->parent()->mapFromScene(d->startScene);
            curLocalPos = drag()->target()->parent()->mapFromScene(event->scenePos());
        } else {
            startLocalPos = d->startScene;
            curLocalPos = event->scenePos();
        }

        int dx = int(qAbs(curLocalPos.x() - startLocalPos.x()));
        int dy = int(qAbs(curLocalPos.y() - startLocalPos.y()));
        if ((d->dragX && !(dx < DragThreshold)) || (d->dragY && !(dy < DragThreshold)))
            d->dragged = true;
        if (!keepMouseGrab()) {
            if ((!d->dragY && dy < DragThreshold && d->dragX && dx > DragThreshold)
                || (!d->dragX && dx < DragThreshold && d->dragY && dy > DragThreshold)
                || (d->dragX && d->dragY)) {
                setKeepMouseGrab(true);
            }
        }

        if(d->dragX) {
            qreal x = (curLocalPos.x() - startLocalPos.x()) + d->startX;
            if (x < drag()->xmin())
                x = drag()->xmin();
            else if (x > drag()->xmax())
                x = drag()->xmax();
            drag()->target()->setX(x);
        }
        if(d->dragY) {
            qreal y = (curLocalPos.y() - startLocalPos.y()) + d->startY;
            if (y < drag()->ymin())
                y = drag()->ymin();
            else if (y > drag()->ymax())
                y = drag()->ymax();
            drag()->target()->setY(y);
        }
    }
    d->moved = true;
    emit positionChanged();
    event->accept();
}


void QFxMouseRegion::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if(!d->absorb)
        QFxItem::mouseReleaseEvent(event);
    else {
        setPressed(false);
        //d->inside = false;
        //emit hoveredChanged();
        event->accept();
    }
}

void QFxMouseRegion::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxMouseRegion);
    if(!d->absorb)
        QFxItem::mouseDoubleClickEvent(event);
    else {
        //d->inside = true;
        //emit hoveredChanged();
        setPressed(true);
        emit this->doubleClicked(d->lastPos.x(), d->lastPos.y());
        event->accept();
    }
}

void QFxMouseRegion::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QFxMouseRegion);
    if(!d->absorb)
        QFxItem::hoverEnterEvent(event);
    else {
        setHovered(true);
        emit entered();
    }
}

void QFxMouseRegion::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_D(QFxMouseRegion);
    if(!d->absorb)
        QFxItem::hoverLeaveEvent(event);
    else {
        setHovered(false);
        emit exited();
    }
}

void QFxMouseRegion::mouseUngrabEvent()
{
    Q_D(QFxMouseRegion);
    if (d->pressed) {
        // if our mouse grab has been removed (probably by Flickable), fix our 
        // state
        d->pressed = false;
        //d->inside = false;
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
        if (d->pressed && d->dragged == false && d->inside == true) {
            d->longPress = true;
            emit pressAndHold(d->lastPos.x(), d->lastPos.y());
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
    return d->hovered || d->inside;
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
    if(d->hovered != h) {
        d->hovered = h;
        emit hoveredChanged();
    }
}

void QFxMouseRegion::setPressed(bool p)
{
    Q_D(QFxMouseRegion);
    bool isclick = d->pressed == true && p == false && d->dragged == false && d->inside == true;

    if(d->pressed != p) {
        d->pressed = p;
        if(d->pressed)
            emit pressed(d->lastPos.x(), d->lastPos.y());
        else {
            emit released(d->lastPos.x(), d->lastPos.y(), isclick, d->longPress);
            if (isclick)
                emit clicked(d->lastPos.x(), d->lastPos.y(), d->longPress);
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
    \qmlproperty int MouseRegion::drag.xmin
    \qmlproperty int MouseRegion::drag.xmax
    \qmlproperty int MouseRegion::drag.ymin
    \qmlproperty int MouseRegion::drag.ymax

    drag provides a convenient way to make an item draggable.

    \list
    \i \c target specifies the item to drag.
    \i \c axis specifies whether dragging can be done horizontally (x), vertically (y), or both (x,y)
    \i the min and max properties limit how far the target can be dragged along the corresponding axes.
    \endlist

    The following example uses drag to blur an image as it moves to the right:
    \code
    <Item id="blurtest" width="600" height="200">
        <Image id="pic" file="pic.png" anchors.verticalCenter="{parent.verticalCenter}" >
            <filter><Blur radius="{pic.x/10}"/></filter>
            <MouseRegion anchors.fill="{parent}"
                        drag.target="{pic}"
                        drag.axis="x"
                        drag.xmin="0"
                        drag.xmax="{blurtest.width-pic.width}" />
        </Image>
    </Item>
    \endcode
*/

QT_END_NAMESPACE
