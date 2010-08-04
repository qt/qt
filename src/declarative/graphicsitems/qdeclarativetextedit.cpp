/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "private/qdeclarativetextedit_p.h"
#include "private/qdeclarativetextedit_p_p.h"

#include "private/qdeclarativeevents_p_p.h"
#include <private/qdeclarativeglobal_p.h>
#include <qdeclarativeinfo.h>

#include <QtCore/qmath.h>

#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPainter>

#include <private/qtextcontrol_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass TextEdit QDeclarativeTextEdit
    \since 4.7
    \brief The TextEdit item displays multiple lines of editable formatted text.
    \inherits Item

    The TextEdit item displays a block of editable, formatted text.

    It can display both plain and rich text. For example:

    \qml
TextEdit {
    width: 240
    text: "<b>Hello</b> <i>World!</i>"
    font.family: "Helvetica"
    font.pointSize: 20
    color: "blue"
    focus: true
}
    \endqml

    \image declarative-textedit.gif

    Setting \l {Item::focus}{focus} to \c true enables the TextEdit item to receive keyboard focus.

    Note that the TextEdit does not implement scrolling, following the cursor, or other behaviors specific
    to a look-and-feel. For example, to add flickable scrolling that follows the cursor:

    \snippet snippets/declarative/texteditor.qml 0

    A particular look-and-feel might use smooth scrolling (eg. using SmoothedFollow), might have a visible
    scrollbar, or a scrollbar that fades in to show location, etc.

    Clipboard support is provided by the cut(), copy(), and paste() functions, and the selection can
    be handled in a traditional "mouse" mechanism by setting selectByMouse, or handled completely
    from QML by manipulating selectionStart and selectionEnd, or using selectAll() or selectWord().

    You can translate between cursor positions (characters from the start of the document) and pixel
    points using positionAt() and positionToRectangle().

    \sa Text, TextInput, {declarative/text/textselection}{Text Selection example}
*/

/*!
    \internal
    \class QDeclarativeTextEdit
    \qmlclass TextEdit

    \brief The QDeclarativeTextEdit class provides an editable formatted text item that you can add to a QDeclarativeView.

    It can display both plain and rich text.

    \image declarative-textedit.png

    A QDeclarativeTextEdit object can be instantiated in QML using the tag \c &lt;TextEdit&gt;.
*/

/*!
    Constructs a new QDeclarativeTextEdit.
*/
QDeclarativeTextEdit::QDeclarativeTextEdit(QDeclarativeItem *parent)
: QDeclarativePaintedItem(*(new QDeclarativeTextEditPrivate), parent)
{
    Q_D(QDeclarativeTextEdit);
    d->init();
}

QString QDeclarativeTextEdit::text() const
{
    Q_D(const QDeclarativeTextEdit);

#ifndef QT_NO_TEXTHTMLPARSER
    if (d->richText)
        return d->document->toHtml();
    else
#endif
        return d->document->toPlainText();
}

/*!
    \qmlproperty string TextEdit::font.family

    Sets the family name of the font.

    The family name is case insensitive and may optionally include a foundry name, e.g. "Helvetica [Cronyx]".
    If the family is available from more than one foundry and the foundry isn't specified, an arbitrary foundry is chosen.
    If the family isn't available a family will be set using the font matching algorithm.
*/

/*!
    \qmlproperty bool TextEdit::font.bold

    Sets whether the font weight is bold.
*/

/*!
    \qmlproperty enumeration TextEdit::font.weight

    Sets the font's weight.

    The weight can be one of:
    \list
    \o Font.Light
    \o Font.Normal - the default
    \o Font.DemiBold
    \o Font.Bold
    \o Font.Black
    \endlist

    \qml
    TextEdit { text: "Hello"; font.weight: Font.DemiBold }
    \endqml
*/

/*!
    \qmlproperty bool TextEdit::font.italic

    Sets whether the font has an italic style.
*/

/*!
    \qmlproperty bool TextEdit::font.underline

    Sets whether the text is underlined.
*/

/*!
    \qmlproperty bool TextEdit::font.strikeout

    Sets whether the font has a strikeout style.
*/

/*!
    \qmlproperty real TextEdit::font.pointSize

    Sets the font size in points. The point size must be greater than zero.
*/

/*!
    \qmlproperty int TextEdit::font.pixelSize

    Sets the font size in pixels.

    Using this function makes the font device dependent.  Use
    \l{TextEdit::font.pointSize} to set the size of the font in a
    device independent manner.
*/

/*!
    \qmlproperty real TextEdit::font.letterSpacing

    Sets the letter spacing for the font.

    Letter spacing changes the default spacing between individual letters in the font.
    A positive value increases the letter spacing by the corresponding pixels; a negative value decreases the spacing.
*/

