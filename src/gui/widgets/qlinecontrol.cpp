/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include "qlinecontrol_p.h"

#ifndef QT_NO_LINEEDIT

#include "qabstractitemview.h"
#include "qclipboard.h"
#ifndef QT_NO_ACCESSIBILITY
#include "qaccessible.h"
#endif
#ifndef QT_NO_IM
#include "qinputcontext.h"
#include "qlist.h"
#endif
#include "qapplication.h"
#ifndef QT_NO_GRAPHICSVIEW
#include "qgraphicssceneevent.h"
#endif

QT_BEGIN_NAMESPACE

/*
    Updates the display text based of the current edit text
    If the text has changed will emit displayTextChanged()
*/
void QLineControl::updateDisplayText()
{
    QString orig = textLayout.text();
    QString str;
    if (m_echoMode == QLineEdit::NoEcho)
        str = QString::fromLatin1("");
    else
        str = m_text;

    if (m_echoMode == QLineEdit::Password || (m_echoMode == QLineEdit::PasswordEchoOnEdit
                && !m_passwordEchoEditing))
        str.fill(m_passwordCharacter);

    // replace certain non-printable characters with spaces (to avoid
    // drawing boxes when using fonts that don't have glyphs for such
    // characters)
    QChar* uc = str.data();
    for (int i = 0; i < (int)str.length(); ++i) {
        if ((uc[i] < 0x20 && uc[i] != 0x09)
            || uc[i] == QChar::LineSeparator
            || uc[i] == QChar::ParagraphSeparator
            || uc[i] == QChar::ObjectReplacementCharacter)
            uc[i] = QChar(0x0020);
    }

    textLayout.setText(str);

    QTextOption option;
    option.setTextDirection(m_layoutDirection);
    option.setFlags(QTextOption::IncludeTrailingSpaces);
    textLayout.setTextOption(option);

    textLayout.beginLayout();
    QTextLine l = textLayout.createLine();
    textLayout.endLayout();
    m_ascent = qRound(l.ascent());

    if (str != orig)
        emit displayTextChanged(str);
}

#ifndef QT_NO_CLIPBOARD
/*
    Copies the currently selected text into the clipboard using the given
    \a mode.
  
    \note If the echo mode is set to a mode other than Normal then copy
    will not work.  This is to prevent using copy as a method of bypassing
    password features of the line control.
*/
void QLineControl::copy(QClipboard::Mode mode) const
{
    QString t = selectedText();
    if (!t.isEmpty() && m_echoMode == QLineEdit::Normal) {
        disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()), this, 0);
        QApplication::clipboard()->setText(t, mode);
        connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                   this, SLOT(_q_clipboardChanged()));
    }
}

/*
    Inserts the text stored in the application clipboard into the line
    control.

    \sa insert()
*/
void QLineControl::paste()
{
    insert(QApplication::clipboard()->text(QClipboard::Clipboard));
}

#endif // !QT_NO_CLIPBOARD

/*
    Handles the behavior for the backspace key or function.
    Removes the current selection if there is a selection, otherwise
    removes the character prior to the cursor position.

    \sa del()
*/
void QLineControl::backspace()
{
    int priorState = undoState;
    if (hasSelectedText()) {
        removeSelectedText();
    } else if (m_cursor) {
            --m_cursor;
            if (maskData)
                m_cursor = prevMaskBlank(m_cursor);
            QChar uc = m_text.at(m_cursor);
            if (m_cursor > 0 && uc.unicode() >= 0xdc00 && uc.unicode() < 0xe000) {
                // second half of a surrogate, check if we have the first half as well,
                // if yes delete both at once
                uc = m_text.at(m_cursor - 1);
                if (uc.unicode() >= 0xd800 && uc.unicode() < 0xdc00) {
                    p_del(true);
                    --m_cursor;
                }
            }
            p_del(true);
    }
    finishChange(priorState);
}

/*
    Handles the behavior for the delete key or function.
    Removes the current selection if there is a selection, otherwise
    removes the character after the cursor position.

    \sa del()
*/
void QLineControl::del()
{
    int priorState = undoState;
    if (hasSelectedText()) {
        removeSelectedText();
    } else {
        int n = textLayout.nextCursorPosition(m_cursor) - m_cursor;
        while (n--)
            p_del();
    }
    finishChange(priorState);
}

/*
    Inserts the given \a newText at the current cursor position.
    If there is any selected text it is removed prior to insertion of
    the new text.
*/
void QLineControl::insert(const QString &newText)
{
    int priorState = undoState;
    removeSelectedText();
    p_insert(newText);
    finishChange(priorState);
}

/*
    Clears the line control text.
*/
void QLineControl::clear()
{
    int priorState = undoState;
    selstart = 0;
    selend = m_text.length();
    removeSelectedText();
    separate();
    finishChange(priorState, /*update*/false, /*edited*/false);
}

/*
    Sets \a length characters from the given \a start position as selected.
    The given \a start position must be within the current text for
    the line control.  If \a length characters cannot be selected, then
    the selection will extend to the end of the current text.
*/
void QLineControl::setSelection(int start, int length)
{
    if(start < 0 || start > (int)m_text.length()){
        qWarning("QLineControl::setSelection: Invalid start position");
        return;
    }

    if (length > 0) {
        selstart = start;
        selend = qMin(start + length, (int)m_text.length());
        m_cursor = selend;
    } else {
        selstart = qMax(start + length, 0);
        selend = start;
        m_cursor = selstart;
    }
}

void QLineControl::_q_clipboardChanged()
{
}

void QLineControl::_q_deleteSelected()
{
    if (!hasSelectedText())
        return;

    int priorState = undoState;
    emit resetInputContext();
    removeSelectedText();
    separate();
    finishChange(priorState);
}

/*
    Initializes the line control with a starting text value of \a txt.
*/
void QLineControl::init(const QString& txt)
{
    m_text = txt;
    updateDisplayText();
    m_cursor = m_text.length();
}

