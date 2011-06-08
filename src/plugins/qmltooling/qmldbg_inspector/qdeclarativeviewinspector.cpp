/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeviewinspector_p.h"
#include "qdeclarativeviewinspector_p_p.h"

#include "editor/liveselectiontool_p.h"
#include "editor/zoomtool_p.h"
#include "editor/colorpickertool_p.h"
#include "editor/livelayeritem_p.h"
#include "editor/boundingrecthighlighter_p.h"

#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeExpression>
#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QGraphicsObject>
#include <QtGui/QApplication>

QT_BEGIN_NAMESPACE

QDeclarativeViewInspectorPrivate::QDeclarativeViewInspectorPrivate(QDeclarativeViewInspector *q) :
    q(q)
{
}

QDeclarativeViewInspectorPrivate::~QDeclarativeViewInspectorPrivate()
{
}

QDeclarativeViewInspector::QDeclarativeViewInspector(QDeclarativeView *view,
                                                   QObject *parent) :
    AbstractViewInspector(parent),
    data(new QDeclarativeViewInspectorPrivate(this))
{
    data->view = view;
    data->manipulatorLayer = new LiveLayerItem(view->scene());
    data->selectionTool = new LiveSelectionTool(this);
    data->zoomTool = new ZoomTool(this);
    data->colorPickerTool = new ColorPickerTool(this);
    data->boundingRectHighlighter = new BoundingRectHighlighter(this);
    data->currentTool = data->selectionTool;

    // to capture ChildRemoved event when viewport changes
    data->view->installEventFilter(this);

    data->setViewport(data->view->viewport());

    connect(data->view, SIGNAL(statusChanged(QDeclarativeView::Status)),
            data.data(), SLOT(_q_onStatusChanged(QDeclarativeView::Status)));

    connect(data->colorPickerTool, SIGNAL(selectedColorChanged(QColor)),
            SIGNAL(selectedColorChanged(QColor)));
    connect(data->colorPickerTool, SIGNAL(selectedColorChanged(QColor)),
            this, SLOT(sendColorChanged(QColor)));

    changeTool(InspectorProtocol::SelectTool);
}

QDeclarativeViewInspector::~QDeclarativeViewInspector()
{
}

void QDeclarativeViewInspector::changeCurrentObjects(const QList<QObject*> &objects)
{
    QList<QGraphicsItem*> items;
    QList<QGraphicsObject*> gfxObjects;
    foreach (QObject *obj, objects) {
        if (QDeclarativeItem *declarativeItem = qobject_cast<QDeclarativeItem*>(obj)) {
            items << declarativeItem;
            gfxObjects << declarativeItem;
        }
    }
    if (designModeBehavior()) {
        data->setSelectedItemsForTools(items);
        data->clearHighlight();
        data->highlight(gfxObjects);
    }
}

void QDeclarativeViewInspector::reloadView()
{
    data->clearHighlight();
    emit reloadRequested();
}

void QDeclarativeViewInspector::changeTool(InspectorProtocol::Tool tool)
{
    switch (tool) {
    case InspectorProtocol::ColorPickerTool:
        data->changeToColorPickerTool();
        break;
    case InspectorProtocol::SelectMarqueeTool:
        data->changeToMarqueeSelectTool();
        break;
    case InspectorProtocol::SelectTool:
        data->changeToSingleSelectTool();
        break;
    case InspectorProtocol::ZoomTool:
        data->changeToZoomTool();
        break;
    }
}

QDeclarativeEngine *QDeclarativeViewInspector::declarativeEngine() const
{
    return data->view->engine();
}

void QDeclarativeViewInspectorPrivate::setViewport(QWidget *widget)
{
    if (viewport.data() == widget)
        return;

    if (viewport)
        viewport.data()->removeEventFilter(q);

    viewport = widget;
    if (viewport) {
        // make sure we get mouse move events
        viewport.data()->setMouseTracking(true);
        viewport.data()->installEventFilter(q);
    }
}

void QDeclarativeViewInspectorPrivate::clearEditorItems()
{
    clearHighlight();
    setSelectedItems(QList<QGraphicsItem*>());
}

