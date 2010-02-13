/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qmlgraphicspathview_p.h"
#include "qmlgraphicspathview_p_p.h"

#include <qmlstate_p.h>
#include <qmlopenmetaobject_p.h>

#include <QDebug>
#include <QEvent>
#include <qlistmodelinterface_p.h>
#include <QGraphicsSceneEvent>

#include <math.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,PathView,QmlGraphicsPathView)


inline qreal qmlMod(qreal x, qreal y)
{
#ifdef QT_USE_MATH_H_FLOATS
    if(sizeof(qreal) == sizeof(float))
        return fmodf(float(x), float(y));
    else
#endif
        return fmod(x, y);
}


class QmlGraphicsPathViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool onPath READ isOnPath NOTIFY onPathChanged)
public:
    QmlGraphicsPathViewAttached(QObject *parent)
    : QObject(parent), mo(new QmlOpenMetaObject(this)), onPath(false)
    {
    }

    ~QmlGraphicsPathViewAttached()
    {
        QmlGraphicsPathView::attachedProperties.remove(parent());
    }

    QVariant value(const QByteArray &name) const
    {
        return mo->value(name);
    }
    void setValue(const QByteArray &name, const QVariant &val)
    {
        mo->setValue(name, val);
    }

    bool isOnPath() const { return onPath; }
    void setOnPath(bool on) {
        if (on != onPath) {
            onPath = on;
            emit onPathChanged();
        }
    }

Q_SIGNALS:
    void onPathChanged();

private:
    QmlOpenMetaObject *mo;
    bool onPath;
};


QmlGraphicsItem *QmlGraphicsPathViewPrivate::getItem(int modelIndex)
{
    Q_Q(QmlGraphicsPathView);
    requestedIndex = modelIndex;
    QmlGraphicsItem *item = model->item(modelIndex);
    if (item) {
        if (QObject *obj = QmlGraphicsPathView::qmlAttachedProperties(item))
            static_cast<QmlGraphicsPathViewAttached *>(obj)->setOnPath(true);
        item->setParentItem(q);
    }
    requestedIndex = -1;
    return item;
}

void QmlGraphicsPathViewPrivate::releaseItem(QmlGraphicsItem *item)
{
    if (!item || !model)
        return;
    if (QObject *obj = QmlGraphicsPathView::qmlAttachedProperties(item))
        static_cast<QmlGraphicsPathViewAttached *>(obj)->setOnPath(false);
    if (model->release(item) == 0) {
        if (QObject *obj = QmlGraphicsPathView::qmlAttachedProperties(item))
            static_cast<QmlGraphicsPathViewAttached *>(obj)->setOnPath(false);
    }
}

/*!
    \qmlclass PathView QmlGraphicsPathView
    \brief The PathView element lays out model-provided items on a path.
    \inherits Item

    The model is typically provided by a QAbstractListModel "C++ model object", but can also be created directly in QML.

    The items are laid out along a path defined by a \l Path and may be flicked to scroll.

    \snippet doc/src/snippets/declarative/pathview/pathview.qml 0

    \image pathview.gif

    \sa Path
*/

QmlGraphicsPathView::QmlGraphicsPathView(QmlGraphicsItem *parent)
  : QmlGraphicsItem(*(new QmlGraphicsPathViewPrivate), parent)
{
    Q_D(QmlGraphicsPathView);
    d->init();
}

QmlGraphicsPathView::~QmlGraphicsPathView()
{
    Q_D(QmlGraphicsPathView);
    if (d->ownModel)
        delete d->model;
}

/*!
    \qmlattachedproperty bool PathView::onPath
    This attached property holds whether the item is currently on the path.

    If a pathItemCount has been set, it is possible that some items may
    be instantiated, but not considered to be currently on the path.
    Usually, these items would be set invisible, for example:

    \code
    Component {
        Rectangle {
            visible: PathView.onPath
            ...
        }
    }
    \endcode

    It is attached to each instance of the delegate.
*/

