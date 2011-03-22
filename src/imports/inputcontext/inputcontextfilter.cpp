/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "inputcontextfilter.h"

#include "declarativeinputcontext.h"

#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass KeyEvent InputContextKeyEvent

    \inqmlmodule Qt.labs.inputcontext

    \brief The KeyEvent object provides information about a key event.
*/

/*!
    \qmlproperty int KeyEvent::key

    This property holds the key code of the key that was pressed or released.
*/

/*!
    \qmlproperty string KeyEvent::text

    This property holds the text that this key generated.
*/

/*!
    \qmlproperty int KeyEvent::modifiers

    This property holds the keyboard modifier flags that existed immediately
    after this event was generated.
*/

/*!
    \qmlproperty bool KeyEvent::autoRepeat

    This property returns true if this event comes from an auto repeating key
    press, on the initial key press this returns false.
*/

/*!
    \qmlproperty int KeyEvent::count

    This property holds the number of keys involved in this event.  If the
    \l text is non-empty this is the length of the string.
*/

/*!
    \qmlproperty bool KeyEvent::accepted

    This property holds whether the event was accepted.

    This is false by default.
*/

/*!
    \qmlclass MouseEvent InputContextMouseEvent

    \inqmlmodule Qt.labs.inputcontext

    \brief The MouseEvent object provides information about a mouse event.

*/

/*!
    \qmlproperty int MouseEvent::x

    This property holds the x position in scene coordinates of the mouse cursor
    at the time of the event.
*/

/*!
    \qmlproperty int MouseEvent::y

    This property holds the y position in scene coordinates of the mouse cursor
    at the time of the event.
*/

/*!
    \qmlproperty enum MouseEvent::button

    This property holds the button that caused the event.
*/

/*!
    \qmlproperty int MouseEvent::buttons

    This property holds the button state when the event was generated.
*/

/*!
    \qmlproperty int MouseEvent::modifiers

    This property holds the keyboard modifier flags that existed when the event
    was generated.
*/

/*!
    \qmlproperty bool MouseEvent::accepted

    This property holds whether the event was accepted.

    This is false by default.
*/

/*!
    \qmlclass MouseHandler InputContextMouseHandler

    \inqmlmodule Qt.labs.inputcontext

    \brief The MouseHandler item provides mouse event handling for input methods.

    The MouseHandler item can be used to handle mouse press, release, move and
    double click events within or surrounding the pre-edit text.
*/

/*!
    \qmlsignal MouseHandler::onPressed(int cursor, MouseEvent mouse)

    This handler is called when there is a press.  The \a cursor parameter is
    the text cursor position of the press within the pre-edit text, and the
    \a mouse parameter holds information about the press.
*/

/*!
    \qmlsignal MouseHandler::onReleased(int cursor, MouseEvent mouse)

    This handler is called when there is a release.  The \a cursor parameter is
    the text cursor position of the press within the pre-edit text, and the
    \a mouse parameter holds information about the release.
*/

/*!
    \qmlsignal MouseHandler::onPositionChanged(int cursor, MouseEvent mouse)

    This handler is called when the mouse position changes.

    The \a cursor parameter is the text cursor position of the press within
    the pre-edit text, and the \a mouse parameter holds information about the
    position change.
*/

/*!
    \qmlsignal MouseHandler::onDoubleClicked(int cursor, MouseEvent mouse)

    This handler is called when there is a double-click.  The \a cursor
    parameter is the text cursor position of the press within the pre-edit
    text, and the \a mouse parameter holds information about the double-click.
*/

InputContextMouseHandler::InputContextMouseHandler(QObject *parent)
    : QObject(parent)
{
    if (DeclarativeInputContext *context = qobject_cast<DeclarativeInputContext *>(
            qApp->inputContext())) {
        context->registerMouseHandler(this);
    }
}

void InputContextMouseHandler::processEvent(QEvent::Type type, int cursor, InputContextMouseEvent *event)
{
    switch (type) {
    case QEvent::MouseButtonPress:
        emit pressed(cursor, event);
        break;
    case QEvent::MouseButtonRelease:
        emit released(cursor, event);
        break;
    case QEvent::MouseButtonDblClick:
        emit doubleClicked(cursor, event);
        break;
    case QEvent::MouseMove:
        emit positionChanged(cursor, event);
        break;
    default:
        break;
    }
}

/*!
    \qmlclass MouseFilter InputContextMouseFilter

    \inqmlmodule Qt.labs.inputcontext

    \brief The MouseFilter item provides mouse event filtering for input methods.

    The MouseFilter item can be used to filter mouse press, release, move and
    double click events received by the item with active focus.
*/

/*!
    \qmlsignal MouseHandler::onPressed(MouseEvent mouse)

    This handler is called when there is a press.  The \a mouse parameter holds
    information about the press.

    If the event is accepted it will not be delivered to the item.
*/

/*!
    \qmlsignal MouseHandler::onReleased(MouseEvent mouse)

    This handler is called when there is a release.  The \a mouse parameter
    holds information about the release.

    If the event is accepted it will not be delivered to the item.
*/

/*!
    \qmlsignal MouseHandler::onPositionChanged(MouseEvent mouse)

    This handler is called when the mouse position changes.

    The \a mouse parameter holds information about the position change.

    If the event is accepted it will not be delivered to the item.
*/

/*!
    \qmlsignal MouseHandler::onDoubleClicked(MouseEvent mouse)

    This handler is called when there is a double-click.  The \a mouse
    parameter holds information about the double-click.

    If the event is accepted it will not be delivered to the item.
*/

InputContextMouseFilter::InputContextMouseFilter(QObject *parent)
    : QObject(parent)
{
    if (DeclarativeInputContext *context = qobject_cast<DeclarativeInputContext *>(
            qApp->inputContext())) {
        context->registerMouseFilter(this);
    }
}

void InputContextMouseFilter::processEvent(QEvent::Type type, InputContextMouseEvent *event)
{
    switch (type) {
    case QEvent::MouseButtonPress:
        emit pressed(event);
        break;
    case QEvent::MouseButtonRelease:
        emit released(event);
        break;
    case QEvent::MouseButtonDblClick:
        emit doubleClicked(event);
        break;
    case QEvent::MouseMove:
        emit positionChanged(event);
        break;
    default:
        break;
    }
}

/*!
    \qmlclass KeyFilter InputContextKeyFilter

    \inqmlmodule Qt.labs.inputcontext

    \brief The KeyFilter item provides key event filtering for input methods.

    The KeyFilter item can be used to filter key press and releae events
    received by the item with active focus.
*/

/*!
    \qmlsignal KeyFilter::onPressed(KeyEvent event)

    This handler is called when there is a key press.  The \a event parameter
    holds information about the press.

    If the event is accepted it will not be delivered to the item.
*/

/*!
    \qmlsignal KeyFilter::onReleased(KeyEvent event)

    This handler is called when there is a key release.  The \a event parameter
    holds information about the release.

    If the event is accepted it will not be delivered to the item.
*/

InputContextKeyFilter::InputContextKeyFilter(QObject *parent)
    : QObject(parent)
{
    if (DeclarativeInputContext *context = qobject_cast<DeclarativeInputContext *>(
            qApp->inputContext())) {
        context->registerKeyFilter(this);
    }
}

void InputContextKeyFilter::processEvent(QEvent::Type type, InputContextKeyEvent *event)
{
    switch (type) {
    case QEvent::KeyPress:
        emit pressed(event);
        break;
    case QEvent::KeyRelease:
        emit released(event);
        break;
    default:
        break;
    }
}

QT_END_NAMESPACE
