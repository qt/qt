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
QSimpleCanvasItemData::QSimpleCanvasItemData()
: buttons(Qt::NoButton), flip(QSimpleCanvasItem::NoFlip), 
  dirty(false), transformValid(true), doNotPaint(false), 
  doNotPaintChildren(false), x(0), y(0), z(0), 
  visible(1), transformUser(0), transformVersion(0), activeOpacity(1)
{
}

QSimpleCanvasItemData::~QSimpleCanvasItemData()
{
    if (transformUser)
        delete transformUser;
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

void QSimpleCanvasItem::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *e)
{
    e->ignore();
}

void QSimpleCanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
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

void QSimpleCanvasItemPrivate::zOrderChildren()
{
    if (!needsZOrder || children.count() <= 1)
        return;

    needsZOrder = false;
    // This is a bubble sort for a reason - it is the fastest sort for a mostly
    // ordered list.  We only expect z ordering to change infrequently.
    bool swap = true;
    int c = 0;
    while(swap) {
        ++c;
        swap = false;
        QSimpleCanvasItem *item = children.first();
        qreal z = item->z();
        for (int ii = 1; ii < children.count(); ++ii) {
            QSimpleCanvasItem *i2 = children.at(ii);
            qreal z2 = i2->z();
            if (z2 < z) {
                swap = true;
                children[ii] = item;
                children[ii - 1] = i2;
            } else {
                item = i2;
                z = z2;
            }
        }
    }
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

qreal QSimpleCanvasItem::x() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return d->graphicsItem->x();
    else if (d->data_ptr)
        return d->data()->x;
    else
        return 0;
}

qreal QSimpleCanvasItem::y() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return d->graphicsItem->y();
    else if (d->data_ptr)
        return d->data()->y;
    else
        return 0;
}

qreal QSimpleCanvasItem::z() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return d->graphicsItem->zValue();
    else if (d->data_ptr)
        return d->data()->z;
    else
        return 0;
}

void QSimpleCanvasItem::setX(qreal x)
{
    Q_D(QSimpleCanvasItem);
    if (x == this->x())
        return;

    qreal oldX = this->x();

    if (d->graphicsItem) {
        d->graphicsItem->setPos(x, y());
    } else {
        d->data()->x = x;
        update();
    }

    geometryChanged(QRectF(this->x(), y(), width(), height()), 
                    QRectF(oldX, y(), width(), height()));
}

void QSimpleCanvasItem::setY(qreal y)
{
    Q_D(QSimpleCanvasItem);
    if (y == this->y())
        return;

    qreal oldY = this->y();

    if (d->graphicsItem) {
        d->graphicsItem->setPos(x(), y);
    } else {
        d->data()->y = y;
        update();
    }

    geometryChanged(QRectF(x(), this->y(), width(), height()), 
                    QRectF(x(), oldY, width(), height()));
}

void QSimpleCanvasItem::setZ(qreal z)
{
    Q_D(QSimpleCanvasItem);
    if (z == this->z())
        return;

    if (d->graphicsItem) {

        if (z < 0)
            d->graphicsItem->setFlag(QGraphicsItem::ItemStacksBehindParent, 
                                     true);
        else
            d->graphicsItem->setFlag(QGraphicsItem::ItemStacksBehindParent, 
                                     false);

        d->graphicsItem->setZValue(z);

    } else {
        if (d->data()->z == z)
            return;

        d->data()->z = z;
        if (parent())
            static_cast<QSimpleCanvasItemPrivate*>(parent()->d_ptr)->needsZOrder = true;
        update();
    }
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
    Q_D(QSimpleCanvasItem);
    qreal oldX = x();
    qreal oldY = y();

    if (d->graphicsItem) {
        d->graphicsItem->setPos(point);
    } else {
        d->data()->x = point.x();
        d->data()->y = point.y();
        update();
    }

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
    if (d->graphicsItem) {
        QTransform t;
        QPointF to = transformOriginPoint();
        if (to.x() != 0. || to.y() != 0.)
            t.translate(to.x(), to.y());
        t.scale(s, s);
        if (to.x() != 0. || to.y() != 0.)
            t.translate(-to.x(), -to.y());
        d->graphicsItem->setTransform(t * d->graphicsItem->transform);
    } else {
        update();
    }
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
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return d->graphicsItem->opacity();
    else if (d->data_ptr)
        return d->data()->visible;
    else
        return 1;
}

