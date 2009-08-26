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

#include <QDebug>
#include <QPen>
#include <QFile>
#include <QEvent>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QGraphicsSceneMouseEvent>
#include <QtScript/qscriptengine.h>
#include <private/qfxperf_p.h>
#include <QtGui/qgraphicstransform.h>

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlopenmetaobject.h>
#include "qmlstate.h"
#include "qlistmodelinterface.h"

#include "qfxview.h"
#include "qmlstategroup.h"

#include "qfxitem_p.h"
#include "qfxitem.h"
#include "qfxevents_p.h"
#include <qmlcomponent.h>

QT_BEGIN_NAMESPACE

#ifndef FLT_MAX
#define FLT_MAX 1E+37
#endif

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Item,QFxItem)

QML_DEFINE_NOCREATE_TYPE(QGraphicsTransform);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Scale,QGraphicsScale)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Rotation,QGraphicsRotation)

/*!
    \qmlclass Transform
    \brief A transformation.
*/

/*!
    \qmlclass Scale
    \brief A Scale object provides a way to scale an Item.

    The scale object gives more control over scaling than using Item's scale property. Specifically,
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

    The origin point for the scale. The scale will be relative to this point.
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
    \qmlclass Rotation
    \brief A Rotation object provides a way to rotate an Item around a point using an axis in 3D space.

    The following example rotates a Rectangle around its interior point 25, 25:
    \qml
    Rectangle {
        width: 100; height: 100
        color: "blue"
        transform: Rotation { origin.x: 25; origin.y: 25; angle: 45}
    }
    \endqml

    Here is an example of various rotations applied to an \l Image.
    \snippet doc/src/snippets/declarative/rotation.qml 0

    \image axisrotation.png
*/

/*!
    \qmlproperty real Rotation::origin.x
    \qmlproperty real Rotation::origin.y

    The point to rotate around.
*/

/*!
    \qmlproperty real Rotation::axis.x
    \qmlproperty real Rotation::axis.y
    \qmlproperty real Rotation::axis.z

    A rotation axis is specified by a vector in 3D space By default the vector defines a rotation around the z-Axis.

    \image 3d-rotation-axis.png

*/

/*!
    \qmlproperty real Rotation::angle

    The angle, in degrees, to rotate.
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
    \class QFxContents
    \ingroup group_utility
    \brief The QFxContents class gives access to the height and width of an item's contents.

*/

QFxContents::QFxContents() : m_x(0), m_y(0), m_width(0), m_height(0)
{
}

/*!
    \qmlproperty qreal Item::childrenRect.x
    \qmlproperty qreal Item::childrenRect.y
    \qmlproperty qreal Item::childrenRect.width
    \qmlproperty qreal Item::childrenRect.height

    The childrenRect properties allow an item access to the geometry of its
    children. This property is useful if you have an item that needs to be
    sized to fit its children.
*/

QRectF QFxContents::rectF() const
{
    return QRectF(m_x, m_y, m_width, m_height);
}

//TODO: optimization: only check sender(), if there is one
void QFxContents::calcHeight()
{
    qreal oldy = m_y;
    qreal oldheight = m_height;

    qreal top = FLT_MAX;
    qreal bottom = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
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
void QFxContents::calcWidth()
{
    qreal oldx = m_x;
    qreal oldwidth = m_width;

    qreal left = FLT_MAX;
    qreal right = 0;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
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

void QFxContents::setItem(QFxItem *item)
{
    m_item = item;

    QList<QGraphicsItem *> children = m_item->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QFxItem *child = qobject_cast<QFxItem *>(children.at(i));
        connect(child, SIGNAL(heightChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(yChanged()), this, SLOT(calcHeight()));
        connect(child, SIGNAL(widthChanged()), this, SLOT(calcWidth()));
        connect(child, SIGNAL(xChanged()), this, SLOT(calcWidth()));
        connect(this, SIGNAL(rectChanged()), m_item, SIGNAL(childrenRectChanged()));
    }

    calcHeight();
    calcWidth();
}

/*
    Key filters can be installed on a QFxItem, but not removed.  Currently they
    are only used by attached objects (which are only destroyed on Item
    destruction), so this isn't a problem.  If in future this becomes any form
    of public API, they will have to support removal too.
*/
class QFxItemKeyFilter
{
public:
    QFxItemKeyFilter(QFxItem * = 0);
    virtual ~QFxItemKeyFilter();

    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);

private:
    QFxItemKeyFilter *m_next;
};

QFxItemKeyFilter::QFxItemKeyFilter(QFxItem *item)
: m_next(0)
{
    QFxItemPrivate *p =
        item?static_cast<QFxItemPrivate *>(QGraphicsItemPrivate::get(item)):0;
    if (p) {
        m_next = p->keyHandler;
        p->keyHandler = this;
    }
}

QFxItemKeyFilter::~QFxItemKeyFilter()
{
}

void QFxItemKeyFilter::keyPressed(QKeyEvent *event)
{
    if (m_next) m_next->keyPressed(event);
}

void QFxItemKeyFilter::keyReleased(QKeyEvent *event)
{
    if (m_next) m_next->keyReleased(event);
}

class QFxKeyNavigationAttachedPrivate : public QObjectPrivate
{
public:
    QFxKeyNavigationAttachedPrivate()
        : QObjectPrivate(), left(0), right(0), up(0), down(0) {}

    QFxItem *left;
    QFxItem *right;
    QFxItem *up;
    QFxItem *down;
};

class QFxKeyNavigationAttached : public QObject, public QFxItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFxKeyNavigationAttached);

    Q_PROPERTY(QFxItem *left READ left WRITE setLeft NOTIFY changed);
    Q_PROPERTY(QFxItem *right READ right WRITE setRight NOTIFY changed);
    Q_PROPERTY(QFxItem *up READ up WRITE setUp NOTIFY changed);
    Q_PROPERTY(QFxItem *down READ down WRITE setDown NOTIFY changed);
