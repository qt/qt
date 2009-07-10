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

#include <qfxtextedit.h>
#include "qfxtextedit_p.h"
#include <private/qtextcontrol_p.h>
#include <private/qfxperf_p.h>
#include "qfxevents_p.h"
#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>

#include <QDebug>


QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QFxTextEdit, TextEdit)

/*!
    \qmlclass TextEdit
    \brief The TextEdit item allows you to add editable formatted text to a scene.

    It can display both plain and rich text. For example:

    \qml
TextEdit {
    id: edit
    text: "<b>Hello</b> <i>World!</i>"
    focus: true
    focusable: true
    font.family: "Helvetica"
    font.size: 20
    color: "blue"
    width: 240
}
    \endqml

    \image declarative-textedit.gif

    \sa Text
*/

/*!
    \internal
    \class QFxTextEdit
    \qmlclass TextEdit
    \ingroup group_coreitems

    \brief The QFxTextEdit class provides an editable formatted text item that you can add to a QFxView.

    It can display both plain and rich text.

    \image declarative-textedit.png

    A QFxTextEdit object can be instantiated in Qml using the tag \c &lt;TextEdit&gt;.
*/

/*!
    Constructs a new QFxTextEdit.
*/
QFxTextEdit::QFxTextEdit(QFxItem *parent)
: QFxPaintedItem(*(new QFxTextEditPrivate), parent)
{
    Q_D(QFxTextEdit);
    d->init();
}

/*!
\internal
*/
QFxTextEdit::QFxTextEdit(QFxTextEditPrivate &dd, QFxItem *parent)
    : QFxPaintedItem(dd, parent)
{
    Q_D(QFxTextEdit);
    d->init();
}

QString QFxTextEdit::text() const
{
    Q_D(const QFxTextEdit);

    if (d->richText)
        return d->document->toHtml();
    else
        return d->document->toPlainText();
}

/*!
    \qmlproperty font TextEdit::font

    Set the TextEdit's font attributes.  \c font.size sets the font's point size.
*/

/*!
    \qmlproperty string TextEdit::text

    The text to display.  If the text format is AutoText the text edit will
    automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().
*/

/*!
    \property QFxTextEdit::text
    \brief the text edit's text

    If the text format is set to AutoText the text edit will try to
    automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    \sa textFormat
*/

void QFxTextEdit::setText(const QString &text)
{
    Q_D(QFxTextEdit);
    if (QFxTextEdit::text() == text)
        return;
    d->text = text;
    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text));
    if (d->richText) {
        d->control->setHtml(text);
    } else {
        d->control->setPlainText(text);
    }
    q_textChanged();
    updateSize();
}

/*!
    \qmlproperty enumeration TextEdit::textFormat

    The way the text property should be displayed.

    Supported text formats are \c AutoText, \c PlainText and \c RichText.

    The default is AutoText.  If the text format is AutoText the text edit
    will automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    \table
    \row
    \o
    \qml
VerticalLayout {
    TextEdit {
        font.size: 24
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.size: 24
        textFormat: "RichText"
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.size: 24
        textFormat: "PlainText"
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \o \image declarative-textformat.png
    \endtable
*/

/*!
    \property QFxTextEdit::textFormat
    \brief this property describes how the text set on the text edit
    should be interpreted.

    Valid values are \c AutoText, \c PlainText and \c RichText.  The
    default value is \c AutoText, meaning the text edit will attempt
    to guess how the text should be interpreted using the
    Qt::mightBeRichText() function.

    \sa text
*/

QFxTextEdit::TextFormat QFxTextEdit::textFormat() const
{
    Q_D(const QFxTextEdit);
    return d->format;
}

void QFxTextEdit::setTextFormat(TextFormat format)
{
    Q_D(QFxTextEdit);
    if (format == d->format)
        return;
    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && Qt::mightBeRichText(d->text));

    if (wasRich && !d->richText) {
        d->control->setPlainText(d->text);
        updateSize();
    } else if (!wasRich && d->richText) {
        d->control->setHtml(d->text);
        updateSize();
    }
    d->format = format;
}