bool QDeclarativeViewInspector::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == data->view) {
        // Event from view
        if (event->type() == QEvent::ChildRemoved) {
            // Might mean that viewport has changed
            if (data->view->viewport() != data->viewport.data())
                data->setViewport(data->view->viewport());
        }
        return QObject::eventFilter(obj, event);
    }

    // Event from viewport
    switch (event->type()) {
    case QEvent::Leave: {
        if (leaveEvent(event))
            return true;
        break;
    }
    case QEvent::MouseButtonPress: {
        if (mousePressEvent(static_cast<QMouseEvent*>(event)))
            return true;
        break;
    }
    case QEvent::MouseMove: {
        if (mouseMoveEvent(static_cast<QMouseEvent*>(event)))
            return true;
        break;
    }
    case QEvent::MouseButtonRelease: {
        if (mouseReleaseEvent(static_cast<QMouseEvent*>(event)))
            return true;
        break;
    }
    case QEvent::KeyPress: {
        if (keyPressEvent(static_cast<QKeyEvent*>(event)))
            return true;
        break;
    }
    case QEvent::KeyRelease: {
        if (keyReleaseEvent(static_cast<QKeyEvent*>(event)))
            return true;
        break;
    }
    case QEvent::MouseButtonDblClick: {
        if (mouseDoubleClickEvent(static_cast<QMouseEvent*>(event)))
            return true;
        break;
    }
    case QEvent::Wheel: {
        if (wheelEvent(static_cast<QWheelEvent*>(event)))
            return true;
        break;
    }
    default: {
        break;
    }
    } //switch

    // standard event processing
    return QObject::eventFilter(obj, event);
}

bool QDeclarativeViewInspector::leaveEvent(QEvent * /*event*/)
{
    if (!designModeBehavior())
        return false;
    data->clearHighlight();
    return true;
}

bool QDeclarativeViewInspector::mousePressEvent(QMouseEvent *event)
{
    if (!designModeBehavior())
        return false;
    data->cursorPos = event->pos();
    data->currentTool->mousePressEvent(event);
    return true;
}

bool QDeclarativeViewInspector::mouseMoveEvent(QMouseEvent *event)
{
    if (!designModeBehavior()) {
        data->clearEditorItems();
        return false;
    }
    data->cursorPos = event->pos();

    QList<QGraphicsItem*> selItems = data->selectableItems(event->pos());
    if (!selItems.isEmpty()) {
        declarativeView()->setToolTip(data->currentTool->titleForItem(selItems.first()));
    } else {
        declarativeView()->setToolTip(QString());
    }
    if (event->buttons()) {
        data->currentTool->mouseMoveEvent(event);
    } else {
        data->currentTool->hoverMoveEvent(event);
    }
    return true;
}

bool QDeclarativeViewInspector::mouseReleaseEvent(QMouseEvent *event)
{
    if (!designModeBehavior())
        return false;

    data->cursorPos = event->pos();
    data->currentTool->mouseReleaseEvent(event);
    return true;
}

bool QDeclarativeViewInspector::keyPressEvent(QKeyEvent *event)
{
    if (!designModeBehavior())
        return false;

    data->currentTool->keyPressEvent(event);
    return true;
}

bool QDeclarativeViewInspector::keyReleaseEvent(QKeyEvent *event)
{
    if (!designModeBehavior())
        return false;

    switch (event->key()) {
    case Qt::Key_V:
        changeTool(InspectorProtocol::SelectTool);
        break;
// disabled because multiselection does not do anything useful without design mode
//    case Qt::Key_M:
//        changeTool(InspectorProtocol::SelectMarqueeTool);
//        break;
    case Qt::Key_I:
        changeTool(InspectorProtocol::ColorPickerTool);
        break;
    case Qt::Key_Z:
        changeTool(InspectorProtocol::ZoomTool);
        break;
    case Qt::Key_Space:
        setAnimationPaused(!animationPaused());
        break;
    default:
        break;
    }

    data->currentTool->keyReleaseEvent(event);
    return true;
}

