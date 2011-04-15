// Commit: b94176e69efc3948696c6774d5a228fc753b5b29
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgtextinput_p.h"
#include "qsgtextinput_p_p.h"
#include "qsgcanvas.h"

#include <private/qdeclarativeglobal_p.h>
#include <private/qwidget_p.h>

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qinputcontext.h>
#include <QTextBoundaryFinder>
#include <qstyle.h>

QT_BEGIN_NAMESPACE

QWidgetPrivate *qt_widget_private(QWidget *widget);

QSGTextInput::QSGTextInput(QSGItem* parent)
: QSGImplicitSizePaintedItem(*(new QSGTextInputPrivate), parent)
{
    Q_D(QSGTextInput);
    d->init();
}

QSGTextInput::~QSGTextInput()
{
}

QString QSGTextInput::text() const
{
    Q_D(const QSGTextInput);
    return d->control->text();
}

void QSGTextInput::setText(const QString &s)
{
    Q_D(QSGTextInput);
    if(s == text())
        return;
    d->control->setText(s);
}

QFont QSGTextInput::font() const
{
    Q_D(const QSGTextInput);
    return d->sourceFont;
}

void QSGTextInput::setFont(const QFont &font)
{
    Q_D(QSGTextInput);
    if (d->sourceFont == font)
        return;

    d->sourceFont = font;
    QFont oldFont = d->font;
    d->font = font;
    if (d->font.pointSizeF() != -1) {
        // 0.5pt resolution
        qreal size = qRound(d->font.pointSizeF()*2.0);
        d->font.setPointSizeF(size/2.0);
    }
    if (oldFont != d->font) {
        d->control->setFont(d->font);
        if(d->cursorItem){
            d->cursorItem->setHeight(QFontMetrics(d->font).height());
            moveCursor();
        }
        updateSize();
    }
    emit fontChanged(d->sourceFont);
}

QColor QSGTextInput::color() const
{
    Q_D(const QSGTextInput);
    return d->color;
}

void QSGTextInput::setColor(const QColor &c)
{
    Q_D(QSGTextInput);
    if (c != d->color) {
        d->color = c;
        update();
        emit colorChanged(c);
    }
}

QColor QSGTextInput::selectionColor() const
{
    Q_D(const QSGTextInput);
    return d->selectionColor;
}

void QSGTextInput::setSelectionColor(const QColor &color)
{
    Q_D(QSGTextInput);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::Highlight, d->selectionColor);
    d->control->setPalette(p);
    if (d->control->hasSelectedText()) 
        update();
    emit selectionColorChanged(color);
}

QColor QSGTextInput::selectedTextColor() const
{
    Q_D(const QSGTextInput);
    return d->selectedTextColor;
}

void QSGTextInput::setSelectedTextColor(const QColor &color)
{
    Q_D(QSGTextInput);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    QPalette p = d->control->palette();
    p.setColor(QPalette::HighlightedText, d->selectedTextColor);
    d->control->setPalette(p);
    if (d->control->hasSelectedText()) 
        update();
    emit selectedTextColorChanged(color);
}

QSGTextInput::HAlignment QSGTextInput::hAlign() const
{
    Q_D(const QSGTextInput);
    return d->hAlign;
}

void QSGTextInput::setHAlign(HAlignment align)
{
    Q_D(QSGTextInput);
    bool forceAlign = d->hAlignImplicit && d->effectiveLayoutMirror;
    d->hAlignImplicit = false;
    if (d->setHAlign(align, forceAlign) && isComponentComplete()) {
        updateRect();
        d->updateHorizontalScroll();
    }
}

void QSGTextInput::resetHAlign()
{
    Q_D(QSGTextInput);
    d->hAlignImplicit = true;
    if (d->determineHorizontalAlignment() && isComponentComplete()) {
        updateRect();
        d->updateHorizontalScroll();
    }
}

QSGTextInput::HAlignment QSGTextInput::effectiveHAlign() const
{
    Q_D(const QSGTextInput);
    QSGTextInput::HAlignment effectiveAlignment = d->hAlign;
    if (!d->hAlignImplicit && d->effectiveLayoutMirror) {
        switch (d->hAlign) {
        case QSGTextInput::AlignLeft:
            effectiveAlignment = QSGTextInput::AlignRight;
            break;
        case QSGTextInput::AlignRight:
            effectiveAlignment = QSGTextInput::AlignLeft;
            break;
        default:
            break;
        }
    }
    return effectiveAlignment;
}

