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

#include "qsimplecanvasitem.h"
#include "qsimplecanvas.h"
#include "qsimplecanvasitem_p.h"
#include "qsimplecanvas_p.h"
#include <qfxitem.h>
#include <QGraphicsSceneEvent>


QT_BEGIN_NAMESPACE

static QHash<QGraphicsScene*,QSimpleCanvas*> sceneMap;

QSimpleCanvasGraphicsView::QSimpleCanvasGraphicsView(QSimpleCanvasPrivate *parent)
: QGraphicsView(parent->q), canvas(parent)
{
    setScene(&_scene);
    sceneMap[&_scene] = parent->q;
    setFrameShape(QFrame::NoFrame);
    viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
}

QSimpleCanvasGraphicsView::~QSimpleCanvasGraphicsView()
{
    sceneMap.remove(&_scene);
}


/*!
  \internal
    \class QSimpleCanvasItem
    \brief The QSimpleCanvasItem class is the base class of canvas items.
 */
QSimpleCanvasLayer::QSimpleCanvasLayer()
{
}

QSimpleCanvasLayer::QSimpleCanvasLayer(QSimpleCanvasItem *parent)
: QSimpleCanvasItem(parent)
{
}

void QSimpleCanvasLayer::addChild(QSimpleCanvasItem *c)
{
    QSimpleCanvasItem::addChild(c);
}

void QSimpleCanvasLayer::addDirty(QSimpleCanvasItem *)
{
}

void QSimpleCanvasLayer::remDirty(QSimpleCanvasItem *)
{
}

QSimpleCanvasLayer *QSimpleCanvasLayer::layer() 
{
    return this;
}

QSimpleCanvasItem::Options QSimpleCanvasItem::options() const
{
    Q_D(const QSimpleCanvasItem);
    return (QSimpleCanvasItem::Options)d->options;
}

bool QSimpleCanvasItem::mouseFilter(QGraphicsSceneMouseEvent *)
{
    return false;
}

bool QSimpleCanvasItem::sceneEvent(QEvent *event)
{
    bool rv = QGraphicsItem::sceneEvent(event);
    if (event->type() == QEvent::UngrabMouse)
        mouseUngrabEvent();
    return rv;
}

QVariant QSimpleCanvasItem::itemChange(GraphicsItemChange change, 
                                       const QVariant &value)
{
    Q_D(QSimpleCanvasItem);
    if (change == ItemSceneHasChanged) {
        QSimpleCanvas *oldCanvas = d->canvas;
        d->canvas = sceneMap[scene()];
        if (d->canvas){
            if (hasFocus())
                canvas()->d->setFocusItem(this, Qt::OtherFocusReason);
            if (options() & QSimpleCanvasItem::IsFocusPanel) {
                if (d->wantsActiveFocusPanelPendingCanvas) {
                    d->hasBeenActiveFocusPanel = true;
                    canvas()->d->setActiveFocusPanel(this);
                    d->wantsActiveFocusPanelPendingCanvas = false;
                }
            }
        }
        if (options() & QSimpleCanvasItem::MouseFilter)
            d->gvRemoveMouseFilter();

        if (oldCanvas != d->canvas)
            canvasChanged();

        if (options() & QSimpleCanvasItem::MouseFilter)
            d->gvAddMouseFilter();
    } 

    return QGraphicsItem::itemChange(change, value);
}


void QSimpleCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
#if 0
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
    owner->mousePressEvent(event);
#endif
    e->ignore();
}

void QSimpleCanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
#if 0
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
    owner->mouseReleaseEvent(event);
    ungrabMouse();
#endif
    e->ignore();
}

void QSimpleCanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
#if 0
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
#endif
    e->ignore();
}

void QSimpleCanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
#if 0
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
#endif
    e->ignore();
}

