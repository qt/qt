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

#include <math.h>
#include <QDebug>
#include <QEvent>
#include "qmlstate.h"
#include "qlistmodelinterface.h"
#include "qmlopenmetaobject.h"

#include "qfxpathview.h"
#include "qfxpathview_p.h"
#include <QGraphicsSceneEvent>

static const int FlickThreshold = 5;

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,PathView,QFxPathView)

class QFxPathViewAttached : public QObject
{
    Q_OBJECT
public:
    QFxPathViewAttached(QObject *parent)
    : QObject(parent), mo(new QmlOpenMetaObject(this))
    {
    }

    ~QFxPathViewAttached()
    {
        QFxPathView::attachedProperties.remove(parent());
    }

    QVariant value(const QByteArray &name) const 
    { 
        return mo->value(name); 
    }
    void setValue(const QByteArray &name, const QVariant &val)
    {
        mo->setValue(name, val);
    }

private:
    QmlOpenMetaObject *mo;
};


/*!
    \internal
    \class QFxPathView
    \brief The QFxPathView class lays out items provided by a model on a path.

    \ingroup group_views

    The model must be a \l QListModelInterface subclass.

    \sa QFxPath
*/

/*!
    \qmlclass PathView
    \brief The PathView element lays out model-provided items on a path.
    \inherits Item

    The model is typically provided by a QAbstractListModel "C++ model object", but can also be created directly in QML.

    The items are laid out along a path defined by a \l Path and may be flicked to scroll.

    \snippet doc/src/snippets/declarative/pathview/pathview.qml 0

    \image pathview.gif

    \sa Path
*/

QFxPathView::QFxPathView(QFxItem *parent)
  : QFxItem(*(new QFxPathViewPrivate), parent)
{
    Q_D(QFxPathView);
    d->init();
}

QFxPathView::QFxPathView(QFxPathViewPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    Q_D(QFxPathView);
    d->init();
}

QFxPathView::~QFxPathView()
{
}

/*!
    \qmlproperty model PathView::model
    This property holds the model providing data for the view.

    The model provides a set of data that is used to create the items for the view.
    For large or dynamic datasets the model is usually provided by a C++ model object.
    Models can also be created directly in XML, using the ListModel element.
*/

/*!
    \property QFxPathView::model
    \brief the model providing data for the view.

    The model must be either a \l QListModelInterface or
    \l QFxVisualItemModel subclass.
*/
QVariant QFxPathView::model() const
{
    Q_D(const QFxPathView);
    return d->modelVariant;
}

void QFxPathView::setModel(const QVariant &model)
{
    Q_D(QFxPathView);
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(createdItem(int, QFxItem*)), this, SLOT(createdItem(int,QFxItem*)));
        for (int i=0; i<d->items.count(); i++){
            QFxItem *p = d->items[i];
            d->model->release(p);
        }
        d->items.clear();
    }

    d->modelVariant = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QFxVisualItemModel *vim = 0;
    if (object && (vim = qobject_cast<QFxVisualItemModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QFxVisualItemModel(qmlContext(this));
            d->ownModel = true;
        }
        d->model->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(createdItem(int, QFxItem*)), this, SLOT(createdItem(int,QFxItem*)));
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
int QFxPathView::count() const
{
    Q_D(const QFxPathView);
    return d->model ? d->model->count() : 0;
}

/*!
    \qmlproperty Path PathView::path
    \default
    This property holds the path used to lay out the items.
    For more information see the \l Path documentation.
*/
QFxPath *QFxPathView::path() const
{
    Q_D(const QFxPathView);
    return d->path;
}

void QFxPathView::setPath(QFxPath *path)
{
    Q_D(QFxPathView);
    d->path = path;
    connect(d->path, SIGNAL(changed()), this, SLOT(refill()));
    d->regenerate();
}

