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

#include "qmlgraphicsitem_p.h"
#include "qmlgraphicsitem.h"

#include "qmlgraphicsevents_p_p.h"

#include <qfxperf_p_p.h>
#include <qmlengine.h>
#include <qmlopenmetaobject_p.h>
#include <qmlstate_p.h>
#include <qmlview.h>
#include <qmlstategroup_p.h>
#include <qmlcomponent.h>

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include <QtCore/qnumeric.h>
#include <QtScript/qscriptengine.h>
#include <QtGui/qgraphicstransform.h>
#include <QtGui/qgraphicseffect.h>
#include <qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE

#ifndef FLT_MAX
#define FLT_MAX 1E+37
#endif

#include "qmlgraphicseffects.cpp"

/*!
    \qmlclass Transform QGraphicsTransform
    \brief The Transform elements provide a way of building advanced transformations on Items.

    The Transform elements let you create and control advanced transformations that can be configured
    independently using specialized properties.

    You can assign any number of Transform elements to an Item. Each Transform is applied in order,
    one at a time, to the Item it's assigned to.

    \sa Rotation, Scale
*/

/*!
    \qmlclass Scale QGraphicsScale
    \brief The Scale object provides a way to scale an Item.

    The Scale object gives more control over scaling than using Item's scale property. Specifically,
    it allows a different scale for the x and y axes, and allows the scale to be relative to an
    arbitrary point.

    The following example scales the X axis of the Rectangle, relative to its interior point 25, 25:
    \qml
    Rectangle {
        width: 100; height: 100
        color: "blue"
        transform: Scale { origin.x: 25; origin.y: 25; xScale: 3}
    }
    \endqml
*/

/*!
    \qmlproperty real Scale::origin.x
    \qmlproperty real Scale::origin.y

    The point that the item is scaled from (i.e., the point that stays fixed relative to the parent as
    the rest of the item grows). By default the origin is 0, 0.
*/

/*!
    \qmlproperty real Scale::xScale

    The scaling factor for the X axis.
*/

/*!
    \qmlproperty real Scale::yScale

    The scaling factor for the Y axis.
*/

/*!
    \qmlclass Rotation QGraphicsRotation
    \brief The Rotation object provides a way to rotate an Item.

    The Rotation object gives more control over rotation than using Item's rotation property.
    Specifically, it allows (z axis) rotation to be relative to an arbitrary point.

    The following example rotates a Rectangle around its interior point 25, 25:
    \qml
    Rectangle {
        width: 100; height: 100
        color: "blue"
        transform: Rotation { origin.x: 25; origin.y: 25; angle: 45}
    }
    \endqml

    Rotation also provides a way to specify 3D-like rotations for Items. For these types of
    rotations you must specify the axis to rotate around in addition to the origin point.

    The following example shows various 3D-like rotations applied to an \l Image.
    \snippet doc/src/snippets/declarative/rotation.qml 0

    \image axisrotation.png
*/

/*!
    \qmlproperty real Rotation::origin.x
    \qmlproperty real Rotation::origin.y

    The origin point of the rotation (i.e., the point that stays fixed relative to the parent as
    the rest of the item rotates). By default the origin is 0, 0.
*/

/*!
    \qmlproperty real Rotation::axis.x
    \qmlproperty real Rotation::axis.y
    \qmlproperty real Rotation::axis.z

    The axis to rotate around. For simple (2D) rotation around a point, you do not need to specify an axis,
    as the default axis is the z axis (\c{ axis { x: 0; y: 0; z: 1 } }).

    For a typical 3D-like rotation you will usually specify both the origin and the axis.

    \image 3d-rotation-axis.png
*/

/*!
    \qmlproperty real Rotation::angle

    The angle to rotate, in degrees clockwise.
*/


/*!
    \group group_animation
    \title Animation
*/

/*!
    \group group_coreitems
    \title Basic Items
*/

/*!
    \group group_effects
    \title Effects
*/

/*!
    \group group_layouts
    \title Layouts
*/

/*!
    \group group_states
    \title States and Transitions
*/

/*!
    \group group_utility
    \title Utility
*/

/*!
    \group group_views
    \title Views
*/

/*!
    \group group_widgets
    \title Widgets
*/

/*!
    \internal
    \class QmlGraphicsContents
    \ingroup group_utility
    \brief The QmlGraphicsContents class gives access to the height and width of an item's contents.

*/

QmlGraphicsContents::QmlGraphicsContents() : m_x(0), m_y(0), m_width(0), m_height(0)
{
}

/*!
    \qmlproperty real Item::childrenRect.x
    \qmlproperty real Item::childrenRect.y
    \qmlproperty real Item::childrenRect.width
    \qmlproperty real Item::childrenRect.height

    The childrenRect properties allow an item access to the geometry of its
    children. This property is useful if you have an item that needs to be
    sized to fit its children.
*/

QRectF QmlGraphicsContents::rectF() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

//TODO: optimization: only check sender(), if there is one
void QmlGraphicsContents::calcHeight()
{
    qreal oldy = m_y;
    qreal oldheight = m_height;

    qreal top = FLT_MAX;
    qreal bottom = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QmlGraphicsItem *child = qobject_cast<QmlGraphicsItem *>(children.at(i));
        if(!child)//### Should this be ignoring non-QmlGraphicsItem graphicsobjects?
            continue;
        qreal y = child->y();
        if (y + child->height() > bottom)
            bottom = y + child->height();
        if (y < top)
            top = y;
    }
    if (!children.isEmpty())
        m_y = top;
    m_height = qMax(bottom - top, qreal(0.0));

    if (m_height != oldheight || m_y != oldy)
        emit rectChanged();
}

//TODO: optimization: only check sender(), if there is one
void QmlGraphicsContents::calcWidth()
{
    qreal oldx = m_x;
    qreal oldwidth = m_width;

    qreal left = FLT_MAX;
    qreal right = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QmlGraphicsItem *child = qobject_cast<QmlGraphicsItem *>(children.at(i));
        if(!child)//### Should this be ignoring non-QmlGraphicsItem graphicsobjects?
            continue;
        qreal x = child->x();
        if (x + child->width() > right)
            right = x + child->width();
        if (x < left)
            left = x;
    }
    if (!children.isEmpty())
        m_x = left;
    m_width = qMax(right - left, qreal(0.0));

    if (m_width != oldwidth || m_x != oldx)
        emit rectChanged();
}