void QSimpleCanvasItem::hoverEnterEvent(QGraphicsSceneHoverEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::mouseUngrabEvent()
{
}

void QSimpleCanvasItem::keyPressEvent(QKeyEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::keyReleaseEvent(QKeyEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::focusOutEvent(QFocusEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::focusInEvent(QFocusEvent *e)
{
    if (!hasFocus())
        setFocus(true);
    e->ignore();
}

void QSimpleCanvasItem::activePanelInEvent()
{
}

void QSimpleCanvasItem::activePanelOutEvent()
{
}

void QSimpleCanvasItem::inputMethodEvent(QInputMethodEvent *e)
{
    e->ignore();
}

QVariant QSimpleCanvasItem::inputMethodQuery(Qt::InputMethodQuery) const
{
    return QVariant();
}

void QSimpleCanvasItem::childrenChanged()
{
}

void QSimpleCanvasItem::setPaintMargin(qreal margin)
{
    Q_D(QSimpleCanvasItem);
    if (margin < d->paintmargin)
        update(); // schedule repaint of old boundingRect
    d->paintmargin = margin;
}

QRectF QSimpleCanvasItem::boundingRect() const
{
    Q_D(const QSimpleCanvasItem);
    return QRectF(-d->paintmargin, -d->paintmargin, d->width+d->paintmargin*2, d->height+d->paintmargin*2);
}

void QSimpleCanvasItem::paintContents(QPainter &)
{
}

void QSimpleCanvasItem::canvasChanged()
{
}

void QSimpleCanvasItem::focusChanged(bool)
{
}

void QSimpleCanvasItem::activeFocusChanged(bool)
{
}

void QSimpleCanvasItem::parentChanged(QSimpleCanvasItem *, QSimpleCanvasItem *)
{
}

/*!
    Returns the item's (0, 0) point relative to its parent.
 */
QPointF QSimpleCanvasItem::pos() const
{
    return QPointF(x(),y());
}

/*!
    Returns the item's (0, 0) point mapped to scene coordinates.
 */
QPointF QSimpleCanvasItem::scenePos() const
{
    return mapToScene(QPointF(0, 0));
}

/*!
    \enum QSimpleCanvasItem::TransformOrigin

    Controls the point about which simple transforms like scale apply.

    \value TopLeft The top-left corner of the item.
    \value TopCenter The center point of the top of the item.
    \value TopRight The top-right corner of the item.
    \value MiddleLeft The left most point of the vertical middle.
    \value Center The center of the item.
    \value MiddleRight The right most point of the vertical middle.
    \value BottomLeft The bottom-left corner of the item.
    \value BottomCenter The center point of the bottom of the item. 
    \value BottomRight The bottom-right corner of the item.
*/

/*!
    Returns the current transform origin.
*/
QSimpleCanvasItem::TransformOrigin QSimpleCanvasItem::transformOrigin() const
{
    Q_D(const QSimpleCanvasItem);
    return d->origin;
}

/*!
    Set the transform \a origin.
*/
void QSimpleCanvasItem::setTransformOrigin(TransformOrigin origin)
{
    Q_D(QSimpleCanvasItem);
    if (origin != d->origin) {
        d->origin = origin;
        update();
    }
}

QPointF QSimpleCanvasItem::transformOriginPoint() const
{
    Q_D(const QSimpleCanvasItem);
    return d->transformOrigin();
}

/*!
    Returns the canvas the item is on, or 0 if the item is not on a canvas.
 */
QSimpleCanvas *QSimpleCanvasItem::canvas() const
{
    Q_D(const QSimpleCanvasItem);
    return d->canvas;
}

/*!
    Returns the parent if the item, or 0 if the item has no parent.
 */
QSimpleCanvasItem *QSimpleCanvasItem::parent() const
{
    Q_D(const QSimpleCanvasItem);
    return d->parent;
}

void QSimpleCanvasItemPrivate::canvasChanged(QSimpleCanvas *newCanvas, QSimpleCanvas *oldCanvas)
{
    Q_Q(QSimpleCanvasItem);
    canvas = newCanvas;
    if (options & QSimpleCanvasItem::MouseFilter) {
        if (oldCanvas) oldCanvas->d->removeMouseFilter(q);
        if (newCanvas) newCanvas->d->installMouseFilter(q);
    }
    if (newCanvas) {
       if (!oldCanvas && hasFocus) 
           newCanvas->d->setFocusItem(q, Qt::OtherFocusReason, false);
       if (wantsActiveFocusPanelPendingCanvas) {
           hasBeenActiveFocusPanel = true;
           newCanvas->d->setActiveFocusPanel(q);
           wantsActiveFocusPanelPendingCanvas = false;
       }
    }

    for (int ii = 0; ii < children.count(); ++ii)
        children.at(ii)->d_func()->canvasChanged(newCanvas, oldCanvas);
    q->canvasChanged();
}

void QSimpleCanvasItem::setFocus(bool focus)
{
    Q_D(QSimpleCanvasItem);
    if (d->hasFocus == focus)
        return;
    QSimpleCanvas *c = canvas();

    if (c) {
        if (focus)
            c->d->setFocusItem(this, Qt::OtherFocusReason);
        else 
            c->d->clearFocusItem(this);
    } else {
        d->setFocus(focus);
        focusChanged(d->hasFocus);
    }
}

qreal QSimpleCanvasItem::z() const
{
    Q_D(const QSimpleCanvasItem);
    return zValue();
}

void QSimpleCanvasItem::setX(qreal x)
{
    Q_D(QSimpleCanvasItem);
    if (x == this->x())
        return;

    qreal oldX = this->x();

    QGraphicsItem::setPos(x, y());

    geometryChanged(QRectF(this->x(), y(), width(), height()), 
                    QRectF(oldX, y(), width(), height()));
}

void QSimpleCanvasItem::setY(qreal y)
{
    if (y == this->y())
        return;

    qreal oldY = this->y();

    QGraphicsItem::setPos(x(), y);

    geometryChanged(QRectF(x(), this->y(), width(), height()), 
                    QRectF(x(), oldY, width(), height()));
}

void QSimpleCanvasItem::setZ(qreal z)
{
    if (z == this->z())
        return;

    if (z < 0)
        setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    else
        setFlag(QGraphicsItem::ItemStacksBehindParent, false);

    setZValue(z);
}

qreal QSimpleCanvasItem::width() const
{
    Q_D(const QSimpleCanvasItem);
    return d->width;
}

void QSimpleCanvasItem::setWidth(qreal w)
{
    Q_D(QSimpleCanvasItem);
    d->widthValid = true;
    if (d->width == w)
        return;

    qreal oldWidth = d->width;

    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), oldWidth, height()));
}

void QSimpleCanvasItem::setImplicitWidth(qreal w)
{
    Q_D(QSimpleCanvasItem);
    if (d->width == w || widthValid())
        return;

    qreal oldWidth = d->width;

    d->width = w;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), oldWidth, height()));
}