void QDeclarativeViewInspector::reparentQmlObject(QObject *object, QObject *newParent)
{
    if (!newParent)
        return;

    object->setParent(newParent);
    QDeclarativeItem *newParentItem = qobject_cast<QDeclarativeItem*>(newParent);
    QDeclarativeItem *item    = qobject_cast<QDeclarativeItem*>(object);
    if (newParentItem && item)
        item->setParentItem(newParentItem);
}

void QDeclarativeViewInspectorPrivate::_q_removeFromSelection(QObject *obj)
{
    QList<QGraphicsItem*> items = selectedItems();
    if (QGraphicsItem *item = qobject_cast<QGraphicsObject*>(obj))
        items.removeOne(item);
    setSelectedItems(items);
}

bool QDeclarativeViewInspector::mouseDoubleClickEvent(QMouseEvent * /*event*/)
{
    if (!designModeBehavior())
        return false;

    return true;
}

bool QDeclarativeViewInspector::wheelEvent(QWheelEvent *event)
{
    if (!designModeBehavior())
        return false;
    data->currentTool->wheelEvent(event);
    return true;
}

void QDeclarativeViewInspectorPrivate::setSelectedItemsForTools(const QList<QGraphicsItem *> &items)
{
    foreach (const QWeakPointer<QGraphicsObject> &obj, currentSelection) {
        if (QGraphicsItem *item = obj.data()) {
            if (!items.contains(item)) {
                QObject::disconnect(obj.data(), SIGNAL(destroyed(QObject*)),
                                    this, SLOT(_q_removeFromSelection(QObject*)));
                currentSelection.removeOne(obj);
            }
        }
    }

    foreach (QGraphicsItem *item, items) {
        if (QGraphicsObject *obj = item->toGraphicsObject()) {
            if (!currentSelection.contains(obj)) {
                QObject::connect(obj, SIGNAL(destroyed(QObject*)),
                                 this, SLOT(_q_removeFromSelection(QObject*)));
                currentSelection.append(obj);
            }
        }
    }

    currentTool->updateSelectedItems();
}

void QDeclarativeViewInspectorPrivate::setSelectedItems(const QList<QGraphicsItem *> &items)
{
    QList<QWeakPointer<QGraphicsObject> > oldList = currentSelection;
    setSelectedItemsForTools(items);
    if (oldList != currentSelection) {
        QList<QObject*> objectList;
        foreach (const QWeakPointer<QGraphicsObject> &graphicsObject, currentSelection) {
            if (graphicsObject)
                objectList << graphicsObject.data();
        }

        q->sendCurrentObjects(objectList);
    }
}

QList<QGraphicsItem *> QDeclarativeViewInspectorPrivate::selectedItems() const
{
    QList<QGraphicsItem *> selection;
    foreach (const QWeakPointer<QGraphicsObject> &selectedObject, currentSelection) {
        if (selectedObject.data())
            selection << selectedObject.data();
    }

    return selection;
}

void QDeclarativeViewInspector::setSelectedItems(QList<QGraphicsItem *> items)
{
    data->setSelectedItems(items);
}

QList<QGraphicsItem *> QDeclarativeViewInspector::selectedItems() const
{
    return data->selectedItems();
}

QDeclarativeView *QDeclarativeViewInspector::declarativeView() const
{
    return data->view;
}

void QDeclarativeViewInspectorPrivate::clearHighlight()
{
    boundingRectHighlighter->clear();
}

void QDeclarativeViewInspectorPrivate::highlight(const QList<QGraphicsObject *> &items)
{
    if (items.isEmpty())
        return;

    QList<QGraphicsObject*> objectList;
    foreach (QGraphicsItem *item, items) {
        QGraphicsItem *child = item;

        if (child) {
            QGraphicsObject *childObject = child->toGraphicsObject();
            if (childObject)
                objectList << childObject;
        }
    }

    boundingRectHighlighter->highlight(objectList);
}