/*!
    \qmlproperty int PathView::currentIndex
    This property holds the index of the current item.
*/
int QFxPathView::currentIndex() const
{
    Q_D(const QFxPathView);
    return d->currentIndex;
}

void QFxPathView::setCurrentIndex(int idx)
{
    Q_D(QFxPathView);
    if (d->model && d->model->count())
        idx = qAbs(idx % d->model->count());
    if (d->model && idx != d->currentIndex) {
        d->currentIndex = idx;
        d->snapToCurrent();
        int itemIndex = (idx - d->firstIndex + d->model->count()) % d->model->count();
        if (itemIndex < d->items.count())
            d->items.at(itemIndex)->setFocus(true);
        emit currentIndexChanged();
    }
}

/*!
    \qmlproperty real PathView::offset

    The offset specifies how far along the path the items are from their initial positions.
*/
qreal QFxPathView::offset() const
{
    Q_D(const QFxPathView);
    return d->_offset;
}

void QFxPathView::setOffset(qreal offset)
{
    Q_D(QFxPathView);
    d->setOffset(offset);
    d->updateCurrent();
}

void QFxPathViewPrivate::setOffset(qreal o)
{
    Q_Q(QFxPathView);
    if (_offset != o) {
        _offset = fmod(o, 100.0);
        if (_offset < 0)
            _offset = 100.0 + _offset;
        q->refill();
    }
}

/*!
    \qmlproperty real PathView::snapPosition
    This property holds the position (0-100) the current item snaps to.
*/

/*!
    \property QFxPathView::snapPosition
    \brief sets the position (0-100) the current item snaps to.

    This property determines the position the nearest item will snap to.
*/
qreal QFxPathView::snapPosition() const
{
    Q_D(const QFxPathView);
    return d->snapPos;
}

void QFxPathView::setSnapPosition(qreal pos)
{
    Q_D(QFxPathView);
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
qreal QFxPathView::dragMargin() const
{
    Q_D(const QFxPathView);
    return d->dragMargin;
}

void QFxPathView::setDragMargin(qreal dragMargin)
{
    Q_D(QFxPathView);
    d->dragMargin = dragMargin;
}

/*!
    \qmlproperty component PathView::delegate

    The delegate provides a template describing what each item in the view should look and act like.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/pathview/pathview.qml 1
*/

/*!
    \property QFxPathView::delegate
    \brief the component to use to render the items.

    The delegate is a component that the view will instantiate and destroy
    as needed to display the items.

*/
QmlComponent *QFxPathView::delegate() const
{
    Q_D(const QFxPathView);
    return d->model ? d->model->delegate() : 0;
}

void QFxPathView::setDelegate(QmlComponent *c)
{
    Q_D(QFxPathView);
    if (!d->ownModel) {
        d->model = new QFxVisualItemModel(qmlContext(this));
        d->ownModel = true;
    }
    d->model->setDelegate(c);
    d->regenerate();
}

/*!
  \property QFxPathView::pathItemCount
  \brief the number of items visible on the path at any one time
  */
/*!
  \qmlproperty int PathView::pathItemCount
  This property holds the number of items visible on the path at any one time
*/
int QFxPathView::pathItemCount() const
{
    Q_D(const QFxPathView);
    return d->pathItems;
}

void QFxPathView::setPathItemCount(int i)
{
    Q_D(QFxPathView);
    if (i == d->pathItems)
        return;
    d->pathItems = i;
    d->regenerate();
}

QPointF QFxPathViewPrivate::pointNear(const QPointF &point, qreal *nearPercent) const
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


void QFxPathView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxPathView);
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
    d->lastPosTime.start();
    d->tl.clear();
}