/*!
    \property QFxTextEdit::font
    \brief the text edit's default font
*/

QmlFont *QFxTextEdit::font()
{
    Q_D(QFxTextEdit);
    return &(d->font);
}

/*!
    \qmlproperty color TextEdit::color

    The text color.

    \qml
// green text using hexadecimal notation
TextEdit { color: "#00FF00"; ...  }

// steelblue text using SVG color name
TextEdit { color: "steelblue"; ...  }
    \endqml
*/

/*!
    \property QFxTextEdit::color
    \brief the text edit's default text color
*/
QColor QFxTextEdit::color() const
{
    Q_D(const QFxTextEdit);
    return d->color;
}

void QFxTextEdit::setColor(const QColor &color)
{
    Q_D(QFxTextEdit);
    if (d->color == color)
        return;

    clearCache();
    d->color = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Text, color);
    d->control->setPalette(pal);
    update();
}

/*!
    \qmlproperty color TextEdit::highlightColor

    The text highlight color, used behind selections.
*/

/*!
    \property QFxTextEdit::highlightColor
    \brief the text edit's default text highlight color
*/
QColor QFxTextEdit::highlightColor() const
{
    Q_D(const QFxTextEdit);
    return d->highlightColor;
}

void QFxTextEdit::setHighlightColor(const QColor &color)
{
    Q_D(QFxTextEdit);
    if (d->highlightColor == color)
        return;

    clearCache();
    d->highlightColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Highlight, color);
    d->control->setPalette(pal);
    update();
}

/*!
    \qmlproperty color TextEdit::highlightedTextColor

    The highlighted text color, used in selections.
*/

/*!
    \property QFxTextEdit::highlightedTextColor
    \brief the text edit's default highlighted text color
*/
QColor QFxTextEdit::highlightedTextColor() const
{
    Q_D(const QFxTextEdit);
    return d->highlightColor;
}

void QFxTextEdit::setHighlightedTextColor(const QColor &color)
{
    Q_D(QFxTextEdit);
    if (d->highlightedTextColor == color)
        return;

    clearCache();
    d->highlightedTextColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::HighlightedText, color);
    d->control->setPalette(pal);
    update();
}

/*!
    \qmlproperty enumeration TextEdit::hAlign
    \qmlproperty enumeration TextEdit::vAlign

    Sets the horizontal and vertical alignment of the text within the TextEdit items
    width and height.  By default, the text is top-left aligned.

    The valid values for \c hAlign are \c AlignLeft, \c AlignRight and 
    \c AlignHCenter.  The valid values for \c vAlign are \c AlignTop, \c AlignBottom 
    and \c AlignVCenter.
*/

/*!
    \property QFxTextEdit::hAlign
    \brief the horizontal alignment of the text.

    Valid values are \c AlignLeft, \c AlignRight, and \c AlignHCenter. The default value is \c AlignLeft.
*/
QFxTextEdit::HAlignment QFxTextEdit::hAlign() const
{
    Q_D(const QFxTextEdit);
    return d->hAlign;
}

void QFxTextEdit::setHAlign(QFxTextEdit::HAlignment alignment)
{
    Q_D(QFxTextEdit);
    if (alignment == d->hAlign)
        return;
    d->hAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
}

/*!
    \property QFxTextEdit::vAlign
    \brief the vertical alignment of the text.

    Valid values are \c AlignTop, \c AlignBottom, and \c AlignVCenter. The default value is \c AlignTop.
*/
QFxTextEdit::VAlignment QFxTextEdit::vAlign() const
{
    Q_D(const QFxTextEdit);
    return d->vAlign;
}

void QFxTextEdit::setVAlign(QFxTextEdit::VAlignment alignment)
{
    Q_D(QFxTextEdit);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
}