void QSimpleCanvasItem::setVisible(qreal v)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
        d->graphicsItem->setOpacity(v);
    } else {
        if (v == visible())
            return;
        if (v == 0)
            update();

        d->data()->visible = v;

        if (v != 0)
            update();
    }
}

void QSimpleCanvasItem::addChild(QSimpleCanvasItem *c)
{
    Q_D(QSimpleCanvasItem);
    d->children.append(c);
    if (!d->graphicsItem) 
        d->needsZOrder = true;
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

void QSimpleCanvasItem::update()
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
        d->graphicsItem->update();
    } else {
        if (!parent())
            return;

        if (d->data()->dirty || 0. == d->data()->visible) return;

        QSimpleCanvasLayer *l = layer(); 
        if (l == this && parent())
            l = parent()->layer();
        if (l) {
            l->addDirty(this);
            d->data()->dirty = true;
            d->data()->transformValid = false;
        }
    }
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
    if (d->graphicsItem)
        d->graphicsItem->setFlag(QGraphicsItem::ItemClipsChildrenToShape, bool(c));
    else
        update();
}

Qt::MouseButtons QSimpleCanvasItem::acceptedMouseButtons() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return d->graphicsItem->acceptedMouseButtons();
    else if (d->data_ptr)
        return (Qt::MouseButtons)d->data()->buttons;
    else
        return Qt::NoButton;
}

void QSimpleCanvasItem::setAcceptedMouseButtons(Qt::MouseButtons buttons)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem)
        d->graphicsItem->setAcceptedMouseButtons(buttons);
    else
        d->data()->buttons = buttons;
}


QRect QSimpleCanvasItem::itemBoundingRect()
{
    return boundingRect().toAlignedRect();
}

QPointF QSimpleCanvasItemPrivate::adjustFrom(const QPointF &p) const
{
    return p;
}

QRectF QSimpleCanvasItemPrivate::adjustFrom(const QRectF &r) const
{
    return r;
}

QPointF QSimpleCanvasItemPrivate::adjustTo(const QPointF &p) const
{
    return p;
}

QRectF QSimpleCanvasItemPrivate::adjustTo(const QRectF &r) const
{
    return r;
}

QPointF QSimpleCanvasItem::mapFromScene(const QPointF &p) const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem) {
        return d->graphicsItem->mapFromScene(p);
    } else {
        QPointF mp = d->adjustFrom(p);
        d->freshenTransforms();
        return d->data()->transformActive.inverted().map(mp);
    }
}

QRectF QSimpleCanvasItem::mapFromScene(const QRectF &r) const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem) {
        return d->graphicsItem->mapFromScene(r).boundingRect();
    } else {
        QRectF mr = d->adjustFrom(r);
        d->freshenTransforms();
        return d->data()->transformActive.inverted().mapRect(mr);
    }
}

QPointF QSimpleCanvasItem::mapToScene(const QPointF &p) const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem) {
        return d->graphicsItem->mapToScene(p);
    } else {
        d->freshenTransforms();
        QPointF rp = d->data()->transformActive.map(p);
        return d->adjustTo(rp);
    }
}

QRectF QSimpleCanvasItem::mapToScene(const QRectF &r) const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem) {
        return d->graphicsItem->mapToScene(r).boundingRect();
    } else {
        d->freshenTransforms();
        QRectF rr = d->data()->transformActive.mapRect(r);
        return d->adjustTo(rr);
    }
}

void QSimpleCanvasItemPrivate::freshenTransforms() const
{
    if (freshenNeeded()) 
        doFreshenTransforms();
}

bool QSimpleCanvasItemPrivate::freshenNeeded() const
{
#if 0
    return parent && 
        (data()->transformVersion == -1 ||
         data()->parentTransformVersion == -1 ||
         parent->d_func()->data()->transformVersion != data()->parentTransformVersion);
#else
    const QSimpleCanvasItemPrivate *me = this;
    while(me) {
        if (me->data_ptr && !me->data_ptr->transformValid)
            return true;
        if (me->parent)
            me = me->parent->d_func();
        else
            me = 0;
    }
    return false;
#endif
}

