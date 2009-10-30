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

#include "qfxtextinput.h"
#include "qfxtextinput_p.h"
#include "qmlinfo.h"
#include <QValidator>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,TextInput,QFxTextInput);
QML_DEFINE_NOCREATE_TYPE(QValidator);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QIntValidator,QIntValidator);

/*!
    \qmlclass TextInput QFxTextInput
    The TextInput item allows you to add an editable line of text to a scene.

    TextInput can only display a single line of text, and can only display
    plain text. However it can provide addition input constraints on the text.

    Input constraints include setting a QValidator, an input mask, or a
    maximum input length.
*/
QFxTextInput::QFxTextInput(QFxItem* parent)
    : QFxPaintedItem(*(new QFxTextInputPrivate), parent)
{
    Q_D(QFxTextInput);
    d->init();
}

/*
   \internal
*/
QFxTextInput::QFxTextInput(QFxTextInputPrivate &dd, QFxItem* parent)
    : QFxPaintedItem(dd, parent)
{
    Q_D(QFxTextInput);
    d->init();
}

QFxTextInput::~QFxTextInput()
{
}

/*!
    \qmlproperty string TextInput::text

    The text in the TextInput.
*/

QString QFxTextInput::text() const
{
    Q_D(const QFxTextInput);
    return d->control->text();
}

void QFxTextInput::setText(const QString &s)
{
    Q_D(QFxTextInput);
    if(s == text())
        return;
    d->control->setText(s);
    //emit textChanged();
}

/*!
    \qmlproperty string TextInput::font.family
    \qmlproperty bool TextInput::font.bold
    \qmlproperty bool TextInput::font.italic
    \qmlproperty bool TextInput::font.underline
    \qmlproperty real TextInput::font.pointSize
    \qmlproperty int TextInput::font.pixelSize

    Set the TextInput's font attributes.
*/
QFont QFxTextInput::font() const
{
    Q_D(const QFxTextInput);
    return d->font;
}

void QFxTextInput::setFont(const QFont &font)
{
    Q_D(QFxTextInput);
    d->font = font;

    d->control->setFont(d->font);
    if(d->cursorItem){
        d->cursorItem->setHeight(QFontMetrics(d->font).height());
        moveCursor();
    }
    updateSize();
}

/*!
    \qmlproperty color TextInput::color

    The text color.
*/
QColor QFxTextInput::color() const
{
    Q_D(const QFxTextInput);
    return d->color;
}

void QFxTextInput::setColor(const QColor &c)
{
    Q_D(QFxTextInput);
    d->color = c;
}


/*!
    \qmlproperty color TextInput::selectionColor

    The text highlight color, used behind selections.
*/
QColor QFxTextInput::selectionColor() const
{
    Q_D(const QFxTextInput);
    return d->selectionColor;
}

void QFxTextInput::setSelectionColor(const QColor &color)
{
    Q_D(QFxTextInput);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::Highlight, d->selectionColor);
    d->control->setPalette(p);
}

/*!
    \qmlproperty color TextInput::selectedTextColor

    The highlighted text color, used in selections.
*/
QColor QFxTextInput::selectedTextColor() const
{
    Q_D(const QFxTextInput);
    return d->selectedTextColor;
}

void QFxTextInput::setSelectedTextColor(const QColor &color)
{
    Q_D(QFxTextInput);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::HighlightedText, d->selectedTextColor);
    d->control->setPalette(p);
}

/*!
    \qmlproperty enumeration TextInput::horizontalAlignment

    Sets the horizontal alignment of the text within the TextInput item's
    width and height.  By default, the text is left aligned.

    TextInput does not have vertical alignment, as the natural height is
    exactly the height of the single line of text. If you set the height
    manually to something larger, TextInput will always be top aligned
    vertically. You can use anchors to align it however you want within
    another item.

    The valid values for \c horizontalAlignment are \c AlignLeft, \c AlignRight and
    \c AlignHCenter.
*/
QFxTextInput::HAlignment QFxTextInput::hAlign() const
{
    Q_D(const QFxTextInput);
    return d->hAlign;
}