/*!
    \qmlproperty real TextEdit::font.wordSpacing

    Sets the word spacing for the font.

    Word spacing changes the default spacing between individual words.
    A positive value increases the word spacing by a corresponding amount of pixels,
    while a negative value decreases the inter-word spacing accordingly.
*/

/*!
    \qmlproperty enumeration TextEdit::font.capitalization

    Sets the capitalization for the text.

    \list
    \o Font.MixedCase - This is the normal text rendering option where no capitalization change is applied.
    \o Font.AllUppercase - This alters the text to be rendered in all uppercase type.
    \o Font.AllLowercase	 - This alters the text to be rendered in all lowercase type.
    \o Font.SmallCaps -	This alters the text to be rendered in small-caps type.
    \o Font.Capitalize - This alters the text to be rendered with the first character of each word as an uppercase character.
    \endlist

    \qml
    TextEdit { text: "Hello"; font.capitalization: Font.AllLowercase }
    \endqml
*/

/*!
    \qmlproperty string TextEdit::text

    The text to display.  If the text format is AutoText the text edit will
    automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().
*/
void QDeclarativeTextEdit::setText(const QString &text)
{
    Q_D(QDeclarativeTextEdit);
    if (QDeclarativeTextEdit::text() == text)
        return;
    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text));
    if (d->richText) {
#ifndef QT_NO_TEXTHTMLPARSER
        d->control->setHtml(text);
#else
        d->control->setPlainText(text);
#endif
    } else {
        d->control->setPlainText(text);
    }
    q_textChanged();
}

/*!
    \qmlproperty enumeration TextEdit::textFormat

    The way the text property should be displayed.

    \list
    \o TextEdit.AutoText
    \o TextEdit.PlainText
    \o TextEdit.RichText
    \o TextEdit.StyledText
    \endlist

    The default is TextEdit.AutoText.  If the text format is TextEdit.AutoText the text edit
    will automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().

    \table
    \row
    \o
    \qml
Column {
    TextEdit {
        font.pointSize: 24
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.pointSize: 24
        textFormat: TextEdit.RichText
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.pointSize: 24
        textFormat: TextEdit.PlainText
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \o \image declarative-textformat.png
    \endtable
*/
QDeclarativeTextEdit::TextFormat QDeclarativeTextEdit::textFormat() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->format;
}

void QDeclarativeTextEdit::setTextFormat(TextFormat format)
{
    Q_D(QDeclarativeTextEdit);
    if (format == d->format)
        return;
    bool wasRich = d->richText;
    d->richText = format == RichText || (format == AutoText && Qt::mightBeRichText(d->text));

    if (wasRich && !d->richText) {
        d->control->setPlainText(d->text);
        updateSize();
    } else if (!wasRich && d->richText) {
#ifndef QT_NO_TEXTHTMLPARSER
        d->control->setHtml(d->text);
#else
        d->control->setPlainText(d->text);
#endif
        updateSize();
    }
    d->format = format;
    emit textFormatChanged(d->format);
}

QFont QDeclarativeTextEdit::font() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->font;
}

void QDeclarativeTextEdit::setFont(const QFont &font)
{
    Q_D(QDeclarativeTextEdit);
    d->font = font;

    clearCache();
    d->document->setDefaultFont(d->font);
    if(d->cursor){
        d->cursor->setHeight(QFontMetrics(d->font).height());
        moveCursorDelegate();
    }
    updateSize();
    update();
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
QColor QDeclarativeTextEdit::color() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->color;
}

void QDeclarativeTextEdit::setColor(const QColor &color)
{
    Q_D(QDeclarativeTextEdit);
    if (d->color == color)
        return;

    clearCache();
    d->color = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Text, color);
    d->control->setPalette(pal);
    update();
    emit colorChanged(d->color);
}

/*!
    \qmlproperty color TextEdit::selectionColor

    The text highlight color, used behind selections.
*/
QColor QDeclarativeTextEdit::selectionColor() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->selectionColor;
}

void QDeclarativeTextEdit::setSelectionColor(const QColor &color)
{
    Q_D(QDeclarativeTextEdit);
    if (d->selectionColor == color)
        return;

    clearCache();
    d->selectionColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Highlight, color);
    d->control->setPalette(pal);
    update();
    emit selectionColorChanged(d->selectionColor);
}

/*!
    \qmlproperty color TextEdit::selectedTextColor

    The selected text color, used in selections.
*/
QColor QDeclarativeTextEdit::selectedTextColor() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->selectedTextColor;
}

void QDeclarativeTextEdit::setSelectedTextColor(const QColor &color)
{
    Q_D(QDeclarativeTextEdit);
    if (d->selectedTextColor == color)
        return;

    clearCache();
    d->selectedTextColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::HighlightedText, color);
    d->control->setPalette(pal);
    update();
    emit selectedTextColorChanged(d->selectedTextColor);
}