void QmlGraphicsContents::setItem(QmlGraphicsItem *item)
{
    m_item = item;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QmlGraphicsItem *child = qobject_cast<QmlGraphicsItem *>(children.at(i));
        if(!child)//### Should this be ignoring non-QmlGraphicsItem graphicsobjects?
            continue;
        connect(child, SIGNAL(heightChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(yChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(widthChanged()), this, SLOT(calcWidth()));
        connect(child, SIGNAL(xChanged()), this, SLOT(calcWidth()));
        connect(this, SIGNAL(rectChanged()), m_item, SIGNAL(childrenRectChanged()));
    }

    calcHeight();
    calcWidth();
}

QmlGraphicsItemKeyFilter::QmlGraphicsItemKeyFilter(QmlGraphicsItem *item)
: m_next(0)
{
    QmlGraphicsItemPrivate *p =
        item?static_cast<QmlGraphicsItemPrivate *>(QGraphicsItemPrivate::get(item)):0;
    if (p) {
        m_next = p->keyHandler;
        p->keyHandler = this;
    }
}

QmlGraphicsItemKeyFilter::~QmlGraphicsItemKeyFilter()
{
}

void QmlGraphicsItemKeyFilter::keyPressed(QKeyEvent *event)
{
    if (m_next) m_next->keyPressed(event);
}

void QmlGraphicsItemKeyFilter::keyReleased(QKeyEvent *event)
{
    if (m_next) m_next->keyReleased(event);
}

void QmlGraphicsItemKeyFilter::inputMethodEvent(QInputMethodEvent *event)
{
    if (m_next) m_next->inputMethodEvent(event);
}

QVariant QmlGraphicsItemKeyFilter::inputMethodQuery(Qt::InputMethodQuery query) const
{
    if (m_next) return m_next->inputMethodQuery(query);
    return QVariant();
}

void QmlGraphicsItemKeyFilter::componentComplete()
{
    if (m_next) m_next->componentComplete();
}


/*!
    \qmlclass KeyNavigation
    \brief The KeyNavigation attached property supports key navigation by arrow keys.

    It is common in key-based UIs to use arrow keys to navigate
    between focussed items.  The KeyNavigation property provides a
    convenient way of specifying which item will gain focus
    when an arrow key is pressed.  The following example provides
    key navigation for a 2x2 grid of items.

    \code
    Grid {
        columns: 2
        width: 100; height: 100
        Rectangle {
            id: item1
            focus: true
            width: 50; height: 50
            color: focus ? "red" : "lightgray"
            KeyNavigation.right: item2
            KeyNavigation.down: item3
        }
        Rectangle {
            id: item2
            width: 50; height: 50
            color: focus ? "red" : "lightgray"
            KeyNavigation.left: item1
            KeyNavigation.down: item4
        }
        Rectangle {
            id: item3
            width: 50; height: 50
            color: focus ? "red" : "lightgray"
            KeyNavigation.right: item4
            KeyNavigation.up: item1
        }
        Rectangle {
            id: item4
            width: 50; height: 50
            color: focus ? "red" : "lightgray"
            KeyNavigation.left: item3
            KeyNavigation.up: item2
        }
    }
    \endcode

    KeyNavigation receives key events after the item it is attached to.
    If the item accepts an arrow key event, the KeyNavigation
    attached property will not receive an event for that key.

    If an item has been set for a direction and the KeyNavigation
    attached property receives the corresponding
    key press and release events, the events will be accepted by
    KeyNaviagtion and will not propagate any further.

    \sa {Keys}{Keys attached property}
*/

/*!
    \qmlproperty Item KeyNavigation::left
    \qmlproperty Item KeyNavigation::right
    \qmlproperty Item KeyNavigation::up
    \qmlproperty Item KeyNavigation::down

    These properties hold the item to assign focus to
    when Key_Left, Key_Right, Key_Up or Key_Down are
    pressed.
*/

QmlGraphicsKeyNavigationAttached::QmlGraphicsKeyNavigationAttached(QObject *parent)
: QObject(*(new QmlGraphicsKeyNavigationAttachedPrivate), parent),
  QmlGraphicsItemKeyFilter(qobject_cast<QmlGraphicsItem*>(parent))
{
}

QmlGraphicsKeyNavigationAttached *
QmlGraphicsKeyNavigationAttached::qmlAttachedProperties(QObject *obj)
{
    return new QmlGraphicsKeyNavigationAttached(obj);
}

QmlGraphicsItem *QmlGraphicsKeyNavigationAttached::left() const
{
    Q_D(const QmlGraphicsKeyNavigationAttached);
    return d->left;
}

void QmlGraphicsKeyNavigationAttached::setLeft(QmlGraphicsItem *i)
{
    Q_D(QmlGraphicsKeyNavigationAttached);
    d->left = i;
    emit changed();
}

QmlGraphicsItem *QmlGraphicsKeyNavigationAttached::right() const
{
    Q_D(const QmlGraphicsKeyNavigationAttached);
    return d->right;
}

void QmlGraphicsKeyNavigationAttached::setRight(QmlGraphicsItem *i)
{
    Q_D(QmlGraphicsKeyNavigationAttached);
    d->right = i;
    emit changed();
}

QmlGraphicsItem *QmlGraphicsKeyNavigationAttached::up() const
{
    Q_D(const QmlGraphicsKeyNavigationAttached);
    return d->up;
}

void QmlGraphicsKeyNavigationAttached::setUp(QmlGraphicsItem *i)
{
    Q_D(QmlGraphicsKeyNavigationAttached);
    d->up = i;
    emit changed();
}

QmlGraphicsItem *QmlGraphicsKeyNavigationAttached::down() const
{
    Q_D(const QmlGraphicsKeyNavigationAttached);
    return d->down;
}

void QmlGraphicsKeyNavigationAttached::setDown(QmlGraphicsItem *i)
{
    Q_D(QmlGraphicsKeyNavigationAttached);
    d->down = i;
    emit changed();
}

void QmlGraphicsKeyNavigationAttached::keyPressed(QKeyEvent *event)
{
    Q_D(QmlGraphicsKeyNavigationAttached);

    event->ignore();

    switch(event->key()) {
    case Qt::Key_Left:
        if (d->left) {
            d->left->setFocus(true);
            event->accept();
        }
        break;
    case Qt::Key_Right:
        if (d->right) {
            d->right->setFocus(true);
            event->accept();
        }
        break;
    case Qt::Key_Up:
        if (d->up) {
            d->up->setFocus(true);
            event->accept();
        }
        break;
    case Qt::Key_Down:
        if (d->down) {
            d->down->setFocus(true);
            event->accept();
        }
        break;
    default:
        break;
    }

    if (!event->isAccepted()) QmlGraphicsItemKeyFilter::keyPressed(event);
}

void QmlGraphicsKeyNavigationAttached::keyReleased(QKeyEvent *event)
{
    Q_D(QmlGraphicsKeyNavigationAttached);

    event->ignore();

    switch(event->key()) {
    case Qt::Key_Left:
        if (d->left) {
            event->accept();
        }
        break;
    case Qt::Key_Right:
        if (d->right) {
            event->accept();
        }
        break;
    case Qt::Key_Up:
        if (d->up) {
            event->accept();
        }
        break;
    case Qt::Key_Down:
        if (d->down) {
            event->accept();
        }
        break;
    default:
        break;
    }

    if (!event->isAccepted()) QmlGraphicsItemKeyFilter::keyReleased(event);
}

/*!
    \qmlclass Keys
    \brief The Keys attached property provides key handling to Items.

    All visual primitives support key handling via the \e Keys
    attached property.  Keys can be handled via the \e onPressed
    and \e onReleased signal properties.

    The signal properties have a \l KeyEvent parameter, named
    \e event which contains details of the event.  If a key is
    handled \e event.accepted should be set to true to prevent the
    event from propagating up the item heirarchy.

    \code
    Item {
        focus: true
        Keys.onPressed: {
            if (event.key == Qt.Key_Left) {
                console.log("move left");
                event.accepted = true;
            }
        }
    }
    \endcode

    Some keys may alternatively be handled via specific signal properties,
    for example \e onSelectPressed.  These handlers automatically set
    \e event.accepted to true.

    \code
    Item {
        focus: true
        Keys.onLeftPressed: console.log("move left")
    }
    \endcode

    See \l {Qt::Key}{Qt.Key} for the list of keyboard codes.

    \sa KeyEvent, {KeyNavigation}{KeyNavigation attached property}
*/

/*!
    \qmlproperty bool Keys::enabled

    This flags enables key handling if true (default); otherwise
    no key handlers will be called.
*/

/*!
    \qmlproperty List<Object> Keys::forwardTo

    This property provides a way to forward key presses, key releases, and keyboard input
    coming from input methods to other items. This can be useful when you want
    one item to handle some keys (e.g. the up and down arrow keys), and another item to
    handle other keys (e.g. the left and right arrow keys).  Once an item that has been
    forwarded keys accepts the event it is no longer forwarded to items later in the
    list.

    This example forwards key events to two lists:
    \qml
    ListView { id: list1 ... }
    ListView { id: list2 ... }
    Keys.forwardTo: [list1, list2]
    focus: true
    \endqml
*/

/*!
    \qmlsignal Keys::onPressed(event)

    This handler is called when a key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onReleased(event)

    This handler is called when a key has been released. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit0Pressed(event)

    This handler is called when the digit '0' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit1Pressed(event)

    This handler is called when the digit '1' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit2Pressed(event)

    This handler is called when the digit '2' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit3Pressed(event)

    This handler is called when the digit '3' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit4Pressed(event)

    This handler is called when the digit '4' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit5Pressed(event)

    This handler is called when the digit '5' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit6Pressed(event)

    This handler is called when the digit '6' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit7Pressed(event)

    This handler is called when the digit '7' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit8Pressed(event)

    This handler is called when the digit '8' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDigit9Pressed(event)

    This handler is called when the digit '9' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onLeftPressed(event)

    This handler is called when the Left arrow has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onRightPressed(event)

    This handler is called when the Right arrow has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onUpPressed(event)

    This handler is called when the Up arrow has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDownPressed(event)

    This handler is called when the Down arrow has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onAsteriskPressed(event)

    This handler is called when the Asterisk '*' has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onEscapePressed(event)

    This handler is called when the Escape key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onReturnPressed(event)

    This handler is called when the Return key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onEnterPressed(event)

    This handler is called when the Enter key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onDeletePressed(event)

    This handler is called when the Delete key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onSpacePressed(event)

    This handler is called when the Space key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onBackPressed(event)

    This handler is called when the Back key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onCancelPressed(event)

    This handler is called when the Cancel key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onSelectPressed(event)

    This handler is called when the Select key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onYesPressed(event)

    This handler is called when the Yes key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onNoPressed(event)

    This handler is called when the No key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onContext1Pressed(event)

    This handler is called when the Context1 key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onContext2Pressed(event)

    This handler is called when the Context2 key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onContext3Pressed(event)

    This handler is called when the Context3 key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onContext4Pressed(event)

    This handler is called when the Context4 key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onCallPressed(event)

    This handler is called when the Call key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onHangupPressed(event)

    This handler is called when the Hangup key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onFlipPressed(event)

    This handler is called when the Flip key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onMenuPressed(event)

    This handler is called when the Menu key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onVolumeUpPressed(event)

    This handler is called when the VolumeUp key has been pressed. The \a event
    parameter provides information about the event.
*/

/*!
    \qmlsignal Keys::onVolumeDownPressed(event)

    This handler is called when the VolumeDown key has been pressed. The \a event
    parameter provides information about the event.
*/

const QmlGraphicsKeysAttached::SigMap QmlGraphicsKeysAttached::sigMap[] = {
    { Qt::Key_Left, "leftPressed" },
    { Qt::Key_Right, "rightPressed" },
    { Qt::Key_Up, "upPressed" },
    { Qt::Key_Down, "downPressed" },
    { Qt::Key_Asterisk, "asteriskPressed" },
    { Qt::Key_NumberSign, "numberSignPressed" },
    { Qt::Key_Escape, "escapePressed" },
    { Qt::Key_Return, "returnPressed" },
    { Qt::Key_Enter, "enterPressed" },
    { Qt::Key_Delete, "deletePressed" },
    { Qt::Key_Space, "spacePressed" },
    { Qt::Key_Back, "backPressed" },
    { Qt::Key_Cancel, "cancelPressed" },
    { Qt::Key_Select, "selectPressed" },
    { Qt::Key_Yes, "yesPressed" },
    { Qt::Key_No, "noPressed" },
    { Qt::Key_Context1, "context1Pressed" },
    { Qt::Key_Context2, "context2Pressed" },
    { Qt::Key_Context3, "context3Pressed" },
    { Qt::Key_Context4, "context4Pressed" },
    { Qt::Key_Call, "callPressed" },
    { Qt::Key_Hangup, "hangupPressed" },
    { Qt::Key_Flip, "flipPressed" },
    { Qt::Key_Menu, "menuPressed" },
    { Qt::Key_VolumeUp, "volumeUpPressed" },
    { Qt::Key_VolumeDown, "volumeDownPressed" },
    { 0, 0 }
};

bool QmlGraphicsKeysAttachedPrivate::isConnected(const char *signalName)
{
    return isSignalConnected(signalIndex(signalName));
}

QmlGraphicsKeysAttached::QmlGraphicsKeysAttached(QObject *parent)
: QObject(*(new QmlGraphicsKeysAttachedPrivate), parent),
  QmlGraphicsItemKeyFilter(qobject_cast<QmlGraphicsItem*>(parent))
{
    Q_D(QmlGraphicsKeysAttached);
    d->item = qobject_cast<QmlGraphicsItem*>(parent);
}

QmlGraphicsKeysAttached::~QmlGraphicsKeysAttached()
{
}

void QmlGraphicsKeysAttached::componentComplete()
{
    Q_D(QmlGraphicsKeysAttached);
    if (d->item) {
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *targetItem = d->finalFocusProxy(d->targets.at(ii));
            if (targetItem && (targetItem->flags() & QGraphicsItem::ItemAcceptsInputMethod)) {
                d->item->setFlag(QGraphicsItem::ItemAcceptsInputMethod);
                break;
            }
        }
    }
}

