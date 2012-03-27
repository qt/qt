/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qbbscreeneventhandler.h"

#include "qbbinputcontext.h"
#include "qbbkeytranslator.h"

#include <QApplication>
#include <QDebug>

#include <errno.h>
#include <sys/keycodes.h>

QT_BEGIN_NAMESPACE

QBBScreenEventHandler::QBBScreenEventHandler()
    : mLastButtonState(Qt::NoButton)
    , mLastMouseWindow(0)
{
    // initialize array of touch points
    for (int i = 0; i < MAX_TOUCH_POINTS; i++) {

        // map array index to id
        mTouchPoints[i].id = i;

        // first finger is primary
        mTouchPoints[i].isPrimary = (i == 0);

        // pressure is not supported - use default
        mTouchPoints[i].pressure = 1.0;

        // nothing touching
        mTouchPoints[i].state = Qt::TouchPointReleased;
    }
}

bool QBBScreenEventHandler::handleEvent(screen_event_t event)
{
    // get the event type
    errno = 0;
    int qnxType;
    int result = screen_get_event_property_iv(event, SCREEN_PROPERTY_TYPE, &qnxType);
    if (result)
        qFatal("QBB: failed to query event type, errno=%d", errno);

    return handleEvent(event, qnxType);
}


bool QBBScreenEventHandler::handleEvent(screen_event_t event, int qnxType)
{
    switch (qnxType) {
    case SCREEN_EVENT_MTOUCH_TOUCH:
    case SCREEN_EVENT_MTOUCH_MOVE:
    case SCREEN_EVENT_MTOUCH_RELEASE:
        handleTouchEvent(event, qnxType);
        break;

    case SCREEN_EVENT_KEYBOARD:
        handleKeyboardEvent(event);
        break;

    case SCREEN_EVENT_POINTER:
        handlePointerEvent(event);
        break;

    case SCREEN_EVENT_CLOSE:
        handleCloseEvent(event);
        break;

    case SCREEN_EVENT_CREATE:
        handleCreateEvent(event);
        break;

    default:
        // event ignored
#if defined(QBBEVENTTHREAD_DEBUG)
        qDebug() << "QBB: QNX unknown event";
#endif
        return false;
    }

    return true;
}

void QBBScreenEventHandler::handleKeyboardEvent(screen_event_t event)
{
    // get flags of key event
    errno = 0;
    int flags;
    int result = screen_get_event_property_iv(event, SCREEN_PROPERTY_KEY_FLAGS, &flags);
    if (result) {
        qFatal("QBB: failed to query event flags, errno=%d", errno);
    }

    // get key code
    errno = 0;
    int sym;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_KEY_SYM, &sym);
    if (result) {
        qFatal("QBB: failed to query event sym, errno=%d", errno);
    }

    int modifiers;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_KEY_MODIFIERS, &modifiers);
    if (result) {
        qFatal("QBB: failed to query event modifiers, errno=%d", errno);
    }

    int scan;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_KEY_SCAN, &scan);
    if (result) {
        qFatal("QBB: failed to query event modifiers, errno=%d", errno);
    }

    int cap;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_KEY_CAP, &cap);
    if (result) {
        qFatal("QBB: failed to query event cap, errno=%d", errno);
    }

    // Let the input context have a stab at it first.
    QWidget *focusWidget = QApplication::focusWidget();
    if (focusWidget) {
        QInputContext* inputContext = focusWidget->inputContext();
        if (inputContext) {
            QBBInputContext *qbbInputContext = dynamic_cast<QBBInputContext *>(inputContext);

            if (qbbInputContext && qbbInputContext->handleKeyboardEvent(flags, sym, modifiers, scan, cap))
                return;
        }
    }

    injectKeyboardEvent(flags, sym, modifiers, scan, cap);
}