bool QSGTextInputPrivate::setHAlign(QSGTextInput::HAlignment alignment, bool forceAlign)
{
    Q_Q(QSGTextInput);
    if ((hAlign != alignment || forceAlign) && alignment <= QSGTextInput::AlignHCenter) { // justify not supported
        QSGTextInput::HAlignment oldEffectiveHAlign = q->effectiveHAlign();
        hAlign = alignment;
        emit q->horizontalAlignmentChanged(alignment);
        if (oldEffectiveHAlign != q->effectiveHAlign())
            emit q->effectiveHorizontalAlignmentChanged();
        return true;
    }
    return false;
}

bool QSGTextInputPrivate::determineHorizontalAlignment()
{
    if (hAlignImplicit) {
        // if no explicit alignment has been set, follow the natural layout direction of the text
        QString text = control->text();
        bool isRightToLeft = text.isEmpty() ? QApplication::keyboardInputDirection() == Qt::RightToLeft : text.isRightToLeft();
        return setHAlign(isRightToLeft ? QSGTextInput::AlignRight : QSGTextInput::AlignLeft);
    }
    return false;
}

void QSGTextInputPrivate::mirrorChange()
{
    Q_Q(QSGTextInput);
    if (q->isComponentComplete()) {
        if (!hAlignImplicit && (hAlign == QSGTextInput::AlignRight || hAlign == QSGTextInput::AlignLeft)) {
            q->updateRect();
            updateHorizontalScroll();
            emit q->effectiveHorizontalAlignmentChanged();
        }
    }
}

bool QSGTextInput::isReadOnly() const
{
    Q_D(const QSGTextInput);
    return d->control->isReadOnly();
}

void QSGTextInput::setReadOnly(bool ro)
{
    Q_D(QSGTextInput);
    if (d->control->isReadOnly() == ro)
        return;

    setFlag(QSGItem::ItemAcceptsInputMethod, !ro);
    d->control->setReadOnly(ro);

    emit readOnlyChanged(ro);
}

int QSGTextInput::maxLength() const
{
    Q_D(const QSGTextInput);
    return d->control->maxLength();
}

void QSGTextInput::setMaxLength(int ml)
{
    Q_D(QSGTextInput);
    if (d->control->maxLength() == ml)
        return;

    d->control->setMaxLength(ml);

    emit maximumLengthChanged(ml);
}

bool QSGTextInput::isCursorVisible() const
{
    Q_D(const QSGTextInput);
    return d->cursorVisible;
}

void QSGTextInput::setCursorVisible(bool on)
{
    Q_D(QSGTextInput);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    d->control->setCursorBlinkPeriod(on?QApplication::cursorFlashTime():0);
    QRect r = d->control->cursorRect();
    if (d->control->inputMask().isEmpty())
        updateRect(r);
    else
        updateRect();
    emit cursorVisibleChanged(d->cursorVisible);
}

int QSGTextInput::cursorPosition() const
{
    Q_D(const QSGTextInput);
    return d->control->cursor();
}
void QSGTextInput::setCursorPosition(int cp)
{
    Q_D(QSGTextInput);
    if (cp < 0 || cp > d->control->text().length())
        return;
    d->control->moveCursor(cp);
}

QRect QSGTextInput::cursorRectangle() const
{
    Q_D(const QSGTextInput);
    QRect r = d->control->cursorRect();
    // Scroll and make consistent with TextEdit
    // QLineControl inexplicably adds 1 to the height and horizontal padding
    // for unicode direction markers.
    r.adjust(5 - d->hscroll, 0, -4 - d->hscroll, -1);
    return r;
}

int QSGTextInput::selectionStart() const
{
    Q_D(const QSGTextInput);
    return d->lastSelectionStart;
}

int QSGTextInput::selectionEnd() const
{
    Q_D(const QSGTextInput);
    return d->lastSelectionEnd;
}

void QSGTextInput::select(int start, int end)
{
    Q_D(QSGTextInput);
    if (start < 0 || end < 0 || start > d->control->text().length() || end > d->control->text().length())
        return;
    d->control->setSelection(start, end-start);
}

QString QSGTextInput::selectedText() const
{
    Q_D(const QSGTextInput);
    return d->control->selectedText();
}

bool QSGTextInput::focusOnPress() const
{
    Q_D(const QSGTextInput);
    return d->focusOnPress;
}