/*!
    \qmlproperty enumeration TextEdit::horizontalAlignment
    \qmlproperty enumeration TextEdit::verticalAlignment

    Sets the horizontal and vertical alignment of the text within the TextEdit item's
    width and height.  By default, the text is top-left aligned.

    Valid values for \c horizontalAlignment are:
    \list
    \o TextEdit.AlignLeft (default)
    \o TextEdit.AlignRight 
    \o TextEdit.AlignHCenter
    \endlist
    
    Valid values for \c verticalAlignment are:
    \list
    \o TextEdit.AlignTop (default)
    \o TextEdit.AlignBottom
    \c TextEdit.AlignVCenter
    \endlist
*/
QDeclarativeTextEdit::HAlignment QDeclarativeTextEdit::hAlign() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->hAlign;
}

void QDeclarativeTextEdit::setHAlign(QDeclarativeTextEdit::HAlignment alignment)
{
    Q_D(QDeclarativeTextEdit);
    if (alignment == d->hAlign)
        return;
    d->hAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    emit horizontalAlignmentChanged(d->hAlign);
}

QDeclarativeTextEdit::VAlignment QDeclarativeTextEdit::vAlign() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->vAlign;
}

void QDeclarativeTextEdit::setVAlign(QDeclarativeTextEdit::VAlignment alignment)
{
    Q_D(QDeclarativeTextEdit);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    emit verticalAlignmentChanged(d->vAlign);
}

/*!
    \qmlproperty enumeration TextEdit::wrapMode

    Set this property to wrap the text to the TextEdit item's width.
    The text will only wrap if an explicit width has been set.

    \list
    \o TextEdit.NoWrap - no wrapping will be performed. If the text contains insufficient newlines, then implicitWidth will exceed a set width.
    \o TextEdit.WordWrap - wrapping is done on word boundaries only. If a word is too long, implicitWidth will exceed a set width.
    \o TextEdit.WrapAnywhere - wrapping is done at any point on a line, even if it occurs in the middle of a word.
    \o TextEdit.Wrap - if possible, wrapping occurs at a word boundary; otherwise it will occur at the appropriate point on the line, even in the middle of a word.
    \endlist

    The default is TextEdit.NoWrap. If you set a width, consider using TextEdit.Wrap.
*/
QDeclarativeTextEdit::WrapMode QDeclarativeTextEdit::wrapMode() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->wrapMode;
}

void QDeclarativeTextEdit::setWrapMode(WrapMode mode)
{
    Q_D(QDeclarativeTextEdit);
    if (mode == d->wrapMode)
        return;
    d->wrapMode = mode;
    d->updateDefaultTextOption();
    updateSize();
    emit wrapModeChanged();
}

/*!
    \qmlproperty real TextEdit::paintedWidth

    Returns the width of the text, including the width past the width
    which is covered due to insufficient wrapping if \l wrapMode is set.
*/
qreal QDeclarativeTextEdit::paintedWidth() const
{
    return implicitWidth();
}

/*!
    \qmlproperty real TextEdit::paintedHeight

    Returns the height of the text, including the height past the height
    that is covered if the text does not fit within the set height.
*/
qreal QDeclarativeTextEdit::paintedHeight() const
{
    return implicitHeight();
}

/*!
    \qmlmethod rectangle TextEdit::positionToRectangle(position)

    Returns the rectangle at the given \a position in the text. The x, y,
    and height properties correspond to the cursor that would describe
    that position.
*/
QRectF QDeclarativeTextEdit::positionToRectangle(int pos) const
{
    Q_D(const QDeclarativeTextEdit);
    QTextCursor c(d->document);
    c.setPosition(pos);
    return d->control->cursorRect(c);

}

/*!
    \qmlmethod int TextEdit::positionAt(x,y)

    Returns the text position closest to pixel position (\a x, \a y).

    Position 0 is before the first character, position 1 is after the first character
    but before the second, and so on until position \l {text}.length, which is after all characters.
*/
int QDeclarativeTextEdit::positionAt(int x, int y) const
{
    Q_D(const QDeclarativeTextEdit);
    int r = d->document->documentLayout()->hitTest(QPoint(x,y-d->yoff), Qt::FuzzyHit);
    return r;
}