/*
    Sets the password echo editing to \a editing.  If password echo editing
    is true, then the text of the password is displayed even if the echo
    mode is set to QLineEdit::PasswordEchoOnEdit.  Password echoing editing
    does not affect other echo modes.
*/
void QLineControl::updatePasswordEchoEditing(bool editing)
{
    m_passwordEchoEditing = editing;
    updateDisplayText();
}

/*
    Returns the cursor position of the given \a x pixel value in relation
    to the displayed text.  The given \a betweenOrOn specified what kind
    of cursor position is requested.
*/
int QLineControl::xToPos(int x, QTextLine::CursorPosition betweenOrOn) const
{
    return textLayout.lineAt(0).xToCursor(x, betweenOrOn);
}

/*
    Returns the bounds of the current cursor, as defined as a
    between characters cursor.
*/
QRect QLineControl::cursorRect() const
{
    QTextLine l = textLayout.lineAt(0);
    int c = m_cursor;
    if (m_preeditCursor != -1)
        c += m_preeditCursor;
    int cix = qRound(l.cursorToX(c));
    int w = m_cursorWidth;
    int ch = l.height() + 1;

    return QRect(cix-5, 0, w+9, ch);
}

/*
    Fixes the current text so that it is valid given any set validators.

    Returns true if the text was changed.  Otherwise returns false.
*/
bool QLineControl::fixup() // this function assumes that validate currently returns != Acceptable
{
#ifndef QT_NO_VALIDATOR
    if (m_validator) {
        QString textCopy = m_text;
        int cursorCopy = m_cursor;
        m_validator->fixup(textCopy);
        if (m_validator->validate(textCopy, cursorCopy) == QValidator::Acceptable) {
            if (textCopy != m_text || cursorCopy != m_cursor)
                p_setText(textCopy, cursorCopy);
            return true;
        }
    }
#endif
    return false;
}

/*
    Moves the cursor to the given position \a pos.   If \a mark is true will
    adjust the currently selected text.
*/
void QLineControl::moveCursor(int pos, bool mark)
{
    if (pos != m_cursor) {
        separate();
        if (maskData)
            pos = pos > m_cursor ? nextMaskBlank(pos) : prevMaskBlank(pos);
    }
    if (mark) {
        int anchor;
        if (selend > selstart && m_cursor == selstart)
            anchor = selend;
        else if (selend > selstart && m_cursor == selend)
            anchor = selstart;
        else
            anchor = m_cursor;
        selstart = qMin(anchor, pos);
        selend = qMax(anchor, pos);
        updateDisplayText();
    } else {
        p_deselect();
    }
    m_cursor = pos;
    if (mark || selDirty) {
        selDirty = false;
        emit selectionChanged();
    }
    emitCursorPositionChanged();
}

/*
    Applies the given input method event \a event to the text of the line
    control
*/
void QLineControl::processInputMethodEvent(QInputMethodEvent *event)
{
    int priorState = undoState;
    removeSelectedText();

    int c = m_cursor; // cursor position after insertion of commit string
    if (event->replacementStart() <= 0)
        c += event->commitString().length() + qMin(-event->replacementStart(), event->replacementLength());

    m_cursor += event->replacementStart();

    // insert commit string
    if (event->replacementLength()) {
        selstart = m_cursor;
        selend = selstart + event->replacementLength();
        removeSelectedText();
    }
    if (!event->commitString().isEmpty())
        insert(event->commitString());

    m_cursor = qMin(c, m_text.length());

    setPreeditArea(m_cursor, event->preeditString());
    m_preeditCursor = event->preeditString().length();
    hideCursor = false;
    QList<QTextLayout::FormatRange> formats;
    for (int i = 0; i < event->attributes().size(); ++i) {
        const QInputMethodEvent::Attribute &a = event->attributes().at(i);
        if (a.type == QInputMethodEvent::Cursor) {
            m_preeditCursor = a.start;
            hideCursor = !a.length;
        } else if (a.type == QInputMethodEvent::TextFormat) {
            QTextCharFormat f = qvariant_cast<QTextFormat>(a.value).toCharFormat();
            if (f.isValid()) {
                QTextLayout::FormatRange o;
                o.start = a.start + m_cursor;
                o.length = a.length;
                o.format = f;
                formats.append(o);
            }
        }
    }
    textLayout.setAdditionalFormats(formats);
    updateDisplayText();
    if (!event->commitString().isEmpty())
        emitCursorPositionChanged();
    finishChange(priorState);
}

/*
    Draws the display text for the line control using the given 
    \a painter, \a clip, and \a offset.  Which aspects of the display text
    are drawn is specified by the given \a flags.

    If the flags contain DrawSelections, then the selection or input mask
    backgrounds and foregrounds will be applied before drawing the text.

    If the flags contain DrawCursor a cursor of the current cursorWidth()
    will be drawn after drawing the text.

    The display text will only be drawn if the flags contain DrawText
*/
void QLineControl::draw(QPainter *painter, const QPoint &offset, const QRect &clip, int flags)
{
    QVector<QTextLayout::FormatRange> selections;
    if (flags & DrawSelections) {
        QTextLayout::FormatRange o;
        if (selstart < selend) {
            o.start = selstart;
            o.length = selend - selstart;
            o.format.setBackground(m_palette.brush(QPalette::Highlight));
            o.format.setForeground(m_palette.brush(QPalette::HighlightedText));
        } else {
            // mask selection
            o.start = m_cursor;
            o.length = 1;
            o.format.setBackground(m_palette.brush(QPalette::Text));
            o.format.setForeground(m_palette.brush(QPalette::Window));
        }
        selections.append(o);
    }

    if (flags & DrawText)
        textLayout.draw(painter, offset, selections, clip);

    if (flags & DrawCursor){
        if(!m_blinkPeriod || m_blinkStatus)
            textLayout.drawCursor(painter, offset, m_cursor, m_cursorWidth);
    }
}

/*
    Sets the selection to cover the word at the given cursor position.
    The word boundries is defined by the behavior of QTextLayout::SkipWords
    cursor mode.
*/
void QLineControl::selectWordAtPos(int cursor)
{
    int c = textLayout.previousCursorPosition(cursor, QTextLayout::SkipWords);
    moveCursor(c, false);
    // ## text layout should support end of words.
    int end = textLayout.nextCursorPosition(cursor, QTextLayout::SkipWords);
    while (end > cursor && m_text[end-1].isSpace())
        --end;
    moveCursor(end, true);
}

