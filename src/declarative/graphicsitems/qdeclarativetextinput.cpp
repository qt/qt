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

#include "qdeclarativetextinput_p.h"
#include "qdeclarativetextinput_p_p.h"

#include <qdeclarativeinfo.h>

#include <QValidator>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

QT_BEGIN_NAMESPACE

/*!
    \qmlclass TextInput QDeclarativeTextInput
  \since 4.7
    The TextInput item allows you to add an editable line of text to a scene.

    TextInput can only display a single line of text, and can only display
    plain text. However it can provide addition input constraints on the text.

    Input constraints include setting a QValidator, an input mask, or a
    maximum input length.
*/
QDeclarativeTextInput::QDeclarativeTextInput(QDeclarativeItem* parent)
    : QDeclarativePaintedItem(*(new QDeclarativeTextInputPrivate), parent)
{
    Q_D(QDeclarativeTextInput);
    d->init();
}

QDeclarativeTextInput::~QDeclarativeTextInput()
{
}

/*!
    \qmlproperty string TextInput::text

    The text in the TextInput.
*/

QString QDeclarativeTextInput::text() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->text();
}

void QDeclarativeTextInput::setText(const QString &s)
{
    Q_D(QDeclarativeTextInput);
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
QFont QDeclarativeTextInput::font() const
{
    Q_D(const QDeclarativeTextInput);
    return d->font;
}

void QDeclarativeTextInput::setFont(const QFont &font)
{
    Q_D(QDeclarativeTextInput);
    if (d->font == font)
        return;

    d->font = font;

    d->control->setFont(d->font);
    if(d->cursorItem){
        d->cursorItem->setHeight(QFontMetrics(d->font).height());
        moveCursor();
    }
    updateSize();
    emit fontChanged(d->font);
}

/*!
    \qmlproperty color TextInput::color

    The text color.
*/
QColor QDeclarativeTextInput::color() const
{
    Q_D(const QDeclarativeTextInput);
    return d->color;
}

void QDeclarativeTextInput::setColor(const QColor &c)
{
    Q_D(QDeclarativeTextInput);
    d->color = c;
}


/*!
    \qmlproperty color TextInput::selectionColor

    The text highlight color, used behind selections.
*/
QColor QDeclarativeTextInput::selectionColor() const
{
    Q_D(const QDeclarativeTextInput);
    return d->selectionColor;
}

void QDeclarativeTextInput::setSelectionColor(const QColor &color)
{
    Q_D(QDeclarativeTextInput);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::Highlight, d->selectionColor);
    d->control->setPalette(p);
    emit selectionColorChanged(color);
}

/*!
    \qmlproperty color TextInput::selectedTextColor

    The highlighted text color, used in selections.
*/
QColor QDeclarativeTextInput::selectedTextColor() const
{
    Q_D(const QDeclarativeTextInput);
    return d->selectedTextColor;
}

void QDeclarativeTextInput::setSelectedTextColor(const QColor &color)
{
    Q_D(QDeclarativeTextInput);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::HighlightedText, d->selectedTextColor);
    d->control->setPalette(p);
    emit selectedTextColorChanged(color);
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
QDeclarativeTextInput::HAlignment QDeclarativeTextInput::hAlign() const
{
    Q_D(const QDeclarativeTextInput);
    return d->hAlign;
}

void QDeclarativeTextInput::setHAlign(HAlignment align)
{
    Q_D(QDeclarativeTextInput);
    if(align == d->hAlign)
        return;
    d->hAlign = align;
    emit horizontalAlignmentChanged(d->hAlign);
}

bool QDeclarativeTextInput::isReadOnly() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->isReadOnly();
}

void QDeclarativeTextInput::setReadOnly(bool ro)
{
    Q_D(QDeclarativeTextInput);
    if (d->control->isReadOnly() == ro)
        return;

    d->control->setReadOnly(ro);

    emit readOnlyChanged(ro);
}

int QDeclarativeTextInput::maxLength() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->maxLength();
}

void QDeclarativeTextInput::setMaxLength(int ml)
{
    Q_D(QDeclarativeTextInput);
    if (d->control->maxLength() == ml)
        return;

    d->control->setMaxLength(ml);

    emit maximumLengthChanged(ml);
}