bool QFxTextEdit::wrap() const
{
    Q_D(const QFxTextEdit);
    return d->wrap;
}

/*!
    \qmlproperty bool TextEdit::wrap

    Set this property to wrap the text to the TextEdit item's width.
    The text will only wrap if an explicit width has been set.

    Wrapping is done on word boundaries (i.e. it is a "word-wrap"). Wrapping is off by default.
*/

/*!
    \property QFxTextEdit::wrap
    \brief If true the text edit wraps text based on the width of the
    text edit.
*/
void QFxTextEdit::setWrap(bool w)
{
    Q_D(QFxTextEdit);
    if (w == d->wrap)
        return;
    d->wrap = w;
    d->updateDefaultTextOption();
    updateSize();
}

/*!
    \qmlproperty TextEdit::cursorVisible
    \brief If true the text edit shows a cursor.

    This property is set and unset when the text edit gets focus, but it can also
    be set directly (useful, for example, if a KeyProxy might forward keys to it).
*/
bool QFxTextEdit::isCursorVisible() const
{
    Q_D(const QFxTextEdit);
    return d->cursorVisible;
}

void QFxTextEdit::setCursorVisible(bool on)
{
    Q_D(QFxTextEdit);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    QFocusEvent focusEvent(on ? QEvent::FocusIn : QEvent::FocusOut);
    if (!on && !d->preserveSelection)
        d->control->setCursorIsFocusIndicator(true);
    d->control->processEvent(&focusEvent, QPointF(0, 0));
}

/*!
    \qmlproperty TextEdit::cursorPosition
    \brief The position of the cursor in the TextEdit.
*/
int QFxTextEdit::cursorPosition() const
{
    Q_D(const QFxTextEdit);
    return d->control->textCursor().position();
}

void QFxTextEdit::setCursorPosition(int pos)
{
    Q_D(QFxTextEdit);
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos);
    d->control->setTextCursor(cursor);
}

/*!
    \qmlproperty TextEdit::cursorDelegate
    \brief The delegate for the cursor in the TextEdit.

    If you set a cursorDelegate for a TextEdit, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the text edit when a cursor is
    needed, and the x and y properties of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QFxItem or
    QFxItem derived item.
*/
QmlComponent* QFxTextEdit::cursorDelegate() const
{
    Q_D(const QFxTextEdit);
    return d->cursorComponent;
}

void QFxTextEdit::setCursorDelegate(QmlComponent* c)
{
    Q_D(QFxTextEdit);
    if(d->cursorComponent){
        delete d->cursorComponent;
        if(d->cursor){
            disconnect(d->control, SIGNAL(cursorPositionChanged()),
                    this, SLOT(moveCursorDelegate()));
            d->control->setCursorWidth(-1);
            dirtyCache(cursorRect());
            delete d->cursor;
            d->cursor = 0;
        }
    }
    d->cursorComponent = c;
    if(c && c->isReady()){
        loadCursorDelegate();
    }else{
        if(c)
            connect(c, SIGNAL(statusChanged()),
                    this, SLOT(loadCursorDelegate()));
    }
}

void QFxTextEdit::loadCursorDelegate()
{
    Q_D(QFxTextEdit);
    if(d->cursorComponent->isLoading())
        return;
    d->cursor = qobject_cast<QFxItem*>(d->cursorComponent->create(qmlContext(this)));
    if(d->cursor){
        connect(d->control, SIGNAL(cursorPositionChanged()),
                this, SLOT(moveCursorDelegate()));
        d->control->setCursorWidth(0);
        dirtyCache(cursorRect());
        d->cursor->setItemParent(this);
        d->cursor->setHeight(QFontMetrics(d->font.font()).height());
        moveCursorDelegate();
    }else{
        qWarning() << "Error loading cursor delegate for TextEdit:" + objectName();
    }
}