void QSGTextInput::setFocusOnPress(bool b)
{
    Q_D(QSGTextInput);
    if (d->focusOnPress == b)
        return;

    d->focusOnPress = b;

    emit activeFocusOnPressChanged(d->focusOnPress);
}

bool QSGTextInput::autoScroll() const
{
    Q_D(const QSGTextInput);
    return d->autoScroll;
}

void QSGTextInput::setAutoScroll(bool b)
{
    Q_D(QSGTextInput);
    if (d->autoScroll == b)
        return;

    d->autoScroll = b;
    //We need to repaint so that the scrolling is taking into account.
    updateSize(true);
    d->updateHorizontalScroll();
    emit autoScrollChanged(d->autoScroll);
}

#ifndef QT_NO_VALIDATOR
QValidator* QSGTextInput::validator() const
{
    Q_D(const QSGTextInput);
    //###const cast isn't good, but needed for property system?
    return const_cast<QValidator*>(d->control->validator());
}

void QSGTextInput::setValidator(QValidator* v)
{
    Q_D(QSGTextInput);
    if (d->control->validator() == v)
        return;

    d->control->setValidator(v);
    if(!d->control->hasAcceptableInput()){
        d->oldValidity = false;
        emit acceptableInputChanged();
    }

    emit validatorChanged();
}
#endif // QT_NO_VALIDATOR

QString QSGTextInput::inputMask() const
{
    Q_D(const QSGTextInput);
    return d->control->inputMask();
}

void QSGTextInput::setInputMask(const QString &im)
{
    Q_D(QSGTextInput);
    if (d->control->inputMask() == im)
        return;

    d->control->setInputMask(im);
    emit inputMaskChanged(d->control->inputMask());
}

bool QSGTextInput::hasAcceptableInput() const
{
    Q_D(const QSGTextInput);
    return d->control->hasAcceptableInput();
}

QSGTextInput::EchoMode QSGTextInput::echoMode() const
{
    Q_D(const QSGTextInput);
    return (QSGTextInput::EchoMode)d->control->echoMode();
}

void QSGTextInput::setEchoMode(QSGTextInput::EchoMode echo)
{
    Q_D(QSGTextInput);
    if (echoMode() == echo)
        return;
    Qt::InputMethodHints imHints = inputMethodHints();
    if (echo == Password || echo == NoEcho)
        imHints |= Qt::ImhHiddenText;
    else
        imHints &= ~Qt::ImhHiddenText;
    if (echo != Normal)
        imHints |= (Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    else
        imHints &= ~(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText);
    setInputMethodHints(imHints);
    d->control->setEchoMode((uint)echo);
    q_textChanged();
    emit echoModeChanged(echoMode());
}

QDeclarativeComponent* QSGTextInput::cursorDelegate() const
{
    Q_D(const QSGTextInput);
    return d->cursorComponent;
}

void QSGTextInput::setCursorDelegate(QDeclarativeComponent* c)
{
    Q_D(QSGTextInput);
    if (d->cursorComponent == c)
        return;

    d->cursorComponent = c;
    if(!c){
        //note that the components are owned by something else
        disconnect(d->control, SIGNAL(cursorPositionChanged(int,int)),
                this, SLOT(moveCursor()));
        delete d->cursorItem;
    }else{
        d->startCreatingCursor();
    }

    emit cursorDelegateChanged();
}

void QSGTextInputPrivate::startCreatingCursor()
{
    Q_Q(QSGTextInput);
    q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
            q, SLOT(moveCursor()));
    if(cursorComponent->isReady()){
        q->createCursor();
    }else if(cursorComponent->isLoading()){
        q->connect(cursorComponent, SIGNAL(statusChanged(int)),
                q, SLOT(createCursor()));
    }else {//isError
        qmlInfo(q, cursorComponent->errors()) << QSGTextInput::tr("Could not load cursor delegate");
    }
}

void QSGTextInput::createCursor()
{
    Q_D(QSGTextInput);
    if(d->cursorComponent->isError()){
        qmlInfo(this, d->cursorComponent->errors()) << tr("Could not load cursor delegate");
        return;
    }

    if(!d->cursorComponent->isReady())
        return;

    if(d->cursorItem)
        delete d->cursorItem;
    d->cursorItem = qobject_cast<QSGItem*>(d->cursorComponent->create());
    if(!d->cursorItem){
        qmlInfo(this, d->cursorComponent->errors()) << tr("Could not instantiate cursor delegate");
        return;
    }

    QDeclarative_setParent_noEvent(d->cursorItem, this);
    d->cursorItem->setParentItem(this);
    d->cursorItem->setX(d->control->cursorToX());
    d->cursorItem->setHeight(d->control->height()-1); // -1 to counter QLineControl's +1 which is not consistent with Text.
}