void QSimpleCanvasItemPrivate::doFreshenTransforms() const
{
    Q_Q(const QSimpleCanvasItem);
    if (parent)
        parent->d_func()->doFreshenTransforms();

    if (freshenNeeded()) {
        if (parent)
            data()->transformActive = parent->d_func()->data()->transformActive;
        else
            data()->transformActive = QSimpleCanvas::Matrix();
        data()->transformActive.translate(q->x(), q->y());
        if (scale != 1.) {
            QPointF to = transformOrigin();
            if (to.x() != 0. || to.y() != 0.)
                data()->transformActive.translate(to.x(), to.y());
            data()->transformActive.scale(scale, scale);
            if (to.x() != 0. || to.y() != 0.)
                data()->transformActive.translate(-to.x(), -to.y());
        }

        Q_Q(const QSimpleCanvasItem);

        if (data()->flip) {
            QRectF br = q->boundingRect();
            data()->transformActive.translate(br.width() / 2., br.height() / 2);
            data()->transformActive.scale((data()->flip & QSimpleCanvasItem::HorizontalFlip)?-1:1,
                                       (data()->flip & QSimpleCanvasItem::VerticalFlip)?-1:1);
            data()->transformActive.translate(-br.width() / 2., -br.height() / 2);
        }
    }
}

QSimpleCanvas::Matrix QSimpleCanvasItem::transform() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return QSimpleCanvasConfig::transformToMatrix(d->graphicsItem->transform);
    else if (d->data()->transformUser)
        return *d->data()->transformUser;
    else
        return QSimpleCanvas::Matrix();
}

void QSimpleCanvasItem::setTransform(const QSimpleCanvas::Matrix &m)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
        d->graphicsItem->transform = QSimpleCanvasConfig::matrixToTransform(m);
        d->graphicsItem->setTransform(QTransform::fromScale(d->scale, d->scale) * d->graphicsItem->transform);
    } else {
        if (!d->data()->transformUser)
            d->data()->transformUser = new QSimpleCanvas::Matrix;
        *d->data()->transformUser = m;
        update();
    }
}

QSimpleCanvasItem *QSimpleCanvasItem::mouseGrabberItem() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem) {
        QGraphicsScene *s = d->graphicsItem->scene();
        if (s) {
            QGraphicsItem *item = s->mouseGrabberItem();
            QSimpleGraphicsItem *dgi = static_cast<QSimpleGraphicsItem *>(item);
            return dgi?static_cast<QSimpleCanvasItem*>(dgi->owner):0;
        }
    } else {
        QSimpleCanvas *c = canvas();
        if (c)
            return c->d->lastMouseItem;
    }
    return 0;
}

void QSimpleCanvasItem::ungrabMouse()
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
        d->graphicsItem->ungrabMouse();
    } else {
        QSimpleCanvas *c = canvas();
        if (c && c->d->lastMouseItem == this) {
            c->d->lastMouseItem->mouseUngrabEvent();
            c->d->lastMouseItem = 0;
        }
    }
}

void QSimpleCanvasItem::grabMouse()
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
        d->graphicsItem->grabMouse();
    } else {
        QSimpleCanvas *c = canvas();
        if (c) {
            if (c->d->lastMouseItem != this) {
                if (c->d->lastMouseItem)
                    c->d->lastMouseItem->mouseUngrabEvent();
                c->d->lastMouseItem = this;
            }
        }
    }
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
    hasActiveFocus = f;

    if (graphicsItem) {
        if (f) {
            if (!(graphicsItem->flags() & QGraphicsItem::ItemIsFocusable))
                graphicsItem->setFlag(QGraphicsItem::ItemIsFocusable);
            graphicsItem->setFocus();
        } else {
            graphicsItem->clearFocus();
            if ((graphicsItem->flags() & QGraphicsItem::ItemIsFocusable) && !focusable)
                graphicsItem->setFlag(QGraphicsItem::ItemIsFocusable, false);
        }

    }
}

QSimpleCanvasItem::Flip QSimpleCanvasItem::flip() const
{
    Q_D(const QSimpleCanvasItem);
    if (d->graphicsItem)
        return NoFlip;
    else if (d->data_ptr)
        return d->data()->flip;
    else
        return NoFlip;
}

void QSimpleCanvasItem::setFlip(Flip f)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem)
        return;

    if (d->data()->flip == f)
        return;

    d->data()->flip = f;
    update();
}

