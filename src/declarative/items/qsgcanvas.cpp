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

#include "qsgcanvas.h"
#include "qsgcanvas_p.h"

#include "qsgitem.h"
#include "qsgitem_p.h"

#include "renderer.h"

#include <QtGui/qpainter.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qvarlengtharray.h>

#include <private/qdeclarativedebugtrace_p.h>

// XXX todo 
#include "qsgitemsmodule_p.h"

QT_BEGIN_NAMESPACE

void qt_scenegraph_register_types();

/*
Focus behavior
==============

Prior to being added to a valid canvas items can set and clear focus with no 
effect.  Only once items are added to a canvas (by way of having a parent set that
already belongs to a canvas) do the focus rules apply.  Focus goes back to 
having no effect if an item is removed from a canvas.

When an item is moved into a new focus scope (either being added to a canvas
for the first time, or having its parent changed), if the focus scope already has 
a scope focused item that takes precedence over the item being added.  Otherwise,
the focus of the added tree is used.  In the case of of a tree of items being 
added to a canvas for the first time, which may have a conflicted focus state (two
or more items in one scope having focus set), the same rule is applied item by item - 
thus the first item that has focus will get it (assuming the scope doesn't already 
have a scope focused item), and the other items will have their focus cleared.
*/

// #define FOCUS_DEBUG
// #define MOUSE_DEBUG
// #define TOUCH_DEBUG
// #define DIRTY_DEBUG

QSGItem::UpdatePaintNodeData::UpdatePaintNodeData()
: opacity(1)
{
}

// ### Multisample should always be on and nonblocking swap should always be off
// ### in the final version...
static QGLFormat getFormat()
{
    QGLFormat format;
    format.setSwapInterval(1);

    const QStringList args = qApp->arguments();
    if (!args.contains("--no-multisample")) {
        format.setSampleBuffers(true);
    } else if (args.contains("--nonblocking-swap")) {
        format.setSwapInterval(0);
    }

    return format;
}

QSGRootItem::QSGRootItem()
{
}

QSGCanvasPrivate::QSGCanvasPrivate()
    : q_ptr(0)
    , rootItem(0)
    , activeFocusItem(0)
    , mouseGrabberItem(0)
    , context(0)
    , animationDriver(0)
    , dirtyItemList(0)
{
}

QSGCanvasPrivate::~QSGCanvasPrivate()
{
}

void QSGCanvasPrivate::init(QSGCanvas *c)
{
    q_ptr = c;

    Q_Q(QSGCanvas);

    // XXX todo 
    QSGItemsModule::defineModule();

    q->setAttribute(Qt::WA_AcceptTouchEvents);

    rootItem = new QSGRootItem;
    QSGItemPrivate *rootItemPrivate = QSGItemPrivate::get(rootItem);
    rootItemPrivate->canvas = q;
    rootItemPrivate->flags |= QSGItem::ItemIsFocusScope;
    rootItemPrivate->focus = true;
    rootItemPrivate->activeFocus = true;
    activeFocusItem = rootItem;
}

void QSGCanvasPrivate::sceneMouseEventForTransform(QGraphicsSceneMouseEvent &sceneEvent,
                                                   const QTransform &transform)
{
    sceneEvent.setPos(transform.map(sceneEvent.scenePos()));
    sceneEvent.setLastPos(transform.map(sceneEvent.lastScenePos()));
    for (int ii = 0; ii < 5; ++ii) {
        if (sceneEvent.buttons() & (1 << ii)) {
            sceneEvent.setButtonDownPos((Qt::MouseButton)(1 << ii), 
                                        transform.map(sceneEvent.buttonDownScenePos((Qt::MouseButton)(1 << ii))));
        }
    }
}