/*
    Completes a change to the line control text.  If the change is not valid
    will undo the line control state back to the given \a validateFromState.

    If \a edited is true and the change is valid, will emit textEdited() in
    addition to textChanged().  Otherwise only emits textChanged() on a valid
    change.

    The \a update value is currently unused.
*/

bool QLineControl::finishChange(int validateFromState, bool update, bool edited)
{
    Q_UNUSED(update)
    bool lineDirty = selDirty;
    if (textDirty) {
        // do validation
        bool wasValidInput = validInput;
        validInput = true;
#ifndef QT_NO_VALIDATOR
        if (m_validator) {
            validInput = false;
            QString textCopy = m_text;
            int cursorCopy = m_cursor;
            validInput = (m_validator->validate(textCopy, cursorCopy) != QValidator::Invalid);
            if (validInput) {
                if (m_text != textCopy) {
                    p_setText(textCopy, cursorCopy);
                    return true;
                }
                m_cursor = cursorCopy;
            }
        }
#endif
        if (validateFromState >= 0 && wasValidInput && !validInput) {
            if (transactions.count())
                return false;
            p_undo(validateFromState);
            history.resize(undoState);
            if (modifiedState > undoState)
                modifiedState = -1;
            validInput = true;
            textDirty = false;
        }
        updateDisplayText();
        lineDirty |= textDirty;
        if (textDirty) {
            textDirty = false;
            QString actualText = text();
            if (edited)
                emit textEdited(actualText);
            emit textChanged(actualText);
        }
    }
    if (selDirty) {
        selDirty = false;
        emit selectionChanged();
    }
    emitCursorPositionChanged();
    return true;
}

/*
    An internal function for setting the text of the line control.
*/
void QLineControl::p_setText(const QString& txt, int pos, bool edited)
{
    p_deselect();
    emit resetInputContext();
    QString oldText = m_text;
    if (maskData) {
        m_text = maskString(0, txt, true);
        m_text += clearString(m_text.length(), m_maxLength - m_text.length());
    } else {
        m_text = txt.isEmpty() ? txt : txt.left(m_maxLength);
    }
    history.clear();
    modifiedState =  undoState = 0;
    m_cursor = (pos < 0 || pos > m_text.length()) ? m_text.length() : pos;
    textDirty = (oldText != m_text);
    finishChange(-1, true, edited);
}


/*
    Adds the given \a command to the undo history
    of the line control.  Does not apply the command.
*/
void QLineControl::addCommand(const Command& cmd)
{
    if (separator && undoState && history[undoState-1].type != Separator) {
        history.resize(undoState + 2);
        history[undoState++] = Command(Separator, m_cursor, 0, selstart, selend);
    } else {
        history.resize(undoState + 1);
    }
    separator = false;
    history[undoState++] = cmd;
}

/*
    Inserts the given string \a s into the line
    control.

    Also adds the appropriate commands into the undo history.
    This function does not call finishChange(), and may leave the text
    in an invalid state.
*/
void QLineControl::p_insert(const QString& s)
{
    if (hasSelectedText())
        addCommand(Command(SetSelection, m_cursor, 0, selstart, selend));
    if (maskData) {
        QString ms = maskString(m_cursor, s);
        for (int i = 0; i < (int) ms.length(); ++i) {
            addCommand (Command(DeleteSelection, m_cursor+i, m_text.at(m_cursor+i), -1, -1));
            addCommand(Command(Insert, m_cursor+i, ms.at(i), -1, -1));
        }
        m_text.replace(m_cursor, ms.length(), ms);
        m_cursor += ms.length();
        m_cursor = nextMaskBlank(m_cursor);
        textDirty = true;
    } else {
        int remaining = m_maxLength - m_text.length();
        if (remaining != 0) {
            m_text.insert(m_cursor, s.left(remaining));
            for (int i = 0; i < (int) s.left(remaining).length(); ++i)
               addCommand(Command(Insert, m_cursor++, s.at(i), -1, -1));
            textDirty = true;
        }
    }
}

/*
    \internal
    deletes a single character from the current text.  If \a wasBackspace,
    the character prior to the cursor is removed.  Otherwise the character
    after the cursor is removed.

    Also adds the appropriate commands into the undo history.
    This function does not call finishChange(), and may leave the text
    in an invalid state.
*/
void QLineControl::p_del(bool wasBackspace)
{
    if (m_cursor < (int) m_text.length()) {
        if (hasSelectedText())
            addCommand(Command(SetSelection, m_cursor, 0, selstart, selend));
        addCommand (Command((CommandType)((maskData?2:0)+(wasBackspace?Remove:Delete)), m_cursor, m_text.at(m_cursor), -1, -1));
        if (maskData) {
            m_text.replace(m_cursor, 1, clearString(m_cursor, 1));
            addCommand(Command(Insert, m_cursor, m_text.at(m_cursor), -1, -1));
        } else {
            m_text.remove(m_cursor, 1);
        }
        textDirty = true;
    }
}

/*
    \internal
    removes the currently selected text from the line control.

    Also adds the appropriate commands into the undo history.
    This function does not call finishChange(), and may leave the text
    in an invalid state.
*/
void QLineControl::removeSelectedText()
{
    if (selstart < selend && selend <= (int) m_text.length()) {
        separate();
        int i ;
        addCommand(Command(SetSelection, m_cursor, 0, selstart, selend));
        if (selstart <= m_cursor && m_cursor < selend) {
            // cursor is within the selection. Split up the commands
            // to be able to restore the correct cursor position
            for (i = m_cursor; i >= selstart; --i)
                addCommand (Command(DeleteSelection, i, m_text.at(i), -1, 1));
            for (i = selend - 1; i > m_cursor; --i)
                addCommand (Command(DeleteSelection, i - m_cursor + selstart - 1, m_text.at(i), -1, -1));
        } else {
            for (i = selend-1; i >= selstart; --i)
                addCommand (Command(RemoveSelection, i, m_text.at(i), -1, -1));
        }
        if (maskData) {
            m_text.replace(selstart, selend - selstart,  clearString(selstart, selend - selstart));
            for (int i = 0; i < selend - selstart; ++i)
                addCommand(Command(Insert, selstart + i, m_text.at(selstart + i), -1, -1));
        } else {
            m_text.remove(selstart, selend - selstart);
        }
        if (m_cursor > selstart)
            m_cursor -= qMin(m_cursor, selend) - selstart;
        p_deselect();
        textDirty = true;
    }
}