void QmlGraphicsKeysAttached::keyPressed(QKeyEvent *event)
{
    Q_D(QmlGraphicsKeysAttached);
    if (!d->enabled || d->inPress) {
        event->ignore();
        return;
    }

    // first process forwards
    if (d->item && d->item->scene()) {
        d->inPress = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i) {
                d->item->scene()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->inPress = false;
                    return;
                }
            }
        }
        d->inPress = false;
    }

    QmlGraphicsKeyEvent ke(*event);
    QByteArray keySignal = keyToSignal(event->key());
    if (!keySignal.isEmpty()) {
        keySignal += "(QmlGraphicsKeyEvent*)";
        if (d->isConnected(keySignal)) {
            // If we specifically handle a key then default to accepted
            ke.setAccepted(true);
            int idx = QmlGraphicsKeysAttached::staticMetaObject.indexOfSignal(keySignal);
            metaObject()->method(idx).invoke(this, Qt::DirectConnection, Q_ARG(QmlGraphicsKeyEvent*, &ke));
        }
    }
    if (!ke.isAccepted())
        emit pressed(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QmlGraphicsItemKeyFilter::keyPressed(event);
}

void QmlGraphicsKeysAttached::keyReleased(QKeyEvent *event)
{
    Q_D(QmlGraphicsKeysAttached);
    if (!d->enabled || d->inRelease) {
        event->ignore();
        return;
    }

    if (d->item && d->item->scene()) {
        d->inRelease = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i) {
                d->item->scene()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->inRelease = false;
                    return;
                }
            }
        }
        d->inRelease = false;
    }

    QmlGraphicsKeyEvent ke(*event);
    emit released(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QmlGraphicsItemKeyFilter::keyReleased(event);
}

void QmlGraphicsKeysAttached::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QmlGraphicsKeysAttached);
    if (d->item && !d->inIM && d->item->scene()) {
        d->inIM = true;
        for (int ii = 0; ii < d->targets.count(); ++ii) {
            QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i && (i->flags() & QGraphicsItem::ItemAcceptsInputMethod)) {
                d->item->scene()->sendEvent(i, event);
                if (event->isAccepted()) {
                    d->imeItem = i;
                    d->inIM = false;
                    return;
                }
            }
        }
        d->inIM = false;
    }
    if (!event->isAccepted()) QmlGraphicsItemKeyFilter::inputMethodEvent(event);
}

class QmlGraphicsItemAccessor : public QGraphicsItem
{
public:
    QVariant doInputMethodQuery(Qt::InputMethodQuery query) const {
        return QGraphicsItem::inputMethodQuery(query);
    }
};

QVariant QmlGraphicsKeysAttached::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QmlGraphicsKeysAttached);
    if (d->item) {
        for (int ii = 0; ii < d->targets.count(); ++ii) {
                QGraphicsItem *i = d->finalFocusProxy(d->targets.at(ii));
            if (i && (i->flags() & QGraphicsItem::ItemAcceptsInputMethod) && i == d->imeItem) { //### how robust is i == d->imeItem check?
                QVariant v = static_cast<QmlGraphicsItemAccessor *>(i)->doInputMethodQuery(query);
                if (v.userType() == QVariant::RectF)
                    v = d->item->mapRectFromItem(i, v.toRectF());  //### cost?
                return v;
            }
        }
    }
    return QmlGraphicsItemKeyFilter::inputMethodQuery(query);
}

QmlGraphicsKeysAttached *QmlGraphicsKeysAttached::qmlAttachedProperties(QObject *obj)
{
    return new QmlGraphicsKeysAttached(obj);
}

/*!
    \class QmlGraphicsItem
    \brief The QmlGraphicsItem class provides the most basic of all visual items in QML.

    All visual items in Qt Declarative inherit from QmlGraphicsItem.  Although QmlGraphicsItem
    has no visual appearance, it defines all the properties that are
    common across visual items - such as the x and y position, the
    width and height, \l {anchor-layout}{anchoring} and key handling.

    You can subclass QmlGraphicsItem to provide your own custom visual item that inherits
    these features.
*/