void QFxTextInput::setHAlign(HAlignment align)
{
    Q_D(QFxTextInput);
    d->hAlign = align;
    //TODO: implement
}

bool QFxTextInput::isReadOnly() const
{
    Q_D(const QFxTextInput);
    return d->control->isReadOnly();
}

void QFxTextInput::setReadOnly(bool ro)
{
    Q_D(QFxTextInput);
    d->control->setReadOnly(ro);
}

int QFxTextInput::maxLength() const
{
    Q_D(const QFxTextInput);
    return d->control->maxLength();
}

void QFxTextInput::setMaxLength(int ml)
{
    Q_D(QFxTextInput);
    d->control->setMaxLength(ml);
}

/*!
    \qmlproperty bool TextInput::cursorVisible
    If true the text edit shows a cursor.

    This property is set and unset when the line edit gets focus, but it can also
    be set directly (useful, for example, if a KeyProxy might forward keys to it).
*/
bool QFxTextInput::isCursorVisible() const
{
    Q_D(const QFxTextInput);
    return d->cursorVisible;
}

void QFxTextInput::setCursorVisible(bool on)
{
    Q_D(QFxTextInput);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    d->control->setCursorBlinkPeriod(on?QApplication::cursorFlashTime():0);
    //d->control should emit the cursor update regions
}

/*!
    \qmlproperty int TextInput::cursorPosition
    The position of the cursor in the TextInput.
*/
int QFxTextInput::cursorPosition() const
{
    Q_D(const QFxTextInput);
    return d->control->cursor();
}
void QFxTextInput::setCursorPosition(int cp)
{
    Q_D(QFxTextInput);
    d->control->moveCursor(cp);
}

/*!
  \internal

  Returns a Rect which encompasses the cursor, but which may be larger than is
  required. Ignores custom cursor delegates.
*/
QRect QFxTextInput::cursorRect() const
{
    Q_D(const QFxTextInput);
    return d->control->cursorRect();
}

/*!
    \qmlproperty int TextInput::selectionStart

    The cursor position before the first character in the current selection.
    Setting this and selectionEnd allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionStart to a value outside of
    the current text, selectionStart will not be changed.

    \sa selectionEnd, cursorPosition, selectedText
*/
int QFxTextInput::selectionStart() const
{
    Q_D(const QFxTextInput);
    return d->lastSelectionStart;
}

void QFxTextInput::setSelectionStart(int s)
{
    Q_D(QFxTextInput);
    if(d->lastSelectionStart == s || s < 0 || s > text().length())
        return;
    d->lastSelectionStart = s;
    d->control->setSelection(s, d->lastSelectionEnd - s);
}

/*!
    \qmlproperty int TextInput::selectionEnd

    The cursor position after the last character in the current selection.
    Setting this and selectionStart allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionEnd to a value outside of
    the current text, selectionEnd will not be changed.

    \sa selectionStart, cursorPosition, selectedText
*/
int QFxTextInput::selectionEnd() const
{
    Q_D(const QFxTextInput);
    return d->lastSelectionEnd;
}

void QFxTextInput::setSelectionEnd(int s)
{
    Q_D(QFxTextInput);
    if(d->lastSelectionEnd == s || s < 0 || s > text().length())
        return;
    d->lastSelectionEnd = s;
    d->control->setSelection(d->lastSelectionStart, s - d->lastSelectionStart);
}

/*!
    \qmlproperty string TextInput::selectedText

    This read-only property provides the text currently selected in the
    text input.

    It is equivalent to the following snippet, but is faster and easier
    to use.

    \qml
    myTextInput.text.toString().substring(myTextInput.selectionStart,
        myTextInput.selectionEnd);
    \endqml
*/
QString QFxTextInput::selectedText() const
{
    Q_D(const QFxTextInput);
    return d->control->selectedText();
}

