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

#ifndef QLINECONTROL_P_H
#define QLINECONTROL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qglobal.h"

#ifndef QT_NO_LINEEDIT
#include "private/qwidget_p.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qtextlayout.h"
#include "QtGui/qstyleoption.h"
#include "QtCore/qpointer.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qclipboard.h"
#include "QtCore/qpoint.h"
#include "QtGui/qcompleter.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QLineControl : public QObject
{
    Q_OBJECT
public:

    QLineControl(const QString &txt = QString())
        : emitingEditingFinished(0),
        m_cursor(0), m_preeditCursor(0), m_layoutDirection(Qt::LeftToRight),
        hideCursor(false), separator(0), readOnly(0),
        dragEnabled(0), m_echoMode(0), textDirty(0), selDirty(0),
        validInput(1), m_blinkPeriod(0), m_blinkTimer(0), m_deleteAllTimer(0),
        m_ascent(0), m_maxLength(32767), lastCursorPos(-1), 
        m_tripleClickTimer(0), maskData(0), modifiedState(0), undoState(0),
        selstart(0), selend(0), m_passwordEchoEditing(false)
    {
        init(txt);
    }

    ~QLineControl()
    {
        delete [] maskData;
    }

    int nextMaskBlank(int pos);
    int prevMaskBlank(int pos);

    bool isUndoAvailable() const;
    bool isRedoAvailable() const;
    void clearUndo();
    bool isModified() const;
    void setModified(bool modified);

    bool allSelected() const;
    bool hasSelectedText() const;

    int width() const;
    int height() const;
    int ascent() const;

    void setSelection(int start, int length);

    QString selectedText() const;
    QString textBeforeSelection() const;
    QString textAfterSelection() const;

    int selectionStart() const;
    int selectionEnd() const;
    bool inSelection(int x) const;

    void removeSelection();

    int start() const;
    int end() const;

#ifndef QT_NO_CLIPBOARD
    void copy(QClipboard::Mode mode = QClipboard::Clipboard) const;
    void paste();
#endif

    int cursor() const;
    int preeditCursor() const;

    int cursorWidth() const;
    void setCursorWidth(int value);

    void moveCursor(int pos, bool mark = false);
    void cursorForward(bool mark, int steps);
    void cursorWordForward(bool mark);
    void cursorWordBackward(bool mark);
    void home(bool mark);
    void end(bool mark);

    int xToPos(int x, QTextLine::CursorPosition = QTextLine::CursorBetweenCharacters) const;
    QRect cursorRect() const;

    qreal cursorToX(int cursor) const;
    qreal cursorToX() const;

    bool isReadOnly() const;
    void setReadOnly(bool enable);

    QString text() const;
    void setText(const QString &);

    QString displayText() const;

    void backspace();
    void del();
    void deselect();
    void selectAll();
    void insert(const QString &);
    void clear();
    void undo();
    void redo();
    void selectWordAtPos(int);

    uint echoMode() const;
    void setEchoMode(uint mode);

    void setMaxLength(int maxLength);
    int maxLength() const;

    const QValidator *validator() const;
    void setValidator(const QValidator *);

#ifndef QT_NO_COMPLETER
    QCompleter *completer() const;
    void setCompleter(const QCompleter*);
    void complete(int key);
#endif

    void setCursorPosition(int pos);
    int cursorPosition() const;

    bool hasAcceptableInput() const;
    bool fixup();

    QString inputMask() const;
    void setInputMask(const QString &mask);

    // input methods
#ifndef QT_NO_IM
    bool composeMode() const;
    void setPreeditArea(int cursor, const QString &text);
#endif

    QString preeditAreaText() const;

    void updatePasswordEchoEditing(bool editing);
    bool passwordEchoEditing() const;

    QChar passwordCharacter() const;
    void setPasswordCharacter(const QChar &character);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection direction);
    void setFont(const QFont &font);

    void processInputMethodEvent(QInputMethodEvent *event);
    void processMouseEvent(QMouseEvent* ev);
    void processKeyEvent(QKeyEvent* ev);

    int cursorBlinkPeriod() const;
    void setCursorBlinkPeriod(int msec);

    QString cancelText() const;
    void setCancelText(QString);

    enum DrawFlags {
        DrawText = 0x01,
        DrawSelections = 0x02,
        DrawCursor = 0x04,
        DrawAll = DrawText | DrawSelections | DrawCursor
    };
    void draw(QPainter *, const QPoint &, const QRect &, int flags = DrawAll);

    bool processEvent(QEvent* ev);

    bool emitingEditingFinished;//Needed in QLineEdit FocusOut event
