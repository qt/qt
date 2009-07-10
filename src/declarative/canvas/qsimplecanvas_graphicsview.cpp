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

#include "qsimplecanvas.h"
#include "qsimplecanvas_p.h"
#include "qsimplecanvasitem.h"
#include "qsimplecanvasitem_p.h"
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>


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

void QSimpleCanvasGraphicsView::resizeEvent(QResizeEvent *e)
{
    _scene.setSceneRect(QRect(QPoint(0, 0), e->size()));
    QGraphicsView::resizeEvent(e);
}

QSimpleGraphicsItem::QSimpleGraphicsItem(QSimpleCanvasItem *canvasItem)
: /*scene(0),*/ owner(canvasItem)
{
    setAcceptedMouseButtons(Qt::NoButton);
}

QSimpleGraphicsItem::~QSimpleGraphicsItem()
{
    owner->d_func()->graphicsItem = 0;
}

void QSimpleGraphicsItem::paint(QPainter *painter, 
                              const QStyleOptionGraphicsItem *, QWidget *)
{
    owner->paintContents(*painter);
}

QRectF QSimpleGraphicsItem::boundingRect() const
{
    return owner->boundingRect();
}

void QSimpleGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
    owner->mousePressEvent(event);
}

void QSimpleGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
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
}

void QSimpleGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
    owner->mouseDoubleClickEvent(event);
}

void QSimpleGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QSimpleCanvasItem *p = owner->parent();
    while(p) {
        if (p->options() & QSimpleCanvasItem::ChildMouseFilter) {
            if (p->mouseFilter(event))
                return;
        }
        p = p->parent();
    }
    owner->mouseMoveEvent(event);
}

bool QSimpleGraphicsItem::sceneEvent(QEvent *event)
{
    bool rv = QGraphicsItem::sceneEvent(event);
    if (event->type() == QEvent::UngrabMouse)
        owner->mouseUngrabEvent();
    return rv;
}

QVariant QSimpleGraphicsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemSceneHasChanged) {
        QSimpleCanvasItemPrivate *owner_d = static_cast<QSimpleCanvasItemPrivate*>(owner->d_ptr);
        QSimpleCanvas *oldCanvas = owner_d->canvas;
        owner_d->canvas = sceneMap[scene()];
        if (owner_d->canvas){
            if (owner->hasFocus())
                owner->canvas()->d->setFocusItem(owner, Qt::OtherFocusReason);
            if (owner->options() & QSimpleCanvasItem::IsFocusPanel) {
                if (owner_d->wantsActiveFocusPanelPendingCanvas) {
                    owner_d->hasBeenActiveFocusPanel = true;
                    owner->canvas()->d->setActiveFocusPanel(owner);
                    owner_d->wantsActiveFocusPanelPendingCanvas = false;
                }
            }
        }
        if (owner->options() & QSimpleCanvasItem::MouseFilter)
            owner_d->gvRemoveMouseFilter();

        if (oldCanvas != owner_d->canvas)
            owner->canvasChanged();

        if (owner->options() & QSimpleCanvasItem::MouseFilter)
            owner_d->gvAddMouseFilter();
    } 

    return QGraphicsItem::itemChange(change, value);
}

void QSimpleGraphicsItem::keyPressEvent(QKeyEvent *event)
{
    owner->keyPressEvent(event);
    QGraphicsItem::keyPressEvent(event);
}

void QSimpleGraphicsItem::keyReleaseEvent(QKeyEvent *event)
{
    owner->keyReleaseEvent(event);
    QGraphicsItem::keyReleaseEvent(event);
}

void QSimpleGraphicsItem::focusInEvent(QFocusEvent *)
{
    if (!owner->hasFocus())
        owner->setFocus(true);
}

void QSimpleCanvasItemPrivate::gvRemoveMouseFilter()
{
    QGraphicsScene *scene = graphicsItem->scene();
    if (!scene) return;

    scene->removeEventFilter(q_ptr);
}

void QSimpleCanvasItemPrivate::gvAddMouseFilter()
{
    QGraphicsScene *scene = graphicsItem->scene();
    if (!scene) return;

    scene->installEventFilter(q_ptr);
}
QT_END_NAMESPACE