/*!
    \qmlproperty int TextEdit::selectionStart

    The cursor position before the first character in the current selection.
    Setting this and selectionEnd allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionStart to a value outside of
    the current text, selectionStart will not be changed.

    \sa selectionEnd, cursorPosition, selectedText
*/
int QFxTextEdit::selectionStart() const
{
    Q_D(const QFxTextEdit);
    return d->control->textCursor().selectionStart();
}

void QFxTextEdit::setSelectionStart(int s)
{
    Q_D(QFxTextEdit);
    if(d->lastSelectionStart == s || s < 0 || s > text().length())
        return;
    d->lastSelectionStart = s;
    d->updateSelection();// Will emit the relevant signals
}

/*!
    \qmlproperty int TextEdit::selectionEnd

    The cursor position after the last character in the current selection.
    Setting this and selectionStart allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionEnd to a value outside of
    the current text, selectionEnd will not be changed.

    \sa selectionStart, cursorPosition, selectedText
*/
int QFxTextEdit::selectionEnd() const
{
    Q_D(const QFxTextEdit);
    return d->control->textCursor().selectionEnd();
}

void QFxTextEdit::setSelectionEnd(int s)
{
    Q_D(QFxTextEdit);
    if(d->lastSelectionEnd == s || s < 0 || s > text().length())
        return;
    d->lastSelectionEnd = s;
    d->updateSelection();// Will emit the relevant signals
}

/*!
    \qmlproperty string TextEdit::selectedText

    This read-only property provides the text currently selected in the
    text edit.

    It is equivalent to the following snippet, but is faster and easier
    to use.
    \code
    //myTextEdit is the id of the TextEdit
    myTextEdit.text.toString().substring(myTextEdit.selectionStart,
            myTextEdit.selectionEnd);
    \endcode
*/
QString QFxTextEdit::selectedText() const
{
    Q_D(const QFxTextEdit);
    return d->control->textCursor().selectedText();
}

/*!
    \qmlproperty bool TextEdit::focusOnPress

    Whether the TextEdit should gain focus on a mouse press. By default this is
    set to false;
*/
bool QFxTextEdit::focusOnPress() const
{
    Q_D(const QFxTextEdit);
    return d->focusOnPress;
}

void QFxTextEdit::setFocusOnPress(bool on)
{
    Q_D(QFxTextEdit);
    if (d->focusOnPress == on)
        return;
    d->focusOnPress = on;
}

/*!
    \qmlproperty bool TextEdit::preserveSelection

    Whether the TextEdit should keep the selection visible when it loses focus to another
    item in the scene. By default this is set to true;
*/
bool QFxTextEdit::preserveSelection() const
{
    Q_D(const QFxTextEdit);
    return d->preserveSelection;
}

void QFxTextEdit::setPreserveSelection(bool on)
{
    Q_D(QFxTextEdit);
    if (d->preserveSelection == on)
        return;
    d->preserveSelection = on;
}

qreal QFxTextEdit::textMargin() const
{
    Q_D(const QFxTextEdit);
    return d->textMargin;
}

void QFxTextEdit::setTextMargin(qreal margin)
{
    Q_D(QFxTextEdit);
    if (d->textMargin == margin)
        return;
    d->textMargin = margin;
    d->document->setDocumentMargin(d->textMargin);
}