private:
    void init(const QString&);
    void removeSelectedText();
    void p_setText(const QString& txt, int pos = -1, bool edited = true);
    void updateDisplayText();

    void p_insert(const QString& s);
    void p_del(bool wasBackspace = false);
    void p_remove(int pos);
    inline void p_deselect() { selDirty |= (selend > selstart); selstart = selend = 0; }
    void p_undo(int until = -1);
    void p_redo();

    QString m_text;
    QPalette m_palette;
    int m_cursor;
    int m_preeditCursor;
    int m_cursorWidth;
    Qt::LayoutDirection m_layoutDirection;
    uint hideCursor : 1; // used to hide the m_cursor inside preedit areas
    uint separator : 1;
    uint readOnly : 1;
    uint dragEnabled : 1;
    uint m_echoMode : 2;
    uint textDirty : 1;
    uint selDirty : 1;
    uint validInput : 1;
    int m_blinkPeriod; // 0 for non-blinking cursor
    int m_blinkTimer;
    int m_deleteAllTimer;
    int m_blinkStatus;
    int m_ascent;
    int m_maxLength;
    int lastCursorPos;
    QList<int> transactions;
    QPoint m_tripleClick;
    int m_tripleClickTimer;
    QString m_cancelText;

    void emitCursorPositionChanged();

    bool finishChange(int validateFromState = -1, bool update = false, bool edited = true);

    QPointer<QValidator> m_validator;
    QPointer<QCompleter> m_completer;
#ifndef QT_NO_COMPLETER
    bool advanceToEnabledItem(int dir);
#endif

    struct MaskInputData {
        enum Casemode { NoCaseMode, Upper, Lower };
        QChar maskChar; // either the separator char or the inputmask
        bool separator;
        Casemode caseMode;
    };
    QString m_inputMask;
    QChar blank;
    MaskInputData *maskData;


    // undo/redo handling
    enum CommandType { Separator, Insert, Remove, Delete, RemoveSelection, DeleteSelection, SetSelection };
    struct Command {
        inline Command() {}
        inline Command(CommandType t, int p, QChar c, int ss, int se) : type(t),uc(c),pos(p),selStart(ss),selEnd(se) {}
        uint type : 4;
        QChar uc;
        int pos, selStart, selEnd;
    };
    int modifiedState;
    int undoState;
    QVector<Command> history;
    void addCommand(const Command& cmd);

    inline void separate() { separator = true; }

    // selection
    int selstart, selend;

    // masking
    void parseInputMask(const QString &maskFields);
    bool isValidInput(QChar key, QChar mask) const;
    bool hasAcceptableInput(const QString &text) const;
    QString maskString(uint pos, const QString &str, bool clear = false) const;
    QString clearString(uint pos, uint len) const;
    QString stripString(const QString &str) const;
    int findInMask(int pos, bool forward, bool findSeparator, QChar searchChar = QChar()) const;


    // complex text layout
    QTextLayout textLayout;

    bool m_passwordEchoEditing;
    QChar m_passwordCharacter;

Q_SIGNALS:
    void cursorPositionChanged(int, int);
    void selectionChanged();

    void displayTextChanged(const QString &);
    void textChanged(const QString &);
    void textEdited(const QString &);

    void resetInputContext();

    void accepted();
    void editingFinished();
    void updateNeeded(const QRect &);
protected:
    virtual void timerEvent ( QTimerEvent * event );
private slots:
    void _q_clipboardChanged();
    void _q_deleteSelected();

};

