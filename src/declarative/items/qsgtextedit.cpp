// Commit: 6980bca15b411f86b9fadb7484a6dd782b9d1403
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

#include "qsgtextedit_p.h"
#include "qsgtextedit_p_p.h"
#include "qsgevents_p_p.h"
#include "qsgcanvas.h"

#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qapplication.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextobject.h>
#include <QtCore/qmath.h>

#include <private/qdeclarativeglobal_p.h>
#include <private/qtextcontrol_p.h>
#include <private/qtextengine_p.h>
#include <private/qwidget_p.h>

QT_BEGIN_NAMESPACE

QWidgetPrivate *qt_widget_private(QWidget *widget);

QSGTextEdit::QSGTextEdit(QSGItem *parent)
: QSGImplicitSizePaintedItem(*(new QSGTextEditPrivate), parent)
{
    Q_D(QSGTextEdit);
    d->init();
}

QString QSGTextEdit::text() const
{
    Q_D(const QSGTextEdit);

#ifndef QT_NO_TEXTHTMLPARSER
    if (d->richText)
        return d->document->toHtml();
    else
#endif
        return d->document->toPlainText();
}

void QSGTextEdit::setText(const QString &text)
{
    Q_D(QSGTextEdit);
    if (QSGTextEdit::text() == text)
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

QSGTextEdit::TextFormat QSGTextEdit::textFormat() const
{
    Q_D(const QSGTextEdit);
    return d->format;
}

void QSGTextEdit::setTextFormat(TextFormat format)
{
    Q_D(QSGTextEdit);
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
    d->control->setAcceptRichText(d->format != PlainText);
    emit textFormatChanged(d->format);
}

QFont QSGTextEdit::font() const
{
    Q_D(const QSGTextEdit);
    return d->sourceFont;
}

void QSGTextEdit::setFont(const QFont &font)
{
    Q_D(QSGTextEdit);
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
        d->document->setDefaultFont(d->font);
        if(d->cursor){
            d->cursor->setHeight(QFontMetrics(d->font).height());
            moveCursorDelegate();
        }
        updateSize();
        update();
    }
    emit fontChanged(d->sourceFont);
}

QColor QSGTextEdit::color() const
{
    Q_D(const QSGTextEdit);
    return d->color;
}

void QSGTextEdit::setColor(const QColor &color)
{
    Q_D(QSGTextEdit);
    if (d->color == color)
        return;

    d->color = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Text, color);
    d->control->setPalette(pal);
    update();
    emit colorChanged(d->color);
}

QColor QSGTextEdit::selectionColor() const
{
    Q_D(const QSGTextEdit);
    return d->selectionColor;
}

void QSGTextEdit::setSelectionColor(const QColor &color)
{
    Q_D(QSGTextEdit);
    if (d->selectionColor == color)
        return;

    d->selectionColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::Highlight, color);
    d->control->setPalette(pal);
    update();
    emit selectionColorChanged(d->selectionColor);
}

QColor QSGTextEdit::selectedTextColor() const
{
    Q_D(const QSGTextEdit);
    return d->selectedTextColor;
}

void QSGTextEdit::setSelectedTextColor(const QColor &color)
{
    Q_D(QSGTextEdit);
    if (d->selectedTextColor == color)
        return;

    d->selectedTextColor = color;
    QPalette pal = d->control->palette();
    pal.setColor(QPalette::HighlightedText, color);
    d->control->setPalette(pal);
    update();
    emit selectedTextColorChanged(d->selectedTextColor);
}

QSGTextEdit::HAlignment QSGTextEdit::hAlign() const
{
    Q_D(const QSGTextEdit);
    return d->hAlign;
}

