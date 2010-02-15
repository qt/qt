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

#include "qmlgraphicstextedit_p.h"
#include "qmlgraphicstextedit_p_p.h"

#include "qmlgraphicsevents_p_p.h"

#include <qfxperf_p_p.h>

#include <QTextLayout>
#include <QTextLine>
#include <QTextDocument>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPainter>

#include <private/qtextcontrol_p.h>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass TextEdit QmlGraphicsTextEdit
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
    \class QmlGraphicsTextEdit
    \qmlclass TextEdit
    \ingroup group_coreitems

    \brief The QmlGraphicsTextEdit class provides an editable formatted text item that you can add to a QmlView.

    It can display both plain and rich text.

    \image declarative-textedit.png

    A QmlGraphicsTextEdit object can be instantiated in Qml using the tag \c &lt;TextEdit&gt;.
*/

/*!
    Constructs a new QmlGraphicsTextEdit.
*/
QmlGraphicsTextEdit::QmlGraphicsTextEdit(QmlGraphicsItem *parent)
: QmlGraphicsPaintedItem(*(new QmlGraphicsTextEditPrivate), parent)
{
    Q_D(QmlGraphicsTextEdit);
    d->init();
}

QString QmlGraphicsTextEdit::text() const
{
    Q_D(const QmlGraphicsTextEdit);

    if (d->richText)
        return d->document->toHtml();
    else
        return d->document->toPlainText();
}

/*!
    \qmlproperty string TextEdit::font.family
    \qmlproperty bool TextEdit::font.bold
    \qmlproperty bool TextEdit::font.italic
    \qmlproperty bool TextEdit::font.underline
    \qmlproperty real TextEdit::font.pointSize
    \qmlproperty int TextEdit::font.pixelSize

    Set the TextEdit's font attributes.
*/

/*!
    \qmlproperty string TextEdit::text

    The text to display.  If the text format is AutoText the text edit will
    automatically determine whether the text should be treated as
    rich text.  This determination is made using Qt::mightBeRichText().
*/
void QmlGraphicsTextEdit::setText(const QString &text)
{
    Q_D(QmlGraphicsTextEdit);
    if (QmlGraphicsTextEdit::text() == text)
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

    Supported text formats are \c AutoText, \c PlainText and \c RichText.

    The default is AutoText.  If the text format is AutoText the text edit
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
        textFormat: "RichText"
        text: "<b>Hello</b> <i>World!</i>"
    }
    TextEdit {
        font.pointSize: 24
        textFormat: "PlainText"
        text: "<b>Hello</b> <i>World!</i>"
    }
}
    \endqml
    \o \image declarative-textformat.png
    \endtable
*/
QmlGraphicsTextEdit::TextFormat QmlGraphicsTextEdit::textFormat() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->format;
}

void QmlGraphicsTextEdit::setTextFormat(TextFormat format)
{
    Q_D(QmlGraphicsTextEdit);
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

QFont QmlGraphicsTextEdit::font() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->font;
}

void QmlGraphicsTextEdit::setFont(const QFont &font)
{
    Q_D(QmlGraphicsTextEdit);
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
QColor QmlGraphicsTextEdit::color() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->color;
}

void QmlGraphicsTextEdit::setColor(const QColor &color)
{
    Q_D(QmlGraphicsTextEdit);
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
QColor QmlGraphicsTextEdit::selectionColor() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->selectionColor;
}

void QmlGraphicsTextEdit::setSelectionColor(const QColor &color)
{
    Q_D(QmlGraphicsTextEdit);
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
QColor QmlGraphicsTextEdit::selectedTextColor() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->selectedTextColor;
}

void QmlGraphicsTextEdit::setSelectedTextColor(const QColor &color)
{
    Q_D(QmlGraphicsTextEdit);
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

    The valid values for \c horizontalAlignment are \c AlignLeft, \c AlignRight and
    \c AlignHCenter.  The valid values for \c verticalAlignment are \c AlignTop, \c AlignBottom
    and \c AlignVCenter.
*/
QmlGraphicsTextEdit::HAlignment QmlGraphicsTextEdit::hAlign() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->hAlign;
}

void QmlGraphicsTextEdit::setHAlign(QmlGraphicsTextEdit::HAlignment alignment)
{
    Q_D(QmlGraphicsTextEdit);
    if (alignment == d->hAlign)
        return;
    d->hAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    emit horizontalAlignmentChanged(d->hAlign);
}

QmlGraphicsTextEdit::VAlignment QmlGraphicsTextEdit::vAlign() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->vAlign;
}

void QmlGraphicsTextEdit::setVAlign(QmlGraphicsTextEdit::VAlignment alignment)
{
    Q_D(QmlGraphicsTextEdit);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    emit verticalAlignmentChanged(d->vAlign);
}

bool QmlGraphicsTextEdit::wrap() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->wrap;
}