inline int QLineControl::nextMaskBlank(int pos)
{
    int c = findInMask(pos, true, false);
    separator |= (c != pos);
    return (c != -1 ?  c : m_maxLength);
}

inline int QLineControl::prevMaskBlank(int pos)
{
    int c = findInMask(pos, false, false);
    separator |= (c != pos);
    return (c != -1 ? c : 0);
}

inline bool QLineControl::isUndoAvailable() const
{ 
    return !readOnly && undoState;
}

inline bool QLineControl::isRedoAvailable() const
{
    return !readOnly && undoState < (int)history.size();
}

inline void QLineControl::clearUndo()
{
    history.clear();
    modifiedState = undoState = 0;
}

inline bool QLineControl::isModified() const
{
    return modifiedState != undoState;
}

inline void QLineControl::setModified(bool modified)
{
    modifiedState = modified ? -1 : undoState;
}

inline bool QLineControl::allSelected() const
{
    return !m_text.isEmpty() && selstart == 0 && selend == (int)m_text.length();
}

inline bool QLineControl::hasSelectedText() const
{
    return !m_text.isEmpty() && selend > selstart;
}

inline int QLineControl::width() const
{
    return qRound(textLayout.lineAt(0).width()) + 1;
}

inline int QLineControl::height() const
{
    return qRound(textLayout.lineAt(0).height()) + 1;
}

inline int QLineControl::ascent() const
{
    return m_ascent;
}

inline QString QLineControl::selectedText() const
{
    if (hasSelectedText())
        return m_text.mid(selstart, selend - selstart);
    return QString();
}

inline QString QLineControl::textBeforeSelection() const
{
    if (hasSelectedText())
        return m_text.left(selstart);
    return QString();
}

inline QString QLineControl::textAfterSelection() const
{
    if (hasSelectedText())
        return m_text.mid(selend);
    return QString();
}

inline int QLineControl::selectionStart() const
{
    return hasSelectedText() ? selstart : -1;
}

inline int QLineControl::selectionEnd() const
{
    return hasSelectedText() ? selend : -1;
}

inline int QLineControl::start() const
{
    return 0;
}

inline int QLineControl::end() const
{
    return m_text.length();
}

inline void QLineControl::removeSelection()
{
    int priorState = undoState;
    removeSelectedText();
    finishChange(priorState);
}

inline bool QLineControl::inSelection(int x) const
{
    if (selstart >= selend)
        return false;
    int pos = xToPos(x, QTextLine::CursorOnCharacter);
    return pos >= selstart && pos < selend;
}

inline int QLineControl::cursor() const
{
    return m_cursor;
}

inline int QLineControl::preeditCursor() const
{
    return m_preeditCursor;
}

inline int QLineControl::cursorWidth() const
{
    return m_cursorWidth;
}

inline void QLineControl::setCursorWidth(int value)
{
    m_cursorWidth = value;
}

inline void QLineControl::cursorForward(bool mark, int steps)
{
    int c = m_cursor;
    if (steps > 0) {
        while(steps--)
            c = textLayout.nextCursorPosition(c);
    } else if (steps < 0) {
        while (steps++)
            c = textLayout.previousCursorPosition(c);
    }
    moveCursor(c, mark);
}

inline void QLineControl::cursorWordForward(bool mark)
{
    moveCursor(textLayout.nextCursorPosition(m_cursor, QTextLayout::SkipWords), mark);
}

inline void QLineControl::home(bool mark)
{
    moveCursor(0, mark);
}

inline void QLineControl::end(bool mark)
{
    moveCursor(text().length(), mark);
}

inline void QLineControl::cursorWordBackward(bool mark)
{
    moveCursor(textLayout.previousCursorPosition(m_cursor, QTextLayout::SkipWords), mark);
}

inline qreal QLineControl::cursorToX(int cursor) const
{
    return textLayout.lineAt(0).cursorToX(cursor);
}

inline qreal QLineControl::cursorToX() const
{
    return cursorToX(m_cursor);
}

