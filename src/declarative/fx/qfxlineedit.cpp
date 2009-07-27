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

#include "qfxlineedit.h"
#include "qfxlineedit_p.h"
#include <QValidator>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,LineEdit,QFxLineEdit);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,QIntValidator,QIntValidator);

/*!
    \qmlclass LineEdit
    \brief The LineEdit item allows you to add an editable line of text to a scene.
*/
QFxLineEdit::QFxLineEdit(QFxItem* parent)
    : QFxPaintedItem(*(new QFxLineEditPrivate), parent)
{
    Q_D(QFxLineEdit);
    d->init();
}

/*
   \internal
*/
QFxLineEdit::QFxLineEdit(QFxLineEditPrivate &dd, QFxItem* parent)
    : QFxPaintedItem(dd, parent)
{
    Q_D(QFxLineEdit);
    d->init();
}

QFxLineEdit::~QFxLineEdit()
{
}

/*!
    \qmlproperty string LineEdit::text

    The text in the LineEdit.
*/

QString QFxLineEdit::text() const
{
    Q_D(const QFxLineEdit);
    return d->control->text();
}

void QFxLineEdit::setText(const QString &s)
{
    Q_D(QFxLineEdit);
    if(s == text())
        return;
    d->control->setText(s);
    //emit textChanged();
}

/*!
    \qmlproperty font LineEdit::font

    Set the LineEdit's font attributes.  \c font.size sets the font's point size.
*/
QmlFont *QFxLineEdit::font()
{
    Q_D(QFxLineEdit);
    return d->font;
}

/*!
    \qmlproperty color LineEdit::color

    The text color.
*/
QColor QFxLineEdit::color() const
{
    Q_D(const QFxLineEdit);
    return d->color;
}

void QFxLineEdit::setColor(const QColor &c)
{
    Q_D(QFxLineEdit);
    d->color = c;
}

QFxText::HAlignment QFxLineEdit::hAlign() const
{
    Q_D(const QFxLineEdit);
    return d->hAlign;
}

void QFxLineEdit::setHAlign(QFxText::HAlignment align)
{
    Q_D(QFxLineEdit);
    d->hAlign = align;
}

bool QFxLineEdit::isReadOnly() const
{
    Q_D(const QFxLineEdit);
    return d->control->isReadOnly();
}

void QFxLineEdit::setReadOnly(bool ro)
{
    Q_D(QFxLineEdit);
    d->control->setReadOnly(ro);
}

int QFxLineEdit::maxLength() const
{
    Q_D(const QFxLineEdit);
    return d->control->maxLength();
}

void QFxLineEdit::setMaxLength(int ml)
{
    Q_D(QFxLineEdit);
    d->control->setMaxLength(ml);
}

/*!
    \qmlproperty LineEdit::cursorPosition
    \brief The position of the cursor in the LineEdit.
*/
int QFxLineEdit::cursorPosition() const
{
    Q_D(const QFxLineEdit);
    return d->control->cursor();
}
void QFxLineEdit::setCursorPosition(int cp)
{
    Q_D(QFxLineEdit);
    d->control->moveCursor(cp);
}

/*!
    \qmlproperty int LineEdit::selectionStart

    The cursor position before the first character in the current selection.
    Setting this and selectionEnd allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionStart to a value outside of
    the current text, selectionStart will not be changed.

    \sa selectionEnd, cursorPosition, selectedText
*/
int QFxLineEdit::selectionStart() const
{
    Q_D(const QFxLineEdit);
    return d->lastSelectionStart;
}

void QFxLineEdit::setSelectionStart(int s)
{
    Q_D(QFxLineEdit);
    if(d->lastSelectionStart == s || s < 0 || s > text().length())
        return;
    d->lastSelectionStart = s;
    d->control->setSelection(s, d->lastSelectionEnd - s);
}