QEvent::Type QSGCanvasPrivate::sceneMouseEventTypeFromMouseEvent(QMouseEvent *event)
{
    switch(event->type()) {
    default:
        Q_ASSERT(!"Unknown event type");
    case QEvent::MouseButtonPress:
        return QEvent::GraphicsSceneMousePress;
    case QEvent::MouseButtonRelease:
        return QEvent::GraphicsSceneMouseRelease;
    case QEvent::MouseButtonDblClick:
        return QEvent::GraphicsSceneMouseDoubleClick;
    case QEvent::MouseMove:
        return QEvent::GraphicsSceneMouseMove;
    }
}

/*!
Fill in the data in \a sceneEvent based on \a event.  This method leaves the item local positions in
\a sceneEvent untouched.  Use sceneMouseEventForTransform() to fill in those details.
*/
void QSGCanvasPrivate::sceneMouseEventFromMouseEvent(QGraphicsSceneMouseEvent &sceneEvent, QMouseEvent *event)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(event);

    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) {
        if ((event->button() & event->buttons()) == event->buttons()) {
            lastMousePosition = event->pos();
        }

        switch (event->button()) {
        default:
            Q_ASSERT(!"Unknown button");
        case Qt::LeftButton:
            buttonDownPositions[0] = event->pos();
            break;
        case Qt::RightButton:
            buttonDownPositions[1] = event->pos();
            break;
        case Qt::MiddleButton:
            buttonDownPositions[2] = event->pos();
            break;
        case Qt::XButton1:
            buttonDownPositions[3] = event->pos();
            break;
        case Qt::XButton2:
            buttonDownPositions[4] = event->pos();
            break;
        }
    }

    sceneEvent.setScenePos(event->pos());
    sceneEvent.setScreenPos(event->globalPos());
    sceneEvent.setLastScenePos(lastMousePosition);
    sceneEvent.setLastScreenPos(q->mapToGlobal(lastMousePosition));
    sceneEvent.setButtons(event->buttons());
    sceneEvent.setButton(event->button());
    sceneEvent.setModifiers(event->modifiers());

    for (int ii = 0; ii < 5; ++ii) {
        if (sceneEvent.buttons() & (1 << ii)) {
            sceneEvent.setButtonDownScenePos((Qt::MouseButton)(1 << ii), buttonDownPositions[ii]);
            sceneEvent.setButtonDownScreenPos((Qt::MouseButton)(1 << ii), q->mapToGlobal(buttonDownPositions[ii]));
        }
    }

    lastMousePosition = event->pos();
}

void QSGCanvasPrivate::setFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions options)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(item);
    Q_ASSERT(scope);

#ifdef FOCUS_DEBUG
    qWarning() << "QSGCanvasPrivate::setFocusInScope():";
    qWarning() << "    scope:" << (QObject *)scope;
    qWarning() << "    scopeSubFocusItem:" << (QObject *)QSGItemPrivate::get(scope)->subFocusItem;
    qWarning() << "    item:" << (QObject *)item;
    qWarning() << "    activeFocusItem:" << (QObject *)activeFocusItem;
#endif

    QSGItemPrivate *scopePrivate = QSGItemPrivate::get(scope);
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);

    QSGItem *oldActiveFocusItem = 0;
    QSGItem *newActiveFocusItem = 0;

    QVarLengthArray<QSGItem *, 20> changed;

    // Does this change the active focus?
    if (scopePrivate->activeFocus) {
        oldActiveFocusItem = activeFocusItem;
        newActiveFocusItem = item;
        while (newActiveFocusItem->isFocusScope() && newActiveFocusItem->scopedFocusItem())
            newActiveFocusItem = newActiveFocusItem->scopedFocusItem();

        Q_ASSERT(oldActiveFocusItem);

        activeFocusItem = 0;
        QFocusEvent event(QEvent::FocusOut, Qt::OtherFocusReason);
        q->sendEvent(oldActiveFocusItem, &event);

        QSGItem *afi = oldActiveFocusItem;
        while (afi != scope) {
            if (QSGItemPrivate::get(afi)->activeFocus) {
                QSGItemPrivate::get(afi)->activeFocus = false;
                changed << afi;
            }
            afi = afi->parentItem();
        }
    }

    QSGItem *oldSubFocusItem = scopePrivate->subFocusItem;
    // Correct focus chain in scope
    if (oldSubFocusItem) {
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = 0;
            sfi = sfi->parentItem();
        }
    }
    {
        scopePrivate->subFocusItem = item;
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = item;
            sfi = sfi->parentItem();
        }
    }

    if (oldSubFocusItem) {
        QSGItemPrivate::get(oldSubFocusItem)->focus = false;
        changed << oldSubFocusItem;
    }

    if (!(options & DontChangeFocusProperty)) {
        itemPrivate->focus = true;
        changed << item;
    }

    if (newActiveFocusItem) {
        activeFocusItem = newActiveFocusItem;

        QSGItemPrivate::get(newActiveFocusItem)->activeFocus = true;
        changed << newActiveFocusItem;

        QSGItem *afi = newActiveFocusItem->parentItem();
        while (afi != scope) {
            if (afi->isFocusScope()) {
                QSGItemPrivate::get(afi)->activeFocus = true;
                changed << afi;
            }
            afi = afi->parentItem();
        }

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    }

    if (!changed.isEmpty()) 
        notifyFocusChangesRecur(changed.data(), changed.count() - 1);
}