/*!
    \qmlclass Item QmlGraphicsItem
    \brief The Item is the most basic of all visual items in QML.

    All visual items in Qt Declarative inherit from Item.  Although Item
    has no visual appearance, it defines all the properties that are
    common across visual items - such as the x and y position, the
    width and height, \l {anchor-layout}{anchoring} and key handling.

    Item is also useful for grouping items together.

    \qml
    Item {
        Image {
            source: "tile.png"
        }
        Image {
            x: 80
            width: 100
            height: 100
            source: "tile.png"
        }
        Image {
            x: 190
            width: 100
            height: 100
            fillMode: Image.Tile
            source: "tile.png"
        }
    }
    \endqml

    \section1 Identity

    Each item has an "id" - the identifier of the Item.

    The identifier can be used in bindings and other expressions to
    refer to the item. For example:

    \qml
    Text { id: myText; ... }
    Text { text: myText.text }
    \endqml

    The identifier is available throughout to the \l {components}{component}
    where it is declared.  The identifier must be unique in the component.

    The id should not be thought of as a "property" - it makes no sense
    to write \c myText.id, for example.

    \section1 Key Handling

    Key handling is available to all Item-based visual elements via the \l {Keys}{Keys}
    attached property.  The \e Keys attached property provides basic handlers such
    as \l {Keys::onPressed}{onPressed} and \l {Keys::onReleased}{onReleased},
    as well as handlers for specific keys, such as
    \l {Keys::onCancelPressed}{onCancelPressed}.  The example below
    assigns \l {qmlfocus}{focus} to the item and handles
    the Left key via the general \e onPressed handler and the Select key via the
    onSelectPressed handler:

    \qml
    Item {
        focus: true
        Keys.onPressed: {
            if (event.key == Qt.Key_Left) {
                console.log("move left");
                event.accepted = true;
            }
        }
        Keys.onSelectPressed: console.log("Selected");
    }
    \endqml

    See the \l {Keys}{Keys} attached property for detailed documentation.

    \ingroup group_coreitems
*/

/*!
    \property QmlGraphicsItem::baseline
    \internal
*/

/*!
    \property QmlGraphicsItem::effect
    \internal
*/

/*!
    \property QmlGraphicsItem::focus
    \internal
*/

/*!
    \property QmlGraphicsItem::wantsFocus
    \internal
*/

/*!
    \property QmlGraphicsItem::transformOrigin
    \internal
*/

/*!
    \fn void QmlGraphicsItem::childrenRectChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::baselineOffsetChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::widthChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::heightChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::stateChanged(const QString &state)
    \internal
*/

/*!
    \fn void QmlGraphicsItem::parentChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::childrenChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::focusChanged()
    \internal
*/

/*!
    \fn void QmlGraphicsItem::wantsFocusChanged()
    \internal
*/

// ### Must fix
struct RegisterAnchorLineAtStartup {
    RegisterAnchorLineAtStartup() {
        qRegisterMetaType<QmlGraphicsAnchorLine>("QmlGraphicsAnchorLine");
    }
};
static RegisterAnchorLineAtStartup registerAnchorLineAtStartup;


/*!
    \fn QmlGraphicsItem::QmlGraphicsItem(QmlGraphicsItem *parent)

    Constructs a QmlGraphicsItem with the given \a parent.
*/
QmlGraphicsItem::QmlGraphicsItem(QmlGraphicsItem* parent)
  : QGraphicsObject(*(new QmlGraphicsItemPrivate), parent, 0)
{
    Q_D(QmlGraphicsItem);
    d->init(parent);
}

/*! \internal
*/
QmlGraphicsItem::QmlGraphicsItem(QmlGraphicsItemPrivate &dd, QmlGraphicsItem *parent)
  : QGraphicsObject(dd, parent, 0)
{
    Q_D(QmlGraphicsItem);
    d->init(parent);
}

/*!
    Destroys the QmlGraphicsItem.
*/
QmlGraphicsItem::~QmlGraphicsItem()
{
    Q_D(QmlGraphicsItem);
    for (int ii = 0; ii < d->changeListeners.count(); ++ii) {
        QmlGraphicsAnchorsPrivate *anchor = d->changeListeners.at(ii).listener->anchorPrivate();
        if (anchor)
            anchor->clearItem(this);
    }
    if (!d->parent || (parentItem() && !parentItem()->QGraphicsItem::d_ptr->inDestructor)) {
        for (int ii = 0; ii < d->changeListeners.count(); ++ii) {
            QmlGraphicsAnchorsPrivate *anchor = d->changeListeners.at(ii).listener->anchorPrivate();
            if (anchor && anchor->item && anchor->item->parentItem() != this) //child will be deleted anyway
                anchor->updateOnComplete();
        }
    }
    for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QmlGraphicsItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QmlGraphicsItemPrivate::Destroyed)
            change.listener->itemDestroyed(this);
    }
    d->changeListeners.clear();
    delete d->_anchorLines; d->_anchorLines = 0;
    delete d->_anchors; d->_anchors = 0;
    delete d->_stateGroup; d->_stateGroup = 0;
}

/*!
    \qmlproperty enum Item::transformOrigin
    This property holds the origin point around which scale and rotation transform.

    Nine transform origins are available, as shown in the image below.

    \image declarative-transformorigin.png

    This example rotates an image around its bottom-right corner.
    \qml
    Image {
        source: "myimage.png"
        transformOrigin: Item.BottomRight
        rotate: 45
    }
    \endqml

    The default transform origin is \c Center.
*/

/*!
    \qmlproperty Item Item::parent
    This property holds the parent of the item.
*/

/*!
    \property QmlGraphicsItem::parent
    This property holds the parent of the item.
*/
void QmlGraphicsItem::setParentItem(QmlGraphicsItem *parent)
{
    QmlGraphicsItem *oldParent = parentItem();
    if (parent == oldParent || !parent) return;

    Q_D(QmlGraphicsItem);
    QObject::setParent(parent);
    d->setParentItemHelper(parent, /*newParentVariant=*/0, /*thisPointerVariant=*/0);
    if (oldParent)
        emit oldParent->childrenChanged();
    emit parentChanged();
}

/*!
    \fn void QmlGraphicsItem::setParent(QmlGraphicsItem *parent)
    \overload
    Sets both the parent object and parent item to \a parent. This
    function avoids the programming error of calling setParent()
    when you mean setParentItem().
*/

/*!
    Returns the QmlGraphicsItem parent of this item.
*/
QmlGraphicsItem *QmlGraphicsItem::parentItem() const
{
    return qobject_cast<QmlGraphicsItem *>(QGraphicsObject::parentItem());
}

/*!
    \qmlproperty list<Item> Item::children
    \qmlproperty list<Object> Item::resources

    The children property contains the list of visual children of this item.
    The resources property contains non-visual resources that you want to
    reference by name.

    Generally you can rely on Item's default property to handle all this for
    you, but it can come in handy in some cases.

    \qml
    Item {
        children: [
            Text {},
            Rectangle {}
        ]
        resources: [
            Component {
                id: myComponent
                Text {}
            }
        ]
    }
    \endqml
*/

/*!
    \property QmlGraphicsItem::children
    \internal
*/

/*!
    \property QmlGraphicsItem::resources
    \internal
*/

/*!
    Returns true if construction of the QML component is complete; otherwise
    returns false.

    It is often desireable to delay some processing until the component is
    completed.

    \sa componentComplete()
*/
bool QmlGraphicsItem::isComponentComplete() const
{
    Q_D(const QmlGraphicsItem);
    return d->_componentComplete;
}

/*!
    \property QmlGraphicsItem::anchors
    \internal
*/

/*! \internal */
QmlGraphicsAnchors *QmlGraphicsItem::anchors()
{
    Q_D(QmlGraphicsItem);
    return d->anchors();
}

void QmlGraphicsItemPrivate::data_removeAt(int)
{
    // ###
}

int QmlGraphicsItemPrivate::data_count() const
{
    // ###
    return 0;
}