QList<QGraphicsItem*> QDeclarativeViewInspectorPrivate::selectableItems(
    const QPointF &scenePos) const
{
    QList<QGraphicsItem*> itemlist = view->scene()->items(scenePos);
    return filterForSelection(itemlist);
}

QList<QGraphicsItem*> QDeclarativeViewInspectorPrivate::selectableItems(const QPoint &pos) const
{
    QList<QGraphicsItem*> itemlist = view->items(pos);
    return filterForSelection(itemlist);
}

QList<QGraphicsItem*> QDeclarativeViewInspectorPrivate::selectableItems(
    const QRectF &sceneRect, Qt::ItemSelectionMode selectionMode) const
{
    QList<QGraphicsItem*> itemlist = view->scene()->items(sceneRect, selectionMode);
    return filterForSelection(itemlist);
}

void QDeclarativeViewInspectorPrivate::changeToSingleSelectTool()
{
    currentToolMode = Constants::SelectionToolMode;
    selectionTool->setRubberbandSelectionMode(false);

    changeToSelectTool();

    emit q->selectToolActivated();
    q->sendCurrentTool(Constants::SelectionToolMode);
}

void QDeclarativeViewInspectorPrivate::changeToSelectTool()
{
    if (currentTool == selectionTool)
        return;

    currentTool->clear();
    currentTool = selectionTool;
    currentTool->clear();
    currentTool->updateSelectedItems();
}

void QDeclarativeViewInspectorPrivate::changeToMarqueeSelectTool()
{
    changeToSelectTool();
    currentToolMode = Constants::MarqueeSelectionToolMode;
    selectionTool->setRubberbandSelectionMode(true);

    emit q->marqueeSelectToolActivated();
    q->sendCurrentTool(Constants::MarqueeSelectionToolMode);
}

void QDeclarativeViewInspectorPrivate::changeToZoomTool()
{
    currentToolMode = Constants::ZoomMode;
    currentTool->clear();
    currentTool = zoomTool;
    currentTool->clear();

    emit q->zoomToolActivated();
    q->sendCurrentTool(Constants::ZoomMode);
}

void QDeclarativeViewInspectorPrivate::changeToColorPickerTool()
{
    if (currentTool == colorPickerTool)
        return;

    currentToolMode = Constants::ColorPickerMode;
    currentTool->clear();
    currentTool = colorPickerTool;
    currentTool->clear();

    emit q->colorPickerActivated();
    q->sendCurrentTool(Constants::ColorPickerMode);
}


static bool isEditorItem(QGraphicsItem *item)
{
    return (item->type() == Constants::EditorItemType
            || item->type() == Constants::ResizeHandleItemType
            || item->data(Constants::EditorItemDataKey).toBool());
}

QList<QGraphicsItem*> QDeclarativeViewInspectorPrivate::filterForSelection(
    QList<QGraphicsItem*> &itemlist) const
{
    foreach (QGraphicsItem *item, itemlist) {
        if (isEditorItem(item))
            itemlist.removeOne(item);
    }

    return itemlist;
}

void QDeclarativeViewInspectorPrivate::_q_onStatusChanged(QDeclarativeView::Status status)
{
    if (status == QDeclarativeView::Ready)
        q->sendReloaded();
}

// adjusts bounding boxes on edges of screen to be visible
QRectF QDeclarativeViewInspector::adjustToScreenBoundaries(const QRectF &boundingRectInSceneSpace)
{
    int marginFromEdge = 1;
    QRectF boundingRect(boundingRectInSceneSpace);
    if (qAbs(boundingRect.left()) - 1 < 2)
        boundingRect.setLeft(marginFromEdge);

    QRect rect = data->view->rect();

    if (boundingRect.right() >= rect.right())
        boundingRect.setRight(rect.right() - marginFromEdge);

    if (qAbs(boundingRect.top()) - 1 < 2)
        boundingRect.setTop(marginFromEdge);

    if (boundingRect.bottom() >= rect.bottom())
        boundingRect.setBottom(rect.bottom() - marginFromEdge);

    return boundingRect;
}

QT_END_NAMESPACE