void QSGCanvasPrivate::clearFocusInScope(QSGItem *scope, QSGItem *item, FocusOptions options)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(item);
    Q_ASSERT(scope);

#ifdef FOCUS_DEBUG
    qWarning() << "QSGCanvasPrivate::clearFocusInScope():";
    qWarning() << "    scope:" << (QObject *)scope;
    qWarning() << "    item:" << (QObject *)item;
    qWarning() << "    activeFocusItem:" << (QObject *)activeFocusItem;
#endif

    QSGItemPrivate *scopePrivate = QSGItemPrivate::get(scope);

    QSGItem *oldActiveFocusItem = 0;
    QSGItem *newActiveFocusItem = 0;

    QVarLengthArray<QSGItem *, 20> changed;

    Q_ASSERT(item == scopePrivate->subFocusItem);

    // Does this change the active focus?
    if (scopePrivate->activeFocus) {
        oldActiveFocusItem = activeFocusItem;
        newActiveFocusItem = scope;
        
        Q_ASSERT(oldActiveFocusItem);

        activeFocusItem = 0;
        QFocusEvent event(QEvent::FocusOut, Qt::OtherFocusReason);
        q->sendEvent(oldActiveFocusItem, &event);

        QSGItem *afi = oldActiveFocusItem;
        while (afi != scope) {
            if (QSGItemPrivate::get(afi)->activeFocus) {
                QSGItemPrivate::get(afi)->activeFocus = false;
                changed << afi;
            }
            afi = afi->parentItem();
        }
    }

    QSGItem *oldSubFocusItem = scopePrivate->subFocusItem;
    // Correct focus chain in scope
    if (oldSubFocusItem) {
        QSGItem *sfi = scopePrivate->subFocusItem->parentItem();
        while (sfi != scope) {
            QSGItemPrivate::get(sfi)->subFocusItem = 0;
            sfi = sfi->parentItem();
        }
    }
    scopePrivate->subFocusItem = 0;

    if (oldSubFocusItem && !(options & DontChangeFocusProperty)) {
        QSGItemPrivate::get(oldSubFocusItem)->focus = false;
        changed << oldSubFocusItem;
    }

    if (newActiveFocusItem) {
        Q_ASSERT(newActiveFocusItem == scope);
        activeFocusItem = scope;

        QFocusEvent event(QEvent::FocusIn, Qt::OtherFocusReason);
        q->sendEvent(newActiveFocusItem, &event); 
    }

    if (!changed.isEmpty()) 
        notifyFocusChangesRecur(changed.data(), changed.count() - 1);
}