/*!
    \qmlproperty model PathView::model
    This property holds the model providing data for the view.

    The model provides a set of data that is used to create the items for the view.
    For large or dynamic datasets the model is usually provided by a C++ model object.
    Models can also be created directly in XML, using the ListModel element.

    \sa {qmlmodels}{Data Models}
*/
QVariant QmlGraphicsPathView::model() const
{
    Q_D(const QmlGraphicsPathView);
    return d->modelVariant;
}

void QmlGraphicsPathView::setModel(const QVariant &model)
{
    Q_D(QmlGraphicsPathView);
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
        for (int i=0; i<d->items.count(); i++){
            QmlGraphicsItem *p = d->items[i];
            d->model->release(p);
        }
        d->items.clear();
    }

    d->modelVariant = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QmlGraphicsVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QmlGraphicsVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
            d->ownModel = true;
        }
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
    }
    d->firstIndex = 0;
    d->pathOffset = 0;
    d->regenerate();
    d->fixOffset();
}

/*!
    \qmlproperty int PathView::count
    This property holds the number of items in the model.
*/
int QmlGraphicsPathView::count() const
{
    Q_D(const QmlGraphicsPathView);
    return d->model ? d->model->count() : 0;
}

/*!
    \qmlproperty Path PathView::path
    \default
    This property holds the path used to lay out the items.
    For more information see the \l Path documentation.
*/
QmlGraphicsPath *QmlGraphicsPathView::path() const
{
    Q_D(const QmlGraphicsPathView);
    return d->path;
}

void QmlGraphicsPathView::setPath(QmlGraphicsPath *path)
{
    Q_D(QmlGraphicsPathView);
    d->path = path;
    connect(d->path, SIGNAL(changed()), this, SLOT(refill()));
    d->regenerate();
}

/*!
    \qmlproperty int PathView::currentIndex
    This property holds the index of the current item.
*/
int QmlGraphicsPathView::currentIndex() const
{
    Q_D(const QmlGraphicsPathView);
    return d->currentIndex;
}

void QmlGraphicsPathView::setCurrentIndex(int idx)
{
    Q_D(QmlGraphicsPathView);
    if (d->model && d->model->count())
        idx = qAbs(idx % d->model->count());
    if (d->model && idx != d->currentIndex) {
        d->currentIndex = idx;
        if (d->model->count()) {
            d->snapToCurrent();
            int itemIndex = (idx - d->firstIndex + d->model->count()) % d->model->count();
            if (itemIndex < d->items.count())
                d->items.at(itemIndex)->setFocus(true);
        }
        emit currentIndexChanged();
    }
}

/*!
    \qmlproperty real PathView::offset

    The offset specifies how far along the path the items are from their initial positions.
*/
qreal QmlGraphicsPathView::offset() const
{
    Q_D(const QmlGraphicsPathView);
    return d->_offset;
}

void QmlGraphicsPathView::setOffset(qreal offset)
{
    Q_D(QmlGraphicsPathView);
    d->setOffset(offset);
    d->updateCurrent();
}

void QmlGraphicsPathViewPrivate::setOffset(qreal o)
{
    Q_Q(QmlGraphicsPathView);
    if (_offset != o) {
        _offset = qmlMod(o, qreal(100.0));
        if (_offset < 0)
            _offset = 100.0 + _offset;
        q->refill();
    }
}

/*!
    \qmlproperty real PathView::snapPosition

    This property determines the position (0-100) the nearest item will snap to.
*/
qreal QmlGraphicsPathView::snapPosition() const
{
    Q_D(const QmlGraphicsPathView);
    return d->snapPos;
}

void QmlGraphicsPathView::setSnapPosition(qreal pos)
{
    Q_D(QmlGraphicsPathView);
    d->snapPos = pos/100;
    d->fixOffset();
}

/*!
    \qmlproperty real PathView::dragMargin
    This property holds the maximum distance from the path that initiate mouse dragging.

    By default the path can only be dragged by clicking on an item.  If
    dragMargin is greater than zero, a drag can be initiated by clicking
    within dragMargin pixels of the path.
*/
qreal QmlGraphicsPathView::dragMargin() const
{
    Q_D(const QmlGraphicsPathView);
    return d->dragMargin;
}