void QSGTextInput::moveCursor()
{
    Q_D(QSGTextInput);
    if(!d->cursorItem)
        return;
    d->updateHorizontalScroll();
    d->cursorItem->setX(d->control->cursorToX() - d->hscroll);
}

QRectF QSGTextInput::positionToRectangle(int pos) const
{
    Q_D(const QSGTextInput);
    if (pos > d->control->cursorPosition())
        pos += d->control->preeditAreaText().length();
    return QRectF(d->control->cursorToX(pos)-d->hscroll,
        0.0,
        d->control->cursorWidth(),
        cursorRectangle().height());
}

int QSGTextInput::positionAt(int x) const
{
    return positionAt(x, CursorBetweenCharacters);
}

int QSGTextInput::positionAt(int x, CursorPosition position) const
{
    Q_D(const QSGTextInput);
    int pos = d->control->xToPos(x + d->hscroll, QTextLine::CursorPosition(position));
    const int cursor = d->control->cursor();
    if (pos > cursor) {
        const int preeditLength = d->control->preeditAreaText().length();
        pos = pos > cursor + preeditLength
                ? pos - preeditLength
                : cursor;
    }
    return pos;
}

void QSGTextInput::keyPressEvent(QKeyEvent* ev)
{
    Q_D(QSGTextInput);
    // Don't allow MacOSX up/down support, and we don't allow a completer.
    bool ignore = (ev->key() == Qt::Key_Up || ev->key() == Qt::Key_Down) && ev->modifiers() == Qt::NoModifier;
    if (!ignore && (d->lastSelectionStart == d->lastSelectionEnd) && (ev->key() == Qt::Key_Right || ev->key() == Qt::Key_Left)) {
        // Ignore when moving off the end unless there is a selection,
        // because then moving will do something (deselect).
        int cursorPosition = d->control->cursor();
        if (cursorPosition == 0)
            ignore = ev->key() == (d->control->layoutDirection() == Qt::LeftToRight ? Qt::Key_Left : Qt::Key_Right);
        if (cursorPosition == d->control->text().length())
            ignore = ev->key() == (d->control->layoutDirection() == Qt::LeftToRight ? Qt::Key_Right : Qt::Key_Left);
    }
    if (ignore) {
        ev->ignore();
    } else {
        d->control->processKeyEvent(ev);
    }
    if (!ev->isAccepted())
        QSGPaintedItem::keyPressEvent(ev);
}

void QSGTextInput::inputMethodEvent(QInputMethodEvent *ev)
{
    Q_D(QSGTextInput);
    const bool wasComposing = d->control->preeditAreaText().length() > 0;
    if (d->control->isReadOnly()) {
        ev->ignore();
    } else {
        d->control->processInputMethodEvent(ev);
        updateSize();
        d->updateHorizontalScroll();
    }
    if (!ev->isAccepted())
        QSGPaintedItem::inputMethodEvent(ev);

    if (wasComposing != (d->control->preeditAreaText().length() > 0))
        emit inputMethodComposingChanged();
}

void QSGTextInput::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextInput);
    if (d->sendMouseEventToInputContext(event, QEvent::MouseButtonDblClick))
        return;
    if (d->selectByMouse) {
        int cursor = d->xToPos(event->pos().x());
        d->control->selectWordAtPos(cursor);
        event->setAccepted(true);
    } else {
        QSGPaintedItem::mouseDoubleClickEvent(event);
    }
}

void QSGTextInput::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextInput);
    if (d->sendMouseEventToInputContext(event, QEvent::MouseButtonPress))
        return;
    if(d->focusOnPress){
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
    if (d->selectByMouse) {
        setKeepMouseGrab(false);
        d->pressPos = event->pos();
    }
    bool mark = event->modifiers() & Qt::ShiftModifier;
    int cursor = d->xToPos(event->pos().x());
    d->control->moveCursor(cursor, mark);
    event->setAccepted(true);
}

void QSGTextInput::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextInput);
    if (d->sendMouseEventToInputContext(event, QEvent::MouseMove))
        return;
    if (d->selectByMouse) {
        if (qAbs(int(event->pos().x() - d->pressPos.x())) > QApplication::startDragDistance())
            setKeepMouseGrab(true);
        moveCursorSelection(d->xToPos(event->pos().x()), d->mouseSelectionMode);
        event->setAccepted(true);
    } else {
        QSGPaintedItem::mouseMoveEvent(event);
    }
}