/*!
    \qmlproperty int LineEdit::selectionEnd

    The cursor position after the last character in the current selection.
    Setting this and selectionStart allows you to specify a selection in the
    text edit.

    Note that if selectionStart == selectionEnd then there is no current
    selection. If you attempt to set selectionEnd to a value outside of
    the current text, selectionEnd will not be changed.

    \sa selectionStart, cursorPosition, selectedText
*/
int QFxLineEdit::selectionEnd() const
{
    Q_D(const QFxLineEdit);
    return d->lastSelectionEnd;
}

void QFxLineEdit::setSelectionEnd(int s)
{
    Q_D(QFxLineEdit);
    if(d->lastSelectionEnd == s || s < 0 || s > text().length())
        return;
    d->lastSelectionEnd = s;
    d->control->setSelection(d->lastSelectionStart, s - d->lastSelectionStart);
}

QString QFxLineEdit::selectedText() const
{
    Q_D(const QFxLineEdit);
    return d->control->selectedText();
}

QObject* QFxLineEdit::validator() const
{
    Q_D(const QFxLineEdit);
    //###const cast isn't good, but needed for property system?
    //###same should be said about using QObject* as the property type
    return const_cast<QValidator*>(d->control->validator());
}

void QFxLineEdit::setValidator(QObject* v)
{
    Q_D(QFxLineEdit);
    QValidator* valid = qobject_cast<QValidator*>(v);
    if(!valid)
        return;
    d->control->setValidator(valid);
    if(!d->control->hasAcceptableInput()){
        d->oldValidity = false;
        emit acceptableInputChanged();
    }
}

QString QFxLineEdit::inputMask() const
{
    Q_D(const QFxLineEdit);
    return d->control->inputMask();
}

void QFxLineEdit::setInputMask(const QString &im)
{
    Q_D(QFxLineEdit);
    d->control->setInputMask(im);
}

bool QFxLineEdit::hasAcceptableInput() const
{
    Q_D(const QFxLineEdit);
    return d->control->hasAcceptableInput();
}

uint QFxLineEdit::echoMode() const
{
    Q_D(const QFxLineEdit);
    return d->control->echoMode();
}

void QFxLineEdit::setEchoMode(uint echo)
{
    Q_D(QFxLineEdit);
    d->control->setEchoMode(echo);
}

/*!
    \qmlproperty LineEdit::cursorDelegate
    \brief The delegate for the cursor in the LineEdit.

    If you set a cursorDelegate for a LineEdit, this delegate will be used for
    drawing the cursor instead of the standard cursor. An instance of the
    delegate will be created and managed by the LineEdit when a cursor is
    needed, and the x property of delegate instance will be set so as
    to be one pixel before the top left of the current character.

    Note that the root item of the delegate component must be a QFxItem or
    QFxItem derived item.
*/
QmlComponent* QFxLineEdit::cursorDelegate() const
{
    Q_D(const QFxLineEdit);
    return d->cursorComponent;
}

void QFxLineEdit::setCursorDelegate(QmlComponent* c)
{
    Q_D(QFxLineEdit);
    if(d->cursorComponent)
        delete d->cursorComponent;
    d->cursorComponent = c;
    d->startCreatingCursor();
}

void QFxLineEditPrivate::startCreatingCursor()
{
    Q_Q(QFxLineEdit);
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
        qWarning() << "You could really use the error checking for QFxLineEdit. We'll implement it soon.";
    }
}

void QFxLineEdit::createCursor()
{
    Q_D(QFxLineEdit);
    //Handle isError too
    if(!d->cursorComponent->isReady())
        return;

    if(d->cursorItem)
        delete d->cursorItem;
    d->cursorItem = qobject_cast<QFxItem*>(d->cursorComponent->create());
    if(!d->cursorItem){
        qWarning() << "You could really use the error reporting for QFxLineEdit. We'll implement it soon.";
        return;
    }

    d->cursorItem->setItemParent(this);
    d->cursorItem->setX(d->control->cursorToX());
    d->cursorItem->setHeight(d->control->height());
}