void QmlGraphicsPathView::setDragMargin(qreal dragMargin)
{
    Q_D(QmlGraphicsPathView);
    d->dragMargin = dragMargin;
}

/*!
    \qmlproperty component PathView::delegate

    The delegate provides a template defining each item instantiated by the view.
    The index is exposed as an accessible \c index property.  Properties of the
    model are also available depending upon the type of \l {qmlmodels}{Data Model}.

    Note that the PathView will layout the items based on the size of the root
    item in the delegate.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/pathview/pathview.qml 1
*/
QmlComponent *QmlGraphicsPathView::delegate() const
{
    Q_D(const QmlGraphicsPathView);
     if (d->model) {
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QmlGraphicsPathView::setDelegate(QmlComponent *c)
{
    Q_D(QmlGraphicsPathView);
    if (!d->ownModel) {
        d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model)) {
        dataModel->setDelegate(c);
        d->regenerate();
    }
}

/*!
  \qmlproperty int PathView::pathItemCount
  This property holds the number of items visible on the path at any one time
*/
int QmlGraphicsPathView::pathItemCount() const
{
    Q_D(const QmlGraphicsPathView);
    return d->pathItems;
}

void QmlGraphicsPathView::setPathItemCount(int i)
{
    Q_D(QmlGraphicsPathView);
    if (i == d->pathItems)
        return;
    d->pathItems = i;
    d->regenerate();
}

QPointF QmlGraphicsPathViewPrivate::pointNear(const QPointF &point, qreal *nearPercent) const
{
    //XXX maybe do recursively at increasing resolution.
    qreal mindist = 1e10; // big number
    QPointF nearPoint = path->pointAt(0);
    qreal nearPc = 0;
    for (qreal i=1; i < 1000; i++) {
        QPointF pt = path->pointAt(i/1000.0);
        QPointF diff = pt - point;
        qreal dist = diff.x()*diff.x() + diff.y()*diff.y();
        if (dist < mindist) {
            nearPoint = pt;
            nearPc = i;
            mindist = dist;
        }
    }

    if (nearPercent)
        *nearPercent = nearPc / 10.0;

    return nearPoint;
}


void QmlGraphicsPathView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsPathView);
    if (!d->items.count())
        return;
    QPointF scenePoint = mapToScene(event->pos());
    int idx = 0;
    for (; idx < d->items.count(); ++idx) {
        QRectF rect = d->items.at(idx)->boundingRect();
        rect = d->items.at(idx)->mapToScene(rect).boundingRect();
        if (rect.contains(scenePoint))
            break;
    }
    if (idx == d->items.count() && d->dragMargin == 0.)  // didn't click on an item
        return;

    d->startPoint = d->pointNear(event->pos(), &d->startPc);
    if (idx == d->items.count()) {
        qreal distance = qAbs(event->pos().x() - d->startPoint.x()) + qAbs(event->pos().y() - d->startPoint.y());
        if (distance > d->dragMargin)
            return;
    }

    d->stealMouse = false;
    d->lastElapsed = 0;
    d->lastDist = 0;
    QmlGraphicsItemPrivate::start(d->lastPosTime);
    d->tl.clear();
}

void QmlGraphicsPathView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsPathView);
    if (d->lastPosTime.isNull())
        return;

    if (!d->stealMouse) {
        QPointF delta = event->pos() - d->startPoint;
        if (qAbs(delta.x()) > QApplication::startDragDistance() && qAbs(delta.y()) > QApplication::startDragDistance())
            d->stealMouse = true;
    }

    if (d->stealMouse) {
        d->moveReason = QmlGraphicsPathViewPrivate::Mouse;
        qreal newPc;
        d->pointNear(event->pos(), &newPc);
        qreal diff = newPc - d->startPc;
        if (diff) {
            setOffset(d->_offset + diff);

            if (diff > 50)
                diff -= 100;
            else if (diff < -50)
                diff += 100;

            d->lastElapsed = QmlGraphicsItemPrivate::restart(d->lastPosTime);
            d->lastDist = diff;
            d->startPc = newPc;
        }
    }
}

