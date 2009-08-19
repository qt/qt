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
#include <QValidator>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,TextInput,QFxTextInput);
QML_DEFINE_NOCREATE_TYPE(QValidator);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QIntValidator,QIntValidator);

/*!
    \qmlclass TextInput
    The TextInput item allows you to add an editable line of text to a scene.
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
    \qmlproperty font TextInput::font

    Set the TextInput's font attributes.  \c font.size sets the font's point size.
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
    //updateSize();
    updateAll();//TODO: Only necessary updates
    update();
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
    \qmlproperty color TextInput::highlightColor

    The text highlight color, used behind selections.
*/
QColor QFxTextInput::highlightColor() const
{
    Q_D(const QFxTextInput);
    return d->highlightColor;
}

void QFxTextInput::setHighlightColor(const QColor &color)
{
    Q_D(QFxTextInput);
    if (d->highlightColor == color)
        return;

    d->highlightColor = color;
    //TODO: implement
}

/*!
    \qmlproperty color TextInput::highlightedTextColor

    The highlighted text color, used in selections.
*/
QColor QFxTextInput::highlightedTextColor() const
{
    Q_D(const QFxTextInput);
    return d->highlightedTextColor;
}

void QFxTextInput::setHighlightedTextColor(const QColor &color)
{
    Q_D(QFxTextInput);
    if (d->highlightedTextColor == color)
        return;

    d->highlightedTextColor = color;
    //TODO: implement
}

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
    updateAll();//TODO: Only update cursor rect
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

QString QFxTextInput::selectedText() const
{
    Q_D(const QFxTextInput);
    return d->control->selectedText();
}

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

bool QFxTextInput::hasAcceptableInput() const
{
    Q_D(const QFxTextInput);
    return d->control->hasAcceptableInput();
}

uint QFxTextInput::echoMode() const
{
    Q_D(const QFxTextInput);
    return d->control->echoMode();
}

void QFxTextInput::setEchoMode(uint echo)
{
    Q_D(QFxTextInput);
    d->control->setEchoMode(echo);
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
    if(d->cursorComponent)
        delete d->cursorComponent;
    d->cursorComponent = c;
    d->startCreatingCursor();
}

void QFxTextInputPrivate::startCreatingCursor()
{
    Q_Q(QFxTextInput);
    if(!cursorComponent){
        q->disconnect(control, SIGNAL(cursorPositionChanged(int, int)),
                q, SLOT(moveCursor()));
        return;
    }
    q->connect(control, SIGNAL(cursorPositionChanged(int, int)),
            q, SLOT(moveCursor()));
    if(cursorComponent->isReady()){
        q->createCursor();
    }else if(cursorComponent->isLoading()){
        q->connect(cursorComponent, SIGNAL(statusChanged(int)),
                q, SLOT(createCursor()));
    }else{//isError
        qWarning() << "You could really use the error checking for QFxTextInput. We'll implement it soon.";
    }
}

void QFxTextInput::createCursor()
{
    Q_D(QFxTextInput);
    //Handle isError too
    if(!d->cursorComponent->isReady())
        return;

    if(d->cursorItem)
        delete d->cursorItem;
    d->cursorItem = qobject_cast<QFxItem*>(d->cursorComponent->create());
    if(!d->cursorItem){
        qWarning() << "You could really use the error reporting for QFxTextInput. We'll implement it soon.";
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
    d->control->processKeyEvent(ev);
    if (!ev->isAccepted())
        QFxPaintedItem::keyPressEvent(ev);
}

void QFxTextInput::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxTextInput);
    setFocus(true);//###Should we make 'focusOnPress' be optional like TextEdit?
    setCursorVisible(true);
    d->focused = true;
    d->control->processEvent(event);
    //event->accept();
}

bool QFxTextInput::event(QEvent* ev)
{
    Q_D(QFxTextInput);
    //Anything we don't deal with ourselves, pass to the control
    switch(ev->type()){
        case QEvent::GraphicsSceneMousePress:
            break;
        default:
            return d->control->processEvent(ev);
    }
    return false;
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
    if (d->control->hasSelectedText())
            flags |= QLineControl::DrawSelections;

    d->control->draw(p, QPoint(0,0), r, flags);

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
        q->connect(control, SIGNAL(updateNeeded(const QRect &)),
        //        q, SLOT(dirtyCache(const QRect &)));
                q, SLOT(updateAll()));
        q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
                q, SLOT(updateAll()));
        q->connect(control, SIGNAL(selectionChanged()),
                q, SLOT(updateAll()));
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
    updateAll();
    emit textChanged();
    if(hasAcceptableInput() != d->oldValidity){
        d->oldValidity = hasAcceptableInput();
        emit acceptableInputChanged();
    }
}

//### Please replace this function with proper updating
void QFxTextInput::updateAll()
{
    clearCache();
    updateSize();
    update();
}

void QFxTextInput::updateSize()
{
    Q_D(QFxTextInput);
    setImplicitHeight(d->control->height());
    //d->control->width() is max width, not current width
    QFontMetrics fm = QFontMetrics(d->font);
    setImplicitWidth(fm.boundingRect(d->control->text()).width()+1);
    setContentsSize(QSize(width(), height()));
}

QT_END_NAMESPACE