public:
    QFxKeyNavigationAttached(QObject * = 0);

    QFxItem *left() const;
    void setLeft(QFxItem *);
    QFxItem *right() const;
    void setRight(QFxItem *);
    QFxItem *up() const;
    void setUp(QFxItem *);
    QFxItem *down() const;
    void setDown(QFxItem *);

    static QFxKeyNavigationAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void changed();

private:
    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);
};

QFxKeyNavigationAttached::QFxKeyNavigationAttached(QObject *parent)
: QObject(*(new QFxKeyNavigationAttachedPrivate), parent),
  QFxItemKeyFilter(qobject_cast<QFxItem*>(parent))
{
}

QFxKeyNavigationAttached *
QFxKeyNavigationAttached::qmlAttachedProperties(QObject *obj)
{
    return new QFxKeyNavigationAttached(obj);
}

QFxItem *QFxKeyNavigationAttached::left() const
{
    Q_D(const QFxKeyNavigationAttached);
    return d->left;
}

void QFxKeyNavigationAttached::setLeft(QFxItem *i)
{
    Q_D(QFxKeyNavigationAttached);
    d->left = i;
    emit changed();
}

QFxItem *QFxKeyNavigationAttached::right() const
{
    Q_D(const QFxKeyNavigationAttached);
    return d->right;
}

void QFxKeyNavigationAttached::setRight(QFxItem *i)
{
    Q_D(QFxKeyNavigationAttached);
    d->right = i;
    emit changed();
}

QFxItem *QFxKeyNavigationAttached::up() const
{
    Q_D(const QFxKeyNavigationAttached);
    return d->up;
}

void QFxKeyNavigationAttached::setUp(QFxItem *i)
{
    Q_D(QFxKeyNavigationAttached);
    d->up = i;
    emit changed();
}

QFxItem *QFxKeyNavigationAttached::down() const
{
    Q_D(const QFxKeyNavigationAttached);
    return d->down;
}

void QFxKeyNavigationAttached::setDown(QFxItem *i)
{
    Q_D(QFxKeyNavigationAttached);
    d->down = i;
    emit changed();
}

void QFxKeyNavigationAttached::keyPressed(QKeyEvent *event)
{
    Q_D(QFxKeyNavigationAttached);

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

    if (!event->isAccepted()) QFxItemKeyFilter::keyPressed(event);
}

void QFxKeyNavigationAttached::keyReleased(QKeyEvent *event)
{
    Q_D(QFxKeyNavigationAttached);

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

    if (!event->isAccepted()) QFxItemKeyFilter::keyReleased(event);
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
                print("move left");
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
        Keys.onLeftPressed: print("move left")
    }
    \endcode

    See \l {Qt::Key}{Qt.Key} for the list of keyboard codes.

    \sa KeyEvent
*/