void QmlGraphicsPathView::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    Q_D(QmlGraphicsPathView);
    if (d->lastPosTime.isNull())
        return;

    qreal elapsed = qreal(d->lastElapsed + QmlGraphicsItemPrivate::elapsed(d->lastPosTime)) / 1000.;
    qreal velocity = elapsed > 0. ? d->lastDist / elapsed : 0;
    if (d->model && d->model->count() && qAbs(velocity) > 5) {
        if (velocity > 100)
            velocity = 100;
        else if (velocity < -100)
            velocity = -100;
        qreal inc = qmlMod(d->_offset - d->snapPos, qreal(100.0 / d->model->count()));
        qreal dist = qAbs(velocity/2 - qmlMod(velocity/2, qreal(100.0 / d->model->count()) - inc));
        d->moveOffset.setValue(d->_offset);
        d->tl.accel(d->moveOffset, velocity, 10, dist);
        d->tl.execute(d->fixupOffsetEvent);
    } else {
        d->fixOffset();
    }

    d->lastPosTime = QTime();
    d->stealMouse = false;
    ungrabMouse();
}

bool QmlGraphicsPathView::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QmlGraphicsPathView);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();
    QGraphicsScene *s = scene();
    QmlGraphicsItem *grabber = s ? qobject_cast<QmlGraphicsItem*>(s->mouseGrabberItem()) : 0;
    if ((d->stealMouse || myRect.contains(event->scenePos().toPoint())) && (!grabber || !grabber->keepMouseGrab())) {
        mouseEvent.setAccepted(false);
        for (int i = 0x1; i <= 0x10; i <<= 1) {
            if (event->buttons() & i) {
                Qt::MouseButton button = Qt::MouseButton(i);
                mouseEvent.setButtonDownPos(button, mapFromScene(event->buttonDownPos(button)));
            }
        }
        mouseEvent.setScenePos(event->scenePos());
        mouseEvent.setLastScenePos(event->lastScenePos());
        mouseEvent.setPos(mapFromScene(event->scenePos()));
        mouseEvent.setLastPos(mapFromScene(event->lastScenePos()));

        switch(mouseEvent.type()) {
        case QEvent::GraphicsSceneMouseMove:
            mouseMoveEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMousePress:
            mousePressEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMouseRelease:
            mouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = qobject_cast<QmlGraphicsItem*>(s->mouseGrabberItem());
        if (grabber && d->stealMouse && !grabber->keepMouseGrab() && grabber != this)
            grabMouse();

        return d->stealMouse;
    } else if (!d->lastPosTime.isNull()) {
        d->lastPosTime = QTime();
    }
    return false;
}

bool QmlGraphicsPathView::sceneEventFilter(QGraphicsItem *i, QEvent *e)
{
    if (!isVisible())
        return QmlGraphicsItem::sceneEventFilter(i, e);

    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        {
            bool ret = sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
            if (e->type() == QEvent::GraphicsSceneMouseRelease)
                return ret;
            break;
        }
    default:
        break;
    }

    return QmlGraphicsItem::sceneEventFilter(i, e);
}

void QmlGraphicsPathView::componentComplete()
{
    Q_D(QmlGraphicsPathView);
    QmlGraphicsItem::componentComplete();
    d->regenerate();

    // move to correct offset
    if (d->items.count()) {
        int itemIndex = (d->currentIndex - d->firstIndex + d->model->count()) % d->model->count();

        itemIndex += d->pathOffset;
        itemIndex %= d->items.count();
        qreal targetOffset = qmlMod(100 + (d->snapPos*100) - 100.0 * itemIndex / d->items.count(), qreal(100.0));

        if (targetOffset < 0)
            targetOffset = 100.0 + targetOffset;
        if (targetOffset != d->_offset) {
            d->moveOffset.setValue(targetOffset);
        }
    }
}