bool QSimpleCanvasItem::widthValid() const
{
    Q_D(const QSimpleCanvasItem);
    return d->widthValid;
}

qreal QSimpleCanvasItem::height() const
{
    Q_D(const QSimpleCanvasItem);
    return d->height;
}

void QSimpleCanvasItem::setHeight(qreal h)
{
    Q_D(QSimpleCanvasItem);
    d->heightValid = true;
    if (d->height == h)
        return;

    qreal oldHeight = d->height;

    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), width(), oldHeight));
}

void QSimpleCanvasItem::setImplicitHeight(qreal h)
{
    Q_D(QSimpleCanvasItem);
    if (d->height == h || heightValid())
        return;

    qreal oldHeight = d->height;

    d->height = h;
    update();

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(x(), y(), width(), oldHeight));
}

bool QSimpleCanvasItem::heightValid() const
{
    Q_D(const QSimpleCanvasItem);
    return d->heightValid;
}

void QSimpleCanvasItem::setPos(const QPointF &point)
{
    qreal oldX = x();
    qreal oldY = y();

    QGraphicsItem::setPos(point);

    geometryChanged(QRectF(x(), y(), width(), height()), 
                    QRectF(oldX, oldY, width(), height()));
}

qreal QSimpleCanvasItem::scale() const
{
    Q_D(const QSimpleCanvasItem);
    return d->scale;
}

