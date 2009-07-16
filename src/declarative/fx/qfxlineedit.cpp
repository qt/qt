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

QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QFxLineEdit,LineEdit);
QML_DEFINE_TYPE(QIntValidator,QIntValidator);

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

QmlFont *QFxLineEdit::font()
{
    Q_D(QFxLineEdit);
    return d->font;
}

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

/*
QFxText::TextStyle QFxLineEdit::style() const
{
    Q_D(const QFxLineEdit);
    return d->style;
}

void QFxLineEdit::setStyle(QFxText::TextStyle style)
{
    Q_D(QFxLineEdit);
    d->style = style;
}

QColor QFxLineEdit::styleColor() const
{
    Q_D(const QFxLineEdit);
    return d->styleColor;
}

void QFxLineEdit::setStyleColor(const QColor &c)
{
    Q_D(QFxLineEdit);
    d->styleColor = c;
}
*/

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

QFxText::VAlignment QFxLineEdit::vAlign() const
{
    Q_D(const QFxLineEdit);
    return d->vAlign;
}

void QFxLineEdit::setVAlign(QFxText::VAlignment align)
{
    Q_D(QFxLineEdit);
    d->vAlign = align;
}

//### Should this also toggle cursor visibility?
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

int QFxLineEdit::selectionLength() const
{
    Q_D(const QFxLineEdit);
    return d->control->selectionEnd() - d->control->selectionStart();
}

void QFxLineEdit::setSelectionLength(int len)
{
    Q_D(QFxLineEdit);
    d->control->setSelection(d->control->cursor(), len);
}

QString QFxLineEdit::selectedText() const
{
    Q_D(const QFxLineEdit);
    return d->control->selectedText();
}

bool QFxLineEdit::isAwesome() const
{
    Q_D(const QFxLineEdit);
    return d->awesome;
}

#include <QTimer> //Can be removed along wit the property
void QFxLineEdit::setAwesome(bool a)
{
    Q_D(QFxLineEdit);
    d->awesome = a;
    if(a){
        setColor(QColor(0,0,255));
        rainbowRedraw();
    }
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

    //TODO: Clean up this cut'n'pasted section from QLineEdit
    QRect lineRect(r);

    int cix = qRound(d->control->cursorToX());

    // horizontal scrolling. d->hscroll is the left indent from the beginning
    // of the text line to the left edge of lineRect. we update this value
    // depending on the delta from the last paint event; in effect this means
    // the below code handles all scrolling based on the textline (widthUsed,
    // minLB, minRB), the line edit rect (lineRect) and the cursor position
    // (cix).
    QFontMetrics fm = QApplication::fontMetrics();
    int minLB = qMax(0, -fm.minLeftBearing());
    int minRB = qMax(0, -fm.minRightBearing());
    int widthUsed = d->control->width() + minRB;
    if ((minLB + widthUsed) <=  lineRect.width()) {
        // text fits in lineRect; use hscroll for alignment
        d->hscroll = 0;
        d->hscroll -= minLB;
    } else if (cix - d->hscroll >= lineRect.width()) {
        // text doesn't fit, cursor is to the right of lineRect (scroll right)
        d->hscroll = cix - lineRect.width() + 1;
    } else if (cix - d->hscroll < 0 && d->hscroll < widthUsed) {
        // text doesn't fit, cursor is to the left of lineRect (scroll left)
        d->hscroll = cix;
    }
    // the y offset is there to keep the baseline constant in case we have script changes in the text.
    QPoint topLeft = lineRect.topLeft() - QPoint(d->hscroll, d->control->ascent() - fm.ascent());

    if(d->hscroll != d->oldScroll)
        moveCursor();

    d->control->draw(p, topLeft, r, flags);

    d->oldScroll = d->hscroll;
    p->restore();
}

void QFxLineEditPrivate::init()
{
        Q_Q(QFxLineEdit);
        control->setCursorWidth(1);
        control->setPasswordCharacter('*');
        control->setLayoutDirection(Qt::LeftToRight);
        control->setSelection(0,0);
        q->setSmooth(true);
        q->setAcceptedMouseButtons(Qt::LeftButton);
        q->setOptions(QFxLineEdit::AcceptsInputMethods | QFxLineEdit::SimpleItem
            | QFxLineEdit::HasContents | QFxLineEdit::MouseEvents);
        q->connect(control, SIGNAL(cursorPositionChanged(int,int)),
                q, SIGNAL(cursorPositionChanged()));
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
        oldSelectLength = q->selectionLength();
}

void QFxLineEdit::selectionChanged()
{
    Q_D(QFxLineEdit);
    emit selectedTextChanged();
    if(selectionLength() != d->oldSelectLength){
        d->oldSelectLength = selectionLength();
        emit selectionLengthChanged();
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

void QFxLineEdit::rainbowRedraw()
{
    Q_D(QFxLineEdit);
    if(!d->awesome)
        return;
    setColor(QColor::fromHsv((d->color.hue() + 5)%360, d->color.saturation(), d->color.value()));
    updateAll();
    QTimer::singleShot(50, this, SLOT(rainbowRedraw()));
}
QT_END_NAMESPACE