/*!
    \qmlmethod int TextEdit::moveCursorSelection(int pos)

    Moves the cursor to \a position and updates the selection accordingly.
    (To only move the cursor, set the \l cursorPosition property.)

    When this method is called it additionally sets either the
    selectionStart or the selectionEnd (whichever was at the previous cursor position)
    to the specified position. This allows you to easily extend and contract the selected
    text range.

    For example, take this sequence of calls:

    \code
        cursorPosition = 5
        moveCursorSelection(9)
        moveCursorSelection(7)
    \endcode

    This moves the cursor to position 5, extend the selection end from 5 to 9
    and then retract the selection end from 9 to 7, leaving the text from position 5 to 7
    selected (the 6th and 7th characters).
*/
void QDeclarativeTextEdit::moveCursorSelection(int pos)
{
    //Note that this is the same as setCursorPosition but with the KeepAnchor flag set
    Q_D(QDeclarativeTextEdit);
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    d->control->setTextCursor(cursor);
}

/*!
    \qmlproperty bool TextEdit::cursorVisible
    If true the text edit shows a cursor.

    This property is set and unset when the text edit gets active focus, but it can also
    be set directly (useful, for example, if a KeyProxy might forward keys to it).
*/
bool QDeclarativeTextEdit::isCursorVisible() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->cursorVisible;
}

void QDeclarativeTextEdit::setCursorVisible(bool on)
{
    Q_D(QDeclarativeTextEdit);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    QFocusEvent focusEvent(on ? QEvent::FocusIn : QEvent::FocusOut);
    if (!on && !d->persistentSelection)
        d->control->setCursorIsFocusIndicator(true);
    d->control->processEvent(&focusEvent, QPointF(0, -d->yoff));
    emit cursorVisibleChanged(d->cursorVisible);
}

/*!
    \qmlproperty int TextEdit::cursorPosition
    The position of the cursor in the TextEdit.
*/
int QDeclarativeTextEdit::cursorPosition() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().position();
}

void QDeclarativeTextEdit::setCursorPosition(int pos)
{
    Q_D(QDeclarativeTextEdit);
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos);
    d->control->setTextCursor(cursor);
}

/*!
    \qmlproperty Component TextEdit::cursorDelegate
    The delegate for the cursor in the TextEdit.

    If you set a cursorDelegate for a TextEdit, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the text edit when a cursor is
    needed, and the x and y properties of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QDeclarativeItem or
    QDeclarativeItem derived item.
*/
QDeclarativeComponent* QDeclarativeTextEdit::cursorDelegate() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->cursorComponent;
}

void QDeclarativeTextEdit::setCursorDelegate(QDeclarativeComponent* c)
{
    Q_D(QDeclarativeTextEdit);
    if(d->cursorComponent){
        if(d->cursor){
            disconnect(d->control, SIGNAL(cursorPositionChanged()),
                    this, SLOT(moveCursorDelegate()));
            d->control->setCursorWidth(-1);
            dirtyCache(cursorRectangle());
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

    emit cursorDelegateChanged();
}

void QDeclarativeTextEdit::loadCursorDelegate()
{
    Q_D(QDeclarativeTextEdit);
    if(d->cursorComponent->isLoading())
        return;
    d->cursor = qobject_cast<QDeclarativeItem*>(d->cursorComponent->create(qmlContext(this)));
    if(d->cursor){
        connect(d->control, SIGNAL(cursorPositionChanged()),
                this, SLOT(moveCursorDelegate()));
        d->control->setCursorWidth(0);
        dirtyCache(cursorRectangle());
        QDeclarative_setParent_noEvent(d->cursor, this);
        d->cursor->setParentItem(this);
        d->cursor->setHeight(QFontMetrics(d->font).height());
        moveCursorDelegate();
    }else{
        qmlInfo(this) << "Error loading cursor delegate.";
    }
}

/*!
    \qmlproperty int TextEdit::selectionStart

    The cursor position before the first character in the current selection.

    This property is read-only. To change the selection, use select(start,end),
    selectAll(), or selectWord().

    \sa selectionEnd, cursorPosition, selectedText
*/
int QDeclarativeTextEdit::selectionStart() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectionStart();
}

/*!
    \qmlproperty int TextEdit::selectionEnd

    The cursor position after the last character in the current selection.

    This property is read-only. To change the selection, use select(start,end),
    selectAll(), or selectWord().

    \sa selectionStart, cursorPosition, selectedText
*/
int QDeclarativeTextEdit::selectionEnd() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectionEnd();
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
QString QDeclarativeTextEdit::selectedText() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectedText();
}

/*!
    \qmlproperty bool TextEdit::activeFocusOnPress

    Whether the TextEdit should gain active focus on a mouse press. By default this is
    set to true.
*/
bool QDeclarativeTextEdit::focusOnPress() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->focusOnPress;
}

void QDeclarativeTextEdit::setFocusOnPress(bool on)
{
    Q_D(QDeclarativeTextEdit);
    if (d->focusOnPress == on)
        return;
    d->focusOnPress = on;
    emit activeFocusOnPressChanged(d->focusOnPress);
}