/*
    Parses the input mask specified by \a maskFields to generate
    the mask data used to handle input masks.
*/
void QLineControl::parseInputMask(const QString &maskFields)
{
    int delimiter = maskFields.indexOf(QLatin1Char(';'));
    if (maskFields.isEmpty() || delimiter == 0) {
        if (maskData) {
            delete [] maskData;
            maskData = 0;
            m_maxLength = 32767;
            p_setText(QString());
        }
        return;
    }

    if (delimiter == -1) {
        blank = QLatin1Char(' ');
        m_inputMask = maskFields;
    } else {
        m_inputMask = maskFields.left(delimiter);
        blank = (delimiter + 1 < maskFields.length()) ? maskFields[delimiter + 1] : QLatin1Char(' ');
    }

    // calculate m_maxLength / maskData length
    m_maxLength = 0;
    QChar c = 0;
    for (int i=0; i<m_inputMask.length(); i++) {
        c = m_inputMask.at(i);
        if (i > 0 && m_inputMask.at(i-1) == QLatin1Char('\\')) {
            m_maxLength++;
            continue;
        }
        if (c != QLatin1Char('\\') && c != QLatin1Char('!') &&
             c != QLatin1Char('<') && c != QLatin1Char('>') &&
             c != QLatin1Char('{') && c != QLatin1Char('}') &&
             c != QLatin1Char('[') && c != QLatin1Char(']'))
            m_maxLength++;
    }

    delete [] maskData;
    maskData = new MaskInputData[m_maxLength];

    MaskInputData::Casemode m = MaskInputData::NoCaseMode;
    c = 0;
    bool s;
    bool escape = false;
    int index = 0;
    for (int i = 0; i < m_inputMask.length(); i++) {
        c = m_inputMask.at(i);
        if (escape) {
            s = true;
            maskData[index].maskChar = c;
            maskData[index].separator = s;
            maskData[index].caseMode = m;
            index++;
            escape = false;
        } else if (c == QLatin1Char('<')) {
                m = MaskInputData::Lower;
        } else if (c == QLatin1Char('>')) {
            m = MaskInputData::Upper;
        } else if (c == QLatin1Char('!')) {
            m = MaskInputData::NoCaseMode;
        } else if (c != QLatin1Char('{') && c != QLatin1Char('}') && c != QLatin1Char('[') && c != QLatin1Char(']')) {
            switch (c.unicode()) {
            case 'A':
            case 'a':
            case 'N':
            case 'n':
            case 'X':
            case 'x':
            case '9':
            case '0':
            case 'D':
            case 'd':
            case '#':
            case 'H':
            case 'h':
            case 'B':
            case 'b':
                s = false;
                break;
            case '\\':
                escape = true;
            default:
                s = true;
                break;
            }

            if (!escape) {
                maskData[index].maskChar = c;
                maskData[index].separator = s;
                maskData[index].caseMode = m;
                index++;
            }
        }
    }
    p_setText(m_text);
}


/* checks if the key is valid compared to the inputMask */
bool QLineControl::isValidInput(QChar key, QChar mask) const
{
    switch (mask.unicode()) {
    case 'A':
        if (key.isLetter())
            return true;
        break;
    case 'a':
        if (key.isLetter() || key == blank)
            return true;
        break;
    case 'N':
        if (key.isLetterOrNumber())
            return true;
        break;
    case 'n':
        if (key.isLetterOrNumber() || key == blank)
            return true;
        break;
    case 'X':
        if (key.isPrint())
            return true;
        break;
    case 'x':
        if (key.isPrint() || key == blank)
            return true;
        break;
    case '9':
        if (key.isNumber())
            return true;
        break;
    case '0':
        if (key.isNumber() || key == blank)
            return true;
        break;
    case 'D':
        if (key.isNumber() && key.digitValue() > 0)
            return true;
        break;
    case 'd':
        if ((key.isNumber() && key.digitValue() > 0) || key == blank)
            return true;
        break;
    case '#':
        if (key.isNumber() || key == QLatin1Char('+') || key == QLatin1Char('-') || key == blank)
            return true;
        break;
    case 'B':
        if (key == QLatin1Char('0') || key == QLatin1Char('1'))
            return true;
        break;
    case 'b':
        if (key == QLatin1Char('0') || key == QLatin1Char('1') || key == blank)
            return true;
        break;
    case 'H':
        if (key.isNumber() || (key >= QLatin1Char('a') && key <= QLatin1Char('f')) || (key >= QLatin1Char('A') && key <= QLatin1Char('F')))
            return true;
        break;
    case 'h':
        if (key.isNumber() || (key >= QLatin1Char('a') && key <= QLatin1Char('f')) || (key >= QLatin1Char('A') && key <= QLatin1Char('F')) || key == blank)
            return true;
        break;
    default:
        break;
    }
    return false;
}

/*
    Returns true if the given text \a str is valid for any
    validator or input mask set for the line control.

    Otherwise returns false
*/
bool QLineControl::hasAcceptableInput(const QString &str) const
{
#ifndef QT_NO_VALIDATOR
    QString textCopy = str;
    int cursorCopy = m_cursor;
    if (m_validator && m_validator->validate(textCopy, cursorCopy)
        != QValidator::Acceptable)
        return false;
#endif

    if (!maskData)
        return true;

    if (str.length() != m_maxLength)
        return false;

    for (int i=0; i < m_maxLength; ++i) {
        if (maskData[i].separator) {
            if (str.at(i) != maskData[i].maskChar)
                return false;
        } else {
            if (!isValidInput(str.at(i), maskData[i].maskChar))
                return false;
        }
    }
    return true;
}