/*!
    Places the item under \a item in the parent item's stack.

    The item itself and \a item must be siblings, or this method has no effect.

    \sa stackOver(), stackAt()
 */
void QSimpleCanvasItem::stackUnder(QSimpleCanvasItem *item)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem)
        return; // XXX

    QSimpleCanvasItem *p = parent();
    if (!p || !item || item == this) return;

    QSimpleCanvasItemPrivate *parent_d_ptr = static_cast<QSimpleCanvasItemPrivate*>(p->d_ptr);
    int idx = parent_d_ptr->children.indexOf(item);
    if (idx == -1) return;

    parent_d_ptr->children.removeAll(this);
    idx = parent_d_ptr->children.indexOf(item);
    parent_d_ptr->children.insert(idx + 1, this);
    parent_d_ptr->needsZOrder = true;

    p->childrenChanged();
}

/*!
    Places the item over \a item in the parent item's stack.

    The item itself and \a item must be siblings, or this method has no effect.

    \sa stackUnder(), stackAt()
 */
void QSimpleCanvasItem::stackOver(QSimpleCanvasItem *item)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem)
        return; // XXX

    QSimpleCanvasItem *p = parent();
    if (!p || !item || item == this) return;

    QSimpleCanvasItemPrivate *parent_d_ptr = static_cast<QSimpleCanvasItemPrivate*>(p->d_ptr);
    int idx = parent_d_ptr->children.indexOf(item);
    if (idx == -1) return;

    parent_d_ptr->children.removeAll(this);
    idx = parent_d_ptr->children.indexOf(item);
    parent_d_ptr->children.insert(idx, this);
    parent_d_ptr->needsZOrder = true;

    p->childrenChanged();
}

/*!
    Places the item at position \a index in the parent item's stack.

    If index is zero or less, the item is placed at the beginning of the 
    stack.  If the index is greater than the number of items in the stack, the
    item is placed at the end.

    \sa stackOver(), stackUnder()
 */
void QSimpleCanvasItem::stackAt(int index)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem)
        return; // XXX

    QSimpleCanvasItem *p = parent();
    if (!p) return;

    QSimpleCanvasItemPrivate *parent_d_ptr = static_cast<QSimpleCanvasItemPrivate*>(p->d_ptr);
    parent_d_ptr->children.removeAll(this);

    if (index < 0) index = 0;
    if (index > parent_d_ptr->children.size()) index = parent_d_ptr->children.size();

    parent_d_ptr->children.insert(index, this);
    parent_d_ptr->needsZOrder = true;
    p->childrenChanged();
}

/*!
    Returns the current stacking index for the child \a item. 

    If \a item is not a child, -1 is returned.

    \sa stackAt()
 */
int QSimpleCanvasItem::indexForChild(QSimpleCanvasItem *item)
{
    Q_D(QSimpleCanvasItem);
    return d->children.indexOf(item);
}

bool QSimpleCanvasItem::eventFilter(QObject *o, QEvent *e)
{
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
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

    if (d->graphicsItem)
        d->graphicsItem->setFlag(QGraphicsItem::ItemHasNoContents, !(d->options & HasContents));

    if ((old & MouseFilter) != (d->options & MouseFilter)) {
        if (d->graphicsItem) {
            if (d->options & MouseFilter)
                d->gvAddMouseFilter();
            else
                d->gvRemoveMouseFilter();

        } else {
            QSimpleCanvas *c = canvas();
            if (c) {
                if (d->options & MouseFilter)
                    c->d->installMouseFilter(this);
                else
                    c->d->removeMouseFilter(this);
            }
        }
    }
}

QSimpleCanvasItem::QSimpleCanvasItem(QSimpleCanvasItemPrivate &dd, QSimpleCanvasItem *parent)
: QObject(dd, parent)
{
}

QSimpleCanvasItem::QSimpleCanvasItem(QSimpleCanvasItem *p)
: QObject(*(new QSimpleCanvasItemPrivate), p)
{
}