void QSGTextInput::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextInput);
    if (d->sendMouseEventToInputContext(event, QEvent::MouseButtonRelease))
        return;
    if (d->selectByMouse)
        setKeepMouseGrab(false);
    if (!d->showInputPanelOnFocus) { // input panel on click
        if (d->focusOnPress && !isReadOnly() && boundingRect().contains(event->pos())) {
            if (canvas() && canvas() == qApp->focusWidget()) {
                qt_widget_private(canvas())->handleSoftwareInputPanel(event->button(), d->clickCausedFocus);
            }
        }
    }
    d->clickCausedFocus = false;
    d->control->processEvent(event);
    if (!event->isAccepted())
        QSGPaintedItem::mouseReleaseEvent(event);
}

bool QSGTextInputPrivate::sendMouseEventToInputContext(
        QGraphicsSceneMouseEvent *event, QEvent::Type eventType)
{
#if !defined QT_NO_IM
    if (event->widget() && control->composeMode()) {
        int tmp_cursor = xToPos(event->pos().x());
        int mousePos = tmp_cursor - control->cursor();
        if (mousePos < 0 || mousePos > control->preeditAreaText().length()) {
            mousePos = -1;
            // don't send move events outside the preedit area
            if (eventType == QEvent::MouseMove)
                return true;
        }

        QInputContext *qic = event->widget()->inputContext();
        if (qic) {
            QMouseEvent mouseEvent(
                    eventType,
                    event->widget()->mapFromGlobal(event->screenPos()),
                    event->screenPos(),
                    event->button(),
                    event->buttons(),
                    event->modifiers());
            // may be causing reset() in some input methods
            qic->mouseHandler(mousePos, &mouseEvent);
            event->setAccepted(mouseEvent.isAccepted());
        }
        if (!control->preeditAreaText().isEmpty())
            return true;
    }
#else
    Q_UNUSED(event);
    Q_UNUSED(eventType)
#endif

    return false;
}

void QSGTextInput::mouseUngrabEvent()
{
    setKeepMouseGrab(false);
}

bool QSGTextInput::event(QEvent* ev)
{
    Q_D(QSGTextInput);
    //Anything we don't deal with ourselves, pass to the control
    bool handled = false;
    switch(ev->type()){
        case QEvent::KeyPress:
        case QEvent::KeyRelease://###Should the control be doing anything with release?
        case QEvent::InputMethod:
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseMove:
        case QEvent::GraphicsSceneMouseRelease:
        case QEvent::GraphicsSceneMouseDoubleClick:
            break;
        default:
            handled = d->control->processEvent(ev);
    }
    if(!handled)
        handled = QSGPaintedItem::event(ev);
    return handled;
}

void QSGTextInput::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    Q_D(QSGTextInput);
    if (newGeometry.width() != oldGeometry.width()) {
        updateSize();
        d->updateHorizontalScroll();
    }
    QSGPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

int QSGTextInputPrivate::calculateTextWidth()
{
    return qRound(control->naturalTextWidth());
}

void QSGTextInputPrivate::updateHorizontalScroll()
{
    Q_Q(QSGTextInput);
    const int preeditLength = control->preeditAreaText().length();
    int cix = qRound(control->cursorToX(control->cursor() + preeditLength));
    QRect br(q->boundingRect().toRect());
    int widthUsed = calculateTextWidth();

    QSGTextInput::HAlignment effectiveHAlign = q->effectiveHAlign();
    if (autoScroll) {
        if (widthUsed <=  br.width()) {
            // text fits in br; use hscroll for alignment
            switch (effectiveHAlign & ~(Qt::AlignAbsolute|Qt::AlignVertical_Mask)) {
            case Qt::AlignRight:
                hscroll = widthUsed - br.width() - 1;
                break;
            case Qt::AlignHCenter:
                hscroll = (widthUsed - br.width()) / 2;
                break;
            default:
                // Left
                hscroll = 0;
                break;
            }
        } else if (cix - hscroll >= br.width()) {
            // text doesn't fit, cursor is to the right of br (scroll right)
            hscroll = cix - br.width() + 1;
        } else if (cix - hscroll < 0 && hscroll < widthUsed) {
            // text doesn't fit, cursor is to the left of br (scroll left)
            hscroll = cix;
        } else if (widthUsed - hscroll < br.width()) {
            // text doesn't fit, text document is to the left of br; align
            // right
            hscroll = widthUsed - br.width() + 1;
        }
        if (preeditLength > 0) {
            // check to ensure long pre-edit text doesn't push the cursor
            // off to the left
             cix = qRound(control->cursorToX(
                     control->cursor() + qMax(0, control->preeditCursor() - 1)));
             if (cix < hscroll)
                 hscroll = cix;
        }
    } else {
        switch (effectiveHAlign) {
        case QSGTextInput::AlignRight:
            hscroll = q->width() - widthUsed;
            break;
        case QSGTextInput::AlignHCenter:
            hscroll = (q->width() - widthUsed) / 2;
            break;
        default:
            // Left
            hscroll = 0;
            break;
        }
    }
}