inline bool QLineControl::isReadOnly() const
{
    return readOnly;
}

inline void QLineControl::setReadOnly(bool enable)
{ 
    readOnly = enable;
}

inline QString QLineControl::text() const
{
    QString res = maskData ?  stripString(m_text) : m_text;
    return (res.isNull() ? QString::fromLatin1("") : res);
}

inline void QLineControl::setText(const QString &txt)
{
    p_setText(txt, -1, false);
}

inline QString QLineControl::displayText() const
{
    return textLayout.text();
}

inline void QLineControl::deselect()
{
    p_deselect();
    finishChange();
}

inline void QLineControl::selectAll()
{
    selstart = selend = m_cursor = 0;
    moveCursor(m_text.length(), true);
}

inline void QLineControl::undo()
{
    p_undo();
    finishChange(-1, true);
}

inline void QLineControl::redo()
{
    p_redo();
    finishChange();
}

inline uint QLineControl::echoMode() const
{
    return m_echoMode;
}

inline void QLineControl::setEchoMode(uint mode)
{
    m_echoMode = mode;
    m_passwordEchoEditing = false;
    updateDisplayText();
}

inline void QLineControl::setMaxLength(int maxLength)
{
    if (maskData)
        return;
    m_maxLength = maxLength;
    setText(m_text);
}

inline int QLineControl::maxLength() const
{
    return m_maxLength;
}

inline const QValidator *QLineControl::validator() const
{
    return m_validator;
}

inline void QLineControl::setValidator(const QValidator *v)
{
    m_validator = const_cast<QValidator*>(v);
}

#ifndef QT_NO_COMPLETER
inline QCompleter *QLineControl::completer() const
{
    return m_completer;
}

/* Note that you must set the widget for the completer seperately */
inline void QLineControl::setCompleter(const QCompleter* c)
{
    m_completer = const_cast<QCompleter*>(c);
}
#endif

inline void QLineControl::setCursorPosition(int pos)
{
    if (pos < 0)
        pos = 0;
    if (pos < m_text.length())
        moveCursor(pos);
}

inline int QLineControl::cursorPosition() const
{
    return m_cursor;
}

inline bool QLineControl::hasAcceptableInput() const
{
    return hasAcceptableInput(m_text);
}

inline QString QLineControl::inputMask() const
{
    return maskData ? m_inputMask + QLatin1Char(';') + blank : QString();
}

inline void QLineControl::setInputMask(const QString &mask)
{
    parseInputMask(mask);
    if (maskData)
        moveCursor(nextMaskBlank(0));
}

// input methods
#ifndef QT_NO_IM
inline bool QLineControl::composeMode() const
{
    return !textLayout.preeditAreaText().isEmpty();
}

inline void QLineControl::setPreeditArea(int cursor, const QString &text)
{
    textLayout.setPreeditArea(cursor, text);
}
#endif

inline QString QLineControl::preeditAreaText() const
{
    return textLayout.preeditAreaText();
}

inline bool QLineControl::passwordEchoEditing() const
{
    return m_passwordEchoEditing;
}

inline QChar QLineControl::passwordCharacter() const
{
    return m_passwordCharacter;
}

inline void QLineControl::setPasswordCharacter(const QChar &character)
{
    m_passwordCharacter = character;
    updateDisplayText();
}

inline Qt::LayoutDirection QLineControl::layoutDirection() const
{
    return m_layoutDirection;
}

inline void QLineControl::setLayoutDirection(Qt::LayoutDirection direction)
{
    if (direction != m_layoutDirection) {
        m_layoutDirection = direction;
        updateDisplayText();
    }
}

inline void QLineControl::setFont(const QFont &font)
{
    textLayout.setFont(font);
    updateDisplayText();
}

inline int QLineControl::cursorBlinkPeriod() const
{
    return m_blinkPeriod;
}

inline QString QLineControl::cancelText() const
{
    return m_cancelText;
}

inline void QLineControl::setCancelText(QString s)
{
    m_cancelText = s;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QT_NO_LINEEDIT

#endif // QLINECONTROL_P_H