/*!
    \qmlproperty bool Keys::enabled

    This flags enables key handling if true (default); otherwise
    no key handlers will be called.
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

class QFxKeysAttachedPrivate : public QObjectPrivate
{
public:
    QFxKeysAttachedPrivate() : QObjectPrivate(), enabled(true) {}

    bool isConnected(const char *signalName);

    bool enabled;
};

class QFxKeysAttached : public QObject, public QFxItemKeyFilter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QFxKeysAttached);

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    QFxKeysAttached(QObject *parent=0);
    ~QFxKeysAttached();

    bool enabled() const { Q_D(const QFxKeysAttached); return d->enabled; }
    void setEnabled(bool enabled) {
        Q_D(QFxKeysAttached);
        if (enabled != d->enabled) {
            d->enabled = enabled;
            emit enabledChanged();
        }
    }

    static QFxKeysAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void enabledChanged();
    void pressed(QFxKeyEvent *event);
    void released(QFxKeyEvent *event);
    void digit0Pressed(QFxKeyEvent *event);
    void digit1Pressed(QFxKeyEvent *event);
    void digit2Pressed(QFxKeyEvent *event);
    void digit3Pressed(QFxKeyEvent *event);
    void digit4Pressed(QFxKeyEvent *event);
    void digit5Pressed(QFxKeyEvent *event);
    void digit6Pressed(QFxKeyEvent *event);
    void digit7Pressed(QFxKeyEvent *event);
    void digit8Pressed(QFxKeyEvent *event);
    void digit9Pressed(QFxKeyEvent *event);

    void leftPressed(QFxKeyEvent *event);
    void rightPressed(QFxKeyEvent *event);
    void upPressed(QFxKeyEvent *event);
    void downPressed(QFxKeyEvent *event);

    void asteriskPressed(QFxKeyEvent *event);
    void numberSignPressed(QFxKeyEvent *event);
    void escapePressed(QFxKeyEvent *event);
    void returnPressed(QFxKeyEvent *event);
    void enterPressed(QFxKeyEvent *event);
    void deletePressed(QFxKeyEvent *event);
    void spacePressed(QFxKeyEvent *event);
    void backPressed(QFxKeyEvent *event);
    void cancelPressed(QFxKeyEvent *event);
    void selectPressed(QFxKeyEvent *event);
    void yesPressed(QFxKeyEvent *event);
    void noPressed(QFxKeyEvent *event);
    void context1Pressed(QFxKeyEvent *event);
    void context2Pressed(QFxKeyEvent *event);
    void context3Pressed(QFxKeyEvent *event);
    void context4Pressed(QFxKeyEvent *event);
    void callPressed(QFxKeyEvent *event);
    void hangupPressed(QFxKeyEvent *event);
    void flipPressed(QFxKeyEvent *event);
    void menuPressed(QFxKeyEvent *event);
    void volumeUpPressed(QFxKeyEvent *event);
    void volumeDownPressed(QFxKeyEvent *event);

private:
    virtual void keyPressed(QKeyEvent *event);
    virtual void keyReleased(QKeyEvent *event);

    const char *keyToSignal(int key) {
        QByteArray keySignal;
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            keySignal = "digit0Pressed";
            keySignal[5] = '0' + (key - Qt::Key_0);
        } else {
            int i = 0;
            while (sigMap[i].key && sigMap[i].key != key)
                ++i;
            keySignal = sigMap[i].sig;
        }
        return keySignal;
    }

    struct SigMap {
        int key;
        const char *sig;
    };

    static const SigMap sigMap[];
};

const QFxKeysAttached::SigMap QFxKeysAttached::sigMap[] = {
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

bool QFxKeysAttachedPrivate::isConnected(const char *signalName)
{
    return isSignalConnected(signalIndex(signalName));
}

QFxKeysAttached::QFxKeysAttached(QObject *parent)
: QObject(*(new QFxKeysAttachedPrivate), parent),
  QFxItemKeyFilter(qobject_cast<QFxItem*>(parent))
{
}

QFxKeysAttached::~QFxKeysAttached()
{
}

void QFxKeysAttached::keyPressed(QKeyEvent *event)
{
    Q_D(QFxKeysAttached);
    if (!d->enabled) {
        event->ignore();
        return;
    }

    QFxKeyEvent ke(*event);
    QByteArray keySignal = keyToSignal(event->key());
    if (!keySignal.isEmpty()) {
        keySignal += "(QFxKeyEvent*)";
        if (d->isConnected(keySignal)) {
            // If we specifically handle a key then default to accepted
            ke.setAccepted(true);
            int idx = QFxKeysAttached::staticMetaObject.indexOfSignal(keySignal);
            metaObject()->method(idx).invoke(this, Q_ARG(QFxKeysAttached, &ke));
        }
    }
    if (!ke.isAccepted())
        emit pressed(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QFxItemKeyFilter::keyPressed(event);
}

void QFxKeysAttached::keyReleased(QKeyEvent *event)
{
    Q_D(QFxKeysAttached);
    if (!d->enabled) {
        event->ignore();
        return;
    }
    QFxKeyEvent ke(*event);
    emit released(&ke);
    event->setAccepted(ke.isAccepted());

    if (!event->isAccepted()) QFxItemKeyFilter::keyReleased(event);
}

QFxKeysAttached *QFxKeysAttached::qmlAttachedProperties(QObject *obj)
{
    return new QFxKeysAttached(obj);
}

/*!
    \qmlclass Item QFxItem
    \brief The Item is the most basic of all visual items in QML.
 */