void QFxPathView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxPathView);
    if (d->lastPosTime.isNull())
        return;

    if (!d->stealMouse) {
        QPointF delta = event->pos() - d->startPoint;
        if (qAbs(delta.x()) > FlickThreshold && qAbs(delta.y()) > FlickThreshold)
            d->stealMouse = true;
    }

    if (d->stealMouse) {
        d->moveReason = QFxPathViewPrivate::Mouse;
        qreal newPc;
        d->pointNear(event->pos(), &newPc);
        qreal diff = newPc - d->startPc;
        if (diff) {
            setOffset(d->_offset + diff);

            if (diff > 50)
                diff -= 100;
            else if (diff < -50)
                diff += 100;

            d->lastElapsed = d->lastPosTime.restart();
            d->lastDist = diff;
            d->startPc = newPc;
        }
    }
}

void QFxPathView::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    Q_D(QFxPathView);
    if (d->lastPosTime.isNull())
        return;

    qreal elapsed = qreal(d->lastElapsed + d->lastPosTime.elapsed()) / 1000.;
    qreal velocity = elapsed > 0. ? d->lastDist / elapsed : 0;
    if (d->model && d->model->count() && qAbs(velocity) > 5) {
        if (velocity > 100)
            velocity = 100;
        else if (velocity < -100)
            velocity = -100;
        qreal inc = fmod(d->_offset - d->snapPos, 100.0 / d->model->count());
        qreal dist = qAbs(velocity/2 - fmod(velocity/2, 100.0 / d->model->count()) - inc);
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

bool QFxPathView::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QFxPathView);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapToScene(QRectF(0, 0, width(), height())).boundingRect();
    QFxItem *grabber = static_cast<QFxItem*>(mouseGrabberItem());
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
        grabber = static_cast<QFxItem*>(mouseGrabberItem());
        if (grabber && d->stealMouse && !grabber->keepMouseGrab() && grabber != this)
            grabMouse();

        return d->stealMouse;
    } else if (!d->lastPosTime.isNull()) {
        d->lastPosTime = QTime();
    }
    return false;
}

bool QFxPathView::mouseFilter(QGraphicsSceneMouseEvent *e)
{
    if (!isVisible())
        return false;

    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        {
            bool ret = sendMouseEvent(e);
            if (e->type() == QEvent::GraphicsSceneMouseRelease)
                return ret;
            break;
        }
    default:
        break;
    }

    return false;
}