/*!
    \qmlproperty bool TextEdit::persistentSelection

    Whether the TextEdit should keep the selection visible when it loses active focus to another
    item in the scene. By default this is set to true;
*/
bool QDeclarativeTextEdit::persistentSelection() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->persistentSelection;
}

void QDeclarativeTextEdit::setPersistentSelection(bool on)
{
    Q_D(QDeclarativeTextEdit);
    if (d->persistentSelection == on)
        return;
    d->persistentSelection = on;
    emit persistentSelectionChanged(d->persistentSelection);
}

/*
   \qmlproperty real TextEdit::textMargin

   The margin, in pixels, around the text in the TextEdit.
*/
qreal QDeclarativeTextEdit::textMargin() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->textMargin;
}

void QDeclarativeTextEdit::setTextMargin(qreal margin)
{
    Q_D(QDeclarativeTextEdit);
    if (d->textMargin == margin)
        return;
    d->textMargin = margin;
    d->document->setDocumentMargin(d->textMargin);
    emit textMarginChanged(d->textMargin);
}

void QDeclarativeTextEdit::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QDeclarativePaintedItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
    Ensures any delayed caching or data loading the class
    needs to performed is complete.
*/
void QDeclarativeTextEdit::componentComplete()
{
    Q_D(QDeclarativeTextEdit);
    QDeclarativePaintedItem::componentComplete();
    if (d->dirty) {
        updateSize();
        d->dirty = false;
    }
}

/*!
    \qmlproperty bool TextEdit::selectByMouse

    Defaults to false.

    If true, the user can use the mouse to select text in some
    platform-specific way. Note that for some platforms this may
    not be an appropriate interaction (eg. may conflict with how
    the text needs to behave inside a Flickable.
*/
bool QDeclarativeTextEdit::selectByMouse() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->selectByMouse;
}

void QDeclarativeTextEdit::setSelectByMouse(bool on)
{
    Q_D(QDeclarativeTextEdit);
    if (d->selectByMouse != on) {
        d->selectByMouse = on;
        emit selectByMouseChanged(on);
    }
}



/*!
    \qmlproperty bool TextEdit::readOnly

    Whether the user an interact with the TextEdit item. If this
    property is set to true the text cannot be edited by user interaction.

    By default this property is false.
*/
void QDeclarativeTextEdit::setReadOnly(bool r)
{
    Q_D(QDeclarativeTextEdit);
    if (r == isReadOnly())
        return;


    Qt::TextInteractionFlags flags = Qt::NoTextInteraction;
    if (r) {
        flags = Qt::TextSelectableByMouse;
    } else {
        flags = Qt::TextEditorInteraction;
    }
    d->control->setTextInteractionFlags(flags);
    if (!r)
        d->control->moveCursor(QTextCursor::End);

    emit readOnlyChanged(r);
}

bool QDeclarativeTextEdit::isReadOnly() const
{
    Q_D(const QDeclarativeTextEdit);
    return !(d->control->textInteractionFlags() & Qt::TextEditable);
}

/*!
    Sets how the text edit should interact with user input to the given
    \a flags.
*/
void QDeclarativeTextEdit::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(QDeclarativeTextEdit);
    d->control->setTextInteractionFlags(flags);
}

/*!
    Returns the flags specifying how the text edit should interact
    with user input.
*/
Qt::TextInteractionFlags QDeclarativeTextEdit::textInteractionFlags() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textInteractionFlags();
}

/*!
    \qmlproperty rectangle TextEdit::cursorRectangle

    The rectangle where the text cursor is rendered
    within the text edit. Read-only.
*/
QRect QDeclarativeTextEdit::cursorRectangle() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->cursorRect().toRect().translated(0,-d->yoff);
}