/*!
    \class QFxItem Item
    \brief The QFxItem class is a generic QFxView item. It is the base class for all other view items.

    \qmltext
    All visual items in Qt Declarative inherit from QFxItem.  Although QFxItem
    has no visual appearance, it defines all the properties that are
    common across visual items - like the x and y position, and the
    width and height. \l {Keys}{Key handling} is also provided by Item.

    QFxItem is also useful for grouping items together.

    \qml
    Item {
        Image {
            file: "tile.png"
        }
        Image {
            x: 80
            width: 100
            height: 100
            file: "tile.png"
        }
        Image {
            x: 190
            width: 100
            height: 100
            tile: true
            file: "tile.png"
        }
    }
    \endqml

    \endqmltext

    \ingroup group_coreitems
*/

/*!
    \fn void QFxItem::activeFocusChanged()

    This signal is emitted when this item gains active focus.
*/

/*!
    \fn void QFxItem::baselineOffsetChanged()

    This signal is emitted when the baseline offset of the item
    is changed.
*/

/*!
    \fn void QFxItem::widthChanged()

    This signal is emitted when the width of the item changes.
*/

/*!
    \fn void QFxItem::heightChanged()

    This signal is emitted when the height of the item changes.
*/

/*!
    \fn void QFxItem::stateChanged(const QString &state)

    This signal is emitted when the \a state of the item changes.

    \sa states-transitions
*/

/*!
    \fn void QFxItem::parentChanged()

    This signal is emitted when the parent of the item changes.

    \sa setParentItem()
*/

/*!
    \fn void QFxItem::focusChanged()

    This signal is emitted when the item's focus state changes.

    \sa QFxItem::setFocus()
*/

static inline void qfxitem_registerAnchorLine() {
    static bool registered = false;
    if (!registered) {
        qRegisterMetaType<QFxAnchorLine>("QFxAnchorLine");
        registered = true;
    }
}

/*!
    \fn QFxItem::QFxItem(QFxItem *parent)

    Constructs a QFxItem with the given \a parent.
*/
QFxItem::QFxItem(QFxItem* parent)
  : QGraphicsObject(*(new QFxItemPrivate), parent, 0)
{
    Q_D(QFxItem);
    qfxitem_registerAnchorLine();
    d->init(parent);
}

/*! \internal
*/
QFxItem::QFxItem(QFxItemPrivate &dd, QFxItem *parent)
  : QGraphicsObject(dd, parent, 0)
{
    Q_D(QFxItem);
    qfxitem_registerAnchorLine();
    d->init(parent);
}

/*!
    Destroys the QFxItem.
*/
QFxItem::~QFxItem()
{
    Q_D(QFxItem);
    for (int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->clearItem(this);
    }
    for (int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        if (anchor->d_func()->item && anchor->d_func()->item->parentItem() != this) //child will be deleted anyway
            anchor->d_func()->updateOnComplete();
    }
    delete d->_anchorLines; d->_anchorLines = 0;
}

/*!
    \qmlproperty enum Item::transformOrigin
    This property holds the origin point around which scale and rotation transform.

    Nine transform origins are available, as shown in the image below.

    \image declarative-transformorigin.png

    This example scales an image about its center.
    \qml
    Image {
        source: "myimage.png"
        transformOrigin: "Center"
        scale: 4
    }
    \endqml

    The default transform origin is \c TopLeft.
*/

/*!
    \qmlproperty Item Item::parent
    This property holds the parent of the item.
*/

/*!
    \property QFxItem::parent
    This property holds the parent of the item.
*/
void QFxItem::setParentItem(QFxItem *parent)
{
    QFxItem *oldParent = parentItem();
    if (parent == oldParent || !parent) return;

    QObject::setParent(parent);
    QGraphicsObject::setParentItem(parent);
}