void QmlGraphicsPathViewPrivate::regenerate()
{
    Q_Q(QmlGraphicsPathView);
    if (!q->isComponentComplete())
        return;

    for (int i=0; i<items.count(); i++){
        QmlGraphicsItem *p = items[i];
        releaseItem(p);
    }
    items.clear();

    if (!isValid())
        return;

    if (firstIndex >= model->count())
        firstIndex = model->count()-1;
    if (pathOffset >= model->count())
        pathOffset = model->count()-1;

    int numItems = pathItems >= 0 ? pathItems : model->count();
    for (int i=0; i < numItems && i < model->count(); ++i){
        int index = (i + firstIndex) % model->count();
        QmlGraphicsItem *item = getItem(index);
        if (!item) {
            qWarning() << "PathView: Cannot create item, index" << (i + firstIndex) % model->count();
            return;
        }
        items.append(item);
        item->setZValue(i);
        if (currentIndex == index)
            item->setFocus(true);
    }
    q->refill();
}

void QmlGraphicsPathViewPrivate::updateItem(QmlGraphicsItem *item, qreal percent)
{
    if (QObject *obj = QmlGraphicsPathView::qmlAttachedProperties(item)) {
        foreach(const QString &attr, path->attributes())
            static_cast<QmlGraphicsPathViewAttached *>(obj)->setValue(attr.toUtf8(), path->attributeAt(attr, percent));
    }
    QPointF pf = path->pointAt(percent);
    item->setX(pf.x() - item->width()*item->scale()/2);
    item->setY(pf.y() - item->height()*item->scale()/2);
}

void QmlGraphicsPathView::refill()
{
    Q_D(QmlGraphicsPathView);
    if (!d->isValid() || !isComponentComplete())
        return;

    QList<qreal> positions;
    for (int i=0; i<d->items.count(); i++){
        qreal percent = i * (100. / d->items.count());
        percent = percent + d->_offset;
        percent = qmlMod(percent, qreal(100.0));
        positions << qAbs(percent/100.0);
    }

    if (d->pathItems==-1) {
        for (int i=0; i<positions.count(); i++)
            d->updateItem(d->items.at(i), positions[i]);
        return;
    }

    QList<qreal> rotatedPositions;
    for (int i=0; i<d->items.count(); i++)
        rotatedPositions << positions[(i + d->pathOffset + d->items.count()) % d->items.count()];

    int wrapIndex= -1;
    for (int i=0; i<d->items.count()-1; i++) {
        if (rotatedPositions[i] > rotatedPositions[i+1]){
            wrapIndex = i;
            break;
        }
    }
    if (wrapIndex != -1 ){
        //A wraparound has occured
        if (wrapIndex < d->items.count()/2){
            while(wrapIndex-- >= 0){
                QmlGraphicsItem* p = d->items.takeFirst();
                d->updateItem(p, 0.0);
                d->releaseItem(p);
                d->firstIndex++;
                d->firstIndex %= d->model->count();
                int index = (d->firstIndex + d->items.count())%d->model->count();
                QmlGraphicsItem *item = d->getItem(index);
                item->setZValue(wrapIndex);
                if (d->currentIndex == index)
                    item->setFocus(true);
                d->items << item;
                d->pathOffset++;
                d->pathOffset=d->pathOffset % d->items.count();
            }
        } else {
            while(wrapIndex++ < d->items.count()-1){
                QmlGraphicsItem* p = d->items.takeLast();
                d->updateItem(p, 1.0);
                d->releaseItem(p);
                d->firstIndex--;
                if (d->firstIndex < 0)
                    d->firstIndex = d->model->count() - 1;
                QmlGraphicsItem *item = d->getItem(d->firstIndex);
                item->setZValue(d->firstIndex);
                if (d->currentIndex == d->firstIndex)
                    item->setFocus(true);
                d->items.prepend(item);
                d->pathOffset--;
                if (d->pathOffset < 0)
                    d->pathOffset = d->items.count() - 1;
            }
        }
        for (int i=0; i<d->items.count(); i++)
            rotatedPositions[i] = positions[(i + d->pathOffset + d->items.count())
                                    % d->items.count()];
    }
    for (int i=0; i<d->items.count(); i++)
        d->updateItem(d->items.at(i), rotatedPositions[i]);
}