void QSGTextInput::paint(QPainter *p)
{
    // XXX todo
    QRect r(0, 0, width(), height());

    Q_D(QSGTextInput);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    p->save();
    p->setPen(QPen(d->color));
    int flags = QLineControl::DrawText;
    if(!isReadOnly() && d->cursorVisible && !d->cursorItem)
        flags |= QLineControl::DrawCursor;
    if (d->control->hasSelectedText())
            flags |= QLineControl::DrawSelections;
    QPoint offset = QPoint(0,0);
    QFontMetrics fm = QFontMetrics(d->font);
    QRect br(boundingRect().toRect());
    if (d->autoScroll) {
        // the y offset is there to keep the baseline constant in case we have script changes in the text.
        offset = br.topLeft() - QPoint(d->hscroll, d->control->ascent() - fm.ascent());
    } else {
        offset = QPoint(d->hscroll, 0);
    }
    d->control->draw(p, offset, r, flags);
    p->restore();
}

QVariant QSGTextInput::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QSGTextInput);
    switch(property) {
    case Qt::ImMicroFocus:
        return cursorRectangle();
    case Qt::ImFont:
        return font();
    case Qt::ImCursorPosition:
        return QVariant(d->control->cursor());
    case Qt::ImSurroundingText:
        if (d->control->echoMode() == PasswordEchoOnEdit && !d->control->passwordEchoEditing())
            return QVariant(displayText());
        else
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

void QSGTextInput::deselect()
{
    Q_D(QSGTextInput);
    d->control->deselect();
}

void QSGTextInput::selectAll()
{
    Q_D(QSGTextInput);
    d->control->setSelection(0, d->control->text().length());
}

bool QSGTextInput::isRightToLeft(int start, int end)
{
    Q_D(QSGTextInput);
    if (start > end) {
        qmlInfo(this) << "isRightToLeft(start, end) called with the end property being smaller than the start.";
        return false;
    } else {
        return d->control->text().mid(start, end - start).isRightToLeft();
    }
}

#ifndef QT_NO_CLIPBOARD
void QSGTextInput::cut()
{
    Q_D(QSGTextInput);
    d->control->copy();
    d->control->del();
}

void QSGTextInput::copy()
{
    Q_D(QSGTextInput);
    d->control->copy();
}

void QSGTextInput::paste()
{
    Q_D(QSGTextInput);
    if (!d->control->isReadOnly())
        d->control->paste();
}
#endif // QT_NO_CLIPBOARD

void QSGTextInput::selectWord()
{
    Q_D(QSGTextInput);
    d->control->selectWordAtPos(d->control->cursor());
}

QString QSGTextInput::passwordCharacter() const
{
    Q_D(const QSGTextInput);
    return QString(d->control->passwordCharacter());
}

void QSGTextInput::setPasswordCharacter(const QString &str)
{
    Q_D(QSGTextInput);
    if(str.length() < 1)
        return;
    d->control->setPasswordCharacter(str.constData()[0]);
    EchoMode echoMode_ = echoMode();
    if (echoMode_ == Password || echoMode_ == PasswordEchoOnEdit) {
        updateSize();
    }
    emit passwordCharacterChanged();
}

QString QSGTextInput::displayText() const
{
    Q_D(const QSGTextInput);
    return d->control->displayText();
}

bool QSGTextInput::selectByMouse() const
{
    Q_D(const QSGTextInput);
    return d->selectByMouse;
}

void QSGTextInput::setSelectByMouse(bool on)
{
    Q_D(QSGTextInput);
    if (d->selectByMouse != on) {
        d->selectByMouse = on;
        emit selectByMouseChanged(on);
    }
}

QSGTextInput::SelectionMode QSGTextInput::mouseSelectionMode() const
{
    Q_D(const QSGTextInput);
    return d->mouseSelectionMode;
}

void QSGTextInput::setMouseSelectionMode(SelectionMode mode)
{
    Q_D(QSGTextInput);
    if (d->mouseSelectionMode != mode) {
        d->mouseSelectionMode = mode;
        emit mouseSelectionModeChanged(mode);
    }
}

bool QSGTextInput::canPaste() const
{
    Q_D(const QSGTextInput);
    return d->canPaste;
}

void QSGTextInput::moveCursorSelection(int position)
{
    Q_D(QSGTextInput);
    d->control->moveCursor(position, true);
    d->updateHorizontalScroll();
}

void QSGTextInput::moveCursorSelection(int pos, SelectionMode mode)
{
    Q_D(QSGTextInput);

    if (mode == SelectCharacters) {
        d->control->moveCursor(pos, true);
    } else if (pos != d->control->cursor()){
        const int cursor = d->control->cursor();
        int anchor;
        if (!d->control->hasSelectedText())
            anchor = d->control->cursor();
        else if (d->control->selectionStart() == d->control->cursor())
            anchor = d->control->selectionEnd();
        else
            anchor = d->control->selectionStart();

        if (anchor < pos || (anchor == pos && cursor < pos)) {
            const QString text = d->control->text();
            QTextBoundaryFinder finder(QTextBoundaryFinder::Word, text);
            finder.setPosition(anchor);

            const QTextBoundaryFinder::BoundaryReasons reasons = finder.boundaryReasons();
            if (anchor < text.length() && (!(reasons & QTextBoundaryFinder::StartWord)
                    || ((reasons & QTextBoundaryFinder::EndWord) && anchor > cursor))) {
                finder.toPreviousBoundary();
            }
            anchor = finder.position() != -1 ? finder.position() : 0;

            finder.setPosition(pos);
            if (pos > 0 && !finder.boundaryReasons())
                finder.toNextBoundary();
            const int cursor = finder.position() != -1 ? finder.position() : text.length();

            d->control->setSelection(anchor, cursor - anchor);
        } else if (anchor > pos || (anchor == pos && cursor > pos)) {
            const QString text = d->control->text();
            QTextBoundaryFinder finder(QTextBoundaryFinder::Word, text);
            finder.setPosition(anchor);

            const QTextBoundaryFinder::BoundaryReasons reasons = finder.boundaryReasons();
            if (anchor > 0 && (!(reasons & QTextBoundaryFinder::EndWord)
                    || ((reasons & QTextBoundaryFinder::StartWord) && anchor < cursor))) {
                finder.toNextBoundary();
            }

            anchor = finder.position() != -1 ? finder.position() : text.length();

            finder.setPosition(pos);
            if (pos < text.length() && !finder.boundaryReasons())
                 finder.toPreviousBoundary();
            const int cursor = finder.position() != -1 ? finder.position() : 0;

            d->control->setSelection(anchor, cursor - anchor);
        }
    }
}

void QSGTextInput::openSoftwareInputPanel()
{
    QEvent event(QEvent::RequestSoftwareInputPanel);
    if (qApp) {
        if (canvas() && canvas() == qApp->focusWidget()) {
            QEvent event(QEvent::RequestSoftwareInputPanel);
            QApplication::sendEvent(canvas(), &event);
        }
    }
}

void QSGTextInput::closeSoftwareInputPanel()
{
    if (qApp) {
        if (canvas() && canvas() == qApp->focusWidget()) {
            QEvent event(QEvent::CloseSoftwareInputPanel);
            QApplication::sendEvent(canvas(), &event);
        }
    }
}

void QSGTextInput::focusInEvent(QFocusEvent *event)
{
    Q_D(const QSGTextInput);
    if (d->showInputPanelOnFocus) {
        if (d->focusOnPress && !isReadOnly()) {
            openSoftwareInputPanel();
        }
    }
    QSGPaintedItem::focusInEvent(event);
}

void QSGTextInput::itemChange(ItemChange change, const ItemChangeData &value)
{
    Q_D(QSGTextInput);
    if (change == ItemActiveFocusHasChanged) {
        bool hasFocus = value.boolValue;
        d->focused = hasFocus;
        setCursorVisible(hasFocus && d->canvas && d->canvas->hasFocus());
        if(echoMode() == QSGTextInput::PasswordEchoOnEdit && !hasFocus)
            d->control->updatePasswordEchoEditing(false);//QLineControl sets it on key events, but doesn't deal with focus events
        if (!hasFocus)
            d->control->deselect();
    }
    QSGItem::itemChange(change, value);
}

bool QSGTextInput::isInputMethodComposing() const
{
    Q_D(const QSGTextInput);
    return d->control->preeditAreaText().length() > 0;
}

void QSGTextInputPrivate::init()
{
    Q_Q(QSGTextInput);
    control->setCursorWidth(1);
    control->setPasswordCharacter(QLatin1Char('*'));
    q->setSmooth(smooth);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QSGItem::ItemAcceptsInputMethod);
    q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
               q, SLOT(cursorPosChanged()));
    q->connect(control, SIGNAL(selectionChanged()),
               q, SLOT(selectionChanged()));
    q->connect(control, SIGNAL(textChanged(QString)),
               q, SLOT(q_textChanged()));
    q->connect(control, SIGNAL(accepted()),
               q, SIGNAL(accepted()));
    q->connect(control, SIGNAL(updateNeeded(QRect)),
               q, SLOT(updateRect(QRect)));