/*!
    Returns the QFxItem parent of this item.
*/
QFxItem *QFxItem::parentItem() const
{
    return qobject_cast<QFxItem *>(QGraphicsObject::parentItem());
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
    \property QFxItem::children

    This property contains the list of visual children of this item.
*/

/*!
    \property QFxItem::resources

    This property contains non-visual resources that you want to
    reference by name.
*/

/*!
    Returns true if construction of the QML component is complete; otherwise
    returns false.

    It is often desireable to delay some processing until the component is
    completed.

    \sa componentComplete()
*/
bool QFxItem::isComponentComplete() const
{
    Q_D(const QFxItem);
    return d->_componentComplete;
}

/*!
    \property QFxItem::anchors
    \brief The anchors (alignments) used by the item.
*/
QFxAnchors *QFxItem::anchors()
{
    Q_D(QFxItem);
    return d->anchors();
}

void QFxItemPrivate::data_removeAt(int)
{
    // ###
}

int QFxItemPrivate::data_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::data_append(QObject *o)
{
    Q_Q(QFxItem);
    QFxItem *i = qobject_cast<QFxItem *>(o);
    if (i)
        q->children()->append(i);
    else
        resources_append(o);
}

void QFxItemPrivate::data_insert(int, QObject *)
{
    // ###
}

QObject *QFxItemPrivate::data_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::data_clear()
{
    // ###
}

void QFxItemPrivate::resources_removeAt(int)
{
    // ###
}

int QFxItemPrivate::resources_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::resources_append(QObject *o)
{
    Q_Q(QFxItem);
    o->setParent(q);
}

void QFxItemPrivate::resources_insert(int, QObject *)
{
    // ###
}

QObject *QFxItemPrivate::resources_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::resources_clear()
{
    // ###
}

void QFxItemPrivate::children_removeAt(int)
{
    // ###
}

int QFxItemPrivate::children_count() const
{
    // ###
    return 0;
}

void QFxItemPrivate::children_append(QFxItem *i)
{
    Q_Q(QFxItem);
    i->setParentItem(q);
}

void QFxItemPrivate::children_insert(int, QFxItem *)
{
    // ###
}

QFxItem *QFxItemPrivate::children_at(int) const
{
    // ###
    return 0;
}

void QFxItemPrivate::children_clear()
{
    // ###
}


void QFxItemPrivate::transform_removeAt(int i)
{
    if (!transformData)
        return;
    transformData->graphicsTransforms.removeAt(i);
    dirtySceneTransform = 1;
}

int QFxItemPrivate::transform_count() const
{
    return transformData ? transformData->graphicsTransforms.size() : 0;
}

void QFxItemPrivate::transform_append(QGraphicsTransform *item)
{
    appendGraphicsTransform(item);
}

void QFxItemPrivate::transform_insert(int, QGraphicsTransform *)
{
    // ###
}

QGraphicsTransform *QFxItemPrivate::transform_at(int idx) const
{
    if (!transformData)
        return 0;
    return transformData->graphicsTransforms.at(idx);
}

void QFxItemPrivate::transform_clear()
{
    if (!transformData)
        return;
    Q_Q(QFxItem);
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
    \property QFxItem::data

    The data property is allows you to freely mix the visual children
    and the non-visual resources of an item.  If you assign a visual
    item to the data list it becomes a child and if you assign any
    other object type, it is added as a resource.

    data is a behind-the-scenes property: you should never need to
    explicitly specify it.
 */
QmlList<QObject *> *QFxItem::data()
{
    Q_D(QFxItem);
    return &d->data;
}

/*!
    \property QFxItem::childrenRect
    \brief The geometry of an item's children.

    childrenRect provides an easy way to access the (collective) position and size of the item's children.
*/
QRectF QFxItem::childrenRect()
{
    Q_D(QFxItem);
    if (!d->_contents) {
        d->_contents = new QFxContents;
        d->_contents->setParent(this);
        d->_contents->setItem(this);
    }
    return d->_contents->rectF();
}

bool QFxItem::clip() const
{
    return flags() & ItemClipsChildrenToShape;
}

void QFxItem::setClip(bool c)
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
  \property QFxItem::width

  Defines the item's width relative to its parent.
 */

/*!
  \property QFxItem::height

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
  This function is called to handle this item's changes in
  geometry from \a oldGeometry to \a newGeometry. If the two
  geometries are the same, it doesn't do anything.
 */
void QFxItem::geometryChanged(const QRectF &newGeometry,
                              const QRectF &oldGeometry)
{
    Q_D(QFxItem);

    if (d->_anchors)
        d->_anchors->d_func()->updateMe();

    if (transformOrigin() != QFxItem::TopLeft)
        setTransformOriginPoint(d->computeTransformOrigin());

    if (newGeometry.x() != oldGeometry.x())
        emit xChanged();
    if (newGeometry.width() != oldGeometry.width())
        emit widthChanged();
    if (newGeometry.y() != oldGeometry.y())
        emit yChanged();
    if (newGeometry.height() != oldGeometry.height())
        emit heightChanged();

    for(int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->update(this, newGeometry, oldGeometry);
    }
}

/*!
  \reimp
*/
void QFxItem::keyPressEvent(QKeyEvent *event)
{
    Q_D(QFxItem);
    if (d->keyHandler)
        d->keyHandler->keyPressed(event);
    else
        event->ignore();
}

/*!
  \reimp
*/
void QFxItem::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QFxItem);
    if (d->keyHandler)
        d->keyHandler->keyReleased(event);
    else
        event->ignore();
}