void QSGCanvasPrivate::notifyFocusChangesRecur(QSGItem **items, int remaining)
{
    QDeclarativeGuard<QSGItem> item(*items);

    if (remaining)
        notifyFocusChangesRecur(items + 1, remaining - 1);

    if (item) {
        QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);

        if (itemPrivate->notifiedFocus != itemPrivate->focus) {
            itemPrivate->notifiedFocus = itemPrivate->focus;
            emit item->focusChanged(itemPrivate->focus);
        }

        if (item && itemPrivate->notifiedActiveFocus != itemPrivate->activeFocus) {
            itemPrivate->notifiedActiveFocus = itemPrivate->activeFocus;
            item->itemChange(QSGItem::ItemActiveFocusHasChanged, qVariantFromValue(itemPrivate->activeFocus));
            emit item->activeFocusChanged(itemPrivate->activeFocus);
        }
    } 
}

void QSGCanvasPrivate::dirtyItem(QSGItem *)
{
    Q_Q(QSGCanvas);

    q->maybeUpdate();
}

void QSGCanvasPrivate::cleanup(Node *n)
{
    Q_Q(QSGCanvas);

    Q_ASSERT(!cleanupNodeList.contains(n));
    cleanupNodeList.append(n);
    q->maybeUpdate();
}

QSGCanvas::QSGCanvas(QWidget *parent, Qt::WindowFlags f)
: QGLWidget(getFormat(), parent /*, 0, f */), d_ptr(new QSGCanvasPrivate)
{
    Q_D(QSGCanvas);

    if (f) QWidget::setWindowFlags(f);
    d->init(this);
}

QSGCanvas::QSGCanvas(QSGCanvasPrivate &dd, QWidget *parent, Qt::WindowFlags f)
: QGLWidget(getFormat(), parent /*, 0, f */), d_ptr(&dd)
{
    Q_D(QSGCanvas);

    if (f) QWidget::setWindowFlags(f);
    d->init(this);
}

QSGCanvas::~QSGCanvas()
{
    Q_D(QSGCanvas);

    delete d->rootItem; d->rootItem = 0;
    d->cleanupNodes();
    delete d_ptr; d_ptr = 0;
}

QSGContext *QSGCanvas::sceneGraphContext() const
{
    Q_D(const QSGCanvas);
    return d->context;
}

void QSGCanvas::setSceneGraphContext(QSGContext *context)
{
    Q_D(QSGCanvas);
    if (d->context) {
        qWarning("QSGCanvas::setSceneGraphContext: context already exists...");
        return;
    }

    d->context = context;
}

QSGItem *QSGCanvas::rootItem() const
{
    Q_D(const QSGCanvas);
    
    return d->rootItem;
}

QSGItem *QSGCanvas::activeFocusItem() const
{
    Q_D(const QSGCanvas);
    
    return d->activeFocusItem;
}

QSGItem *QSGCanvas::mouseGrabberItem() const
{
    Q_D(const QSGCanvas);
    
    return d->mouseGrabberItem;
}

bool QSGCanvas::event(QEvent *e)
{
    // XXX todo
    switch (e->type()) {
    case QEvent::TouchBegin:
#ifdef TOUCH_DEBUG
        qWarning("touchBeginEvent");
#endif
        return true;
    case QEvent::TouchUpdate:
#ifdef TOUCH_DEBUG
        qWarning("touchUpdateEvent");
#endif
        return true;
    case QEvent::TouchEnd:
#ifdef TOUCH_DEBUG
        qWarning("touchEndEvent");
#endif
        return true;
    default:
        return QGLWidget::event(e);
    }
}

void QSGCanvas::keyPressEvent(QKeyEvent *e)
{
    Q_D(QSGCanvas);
    
    sendEvent(d->activeFocusItem, e);
}

void QSGCanvas::keyReleaseEvent(QKeyEvent *e)
{
    Q_D(QSGCanvas);
    
    sendEvent(d->activeFocusItem, e);
}

void QSGCanvas::inputMethodEvent(QInputMethodEvent *e)
{
    Q_D(QSGCanvas);

    sendEvent(d->activeFocusItem, e);
}

