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

#include "inputcontextmodule.h"

#include "declarativeinputcontext.h"

#include <QtCore/qdebug.h>
#include <QtGui/qapplication.h>
#include <QtGui/qevent.h>
#include <QtGui/qtextformat.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlmodule Qt.labs.inputcontext InputContextModule

    \brief The Qt.labs.inputcontext module provides an API for implementing input methods is QML.
*/

InputContextModule::InputContextModule(QObject *parent)
    : QObject(parent)
    , m_inputContext(qobject_cast<DeclarativeInputContext *>(qApp->inputContext()))
    , m_focusWidget(m_inputContext ? m_inputContext->focusWidget() : 0)
    , m_visible(false)
{
    if (m_inputContext)
        m_inputContext->setModule(this);
}

InputContextModule::~InputContextModule()
{
    if (m_inputContext)
        m_inputContext->setModule(0);
}

/*!
    \qmlproperty bool focus

    \inqmlmodule Qt.labs.inputcontext

    This property identifies whether an item that takes text input has active focus.
*/

bool InputContextModule::hasFocus() const
{
    return m_focusWidget != 0;
}

void InputContextModule::setFocusWidget(QWidget *widget)
{
    m_focusWidget = widget;

    if (!m_focusWidget)
        setVisible(false);

    emit focusChanged();
}

/*!
    \qmlproperty bool softwareInputPanelVisible

    \inqmlmodule Qt.labs.inputcontext

    This property identifies whether the item with focus has requested a
    software input panel.
*/

bool InputContextModule::isVisible() const
{
    return m_visible;
}

void InputContextModule::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;

        emit visibleChanged();
    }
}

/*!
    \qmlproperty string preeditText

    \inqmlmodule Qt.labs.inputcontext

    This property holds the uncommited text that is displayed in the item that
    has focus.
*/

QString InputContextModule::preeditText() const
{
    return m_preeditText;
}

void InputContextModule::setPreeditText(const QString &text)
{
    if (text != m_preeditText)
        sendPreedit(text);
}

/*!
    \qmlproperty rectangle microFocus

    \inqmlmodule Qt.labs.inputcontext

    This property holds a rectangle in scene coordinates around the position
    of the cursor.
*/

QRect InputContextModule::microFocus() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImMicroFocus).toRect()
            : QRect();
}

/*!
    \qmlproperty font font

    \inqmlmodule Qt.labs.inputcontext

    This property holds the font of the text that currently has focus.
*/

QFont InputContextModule::font() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImFont).value<QFont>()
            : QFont();
}

/*!
    \qmlproperty int cursorPosition

    \inqmlmodule Qt.labs.inputcontext

    This property holds the position of the text cursor in the
    \l surroundingText.
*/

int InputContextModule::cursorPosition() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImCursorPosition).toInt()
            : 0;
}

/*!
    \qmlproperty int anchorPosition

    \inqmlmodule Qt.labs.inputcontext

    This property holds the position of the selection anchor in the
    \l surroundingText.  If no text is selected this is the same as the
    \l cursorPosition.
*/

int InputContextModule::anchorPosition() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImAnchorPosition).toInt()
            : 0;
}

/*!
    \qmlproperty int maximumTextLength

    \inqmlmodule Qt.labs.inputcontext

    This property holds the maximum number of characters that the item with
    focus can hold.  If there is no limit -1 is returned.
*/

int InputContextModule::maximumTextLength() const
{
    QVariant length;
    if (m_focusWidget)
        length = m_focusWidget->inputMethodQuery(Qt::ImMaximumTextLength);
    return length.isValid() ? length.toInt() : -1;
}

/*!
    \qmlproperty string surroundingText

    \inqmlmodule Qt.labs.inputcontext

    This property holds the plain text around the input area.  For example the
    current paragraph.
*/

QString InputContextModule::surroundingText() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImSurroundingText).toString()
            : QString();
}

/*!
    \qmlproperty string selectedText

    \inqmlmodule Qt.labs.inputcontext

    This property holds the currently selected text.
*/

QString InputContextModule::selectedText() const
{
    return m_focusWidget
            ? m_focusWidget->inputMethodQuery(Qt::ImCurrentSelection).toString()
            : QString();
}