/*!
  \qmlproperty string Item::id
  This property holds the identifier for the item.

  The identifier can be used in bindings and other expressions to
  refer to the item. For example:

  \qml
  Text { id: myText; ... }
  Text { text: myText.text }
  \endqml

  The identifier is available throughout to the \l {components}{component}
  where it is declared.  The identifier must be unique in thecomponent.
*/

/*!
    \internal
*/
QFxAnchorLine QFxItem::left() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->left;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::right() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->right;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::horizontalCenter() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->hCenter;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::top() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->top;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::bottom() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->bottom;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::verticalCenter() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->vCenter;
}

/*!
    \internal
*/
QFxAnchorLine QFxItem::baseline() const
{
    Q_D(const QFxItem);
    return d->anchorLines()->baseline;
}

/*!
  \property QFxItem::top

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::bottom

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::left

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::right

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::horizontalCenter

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
*/

/*!
  \property QFxItem::verticalCenter

  One of the anchor lines of the item.

  For more information see \l {anchor-layout}{Anchor Layouts}.
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
  \property QFxItem::baselineOffset
  \brief The position of the item's baseline in local coordinates.

  The baseline of a Text item is the imaginary line on which the text
  sits. Controls containing text usually set their baseline to the
  baseline of their text.

  For non-text items, a default baseline offset of 0 is used.
*/
qreal QFxItem::baselineOffset() const
{
    Q_D(const QFxItem);
    if (!d->_baselineOffset.isValid()) {
        return 0.0;
    } else
        return d->_baselineOffset;
}

/*!
    \internal
*/
void QFxItem::setBaselineOffset(qreal offset)
{
    Q_D(QFxItem);
    if (offset == d->_baselineOffset)
        return;

    d->_baselineOffset = offset;
    emit baselineOffsetChanged();

    for(int ii = 0; ii < d->dependantAnchors.count(); ++ii) {
        QFxAnchors *anchor = d->dependantAnchors.at(ii);
        anchor->d_func()->updateVerticalAnchors();
    }
}

/*!
  \qmlproperty real Item::rotation
  This property holds the rotation of the item in degrees.

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
          color: "green"
          width: 25; height: 25
      }
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
  example, you might need to use an opacity filter (not yet available) instead.

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

  \sa setKeepMouseGrab
 */