void QmlGraphicsItemPrivate::data_append(QObject *o)
{
    Q_Q(QmlGraphicsItem);
    QmlGraphicsItem *i = qobject_cast<QmlGraphicsItem *>(o);
    if (i)
        q->fxChildren()->append(i);
    else
        resources_append(o);
}

void QmlGraphicsItemPrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QmlGraphicsItemPrivate::data_at(int) const
{
    // ###
    return 0;
}

void QmlGraphicsItemPrivate::data_clear()
{
    // ###
}

void QmlGraphicsItemPrivate::resources_removeAt(int)
{
    // ###
}

int QmlGraphicsItemPrivate::resources_count() const
{
    Q_Q(const QmlGraphicsItem);
    return q->children().count();
}

void QmlGraphicsItemPrivate::resources_append(QObject *o)
{
    Q_Q(QmlGraphicsItem);
    o->setParent(q);
}

void QmlGraphicsItemPrivate::resources_insert(int, QObject *)
{
    // ###
}

QObject *QmlGraphicsItemPrivate::resources_at(int idx) const
{
    Q_Q(const QmlGraphicsItem);
    QObjectList children = q->children();
    if (idx < children.count())
        return children.at(idx);
    else
        return 0;
}

void QmlGraphicsItemPrivate::resources_clear()
{
    // ###
}

void QmlGraphicsItemPrivate::children_removeAt(int)
{
    // ###
}

int QmlGraphicsItemPrivate::children_count() const
{
    Q_Q(const QmlGraphicsItem);
    return q->childItems().count();
}

void QmlGraphicsItemPrivate::children_append(QmlGraphicsItem *i)
{
    Q_Q(QmlGraphicsItem);
    i->setParentItem(q);
}

void QmlGraphicsItemPrivate::children_insert(int, QmlGraphicsItem *)
{
    // ###
}

QmlGraphicsItem *QmlGraphicsItemPrivate::children_at(int idx) const
{
    Q_Q(const QmlGraphicsItem);
    QList<QGraphicsItem *> children = q->childItems();
    if (idx < children.count())
        return qobject_cast<QmlGraphicsItem *>(children.at(idx));
    else
        return 0;
}

void QmlGraphicsItemPrivate::children_clear()
{
    // ###
}


void QmlGraphicsItemPrivate::transform_removeAt(int i)
{
    if (!transformData)
        return;
    transformData->graphicsTransforms.removeAt(i);
    dirtySceneTransform = 1;
}

int QmlGraphicsItemPrivate::transform_count() const
{
    return transformData ? transformData->graphicsTransforms.size() : 0;
}

void QmlGraphicsItemPrivate::transform_append(QGraphicsTransform *item)
{
    appendGraphicsTransform(item);
}

void QmlGraphicsItemPrivate::transform_insert(int, QGraphicsTransform *)
{
    // ###
}

QGraphicsTransform *QmlGraphicsItemPrivate::transform_at(int idx) const
{
    if (!transformData)
        return 0;
    return transformData->graphicsTransforms.at(idx);
}

void QmlGraphicsItemPrivate::transform_clear()
{
    if (!transformData)
        return;
    Q_Q(QmlGraphicsItem);
    q->setTransformations(QList<QGraphicsTransform *>());
}

/*!
    \qmlproperty list<Object> Item::data
    \default

    The data property is allows you to freely mix visual children and resources
    of an item.  If you assign a visual item to the data list it becomes
    a child and if you assign any other object type, it is added as a resource.

    So you can write:
    \qml
    Item {
        Text {}
        Rectangle {}
        Script {}
    }
    \endqml

    instead of:
    \qml
    Item {
        children: [
            Text {},
            Rectangle {}
        ]
        resources: [
            Script {}
        ]
    }
    \endqml

    data is a behind-the-scenes property: you should never need to explicitly
    specify it.
 */

/*!
    \property QmlGraphicsItem::data
    \internal
*/

/*! \internal */
QmlList<QObject *> *QmlGraphicsItem::data()
{
    Q_D(QmlGraphicsItem);
    return &d->data;
}

/*!
    \property QmlGraphicsItem::childrenRect
    \brief The geometry of an item's children.

    childrenRect provides an easy way to access the (collective) position and size of the item's children.
*/
QRectF QmlGraphicsItem::childrenRect()
{
    Q_D(QmlGraphicsItem);
    if (!d->_contents) {
        d->_contents = new QmlGraphicsContents;
        d->_contents->setParent(this);
        d->_contents->setItem(this);
    }
    return d->_contents->rectF();
}

bool QmlGraphicsItem::clip() const
{
    return flags() & ItemClipsChildrenToShape;
}

void QmlGraphicsItem::setClip(bool c)
{
    setFlag(ItemClipsChildrenToShape, c);
}

/*!
  \qmlproperty real Item::x
  \qmlproperty real Item::y
  \qmlproperty real Item::width
  \qmlproperty real Item::height

  Defines the item's position and size relative to its parent.

  \qml
  Item { x: 100; y: 100; width: 100; height: 100 }
  \endqml
 */

/*!
  \property QmlGraphicsItem::width

  Defines the item's width relative to its parent.
 */

/*!
  \property QmlGraphicsItem::height

  Defines the item's height relative to its parent.
 */

/*!
  \qmlproperty real Item::z

  Sets the stacking order of the item.  By default the stacking order is 0.

  Items with a higher stacking value are drawn on top of items with a
  lower stacking order.  Items with the same stacking value are drawn
  bottom up in the order they appear.  Items with a negative stacking
  value are drawn under their parent's content.

  The following example shows the various effects of stacking order.

  \table
  \row
  \o \image declarative-item_stacking1.png
  \o Same \c z - later children above earlier children:
  \qml
  Item {
      Rectangle {
          color: "red"
          width: 100; height: 100
      }
      Rectangle {
          color: "blue"
          x: 50; y: 50; width: 100; height: 100
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking2.png
  \o Higher \c z on top:
  \qml
  Item {
      Rectangle {
          z: 1
          color: "red"
          width: 100; height: 100
      }
      Rectangle {
          color: "blue"
          x: 50; y: 50; width: 100; height: 100
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking3.png
  \o Same \c z - children above parents:
  \qml
  Item {
      Rectangle {
          color: "red"
          width: 100; height: 100
          Rectangle {
              color: "blue"
              x: 50; y: 50; width: 100; height: 100
          }
      }
  }
  \endqml
  \row
  \o \image declarative-item_stacking4.png
  \o Lower \c z below:
  \qml
  Item {
      Rectangle {
          color: "red"
          width: 100; height: 100
          Rectangle {
              z: -1
              color: "blue"
              x: 50; y: 50; width: 100; height: 100
          }
      }
  }
  \endqml
  \endtable
 */

/*!
    \qmlproperty bool Item::visible

    Whether the item is visible. By default this is true.

    \note visible is not linked to actual visibility; if an item
    moves off screen, or the opacity changes to 0, this will
    not affect the visible property.
*/


/*!
  This function is called to handle this item's changes in
  geometry from \a oldGeometry to \a newGeometry. If the two
  geometries are the same, it doesn't do anything.
 */
void QmlGraphicsItem::geometryChanged(const QRectF &newGeometry,
                              const QRectF &oldGeometry)
{
    Q_D(QmlGraphicsItem);

    if (d->_anchors)
        d->_anchors->d_func()->updateMe();

    if (transformOrigin() != QmlGraphicsItem::TopLeft)
        setTransformOriginPoint(d->computeTransformOrigin());

    if (newGeometry.x() != oldGeometry.x())
        emit xChanged();
    if (newGeometry.width() != oldGeometry.width())
        emit widthChanged();
    if (newGeometry.y() != oldGeometry.y())
        emit yChanged();
    if (newGeometry.height() != oldGeometry.height())
        emit heightChanged();

    for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QmlGraphicsItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QmlGraphicsItemPrivate::Geometry)
            change.listener->itemGeometryChanged(this, newGeometry, oldGeometry);
    }
}

void QmlGraphicsItemPrivate::removeItemChangeListener(QmlGraphicsItemChangeListener *listener, ChangeTypes types)
{
    ChangeListener change(listener, types);
    changeListeners.removeOne(change);
}