bool QSGCanvasPrivate::deliverInitialMousePressEvent(QSGItem *item, QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGCanvas);

    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
    if (itemPrivate->opacity == 0.0)
        return false;

    if (itemPrivate->flags & QSGItem::ItemClipsChildrenToShape) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (!QRectF(0, 0, item->width(), item->height()).contains(p))
            return false;
    }

    QList<QSGItem *> children = itemPrivate->paintOrderChildItems();
    for (int ii = children.count() - 1; ii >= 0; --ii) {
        QSGItem *child = children.at(ii);
        if (!child->isEnabled())
            continue;
        if (deliverInitialMousePressEvent(child, event))
            return true;
    }

    if (itemPrivate->acceptedMouseButtons & event->button()) {
        QPointF p = item->mapFromScene(event->scenePos());
        if (QRectF(0, 0, item->width(), item->height()).contains(p)) {
            sceneMouseEventForTransform(*event, itemPrivate->canvasToItemTransform());
            event->accept();
            mouseGrabberItem = item;
            q->sendEvent(item, event);
            if (event->isAccepted()) 
                return true;
            mouseGrabberItem = 0;
        }
    }

    return false;
}

bool QSGCanvasPrivate::deliverMouseEvent(QGraphicsSceneMouseEvent *sceneEvent)
{
    Q_Q(QSGCanvas);

    if (!mouseGrabberItem && 
         sceneEvent->type() == QEvent::GraphicsSceneMousePress &&
         (sceneEvent->button() & sceneEvent->buttons()) == sceneEvent->buttons()) {
        
        return deliverInitialMousePressEvent(rootItem, sceneEvent);
    }

    if (mouseGrabberItem) {
        QSGItemPrivate *mgPrivate = QSGItemPrivate::get(mouseGrabberItem);
        sceneMouseEventForTransform(*sceneEvent, mgPrivate->canvasToItemTransform());

        sceneEvent->accept();
        q->sendEvent(mouseGrabberItem, sceneEvent);
        if (sceneEvent->isAccepted())
            return true;
    }

    return false;
}

void QSGCanvas::mousePressEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mousePressEvent()" << event->pos() << event->button() << event->buttons();
#endif

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

void QSGCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseReleaseEvent()" << event->pos() << event->button() << event->buttons();
#endif

    if (!d->mouseGrabberItem) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());

    d->mouseGrabberItem = 0;
}

void QSGCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseDoubleClickEvent()" << event->pos() << event->button() << event->buttons();
#endif

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    if (!d->mouseGrabberItem && (event->button() & event->buttons()) == event->buttons()) {
        if (d->deliverInitialMousePressEvent(d->rootItem, &sceneEvent))
            event->accept();
        else
            event->ignore();
        return;
    } 

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

void QSGCanvas::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(QSGCanvas);
    
#ifdef MOUSE_DEBUG
    qWarning() << "QSGCanvas::mouseMoveEvent()" << event->pos() << event->button() << event->buttons();
#endif

    QGraphicsSceneMouseEvent sceneEvent(d->sceneMouseEventTypeFromMouseEvent(event));
    d->sceneMouseEventFromMouseEvent(sceneEvent, event);

    d->deliverMouseEvent(&sceneEvent);
    event->setAccepted(sceneEvent.isAccepted());
}

void QSGCanvas::wheelEvent(QWheelEvent *)
{
    // XXX todo
#ifdef MOUSE_DEBUG
    qWarning("wheelEvent");
#endif
}

bool QSGCanvasPrivate::sendFilteredMouseEvent(QSGItem *target, QSGItem *item, QGraphicsSceneMouseEvent *event)
{
    if (!target)
        return false;

    if (sendFilteredMouseEvent(target->parentItem(), item, event))
        return true;

    QSGItemPrivate *targetPrivate = QSGItemPrivate::get(target);
    if (targetPrivate->filtersChildMouseEvents) 
        if (target->childMouseEventFilter(item, event))
            return true;

    return false;
}