bool QFxItem::keepMouseGrab() const
{
    Q_D(const QFxItem);
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

  \sa keepMouseGrab
 */
void QFxItem::setKeepMouseGrab(bool keep)
{
    Q_D(QFxItem);
    d->_keepMouse = keep;
}

/*!
  This function emits the \e activeFocusChanged signal.
  \a flag is not used.
 */
void QFxItem::activeFocusChanged(bool flag)
{
    Q_UNUSED(flag);
    emit activeFocusChanged();
}

/*!
  This function emits the \e focusChanged signal.

  Subclasses overriding this function should call up
  to their base class.
*/
void QFxItem::focusChanged(bool flag)
{
    Q_UNUSED(flag);
    emit focusChanged();
}

QmlList<QFxItem *> *QFxItem::children()
{
    Q_D(QFxItem);
    return &(d->children);
}

QmlList<QObject *> *QFxItem::resources()
{
    Q_D(QFxItem);
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

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::states
  This property holds a list of states defined by the item.

  \sa {states-transitions}{States and Transitions}
*/
QmlList<QmlState *>* QFxItem::states()
{
    Q_D(QFxItem);
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

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::transitions
  This property holds a list of transitions defined by the item.

  \sa {states-transitions}{States and Transitions}
*/
QmlList<QmlTransition *>* QFxItem::transitions()
{
    Q_D(QFxItem);
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
  \property QFxItem::clip
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

  \sa {states-transitions}{States and Transitions}
*/

/*!
  \property QFxItem::state

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

  \sa {states-transitions}{States and Transitions}
*/
QString QFxItem::state() const
{
    Q_D(const QFxItem);
    if (!d->_stateGroup)
        return QString();
    else
        return d->_stateGroup->state();
}

void QFxItem::setState(const QString &state)
{
    Q_D(QFxItem);
    d->states()->setState(state);
}

/*!
  \qmlproperty list<Transform> Item::transform
  This property holds the list of transformations to apply.

  For more information see \l Transform.
*/

/*!
  \property QFxItem::transform
  This property holds a list of transformations set on the item.
*/
QmlList<QGraphicsTransform *>* QFxItem::transform()
{
    Q_D(QFxItem);
    return &(d->transform);
}

/*!
  classBegin() is called when the item is constructed, but its
  properties have not yet been set.

  \sa componentComplete(), isComponentComplete()
*/
void QFxItem::classBegin()
{
    Q_D(QFxItem);
    d->_componentComplete = false;
    if (d->_stateGroup)
        d->_stateGroup->classBegin();
    if (d->_anchors)
        d->_anchors->classBegin();
}

/*!
  componentComplete() is called when all items in the component
  have been constructed.  It is often desireable to delay some
  processing until the component is complete an all bindings in the
  component have been resolved.
*/
void QFxItem::componentComplete()
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::ItemComponentComplete> cc;
#endif

    Q_D(QFxItem);
    d->_componentComplete = true;
    if (d->_stateGroup)
        d->_stateGroup->componentComplete();
    if (d->_anchors) {
        d->_anchors->componentComplete();
        d->_anchors->d_func()->updateOnComplete();
    }
}

QmlStateGroup *QFxItemPrivate::states()
{
    Q_Q(QFxItem);
    if (!_stateGroup) {
        _stateGroup = new QmlStateGroup(q);
        if (!_componentComplete)
            _stateGroup->classBegin();
        QObject::connect(_stateGroup, SIGNAL(stateChanged(QString)),
                         q, SIGNAL(stateChanged(QString)));
    }

    return _stateGroup;
}

QFxItemPrivate::AnchorLines::AnchorLines(QFxItem *q)
{
    left.item = q;
    left.anchorLine = QFxAnchorLine::Left;
    right.item = q;
    right.anchorLine = QFxAnchorLine::Right;
    hCenter.item = q;
    hCenter.anchorLine = QFxAnchorLine::HCenter;
    top.item = q;
    top.anchorLine = QFxAnchorLine::Top;
    bottom.item = q;
    bottom.anchorLine = QFxAnchorLine::Bottom;
    vCenter.item = q;
    vCenter.anchorLine = QFxAnchorLine::VCenter;
    baseline.item = q;
    baseline.anchorLine = QFxAnchorLine::Baseline;
}

QPointF QFxItemPrivate::computeTransformOrigin() const
{
    Q_Q(const QFxItem);

    QRectF br = q->boundingRect();

    switch(origin) {
    default:
    case QFxItem::TopLeft:
        return QPointF(0, 0);
    case QFxItem::Top:
        return QPointF(br.width() / 2., 0);
    case QFxItem::TopRight:
        return QPointF(br.width(), 0);
    case QFxItem::Left:
        return QPointF(0, br.height() / 2.);
    case QFxItem::Center:
        return QPointF(br.width() / 2., br.height() / 2.);
    case QFxItem::Right:
        return QPointF(br.width(), br.height() / 2.);
    case QFxItem::BottomLeft:
        return QPointF(0, br.height());
    case QFxItem::Bottom:
        return QPointF(br.width() / 2., br.height());
    case QFxItem::BottomRight:
        return QPointF(br.width(), br.height());
    }
}

/*!
    \reimp
*/
bool QFxItem::sceneEvent(QEvent *event)
{
    return QGraphicsItem::sceneEvent(event);
}

/*!
    \reimp
*/
QVariant QFxItem::itemChange(GraphicsItemChange change,
                                       const QVariant &value)
{
    if (change == ItemParentHasChanged) {
        emit parentChanged();
    }

    return QGraphicsItem::itemChange(change, value);
}

/*!
    \reimp
*/
QRectF QFxItem::boundingRect() const
{
    Q_D(const QFxItem);
    return QRectF(0, 0, d->width, d->height);
}

/*!
    \enum QFxItem::TransformOrigin

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
QFxItem::TransformOrigin QFxItem::transformOrigin() const
{
    Q_D(const QFxItem);
    return d->origin;
}

/*!
    Set the transform \a origin.
*/
void QFxItem::setTransformOrigin(TransformOrigin origin)
{
    Q_D(QFxItem);
    if (origin != d->origin) {
        d->origin = origin;
        QGraphicsItem::setTransformOriginPoint(d->computeTransformOrigin());
    }
}

/*!
    \property QFxItem::smooth
    \brief whether the item is smoothly transformed.

    This property is provided purely for the purpose of optimization. Turning
    smooth transforms off is faster, but looks worse; turning smooth
    transformations on is slower, but looks better.

    By default smooth transformations are off.
*/
bool QFxItem::smoothTransform() const
{
    Q_D(const QFxItem);
    return d->smooth;
}

void QFxItem::setSmoothTransform(bool s)
{
    Q_D(QFxItem);
    if (d->smooth == s)
        return;
    d->smooth = s;
    update();
}

qreal QFxItem::width() const
{
    Q_D(const QFxItem);
    return d->width;
}

void QFxItem::setWidth(qreal w)
{
    Q_D(QFxItem);
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

void QFxItem::setImplicitWidth(qreal w)
{
    Q_D(QFxItem);
    if (d->width == w || widthValid())
        return;

    qreal oldWidth = d->width;

    prepareGeometryChange();
    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), oldWidth, height()));
}

bool QFxItem::widthValid() const
{
    Q_D(const QFxItem);
    return d->widthValid;
}

qreal QFxItem::height() const
{
    Q_D(const QFxItem);
    return d->height;
}

void QFxItem::setHeight(qreal h)
{
    Q_D(QFxItem);
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

void QFxItem::setImplicitHeight(qreal h)
{
    Q_D(QFxItem);
    if (d->height == h || heightValid())
        return;

    qreal oldHeight = d->height;

    prepareGeometryChange();
    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()),
                    QRectF(x(), y(), width(), oldHeight));
}