void QFxTextEdit::geometryChanged(const QRectF &newGeometry, 
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QFxPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
   \internal
*/
QString QFxTextEdit::propertyInfo() const
{
    Q_D(const QFxTextEdit);
    return QChar(QLatin1Char('\"')) + d->text + QChar(QLatin1Char('\"'));
}

/*!
    Ensures any delayed caching or data loading the class
    needs to performed is complete.
*/
void QFxTextEdit::componentComplete()
{
    Q_D(QFxTextEdit);
    QFxPaintedItem::componentComplete();
    if (d->dirty) {
        updateSize();
        d->dirty = false;
    }
}

/*!
    \qmlproperty bool TextEdit::readOnly

    Whether the user an interact with the TextEdit item.  If this
    property is set to true the text cannot be edited by user interaction.

    By default this property is false.
*/

/*!
    \property QFxTextEdit::readOnly
    \brief If this property is true the text can not be edited by user interaction.

    Changing this property will modify the text interaction flags.  If
    you require more specific control about how user interaction
    with the text edit is handled, use setTextInteractionFlags() instead.

    \sa setTextInteractionFlags()
*/
void QFxTextEdit::setReadOnly(bool r)
{
    Q_D(QFxTextEdit);

    Qt::TextInteractionFlags flags = Qt::NoTextInteraction;
    if (r) {
        flags = Qt::TextSelectableByMouse;
    } else {
        flags = Qt::TextEditorInteraction;
    }
    d->control->setTextInteractionFlags(flags);
    if (!r)
        d->control->moveCursor(QTextCursor::End);
}

bool QFxTextEdit::isReadOnly() const
{
    Q_D(const QFxTextEdit);
    return !(d->control->textInteractionFlags() & Qt::TextEditable);
}

/*!
    Sets how the text edit should interact with user input to the given
    \a flags.
*/
void QFxTextEdit::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(QFxTextEdit);
    d->control->setTextInteractionFlags(flags);
}

/*!
    Returns the flags specifying how the text edit should interact
    with user input.
*/
Qt::TextInteractionFlags QFxTextEdit::textInteractionFlags() const
{
    Q_D(const QFxTextEdit);
    return d->control->textInteractionFlags();
}

/*!
    Returns the cursor for the point at the given \a pos on the 
    text edit.
*/
QTextCursor QFxTextEdit::cursorForPosition(const QPoint &pos) const
{
    Q_D(const QFxTextEdit);
    return d->control->cursorForPosition(pos);
}

/*!
    Returns the rectangle where the given text \a cursor is rendered
    within the text edit.
*/
QRect QFxTextEdit::cursorRect(const QTextCursor &cursor) const
{
    Q_D(const QFxTextEdit);
    if (cursor.isNull())
        return QRect();

    return d->control->cursorRect(cursor).toRect();
}

/*!
    Returns the rectangle where the text cursor is rendered
    within the text edit.
*/
QRect QFxTextEdit::cursorRect() const
{
    Q_D(const QFxTextEdit);
    return d->control->cursorRect().toRect();
}


/*!
    Sets the text cursor for the text edit to the given \a cursor.
*/
void QFxTextEdit::setTextCursor(const QTextCursor &cursor)
{
    Q_D(QFxTextEdit);
    d->control->setTextCursor(cursor);
}

/*!
    Returns the text cursor for the text edit.
*/
QTextCursor QFxTextEdit::textCursor() const
{
    Q_D(const QFxTextEdit);
    return d->control->textCursor();
}

/*!
Moves the cursor by performing the given \a operation.

If \a mode is QTextCursor::KeepAnchor, the cursor selects the text it moves over. This is the same effect that the user achieves when they hold down the Shift key and move the cursor with the cursor keys.
*/
void QFxTextEdit::moveCursor(QTextCursor::MoveOperation operation, QTextCursor::MoveMode mode)
{
    Q_D(QFxTextEdit);
    d->control->moveCursor(operation, mode);
}