/*!
    \qmlproperty bool TextInput::focusOnPress

    Whether the TextInput should gain focus on a mouse press. By default this is
    set to true.
*/
bool QFxTextInput::focusOnPress() const
{
    Q_D(const QFxTextInput);
    return d->focusOnPress;
}

void QFxTextInput::setFocusOnPress(bool b)
{
    Q_D(QFxTextInput);
    d->focusOnPress = b;
}

/*!
    \qmlproperty QValidator* TextInput::validator

    Allows you to set a QValidator on the TextInput. When a validator is set
    the TextInput will only accept input which leaves the text property in
    an acceptable or intermediate state. The accepted signal will only be sent
    if the text is in an acceptable state when enter is pressed.

    \sa acceptableInput, inputMask
*/
QValidator* QFxTextInput::validator() const
{
    Q_D(const QFxTextInput);
    //###const cast isn't good, but needed for property system?
    return const_cast<QValidator*>(d->control->validator());
}

void QFxTextInput::setValidator(QValidator* v)
{
    Q_D(QFxTextInput);
    d->control->setValidator(v);
    if(!d->control->hasAcceptableInput()){
        d->oldValidity = false;
        emit acceptableInputChanged();
    }
}

/*!
    \qmlproperty string TextInput::inputMask

    Allows you to set an input mask on the TextInput, restricting the allowable
    text inputs. See QLineEdit::inputMask for further details, as the exact
    same mask strings are used by TextInput.

    \sa acceptableInput, validator
*/
QString QFxTextInput::inputMask() const
{
    Q_D(const QFxTextInput);
    return d->control->inputMask();
}

void QFxTextInput::setInputMask(const QString &im)
{
    Q_D(QFxTextInput);
    d->control->setInputMask(im);
}

/*!
    \qmlproperty bool TextInput::acceptableInput

    This property is always true unless a validator or input mask has been set.
    If a validator or input mask has been set, this property will only be true
    if the current text is acceptable to the validator or input mask as a final
    string (not as an intermediate string).
*/
bool QFxTextInput::hasAcceptableInput() const
{
    Q_D(const QFxTextInput);
    return d->control->hasAcceptableInput();
}

/*!
    \qmlproperty TextInput.EchoMode TextInput::echoMode

    Specifies how the text should be displayed in the TextInput.
    The default is Normal, which displays the text as it is. Other values
    are Password, which displays asterixes instead of characters, NoEcho,
    which displays nothing, and PasswordEchoOnEdit, which displays all but the
    current character as asterixes.

*/
QFxTextInput::EchoMode QFxTextInput::echoMode() const
{
    Q_D(const QFxTextInput);
    return (QFxTextInput::EchoMode)d->control->echoMode();
}

void QFxTextInput::setEchoMode(QFxTextInput::EchoMode echo)
{
    Q_D(QFxTextInput);
    d->control->setEchoMode((uint)echo);
}

/*!
    \qmlproperty Component TextInput::cursorDelegate
    The delegate for the cursor in the TextInput.

    If you set a cursorDelegate for a TextInput, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the TextInput when a cursor is
    needed, and the x property of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QFxItem or
    QFxItem derived item.
*/
QmlComponent* QFxTextInput::cursorDelegate() const
{
    Q_D(const QFxTextInput);
    return d->cursorComponent;
}

void QFxTextInput::setCursorDelegate(QmlComponent* c)
{
    Q_D(QFxTextInput);
    d->cursorComponent = c;
    if(!c){
        //note that the components are owned by something else
        disconnect(d->control, SIGNAL(cursorPositionChanged(int, int)),
                this, SLOT(moveCursor()));
        delete d->cursorItem;
    }else{
        d->startCreatingCursor();
    }
}