void QSGTextEdit::setHAlign(HAlignment align)
{
    Q_D(QSGTextEdit);
    bool forceAlign = d->hAlignImplicit && d->effectiveLayoutMirror;
    d->hAlignImplicit = false;
    if (d->setHAlign(align, forceAlign) && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

void QSGTextEdit::resetHAlign()
{
    Q_D(QSGTextEdit);
    d->hAlignImplicit = true;
    if (d->determineHorizontalAlignment() && isComponentComplete()) {
        d->updateDefaultTextOption();
        updateSize();
    }
}

QSGTextEdit::HAlignment QSGTextEdit::effectiveHAlign() const
{
    Q_D(const QSGTextEdit);
    QSGTextEdit::HAlignment effectiveAlignment = d->hAlign;
    if (!d->hAlignImplicit && d->effectiveLayoutMirror) {
        switch (d->hAlign) {
        case QSGTextEdit::AlignLeft:
            effectiveAlignment = QSGTextEdit::AlignRight;
            break;
        case QSGTextEdit::AlignRight:
            effectiveAlignment = QSGTextEdit::AlignLeft;
            break;
        default:
            break;
        }
    }
    return effectiveAlignment;
}

bool QSGTextEditPrivate::setHAlign(QSGTextEdit::HAlignment alignment, bool forceAlign)
{
    Q_Q(QSGTextEdit);
    if (hAlign != alignment || forceAlign) {
        QSGTextEdit::HAlignment oldEffectiveHAlign = q->effectiveHAlign();
        hAlign = alignment;
        emit q->horizontalAlignmentChanged(alignment);
        if (oldEffectiveHAlign != q->effectiveHAlign())
            emit q->effectiveHorizontalAlignmentChanged();
        return true;
    }
    return false;
}

bool QSGTextEditPrivate::determineHorizontalAlignment()
{
    Q_Q(QSGTextEdit);
    if (hAlignImplicit && q->isComponentComplete()) {
        bool alignToRight = text.isEmpty() ? QApplication::keyboardInputDirection() == Qt::RightToLeft : rightToLeftText;
        return setHAlign(alignToRight ? QSGTextEdit::AlignRight : QSGTextEdit::AlignLeft);
    }
    return false;
}

void QSGTextEditPrivate::mirrorChange()
{
    Q_Q(QSGTextEdit);
    if (q->isComponentComplete()) {
        if (!hAlignImplicit && (hAlign == QSGTextEdit::AlignRight || hAlign == QSGTextEdit::AlignLeft)) {
            updateDefaultTextOption();
            q->updateSize();
            emit q->effectiveHorizontalAlignmentChanged();
        }
    }
}

QSGTextEdit::VAlignment QSGTextEdit::vAlign() const
{
    Q_D(const QSGTextEdit);
    return d->vAlign;
}

void QSGTextEdit::setVAlign(QSGTextEdit::VAlignment alignment)
{
    Q_D(QSGTextEdit);
    if (alignment == d->vAlign)
        return;
    d->vAlign = alignment;
    d->updateDefaultTextOption();
    updateSize();
    emit verticalAlignmentChanged(d->vAlign);
}

QSGTextEdit::WrapMode QSGTextEdit::wrapMode() const
{
    Q_D(const QSGTextEdit);
    return d->wrapMode;
}

void QSGTextEdit::setWrapMode(WrapMode mode)
{
    Q_D(QSGTextEdit);
    if (mode == d->wrapMode)
        return;
    d->wrapMode = mode;
    d->updateDefaultTextOption();
    updateSize();
    emit wrapModeChanged();
}

int QSGTextEdit::lineCount() const
{
    Q_D(const QSGTextEdit);
    return d->lineCount;
}

qreal QSGTextEdit::paintedWidth() const
{
    Q_D(const QSGTextEdit);
    return d->paintedSize.width();
}

qreal QSGTextEdit::paintedHeight() const
{
    Q_D(const QSGTextEdit);
    return d->paintedSize.height();
}

QRectF QSGTextEdit::positionToRectangle(int pos) const
{
    Q_D(const QSGTextEdit);
    QTextCursor c(d->document);
    c.setPosition(pos);
    return d->control->cursorRect(c);

}

int QSGTextEdit::positionAt(int x, int y) const
{
    Q_D(const QSGTextEdit);
    int r = d->document->documentLayout()->hitTest(QPoint(x,y-d->yoff), Qt::FuzzyHit);
    QTextCursor cursor = d->control->textCursor();
    if (r > cursor.position()) {
        // The cursor position includes positions within the preedit text, but only positions in the
        // same text block are offset so it is possible to get a position that is either part of the
        // preedit or the next text block.
        QTextLayout *layout = cursor.block().layout();
        const int preeditLength = layout
                ? layout->preeditAreaText().length()
                : 0;
        if (preeditLength > 0
                && d->document->documentLayout()->blockBoundingRect(cursor.block()).contains(x,y-d->yoff)) {
            r = r > cursor.position() + preeditLength
                    ? r - preeditLength
                    : cursor.position();
        }
    }
    return r;
}

void QSGTextEdit::moveCursorSelection(int pos)
{
    //Note that this is the same as setCursorPosition but with the KeepAnchor flag set
    Q_D(QSGTextEdit);
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    cursor.setPosition(pos, QTextCursor::KeepAnchor);
    d->control->setTextCursor(cursor);
}

void QSGTextEdit::moveCursorSelection(int pos, SelectionMode mode)
{
    Q_D(QSGTextEdit);
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos)
        return;
    if (mode == SelectCharacters) {
        cursor.setPosition(pos, QTextCursor::KeepAnchor);
    } else if (cursor.anchor() < pos || (cursor.anchor() == pos && cursor.position() < pos)) {
        if (cursor.anchor() > cursor.position()) {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
            if (cursor.position() == cursor.anchor())
                cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
            else
                cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
        } else {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        }

        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
        if (cursor.position() != pos)
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
    } else if (cursor.anchor() > pos || (cursor.anchor() == pos && cursor.position() > pos)) {
        if (cursor.anchor() < cursor.position()) {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
        } else {
            cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
            if (cursor.position() != cursor.anchor()) {
                cursor.setPosition(cursor.anchor(), QTextCursor::MoveAnchor);
                cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::MoveAnchor);
            }
        }

        cursor.setPosition(pos, QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        if (cursor.position() != pos) {
            cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
        }
    }
    d->control->setTextCursor(cursor);
}

bool QSGTextEdit::isCursorVisible() const
{
    Q_D(const QSGTextEdit);
    return d->cursorVisible;
}

void QSGTextEdit::setCursorVisible(bool on)
{
    Q_D(QSGTextEdit);
    if (d->cursorVisible == on)
        return;
    d->cursorVisible = on;
    QFocusEvent focusEvent(on ? QEvent::FocusIn : QEvent::FocusOut);
    if (!on && !d->persistentSelection)
        d->control->setCursorIsFocusIndicator(true);
    d->control->processEvent(&focusEvent, QPointF(0, -d->yoff));
    emit cursorVisibleChanged(d->cursorVisible);
}

int QSGTextEdit::cursorPosition() const
{
    Q_D(const QSGTextEdit);
    return d->control->textCursor().position();
}

void QSGTextEdit::setCursorPosition(int pos)
{
    Q_D(QSGTextEdit);
    if (pos < 0 || pos > d->text.length())
        return;
    QTextCursor cursor = d->control->textCursor();
    if (cursor.position() == pos && cursor.anchor() == pos)
        return;
    cursor.setPosition(pos);
    d->control->setTextCursor(cursor);
}

QDeclarativeComponent* QSGTextEdit::cursorDelegate() const
{
    Q_D(const QSGTextEdit);
    return d->cursorComponent;
}

void QSGTextEdit::setCursorDelegate(QDeclarativeComponent* c)
{
    Q_D(QSGTextEdit);
    if(d->cursorComponent){
        if(d->cursor){
            disconnect(d->control, SIGNAL(cursorPositionChanged()),
                    this, SLOT(moveCursorDelegate()));
            d->control->setCursorWidth(-1);
            update(cursorRectangle());
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

void QSGTextEdit::loadCursorDelegate()
{
    Q_D(QSGTextEdit);
    if(d->cursorComponent->isLoading())
        return;
    d->cursor = qobject_cast<QSGItem*>(d->cursorComponent->create(qmlContext(this)));
    if(d->cursor){
        connect(d->control, SIGNAL(cursorPositionChanged()),
                this, SLOT(moveCursorDelegate()));
        d->control->setCursorWidth(0);
        update(cursorRectangle());
        QDeclarative_setParent_noEvent(d->cursor, this);
        d->cursor->setParentItem(this);
        d->cursor->setHeight(QFontMetrics(d->font).height());
        moveCursorDelegate();
    }else{
        qmlInfo(this) << "Error loading cursor delegate.";
    }
}

int QSGTextEdit::selectionStart() const
{
    Q_D(const QSGTextEdit);
    return d->control->textCursor().selectionStart();
}

int QSGTextEdit::selectionEnd() const
{
    Q_D(const QSGTextEdit);
    return d->control->textCursor().selectionEnd();
}

QString QSGTextEdit::selectedText() const
{
    Q_D(const QSGTextEdit);
    return d->control->textCursor().selectedText();
}

bool QSGTextEdit::focusOnPress() const
{
    Q_D(const QSGTextEdit);
    return d->focusOnPress;
}

void QSGTextEdit::setFocusOnPress(bool on)
{
    Q_D(QSGTextEdit);
    if (d->focusOnPress == on)
        return;
    d->focusOnPress = on;
    emit activeFocusOnPressChanged(d->focusOnPress);
}

bool QSGTextEdit::persistentSelection() const
{
    Q_D(const QSGTextEdit);
    return d->persistentSelection;
}

void QSGTextEdit::setPersistentSelection(bool on)
{
    Q_D(QSGTextEdit);
    if (d->persistentSelection == on)
        return;
    d->persistentSelection = on;
    emit persistentSelectionChanged(d->persistentSelection);
}

qreal QSGTextEdit::textMargin() const
{
    Q_D(const QSGTextEdit);
    return d->textMargin;
}

void QSGTextEdit::setTextMargin(qreal margin)
{
    Q_D(QSGTextEdit);
    if (d->textMargin == margin)
        return;
    d->textMargin = margin;
    d->document->setDocumentMargin(d->textMargin);
    emit textMarginChanged(d->textMargin);
}

void QSGTextEdit::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    if (newGeometry.width() != oldGeometry.width())
        updateSize();
    QSGPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void QSGTextEdit::componentComplete()
{
    Q_D(QSGTextEdit);
    QSGPaintedItem::componentComplete();
    if (d->dirty) {
        d->determineHorizontalAlignment();
        d->updateDefaultTextOption();
        updateSize();
        d->dirty = false;
    }
}

bool QSGTextEdit::selectByMouse() const
{
    Q_D(const QSGTextEdit);
    return d->selectByMouse;
}

void QSGTextEdit::setSelectByMouse(bool on)
{
    Q_D(QSGTextEdit);
    if (d->selectByMouse != on) {
        d->selectByMouse = on;
        setKeepMouseGrab(on);
        if (on)
            setTextInteractionFlags(d->control->textInteractionFlags() | Qt::TextSelectableByMouse);
        else
            setTextInteractionFlags(d->control->textInteractionFlags() & ~Qt::TextSelectableByMouse);
        emit selectByMouseChanged(on);
    }
}

QSGTextEdit::SelectionMode QSGTextEdit::mouseSelectionMode() const
{
    Q_D(const QSGTextEdit);
    return d->mouseSelectionMode;
}

void QSGTextEdit::setMouseSelectionMode(SelectionMode mode)
{
    Q_D(QSGTextEdit);
    if (d->mouseSelectionMode != mode) {
        d->mouseSelectionMode = mode;
        d->control->setWordSelectionEnabled(mode == SelectWords);
        emit mouseSelectionModeChanged(mode);
    }
}

void QSGTextEdit::setReadOnly(bool r)
{
    Q_D(QSGTextEdit);
    if (r == isReadOnly())
        return;

    setFlag(QSGItem::ItemAcceptsInputMethod, !r);
    Qt::TextInteractionFlags flags = Qt::LinksAccessibleByMouse;
    if (d->selectByMouse)
        flags = flags | Qt::TextSelectableByMouse;
    if (!r)
        flags = flags | Qt::TextSelectableByKeyboard | Qt::TextEditable;
    d->control->setTextInteractionFlags(flags);
    if (!r)
        d->control->moveCursor(QTextCursor::End);

    emit readOnlyChanged(r);
}

bool QSGTextEdit::isReadOnly() const
{
    Q_D(const QSGTextEdit);
    return !(d->control->textInteractionFlags() & Qt::TextEditable);
}

void QSGTextEdit::setTextInteractionFlags(Qt::TextInteractionFlags flags)
{
    Q_D(QSGTextEdit);
    d->control->setTextInteractionFlags(flags);
}

Qt::TextInteractionFlags QSGTextEdit::textInteractionFlags() const
{
    Q_D(const QSGTextEdit);
    return d->control->textInteractionFlags();
}

QRect QSGTextEdit::cursorRectangle() const
{
    Q_D(const QSGTextEdit);
    return d->control->cursorRect().toRect().translated(0,-d->yoff);
}

bool QSGTextEdit::event(QEvent *event)
{
    Q_D(QSGTextEdit);
    if (event->type() == QEvent::ShortcutOverride) {
        d->control->processEvent(event, QPointF(0, -d->yoff));
        return event->isAccepted();
    }
    return QSGPaintedItem::event(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QSGTextEdit::keyPressEvent(QKeyEvent *event)
{
    Q_D(QSGTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QSGPaintedItem::keyPressEvent(event);
}

/*!
\overload
Handles the given key \a event.
*/
void QSGTextEdit::keyReleaseEvent(QKeyEvent *event)
{
    Q_D(QSGTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QSGPaintedItem::keyReleaseEvent(event);
}

void QSGTextEdit::deselect()
{
    Q_D(QSGTextEdit);
    QTextCursor c = d->control->textCursor();
    c.clearSelection();
    d->control->setTextCursor(c);
}

void QSGTextEdit::selectAll()
{
    Q_D(QSGTextEdit);
    d->control->selectAll();
}

void QSGTextEdit::selectWord()
{
    Q_D(QSGTextEdit);
    QTextCursor c = d->control->textCursor();
    c.select(QTextCursor::WordUnderCursor);
    d->control->setTextCursor(c);
}

void QSGTextEdit::select(int start, int end)
{
    Q_D(QSGTextEdit);
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

bool QSGTextEdit::isRightToLeft(int start, int end)
{
    Q_D(QSGTextEdit);
    if (start > end) {
        qmlInfo(this) << "isRightToLeft(start, end) called with the end property being smaller than the start.";
        return false;
    } else {
        return d->text.mid(start, end - start).isRightToLeft();
    }
}

#ifndef QT_NO_CLIPBOARD
void QSGTextEdit::cut()
{
    Q_D(QSGTextEdit);
    d->control->cut();
}

void QSGTextEdit::copy()
{
    Q_D(QSGTextEdit);
    d->control->copy();
}

void QSGTextEdit::paste()
{
    Q_D(QSGTextEdit);
    d->control->paste();
}
#endif // QT_NO_CLIPBOARD

/*!
\overload
Handles the given mouse \a event.
*/
void QSGTextEdit::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextEdit);
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
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QSGPaintedItem::mousePressEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QSGTextEdit::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!d->showInputPanelOnFocus) { // input panel on click
        if (d->focusOnPress && !isReadOnly() && boundingRect().contains(event->pos())) {
            if (canvas() && canvas() == qApp->focusWidget()) {
                qt_widget_private(canvas())->handleSoftwareInputPanel(event->button(), d->clickCausedFocus);
            }
        }
    }
    d->clickCausedFocus = false;

    if (!event->isAccepted())
        QSGPaintedItem::mouseReleaseEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QSGTextEdit::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QSGPaintedItem::mouseDoubleClickEvent(event);
}

/*!
\overload
Handles the given mouse \a event.
*/
void QSGTextEdit::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGTextEdit);
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (!event->isAccepted())
        QSGPaintedItem::mouseMoveEvent(event);
}

/*!
\overload
Handles the given input method \a event.
*/
void QSGTextEdit::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QSGTextEdit);
    const bool wasComposing = isInputMethodComposing();
    d->control->processEvent(event, QPointF(0, -d->yoff));
    if (wasComposing != isInputMethodComposing())
        emit inputMethodComposingChanged();
}

void QSGTextEdit::itemChange(ItemChange change, const ItemChangeData &value)
{
    Q_D(QSGTextEdit);
    if (change == ItemActiveFocusHasChanged) {
        setCursorVisible(value.boolValue && d->canvas && d->canvas->hasFocus());
    }
    QSGItem::itemChange(change, value);
}

/*!
\overload
Returns the value of the given \a property.
*/
QVariant QSGTextEdit::inputMethodQuery(Qt::InputMethodQuery property) const
{
    Q_D(const QSGTextEdit);
    return d->control->inputMethodQuery(property);
}

/*!
Draws the contents of the text edit using the given \a painter within
the given \a bounds.
*/
void QSGTextEdit::paint(QPainter *painter)
{
    // XXX todo
    QRect bounds(0, 0, width(), height());
    Q_D(QSGTextEdit);

    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->translate(0,d->yoff);

    d->control->drawContents(painter, bounds.translated(0,-d->yoff));

    painter->translate(0,-d->yoff);
}

void QSGTextEdit::updateImgCache(const QRectF &rf)
{
    Q_D(const QSGTextEdit);
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
    update(r);
}

bool QSGTextEdit::canPaste() const
{
    Q_D(const QSGTextEdit);
    return d->canPaste;
}

bool QSGTextEdit::isInputMethodComposing() const
{
    Q_D(const QSGTextEdit);
    if (QTextLayout *layout = d->control->textCursor().block().layout())
        return layout->preeditAreaText().length() > 0;
    return false;
}

void QSGTextEditPrivate::init()
{
    Q_Q(QSGTextEdit);

    q->setSmooth(smooth);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QSGItem::ItemAcceptsInputMethod);

    control = new QTextControl(q);
    control->setIgnoreUnusedNavigationEvents(true);
    control->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByKeyboard | Qt::TextEditable);
    control->setDragEnabled(false);

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
    QObject::connect(control, SIGNAL(linkActivated(QString)), q, SIGNAL(linkActivated(QString)));
#ifndef QT_NO_CLIPBOARD
    QObject::connect(q, SIGNAL(readOnlyChanged(bool)), q, SLOT(q_canPasteChanged()));
    QObject::connect(QApplication::clipboard(), SIGNAL(dataChanged()), q, SLOT(q_canPasteChanged()));
    canPaste = control->canPaste();
#endif

    document = control->document();
    document->setDefaultFont(font);
    document->setDocumentMargin(textMargin);
    document->setUndoRedoEnabled(false); // flush undo buffer.
    document->setUndoRedoEnabled(true);
    updateDefaultTextOption();
}