/*!
\overload
Handles the given \a event.
*/
bool QDeclarativeTextEdit::event(QEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(0, -d->yoff));
        return event->isAccepted();
    }
    return QDeclarativePaintedItem::event(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QDeclarativeTextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    keyPressPreHandler(event);
    if (!event->isAccepted())
        d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QDeclarativePaintedItem::keyPressEvent(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QDeclarativeTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    keyReleasePreHandler(event);
    if (!event->isAccepted())
        d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QDeclarativePaintedItem::keyReleaseEvent(event);
}

void QDeclarativeTextEditPrivate::focusChanged(bool hasFocus)
{
    Q_Q(QDeclarativeTextEdit);
    q->setCursorVisible(hasFocus);
    QDeclarativeItemPrivate::focusChanged(hasFocus);
}

/*!
    \qmlmethod void TextEdit::selectAll()

    Causes all text to be selected.
*/
void QDeclarativeTextEdit::selectAll()
{
    Q_D(QDeclarativeTextEdit);
    d->control->selectAll();
}

/*!
    \qmlmethod void TextEdit::selectWord()

    Causes the word closest to the current cursor position to be selected.
*/
void QDeclarativeTextEdit::selectWord()
{
    Q_D(QDeclarativeTextEdit);
    QTextCursor c = d->control->textCursor();
    c.select(QTextCursor::WordUnderCursor);
    d->control->setTextCursor(c);
}

/*!
    \qmlmethod void TextEdit::select(start,end)

    Causes the text from \a start to \a end to be selected.

    If either start or end is out of range, the selection is not changed.

    After calling this, selectionStart will become the lesser
    and selectionEnd will become the greater (regardless of the order passed
    to this method).

    \sa selectionStart, selectionEnd
*/
void QDeclarativeTextEdit::select(int start, int end)
{
    Q_D(QDeclarativeTextEdit);
    if (start < 0 || end < 0 || start > d->text.length() || end > d->text.length())
        return;
    QTextCursor cursor = d->control->textCursor();
    cursor.beginEditBlock();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    d->control->setTextCursor(cursor);

    // QTBUG-11100
    updateSelectionMarkers();
}

#ifndef QT_NO_CLIPBOARD
/*!
    \qmlmethod TextEdit::cut()

    Moves the currently selected text to the system clipboard.
*/
void QDeclarativeTextEdit::cut()
{
    Q_D(QDeclarativeTextEdit);
    d->control->cut();
}

/*!
    \qmlmethod TextEdit::copy()

    Copies the currently selected text to the system clipboard.
*/
void QDeclarativeTextEdit::copy()
{
    Q_D(QDeclarativeTextEdit);
    d->control->copy();
}

/*!
    \qmlmethod TextEdit::paste()

    Replaces the currently selected text by the contents of the system clipboard.
*/
void QDeclarativeTextEdit::paste()
{
    Q_D(QDeclarativeTextEdit);
    d->control->paste();
}
#endif // QT_NO_CLIPBOARD

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    if (d->focusOnPress){
        bool hadActiveFocus = hasActiveFocus();
        forceActiveFocus();
        if (d->showInputPanelOnFocus) {
            if (hasActiveFocus() && hadActiveFocus && !isReadOnly()) {
                // re-open input panel on press if already focused
                openSoftwareInputPanel();
            }
        } else { // show input panel on click
            if (hasActiveFocus() && !hadActiveFocus) {
                d->clickCausedFocus = true;
            }
        }
    }
    if (event->type() != QEvent::GraphicsSceneMouseDoubleClick || d->selectByMouse)
        d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QDeclarativePaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!d->showInputPanelOnFocus) { // input panel on click
        if (d->focusOnPress && !isReadOnly() && boundingRect().contains(event->pos())) {
            if (QGraphicsView * view = qobject_cast<QGraphicsView*>(qApp->focusWidget())) {
                if (view->scene() && view->scene() == scene()) {
                    qt_widget_private(view)->handleSoftwareInputPanel(event->button(), d->clickCausedFocus);
                }
            }
        }
    }
    d->clickCausedFocus = false;

    if (!event->isAccepted())
        QDeclarativePaintedItem::mouseReleaseEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextEdit::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    if (d->selectByMouse) {
        d->control->processEvent(event, QPointF(0, -d->yoff));
        if (!event->isAccepted())
            QDeclarativePaintedItem::mouseDoubleClickEvent(event);
    } else {
        QDeclarativePaintedItem::mouseDoubleClickEvent(event);
    }
}

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    if (d->selectByMouse) {
        d->control->processEvent(event, QPointF(0, -d->yoff));
        if (!event->isAccepted())
            QDeclarativePaintedItem::mouseMoveEvent(event);
        event->setAccepted(true);
    } else {
        QDeclarativePaintedItem::mouseMoveEvent(event);
    }
}

/*!
\overload
Handles the given input method \a event.
*/
void QDeclarativeTextEdit::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
}

/*!
\overload
Returns the value of the given \a property.
*/
QVariant QDeclarativeTextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->inputMethodQuery(property);
}

/*!
Draws the contents of the text edit using the given \a painter within
the given \a bounds.
*/
void QDeclarativeTextEdit::drawContents(QPainter *painter, const QRect &bounds)
{
    Q_D(QDeclarativeTextEdit);

    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->translate(0,d->yoff);

    d->control->drawContents(painter, bounds.translated(0,-d->yoff));

    painter->translate(0,-d->yoff);
}

void QDeclarativeTextEdit::updateImgCache(const QRectF &rf)
{
    Q_D(const QDeclarativeTextEdit);
    QRect r;
    if (!rf.isValid()) {
        r = QRect(0,0,INT_MAX,INT_MAX);
    } else {
        r = rf.toRect();
        if (r.height() > INT_MAX/2) {
            // Take care of overflow when translating "everything"
            r.setTop(r.y() + d->yoff);
            r.setBottom(INT_MAX/2);
        } else {
            r = r.translated(0,d->yoff);
        }
    }
    dirtyCache(r);
    emit update();
}