void QFxTextInputPrivate::startCreatingCursor()
{
    Q_Q(QFxTextInput);
    q->connect(control, SIGNAL(cursorPositionChanged(int, int)),
            q, SLOT(moveCursor()));
    if(cursorComponent->isReady()){
        q->createCursor();
    }else if(cursorComponent->isLoading()){
        q->connect(cursorComponent, SIGNAL(statusChanged(int)),
                q, SLOT(createCursor()));
    }else{//isError
        qmlInfo(QFxTextInput::tr("Could not load cursor delegate"), q);
        qWarning() << cursorComponent->errors();
    }
}

void QFxTextInput::createCursor()
{
    Q_D(QFxTextInput);
    if(d->cursorComponent->isError()){
        qmlInfo(tr("Could not load cursor delegate"),this);
        qWarning() << d->cursorComponent->errors();
        return;
    }

    if(!d->cursorComponent->isReady())
        return;

    if(d->cursorItem)
        delete d->cursorItem;
    d->cursorItem = qobject_cast<QFxItem*>(d->cursorComponent->create());
    if(!d->cursorItem){
        qmlInfo(tr("Could not instantiate cursor delegate"),this);
        //The failed instantiation should print its own error messages
        return;
    }

    d->cursorItem->setParentItem(this);
    d->cursorItem->setX(d->control->cursorToX());
    d->cursorItem->setHeight(d->control->height());
}

void QFxTextInput::moveCursor()
{
    Q_D(QFxTextInput);
    if(!d->cursorItem)
        return;
    d->cursorItem->setX(d->control->cursorToX() - d->hscroll);
}

int QFxTextInput::xToPos(int x)
{
    Q_D(const QFxTextInput);
    return d->control->xToPos(x - d->hscroll);
}

void QFxTextInput::focusChanged(bool hasFocus)
{
    Q_D(QFxTextInput);
    d->focused = hasFocus;
    setCursorVisible(hasFocus);
    QFxItem::focusChanged(hasFocus);
}

void QFxTextInput::keyPressEvent(QKeyEvent* ev)
{
    Q_D(QFxTextInput);
    if((d->control->cursor() == 0 && ev->key() == Qt::Key_Left)
            || (d->control->cursor() == d->control->text().length()
                && ev->key() == Qt::Key_Right)){
        //ignore when moving off the end
        ev->ignore();
    }else{
        d->control->processKeyEvent(ev);
    }
    if (!ev->isAccepted())
        QFxPaintedItem::keyPressEvent(ev);
}

void QFxTextInput::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextInput);
    if(d->focusOnPress){
        setFocus(true);
        setCursorVisible(true);
        d->focused = true;
    }
    d->control->processEvent(event);
}

bool QFxTextInput::event(QEvent* ev)
{
    Q_D(QFxTextInput);
    //Anything we don't deal with ourselves, pass to the control
    bool handled = false;
    switch(ev->type()){
        case QEvent::KeyPress:
        case QEvent::KeyRelease://###Should the control be doing anything with release?
        case QEvent::GraphicsSceneMousePress:
            break;
        default:
            handled = d->control->processEvent(ev);
    }
    if(!handled)
        return QFxPaintedItem::event(ev);
    return true;
}

void QFxTextInput::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QFxPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QFxTextInput::drawContents(QPainter *p, const QRect &r)
{
    Q_D(QFxTextInput);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    p->save();
    p->setPen(QPen(d->color));
    int flags = QLineControl::DrawText;
    if(!isReadOnly() && d->cursorVisible && !d->cursorItem)
        flags |= QLineControl::DrawCursor;
    if (d->control->hasSelectedText()){
            flags |= QLineControl::DrawSelections;
    }

    QPoint offset = QPoint(0,0);
    QRect clipRect = r;
    d->control->draw(p, offset, clipRect, flags);

    p->restore();
}

void QFxTextInput::selectAll()
{
    Q_D(QFxTextInput);
    d->control->setSelection(0, d->control->text().length());
}