bool QFxItem::heightValid() const
{
    Q_D(const QFxItem);
    return d->heightValid;
}

/*!
  \qmlproperty bool Item::focus
  This property indicates whether the item has has an active focus request. Set this
  property to true to request active focus.
*/

bool QFxItem::hasFocus() const
{
    const QGraphicsItem *current = this->parentItem();
    while (current && !(current->flags() & ItemAutoDetectsFocusProxy))
        current = current->parentItem();

    if (current)
        return current->focusProxy() == this;
    else
        return QGraphicsItem::hasFocus();
}

void QFxItem::setFocus(bool focus)
{
    QGraphicsScene *s = scene();
    if (!s) {
        if (focus) QGraphicsItem::setFocus(Qt::OtherFocusReason);
        else QGraphicsItem::clearFocus();
        focusChanged(focus);
        return;
    }

    QGraphicsItem *current = this->parentItem();
    while (current && !(current->flags() & ItemAutoDetectsFocusProxy))
        current = current->parentItem();

    if (!current) {
        if (focus) QGraphicsItem::setFocus(Qt::OtherFocusReason);
        else QGraphicsItem::clearFocus();
        focusChanged(focus);
        return;
    }

    if (current->focusProxy() && current->focusProxy() != this) {
        QFxItem *currentItem = qobject_cast<QFxItem *>(current->focusProxy());
        if (currentItem)
            currentItem->setFocus(false);
    }

    if (current->focusProxy() == this && !focus)
        current->setFocusProxy(0);
    else if (focus)
        current->setFocusProxy(this);

    focusChanged(focus);
}

/*!
  \qmlproperty bool Item::activeFocus
  This property indicates whether the item has the active focus.
*/

bool QFxItem::hasActiveFocus() const
{
    return QGraphicsItem::hasFocus();
}

/*!
    \reimp
*/
void QFxItem::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

/*!
    \reimp
*/
bool QFxItem::event(QEvent *ev)
{
    return QGraphicsObject::event(ev);
}

QDebug operator<<(QDebug debug, QFxItem *item)
{
    if (!item) {
        debug << "QFxItem(0)";
        return debug;
    }

    debug << item->metaObject()->className() << "(this =" << ((void*)item)
          << ", parent =" << ((void*)item->parentItem())
          << ", geometry =" << QRectF(item->pos(), QSizeF(item->width(), item->height()))
          << ", z =" << item->zValue() << ")";
    return debug;
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxKeysAttached)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Keys,QFxKeysAttached)
QML_DECLARE_TYPE(QFxKeyNavigationAttached)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,KeyNavigation,QFxKeyNavigationAttached)

#include "moc_qfxitem.cpp"
#include "qfxitem.moc"