void QBBScreenEventHandler::injectKeyboardEvent(int flags, int sym, int modifiers, int scan, int cap)
{
    Q_UNUSED(scan);

    Qt::KeyboardModifiers qtMod = Qt::NoModifier;
    if (modifiers & KEYMOD_SHIFT)
        qtMod |= Qt::ShiftModifier;
    if (modifiers & KEYMOD_CTRL)
        qtMod |= Qt::ControlModifier;
    if (modifiers & KEYMOD_ALT)
        qtMod |= Qt::AltModifier;

    // determine event type
    QEvent::Type type = (flags & KEY_DOWN) ? QEvent::KeyPress : QEvent::KeyRelease;

    // Check if the key cap is valid
    if (flags & KEY_CAP_VALID) {
        Qt::Key key;
        QString keyStr;

        if (cap >= 0x20 && cap <= 0x0ff) {
            key = Qt::Key(std::toupper(cap));   // Qt expects the CAP to be upper case.

            if ( qtMod & Qt::ControlModifier ) {
                keyStr = QChar((int)(key & 0x3f));
            } else {
                if (flags & KEY_SYM_VALID) {
                    keyStr = QChar(sym);
                }
            }
        } else if ((cap > 0x0ff && cap < UNICODE_PRIVATE_USE_AREA_FIRST) || cap > UNICODE_PRIVATE_USE_AREA_LAST) {
            key = (Qt::Key)cap;
            keyStr = QChar(sym);
        } else {
            if (isKeypadKey(cap))
                qtMod |= Qt::KeypadModifier; // Is this right?
            key = keyTranslator(cap);
        }

        QWindowSystemInterface::handleExtendedKeyEvent(QApplication::activeWindow(), type, key, qtMod,
                                                       scan, sym, modifiers, keyStr);
#if defined(QBBScreenEventHandler_DEBUG)
        qDebug() << "QBB: Qt key t=" << type << ", k=" << key << ", s=" << keyStr;
#endif
    }
}

void QBBScreenEventHandler::injectPointerMoveEvent(int x, int y)
{
#if defined(QBBScreenEventHandler_DEBUG)
    qDebug() << "Injecting mouse event..." << x << y;
#endif

    QWidget *w = qApp->topLevelAt(x, y);
    void *qnxWindow = w ? w->platformWindow() : 0;

    // Generate enter and leave events as needed.
    if (qnxWindow != mLastMouseWindow) {
        QWidget* wOld = QWidget::find( (WId)mLastMouseWindow );

        if (wOld) {
            QWindowSystemInterface::handleLeaveEvent(wOld);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt leave, w=" << wOld;
#endif
        }

        if (w) {
            QWindowSystemInterface::handleEnterEvent(w);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt enter, w=" << w;
#endif
        }
    }

    mLastMouseWindow = qnxWindow;

    // convert point to local coordinates
    QPoint globalPoint(x, y);
    QPoint localPoint(x, y);

    if (w)
        localPoint = QPoint(x - w->x(), y - w->y());

    // Convert buttons.
    Qt::MouseButtons buttons = mLastButtonState;

    if (w) {
        // Inject mouse event into Qt only if something has changed.
        if (mLastGlobalMousePoint != globalPoint ||
            mLastLocalMousePoint != localPoint ||
            mLastButtonState != buttons) {
            QWindowSystemInterface::handleMouseEvent(w, localPoint, globalPoint, buttons);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt mouse, w=" << w << ", (" << localPoint.x() << "," << localPoint.y() << "), b=" << (int)buttons;
#endif
        }
    }

    mLastGlobalMousePoint = globalPoint;
    mLastLocalMousePoint = localPoint;
    mLastButtonState = buttons;
}