/*!
    \qmlproperty bool TextInput::smooth

    Set this property if you want the text to be smoothly scaled or
    transformed.  Smooth filtering gives better visual quality, but is slower.  If
    the item is displayed at its natural size, this property has no visual or
    performance effect.

    \note Generally scaling artifacts are only visible if the item is stationary on
    the screen.  A common pattern when animating an item is to disable smooth
    filtering at the beginning of the animation and reenable it at the conclusion.
*/

void QFxTextInputPrivate::init()
{
    Q_Q(QFxTextInput);
    control->setCursorWidth(1);
    control->setPasswordCharacter(QLatin1Char('*'));
    control->setLayoutDirection(Qt::LeftToRight);
    q->setSmoothTransform(smooth);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QGraphicsItem::ItemHasNoContents, false);
    q->setFlag(QGraphicsItem::ItemAcceptsInputMethod);
    q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
               q, SLOT(cursorPosChanged()));
    q->connect(control, SIGNAL(selectionChanged()),
               q, SLOT(selectionChanged()));
    q->connect(control, SIGNAL(textChanged(const QString &)),
               q, SLOT(q_textChanged()));
    q->connect(control, SIGNAL(accepted()),
               q, SIGNAL(accepted()));
    q->connect(control, SIGNAL(updateNeeded(QRect)),
               q, SLOT(updateRect(QRect)));
    q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
               q, SLOT(updateRect()));//TODO: Only update rect between pos's
    q->connect(control, SIGNAL(selectionChanged()),
               q, SLOT(updateRect()));//TODO: Only update rect in selection
    //Note that above TODOs probably aren't that big a savings
    q->updateSize();
    oldValidity = control->hasAcceptableInput();
    lastSelectionStart = 0;
    lastSelectionEnd = 0;
}

void QFxTextInput::cursorPosChanged()
{
    Q_D(QFxTextInput);
    emit cursorPositionChanged();

    if(!d->control->hasSelectedText()){
        if(d->lastSelectionStart != d->control->cursor()){
            d->lastSelectionStart = d->control->cursor();
            emit selectionStartChanged();
        }
        if(d->lastSelectionEnd != d->control->cursor()){
            d->lastSelectionEnd = d->control->cursor();
            emit selectionEndChanged();
        }
    }
}

void QFxTextInput::selectionChanged()
{
    Q_D(QFxTextInput);
    emit selectedTextChanged();

    if(d->lastSelectionStart != d->control->selectionStart()){
        d->lastSelectionStart = d->control->selectionStart();
        if(d->lastSelectionStart == -1)
            d->lastSelectionStart = d->control->cursor();
        emit selectionStartChanged();
    }
    if(d->lastSelectionEnd != d->control->selectionEnd()){
        d->lastSelectionEnd = d->control->selectionEnd();
        if(d->lastSelectionEnd == -1)
            d->lastSelectionEnd = d->control->cursor();
        emit selectionEndChanged();
    }
}

void QFxTextInput::q_textChanged()
{
    Q_D(QFxTextInput);
    updateSize();
    emit textChanged();
    if(hasAcceptableInput() != d->oldValidity){
        d->oldValidity = hasAcceptableInput();
        emit acceptableInputChanged();
    }
}

void QFxTextInput::updateRect(const QRect &r)
{
    if(r == QRect())
        clearCache();
    else
        dirtyCache(r);
    update();
}

void QFxTextInput::updateSize(bool needsRedraw)
{
    Q_D(QFxTextInput);
    int w = width();
    int h = height();
    setImplicitHeight(d->control->height());
    int cursorWidth = d->control->cursorWidth();
    if(d->cursorItem)
        cursorWidth = d->cursorItem->width();
    //### Is QFontMetrics too slow?
    QFontMetricsF fm(d->font);
    setImplicitWidth(fm.width(d->control->displayText())+cursorWidth);
    setContentsSize(QSize(width(), height()));//Repaints if changed
    if(w==width() && h==height() && needsRedraw){
        clearCache();
        update();
    }
}

QT_END_NAMESPACE