/*! \internal */
void QmlGraphicsItem::keyPressEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsItem);
    if (d->keyHandler)
        d->keyHandler->keyPressed(event);
    else
        event->ignore();
}

/*! \internal */
void QmlGraphicsItem::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsItem);
    if (d->keyHandler)
        d->keyHandler->keyReleased(event);
    else
        event->ignore();
}

/*! \internal */
void QmlGraphicsItem::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QmlGraphicsItem);
    if (d->keyHandler)
        d->keyHandler->inputMethodEvent(event);
    else
        event->ignore();
}

/*! \internal */
QVariant QmlGraphicsItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    Q_D(const QmlGraphicsItem);
    QVariant v;
    if (d->keyHandler)
        v = d->keyHandler->inputMethodQuery(query);

    if (!v.isValid())
        v = QGraphicsObject::inputMethodQuery(query);

    return v;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::left() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->left;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::right() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->right;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::horizontalCenter() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->hCenter;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::top() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->top;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::bottom() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->bottom;
}

/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::verticalCenter() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->vCenter;
}


/*!
    \internal
*/
QmlGraphicsAnchorLine QmlGraphicsItem::baseline() const
{
    Q_D(const QmlGraphicsItem);
    return d->anchorLines()->baseline;
}

/*!
  \property QmlGraphicsItem::top
  \internal
*/

/*!
  \property QmlGraphicsItem::bottom
  \internal
*/

/*!
  \property QmlGraphicsItem::left
  \internal
*/

/*!
  \property QmlGraphicsItem::right
  \internal
*/

/*!
  \property QmlGraphicsItem::horizontalCenter
  \internal
*/

/*!
  \property QmlGraphicsItem::verticalCenter
  \internal
*/

/*!
  \qmlproperty AnchorLine Item::top
  \qmlproperty AnchorLine Item::bottom
  \qmlproperty AnchorLine Item::left
  \qmlproperty AnchorLine Item::right
  \qmlproperty AnchorLine Item::horizontalCenter
  \qmlproperty AnchorLine Item::verticalCenter
  \qmlproperty AnchorLine Item::baseline

  The anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \qmlproperty AnchorLine Item::anchors.top
  \qmlproperty AnchorLine Item::anchors.bottom
  \qmlproperty AnchorLine Item::anchors.left
  \qmlproperty AnchorLine Item::anchors.right
  \qmlproperty AnchorLine Item::anchors.horizontalCenter
  \qmlproperty AnchorLine Item::anchors.verticalCenter
  \qmlproperty AnchorLine Item::anchors.baseline

  \qmlproperty Item Item::anchors.fill
  \qmlproperty Item Item::anchors.centerIn

  \qmlproperty real Item::anchors.margins
  \qmlproperty real Item::anchors.topMargin
  \qmlproperty real Item::anchors.bottomMargin
  \qmlproperty real Item::anchors.leftMargin
  \qmlproperty real Item::anchors.rightMargin
  \qmlproperty real Item::anchors.horizontalCenterOffset
  \qmlproperty real Item::anchors.verticalCenterOffset
  \qmlproperty real Item::anchors.baselineOffset

  Anchors provide a way to position an item by specifying its
  relationship with other items.

  Margins apply to top, bottom, left, right, and fill anchors.
  The margins property can be used to set all of the various margins at once, to the same value.

  Offsets apply for horizontal center, vertical center, and baseline anchors.

  \table
  \row
  \o \image declarative-anchors_example.png
  \o Text anchored to Image, horizontally centered and vertically below, with a margin.
  \qml
  Image { id: pic; ... }
  Text {
      id: label
      anchors.horizontalCenter: pic.horizontalCenter
      anchors.top: pic.bottom
      anchors.topMargin: 5
      ...
  }
  \endqml
  \row
  \o \image declarative-anchors_example2.png
  \o
  Left of Text anchored to right of Image, with a margin. The y
  property of both defaults to 0.

  \qml
    Image { id: pic; ... }
    Text {
        id: label
        anchors.left: pic.right
        anchors.leftMargin: 5
        ...
    }
  \endqml
  \endtable

  anchors.fill provides a convenient way for one item to have the
  same geometry as another item, and is equivalent to connecting all
  four directional anchors.

  \note You can only anchor an item to siblings or a parent.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QmlGraphicsItem::baselineOffset
  \brief The position of the item's baseline in local coordinates.

  The baseline of a Text item is the imaginary line on which the text
  sits. Controls containing text usually set their baseline to the
  baseline of their text.

  For non-text items, a default baseline offset of 0 is used.
*/
qreal QmlGraphicsItem::baselineOffset() const
{
    Q_D(const QmlGraphicsItem);
    if (!d->_baselineOffset.isValid()) {
        return 0.0;
    } else
        return d->_baselineOffset;
}

void QmlGraphicsItem::setBaselineOffset(qreal offset)
{
    Q_D(QmlGraphicsItem);
    if (offset == d->_baselineOffset)
        return;

    d->_baselineOffset = offset;
    emit baselineOffsetChanged();

    for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
        const QmlGraphicsItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
        if (change.types & QmlGraphicsItemPrivate::Geometry) {
            QmlGraphicsAnchorsPrivate *anchor = change.listener->anchorPrivate();
            if (anchor)
                anchor->updateVerticalAnchors();
        }
    }
}

/*!
  \qmlproperty real Item::rotation
  This property holds the rotation of the item in degrees clockwise.

  This specifies how many degrees to rotate the item around its transformOrigin.
  The default rotation is 0 degrees (i.e. not rotated at all).

  \table
  \row
  \o \image declarative-rotation.png
  \o
  \qml
  Rectangle {
      color: "blue"
      width: 100; height: 100
      Rectangle {
          color: "red"
          x: 25; y: 25; width: 50; height: 50
          rotation: 30
      }
  }
  \endqml
  \endtable
*/

/*!
  \qmlproperty real Item::scale
  This property holds the scale of the item.

  A scale of less than 1 means the item will be displayed smaller than
  normal, and a scale of greater than 1 means the item will be
  displayed larger than normal.  A negative scale means the item will
  be mirrored.

  By default, items are displayed at a scale of 1 (i.e. at their
  normal size).

  Scaling is from the item's transformOrigin.

  \table
  \row
  \o \image declarative-scale.png
  \o
  \qml
  Rectangle {
      color: "blue"
      width: 100; height: 100
      Rectangle {
          color: "green"
          width: 25; height: 25
      }
      Rectangle {
          color: "red"
          x: 25; y: 25; width: 50; height: 50
          scale: 1.4
      }
  }
  \endqml
  \endtable
*/

/*!
  \qmlproperty real Item::opacity

  The opacity of the item.  Opacity is specified as a number between 0
  (fully transparent) and 1 (fully opaque).  The default is 1.

  Opacity is an \e inherited attribute.  That is, the opacity is
  also applied individually to child items.  In almost all cases this
  is what you want.  If you can spot the issue in the following
  example, you might need to use an \l Opacity effect instead.

  \table
  \row
  \o \image declarative-item_opacity1.png
  \o
  \qml
    Item {
        Rectangle {
            color: "red"
            width: 100; height: 100
            Rectangle {
                color: "blue"
                x: 50; y: 50; width: 100; height: 100
            }
        }
    }
  \endqml
  \row
  \o \image declarative-item_opacity2.png
  \o
  \qml
    Item {
        Rectangle {
            opacity: 0.5
            color: "red"
            width: 100; height: 100
            Rectangle {
                color: "blue"
                x: 50; y: 50; width: 100; height: 100
            }
        }
    }
  \endqml
  \endtable
*/

/*!
  Returns a value indicating whether mouse input should
  remain with this item exclusively.

  \sa setKeepMouseGrab()
 */
bool QmlGraphicsItem::keepMouseGrab() const
{
    Q_D(const QmlGraphicsItem);
    return d->_keepMouse;
}

/*!
  The flag indicating whether the mouse should remain
  with this item is set to \a keep.

  This is useful for items that wish to grab and keep mouse
  interaction following a predefined gesture.  For example,
  an item that is interested in horizontal mouse movement
  may set keepMouseGrab to true once a threshold has been
  exceeded.  Once keepMouseGrab has been set to true, filtering
  items will not react to mouse events.

  If the item does not indicate that it wishes to retain mouse grab,
  a filtering item may steal the grab. For example, Flickable may attempt
  to steal a mouse grab if it detects that the user has begun to
  move the viewport.

  \sa keepMouseGrab()
 */