/*
  Applies the inputMask on \a str starting from position \a pos in the mask. \a clear
  specifies from where characters should be gotten when a separator is met in \a str - true means
  that blanks will be used, false that previous input is used.
  Calling this when no inputMask is set is undefined.
*/
QString QLineControl::maskString(uint pos, const QString &str, bool clear) const
{
    if (pos >= (uint)m_maxLength)
        return QString::fromLatin1("");

    QString fill;
    fill = clear ? clearString(0, m_maxLength) : m_text;

    int strIndex = 0;
    QString s = QString::fromLatin1("");
    int i = pos;
    while (i < m_maxLength) {
        if (strIndex < str.length()) {
            if (maskData[i].separator) {
                s += maskData[i].maskChar;
                if (str[(int)strIndex] == maskData[i].maskChar)
                    strIndex++;
                ++i;
            } else {
                if (isValidInput(str[(int)strIndex], maskData[i].maskChar)) {
                    switch (maskData[i].caseMode) {
                    case MaskInputData::Upper:
                        s += str[(int)strIndex].toUpper();
                        break;
                    case MaskInputData::Lower:
                        s += str[(int)strIndex].toLower();
                        break;
                    default:
                        s += str[(int)strIndex];
                    }
                    ++i;
                } else {
                    // search for separator first
                    int n = findInMask(i, true, true, str[(int)strIndex]);
                    if (n != -1) {
                        if (str.length() != 1 || i == 0 || (i > 0 && (!maskData[i-1].separator || maskData[i-1].maskChar != str[(int)strIndex]))) {
                            s += fill.mid(i, n-i+1);
                            i = n + 1; // update i to find + 1
                        }
                    } else {
                        // search for valid blank if not
                        n = findInMask(i, true, false, str[(int)strIndex]);
                        if (n != -1) {
                            s += fill.mid(i, n-i);
                            switch (maskData[n].caseMode) {
                            case MaskInputData::Upper:
                                s += str[(int)strIndex].toUpper();
                                break;
                            case MaskInputData::Lower:
                                s += str[(int)strIndex].toLower();
                                break;
                            default:
                                s += str[(int)strIndex];
                            }
                            i = n + 1; // updates i to find + 1
                        }
                    }
                }
                strIndex++;
            }
        } else
            break;
    }

    return s;
}



/*
  Returns a "cleared" string with only separators and blank chars.
  Calling this when no inputMask is set is undefined.
*/
QString QLineControl::clearString(uint pos, uint len) const
{
    if (pos >= (uint)m_maxLength)
        return QString();

    QString s;
    int end = qMin((uint)m_maxLength, pos + len);
    for (int i=pos; i<end; i++)
        if (maskData[i].separator)
            s += maskData[i].maskChar;
        else
            s += blank;

    return s;
}

/*
  Strips blank parts of the input in a QLineControl when an inputMask is set,
  separators are still included. Typically "127.0__.0__.1__" becomes "127.0.0.1".
*/
QString QLineControl::stripString(const QString &str) const
{
    if (!maskData)
        return str;

    QString s;
    int end = qMin(m_maxLength, (int)str.length());
    for (int i=0; i < end; i++)
        if (maskData[i].separator)
            s += maskData[i].maskChar;
        else
            if (str[i] != blank)
                s += str[i];

    return s;
}

/* searches forward/backward in maskData for either a separator or a blank */
int QLineControl::findInMask(int pos, bool forward, bool findSeparator, QChar searchChar) const
{
    if (pos >= m_maxLength || pos < 0)
        return -1;

    int end = forward ? m_maxLength : -1;
    int step = forward ? 1 : -1;
    int i = pos;

    while (i != end) {
        if (findSeparator) {
            if (maskData[i].separator && maskData[i].maskChar == searchChar)
                return i;
        } else {
            if (!maskData[i].separator) {
                if (searchChar.isNull())
                    return i;
                else if (isValidInput(searchChar, maskData[i].maskChar))
                    return i;
            }
        }
        i += step;
    }
    return -1;
}

void QLineControl::p_undo(int until)
{
    if (!isUndoAvailable())
        return;
    p_deselect();
    while (undoState && undoState > until) {
        Command& cmd = history[--undoState];
        switch (cmd.type) {
        case Insert:
            m_text.remove(cmd.pos, 1);
            m_cursor = cmd.pos;
            break;
        case SetSelection:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            m_cursor = cmd.pos;
            break;
        case Remove:
        case RemoveSelection:
            m_text.insert(cmd.pos, cmd.uc);
            m_cursor = cmd.pos + 1;
            break;
        case Delete:
        case DeleteSelection:
            m_text.insert(cmd.pos, cmd.uc);
            m_cursor = cmd.pos;
            break;
        case Separator:
            continue;
        }
        if (until < 0 && undoState) {
            Command& next = history[undoState-1];
            if (next.type != cmd.type && next.type < RemoveSelection
                 && (cmd.type < RemoveSelection || next.type == Separator))
                break;
        }
    }
    textDirty = true;
    emitCursorPositionChanged();
}

void QLineControl::p_redo() {
    if (!isRedoAvailable())
        return;
    p_deselect();
    while (undoState < (int)history.size()) {
        Command& cmd = history[undoState++];
        switch (cmd.type) {
        case Insert:
            m_text.insert(cmd.pos, cmd.uc);
            m_cursor = cmd.pos + 1;
            break;
        case SetSelection:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            m_cursor = cmd.pos;
            break;
        case Remove:
        case Delete:
        case RemoveSelection:
        case DeleteSelection:
            m_text.remove(cmd.pos, 1);
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            m_cursor = cmd.pos;
            break;
        case Separator:
            selstart = cmd.selStart;
            selend = cmd.selEnd;
            m_cursor = cmd.pos;
            break;
        }
        if (undoState < (int)history.size()) {
            Command& next = history[undoState];
            if (next.type != cmd.type && cmd.type < RemoveSelection && next.type != Separator
                 && (next.type < RemoveSelection || cmd.type == Separator))
                break;
        }
    }
    textDirty = true;
    emitCursorPositionChanged();
}