/*!
    \qmlproperty bool TextEdit::wrap

    Set this property to wrap the text to the TextEdit item's width.
    The text will only wrap if an explicit width has been set.

    Wrapping is done on word boundaries (i.e. it is a "word-wrap"). Wrapping is off by default.
*/
void QmlGraphicsTextEdit::setWrap(bool w)
{
    Q_D(QmlGraphicsTextEdit);
    if (w == d->wrap)
        return;
    d->wrap = w;
    d->updateDefaultTextOption();
    updateSize();
    emit wrapChanged(d->wrap);
}

/*!
    \qmlproperty bool TextEdit::cursorVisible
    If true the text edit shows a cursor.

    This property is set and unset when the text edit gets focus, but it can also
    be set directly (useful, for example, if a KeyProxy might forward keys to it).
*/
bool QmlGraphicsTextEdit::isCursorVisible() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->cursorVisible;
}

void QmlGraphicsTextEdit::setCursorVisible(bool on)
{
    Q_D(QmlGraphicsTextEdit);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    QFocusEvent focusEvent(on ? QEvent::FocusIn : QEvent::FocusOut);
    if (!on && !d->persistentSelection)
        d->control->setCursorIsFocusIndicator(true);
    d->control->processEvent(&focusEvent, QPointF(0, 0));
    emit cursorVisibleChanged(d->cursorVisible);
}

/*!
    \qmlproperty int TextEdit::cursorPosition
    The position of the cursor in the TextEdit.
*/
int QmlGraphicsTextEdit::cursorPosition() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->textCursor().position();
}

void QmlGraphicsTextEdit::setCursorPosition(int pos)
{
    Q_D(QmlGraphicsTextEdit);
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

    Note that the root item of the delegate component must be a QmlGraphicsItem or
    QmlGraphicsItem derived item.
*/
QmlComponent* QmlGraphicsTextEdit::cursorDelegate() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->cursorComponent;
}

void QmlGraphicsTextEdit::setCursorDelegate(QmlComponent* c)
{
    Q_D(QmlGraphicsTextEdit);
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

void QmlGraphicsTextEdit::loadCursorDelegate()
{
    Q_D(QmlGraphicsTextEdit);
    if(d->cursorComponent->isLoading())
        return;
    d->cursor = qobject_cast<QmlGraphicsItem*>(d->cursorComponent->create(qmlContext(this)));
    if(d->cursor){
        connect(d->control, SIGNAL(cursorPositionChanged()),
                this, SLOT(moveCursorDelegate()));
        d->control->setCursorWidth(0);
        dirtyCache(cursorRect());
        d->cursor->setParentItem(this);
        d->cursor->setHeight(QFontMetrics(d->font).height());
        moveCursorDelegate();
    }else{
        qWarning() << QLatin1String("Error loading cursor delegate for TextEdit:") + objectName();
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
int QmlGraphicsTextEdit::selectionStart() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->textCursor().selectionStart();
}

void QmlGraphicsTextEdit::setSelectionStart(int s)
{
    Q_D(QmlGraphicsTextEdit);
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
int QmlGraphicsTextEdit::selectionEnd() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->textCursor().selectionEnd();
}

void QmlGraphicsTextEdit::setSelectionEnd(int s)
{
    Q_D(QmlGraphicsTextEdit);
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
QString QmlGraphicsTextEdit::selectedText() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->textCursor().selectedText();
}

/*!
    \qmlproperty bool TextEdit::focusOnPress

    Whether the TextEdit should gain focus on a mouse press. By default this is
    set to true.
*/
bool QmlGraphicsTextEdit::focusOnPress() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->focusOnPress;
}

void QmlGraphicsTextEdit::setFocusOnPress(bool on)
{
    Q_D(QmlGraphicsTextEdit);
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
bool QmlGraphicsTextEdit::persistentSelection() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->persistentSelection;
}

void QmlGraphicsTextEdit::setPersistentSelection(bool on)
{
    Q_D(QmlGraphicsTextEdit);
    if (d->persistentSelection == on)
        return;
    d->persistentSelection = on;
    emit persistentSelectionChanged(d->persistentSelection);
}

qreal QmlGraphicsTextEdit::textMargin() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->textMargin;
}

void QmlGraphicsTextEdit::setTextMargin(qreal margin)
{
    Q_D(QmlGraphicsTextEdit);
    if (d->textMargin == margin)
        return;
    d->textMargin = margin;
    d->document->setDocumentMargin(d->textMargin);
    emit textMarginChanged(d->textMargin);
}

void QmlGraphicsTextEdit::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QmlGraphicsPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

/*!
    Ensures any delayed caching or data loading the class
    needs to performed is complete.
*/
void QmlGraphicsTextEdit::componentComplete()
{
    Q_D(QmlGraphicsTextEdit);
    QmlGraphicsPaintedItem::componentComplete();
    if (d->dirty) {
        updateSize();
        d->dirty = false;
    }
}

/*!
    \qmlproperty bool TextEdit::readOnly

    Whether the user an interact with the TextEdit item. If this
    property is set to true the text cannot be edited by user interaction.

    By default this property is false.
*/
void QmlGraphicsTextEdit::setReadOnly(bool r)
{
    Q_D(QmlGraphicsTextEdit);    
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

bool QmlGraphicsTextEdit::isReadOnly() const
{
    Q_D(const QmlGraphicsTextEdit);
    return !(d->control->textInteractionFlags() & Qt::TextEditable);
}

/*!
    Sets how the text edit should interact with user input to the given
    \a flags.
*/
void QmlGraphicsTextEdit::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->setTextInteractionFlags(flags);
}