void QmlGraphicsItem::setKeepMouseGrab(bool keep)
{
    Q_D(QmlGraphicsItem);
    d->_keepMouse = keep;
}

/*!
  \internal

  This function emits the \e focusChanged signal.

  Subclasses overriding this function should call up
  to their base class.
*/
void QmlGraphicsItem::focusChanged(bool flag)
{
    Q_UNUSED(flag);
    emit focusChanged();
}

/*! \internal */
QmlList<QmlGraphicsItem *> *QmlGraphicsItem::fxChildren()
{
    Q_D(QmlGraphicsItem);
    return &(d->children);
}

/*! \internal */
QmlList<QObject *> *QmlGraphicsItem::resources()
{
    Q_D(QmlGraphicsItem);
    return &(d->resources);
}

/*!
  \qmlproperty list<State> Item::states
  This property holds a list of states defined by the item.

  \qml
  Item {
    states: [
      State { ... },
      State { ... }
      ...
    ]
  }
  \endqml

  \sa {qmlstate}{States}
*/

/*!
  \property QmlGraphicsItem::states
  \internal
*/
/*! \internal */
QmlList<QmlState *>* QmlGraphicsItem::states()
{
    Q_D(QmlGraphicsItem);
    return d->states()->statesProperty();
}

/*!
  \qmlproperty list<Transition> Item::transitions
  This property holds a list of transitions defined by the item.

  \qml
  Item {
    transitions: [
      Transition { ... },
      Transition { ... }
      ...
    ]
  }
  \endqml

  \sa {state-transitions}{Transitions}
*/

/*!
  \property QmlGraphicsItem::transitions
  \internal
*/

/*! \internal */
QmlList<QmlTransition *>* QmlGraphicsItem::transitions()
{
    Q_D(QmlGraphicsItem);
    return d->states()->transitionsProperty();
}

/*
  \qmlproperty list<Filter> Item::filter
  This property holds a list of graphical filters to be applied to the item.

  \l {Filter}{Filters} include things like \l {Blur}{blurring}
  the item, or giving it a \l Reflection.  Some
  filters may not be available on all canvases; if a filter is not
  available on a certain canvas, it will simply not be applied for
  that canvas (but the QML will still be considered valid).

  \qml
  Item {
    filter: [
      Blur { ... },
      Relection { ... }
      ...
    ]
  }
  \endqml
*/

/*!
  \qmlproperty bool Item::clip
  This property holds whether clipping is enabled.

  if clipping is enabled, an item will clip its own painting, as well
  as the painting of its children, to its bounding rectangle.

  Non-rectangular clipping regions are not supported for performance reasons.
*/

/*!
  \property QmlGraphicsItem::clip
  This property holds whether clipping is enabled.

  if clipping is enabled, an item will clip its own painting, as well
  as the painting of its children, to its bounding rectangle.

  Non-rectangular clipping regions are not supported for performance reasons.
*/

/*!
  \qmlproperty string Item::state

  This property holds the name of the current state of the item.

  This property is often used in scripts to change between states. For
  example:

  \qml
    Script {
        function toggle() {
            if (button.state == 'On')
                button.state = 'Off';
            else
                button.state = 'On';
        }
    }
  \endqml

  If the item is in its base state (i.e. no explicit state has been
  set), \c state will be a blank string. Likewise, you can return an
  item to its base state by setting its current state to \c ''.

  \sa {qmlstates}{States}
*/

/*!
  \property QmlGraphicsItem::state
  \internal
*/

/*! \internal */
QString QmlGraphicsItem::state() const
{
    Q_D(const QmlGraphicsItem);
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

/*! \internal */
void QmlGraphicsItem::setState(const QString &state)
{
    Q_D(QmlGraphicsItem);
    d->states()->setState(state);
}

/*!
  \qmlproperty list<Transform> Item::transform
  This property holds the list of transformations to apply.

  For more information see \l Transform.
*/

/*!
  \property QmlGraphicsItem::transform
  \internal
*/

/*! \internal */
QmlList<QGraphicsTransform *>* QmlGraphicsItem::transform()
{
    Q_D(QmlGraphicsItem);
    return &(d->transform);
}

/*!
  \internal

  classBegin() is called when the item is constructed, but its
  properties have not yet been set.

  \sa componentComplete(), isComponentComplete()
*/
void QmlGraphicsItem::classBegin()
{
    Q_D(QmlGraphicsItem);
    d->_componentComplete = false;
    if (d->_stateGroup)
        d->_stateGroup->classBegin();
    if (d->_anchors)
        d->_anchors->classBegin();
}

/*!
  \internal

  componentComplete() is called when all items in the component
  have been constructed.  It is often desireable to delay some
  processing until the component is complete an all bindings in the
  component have been resolved.
*/
void QmlGraphicsItem::componentComplete()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::ItemComponentComplete> cc;
#endif

    Q_D(QmlGraphicsItem);
    d->_componentComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->componentComplete();
    if (d->_anchors) {
        d->_anchors->componentComplete();
        d->_anchors->d_func()->updateOnComplete();
    }
    if (d->keyHandler)
        d->keyHandler->componentComplete();
}

QmlStateGroup *QmlGraphicsItemPrivate::states()
{
    Q_Q(QmlGraphicsItem);
    if (!_stateGroup) {
        _stateGroup = new QmlStateGroup;
        if (!_componentComplete)
            _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         q, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}

QmlGraphicsItemPrivate::AnchorLines::AnchorLines(QmlGraphicsItem *q)
{
    left.item = q;
    left.anchorLine = QmlGraphicsAnchorLine::Left;
    right.item = q;
    right.anchorLine = QmlGraphicsAnchorLine::Right;
    hCenter.item = q;
    hCenter.anchorLine = QmlGraphicsAnchorLine::HCenter;
    top.item = q;
    top.anchorLine = QmlGraphicsAnchorLine::Top;
    bottom.item = q;
    bottom.anchorLine = QmlGraphicsAnchorLine::Bottom;
    vCenter.item = q;
    vCenter.anchorLine = QmlGraphicsAnchorLine::VCenter;
    baseline.item = q;
    baseline.anchorLine = QmlGraphicsAnchorLine::Baseline;
}

QPointF QmlGraphicsItemPrivate::computeTransformOrigin() const
{
    Q_Q(const QmlGraphicsItem);

    QRectF br = q->boundingRect();

    switch(origin) {
    default:
    case QmlGraphicsItem::TopLeft:
        return QPointF(0, 0);
    case QmlGraphicsItem::Top:
        return QPointF(br.width() / 2., 0);
    case QmlGraphicsItem::TopRight:
        return QPointF(br.width(), 0);
    case QmlGraphicsItem::Left:
        return QPointF(0, br.height() / 2.);
    case QmlGraphicsItem::Center:
        return QPointF(br.width() / 2., br.height() / 2.);
    case QmlGraphicsItem::Right:
        return QPointF(br.width(), br.height() / 2.);
    case QmlGraphicsItem::BottomLeft:
        return QPointF(0, br.height());
    case QmlGraphicsItem::Bottom:
        return QPointF(br.width() / 2., br.height());
    case QmlGraphicsItem::BottomRight:
        return QPointF(br.width(), br.height());
    }
}

/*! \internal */
bool QmlGraphicsItem::sceneEvent(QEvent *event)
{
    bool rv = QGraphicsItem::sceneEvent(event);

    if (event->type() == QEvent::FocusIn ||
        event->type() == QEvent::FocusOut) {
        focusChanged(hasFocus());
    }

    return rv;
}

/*! \internal */
QVariant QmlGraphicsItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    Q_D(const QmlGraphicsItem);
    switch (change) {
    case ItemParentHasChanged:
        emit parentChanged();
        break;
    case ItemChildAddedChange:
    case ItemChildRemovedChange:
        emit childrenChanged();
        break;
    case ItemVisibleHasChanged: {
            for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
                const QmlGraphicsItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
                if (change.types & QmlGraphicsItemPrivate::Visibility) {
                    change.listener->itemVisibilityChanged(this);
                }
            }
        }
        break;
    case ItemOpacityHasChanged: {
            for(int ii = 0; ii < d->changeListeners.count(); ++ii) {
                const QmlGraphicsItemPrivate::ChangeListener &change = d->changeListeners.at(ii);
                if (change.types & QmlGraphicsItemPrivate::Opacity) {
                    change.listener->itemOpacityChanged(this);
                }
            }
        }
        break;
    default:
        break;
    }

    return QGraphicsItem::itemChange(change, value);
}