bool QSGCanvas::sendEvent(QSGItem *item, QEvent *e) 
{ 
    Q_D(QSGCanvas);
    
    if (!item) {
        qWarning("QSGCanvas::sendEvent: Cannot send event to a null item");
        return false;
    }

    Q_ASSERT(e);

    switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
        QSGItemPrivate::get(item)->deliverKeyEvent(static_cast<QKeyEvent *>(e));
        while (!e->isAccepted() && (item = item->parentItem())) {
            e->accept();
            QSGItemPrivate::get(item)->deliverKeyEvent(static_cast<QKeyEvent *>(e));
        }
        break;
    case QEvent::InputMethod:
        QSGItemPrivate::get(item)->deliverInputMethodEvent(static_cast<QInputMethodEvent *>(e));
        while (!e->isAccepted() && (item = item->parentItem())) {
            e->accept();
            QSGItemPrivate::get(item)->deliverInputMethodEvent(static_cast<QInputMethodEvent *>(e));
        }
        break;
    case QEvent::FocusIn:
    case QEvent::FocusOut:
        QSGItemPrivate::get(item)->deliverFocusEvent(static_cast<QFocusEvent *>(e));
        break;
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
    case QEvent::GraphicsSceneMouseMove:
        // XXX todo - should sendEvent be doing this?  how does it relate to forwarded events? 
        {
            QGraphicsSceneMouseEvent *se = static_cast<QGraphicsSceneMouseEvent *>(e);
            if (!d->sendFilteredMouseEvent(item->parentItem(), item, se)) {
                se->accept();
                QSGItemPrivate::get(item)->deliverMouseEvent(se);
            }
        }
        break;
    default:
        break;
    }

    return false; 
}

void QSGCanvas::showEvent(QShowEvent *e)
{
    Q_D(QSGCanvas);
    
    QGLWidget::showEvent(e);

    d->initializeSceneGraph();
}

bool QSGCanvasPrivate::updateEffectiveOpacity(QSGItem *item)
{
    QSGItemPrivate *priv = QSGItemPrivate::get(item);
    if (priv->parentItem && updateEffectiveOpacity(priv->parentItem))
        return true;

    if (priv->dirtyAttributes & QSGItemPrivate::OpacityValue) {
        updateEffectiveOpacityRoot(item, priv->parentItem?QSGItemPrivate::get(priv->parentItem)->effectiveOpacity:1);
        return true;
    } else {
        return false;
    }
}

void QSGCanvasPrivate::updateEffectiveOpacityRoot(QSGItem *item, qreal parentOpacity)
{
    QSGItemPrivate *priv = QSGItemPrivate::get(item);
    priv->dirtyAttributes &= ~QSGItemPrivate::OpacityValue;

    qreal v = parentOpacity * priv->opacity;
    if (qFuzzyCompare(v, priv->effectiveOpacity))
        return;

    priv->effectiveOpacity = v;
    priv->dirtyAttributes |= QSGItemPrivate::EffectiveOpacity;

    if (v != 0.)
        priv->addToDirtyList();

    for (int ii = 0; ii < priv->childItems.count(); ++ii)
        updateEffectiveOpacityRoot(priv->childItems.at(ii), v);
}

void QSGCanvasPrivate::cleanupNodes()
{
    for (int ii = 0; ii < cleanupNodeList.count(); ++ii)
        delete cleanupNodeList.at(ii);
    cleanupNodeList.clear();
}