/*!
    \qmlproperty bool TextInput::cursorVisible
    Set to true when the TextInput shows a cursor.

    This property is set and unset when the TextInput gets focus, so that other
    properties can be bound to whether the cursor is currently showing. As it
    gets set and unset automatically, when you set the value yourself you must
    keep in mind that your value may be overwritten.

    It can be set directly in script, for example if a KeyProxy might
    forward keys to it and you desire it to look active when this happens
    (but without actually giving it the focus).

    It should not be set directly on the element, like in the below QML,
    as the specified value will be overridden an lost on focus changes.

    \code
    TextInput {
        text: "Text"
        cursorVisible: false
    }
    \endcode

    In the above snippet the cursor will still become visible when the
    TextInput gains focus.
*/
bool QDeclarativeTextInput::isCursorVisible() const
{
    Q_D(const QDeclarativeTextInput);
    return d->cursorVisible;
}

void QDeclarativeTextInput::setCursorVisible(bool on)
{
    Q_D(QDeclarativeTextInput);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    d->control->setCursorBlinkPeriod(on?QApplication::cursorFlashTime():0);
    //d->control should emit the cursor update regions
    emit cursorVisibleChanged(d->cursorVisible);
}

/*!
    \qmlproperty int TextInput::cursorPosition
    The position of the cursor in the TextInput.
*/
int QDeclarativeTextInput::cursorPosition() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->cursor();
}
void QDeclarativeTextInput::setCursorPosition(int cp)
{
    Q_D(QDeclarativeTextInput);
    d->control->moveCursor(cp);
}