/*
    If the current cursor position differs from the last emited cursor
    position, emits cursorPositionChanged().
*/
void QLineControl::emitCursorPositionChanged()
{
    if (m_cursor != lastCursorPos) {
        const int oldLast = lastCursorPos;
        lastCursorPos = m_cursor;
        cursorPositionChanged(oldLast, m_cursor);
    }
}

#ifndef QT_NO_COMPLETER
// iterating forward(dir=1)/backward(dir=-1) from the
// current row based. dir=0 indicates a new completion prefix was set.
bool QLineControl::advanceToEnabledItem(int dir)
{
    int start = m_completer->currentRow();
    if (start == -1)
        return false;
    int i = start + dir;
    if (dir == 0) dir = 1;
    do {
        if (!m_completer->setCurrentRow(i)) {
            if (!m_completer->wrapAround())
                break;
            i = i > 0 ? 0 : m_completer->completionCount() - 1;
        } else {
            QModelIndex currentIndex = m_completer->currentIndex();
            if (m_completer->completionModel()->flags(currentIndex) & Qt::ItemIsEnabled)
                return true;
            i += dir;
        }
    } while (i != start);

    m_completer->setCurrentRow(start); // restore
    return false;
}

void QLineControl::complete(int key)
{
    if (!m_completer || isReadOnly() || echoMode() != QLineEdit::Normal)
        return;

    QString text = this->text();
    if (m_completer->completionMode() == QCompleter::InlineCompletion) {
        if (key == Qt::Key_Backspace)
            return;
        int n = 0;
        if (key == Qt::Key_Up || key == Qt::Key_Down) {
            if (textAfterSelection().length())
                return;
            QString prefix = hasSelectedText() ? textBeforeSelection()
                : text;
            if (text.compare(m_completer->currentCompletion(), m_completer->caseSensitivity()) != 0
                || prefix.compare(m_completer->completionPrefix(), m_completer->caseSensitivity()) != 0) {
                m_completer->setCompletionPrefix(prefix);
            } else {
                n = (key == Qt::Key_Up) ? -1 : +1;
            }
        } else {
            m_completer->setCompletionPrefix(text);
        }
        if (!advanceToEnabledItem(n))
            return;
    } else {
#ifndef QT_KEYPAD_NAVIGATION
        if (text.isEmpty()) {
            m_completer->popup()->hide();
            return;
        }
#endif
        m_completer->setCompletionPrefix(text);
    }

    m_completer->complete();
}
#endif

void QLineControl::setCursorBlinkPeriod(int msec)
{
    if(msec == m_blinkPeriod)
        return;
    if(m_blinkTimer){
        killTimer(m_blinkTimer);
    }
    if(msec){
        m_blinkTimer = startTimer(msec/2);
        m_blinkStatus = 1;
    }else{
        m_blinkTimer = 0;
        if(m_blinkStatus == 0)
            emit updateNeeded(inputMask().isEmpty()?cursorRect():QRect());
    }
    m_blinkPeriod = msec;
}

void QLineControl::timerEvent ( QTimerEvent * event )
{
    if(event->timerId() == m_blinkTimer) {
        m_blinkStatus = !m_blinkStatus;
        emit updateNeeded(inputMask().isEmpty()?cursorRect():QRect());
    }else if(event->timerId() == m_deleteAllTimer){
        killTimer(m_deleteAllTimer);
        m_deleteAllTimer = 0;
        clear();
    }else if(event->timerId() == m_tripleClickTimer){
        killTimer(m_tripleClickTimer);
        m_tripleClickTimer = 0;
    }
}

bool QLineControl::processEvent(QEvent* ev)
{
#ifdef QT_KEYPAD_NAVIGATION
    if (QApplication::keypadNavigationEnabled()) {
        if ((ev->type() == QEvent::KeyPress) || (ev->type() == QEvent::KeyRelease)) {
            QKeyEvent *ke = (QKeyEvent *)ev;
            if (ke->key() == Qt::Key_Back) {
                if (ke->isAutoRepeat()) {
                    // Swallow it. We don't want back keys running amok.
                    ke->accept();
                    return true;
                }
                if ((ev->type() == QEvent::KeyRelease)
                    && !isReadOnly()
                    && deleteAllTimer) {
                    killTimer(m_deleteAllTimer);
                    m_deleteAllTimer = 0;
                    backspace();
                    ke->accept();
                    return true;
                }
            }
        }
    }
#endif
    switch(ev->type()){
#ifndef QT_NO_GRAPHICSVIEW
        case QEvent::GraphicsSceneMouseMove:
        case QEvent::GraphicsSceneMouseRelease:
        case QEvent::GraphicsSceneMousePress:{
               QGraphicsSceneMouseEvent *gvEv = static_cast<QGraphicsSceneMouseEvent*>(ev);
               QMouseEvent* mouse = new QMouseEvent(ev->type(),
                    gvEv->pos().toPoint(), gvEv->button(), gvEv->buttons(), gvEv->modifiers());
               processMouseEvent(mouse); break;
        }
#endif
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
            processMouseEvent(static_cast<QMouseEvent*>(ev)); break;
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            processKeyEvent(static_cast<QKeyEvent*>(ev)); break;
        case QEvent::InputMethod:
            processInputMethodEvent(static_cast<QInputMethodEvent*>(ev)); break;
#ifndef QT_NO_SHORTCUT
        case QEvent::ShortcutOverride:{
            QKeyEvent* ke = static_cast<QKeyEvent*>(ev);
            if (ke == QKeySequence::Copy
                || ke == QKeySequence::Paste
                || ke == QKeySequence::Cut
                || ke == QKeySequence::Redo
                || ke == QKeySequence::Undo
                || ke == QKeySequence::MoveToNextWord
                || ke == QKeySequence::MoveToPreviousWord
                || ke == QKeySequence::MoveToStartOfDocument
                || ke == QKeySequence::MoveToEndOfDocument
                || ke == QKeySequence::SelectNextWord
                || ke == QKeySequence::SelectPreviousWord
                || ke == QKeySequence::SelectStartOfLine
                || ke == QKeySequence::SelectEndOfLine
                || ke == QKeySequence::SelectStartOfBlock
                || ke == QKeySequence::SelectEndOfBlock
                || ke == QKeySequence::SelectStartOfDocument
                || ke == QKeySequence::SelectAll
                || ke == QKeySequence::SelectEndOfDocument) {
                ke->accept();
            } else if (ke->modifiers() == Qt::NoModifier || ke->modifiers() == Qt::ShiftModifier
                       || ke->modifiers() == Qt::KeypadModifier) {
                if (ke->key() < Qt::Key_Escape) {
                    ke->accept();
                } else {
                    switch (ke->key()) {
                    case Qt::Key_Delete:
                    case Qt::Key_Home:
                    case Qt::Key_End:
                    case Qt::Key_Backspace:
                    case Qt::Key_Left:
                    case Qt::Key_Right:
                        ke->accept();
                    default:
                        break;
                    }
                }
            }
        }
#endif
        default:
            return false;
    }
    return true;
}