/*!
    \qmlproperty bool TextEdit::smooth

    This property holds whether the text is smoothly scaled or transformed.

    Smooth filtering gives better visual quality, but is slower.  If
    the item is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the item is stationary on
    the screen.  A common pattern when animating an item is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QDeclarativeTextEditPrivate::init()
{
    Q_Q(QDeclarativeTextEdit);

    q->setSmooth(smooth);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
    q->setFlag(QGraphicsItem::ItemAcceptsInputMethod);

    control = new QTextControl(q);
    control->setIgnoreUnusedNavigationEvents(true);

    // QTextControl follows the default text color
    // defined by the platform, declarative text
    // should be black by default
    QPalette pal = control->palette();
    if (pal.color(QPalette::Text) != color) {
        pal.setColor(QPalette::Text, color);
        control->setPalette(pal);
    }

    QObject::connect(control, SIGNAL(updateRequest(QRectF)), q, SLOT(updateImgCache(QRectF)));

    QObject::connect(control, SIGNAL(textChanged()), q, SLOT(q_textChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SIGNAL(selectionChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SIGNAL(cursorPositionChanged()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SIGNAL(cursorRectangleChanged()));

    document = control->document();
    document->setDefaultFont(font);
    document->setDocumentMargin(textMargin);
    document->setUndoRedoEnabled(false); // flush undo buffer.
    document->setUndoRedoEnabled(true);
    updateDefaultTextOption();
}

void QDeclarativeTextEdit::q_textChanged()
{
    Q_D(QDeclarativeTextEdit);
    d->text = text();
    updateSize();
    updateMicroFocus();
    emit textChanged(d->text);
}

void QDeclarativeTextEdit::moveCursorDelegate()
{
    Q_D(QDeclarativeTextEdit);
    if(!d->cursor)
        return;
    QRectF cursorRect = d->control->cursorRect();
    d->cursor->setX(cursorRect.x());
    d->cursor->setY(cursorRect.y());
}

void QDeclarativeTextEditPrivate::updateSelection()
{
    Q_Q(QDeclarativeTextEdit);
    QTextCursor cursor = control->textCursor();
    bool startChange = (lastSelectionStart != cursor.selectionStart());
    bool endChange = (lastSelectionEnd != cursor.selectionEnd());
    cursor.beginEditBlock();
    cursor.setPosition(lastSelectionStart, QTextCursor::MoveAnchor);
    cursor.setPosition(lastSelectionEnd, QTextCursor::KeepAnchor);
    cursor.endEditBlock();
    control->setTextCursor(cursor);
    if(startChange)
        q->selectionStartChanged();
    if(endChange)
        q->selectionEndChanged();
}

void QDeclarativeTextEdit::updateSelectionMarkers()
{
    Q_D(QDeclarativeTextEdit);
    if(d->lastSelectionStart != d->control->textCursor().selectionStart()){
        d->lastSelectionStart = d->control->textCursor().selectionStart();
        emit selectionStartChanged();
    }
    if(d->lastSelectionEnd != d->control->textCursor().selectionEnd()){
        d->lastSelectionEnd = d->control->textCursor().selectionEnd();
        emit selectionEndChanged();
    }
    updateMicroFocus();
}

QRectF QDeclarativeTextEdit::boundingRect() const
{
    Q_D(const QDeclarativeTextEdit);
    QRectF r = QDeclarativePaintedItem::boundingRect();
    int cursorWidth = 1;
    if(d->cursor)
        cursorWidth = d->cursor->width();
    if(!d->document->isEmpty())
        cursorWidth += 3;// ### Need a better way of accounting for space between char and cursor

    // Could include font max left/right bearings to either side of rectangle.

    r.setRight(r.right() + cursorWidth);
    return r.translated(0,d->yoff);
}


//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void QDeclarativeTextEdit::updateSize()
{
    Q_D(QDeclarativeTextEdit);
    if (isComponentComplete()) {
        QFontMetrics fm = QFontMetrics(d->font);
        int dy = height();
        // ### assumes that if the width is set, the text will fill to edges
        // ### (unless wrap is false, then clipping will occur)
        if (widthValid() && d->document->textWidth() != width())
            d->document->setTextWidth(width());
        dy -= (int)d->document->size().height();

        int nyoff;
        if (heightValid()) {
            if (d->vAlign == AlignBottom)
                nyoff = dy;
            else if (d->vAlign == AlignVCenter)
                nyoff = dy/2;
            else
                nyoff = 0;
        } else {
            nyoff = 0;
        }
        if (nyoff != d->yoff) {
            prepareGeometryChange();
            d->yoff = nyoff;
        }
        setBaselineOffset(fm.ascent() + d->yoff + d->textMargin);

        //### need to comfirm cost of always setting these
        int newWidth = qCeil(d->document->idealWidth());
        if (!widthValid() && d->document->textWidth() != newWidth)
            d->document->setTextWidth(newWidth); // ### Text does not align if width is not set (QTextDoc bug)
        // ### Setting the implicitWidth triggers another updateSize(), and unless there are bindings nothing has changed.
        setImplicitWidth(newWidth);
        qreal newHeight = d->document->isEmpty() ? fm.height() : (int)d->document->size().height();
        setImplicitHeight(newHeight);

        setContentsSize(QSize(newWidth, newHeight));

        emit paintedSizeChanged();
    } else {
        d->dirty = true;
    }
    emit update();
}

void QDeclarativeTextEditPrivate::updateDefaultTextOption()
{
    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();
    opt.setAlignment((Qt::Alignment)(int)(hAlign | vAlign));

    QTextOption::WrapMode oldWrapMode = opt.wrapMode();
    opt.setWrapMode(QTextOption::WrapMode(wrapMode));

    if (oldWrapMode == opt.wrapMode() && oldAlignment == opt.alignment())
        return;
    document->setDefaultTextOption(opt);
}


/*!
    \qmlmethod void TextEdit::openSoftwareInputPanel()

    Opens software input panels like virtual keyboards for typing, useful for
    customizing when you want the input keyboard to be shown and hidden in
    your application.

    By default the opening of input panels follows the platform style. On Symbian^1 and
    Symbian^3 -based devices the panels are opened by clicking TextEdit. On other platforms
    the panels are automatically opened when TextEdit element gains active focus. Input panels are
    always closed if no editor has active focus.

    You can disable the automatic behavior by setting the property \c activeFocusOnPress to false
    and use functions openSoftwareInputPanel() and closeSoftwareInputPanel() to implement
    the behavior you want.

    Only relevant on platforms, which provide virtual keyboards.

    \code
        import Qt 4.7
        TextEdit {
            id: textEdit
            text: "Hello world!"
            activeFocusOnPress: false
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!textEdit.activeFocus) {
                        textEdit.forceActiveFocus();
                        textEdit.openSoftwareInputPanel();
                    } else {
                        textEdit.focus = false;
                    }
                }
                onPressAndHold: textEdit.closeSoftwareInputPanel();
            }
        }
    \endcode
*/
void QDeclarativeTextEdit::openSoftwareInputPanel()
{
    QEvent event(QEvent::RequestSoftwareInputPanel);
    if (qApp) {
        if (QGraphicsView * view = qobject_cast<QGraphicsView*>(qApp->focusWidget())) {
            if (view->scene() && view->scene() == scene()) {
                QApplication::sendEvent(view, &event);
            }
        }
    }
}