void QSGTextEdit::q_textChanged()
{
    Q_D(QSGTextEdit);
    d->text = text();
    d->rightToLeftText = d->document->begin().layout()->engine()->isRightToLeft();
    d->determineHorizontalAlignment();
    d->updateDefaultTextOption();
    updateSize();
    updateTotalLines();
    updateMicroFocus();
    emit textChanged(d->text);
}

void QSGTextEdit::moveCursorDelegate()
{
    Q_D(QSGTextEdit);
    if(!d->cursor)
        return;
    QRectF cursorRect = d->control->cursorRect();
    d->cursor->setX(cursorRect.x());
    d->cursor->setY(cursorRect.y());
}

void QSGTextEditPrivate::updateSelection()
{
    Q_Q(QSGTextEdit);
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

void QSGTextEdit::updateSelectionMarkers()
{
    Q_D(QSGTextEdit);
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

QRectF QSGTextEdit::boundingRect() const
{
    Q_D(const QSGTextEdit);
    QRectF r = QSGPaintedItem::boundingRect();
    int cursorWidth = 1;
    if(d->cursor)
        cursorWidth = d->cursor->width();
    if(!d->document->isEmpty())
        cursorWidth += 3;// ### Need a better way of accounting for space between char and cursor

    // Could include font max left/right bearings to either side of rectangle.

    r.setRight(r.right() + cursorWidth);
    return r.translated(0,d->yoff);
}

qreal QSGTextEditPrivate::getImplicitWidth() const
{
    Q_Q(const QSGTextEdit);
    if (!requireImplicitWidth) {
        // We don't calculate implicitWidth unless it is required.
        // We need to force a size update now to ensure implicitWidth is calculated
        const_cast<QSGTextEditPrivate*>(this)->requireImplicitWidth = true;
        const_cast<QSGTextEdit*>(q)->updateSize();
    }
    return implicitWidth;
}

//### we should perhaps be a bit smarter here -- depending on what has changed, we shouldn't
//    need to do all the calculations each time
void QSGTextEdit::updateSize()
{
    Q_D(QSGTextEdit);
    if (isComponentComplete()) {
        qreal naturalWidth = d->implicitWidth;
        // ### assumes that if the width is set, the text will fill to edges
        // ### (unless wrap is false, then clipping will occur)
        if (widthValid()) {
            if (!d->requireImplicitWidth) {
                emit implicitWidthChanged();
                // if the implicitWidth is used, then updateSize() has already been called (recursively)
                if (d->requireImplicitWidth)
                    return;
            }
            if (d->requireImplicitWidth) {
                d->document->setTextWidth(-1);
                naturalWidth = d->document->idealWidth();
            }
            if (d->document->textWidth() != width())
                d->document->setTextWidth(width());
        } else {
            d->document->setTextWidth(-1);
        }
        QFontMetrics fm = QFontMetrics(d->font);
        int dy = height();
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
        if (nyoff != d->yoff) 
            d->yoff = nyoff;
        setBaselineOffset(fm.ascent() + d->yoff + d->textMargin);

        //### need to comfirm cost of always setting these
        int newWidth = qCeil(d->document->idealWidth());
        if (!widthValid() && d->document->textWidth() != newWidth)
            d->document->setTextWidth(newWidth); // ### Text does not align if width is not set (QTextDoc bug)
        // ### Setting the implicitWidth triggers another updateSize(), and unless there are bindings nothing has changed.
        if (!widthValid())
            setImplicitWidth(newWidth);
        else if (d->requireImplicitWidth)
            setImplicitWidth(naturalWidth);
        qreal newHeight = d->document->isEmpty() ? fm.height() : (int)d->document->size().height();
        setImplicitHeight(newHeight);

        d->paintedSize = QSize(newWidth, newHeight);
        setContentsSize(d->paintedSize);
        emit paintedSizeChanged();
    } else {
        d->dirty = true;
    }
    update();
}

void QSGTextEdit::updateTotalLines()
{
    Q_D(QSGTextEdit);

    int subLines = 0;

    for (QTextBlock it = d->document->begin(); it != d->document->end(); it = it.next()) {
        QTextLayout *layout = it.layout();
        if (!layout)
            continue;
        subLines += layout->lineCount()-1;
    }

    int newTotalLines = d->document->lineCount() + subLines;
    if (d->lineCount != newTotalLines) {
        d->lineCount = newTotalLines;
        emit lineCountChanged();
    }
}

void QSGTextEditPrivate::updateDefaultTextOption()
{
    Q_Q(QSGTextEdit);
    QTextOption opt = document->defaultTextOption();
    int oldAlignment = opt.alignment();

    QSGTextEdit::HAlignment horizontalAlignment = q->effectiveHAlign();
    if (rightToLeftText) {
        if (horizontalAlignment == QSGTextEdit::AlignLeft)
            horizontalAlignment = QSGTextEdit::AlignRight;
        else if (horizontalAlignment == QSGTextEdit::AlignRight)
            horizontalAlignment = QSGTextEdit::AlignLeft;
    }
    opt.setAlignment((Qt::Alignment)(int)(horizontalAlignment | vAlign));

    QTextOption::WrapMode oldWrapMode = opt.wrapMode();
    opt.setWrapMode(QTextOption::WrapMode(wrapMode));

    if (oldWrapMode == opt.wrapMode() && oldAlignment == opt.alignment())
        return;
    document->setDefaultTextOption(opt);
}


void QSGTextEdit::openSoftwareInputPanel()
{
    if (qApp) {
        if (canvas() && canvas() == qApp->focusWidget()) {
            QEvent event(QEvent::RequestSoftwareInputPanel);
            QApplication::sendEvent(canvas(), &event);
        }
    }
}

void QSGTextEdit::closeSoftwareInputPanel()
{
    if (qApp) {
        if (canvas() && canvas() == qApp->focusWidget()) {
            QEvent event(QEvent::CloseSoftwareInputPanel);
            QApplication::sendEvent(canvas(), &event);
        }
    }
}

void QSGTextEdit::focusInEvent(QFocusEvent *event)
{
    Q_D(const QSGTextEdit);
    if (d->showInputPanelOnFocus) {
        if (d->focusOnPress && !isReadOnly()) {
            openSoftwareInputPanel();
        }
    }
    QSGPaintedItem::focusInEvent(event);
}

void QSGTextEdit::q_canPasteChanged()
{
    Q_D(QSGTextEdit);
    bool old = d->canPaste;
    d->canPaste = d->control->canPaste();
    if(old!=d->canPaste)
        emit canPasteChanged();
}

QT_END_NAMESPACE