/*! \internal */
QRectF QmlGraphicsItem::boundingRect() const
{
    Q_D(const QmlGraphicsItem);
    return QRectF(0, 0, d->width, d->height);
}

/*!
    \enum QmlGraphicsItem::TransformOrigin

    Controls the point about which simple transforms like scale apply.

    \value TopLeft The top-left corner of the item.
    \value Top The center point of the top of the item.
    \value TopRight The top-right corner of the item.
    \value Left The left most point of the vertical middle.
    \value Center The center of the item.
    \value Right The right most point of the vertical middle.
    \value BottomLeft The bottom-left corner of the item.
    \value Bottom The center point of the bottom of the item.
    \value BottomRight The bottom-right corner of the item.
*/

/*!
    Returns the current transform origin.
*/
QmlGraphicsItem::TransformOrigin QmlGraphicsItem::transformOrigin() const
{
    Q_D(const QmlGraphicsItem);
    return d->origin;
}

/*!
    Set the transform \a origin.
*/
void QmlGraphicsItem::setTransformOrigin(TransformOrigin origin)
{
    Q_D(QmlGraphicsItem);
    if (origin != d->origin) {
        d->origin = origin;
        QGraphicsItem::setTransformOriginPoint(d->computeTransformOrigin());
        emit transformOriginChanged(d->origin);
    }
}

/*!
    \property QmlGraphicsItem::smooth
    \brief whether the item is smoothly transformed.

    This property is provided purely for the purpose of optimization. Turning
    smooth transforms off is faster, but looks worse; turning smooth
    transformations on is slower, but looks better.

    By default smooth transformations are off.
*/

/*!
    Returns true if the item should be drawn with antialiasing and
    smooth pixmap filtering, false otherwise.

    The default is false.

    \sa setSmooth()
*/
bool QmlGraphicsItem::smooth() const
{
    Q_D(const QmlGraphicsItem);
    return d->smooth;
}

/*!
    Sets whether the item should be drawn with antialiasing and
    smooth pixmap filtering to \a smooth.

    \sa smooth()
*/
void QmlGraphicsItem::setSmooth(bool smooth)
{
    Q_D(QmlGraphicsItem);
    if (d->smooth == smooth)
        return;
    d->smooth = smooth;
    update();
}

qreal QmlGraphicsItem::width() const
{
    Q_D(const QmlGraphicsItem);
    return d->width;
}

void QmlGraphicsItem::setWidth(qreal w)
{
    Q_D(QmlGraphicsItem);
    if (qIsNaN(w))
        return;

    d->widthValid = true;
    if (d->width == w)
        return;

    qreal oldWidth = d->width;

    prepareGeometryChange();
    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, height()));
}

void QmlGraphicsItem::resetWidth()
{
    Q_D(QmlGraphicsItem);
    d->widthValid = false;
    setImplicitWidth(implicitWidth());
}

/*!
    Returns the width of the item that is implied by other properties that determine the content.
*/
qreal QmlGraphicsItem::implicitWidth() const
{
    Q_D(const QmlGraphicsItem);
    return d->implicitWidth;
}

/*!
    Sets the implied width of the item to \a w.
    This is the width implied by other properties that determine the content.
*/
void QmlGraphicsItem::setImplicitWidth(qreal w)
{
    Q_D(QmlGraphicsItem);
    d->implicitWidth = w;
    if (d->width == w || widthValid())
        return;

    qreal oldWidth = d->width;

    prepareGeometryChange();
    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, height()));
}

/*!
    Returns whether the width property has been set explicitly.
*/
bool QmlGraphicsItem::widthValid() const
{
    Q_D(const QmlGraphicsItem);
    return d->widthValid;
}

qreal QmlGraphicsItem::height() const
{
    Q_D(const QmlGraphicsItem);
    return d->height;
}

void QmlGraphicsItem::setHeight(qreal h)
{
    Q_D(QmlGraphicsItem);
    if (qIsNaN(h))
        return;

    d->heightValid = true;
    if (d->height == h)
        return;

    qreal oldHeight = d->height;

    prepareGeometryChange();
    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), width(), oldHeight));
}

void QmlGraphicsItem::resetHeight()
{
    Q_D(QmlGraphicsItem);
    d->heightValid = false;
    setImplicitHeight(implicitHeight());
}

/*!
    Returns the height of the item that is implied by other properties that determine the content.
*/
qreal QmlGraphicsItem::implicitHeight() const
{
    Q_D(const QmlGraphicsItem);
    return d->implicitHeight;
}

/*!
    Sets the implied height of the item to \a h.
    This is the height implied by other properties that determine the content.
*/
void QmlGraphicsItem::setImplicitHeight(qreal h)
{
    Q_D(QmlGraphicsItem);
    d->implicitHeight = h;
    if (d->height == h || heightValid())
        return;

    qreal oldHeight = d->height;

    prepareGeometryChange();
    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), width(), oldHeight));
}

/*!
    Returns whether the height property has been set explicitly.
*/
bool QmlGraphicsItem::heightValid() const
{
    Q_D(const QmlGraphicsItem);
    return d->heightValid;
}

/*!
  \qmlproperty bool Item::wantsFocus

  This property indicates whether the item has has an active focus request.

  \sa {qmlfocus}{Keyboard Focus}
*/

/*! \internal */
bool QmlGraphicsItem::wantsFocus() const
{
    return focusItem() != 0;
}

/*!
  \qmlproperty bool Item::focus
  This property indicates whether the item has keyboard input focus. Set this
  property to true to request focus.

  \sa {qmlfocus}{Keyboard Focus}
*/

/*! \internal */
bool QmlGraphicsItem::hasFocus() const
{
    return QGraphicsItem::hasFocus();
}

/*! \internal */
void QmlGraphicsItem::setFocus(bool focus)
{
    if (focus)
        QGraphicsItem::setFocus(Qt::OtherFocusReason);
    else
        QGraphicsItem::clearFocus();
}

/*!
    \reimp
    \internal
*/
void QmlGraphicsItem::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

/*!
    \reimp
    \internal
*/
bool QmlGraphicsItem::event(QEvent *ev)
{
    return QGraphicsObject::event(ev);
}

QDebug operator<<(QDebug debug, QmlGraphicsItem *item)
{
    if (!item) {
        debug << "QmlGraphicsItem(0)";
        return debug;
    }

    debug << item->metaObject()->className() << "(this =" << ((void*)item)
          << ", parent =" << ((void*)item->parentItem())
          << ", geometry =" << QRectF(item->pos(), QSizeF(item->width(), item->height()))
          << ", z =" << item->zValue() << ')';
    return debug;
}

int QmlGraphicsItemPrivate::consistentTime = -1;
void QmlGraphicsItemPrivate::setConsistentTime(int t)
{
    consistentTime = t;
}

QTime QmlGraphicsItemPrivate::currentTime()
{
    if (consistentTime == -1)
        return QTime::currentTime();
    else
        return QTime(0, 0).addMSecs(consistentTime);
}

void QmlGraphicsItemPrivate::start(QTime &t)
{
    t = currentTime();
}

int QmlGraphicsItemPrivate::elapsed(QTime &t)
{
    int n = t.msecsTo(currentTime());
    if (n < 0)                                // passed midnight
        n += 86400 * 1000;
    return n;
}

int QmlGraphicsItemPrivate::restart(QTime &t)
{
    QTime time = currentTime();
    int n = t.msecsTo(time);
    if (n < 0)                                // passed midnight
        n += 86400*1000;
    t = time;
    return n;
}

QT_END_NAMESPACE

#include <moc_qmlgraphicsitem.cpp>