/*!
\overload
Handles the given \a event.
*/
bool QFxTextEdit::event(QEvent *event)
{
    Q_D(QFxTextEdit);
    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(0, 0));
        return true;
    }
    return QFxPaintedItem::event(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QFxTextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(QFxTextEdit);
    //### experiment with allowing 'overrides' to key events
    QFxKeyEvent ke(*event);
    emit keyPress(&ke);
    event->setAccepted(ke.isAccepted());
    if (event->isAccepted())
        return;

    //### this causes non-standard cursor behavior in some cases.
    //     is it still needed?
    /*QTextCursor c = textCursor();
    QTextCursor::MoveOperation op = QTextCursor::NoMove;
    if (event == QKeySequence::MoveToNextChar) {
        op = QTextCursor::Right;
    } else if (event == QKeySequence::MoveToPreviousChar) {
        op = QTextCursor::Left;
    } else if (event == QKeySequence::MoveToNextWord) {
        op = QTextCursor::WordRight;
    } else if (event == QKeySequence::MoveToPreviousWord) {
        op = QTextCursor::WordLeft;
    } else if (event == QKeySequence::MoveToNextLine) {
        op = QTextCursor::Down;
    } else if (event == QKeySequence::MoveToPreviousLine) {
        op = QTextCursor::Up;
    }

    if (op != QTextCursor::NoMove && !c.movePosition(op))
        event->ignore();
    else*/
        d->control->processEvent(event, QPointF(0, 0));
}

/*!
\overload
Handles the given key \a event.
*/
void QFxTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QFxTextEdit);
    //### experiment with allowing 'overrides' to key events
    QFxKeyEvent ke(*event);
    emit keyRelease(&ke);
    event->setAccepted(ke.isAccepted());
    if (event->isAccepted())
        return;

    d->control->processEvent(event, QPointF(0, 0));
}

/*!
    \overload
    Handles changing of the focus property.  Focus is applied to the control
    even if the edit does not have active focus.  This is because things
    like KeyProxy can give the behavior of focus even when activeFocus isn't
    true.
*/
void QFxTextEdit::focusChanged(bool hasFocus)
{
    setCursorVisible(hasFocus);
}

/*!
    Causes all text to be selected.
*/
void QFxTextEdit::selectAll()
{
    Q_D(QFxTextEdit);
    d->control->selectAll();
}

/*!
\overload
Handles the given mouse \a event.
*/
void QFxTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextEdit);
    if (d->focusOnPress)
        setFocus(true);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QFxPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QFxTextEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QFxPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QFxTextEdit::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QFxPaintedItem::mouseDoubleClickEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QFxTextEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QFxPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given input method \a event.
*/
void QFxTextEdit::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QFxTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
}

/*!
\overload
Returns the value of the given \a property.
*/
QVariant QFxTextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QFxTextEdit);
    return d->control->inputMethodQuery(property);
}

/*!
Draws the contents of the text edit using the given \a painter within
the given \a bounds.
*/
void QFxTextEdit::drawContents(QPainter *painter, const QRect &bounds)
{
    Q_D(QFxTextEdit);

    painter->setRenderHint(QPainter::TextAntialiasing, true);

    d->control->drawContents(painter, bounds);
}

/*!
This signal is emitted when the font of the item changes.
*/
void QFxTextEdit::fontChanged()
{
    Q_D(QFxTextEdit);
    clearCache();
    d->document->setDefaultFont(d->font.font());
    if(d->cursor){
        d->cursor->setHeight(QFontMetrics(d->font.font()).height());
        moveCursorDelegate();
    }
    updateSize();
    emit update();
}

void QFxTextEdit::updateImgCache(const QRectF &r)
{
    dirtyCache(r.toRect());
    emit update();
}