/*!
  \internal

  Returns a Rect which encompasses the cursor, but which may be larger than is
  required. Ignores custom cursor delegates.
*/
QRect QDeclarativeTextInput::cursorRect() const
{
    Q_D(const QDeclarativeTextInput);
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
int QDeclarativeTextInput::selectionStart() const
{
    Q_D(const QDeclarativeTextInput);
    return d->lastSelectionStart;
}

void QDeclarativeTextInput::setSelectionStart(int s)
{
    Q_D(QDeclarativeTextInput);
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
int QDeclarativeTextInput::selectionEnd() const
{
    Q_D(const QDeclarativeTextInput);
    return d->lastSelectionEnd;
}

void QDeclarativeTextInput::setSelectionEnd(int s)
{
    Q_D(QDeclarativeTextInput);
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
QString QDeclarativeTextInput::selectedText() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->selectedText();
}

/*!
    \qmlproperty bool TextInput::focusOnPress

    Whether the TextInput should gain focus on a mouse press. By default this is
    set to true.
*/
bool QDeclarativeTextInput::focusOnPress() const
{
    Q_D(const QDeclarativeTextInput);
    return d->focusOnPress;
}

void QDeclarativeTextInput::setFocusOnPress(bool b)
{
    Q_D(QDeclarativeTextInput);
    if (d->focusOnPress == b)
        return;

    d->focusOnPress = b;

    emit focusOnPressChanged(d->focusOnPress);
}

/*!
    \qmlproperty QValidator* TextInput::validator

    Allows you to set a QValidator on the TextInput. When a validator is set
    the TextInput will only accept input which leaves the text property in
    an acceptable or intermediate state. The accepted signal will only be sent
    if the text is in an acceptable state when enter is pressed.

    Currently supported validators are QIntValidator, QDoubleValidator and
    QRegExpValidator. For details, refer to their C++ documentation and remember
    that all Q_PROPERTIES are accessible from Qml. A brief usage guide follows:

    QIntValidator and QDoubleValidator both are controllable through two properties,
    top and bottom. The difference is that for QIntValidator the top and bottom properties
    should be integers, and for QDoubleValidator they should be doubles. QRegExpValidator
    has a single string property, regExp, which should be set to the regular expression to
    be used for validation. An example of using validators is shown below, which allows
    input of integers between 11 and 31 into the text input:

    \code
    import Qt 4.6
    TextInput{
        validator: QIntValidator{bottom: 11; top: 31;}
        focus: true
    }
    \endcode

    \sa acceptableInput, inputMask
*/
QValidator* QDeclarativeTextInput::validator() const
{
    Q_D(const QDeclarativeTextInput);
    //###const cast isn't good, but needed for property system?
    return const_cast<QValidator*>(d->control->validator());
}

void QDeclarativeTextInput::setValidator(QValidator* v)
{
    Q_D(QDeclarativeTextInput);
    if (d->control->validator() == v)
        return;

    d->control->setValidator(v);
    if(!d->control->hasAcceptableInput()){
        d->oldValidity = false;
        emit acceptableInputChanged();
    }

    emit validatorChanged();
}

/*!
    \qmlproperty string TextInput::inputMask

    Allows you to set an input mask on the TextInput, restricting the allowable
    text inputs. See QLineEdit::inputMask for further details, as the exact
    same mask strings are used by TextInput.

    \sa acceptableInput, validator
*/
QString QDeclarativeTextInput::inputMask() const
{
    Q_D(const QDeclarativeTextInput);
    return d->control->inputMask();
}

void QDeclarativeTextInput::setInputMask(const QString &im)
{
    Q_D(QDeclarativeTextInput);
    if (d->control->inputMask() == im)
        return;

    d->control->setInputMask(im);
    emit inputMaskChanged(d->control->inputMask());
}

/*!
    \qmlproperty bool TextInput::acceptableInput

    This property is always true unless a validator or input mask has been set.
    If a validator or input mask has been set, this property will only be true
    if the current text is acceptable to the validator or input mask as a final
    string (not as an intermediate string).
*/
bool QDeclarativeTextInput::hasAcceptableInput() const
{
    Q_D(const QDeclarativeTextInput);
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
QDeclarativeTextInput::EchoMode QDeclarativeTextInput::echoMode() const
{
    Q_D(const QDeclarativeTextInput);
    return (QDeclarativeTextInput::EchoMode)d->control->echoMode();
}

void QDeclarativeTextInput::setEchoMode(QDeclarativeTextInput::EchoMode echo)
{
    Q_D(QDeclarativeTextInput);
    if (echoMode() == echo)
        return;

    d->control->setEchoMode((uint)echo);
    emit echoModeChanged(echoMode());
}

/*!
    \qmlproperty Component TextInput::cursorDelegate
    The delegate for the cursor in the TextInput.

    If you set a cursorDelegate for a TextInput, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the TextInput when a cursor is
    needed, and the x property of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QDeclarativeItem or
    QDeclarativeItem derived item.
*/
QDeclarativeComponent* QDeclarativeTextInput::cursorDelegate() const
{
    Q_D(const QDeclarativeTextInput);
    return d->cursorComponent;
}

void QDeclarativeTextInput::setCursorDelegate(QDeclarativeComponent* c)
{
    Q_D(QDeclarativeTextInput);
    if (d->cursorComponent == c)
        return;

    d->cursorComponent = c;
    if(!c){
        //note that the components are owned by something else
        disconnect(d->control, SIGNAL(cursorPositionChanged(int, int)),
                this, SLOT(moveCursor()));
        delete d->cursorItem;
    }else{
        d->startCreatingCursor();
    }

    emit cursorDelegateChanged();
}

void QDeclarativeTextInputPrivate::startCreatingCursor()
{
    Q_Q(QDeclarativeTextInput);
    q->connect(control, SIGNAL(cursorPositionChanged(int, int)),
            q, SLOT(moveCursor()));
    if(cursorComponent->isReady()){
        q->createCursor();
    }else if(cursorComponent->isLoading()){
        q->connect(cursorComponent, SIGNAL(statusChanged(int)),
                q, SLOT(createCursor()));
    }else{//isError
        qmlInfo(q) << QDeclarativeTextInput::tr("Could not load cursor delegate");
        qWarning() << cursorComponent->errors();
    }
}

void QDeclarativeTextInput::createCursor()
{
    Q_D(QDeclarativeTextInput);
    if(d->cursorComponent->isError()){
        qmlInfo(this) << tr("Could not load cursor delegate");
        qWarning() << d->cursorComponent->errors();
        return;
    }

    if(!d->cursorComponent->isReady())
        return;

    if(d->cursorItem)
        delete d->cursorItem;
    d->cursorItem = qobject_cast<QDeclarativeItem*>(d->cursorComponent->create());
    if(!d->cursorItem){
        qmlInfo(this) << tr("Could not instantiate cursor delegate");
        //The failed instantiation should print its own error messages
        return;
    }

    d->cursorItem->setParentItem(this);
    d->cursorItem->setX(d->control->cursorToX());
    d->cursorItem->setHeight(d->control->height());
}

void QDeclarativeTextInput::moveCursor()
{
    Q_D(QDeclarativeTextInput);
    if(!d->cursorItem)
        return;
    d->cursorItem->setX(d->control->cursorToX() - d->hscroll);
}

int QDeclarativeTextInput::xToPos(int x)
{
    Q_D(const QDeclarativeTextInput);
    return d->control->xToPos(x - d->hscroll);
}

void QDeclarativeTextInput::focusChanged(bool hasFocus)
{
    Q_D(QDeclarativeTextInput);
    d->focused = hasFocus;
    setCursorVisible(hasFocus);
    QDeclarativeItem::focusChanged(hasFocus);
}

void QDeclarativeTextInput::keyPressEvent(QKeyEvent* ev)
{
    Q_D(QDeclarativeTextInput);
    if(((d->control->cursor() == 0 && ev->key() == Qt::Key_Left)
            || (d->control->cursor() == d->control->text().length()
                && ev->key() == Qt::Key_Right))
            && (d->lastSelectionStart == d->lastSelectionEnd)){
        //ignore when moving off the end
        //unless there is a selection, because then moving will do something (deselect)
        ev->ignore();
    }else{
        d->control->processKeyEvent(ev);
    }
    if (!ev->isAccepted())
        QDeclarativePaintedItem::keyPressEvent(ev);
}

void QDeclarativeTextInput::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextInput);
    if(d->focusOnPress){
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
    d->control->processEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QDeclarativeTextInput::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QDeclarativeTextInput);
    QWidget *widget = event->widget();
    if (widget && !d->control->isReadOnly() && boundingRect().contains(event->pos()))
        qt_widget_private(widget)->handleSoftwareInputPanel(event->button(), d->focusOnPress);
    d->control->processEvent(event);
}

bool QDeclarativeTextInput::event(QEvent* ev)
{
    Q_D(QDeclarativeTextInput);
    //Anything we don't deal with ourselves, pass to the control
    bool handled = false;
    switch(ev->type()){
        case QEvent::KeyPress:
        case QEvent::KeyRelease://###Should the control be doing anything with release?
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseRelease:
            break;
        default:
            handled = d->control->processEvent(ev);
    }
    if(!handled)
        return QDeclarativePaintedItem::event(ev);
    return true;
}

void QDeclarativeTextInput::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QDeclarativePaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QDeclarativeTextInput::drawContents(QPainter *p, const QRect &r)
{
    Q_D(QDeclarativeTextInput);
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
    if(d->hAlign != AlignLeft){
        QFontMetrics fm = QFontMetrics(d->font);
        //###Is this using bearing appropriately?
        int minLB = qMax(0, -fm.minLeftBearing());
        int minRB = qMax(0, -fm.minRightBearing());
        int widthUsed = qRound(d->control->naturalTextWidth()) + 1 + minRB;
        int hOffset = 0;
        if(d->hAlign == AlignRight){
            hOffset = width() - widthUsed;
        }else if(d->hAlign == AlignHCenter){
            hOffset = (width() - widthUsed) / 2;
        }
        hOffset -= minLB;
        offset = QPoint(hOffset, 0);
    }
    QRect clipRect = r;
    d->control->draw(p, offset, clipRect, flags);

    p->restore();
}

/*!
\overload
Returns the value of the given \a property.
*/
QVariant QDeclarativeTextInput::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QDeclarativeTextInput);
    switch(property) {
    case Qt::ImFont:
        return font();
    case Qt::ImCursorPosition:
        return QVariant(d->control->cursor());
    case Qt::ImSurroundingText:
        return QVariant(text());
    case Qt::ImCurrentSelection:
        return QVariant(selectedText());
    case Qt::ImMaximumTextLength:
        return QVariant(maxLength());
    case Qt::ImAnchorPosition:
        if (d->control->selectionStart() == d->control->selectionEnd())
            return QVariant(d->control->cursor());
        else if (d->control->selectionStart() == d->control->cursor())
            return QVariant(d->control->selectionEnd());
        else
            return QVariant(d->control->selectionStart());
    default:
        return QVariant();
    }
}

void QDeclarativeTextInput::selectAll()
{
    Q_D(QDeclarativeTextInput);
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

void QDeclarativeTextInputPrivate::init()
{
    Q_Q(QDeclarativeTextInput);
    control->setCursorWidth(1);
    control->setPasswordCharacter(QLatin1Char('*'));
    control->setLayoutDirection(Qt::LeftToRight);
    q->setSmooth(smooth);
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

void QDeclarativeTextInput::cursorPosChanged()
{
    Q_D(QDeclarativeTextInput);
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

void QDeclarativeTextInput::selectionChanged()
{
    Q_D(QDeclarativeTextInput);
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

void QDeclarativeTextInput::q_textChanged()
{
    Q_D(QDeclarativeTextInput);
    updateSize();
    emit textChanged();
    if(hasAcceptableInput() != d->oldValidity){
        d->oldValidity = hasAcceptableInput();
        emit acceptableInputChanged();
    }
}

void QDeclarativeTextInput::updateRect(const QRect &r)
{
    if(r == QRect())
        clearCache();
    else
        dirtyCache(r);
    update();
}

void QDeclarativeTextInput::updateSize(bool needsRedraw)
{
    Q_D(QDeclarativeTextInput);
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