void QLineControl::processMouseEvent(QMouseEvent* ev)
{

    switch (ev->type()){
        case QEvent::GraphicsSceneMousePress:
        case QEvent::MouseButtonPress:{
            if (m_tripleClickTimer && (ev->pos() - m_tripleClick).manhattanLength() <
                QApplication::startDragDistance()) {
                selectAll();
                return;
            }
            if (ev->button() == Qt::RightButton)
                return;

            bool mark = ev->modifiers() & Qt::ShiftModifier;
            int cursor = xToPos(ev->pos().x());
            moveCursor(cursor, mark);
            break;
        }
        case QEvent::MouseButtonDblClick:
            if (ev->button() == Qt::LeftButton) {
                selectWordAtPos(xToPos(ev->pos().x()));
                if(m_tripleClickTimer)
                    killTimer(m_tripleClickTimer);
                m_tripleClickTimer = startTimer(QApplication::doubleClickInterval());
                m_tripleClick = ev->pos();
            }
            break;
        case QEvent::GraphicsSceneMouseRelease:
        case QEvent::MouseButtonRelease:
#ifndef QT_NO_CLIPBOARD
            if (QApplication::clipboard()->supportsSelection()) {
                if (ev->button() == Qt::LeftButton) {
                    copy(QClipboard::Selection);
                } else if (!isReadOnly() && ev->button() == Qt::MidButton) {
                    deselect();
                    insert(QApplication::clipboard()->text(QClipboard::Selection));
                }
            }
#endif
            break;
        case QEvent::GraphicsSceneMouseMove:
        case QEvent::MouseMove:
            if (ev->buttons() & Qt::LeftButton) {
                    moveCursor(xToPos(ev->pos().x()), true);
            }
            break;
        default:
            break;
    }
}

void QLineControl::processKeyEvent(QKeyEvent* event)
{

    bool inlineCompletionAccepted = false;

#ifndef QT_NO_COMPLETER
    if (m_completer) {
        QCompleter::CompletionMode completionMode = m_completer->completionMode();
        if ((completionMode == QCompleter::PopupCompletion
             || completionMode == QCompleter::UnfilteredPopupCompletion)
            && m_completer->popup()
            && m_completer->popup()->isVisible()) {
            // The following keys are forwarded by the completer to the widget
            // Ignoring the events lets the completer provide suitable default behavior
            switch (event->key()) {
            case Qt::Key_Escape:
                event->ignore();
                return;
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Select:
                if (!QApplication::keypadNavigationEnabled())
                    break;
#endif
                m_completer->popup()->hide(); // just hide. will end up propagating to parent
            default:
                break; // normal key processing
            }
        } else if (completionMode == QCompleter::InlineCompletion) {
            switch (event->key()) {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_F4:
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Select:
                if (!QApplication::keypadNavigationEnabled())
                    break;
#endif
                if (!m_completer->currentCompletion().isEmpty() && hasSelectedText()
                    && textAfterSelection().isEmpty()) {
                    setText(m_completer->currentCompletion());
                    inlineCompletionAccepted = true;
                }
            default:
                break; // normal key processing
            }
        }
    }
#endif // QT_NO_COMPLETER



    if (echoMode() == QLineEdit::PasswordEchoOnEdit
        && !passwordEchoEditing()
        && !isReadOnly()
        && !event->text().isEmpty()
#ifdef QT_KEYPAD_NAVIGATION
        && event->key() != Qt::Key_Select
        && event->key() != Qt::Key_Up
        && event->key() != Qt::Key_Down
        && event->key() != Qt::Key_Back
#endif
        && !(event->modifiers() & Qt::ControlModifier)) {
        // Clear the edit and reset to normal echo mode while editing; the
        // echo mode switches back when the edit loses focus
        // ### resets current content.  dubious code; you can
        // navigate with keys up, down, back, and select(?), but if you press
        // "left" or "right" it clears?
        updatePasswordEchoEditing(true);
        clear();
    }

    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (hasAcceptableInput() || fixup()) {
            emit accepted();
            emitingEditingFinished = true;
            emit editingFinished();
            emitingEditingFinished = false;
        }
        if (inlineCompletionAccepted)
            event->accept();
        else
            event->ignore();
        return;
    }
    bool unknown = false;

    if (false) {
    }
#ifndef QT_NO_SHORTCUT
    else if (event == QKeySequence::Undo) {
        if (!isReadOnly())
            undo();
    }
    else if (event == QKeySequence::Redo) {
        if (!isReadOnly())
            redo();
    }
    else if (event == QKeySequence::SelectAll) {
        selectAll();
    }
#ifndef QT_NO_CLIPBOARD
    else if (event == QKeySequence::Copy) {
        copy();
    }
    else if (event == QKeySequence::Paste) {
        if (!isReadOnly())
            paste();
    }
    else if (event == QKeySequence::Cut) {
        if (!isReadOnly()) {
            copy();
            del();
        }
    }
    else if (event == QKeySequence::DeleteEndOfLine) {
        if (!isReadOnly()) {
            setSelection(cursor(), end());
            copy();
            del();
        }
    }