void QFxTextEditPrivate::init()
{
    Q_Q(QFxTextEdit);

    q->setSmooth(true);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setOptions(QFxTextEdit::AcceptsInputMethods | QFxTextEdit::SimpleItem
        | QFxTextEdit::HasContents | QFxTextEdit::MouseEvents);

    QObject::connect(&font, SIGNAL(updated()), q, SLOT(fontChanged()));

    control = new QTextControl(q);

    QObject::connect(control, SIGNAL(updateRequest(QRectF)), q, SLOT(updateImgCache(QRectF)));

    QObject::connect(control, SIGNAL(textChanged()), q, SLOT(q_textChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SIGNAL(selectionChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SIGNAL(cursorPositionChanged()));

    document = control->document();
    document->setDefaultFont(font.font());
    document->setDocumentMargin(textMargin);
    document->setUndoRedoEnabled(false); // flush undo buffer.
    document->setUndoRedoEnabled(true);
    updateDefaultTextOption();
}

void QFxTextEdit::q_textChanged()
{
    if (!widthValid())
        updateSize();   //### optimize: we get 3 calls to updateSize every time a letter is typed
    emit textChanged(text());
}

void QFxTextEdit::moveCursorDelegate()
{
    Q_D(QFxTextEdit);
    if(!d->cursor)
        return;
    QRectF cursorRect = d->control->cursorRect();
    d->cursor->setX(cursorRect.x());
    d->cursor->setY(cursorRect.y());
}

void QFxTextEditPrivate::updateSelection()
{
    Q_Q(QFxTextEdit);
    QTextCursor cursor = control->textCursor();
    bool startChange = (lastSelectionStart != cursor.selectionStart());
    bool endChange = (lastSelectionEnd != cursor.selectionEnd());
    //### Is it worth calculating a more minimal set of movements?
    cursor.beginEditBlock();
    cursor.setPosition(lastSelectionStart, QTextCursor::MoveAnchor);
    cursor.setPosition(lastSelectionEnd, QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    control->setTextCursor(cursor);
    if(startChange)
        q->selectionStartChanged();
    if(endChange)
        q->selectionEndChanged();
    startChange = (lastSelectionStart != control->textCursor().selectionStart());
    endChange = (lastSelectionEnd != control->textCursor().selectionEnd());
    if(startChange || endChange)
        qWarning() << "QFxTextEditPrivate::updateSelection() has failed you.";
}

void QFxTextEdit::updateSelectionMarkers()
{
    Q_D(QFxTextEdit);
    if(d->lastSelectionStart != d->control->textCursor().selectionStart()){
        d->lastSelectionStart = d->control->textCursor().selectionStart();
        emit selectionStartChanged();
    }
    if(d->lastSelectionEnd != d->control->textCursor().selectionEnd()){
        d->lastSelectionEnd = d->control->textCursor().selectionEnd();
        emit selectionEndChanged();
    }
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void QFxTextEdit::updateSize()
{
    Q_D(QFxTextEdit);
    if (isComponentComplete()) {
        QFontMetrics fm = QFontMetrics(d->font.font());
        int dy = height();
        // ### assumes that if the width is set, the text will fill to edges
        // ### (unless wrap is false, then clipping will occur)
        if (widthValid())
            d->document->setTextWidth(width());
        dy -= (int)d->document->size().height();

        int yoff = 0;
        if (heightValid()) {
            if (d->vAlign == AlignBottom)
                yoff = dy;
            else if (d->vAlign == AlignVCenter)
                yoff = dy/2;
        }
        setBaselineOffset(fm.ascent() + yoff + d->textMargin);
        if (!widthValid()) {
            int newWidth = (int)d->document->idealWidth();
            d->document->setTextWidth(newWidth); // ### QTextDoc> Alignment will not work unless textWidth is set
            setImplicitWidth(newWidth);
        }
        if (!heightValid()) {
            if (d->text.isEmpty()) {
                setImplicitHeight(fm.height());
            } else { 
                setImplicitHeight((int)d->document->size().height());
            } 
        }
        setContentsSize(QSize(width(), height()));
    } else {
        d->dirty = true;
    }
    emit update();
}

void QFxTextEditPrivate::updateDefaultTextOption()
{
    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();
    opt.setAlignment((Qt::Alignment)(int)(hAlign | vAlign));
    
    QTextOption::WrapMode oldWrapMode = opt.wrapMode();

    if (wrap)
        opt.setWrapMode(QTextOption::WordWrap);
    else
        opt.setWrapMode(QTextOption::NoWrap);

    if (oldWrapMode == opt.wrapMode() && oldAlignment == opt.alignment())
        return;
    document->setDefaultTextOption(opt);
}

QT_END_NAMESPACE