void QBBScreenEventHandler::handlePointerEvent(screen_event_t event)
{
    errno = 0;

    // Query the window that was clicked
    void *qnxWindow;
    int result = screen_get_event_property_pv(event, SCREEN_PROPERTY_WINDOW, &qnxWindow);
    if (result) {
        qFatal("QBB: failed to query event window, errno=%d", errno);
    }

    // Query the button states
    int buttonState = 0;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_BUTTONS, &buttonState);
    if (result) {
        qFatal("QBB: failed to query event button state, errno=%d", errno);
    }

    // Query the window position
    int windowPos[2];
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_SOURCE_POSITION, windowPos);
    if (result) {
        qFatal("QBB: failed to query event window position, errno=%d", errno);
    }

    // Query the screen position
    int pos[2];
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_POSITION, pos);
    if (result) {
        qFatal("QBB: failed to query event position, errno=%d", errno);
    }

    // Query the wheel delta
    int wheelDelta = 0;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_MOUSE_WHEEL, &wheelDelta);
    if (result) {
        qFatal("QBB: failed to query event wheel delta, errno=%d", errno);
    }

    // map window to top-level widget
    QWidget* w = QWidget::find( (WId)qnxWindow );

    // Generate enter and leave events as needed.
    if (qnxWindow != mLastMouseWindow) {
        QWidget* wOld = QWidget::find( (WId)mLastMouseWindow );

        if (wOld) {
            QWindowSystemInterface::handleLeaveEvent(wOld);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt leave, w=" << wOld;
#endif
        }

        if (w) {
            QWindowSystemInterface::handleEnterEvent(w);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt enter, w=" << w;
#endif
        }
    }
    mLastMouseWindow = qnxWindow;

    // Apply scaling to wheel delta and invert value for Qt. We'll probably want to scale
    // this via a system preference at some point. But for now this is a sane value and makes
    // the wheel usable.
    wheelDelta *= -10;

    // convert point to local coordinates
    QPoint globalPoint(pos[0], pos[1]);
    QPoint localPoint(windowPos[0], windowPos[1]);

    // Convert buttons.
    // Some QNX header files invert 'Right Button versus "Left Button' ('Right' == 0x01). But they also offer a 'Button Swap' bit,
    // so we may receive events as shown. (If this is wrong, the fix is easy.)
    // QNX Button mask is 8 buttons wide, with a maximum value of 0x80.
    Qt::MouseButtons buttons = Qt::NoButton;
    if (buttonState & 0x01)
        buttons |= Qt::LeftButton;
    if (buttonState & 0x02)
        buttons |= Qt::MidButton;
    if (buttonState & 0x04)
        buttons |= Qt::RightButton;
    if (buttonState & 0x08)
        buttons |= Qt::XButton1;
    if (buttonState & 0x10)
        buttons |= Qt::XButton2;

    if (w) {
        // Inject mouse event into Qt only if something has changed.
        if (mLastGlobalMousePoint != globalPoint ||
            mLastLocalMousePoint != localPoint ||
            mLastButtonState != buttons) {
            QWindowSystemInterface::handleMouseEvent(w, localPoint, globalPoint, buttons);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt mouse, w=" << w << ", (" << localPoint.x() << "," << localPoint.y() << "), b=" << (int)buttons;
#endif
        }

        if (wheelDelta) {
            // Screen only supports a single wheel, so we will assume Vertical orientation for
            // now since that is pretty much standard.
            QWindowSystemInterface::handleWheelEvent(w, localPoint, globalPoint, wheelDelta, Qt::Vertical);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt wheel, w=" << w << ", (" << localPoint.x() << "," << localPoint.y() << "), d=" << (int)wheelDelta;
#endif
        }
    }

    mLastGlobalMousePoint = globalPoint;
    mLastLocalMousePoint = localPoint;
    mLastButtonState = buttons;
}