void QSimpleCanvasItem::setScale(qreal s)
{
    Q_D(QSimpleCanvasItem);
    d->scale = s;
    QTransform t;
    QPointF to = transformOriginPoint();
    if (to.x() != 0. || to.y() != 0.)
        t.translate(to.x(), to.y());
    t.scale(s, s);
    if (to.x() != 0. || to.y() != 0.)
        t.translate(-to.x(), -to.y());
    setTransform(t * d->transform);
}

bool QSimpleCanvasItem::isVisible() const
{
    if (visible() <= 0)
        return false;
    else if (!parent())
        return true;
    else
        return parent()->isVisible();
}

qreal QSimpleCanvasItem::visible() const
{
    return opacity();
}

void QSimpleCanvasItem::setVisible(qreal v)
{
    setOpacity(v);
}

void QSimpleCanvasItem::addChild(QSimpleCanvasItem *c)
{
    Q_D(QSimpleCanvasItem);
    d->children.append(c);
    childrenChanged();
}

void QSimpleCanvasItem::remChild(QSimpleCanvasItem *c)
{
    Q_D(QSimpleCanvasItem);
    d->children.removeAll(c);
    childrenChanged();
}

const QList<QSimpleCanvasItem *> &QSimpleCanvasItem::children() const
{
    Q_D(const QSimpleCanvasItem);
    return d->children;
}

bool QSimpleCanvasItem::hasChildren() const
{   
    Q_D(const QSimpleCanvasItem);
    return !d->children.isEmpty();
}

QSimpleCanvasLayer *QSimpleCanvasItem::layer() 
{
    if (parent())
        return parent()->layer();
    else
        return 0;
}

bool QSimpleCanvasItem::clip() const
{
    Q_D(const QSimpleCanvasItem);
    return d->clip;
}

void QSimpleCanvasItem::setClip(bool c)
{
    Q_D(const QSimpleCanvasItem);
    if (bool(d->clip) == c)
        return;

    if (c) 
        setClipType(ClipToRect);
    else 
        setClipType(NoClip);

    update();
}

QSimpleCanvasItem::ClipType QSimpleCanvasItem::clipType() const
{
    Q_D(const QSimpleCanvasItem);
    return d->clip;
}

void QSimpleCanvasItem::setClipType(ClipType c)
{
    Q_D(QSimpleCanvasItem);
    d->clip = c;
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, bool(c));
}

QRect QSimpleCanvasItem::itemBoundingRect()
{
    return boundingRect().toAlignedRect();
}

QPointF QSimpleCanvasItem::mapFromScene(const QPointF &p) const
{
    return QGraphicsItem::mapFromScene(p);
}

QRectF QSimpleCanvasItem::mapFromScene(const QRectF &r) const
{
    return QGraphicsItem::mapFromScene(r).boundingRect();
}

QPointF QSimpleCanvasItem::mapToScene(const QPointF &p) const
{
    return QGraphicsItem::mapToScene(p);
}

QRectF QSimpleCanvasItem::mapToScene(const QRectF &r) const
{
    return QGraphicsItem::mapToScene(r).boundingRect();
}

QTransform QSimpleCanvasItem::transform() const
{
    Q_D(const QSimpleCanvasItem);
    return d->transform;
}

void QSimpleCanvasItem::setTransform(const QTransform &m)
{
    Q_D(QSimpleCanvasItem);
    d->transform = m;
    QGraphicsItem::setTransform(QTransform::fromScale(d->scale, d->scale) * d->transform);
}

QSimpleCanvasItem *QSimpleCanvasItem::mouseGrabberItem() const
{
    QGraphicsScene *s = scene();
    if (s) {
        QGraphicsItem *item = s->mouseGrabberItem();
        return static_cast<QSimpleCanvasItem*>(item); // ###
    }
    return 0;
}