void QFxPathViewPrivate::regenerate()
{
    Q_Q(QFxPathView);
    for (int i=0; i<items.count(); i++){
        QFxItem *p = items[i];
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
        QFxItem *item = getItem(index);
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

void QFxPathViewPrivate::updateItem(QFxItem *item, qreal percent)
{
    if (QObject *obj = QFxPathView::qmlAttachedProperties(item)) {
        foreach(const QString &attr, path->attributes())
            static_cast<QFxPathViewAttached *>(obj)->setValue(attr.toLatin1(), path->attributeAt(attr, percent));
    }
    QPointF pf = path->pointAt(percent);
    item->setX(pf.x() - item->width()*item->scale()/2);
    item->setY(pf.y() - item->height()*item->scale()/2);
}

void QFxPathView::refill()
{
    Q_D(QFxPathView);
    if (!d->isValid())
        return;

    QList<qreal> positions;
    for (int i=0; i<d->items.count(); i++){
        qreal percent = i * (100. / d->items.count());
        percent = percent + d->_offset;
        percent = fmod(percent,100.);
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
                QFxItem* p = d->items.takeFirst();
                d->updateItem(p, 0.0);
                d->releaseItem(p);
                d->firstIndex++;
                d->firstIndex %= d->model->count();
                int index = (d->firstIndex + d->items.count())%d->model->count();
                QFxItem *item = d->getItem(index);
                item->setZValue(wrapIndex);
                if (d->currentIndex == index)
                    item->setFocus(true);
                d->items << item;
                d->pathOffset++;
                d->pathOffset=d->pathOffset % d->items.count();
            }
        } else {
            while(wrapIndex++ < d->items.count()-1){
                QFxItem* p = d->items.takeLast();
                d->updateItem(p, 1.0);
                d->releaseItem(p);
                d->firstIndex--;
                if (d->firstIndex < 0)
                    d->firstIndex = d->model->count() - 1;
                QFxItem *item = d->getItem(d->firstIndex);
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

void QFxPathView::itemsInserted(int modelIndex, int count)
{
    //XXX support animated insertion
    Q_D(QFxPathView);
    if (!d->isValid())
        return;
    if (d->pathItems == -1) {
        for (int i = 0; i < count; ++i) {
            QFxItem *item = d->getItem(modelIndex + i);
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
    qreal targetOffset = fmod(100 + (d->snapPos*100) - 100.0 * itemIndex / d->items.count(), 100);

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset != d->_offset)
        d->moveOffset.setValue(targetOffset);
}

void QFxPathView::itemsRemoved(int modelIndex, int count)
{
    //XXX support animated removal
    Q_D(QFxPathView);
    if (!d->isValid())
        return;
    if (d->pathItems == -1) {
        for (int i = 0; i < count; ++i) {
            QFxItem* p = d->items.takeAt(modelIndex);
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
    qreal targetOffset = fmod(100 + (d->snapPos*100) - 100.0 * itemIndex / d->items.count(), 100);

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset != d->_offset)
        d->moveOffset.setValue(targetOffset);
}

void QFxPathView::createdItem(int index, QFxItem *item)
{
    Q_D(QFxPathView);
    if (d->requestedIndex != index) {
        item->setParentItem(this);
        d->updateItem(item, index < d->firstIndex ? 0.0 : 1.0);
    }
}

void QFxPathView::destroyingItem(QFxItem *item)
{
    Q_UNUSED(item);
}

void QFxPathView::ticked()
{
    Q_D(QFxPathView);
    d->updateCurrent();
}

// find the item closest to the snap position
int QFxPathViewPrivate::calcCurrentIndex()
{
    int current = -1;
    if (model && items.count()) {
        _offset = fmod(_offset, 100.0);
        if (_offset < 0)
            _offset += 100.0;

        if (pathItems == -1) {
            qreal delta = fmod(_offset - snapPos, 100.0);
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
                percent = fmod(percent,100.);
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

void QFxPathViewPrivate::updateCurrent()
{
    Q_Q(QFxPathView);
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

void QFxPathViewPrivate::fixOffset()
{
    Q_Q(QFxPathView);
    if (model && items.count()) {
        int curr = calcCurrentIndex();
        if (curr != currentIndex)
            q->setCurrentIndex(curr);
        else
            snapToCurrent();
    }
}

void QFxPathViewPrivate::snapToCurrent()
{
    if (!model || model->count() <= 0)
        return;

    int itemIndex = (currentIndex - firstIndex + model->count()) % model->count();

    //Rounds is the number of times round to make the current item visible
    int rounds = itemIndex / items.count();
    int otherWayRounds = (model->count() - (itemIndex)) / items.count() + 1;
    if (otherWayRounds < rounds)
        rounds = -otherWayRounds;

    itemIndex += pathOffset;
    itemIndex %= items.count();
    qreal targetOffset = fmod(100 + (snapPos*100) - 100.0 * itemIndex / items.count(), 100);

    if (targetOffset < 0)
        targetOffset = 100.0 + targetOffset;
    if (targetOffset == _offset && rounds == 0)
        return;

    moveReason = Other;
    tl.clear();
    moveOffset.setValue(_offset);

    if (rounds!=0){
        //Compensate if the targetOffset would bring the target it from off the screen
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

QHash<QObject*, QObject*> QFxPathView::attachedProperties;
QObject *QFxPathView::qmlAttachedProperties(QObject *obj)
{
    QObject *rv = attachedProperties.value(obj);
    if (!rv) {
        rv = new QFxPathViewAttached(obj);
        attachedProperties.insert(obj, rv);
    }
    return rv;
}

QT_END_NAMESPACE

#include "qfxpathview.moc"