#ifndef QT_NO_CLIPBOARD
    q->connect(q, SIGNAL(readOnlyChanged(bool)),
            q, SLOT(q_canPasteChanged()));
    q->connect(QApplication::clipboard(), SIGNAL(dataChanged()),
            q, SLOT(q_canPasteChanged()));
    canPaste = !control->isReadOnly() && QApplication::clipboard()->text().length() != 0;
#endif // QT_NO_CLIPBOARD
    q->connect(control, SIGNAL(updateMicroFocus()),
               q, SLOT(updateMicroFocus()));
    q->connect(control, SIGNAL(displayTextChanged(QString)),
               q, SLOT(updateRect()));
    q->updateSize();
    oldValidity = control->hasAcceptableInput();
    lastSelectionStart = 0;
    lastSelectionEnd = 0;
    QPalette p = control->palette();
    selectedTextColor = p.color(QPalette::HighlightedText);
    selectionColor = p.color(QPalette::Highlight);
    determineHorizontalAlignment();
}

void QSGTextInput::cursorPosChanged()
{
    Q_D(QSGTextInput);
    d->updateHorizontalScroll();
    updateRect();//TODO: Only update rect between pos's
    updateMicroFocus();
    emit cursorPositionChanged();
    // XXX todo - not in 4.8?
#if 0
    d->control->resetCursorBlinkTimer();
#endif

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

void QSGTextInput::selectionChanged()
{
    Q_D(QSGTextInput);
    updateRect();//TODO: Only update rect in selection
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

void QSGTextInput::q_textChanged()
{
    Q_D(QSGTextInput);
    updateSize();
    d->determineHorizontalAlignment();
    d->updateHorizontalScroll();
    updateMicroFocus();
    emit textChanged();
    emit displayTextChanged();
    if(hasAcceptableInput() != d->oldValidity){
        d->oldValidity = hasAcceptableInput();
        emit acceptableInputChanged();
    }
}

void QSGTextInput::updateRect(const QRect &r)
{
    Q_D(QSGTextInput);
    if(r == QRect())
        update();
    else
        update(QRect(r.x() - d->hscroll, r.y(), r.width(), r.height()));
}

QRectF QSGTextInput::boundingRect() const
{
    Q_D(const QSGTextInput);
    QRectF r = QSGPaintedItem::boundingRect();

    int cursorWidth = d->cursorItem ? d->cursorItem->width() : d->control->cursorWidth();

    // Could include font max left/right bearings to either side of rectangle.

    r.setRight(r.right() + cursorWidth);
    return r;
}

void QSGTextInput::updateSize(bool needsRedraw)
{
    Q_D(QSGTextInput);
    int w = width();
    int h = height();
    setImplicitHeight(d->control->height()-1); // -1 to counter QLineControl's +1 which is not consistent with Text.
    setImplicitWidth(d->calculateTextWidth());
    setContentsSize(QSize(width(), height()));
    if(w==width() && h==height() && needsRedraw)
        update();
}

void QSGTextInput::q_canPasteChanged()
{
    Q_D(QSGTextInput);
    bool old = d->canPaste;
#ifndef QT_NO_CLIPBOARD
    d->canPaste = !d->control->isReadOnly() && QApplication::clipboard()->text().length() != 0;
#endif
    if(d->canPaste != old)
        emit canPasteChanged();
}

QT_END_NAMESPACE

