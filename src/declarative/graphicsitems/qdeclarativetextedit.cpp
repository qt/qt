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
    \brief The TextEdit item allows you to add editable formatted text to a scene.

    It can display both plain and rich text. For example:

    \qml
TextEdit {
    id: edit
    text: "<b>Hello</b> <i>World!</i>"
    focus: true
    font.family: "Helvetica"
    font.pointSize: 20
    color: "blue"
    width: 240
}
    \endqml

    \image declarative-textedit.gif

    \sa Text
*/

/*!
    \internal
    \class QDeclarativeTextEdit
    \qmlclass TextEdit

    \brief The QDeclarativeTextEdit class provides an editable formatted text item that you can add to a QDeclarativeView.

    It can display both plain and rich text.

    \image declarative-textedit.png

    A QDeclarativeTextEdit object can be instantiated in Qml using the tag \c &lt;TextEdit&gt;.
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

    if (d->richText)
        return d->document->toHtml();
    else
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
    \qmlproperty bool TextEdit::font.outline

    Sets whether the font has an outline style.
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

    Using this function makes the font device dependent.
    Use \c pointSize to set the size of the font in a device independent manner.
*/

/*!
    \qmlproperty real TextEdit::font.letterSpacing

    Sets the letter spacing for the font.

    Letter spacing changes the default spacing between individual letters in the font.
    A value of 100 will keep the spacing unchanged; a value of 200 will enlarge the spacing after a character by
    the width of the character itself.
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
    d->text = text;
    d->richText = d->format == RichText || (d->format == AutoText && Qt::mightBeRichText(text));
    if (d->richText) {
        d->control->setHtml(text);
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
        d->control->setHtml(d->text);
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

    Sets the horizontal and vertical alignment of the text within the TextEdit items
    width and height.  By default, the text is top-left aligned.

    The valid values for \c horizontalAlignment are \c TextEdit.AlignLeft, \c TextEdit.AlignRight and
    \c TextEdit.AlignHCenter.  The valid values for \c verticalAlignment are \c TextEdit.AlignTop, \c TextEdit.AlignBottom
    and \c TextEdit.AlignVCenter.
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
    \o TextEdit.NoWrap - no wrapping will be performed.
    \o TextEdit.WordWrap - wrapping is done on word boundaries.
    \o TextEdit.WrapAnywhere - Text can be wrapped at any point on a line, even if it occurs in the middle of a word.
    \o TextEdit.WrapAtWordBoundaryOrAnywhere - If possible, wrapping occurs at a word boundary; otherwise it
       will occur at the appropriate point on the line, even in the middle of a word.
    \endlist

    The default is TextEdit.NoWrap.
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
    \qmlproperty bool TextEdit::cursorVisible
    If true the text edit shows a cursor.

    This property is set and unset when the text edit gets focus, but it can also
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
        dirtyCache(cursorRect());
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
    Setting this and selectionEnd allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionStart to a value outside of
    the current text, selectionStart will not be changed.

    \sa selectionEnd, cursorPosition, selectedText
*/
int QDeclarativeTextEdit::selectionStart() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectionStart();
}

void QDeclarativeTextEdit::setSelectionStart(int s)
{
    Q_D(QDeclarativeTextEdit);
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
int QDeclarativeTextEdit::selectionEnd() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectionEnd();
}

void QDeclarativeTextEdit::setSelectionEnd(int s)
{
    Q_D(QDeclarativeTextEdit);
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
QString QDeclarativeTextEdit::selectedText() const
{
    Q_D(const QDeclarativeTextEdit);
    return d->control->textCursor().selectedText();
}

/*!
    \qmlproperty bool TextEdit::focusOnPress

    Whether the TextEdit should gain focus on a mouse press. By default this is
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
    emit focusOnPressChanged(d->focusOnPress);
}

/*!
    \qmlproperty bool TextEdit::persistentSelection

    Whether the TextEdit should keep the selection visible when it loses focus to another
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
    \qmlproperty string TextEdit::selectByMouse

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
    Returns the rectangle where the text cursor is rendered
    within the text edit.
*/
QRect QDeclarativeTextEdit::cursorRect() const
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
    Causes all text to be selected.
*/
void QDeclarativeTextEdit::selectAll()
{
    Q_D(QDeclarativeTextEdit);
    d->control->selectAll();
}

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextEdit);
    bool hadFocus = hasFocus();
    if (d->focusOnPress){
        QGraphicsItem *p = parentItem();//###Is there a better way to find my focus scope?
        while(p) {
            if (p->flags() & QGraphicsItem::ItemIsFocusScope)
                p->setFocus();
            p = p->parentItem();
        }
        setFocus(true);
    }
    if (!hadFocus && hasFocus())
        d->clickCausedFocus = true;
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
    QWidget *widget = event->widget();
    if (widget && (d->control->textInteractionFlags() & Qt::TextEditable) && boundingRect().contains(event->pos()))
        qt_widget_private(widget)->handleSoftwareInputPanel(event->button(), d->clickCausedFocus);
    d->clickCausedFocus = false;

    d->control->processEvent(event, QPointF(0, -d->yoff));
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
    QRect r = rf.toRect();
    if (r != QRect(0,0,INT_MAX,INT_MAX)) // Don't translate "everything"
        r = r.translated(0,d->yoff);
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

    QObject::connect(control, SIGNAL(updateRequest(QRectF)), q, SLOT(updateImgCache(QRectF)));

    QObject::connect(control, SIGNAL(textChanged()), q, SLOT(q_textChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SIGNAL(selectionChanged()));
    QObject::connect(control, SIGNAL(selectionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SLOT(updateSelectionMarkers()));
    QObject::connect(control, SIGNAL(cursorPositionChanged()), q, SIGNAL(cursorPositionChanged()));

    document = control->document();
    document->setDefaultFont(font);
    document->setDocumentMargin(textMargin);
    document->setUndoRedoEnabled(false); // flush undo buffer.
    document->setUndoRedoEnabled(true);
    updateDefaultTextOption();
}

void QDeclarativeTextEdit::q_textChanged()
{
    updateSize();
    emit textChanged(text());
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
        if (widthValid())
            d->document->setTextWidth(width());
        dy -= (int)d->document->size().height();

        if (heightValid()) {
            if (d->vAlign == AlignBottom)
                d->yoff = dy;
            else if (d->vAlign == AlignVCenter)
                d->yoff = dy/2;
        } else {
            d->yoff = 0;
        }
        setBaselineOffset(fm.ascent() + d->yoff + d->textMargin);

        //### need to comfirm cost of always setting these
        int newWidth = qCeil(d->document->idealWidth());
        if (!widthValid())
            d->document->setTextWidth(newWidth); // ### Text does not align if width is not set (QTextDoc bug)
        int cursorWidth = 1;
        if(d->cursor)
            cursorWidth = d->cursor->width();
        newWidth += cursorWidth;
        if(!d->document->isEmpty())
            newWidth += 3;// ### Need a better way of accounting for space between char and cursor
        // ### Setting the implicitWidth triggers another updateSize(), and unless there are bindings nothing has changed.
        setImplicitWidth(newWidth);
        setImplicitHeight(d->text.isEmpty() ? fm.height() : (int)d->document->size().height());

        setContentsSize(QSize(width(), height()));
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

QT_END_NAMESPACE