void QmlGraphicsPathView::itemsInserted(int modelIndex, int count)
{
    //XXX support animated insertion
    Q_D(QmlGraphicsPathView);
    if (!d->isValid() || !isComponentComplete())
        return;
    if (d->pathItems == -1) {
        for (int i = 0; i < count; ++i) {
            QmlGraphicsItem *item = d->getItem(modelIndex + i);
            item->setZValue(modelIndex + i);
            d->items.insert(modelIndex + i, item);
        }
        refill();
    } else {
        //XXX This is pretty heavy handed until we reference count items.
        d->regenerate();
    }

    // make sure the current item is still at the snap position
    int itemIndex = (d->currentIndex - d->firstIndex + d->model->count())%d->model->count();
    itemIndex += d->pathOffset;
    itemIndex %= d->items.count();
    qreal targetOffset = qmlMod(100 + (d->snapPos*100) - 100.0 * itemIndex / d->items.count(), qreal(100.0));

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset != d->_offset)
        d->moveOffset.setValue(targetOffset);
}

void QmlGraphicsPathView::itemsRemoved(int modelIndex, int count)
{
    //XXX support animated removal
    Q_D(QmlGraphicsPathView);
    if (!d->isValid() || !isComponentComplete())
        return;
    if (d->pathItems == -1) {
        for (int i = 0; i < count; ++i) {
            QmlGraphicsItem* p = d->items.takeAt(modelIndex);
            d->model->release(p);
        }
        d->snapToCurrent();
        refill();
    } else {
        d->regenerate();
    }

    if (d->model->count() == 0) {
        d->currentIndex = -1;
        d->moveOffset.setValue(0);
        return;
    }

    // make sure the current item is still at the snap position
    if (d->currentIndex >= d->model->count())
        d->currentIndex = d->model->count() - 1;
    int itemIndex = (d->currentIndex - d->firstIndex + d->model->count())%d->model->count();
    itemIndex += d->pathOffset;
    itemIndex %= d->items.count();
    qreal targetOffset = qmlMod(100 + (d->snapPos*100) - 100.0 * itemIndex / d->items.count(), qreal(100.0));

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset != d->_offset)
        d->moveOffset.setValue(targetOffset);
}

void QmlGraphicsPathView::createdItem(int index, QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsPathView);
    if (d->requestedIndex != index) {
        item->setParentItem(this);
        d->updateItem(item, index < d->firstIndex ? 0.0 : 1.0);
    }
}

void QmlGraphicsPathView::destroyingItem(QmlGraphicsItem *item)
{
    Q_UNUSED(item);
}

void QmlGraphicsPathView::ticked()
{
    Q_D(QmlGraphicsPathView);
    d->updateCurrent();
}

// find the item closest to the snap position
int QmlGraphicsPathViewPrivate::calcCurrentIndex()
{
    int current = -1;
    if (model && items.count()) {
        _offset = qmlMod(_offset, qreal(100.0));
        if (_offset < 0)
            _offset += 100.0;

        if (pathItems == -1) {
            qreal delta = qmlMod(_offset - snapPos, qreal(100.0));
            if (delta < 0)
                delta = 100.0 + delta;
            int ii = model->count() - qRound(delta * model->count() / 100);
            if (ii < 0)
                ii = 0;
            current = ii;
        } else {
            qreal bestDiff=1e9;
            int bestI=-1;
            for (int i=0; i<items.count(); i++){
                qreal percent = i * (100. / items.count());
                percent = percent + _offset;
                percent = qmlMod(percent, qreal(100.0));
                qreal diff = qAbs(snapPos - (percent/100.0));
                if (diff < bestDiff){
                    bestDiff = diff;
                    bestI = i;
                }
            }
            int modelIndex = (bestI - pathOffset + items.count())%items.count();
            modelIndex += firstIndex;
            current = modelIndex;
        }
        current = qAbs(current % model->count());
    }

    return current;
}