void QBBScreenEventHandler::handleTouchEvent(screen_event_t event, int qnxType)
{
    // get display coordinates of touch
    errno = 0;
    int pos[2];
    int result = screen_get_event_property_iv(event, SCREEN_PROPERTY_POSITION, pos);
    if (result) {
        qFatal("QBB: failed to query event position, errno=%d", errno);
    }

    // get window coordinates of touch
    errno = 0;
    int windowPos[2];
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_SOURCE_POSITION, windowPos);
    if (result) {
        qFatal("QBB: failed to query event window position, errno=%d", errno);
    }

    // determine which finger touched
    errno = 0;
    int touchId;
    result = screen_get_event_property_iv(event, SCREEN_PROPERTY_TOUCH_ID, &touchId);
    if (result) {
        qFatal("QBB: failed to query event touch id, errno=%d", errno);
    }

    // determine which window was touched
    errno = 0;
    void *qnxWindow;
    result = screen_get_event_property_pv(event, SCREEN_PROPERTY_WINDOW, &qnxWindow);
    if (result) {
        qFatal("QBB: failed to query event window, errno=%d", errno);
    }

    // check if finger is valid
    if (touchId < MAX_TOUCH_POINTS) {

        // map window to top-level widget
        QWidget* w = QWidget::find( (WId)qnxWindow );

        // Generate enter and leave events as needed.
        if (qnxWindow != mLastMouseWindow) {
            QWidget* wOld = QWidget::find( (WId)mLastMouseWindow );

            if (wOld) {
                QWindowSystemInterface::handleLeaveEvent(wOld);
    #if defined(QBBScreenEventHandler_DEBUG)
                qDebug() << "QBB: Qt leave, w=" << wOld;
    #endif
            }

            if (w) {
                QWindowSystemInterface::handleEnterEvent(w);
    #if defined(QBBScreenEventHandler_DEBUG)
                qDebug() << "QBB: Qt enter, w=" << w;
    #endif
            }
        }
        mLastMouseWindow = qnxWindow;

        if (w) {
            // convert primary touch to mouse event
            if (touchId == 0) {

                // convert point to local coordinates
                QPoint globalPoint(pos[0], pos[1]);
                QPoint localPoint(windowPos[0], windowPos[1]);

                // map touch state to button state
                Qt::MouseButtons buttons = (qnxType == SCREEN_EVENT_MTOUCH_RELEASE) ? Qt::NoButton : Qt::LeftButton;

                // inject event into Qt
                QWindowSystemInterface::handleMouseEvent(w, localPoint, globalPoint, buttons);
#if defined(QBBScreenEventHandler_DEBUG)
                qDebug() << "QBB: Qt mouse, w=" << w << ", (" << localPoint.x() << "," << localPoint.y() << "), b=" << buttons;
#endif
            }

            // get size of screen which contains window
            QPlatformScreen* platformScreen = QPlatformScreen::platformScreenForWidget(w);
            QSize screenSize = platformScreen->physicalSize();

            // update cached position of current touch point
            mTouchPoints[touchId].normalPosition = QPointF( ((qreal)pos[0]) / screenSize.width(), ((qreal)pos[1]) / screenSize.height() );
            mTouchPoints[touchId].area = QRectF( pos[0], pos[1], 0.0, 0.0 );

            // determine event type and update state of current touch point
            QEvent::Type type = QEvent::None;
            switch (qnxType) {
            case SCREEN_EVENT_MTOUCH_TOUCH:
                mTouchPoints[touchId].state = Qt::TouchPointPressed;
                type = QEvent::TouchBegin;
                break;
            case SCREEN_EVENT_MTOUCH_MOVE:
                mTouchPoints[touchId].state = Qt::TouchPointMoved;
                type = QEvent::TouchUpdate;
                break;
            case SCREEN_EVENT_MTOUCH_RELEASE:
                mTouchPoints[touchId].state = Qt::TouchPointReleased;
                type = QEvent::TouchEnd;
                break;
            }

            // build list of active touch points
            QList<QWindowSystemInterface::TouchPoint> pointList;
            for (int i = 0; i < MAX_TOUCH_POINTS; i++) {
                if (i == touchId) {
                    // current touch point is always active
                    pointList.append(mTouchPoints[i]);
                } else if (mTouchPoints[i].state != Qt::TouchPointReleased) {
                    // finger is down but did not move
                    mTouchPoints[i].state = Qt::TouchPointStationary;
                    pointList.append(mTouchPoints[i]);
                }
            }

            // inject event into Qt
            QWindowSystemInterface::handleTouchEvent(w, type, QTouchEvent::TouchScreen, pointList);
#if defined(QBBScreenEventHandler_DEBUG)
            qDebug() << "QBB: Qt touch, w=" << w << ", p=(" << pos[0] << "," << pos[1] << "), t=" << type;
#endif
        }
    }
}

void QBBScreenEventHandler::handleCloseEvent(screen_event_t event)
{
    screen_window_t window = 0;
    if (screen_get_event_property_pv(event, SCREEN_PROPERTY_WINDOW, (void**)&window) != 0)
        qFatal("QBB: failed to query event window property, errno=%d", errno);

    emit windowClosed(window);

    // map window to top-level widget
    QWidget* w = QWidget::find( (WId)window );
    if (w != NULL)
        QWindowSystemInterface::handleCloseEvent(w);
}

void QBBScreenEventHandler::handleCreateEvent(screen_event_t event)
{
    screen_window_t window = 0;
    if (screen_get_event_property_pv(event, SCREEN_PROPERTY_WINDOW, (void**)&window) != 0)
        qFatal("QBB: failed to query event window property, errno=%d", errno);

    emit newWindowCreated(window);
}

#include "moc_qbbscreeneventhandler.cpp"

QT_END_NAMESPACE