bool QSimpleCanvasItem::isFocusable() const
{
    Q_D(const QSimpleCanvasItem);
    return d->focusable;
}

void QSimpleCanvasItem::setFocusable(bool f)
{
    Q_D(QSimpleCanvasItem);
    d->focusable = f;
}

bool QSimpleCanvasItem::hasFocus() const
{
    Q_D(const QSimpleCanvasItem);
    return d->hasFocus;
}

void QSimpleCanvasItemPrivate::setFocus(bool f)
{
    hasFocus = f;
}

void QSimpleCanvasItemPrivate::setActiveFocus(bool f)
{
    Q_Q(QSimpleCanvasItem);
    hasActiveFocus = f;

    if (f) {
        if (!(q->flags() & QGraphicsItem::ItemIsFocusable))
            q->setFlag(QGraphicsItem::ItemIsFocusable);
        q->QGraphicsItem::setFocus();
    } else {
        q->clearFocus();
        if ((q->flags() & QGraphicsItem::ItemIsFocusable) && !focusable)
            q->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

bool QSimpleCanvasItem::eventFilter(QObject *o, QEvent *e)
{
    switch(e->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
        if (mouseFilter(static_cast<QGraphicsSceneMouseEvent *>(e))) 
            return true;
        break;
    default:
        break;
    }

    return QObject::eventFilter(o, e);
}

void QSimpleCanvasItem::setOptions(Options options, bool set)
{
    Q_D(QSimpleCanvasItem);
    Options old = (Options)d->options;

    if (options & IsFocusPanel) {
        if (!set) {
            qWarning("QSimpleCanvasItem::setOptions: Cannot unset IsFocusPanel");
            return;
        } else if (hasChildren()) {
            qWarning("QSimpleCanvasItem::setOptions: Cannot set IsFocusPanel once item has children");
            return;
        }
    }

    if (options & IsFocusRealm) {
        if (!set) {
            qWarning("QSimpleCanvasItem::setOptions: Cannot unset IsFocusRealm");
            return;
        } 
    }

    if (set)
        d->options |= options;
    else
        d->options &= ~options;

    if ((d->options & IsFocusPanel) && (d->options & IsFocusRealm)) {
        qWarning("QSimpleCanvasItem::setOptions: Cannot set both IsFocusPanel and IsFocusRealm.  IsFocusRealm will be unset.");
        d->options &= ~IsFocusRealm;
    }

    setFlag(QGraphicsItem::ItemHasNoContents, !(d->options & HasContents));

    if ((old & MouseFilter) != (d->options & MouseFilter)) {
        if (d->options & MouseFilter)
            d->gvAddMouseFilter();
        else
            d->gvRemoveMouseFilter();
    }
}

QSimpleCanvasItem::QSimpleCanvasItem(QSimpleCanvasItemPrivate &dd, QSimpleCanvasItem *parent)
: QGraphicsObject(dd, parent, 0) 
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
}

QSimpleCanvasItem::QSimpleCanvasItem(QSimpleCanvasItem *p)
: QGraphicsObject(*(new QSimpleCanvasItemPrivate), p, 0) 
{
    setFlag(QGraphicsItem::ItemHasNoContents, true);
}

QSimpleCanvasItem::~QSimpleCanvasItem()
{ 
    Q_D(QSimpleCanvasItem);
    if ((d->options & (IsFocusPanel|IsFocusRealm)) && d->canvas)
        d->canvas->d->focusPanelData.remove(this);
    if (d->hasFocus && d->canvas) {
        QSimpleCanvasItem *prnt = parent();
        while (prnt && !(prnt->options() & (IsFocusPanel|IsFocusRealm)))
            prnt = prnt->parent();
        if (prnt && d->canvas->d->focusPanelData.value(prnt) == this)
            d->canvas->d->focusPanelData.remove(prnt);
    }

    qDeleteAll(children());
    if (parent())
        parent()->remChild(this);
}

QSimpleCanvasItem::operator QGraphicsItem *()
{
    return this;
}

QSimpleCanvasItem::operator QmlDebuggerStatus *()
{
    Q_D(QSimpleCanvasItem);
    if(!d->debuggerStatus)
        d->debuggerStatus = new QSimpleCanvasItemDebuggerStatus(this);
    return d->debuggerStatus;
}

QPointF QSimpleCanvasItemPrivate::transformOrigin() const
{
    Q_Q(const QSimpleCanvasItem);

    QRectF br = q->boundingRect();

    switch(origin) {
    default:
    case QSimpleCanvasItem::TopLeft:
        return QPointF(0, 0);
    case QSimpleCanvasItem::TopCenter:
        return QPointF(br.width() / 2., 0);
    case QSimpleCanvasItem::TopRight:
        return QPointF(br.width(), 0);
    case QSimpleCanvasItem::MiddleLeft:
        return QPointF(0, br.height() / 2.);
    case QSimpleCanvasItem::Center:
        return QPointF(br.width() / 2., br.height() / 2.);
    case QSimpleCanvasItem::MiddleRight:
        return QPointF(br.width(), br.height() / 2.);
    case QSimpleCanvasItem::BottomLeft:
        return QPointF(0, br.height());
    case QSimpleCanvasItem::BottomCenter:
        return QPointF(br.width() / 2., br.height());
    case QSimpleCanvasItem::BottomRight:
        return QPointF(br.width(), br.height());
    }
}

void QSimpleCanvasItemPrivate::setParentInternal(QSimpleCanvasItem *p)
{
    Q_Q(QSimpleCanvasItem);
    QSimpleCanvasItem *oldParent = parent;
    if (oldParent)
        oldParent->remChild(q);

    parent = p;
    q->setParentItem(p);

    if (parent)
        p->addChild(q);
}

/*!
    \fn void QSimpleCanvasItem::setParent(QSimpleCanvasItem *parent)

    Sets the parent of the item to \a parent.
 */
void QSimpleCanvasItem::setParent(QSimpleCanvasItem *p)
{
    Q_D(QSimpleCanvasItem);
    if (p == parent() || !p) return;

    QObject::setParent(p);

    QSimpleCanvasItem *oldParent = d->parent;
    d->setParentInternal(p);
    parentChanged(p, oldParent);
}

int QSimpleCanvasItemPrivate::dump(int indent) 
{
    Q_Q(QSimpleCanvasItem);
    QByteArray ba(indent * 2, ' ');

    QByteArray state;
    if (options & QSimpleCanvasItem::MouseFilter)
        state.append("i");
    else
        state.append("-");
    if (options & QSimpleCanvasItem::HoverEvents)
        state.append("h");
    else
        state.append("-");
    if (options & QSimpleCanvasItem::MouseEvents)
        state.append("m");
    else
        state.append("-");
    if (options & QSimpleCanvasItem::HasContents)
        state.append("c");
    else
        state.append("-");
    if (options & QSimpleCanvasItem::SimpleItem)
        state.append("s");
    else
        state.append("-");
    if (options & QSimpleCanvasItem::IsFocusPanel) {
        if (q->activeFocusPanel())
            state.append("P");
        else
            state.append("p");
    } else {
        state.append("-");
    }
    if (options & QSimpleCanvasItem::IsFocusRealm)
        state.append("r");
    else
        state.append("-");
    if (q->hasFocus()) {
        if (q->hasActiveFocus())
            state.append("F");
        else
            state.append("f");
    } else {
        if (q->hasActiveFocus())
            state.append("X");
        else
            state.append("-");
    }

    QByteArray name;
    QFxItem *i = qobject_cast<QFxItem *>(q);
    if (i)
        name = i->id().toLatin1();
    qWarning().nospace() << ba.constData() << state.constData() << " " << children.count() << " " << " " << name.constData();

    int rv = 0;

    for (int ii = 0; ii < children.count(); ++ii)
        rv += children.at(ii)->d_func()->dump(indent + 1);

    return rv + 1;
}

bool QSimpleCanvasItemPrivate::checkFocusState(FocusStateCheckDatas d,
                                           FocusStateCheckRDatas *r)
{
    Q_Q(QSimpleCanvasItem);

    bool rv = true;
    bool isRealm = (options & QSimpleCanvasItem::IsFocusPanel ||
                    options & QSimpleCanvasItem::IsFocusRealm);

    if (options & QSimpleCanvasItem::IsFocusPanel) {

        if (q->activeFocusPanel()) {
            if (d & InActivePanel) {
                qWarning() << "State ERROR: Nested active focus panels";
                rv = false;
            }

            d |= InActivePanel;
        } else {
            d &= ~InActivePanel;
        }

    }

    if (q->hasActiveFocus()) {
        if (!(d & InActivePanel)) {
            qWarning() << "State ERROR: Active focus in non-active panel";
            rv = false;
        }

        if (d & InRealm && !(d & InActiveFocusedRealm)) {
            qWarning() << "State ERROR: Active focus in non-active-focused realm";
            rv = false;
        }

        if (!q->hasFocus()) {
            qWarning() << "State ERROR: Active focus on element that does not have focus";
            rv = false;
        }

        if (*r & SeenActiveFocus) {
            qWarning() << "State ERROR: Two active focused elements in same realm";
            rv = false;
        }

        *r |= SeenActiveFocus;
    }

    if (q->hasFocus()) {
        if (*r & SeenFocus) {
            qWarning() << "State ERROR: Two focused elements in same realm";
            rv = false;
        }

        *r |= SeenFocus;
    }

    if (options & QSimpleCanvasItem::IsFocusRealm) {
        d |= InRealm;

        if (q->hasActiveFocus())
            d |= InActiveFocusedRealm;
        else
            d &= ~InActiveFocusedRealm;
    }

    FocusStateCheckRDatas newR = NoCheckRData;
    if (isRealm) 
        r = &newR;

    for (int ii = 0; ii < children.count(); ++ii)
        rv &= children.at(ii)->d_func()->checkFocusState(d, r);

    return rv;
}

bool QSimpleCanvasItem::activeFocusPanel() const
{
    QSimpleCanvas *c = canvas();
    if (!c) {
        Q_D(const QSimpleCanvasItem);
        return d->wantsActiveFocusPanelPendingCanvas;
    } else {
        return c->activeFocusPanel() == this;
    }
}

void QSimpleCanvasItem::setActiveFocusPanel(bool b)
{
    if (!(options() & IsFocusPanel)) {
        qWarning("QSimpleCanvasItem::setActiveFocusPanel: Item is not a focus panel");
        return;
    }

    QSimpleCanvas *c = canvas();
    Q_D(QSimpleCanvasItem);
    if (c) {
        if (b) {
            d->hasBeenActiveFocusPanel = true;
            c->d->setActiveFocusPanel(this);
        } else if (d->hasBeenActiveFocusPanel) {
            d->hasBeenActiveFocusPanel = false;
            c->d->clearFocusPanel(this);
        }
    } else {
        d->wantsActiveFocusPanelPendingCanvas = b;
    }
}

bool QSimpleCanvasItem::hasActiveFocus() const
{
    Q_D(const QSimpleCanvasItem);
    return d->hasActiveFocus;
}

QSimpleCanvasItem *QSimpleCanvasItem::findFirstFocusChild() const
{
    Q_D(const QSimpleCanvasItem);

    const QList<QSimpleCanvasItem *> &children = d->children;

    for (int i = 0; i < children.count(); ++i) {
        QSimpleCanvasItem *child = children.at(i);
        if (child->options() & IsFocusPanel)
            continue;

        if (child->isFocusable())
            return child;

        QSimpleCanvasItem *testFocus = child->findFirstFocusChild();
        if (testFocus)
            return testFocus;
    }

    return 0;
}

QSimpleCanvasItem *QSimpleCanvasItem::findLastFocusChild() const
{
    Q_D(const QSimpleCanvasItem);

    const QList<QSimpleCanvasItem *> &children = d->children;

    for (int i = children.count()-1; i >= 0; --i) {
        QSimpleCanvasItem *child = children.at(i);
        if (child->options() & IsFocusPanel)
            continue;

        if (child->isFocusable())
            return child;
        QSimpleCanvasItem *testFocus = child->findLastFocusChild();
        if (testFocus)
            return testFocus;
    }

    return 0;
}

QSimpleCanvasItem *QSimpleCanvasItem::findPrevFocus(QSimpleCanvasItem *item)
{
    QSimpleCanvasItem *focusChild = item->findLastFocusChild();
    if (focusChild)
        return focusChild;

    if (item->options() & IsFocusPanel) {
        if (item->isFocusable())
            return item;
        else
            return 0;
    }

    QSimpleCanvasItem *parent = item->parent();
    while (parent) {
        const QList<QSimpleCanvasItem *> &children = parent->d_func()->children;

        int idx = children.indexOf(item);
        QSimpleCanvasItem *testFocus = 0;
        if (idx > 0) {
            while (--idx >= 0) {
                testFocus = children.at(idx);
                if (testFocus->options() & IsFocusPanel)
                    continue;
                if (testFocus->isFocusable())
                    return testFocus;
                testFocus = testFocus->findLastFocusChild();
                if (testFocus)
                    return testFocus;
            }
        }
        if (parent->options() & IsFocusPanel) {
            if (parent->isFocusable())
                return parent;
            else
                return 0;
        }
        item = parent;
        parent = parent->parent();
    }

    return 0;
}

QSimpleCanvasItem *QSimpleCanvasItem::findNextFocus(QSimpleCanvasItem *item)
{
    QSimpleCanvasItem *focusChild = item->findFirstFocusChild();
    if (focusChild)
        return focusChild;

    if (item->options() & IsFocusPanel) {
        if (item->isFocusable())
            return item;
        else
            return 0;
    }

    QSimpleCanvasItem *parent = item->parent();
    while (parent) {
        const QList<QSimpleCanvasItem *> &children = parent->d_func()->children;

        int idx = children.indexOf(item);
        QSimpleCanvasItem *testFocus = 0;
        if (idx >= 0) {
            while (++idx < children.count()) {
                testFocus = children.at(idx);
                if (testFocus->options() & IsFocusPanel)
                    continue;
                if (testFocus->isFocusable())
                    return testFocus;
                testFocus = testFocus->findFirstFocusChild();
                if (testFocus)
                    return testFocus;
            }
        }
        if (parent->options() & IsFocusPanel) {
            if (parent->isFocusable())
                return parent;
            else
                return 0;
        }
        item = parent;
        parent = parent->parent();
    }

    return 0;
}

QPixmap QSimpleCanvasItem::string(const QString &str, const QColor &c, const QFont &f)
{
    QFontMetrics fm(f);
    QSize size(fm.width(str), fm.height()*(str.count(QLatin1Char('\n'))+1)); //fm.boundingRect(str).size();
    QPixmap img(size);
    img.fill(Qt::transparent);
    QPainter p(&img);
    p.setPen(c);
    p.setFont(f);
    p.drawText(img.rect(), Qt::AlignVCenter, str);
    return img;
}

void QSimpleCanvasItem::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    paintContents(*p);
}

void QSimpleCanvasItem::geometryChanged(const QRectF &, const QRectF &)
{
}

void QSimpleCanvasItemPrivate::gvRemoveMouseFilter()
{
    Q_Q(QSimpleCanvasItem);
    QGraphicsScene *scene = q->scene();
    if (!scene) return;

    scene->removeEventFilter(q);
}

void QSimpleCanvasItemPrivate::gvAddMouseFilter()
{
    Q_Q(QSimpleCanvasItem);
    QGraphicsScene *scene = q->scene();
    if (!scene) return;

    scene->installEventFilter(q);
}

QT_END_NAMESPACE