#endif //QT_NO_CLIPBOARD
    else if (event == QKeySequence::MoveToStartOfLine) {
        home(0);
    }
    else if (event == QKeySequence::MoveToEndOfLine) {
        end(0);
    }
    else if (event == QKeySequence::SelectStartOfLine) {
        home(1);
    }
    else if (event == QKeySequence::SelectEndOfLine) {
        end(1);
    }
    else if (event == QKeySequence::MoveToNextChar) {
#if !defined(Q_WS_WIN) || defined(QT_NO_COMPLETER)
        if (hasSelectedText()) {
#else
        if (hasSelectedText() && m_completer
            && m_completer->completionMode() == QCompleter::InlineCompletion) {
#endif
            moveCursor(selectionEnd(), false);
        } else {
            cursorForward(0, layoutDirection() == Qt::LeftToRight ? 1 : -1);
        }
    }
    else if (event == QKeySequence::SelectNextChar) {
        cursorForward(1, layoutDirection() == Qt::LeftToRight ? 1 : -1);
    }
    else if (event == QKeySequence::MoveToPreviousChar) {
#if !defined(Q_WS_WIN) || defined(QT_NO_COMPLETER)
        if (hasSelectedText()) {
#else
        if (hasSelectedText() && m_completer
            && m_completer->completionMode() == QCompleter::InlineCompletion) {
#endif
            moveCursor(selectionStart(), false);
        } else {
            cursorForward(0, layoutDirection() == Qt::LeftToRight ? -1 : 1);
        }
    }
    else if (event == QKeySequence::SelectPreviousChar) {
        cursorForward(1, layoutDirection() == Qt::LeftToRight ? -1 : 1);
    }
    else if (event == QKeySequence::MoveToNextWord) {
        if (echoMode() == QLineEdit::Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordForward(0) : cursorWordBackward(0);
        else
            layoutDirection() == Qt::LeftToRight ? end(0) : home(0);
    }
    else if (event == QKeySequence::MoveToPreviousWord) {
        if (echoMode() == QLineEdit::Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordBackward(0) : cursorWordForward(0);
        else if (!isReadOnly()) {
            layoutDirection() == Qt::LeftToRight ? home(0) : end(0);
        }
    }
    else if (event == QKeySequence::SelectNextWord) {
        if (echoMode() == QLineEdit::Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordForward(1) : cursorWordBackward(1);
        else
            layoutDirection() == Qt::LeftToRight ? end(1) : home(1);
    }
    else if (event == QKeySequence::SelectPreviousWord) {
        if (echoMode() == QLineEdit::Normal)
            layoutDirection() == Qt::LeftToRight ? cursorWordBackward(1) : cursorWordForward(1);
        else
            layoutDirection() == Qt::LeftToRight ? home(1) : end(1);
    }
    else if (event == QKeySequence::Delete) {
        if (!isReadOnly())
            del();
    }
    else if (event == QKeySequence::DeleteEndOfWord) {
        if (!isReadOnly()) {
            cursorWordForward(true);
            del();
        }
    }
    else if (event == QKeySequence::DeleteStartOfWord) {
        if (!isReadOnly()) {
            cursorWordBackward(true);
            del();
        }
    }
#endif // QT_NO_SHORTCUT
    else {
#ifdef Q_WS_MAC
        if (event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
            Qt::KeyboardModifiers myModifiers = (event->modifiers() & ~Qt::KeypadModifier);
            if (myModifiers & Qt::ShiftModifier) {
                if (myModifiers == (Qt::ControlModifier|Qt::ShiftModifier)
                        || myModifiers == (Qt::AltModifier|Qt::ShiftModifier)
                        || myModifiers == Qt::ShiftModifier) {

                    event->key() == Qt::Key_Up ? home(1) : end(1);
                }
            } else {
                if ((myModifiers == Qt::ControlModifier
                     || myModifiers == Qt::AltModifier
                     || myModifiers == Qt::NoModifier)) {
                    event->key() == Qt::Key_Up ? home(0) : end(0);
                }
            }
        }
#endif
        if (event->modifiers() & Qt::ControlModifier) {
            switch (event->key()) {
            case Qt::Key_Backspace:
                if (!isReadOnly()) {
                    cursorWordBackward(true);
                    del();
                }
                break;
#ifndef QT_NO_COMPLETER
            case Qt::Key_Up:
            case Qt::Key_Down:
                complete(event->key());
                break;
#endif
#if defined(Q_WS_X11)
            case Qt::Key_E:
                end(0);
                break;

            case Qt::Key_U:
                if (!isReadOnly()) {
                    setSelection(0, text().size());
#ifndef QT_NO_CLIPBOARD
                    copy();
#endif
                    del();
                }
            break;
#endif
            default:
                unknown = true;
            }
        } else { // ### check for *no* modifier
            switch (event->key()) {
            case Qt::Key_Backspace:
                if (!isReadOnly()) {
                    backspace();
#ifndef QT_NO_COMPLETER
                    complete(Qt::Key_Backspace);
#endif
                }
                break;
#ifdef QT_KEYPAD_NAVIGATION
            case Qt::Key_Back:
                if (QApplication::keypadNavigationEnabled() && !event->isAutoRepeat()
                    && !isReadOnly()) {
                    if (text().length() == 0) {
                        setText(m_cancelText);

                        if (passwordEchoEditing)
                            updatePasswordEchoEditing(false);

                        setEditFocus(false);
                    } else if (!deleteAllTimer) {
                        deleteAllTimer = startTimer(750);
                    }
                } else {
                    unknown = true;
                }
                break;
#endif

            default:
                unknown = true;
            }
        }
    }

    if (event->key() == Qt::Key_Direction_L || event->key() == Qt::Key_Direction_R) {
        setLayoutDirection((event->key() == Qt::Key_Direction_L) ? Qt::LeftToRight : Qt::RightToLeft);
        unknown = false;
    }

    if (unknown && !isReadOnly()) {
        QString t = event->text();
        if (!t.isEmpty() && t.at(0).isPrint() &&
                ((event->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::NoModifier)) {
            insert(t);
#ifndef QT_NO_COMPLETER
            complete(event->key());
#endif
            event->accept();
            return;
        }
    }

    if (unknown)
        event->ignore();
    else
        event->accept();
}


QT_END_NAMESPACE

#endif