/*!
    \qmlmethod sendKeyPress(int key, string text, int modifiers, bool autoRepeat)

    \inqmlmodule Qt.labs.inputcontext

    This method sends a key press event to the item that currently has focus.

    Int key is the code for the Qt::Key that the event loop should listen for.
    If key is 0, the event is not a result of a known key; for example, it may
    be the result of a compose sequence or keyboard macro. The modifiers holds
    the keyboard modifiers, and the given text is the Unicode text that the key
    generated. If autorep is true, isAutoRepeat() will be true. count is the
    number of keys involved in the event.
*/
void InputContextModule::sendKeyPress(
        int key, const QString &text, int modifiers, bool autoRepeat, int count)
{
    if (m_focusWidget) {
        QKeyEvent event(
                QEvent::KeyPress, key, Qt::KeyboardModifiers(modifiers), text, autoRepeat, count);
        if (!m_inputContext || !m_inputContext->filterKeyEvent(&event))
            QApplication::sendEvent(m_focusWidget, &event);
    }
}

/*!
    \qmlmethod sendKeyRelease(int key, string text, int modifiers)

    \inqmlmodule Qt.labs.inputcontext

    This method sends a key release event to the item that currently has focus.

    Int key is the code for the Qt::Key that the event loop should listen for.
    If key is 0, the event is not a result of a known key; for example, it may
    be the result of a compose sequence or keyboard macro. The modifiers holds
    the keyboard modifiers, and the given text is the Unicode text that the key
    generated. count is the number of keys involved in the event.
*/
void InputContextModule::sendKeyRelease(int key, const QString &text, int modifiers, int count)
{
    if (m_focusWidget) {
        QKeyEvent event(
                QEvent::KeyRelease, key, Qt::KeyboardModifiers(modifiers), text, false, count);
        if (!m_inputContext || !m_inputContext->filterKeyEvent(&event))
            QApplication::sendEvent(m_focusWidget, &event);
    }
}

/*!
    \qmlmethod sendPreedit(string text, int cursor = -1)

    \inqmlmodule Qt.labs.inputcontext

    Sends a pre-edit event to the item with active focus.

    This will set \l preeditText to \a text, and position the text \a cursor
    within the pre-edit text.  If the value of cursor is -1 the cursor will be
    positioned at the end of the pre-edit text.
*/
void InputContextModule::sendPreedit(const QString &text, int cursor)
{
    const QString preedit = m_preeditText;
    m_preeditText = text;

    if (m_inputContext) {
        QList<QInputMethodEvent::Attribute> attributes;

        if (cursor >= 0 && cursor <= text.length()) {
            attributes.append(QInputMethodEvent::Attribute(
                    QInputMethodEvent::Cursor, cursor, text.length(), QVariant()));
        } else {
            cursor = text.length();
        }

        if (cursor > 0) {
            attributes.append(QInputMethodEvent::Attribute(
                    QInputMethodEvent::TextFormat,
                    0,
                    cursor,
                    m_inputContext->standardFormat(QInputContext::PreeditFormat)));
        }
        if (cursor < text.length()) {
            attributes.append(QInputMethodEvent::Attribute(
                    QInputMethodEvent::TextFormat,
                    cursor,
                    text.length(),
                    m_inputContext->standardFormat(QInputContext::SelectionFormat)));
        }

        m_inputContext->sendEvent(QInputMethodEvent(text, attributes));
    }

    if (m_preeditText != preedit)
        emit preeditTextChanged();
}


/*!
    \qmlmethod commit()

    \inqmlmodule Qt.labs.inputcontext

    Commits \l preeditText to the item with active focus.
*/
void InputContextModule::commit()
{
    // Create an explicit copy of m_preeditText as the reference value is cleared before sending
    // the event.
    commit(QString(m_preeditText));
}

/*!
    \qmlmethod commit(string)

    \inqmlmodule Qt.labs.inputcontext

    Commits \a text to the item with active focus and clears the current
    \l preeditText.  The text will be inserted into the \l surroundingText at a
    position \a replacementStart relative to the \l cursorPosition and will
    replace \a replacementLength characters.
*/
void InputContextModule::commit(const QString &text, int replacementStart, int replacementLength)
{
    const QString preedit = m_preeditText;
    m_preeditText.clear();

    if (m_inputContext) {
        QInputMethodEvent inputEvent;
        inputEvent.setCommitString(text, replacementStart, replacementLength);
        m_inputContext->sendEvent(inputEvent);
    }

    if (m_preeditText != preedit)
        emit preeditTextChanged();
}

/*!
    \qmlmethod clear()

    \inqmlmodule Qt.labs.inputcontext

    Clears the current \l preeditText.
*/
void InputContextModule::clear()
{
    const QString preedit = m_preeditText;
    m_preeditText.clear();

    if (m_inputContext)
        m_inputContext->sendEvent(QInputMethodEvent());

    if (m_preeditText != preedit)
        emit preeditTextChanged();
}

void InputContextModule::update()
{
    emit contextUpdated();
}

QT_END_NAMESPACE