QSimpleCanvasItem::~QSimpleCanvasItem()
{ 
    Q_D(QSimpleCanvasItem);
    if (d->graphicsItem) {
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
        delete d->graphicsItem;
    } else {
        update();
        setOptions(MouseFilter, false);

        if (d->canvas){
            if (d->canvas->focusItem() == this)
                d->canvas->d->focusItem = 0;
            if (d->canvas->d->lastFocusItem == this)
                d->canvas->d->lastFocusItem = 0;
            if (d->hasBeenActiveFocusPanel) 
                d->canvas->d->clearFocusPanel(this);
            if (d->hasFocus)
                d->canvas->d->clearFocusItem(this);
        }

        while(!d->children.isEmpty()) {
            QSimpleCanvasItem *child = d->children.takeFirst();
            delete child;
        }

        if (parent() && d->data_ptr && d->data()->dirty) {
            QSimpleCanvasLayer *l = parent()->layer();
            if (l) {
                l->remDirty(this);
            }
        }
        if (d->parent)
            d->parent->remChild(this);


        if (d->data_ptr)
            delete d->data_ptr;
    }
}

QSimpleCanvasItem::operator QGraphicsItem *()
{
    Q_D(QSimpleCanvasItem);
    if (!d->graphicsItem) {
        if (parent()) {
            qWarning("QSimpleCanvasItem: Only the root item can be converted into a QGraphicsItem");
            return 0;
        }
        d->convertToGraphicsItem();
    }
    return d->graphicsItem;
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
    if (graphicsItem) {
        if (oldParent)
            oldParent->remChild(q);

        parent = p;
        graphicsItem->setParentItem(p->d_func()->graphicsItem);

        if (parent)
            p->addChild(q);

    } else {
        bool canvasChange = false;
        if (p)
            canvasChange = (p->d_func()->canvas != canvas);
        QSimpleCanvas *old = canvas;

        QSimpleCanvasLayer *o = q->layer();
        if (q->parent()) {
            q->update();
            q->parent()->remChild(q);
        }
        parent = p;
        QSimpleCanvasLayer *n = 0;
        if (q->parent()) {
            q->parent()->addChild(q);
            n = q->layer();
        }

        if (o != n) {
            data()->dirty = false;
            data()->transformValid = false;
            if (o) o->remDirty(q);
            if (n) n->addDirty(q);
        }

        if (canvasChange)
            canvasChanged(p->d_func()->canvas, old);

        q->update();
    }
}

void QSimpleCanvasItemPrivate::convertToGraphicsItem(QGraphicsItem *parent)
{
    Q_Q(QSimpleCanvasItem);
    Q_ASSERT(!graphicsItem);
    graphicsItem = new QSimpleGraphicsItem(q);
    graphicsItem->setFlag(QGraphicsItem::ItemHasNoContents, !(q->options() & QSimpleCanvasItem::HasContents));
    if (parent)
        graphicsItem->setParentItem(parent);

    QSimpleCanvasItemData *old = data_ptr;
    data_ptr = 0;

    if (old) {
        q->QSimpleCanvasItem::setX(old->x);
        q->QSimpleCanvasItem::setY(old->y);
        q->QSimpleCanvasItem::setZ(old->z);
        q->QSimpleCanvasItem::setVisible(old->visible);
        if (old->transformUser)
            q->QSimpleCanvasItem::setTransform(*old->transformUser);
        q->QSimpleCanvasItem::setFlip(old->flip);
        q->QSimpleCanvasItem::setAcceptedMouseButtons((Qt::MouseButtons)old->buttons);
        delete old;
    }

    if (scale != 1) {
        qreal s = scale;
        scale = 1;
        q->QSimpleCanvasItem::setScale(s);
    }

    q->setClipType(clip);

    for (int ii = 0; ii < children.count(); ++ii) 
        static_cast<QSimpleCanvasItemPrivate*>(children.at(ii)->d_ptr)->convertToGraphicsItem(graphicsItem);
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

    if (d->graphicsItem && !static_cast<QSimpleCanvasItemPrivate*>(p->d_ptr)->graphicsItem)
        qWarning("QSimpleCanvasItem: Cannot reparent a QGraphicsView item to a QSimpleCanvas item");

    if (static_cast<QSimpleCanvasItemPrivate*>(p->d_ptr)->graphicsItem && !d->graphicsItem) {
        d->setParentInternal(0);
        d->convertToGraphicsItem();
    }

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
    qWarning().nospace() << ba.constData() << state.constData() << " " << children.count() << " " << q << " " << name.constData();

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

void QSimpleCanvasItem::geometryChanged(const QRectF &, const QRectF &)
{
}

QT_END_NAMESPACE