void QFxLineEdit::moveCursor()
{
    Q_D(QFxLineEdit);
    if(!d->cursorItem)
        return;
    d->cursorItem->setX(d->control->cursorToX() - d->hscroll);
}

/*
int QFxLineEdit::scrollDuration() const
{
    Q_D(const QFxLineEdit);
    return d->scrollDur;
}

void QFxLineEdit::setScrollDuration(int s)
{
    Q_D(QFxLineEdit);
    d->scrollDur = s;
    //Need to update cur anims as well
}
*/
int QFxLineEdit::xToPos(int x)
{
    Q_D(const QFxLineEdit);
    return d->control->xToPos(x - d->hscroll);
}

void QFxLineEdit::focusChanged(bool hasFocus)
{
    Q_D(QFxLineEdit);
    if(d->focused && !hasFocus){
        d->focused = false;
        d->control->setCursorBlinkPeriod(0);
        updateAll();//Only need the cursor rect
    }else{
        d->focused = hasFocus;
        updateAll();//Only need the cursor rect
    }
}

void QFxLineEdit::keyPressEvent(QKeyEvent* ev)
{
    Q_D(QFxLineEdit);
    d->control->processKeyEvent(ev);
}

void QFxLineEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxLineEdit);
    setFocus(true);
    d->control->setCursorBlinkPeriod(QApplication::cursorFlashTime());
    d->focused = true;
    d->control->processEvent(event);
    //event->accept();
}

bool QFxLineEdit::event(QEvent* ev)
{
    Q_D(QFxLineEdit);
    //Anything we don't deal with ourselves, pass to the control
    switch(ev->type()){
        case QEvent::GraphicsSceneMousePress:
            break;
        default:
            return d->control->processEvent(ev);
    }
    return false;
}

void QFxLineEdit::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QFxPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QFxLineEdit::drawContents(QPainter *p, const QRect &r)
{
    Q_D(QFxLineEdit);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    p->save();
    p->setPen(QPen(d->color));
    int flags = QLineControl::DrawText;
    if(!isReadOnly() && d->focused && !d->cursorItem)
        flags |= QLineControl::DrawCursor;
    if (d->control->hasSelectedText())
            flags |= QLineControl::DrawSelections;

    d->control->draw(p, QPoint(0,0), r, flags);

    p->restore();
}

void QFxLineEditPrivate::init()
{
        Q_Q(QFxLineEdit);
        control->setCursorWidth(1);
        control->setPasswordCharacter(QLatin1Char('*'));
        control->setLayoutDirection(Qt::LeftToRight);
        q->setSmooth(true);
        q->setAcceptedMouseButtons(Qt::LeftButton);
        q->setOptions(QFxLineEdit::AcceptsInputMethods | QFxLineEdit::SimpleItem
            | QFxLineEdit::HasContents | QFxLineEdit::MouseEvents);
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
        if(!font)
            font = new QmlFont();
        q->updateSize();
        oldValidity = control->hasAcceptableInput();
        lastSelectionStart = 0;
        lastSelectionEnd = 0;
}

void QFxLineEdit::cursorPosChanged()
{
    Q_D(QFxLineEdit);
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

void QFxLineEdit::selectionChanged()
{
    Q_D(QFxLineEdit);
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

void QFxLineEdit::q_textChanged()
{
    Q_D(QFxLineEdit);
    updateAll();
    emit textChanged();
    if(hasAcceptableInput() != d->oldValidity){
        d->oldValidity = hasAcceptableInput();
        emit acceptableInputChanged();
    }
}

//### Please replace this function with proper updating
void QFxLineEdit::updateAll()
{
    clearCache();
    updateSize();
    update();
}

void QFxLineEdit::updateSize()
{
    Q_D(QFxLineEdit);
    setImplicitHeight(d->control->height());
    //d->control->width() is max width, not current width
    QFontMetrics fm = QFontMetrics(d->font->font());
    setImplicitWidth(fm.boundingRect(d->control->text()).width()+1);
    setContentsSize(QSize(width(), height()));
}

QT_END_NAMESPACE