/*!
    Returns the flags specifying how the text edit should interact
    with user input.
*/
Qt::TextInteractionFlags QmlGraphicsTextEdit::textInteractionFlags() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->textInteractionFlags();
}

/*!
    Returns the rectangle where the text cursor is rendered
    within the text edit.
*/
QRect QmlGraphicsTextEdit::cursorRect() const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->cursorRect().toRect();
}


/*!
\overload
Handles the given \a event.
*/
bool QmlGraphicsTextEdit::event(QEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(0, 0));
        return event->isAccepted();
    }
    return QmlGraphicsPaintedItem::event(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QmlGraphicsTextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->processEvent(event, QPointF(0, 0));

    if (!event->isAccepted())
        QmlGraphicsPaintedItem::keyPressEvent(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QmlGraphicsTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::keyReleaseEvent(event);
}

/*!
    \overload
    Handles changing of the focus property.  Focus is applied to the control
    even if the edit does not have active focus.  This is because things
    like KeyProxy can give the behavior of focus even when hasFocus() isn't
    true.
*/
void QmlGraphicsTextEdit::focusChanged(bool hasFocus)
{
    setCursorVisible(hasFocus);
    QmlGraphicsItem::focusChanged(hasFocus);
}

/*!
    Causes all text to be selected.
*/
void QmlGraphicsTextEdit::selectAll()
{
    Q_D(QmlGraphicsTextEdit);
    d->control->selectAll();
}

/*!
\overload
Handles the given mouse \a event.
*/
void QmlGraphicsTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    if (d->focusOnPress){
        QGraphicsItem *p = parentItem();//###Is there a better way to find my focus scope?
        while(p) {
            if(p->flags() & QGraphicsItem::ItemIsFocusScope){
                p->setFocus();
                break;
            }
            p = p->parentItem();
        }
        setFocus(true);
    }
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QmlGraphicsTextEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    QWidget *widget = event->widget();
    if (widget && (d->control->textInteractionFlags() & Qt::TextEditable) && boundingRect().contains(event->pos()))
        qt_widget_private(widget)->handleSoftwareInputPanel(event->button(), d->focusOnPress);

    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QmlGraphicsTextEdit::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::mouseDoubleClickEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QmlGraphicsTextEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
    if (!event->isAccepted())
        QmlGraphicsPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given input method \a event.
*/
void QmlGraphicsTextEdit::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QmlGraphicsTextEdit);
    d->control->processEvent(event, QPointF(0, 0));
}

/*!
\overload
Returns the value of the given \a property.
*/
QVariant QmlGraphicsTextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QmlGraphicsTextEdit);
    return d->control->inputMethodQuery(property);
}

/*!
Draws the contents of the text edit using the given \a painter within
the given \a bounds.
*/
void QmlGraphicsTextEdit::drawContents(QPainter *painter, const QRect &bounds)
{
    Q_D(QmlGraphicsTextEdit);

    painter->setRenderHint(QPainter::TextAntialiasing, true);

    d->control->drawContents(painter, bounds);
}

void QmlGraphicsTextEdit::updateImgCache(const QRectF &r)
{
    dirtyCache(r.toRect());
    emit update();
}

/*!
    \qmlproperty bool TextEdit::smooth

    Set this property if you want the text to be smoothly scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the item is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the item is stationary on
    the screen.  A common pattern when animating an item is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QmlGraphicsTextEditPrivate::init()
{
    Q_Q(QmlGraphicsTextEdit);

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

void QmlGraphicsTextEdit::q_textChanged()
{
    updateSize();
    emit textChanged(text());
}

void QmlGraphicsTextEdit::moveCursorDelegate()
{
    Q_D(QmlGraphicsTextEdit);
    if(!d->cursor)
        return;
    QRectF cursorRect = d->control->cursorRect();
    d->cursor->setX(cursorRect.x());
    d->cursor->setY(cursorRect.y());
}

void QmlGraphicsTextEditPrivate::updateSelection()
{
    Q_Q(QmlGraphicsTextEdit);
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
        qWarning() << "QmlGraphicsTextEditPrivate::updateSelection() has failed you.";
}

void QmlGraphicsTextEdit::updateSelectionMarkers()
{
    Q_D(QmlGraphicsTextEdit);
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
void QmlGraphicsTextEdit::updateSize()
{
    Q_D(QmlGraphicsTextEdit);
    if (isComponentComplete()) {
        QFontMetrics fm = QFontMetrics(d->font);
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

        //### need to comfirm cost of always setting these
        int newWidth = (int)d->document->idealWidth();
        d->document->setTextWidth(newWidth); // ### QTextDoc> Alignment will not work unless textWidth is set. Does Text need this line as well?
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

void QmlGraphicsTextEditPrivate::updateDefaultTextOption()
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