void QmlGraphicsPathViewPrivate::updateCurrent()
{
    Q_Q(QmlGraphicsPathView);
    if (moveReason != Mouse)
        return;
    int idx = calcCurrentIndex();
    if (model && idx != currentIndex) {
        currentIndex = idx;
        int itemIndex = (idx - firstIndex + model->count()) % model->count();
        if (itemIndex < items.count())
            items.at(itemIndex)->setFocus(true);
        emit q->currentIndexChanged();
    }
}

void QmlGraphicsPathViewPrivate::fixOffset()
{
    Q_Q(QmlGraphicsPathView);
    if (model && items.count()) {
        int curr = calcCurrentIndex();
        if (curr != currentIndex)
            q->setCurrentIndex(curr);
        else
            snapToCurrent();
    }
}

void QmlGraphicsPathViewPrivate::snapToCurrent()
{
    if (!model || model->count() <= 0)
        return;

    int itemIndex = (currentIndex - firstIndex + model->count()) % model->count();

    //Rounds is the number of times round to make the current item visible
    int rounds = itemIndex / items.count();
    int otherWayRounds = (model->count() - (itemIndex)) / items.count();
    if (otherWayRounds < rounds)
        rounds = -otherWayRounds;

    itemIndex += pathOffset;
    if(model->count() % items.count() && itemIndex - model->count() + items.count() > 0){
        //When model.count() is not a multiple of pathItemCount we need to manually
        //fix the index so that going backwards one step works correctly.
        itemIndex = itemIndex - model->count() + items.count();
    }
    itemIndex %= items.count();
    qreal targetOffset = qmlMod(100 + (snapPos*100) - 100.0 * itemIndex / items.count(), qreal(100.0));

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset == _offset && rounds == 0)
        return;

    moveReason = Other;
    tl.clear();
    moveOffset.setValue(_offset);

    if (rounds!=0){
        //Compensate if the targetOffset would bring the target in from off the screen
        qreal distance = targetOffset - _offset;
        if (distance <= -50)
            rounds--;
        if (distance > 50)
            rounds++;
        tl.move(moveOffset, targetOffset + 100.0*(-rounds), QEasingCurve(QEasingCurve::InOutQuad),
                int(100*items.count()*qMax((qreal)(2.0/items.count()),(qreal)qAbs(rounds))));
        tl.execute(fixupOffsetEvent);
        return;
    }

    if (targetOffset - _offset > 50.0) {
        qreal distance = 100 - targetOffset + _offset;
        tl.move(moveOffset, 0.0, QEasingCurve(QEasingCurve::OutQuad), int(200 * _offset / distance));
        tl.set(moveOffset, 100.0);
        tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InQuad), int(200 * (100-targetOffset) / distance));
    } else if (targetOffset - _offset <= -50.0) {
        qreal distance = 100 - _offset + targetOffset;
        tl.move(moveOffset, 100.0, QEasingCurve(QEasingCurve::OutQuad), int(200 * (100-_offset) / distance));
        tl.set(moveOffset, 0.0);
        tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InQuad), int(200 * targetOffset / distance));
    } else {
        tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InOutQuad), 200);
    }
}

QHash<QObject*, QObject*> QmlGraphicsPathView::attachedProperties;
QObject *QmlGraphicsPathView::qmlAttachedProperties(QObject *obj)
{
    QObject *rv = attachedProperties.value(obj);
    if (!rv) {
        rv = new QmlGraphicsPathViewAttached(obj);
        attachedProperties.insert(obj, rv);
    }
    return rv;
}

QT_END_NAMESPACE

#include <qmlgraphicspathview.moc>