void QSGCanvasPrivate::updateDirtyNodes()
{
#ifdef DIRTY_DEBUG
    qWarning() << "QSGCanvasPrivate::updateDirtyNodes():";
#endif

    cleanupNodes();

    QSGItem *iter = dirtyItemList;
    while (iter) {
        QSGItemPrivate *priv = QSGItemPrivate::get(iter);

        if (priv->dirtyAttributes & QSGItemPrivate::ParentChanged) {
            priv->dirtyAttributes |= QSGItemPrivate::OpacityValue;
        } else if (priv->dirtyAttributes & QSGItemPrivate::OpacityValue && 
                   QSGItemPrivate::get(priv->parentItem)->effectiveOpacity == 0.) {
            priv->dirtyAttributes &= ~QSGItemPrivate::OpacityValue;
        }

        iter = priv->nextDirtyItem;
    }

    iter = dirtyItemList;
    while (iter) {
        QSGItemPrivate *priv = QSGItemPrivate::get(iter);
        if (priv->dirtyAttributes & QSGItemPrivate::OpacityValue) 
            updateEffectiveOpacity(iter);
        iter = priv->nextDirtyItem;
    }

    QSGItem *updateList = dirtyItemList;
    dirtyItemList = 0;
    if (updateList) QSGItemPrivate::get(updateList)->prevDirtyItem = &updateList;

    while (updateList) {
        QSGItem *item = updateList;
        QSGItemPrivate *itemPriv = QSGItemPrivate::get(item);
        itemPriv->removeFromDirtyList();

#ifdef DIRTY_DEBUG
        qWarning() << "   Node:" << item << qPrintable(itemPriv->dirtyToString());
#endif
        updateDirtyNode(item);
    }
}

void QSGCanvasPrivate::updateDirtyNode(QSGItem *item)
{
    QSGItemPrivate *itemPriv = QSGItemPrivate::get(item);
    quint32 dirty = itemPriv->dirtyAttributes;
    itemPriv->dirtyAttributes = 0;

    if ((dirty & QSGItemPrivate::TransformUpdateMask) ||
        (dirty & QSGItemPrivate::Size && itemPriv->origin != QSGItem::TopLeft && 
         (itemPriv->scale != 1. || itemPriv->rotation != 0.))) {

        QMatrix4x4 matrix;

        if (!itemPriv->effectRefCount) {
            if (itemPriv->x != 0. || itemPriv->y != 0.) 
                matrix.translate(itemPriv->x, itemPriv->y);

            if (itemPriv->scale != 1. || itemPriv->rotation != 0.) {
                QPointF origin = itemPriv->computeTransformOrigin();
                matrix.translate(origin.x(), origin.y());
                if (itemPriv->scale != 1.)
                    matrix.scale(itemPriv->scale, itemPriv->scale);
                if (itemPriv->rotation != 0.)
                    matrix.rotate(itemPriv->rotation, 0, 0, 1);
                matrix.translate(-origin.x(), -origin.y());
            }
        }

        if (dirty & QSGItemPrivate::ComplexTransformUpdateMask) {
            for (int ii = 0; ii < itemPriv->transforms.count(); ++ii)
                itemPriv->transforms.at(ii)->applyTo(&matrix);
        }

        itemPriv->itemNode()->setMatrix(matrix);
    }

    bool clipEffectivelyChanged = dirty & QSGItemPrivate::Clip &&
                                  ((item->clip() == false) != (itemPriv->clipNode == 0));

    if (dirty & QSGItemPrivate::ChildrenUpdateMask || clipEffectivelyChanged) {

        while (itemPriv->childContainerNode()->childCount()) 
            itemPriv->childContainerNode()->removeChildNode(itemPriv->childContainerNode()->childAtIndex(0));

        if (clipEffectivelyChanged) {
            if (item->clip()) {
                Q_ASSERT(itemPriv->clipNode == 0);
                itemPriv->clipNode = new QSGClipNode(QRectF(0, 0, itemPriv->width, itemPriv->height));
                itemPriv->itemNode()->appendChildNode(itemPriv->clipNode);
            } else {
                Q_ASSERT(itemPriv->clipNode != 0);
                delete itemPriv->clipNode;
                itemPriv->clipNode = 0;
            }
        }

        QList<QSGItem *> orderedChildren = itemPriv->paintOrderChildItems();
        int ii = 0;

        itemPriv->paintNodeIndex = 0;
        for (; ii < orderedChildren.count() && orderedChildren.at(ii)->z() < 0; ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            if (childPrivate->effectRefCount == 0) {
                itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
                itemPriv->paintNodeIndex++;
            }
        }

        if (itemPriv->paintNode)
            itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);

        for (; ii < orderedChildren.count(); ++ii) {
            QSGItemPrivate *childPrivate = QSGItemPrivate::get(orderedChildren.at(ii));
            if (childPrivate->itemNode()->parent())
                childPrivate->itemNode()->parent()->removeChildNode(childPrivate->itemNode());

            if (childPrivate->effectRefCount == 0) {
                itemPriv->childContainerNode()->appendChildNode(childPrivate->itemNode());
            }
        }
    }

    if ((dirty & QSGItemPrivate::Size || clipEffectivelyChanged) && itemPriv->clipNode) {
        itemPriv->clipNode->setRect(QRectF(0, 0, itemPriv->width, itemPriv->height));
        itemPriv->clipNode->update();
    }

    if (dirty & QSGItemPrivate::ContentUpdateMask) {

        itemPriv->itemNode()->setSubtreeEnabled(itemPriv->effectiveOpacity > 0);

        if (itemPriv->flags & QSGItem::ItemHasContents) {
            updatePaintNodeData.opacity = itemPriv->effectiveOpacity; 
            itemPriv->paintNode = item->updatePaintNode(itemPriv->paintNode, &updatePaintNodeData);

            Q_ASSERT(itemPriv->paintNode == 0 || 
                     itemPriv->paintNode->parent() == 0 ||
                     itemPriv->paintNode->parent() == itemPriv->childContainerNode());

            if (itemPriv->paintNode && itemPriv->paintNode->parent() == 0) {
                if (itemPriv->childContainerNode()->childCount() == 0)
                    itemPriv->childContainerNode()->appendChildNode(itemPriv->paintNode);
                else 
                    itemPriv->childContainerNode()->insertChildNodeBefore(itemPriv->paintNode, itemPriv->childContainerNode()->childAtIndex(itemPriv->paintNodeIndex));
            }
        } else if (itemPriv->paintNode) {
            delete itemPriv->paintNode;
        }
    } 
}