/*!
    \qmlmethod void TextEdit::closeSoftwareInputPanel()

    Closes a software input panel like a virtual keyboard shown on the screen, useful
    for customizing when you want the input keyboard to be shown and hidden in
    your application.

    By default the opening of input panels follows the platform style. On Symbian^1 and
    Symbian^3 -based devices the panels are opened by clicking TextEdit. On other platforms
    the panels are automatically opened when TextEdit element gains active focus. Input panels are
    always closed if no editor has active focus.

    You can disable the automatic behavior by setting the property \c activeFocusOnPress to false
    and use functions openSoftwareInputPanel() and closeSoftwareInputPanel() to implement
    the behavior you want.

    Only relevant on platforms, which provide virtual keyboards.

    \code
        import Qt 4.7
        TextEdit {
            id: textEdit
            text: "Hello world!"
            activeFocusOnPress: false
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (!textEdit.activeFocus) {
                        textEdit.forceActiveFocus();
                        textEdit.openSoftwareInputPanel();
                    } else {
                        textEdit.focus = false;
                    }
                }
                onPressAndHold: textEdit.closeSoftwareInputPanel();
            }
        }
    \endcode
*/
void QDeclarativeTextEdit::closeSoftwareInputPanel()
{
    QEvent event(QEvent::CloseSoftwareInputPanel);
    if (qApp) {
        if (QGraphicsView * view = qobject_cast<QGraphicsView*>(qApp->focusWidget())) {
            if (view->scene() && view->scene() == scene()) {
                QApplication::sendEvent(view, &event);
            }
        }
    }
}

void QDeclarativeTextEdit::focusInEvent(QFocusEvent *event)
{
    Q_D(const QDeclarativeTextEdit);
    if (d->showInputPanelOnFocus) {
        if (d->focusOnPress && !isReadOnly()) {
            openSoftwareInputPanel();
        }
    }
    QDeclarativePaintedItem::focusInEvent(event);
}

QT_END_NAMESPACE