void QSGCanvas::paintEvent(QPaintEvent *)
{
    Q_D(QSGCanvas);
    
    Q_ASSERT(d->context);

    // XXX todo - this should probably be done elsewhere
    while (!d->polishItems.isEmpty()) {
        QSet<QSGItem *>::Iterator iter = d->polishItems.begin();
        QSGItem *item = *iter;
        d->polishItems.erase(iter);
        QSGItemPrivate::get(item)->polishScheduled = false;
        item->updatePolish();
    }

    d->updateDirtyNodes();

    QGLContext *glctx = const_cast<QGLContext *>(context());
    glctx->makeCurrent();

    QDeclarativeDebugTrace::addEvent(QDeclarativeDebugTrace::FramePaint);
    QDeclarativeDebugTrace::startRange(QDeclarativeDebugTrace::Painting);

    d->context->renderer()->setDeviceRect(rect());
    d->context->renderer()->setProjectMatrixToDeviceRect();

    d->context->renderNextFrame();

    glctx->swapBuffers();

    QDeclarativeDebugTrace::endRange(QDeclarativeDebugTrace::Painting);
}

void QSGCanvas::maybeUpdate()
{
    Q_D(QSGCanvas);
    
    if (!d->animationDriver || !d->animationDriver->isRunning())
        update();
}

void QSGCanvasPrivate::initializeSceneGraph()
{
    Q_Q(QSGCanvas);

    if (!context) 
        context = QSGContext::createDefaultContext();

    if (context->isReady())
        return;

    animationDriver = context->createAnimationDriver(q);
    if (animationDriver)
        animationDriver->install();

    QGLContext *glctx = const_cast<QGLContext *>(QGLContext::currentContext());
    context->initialize(glctx);

    context->renderer()->setDeviceRect(q->rect());
    context->renderer()->setProjectMatrixToDeviceRect();

    QObject::connect(context->renderer(), SIGNAL(sceneGraphChanged()), q, SLOT(maybeUpdate()));

    if (!QSGItemPrivate::get(rootItem)->itemNode()->parent()) 
        context->rootNode()->appendChildNode(QSGItemPrivate::get(rootItem)->itemNode());
}

QT_END_NAMESPACE
