// Commit: ac704e9f682378a5ec56e3f5c195dcf2f2dfa1ac
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

#include "qsgpathview_p.h"
#include "qsgpathview_p_p.h"
#include "qsgcanvas.h"

#include <private/qdeclarativestate_p.h>
#include <private/qdeclarativeopenmetaobject_p.h>
#include <private/qlistmodelinterface_p.h>

#include <QtGui/qevent.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qapplication.h>
#include <QtCore/qmath.h>
#include <math.h>

QT_BEGIN_NAMESPACE

inline qreal qmlMod(qreal x, qreal y)
{
#ifdef QT_USE_MATH_H_FLOATS
    if(sizeof(qreal) == sizeof(float))
        return fmodf(float(x), float(y));
    else
#endif
        return fmod(x, y);
}

static QDeclarativeOpenMetaObjectType *qPathViewAttachedType = 0;

QSGPathViewAttached::QSGPathViewAttached(QObject *parent)
: QObject(parent), m_percent(-1), m_view(0), m_onPath(false), m_isCurrent(false)
{
    if (qPathViewAttachedType) {
        m_metaobject = new QDeclarativeOpenMetaObject(this, qPathViewAttachedType);
        m_metaobject->setCached(true);
    } else {
        m_metaobject = new QDeclarativeOpenMetaObject(this);
    }
}

QSGPathViewAttached::~QSGPathViewAttached()
{
}

QVariant QSGPathViewAttached::value(const QByteArray &name) const
{
    return m_metaobject->value(name);
}
void QSGPathViewAttached::setValue(const QByteArray &name, const QVariant &val)
{
    m_metaobject->setValue(name, val);
}


void QSGPathViewPrivate::init()
{
    Q_Q(QSGPathView);
    offset = 0;
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFlag(QSGItem::ItemIsFocusScope);
    q->setFiltersChildMouseEvents(true);
    q->connect(&tl, SIGNAL(updated()), q, SLOT(ticked()));
    lastPosTime.invalidate();
    static int timelineCompletedIdx = -1;
    static int movementEndingIdx = -1;
    if (timelineCompletedIdx == -1) {
        timelineCompletedIdx = QDeclarativeTimeLine::staticMetaObject.indexOfSignal("completed()");
        movementEndingIdx = QSGPathView::staticMetaObject.indexOfSlot("movementEnding()");
    }
    QMetaObject::connect(&tl, timelineCompletedIdx,
                         q, movementEndingIdx, Qt::DirectConnection);
}

QSGItem *QSGPathViewPrivate::getItem(int modelIndex)
{
    Q_Q(QSGPathView);
    requestedIndex = modelIndex;
    QSGItem *item = model->item(modelIndex, false);
    if (item) {
        if (!attType) {
            // pre-create one metatype to share with all attached objects
            attType = new QDeclarativeOpenMetaObjectType(&QSGPathViewAttached::staticMetaObject, qmlEngine(q));
            foreach(const QString &attr, path->attributes())
                attType->createProperty(attr.toUtf8());
        }
        qPathViewAttachedType = attType;
        QSGPathViewAttached *att = static_cast<QSGPathViewAttached *>(qmlAttachedPropertiesObject<QSGPathView>(item));
        qPathViewAttachedType = 0;
        if (att) {
            att->m_view = q;
            att->setOnPath(true);
        }
        item->setParentItem(q);
        QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
        itemPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
    }
    requestedIndex = -1;
    return item;
}

void QSGPathViewPrivate::releaseItem(QSGItem *item)
{
    if (!item || !model)
        return;
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
    itemPrivate->removeItemChangeListener(this, QSGItemPrivate::Geometry);
    if (model->release(item) == 0) {
        // item was not destroyed, and we no longer reference it.
        if (QSGPathViewAttached *att = attached(item))
            att->setOnPath(false);
    }
}

QSGPathViewAttached *QSGPathViewPrivate::attached(QSGItem *item)
{
    return static_cast<QSGPathViewAttached *>(qmlAttachedPropertiesObject<QSGPathView>(item, false));
}

void QSGPathViewPrivate::clear()
{
    for (int i=0; i<items.count(); i++){
        QSGItem *p = items[i];
        releaseItem(p);
    }
    items.clear();
}

void QSGPathViewPrivate::updateMappedRange()
{
    if (model && pathItems != -1 && pathItems < modelCount)
        mappedRange = qreal(pathItems)/modelCount;
    else
        mappedRange = 1.0;
}

qreal QSGPathViewPrivate::positionOfIndex(qreal index) const
{
    qreal pos = -1.0;

    if (model && index >= 0 && index < modelCount) {
        qreal start = 0.0;
        if (haveHighlightRange && highlightRangeMode != QSGPathView::NoHighlightRange)
            start = highlightRangeStart;
        qreal globalPos = index + offset;
        globalPos = qmlMod(globalPos, qreal(modelCount)) / modelCount;
        if (pathItems != -1 && pathItems < modelCount) {
            globalPos += start * mappedRange;
            globalPos = qmlMod(globalPos, 1.0);
            if (globalPos < mappedRange)
                pos = globalPos / mappedRange;
        } else {
            pos = qmlMod(globalPos + start, 1.0);
        }
    }

    return pos;
}

void QSGPathViewPrivate::createHighlight()
{
    Q_Q(QSGPathView);
    if (!q->isComponentComplete())
        return;

    bool changed = false;
    if (highlightItem) {
        delete highlightItem;
        highlightItem = 0;
        changed = true;
    }

    QSGItem *item = 0;
    if (highlightComponent) {
        QDeclarativeContext *highlightContext = new QDeclarativeContext(qmlContext(q));
        QObject *nobj = highlightComponent->create(highlightContext);
        if (nobj) {
            QDeclarative_setParent_noEvent(highlightContext, nobj);
            item = qobject_cast<QSGItem *>(nobj);
            if (!item)
                delete nobj;
        } else {
            delete highlightContext;
        }
    } else {
        item = new QSGItem;
    }
    if (item) {
        QDeclarative_setParent_noEvent(item, q);
        item->setParentItem(q);
        highlightItem = item;
        changed = true;
    }
    if (changed)
        emit q->highlightItemChanged();
}

void QSGPathViewPrivate::updateHighlight()
{
    Q_Q(QSGPathView);
    if (!q->isComponentComplete() || !isValid())
        return;
    if (highlightItem) {
        if (haveHighlightRange && highlightRangeMode == QSGPathView::StrictlyEnforceRange) {
            updateItem(highlightItem, highlightRangeStart);
        } else {
            qreal target = currentIndex;

            offsetAdj = 0.0;
            tl.reset(moveHighlight);
            moveHighlight.setValue(highlightPosition);

            const int duration = highlightMoveDuration;

            if (target - highlightPosition > modelCount/2) {
                highlightUp = false;
                qreal distance = modelCount - target + highlightPosition;
                tl.move(moveHighlight, 0.0, QEasingCurve(QEasingCurve::InQuad), int(duration * highlightPosition / distance));
                tl.set(moveHighlight, modelCount-0.01);
                tl.move(moveHighlight, target, QEasingCurve(QEasingCurve::OutQuad), int(duration * (modelCount-target) / distance));
            } else if (target - highlightPosition <= -modelCount/2) {
                highlightUp = true;
                qreal distance = modelCount - highlightPosition + target;
                tl.move(moveHighlight, modelCount-0.01, QEasingCurve(QEasingCurve::InQuad), int(duration * (modelCount-highlightPosition) / distance));
                tl.set(moveHighlight, 0.0);
                tl.move(moveHighlight, target, QEasingCurve(QEasingCurve::OutQuad), int(duration * target / distance));
            } else {
                highlightUp = highlightPosition - target < 0;
                tl.move(moveHighlight, target, QEasingCurve(QEasingCurve::InOutQuad), duration);
            }
        }
    }
}

void QSGPathViewPrivate::setHighlightPosition(qreal pos)
{
    if (pos != highlightPosition) {
        qreal start = 0.0;
        qreal end = 1.0;
        if (haveHighlightRange && highlightRangeMode != QSGPathView::NoHighlightRange) {
            start = highlightRangeStart;
            end = highlightRangeEnd;
        }

        qreal range = qreal(modelCount);
        // calc normalized position of highlight relative to offset
        qreal relativeHighlight = qmlMod(pos + offset, range) / range;

        if (!highlightUp && relativeHighlight > end * mappedRange) {
            qreal diff = 1.0 - relativeHighlight;
            setOffset(offset + diff * range);
        } else if (highlightUp && relativeHighlight >= (end - start) * mappedRange) {
            qreal diff = relativeHighlight - (end - start) * mappedRange;
            setOffset(offset - diff * range - 0.00001);
        }

        highlightPosition = pos;
        qreal pathPos = positionOfIndex(pos);
        updateItem(highlightItem, pathPos);
        if (QSGPathViewAttached *att = attached(highlightItem))
            att->setOnPath(pathPos != -1.0);
    }
}

void QSGPathView::pathUpdated()
{
    Q_D(QSGPathView);
    QList<QSGItem*>::iterator it = d->items.begin();
    while (it != d->items.end()) {
        QSGItem *item = *it;
        if (QSGPathViewAttached *att = d->attached(item))
            att->m_percent = -1;
        ++it;
    }
    refill();
}

void QSGPathViewPrivate::updateItem(QSGItem *item, qreal percent)
{
    if (QSGPathViewAttached *att = attached(item)) {
        if (qFuzzyCompare(att->m_percent, percent))
            return;
        att->m_percent = percent;
        foreach(const QString &attr, path->attributes())
            att->setValue(attr.toUtf8(), path->attributeAt(attr, percent));
    }
    QPointF pf = path->pointAt(percent);
    item->setX(qRound(pf.x() - item->width()/2));
    item->setY(qRound(pf.y() - item->height()/2));
}

void QSGPathViewPrivate::regenerate()
{
    Q_Q(QSGPathView);
    if (!q->isComponentComplete())
        return;

    clear();

    if (!isValid())
        return;

    firstIndex = -1;
    updateMappedRange();
    q->refill();
}

QSGPathView::QSGPathView(QSGItem *parent)
  : QSGItem(*(new QSGPathViewPrivate), parent)
{
    Q_D(QSGPathView);
    d->init();
}

QSGPathView::~QSGPathView()
{
    Q_D(QSGPathView);
    d->clear();
    if (d->attType)
        d->attType->release();
    if (d->ownModel)
        delete d->model;
}

QVariant QSGPathView::model() const
{
    Q_D(const QSGPathView);
    return d->modelVariant;
}

void QSGPathView::setModel(const QVariant &model)
{
    Q_D(QSGPathView);
    if (d->modelVariant == model)
        return;

    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        disconnect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        for (int i=0; i<d->items.count(); i++){
            QSGItem *p = d->items[i];
            d->model->release(p);
        }
        d->items.clear();
    }

    d->modelVariant = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QSGVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QSGVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QSGVisualDataModel(qmlContext(this), this);
            d->ownModel = true;
        }
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    d->modelCount = 0;
    if (d->model) {
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        connect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        d->modelCount = d->model->count();
        if (d->model->count())
            d->offset = qmlMod(d->offset, qreal(d->model->count()));
        if (d->offset < 0)
            d->offset = d->model->count() + d->offset;
}
    d->regenerate();
    d->fixOffset();
    emit countChanged();
    emit modelChanged();
}

int QSGPathView::count() const
{
    Q_D(const QSGPathView);
    return d->model ? d->modelCount : 0;
}

QDeclarativePath *QSGPathView::path() const
{
    Q_D(const QSGPathView);
    return d->path;
}

void QSGPathView::setPath(QDeclarativePath *path)
{
    Q_D(QSGPathView);
    if (d->path == path)
        return;
    if (d->path)
        disconnect(d->path, SIGNAL(changed()), this, SLOT(pathUpdated()));
    d->path = path;
    connect(d->path, SIGNAL(changed()), this, SLOT(pathUpdated()));
    if (d->isValid() && isComponentComplete()) {
        d->clear();
        if (d->attType) {
            d->attType->release();
            d->attType = 0;
        }
        d->regenerate();
    }
    emit pathChanged();
}

int QSGPathView::currentIndex() const
{
    Q_D(const QSGPathView);
    return d->currentIndex;
}

void QSGPathView::setCurrentIndex(int idx)
{
    Q_D(QSGPathView);
    if (d->model && d->modelCount)
        idx = qAbs(idx % d->modelCount);
    if (d->model && idx != d->currentIndex) {
        if (d->modelCount) {
            int itemIndex = (d->currentIndex - d->firstIndex + d->modelCount) % d->modelCount;
            if (itemIndex < d->items.count()) {
                if (QSGItem *item = d->items.at(itemIndex)) {
                    if (QSGPathViewAttached *att = d->attached(item))
                        att->setIsCurrentItem(false);
                }
            }
        }
        d->currentItem = 0;
        d->moveReason = QSGPathViewPrivate::SetIndex;
        d->currentIndex = idx;
        if (d->modelCount) {
            if (d->haveHighlightRange && d->highlightRangeMode == QSGPathView::StrictlyEnforceRange)
                d->snapToCurrent();
            int itemIndex = (idx - d->firstIndex + d->modelCount) % d->modelCount;
            if (itemIndex < d->items.count()) {
                d->currentItem = d->items.at(itemIndex);
                d->currentItem->setFocus(true);
                if (QSGPathViewAttached *att = d->attached(d->currentItem))
                    att->setIsCurrentItem(true);
            }
            d->currentItemOffset = d->positionOfIndex(d->currentIndex);
            d->updateHighlight();
        }
        emit currentIndexChanged();
    }
}

void QSGPathView::incrementCurrentIndex()
{
    Q_D(QSGPathView);
    d->moveDirection = QSGPathViewPrivate::Positive;
    setCurrentIndex(currentIndex()+1);
}

void QSGPathView::decrementCurrentIndex()
{
    Q_D(QSGPathView);
    if (d->model && d->modelCount) {
        int idx = currentIndex()-1;
        if (idx < 0)
            idx = d->modelCount - 1;
        d->moveDirection = QSGPathViewPrivate::Negative;
        setCurrentIndex(idx);
    }
}

qreal QSGPathView::offset() const
{
    Q_D(const QSGPathView);
    return d->offset;
}

void QSGPathView::setOffset(qreal offset)
{
    Q_D(QSGPathView);
    d->setOffset(offset);
    d->updateCurrent();
}

void QSGPathViewPrivate::setOffset(qreal o)
{
    Q_Q(QSGPathView);
    if (offset != o) {
        if (isValid() && q->isComponentComplete()) {
            offset = qmlMod(o, qreal(modelCount));
            if (offset < 0)
                offset += qreal(modelCount);
            q->refill();
        } else {
            offset = o;
        }
        emit q->offsetChanged();
    }
}

void QSGPathViewPrivate::setAdjustedOffset(qreal o)
{
    setOffset(o+offsetAdj);
}

QDeclarativeComponent *QSGPathView::highlight() const
{
    Q_D(const QSGPathView);
    return d->highlightComponent;
}

void QSGPathView::setHighlight(QDeclarativeComponent *highlight)
{
    Q_D(QSGPathView);
    if (highlight != d->highlightComponent) {
        d->highlightComponent = highlight;
        d->createHighlight();
        d->updateHighlight();
        emit highlightChanged();
    }
}

QSGItem *QSGPathView::highlightItem()
{
    Q_D(const QSGPathView);
    return d->highlightItem;
}

qreal QSGPathView::preferredHighlightBegin() const
{
    Q_D(const QSGPathView);
    return d->highlightRangeStart;
}

void QSGPathView::setPreferredHighlightBegin(qreal start)
{
    Q_D(QSGPathView);
    if (d->highlightRangeStart == start || start < 0 || start > 1.0)
        return;
    d->highlightRangeStart = start;
    d->haveHighlightRange = d->highlightRangeMode != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    refill();
    emit preferredHighlightBeginChanged();
}

qreal QSGPathView::preferredHighlightEnd() const
{
    Q_D(const QSGPathView);
    return d->highlightRangeEnd;
}

void QSGPathView::setPreferredHighlightEnd(qreal end)
{
    Q_D(QSGPathView);
    if (d->highlightRangeEnd == end || end < 0 || end > 1.0)
        return;
    d->highlightRangeEnd = end;
    d->haveHighlightRange = d->highlightRangeMode != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    refill();
    emit preferredHighlightEndChanged();
}

QSGPathView::HighlightRangeMode QSGPathView::highlightRangeMode() const
{
    Q_D(const QSGPathView);
    return d->highlightRangeMode;
}

void QSGPathView::setHighlightRangeMode(HighlightRangeMode mode)
{
    Q_D(QSGPathView);
    if (d->highlightRangeMode == mode)
        return;
    d->highlightRangeMode = mode;
    d->haveHighlightRange = d->highlightRangeMode != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit highlightRangeModeChanged();
}

int QSGPathView::highlightMoveDuration() const
{
    Q_D(const QSGPathView);
    return d->highlightMoveDuration;
}

void QSGPathView::setHighlightMoveDuration(int duration)
{
    Q_D(QSGPathView);
    if (d->highlightMoveDuration == duration)
        return;
    d->highlightMoveDuration = duration;
    emit highlightMoveDurationChanged();
}

qreal QSGPathView::dragMargin() const
{
    Q_D(const QSGPathView);
    return d->dragMargin;
}

void QSGPathView::setDragMargin(qreal dragMargin)
{
    Q_D(QSGPathView);
    if (d->dragMargin == dragMargin)
        return;
    d->dragMargin = dragMargin;
    emit dragMarginChanged();
}

qreal QSGPathView::flickDeceleration() const
{
    Q_D(const QSGPathView);
    return d->deceleration;
}

void QSGPathView::setFlickDeceleration(qreal dec)
{
    Q_D(QSGPathView);
    if (d->deceleration == dec)
        return;
    d->deceleration = dec;
    emit flickDecelerationChanged();
}

bool QSGPathView::isInteractive() const
{
    Q_D(const QSGPathView);
    return d->interactive;
}

void QSGPathView::setInteractive(bool interactive)
{
    Q_D(QSGPathView);
    if (interactive != d->interactive) {
        d->interactive = interactive;
        if (!interactive)
            d->tl.clear();
        emit interactiveChanged();
    }
}

bool QSGPathView::isMoving() const
{
    Q_D(const QSGPathView);
    return d->moving;
}

bool QSGPathView::isFlicking() const
{
    Q_D(const QSGPathView);
    return d->flicking;
}

QDeclarativeComponent *QSGPathView::delegate() const
{
    Q_D(const QSGPathView);
     if (d->model) {
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QSGPathView::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QSGPathView);
    if (delegate == this->delegate())
        return;
    if (!d->ownModel) {
        d->model = new QSGVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        d->modelCount = dataModel->count();
        d->regenerate();
        emit delegateChanged();
    }
}

int QSGPathView::pathItemCount() const
{
    Q_D(const QSGPathView);
    return d->pathItems;
}

void QSGPathView::setPathItemCount(int i)
{
    Q_D(QSGPathView);
    if (i == d->pathItems)
        return;
    if (i < 1)
        i = 1;
    d->pathItems = i;
    d->updateMappedRange();
    if (d->isValid() && isComponentComplete()) {
        d->regenerate();
    }
    emit pathItemCountChanged();
}

QPointF QSGPathViewPrivate::pointNear(const QPointF &point, qreal *nearPercent) const
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
        *nearPercent = nearPc / 1000.0;

    return nearPoint;
}

void QSGPathView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPathView);
    if (d->interactive) {
        d->handleMousePressEvent(event);
        event->accept();
    } else {
        QSGItem::mousePressEvent(event);
    }
}

void QSGPathViewPrivate::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGPathView);
    if (!interactive || !items.count())
        return;
    QPointF scenePoint = q->mapToScene(event->pos());
    int idx = 0;
    for (; idx < items.count(); ++idx) {
        QRectF rect = items.at(idx)->boundingRect();
        rect = items.at(idx)->mapRectToScene(rect);
        if (rect.contains(scenePoint))
            break;
    }
    if (idx == items.count() && dragMargin == 0.)  // didn't click on an item
        return;

    startPoint = pointNear(event->pos(), &startPc);
    if (idx == items.count()) {
        qreal distance = qAbs(event->pos().x() - startPoint.x()) + qAbs(event->pos().y() - startPoint.y());
        if (distance > dragMargin)
            return;
    }

    if (tl.isActive() && flicking)
        stealMouse = true; // If we've been flicked then steal the click.
    else
        stealMouse = false;

    lastElapsed = 0;
    lastDist = 0;
    QSGItemPrivate::start(lastPosTime);
    tl.clear();
}

void QSGPathView::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPathView);
    if (d->interactive) {
        d->handleMouseMoveEvent(event);
        if (d->stealMouse)
            setKeepMouseGrab(true);
        event->accept();
    } else {
        QSGItem::mouseMoveEvent(event);
    }
}

void QSGPathViewPrivate::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Q_Q(QSGPathView);
    if (!interactive || !lastPosTime.isValid())
        return;

    qreal newPc;
    QPointF pathPoint = pointNear(event->pos(), &newPc);
    if (!stealMouse) {
        QPointF delta = pathPoint - startPoint;
        if (qAbs(delta.x()) > QApplication::startDragDistance() || qAbs(delta.y()) > QApplication::startDragDistance()) {
            stealMouse = true;
            startPc = newPc;
        }
    }

    if (stealMouse) {
        moveReason = QSGPathViewPrivate::Mouse;
        qreal diff = (newPc - startPc)*modelCount*mappedRange;
        if (diff) {
            q->setOffset(offset + diff);

            if (diff > modelCount/2)
                diff -= modelCount;
            else if (diff < -modelCount/2)
                diff += modelCount;

            lastElapsed = QSGItemPrivate::restart(lastPosTime);
            lastDist = diff;
            startPc = newPc;
        }
        if (!moving) {
            moving = true;
            emit q->movingChanged();
            emit q->movementStarted();
        }
    }
}

void QSGPathView::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPathView);
    if (d->interactive) {
        d->handleMouseReleaseEvent(event);
        event->accept();
        ungrabMouse();
    } else {
        QSGItem::mouseReleaseEvent(event);
    }
}

void QSGPathViewPrivate::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    Q_Q(QSGPathView);
    stealMouse = false;
    q->setKeepMouseGrab(false);
    if (!interactive || !lastPosTime.isValid())
        return;

    qreal elapsed = qreal(lastElapsed + QSGItemPrivate::elapsed(lastPosTime)) / 1000.;
    qreal velocity = elapsed > 0. ? lastDist / elapsed : 0;
    if (model && modelCount && qAbs(velocity) > 1.) {
        qreal count = pathItems == -1 ? modelCount : pathItems;
        if (qAbs(velocity) > count * 2) // limit velocity
            velocity = (velocity > 0 ? count : -count) * 2;
        // Calculate the distance to be travelled
        qreal v2 = velocity*velocity;
        qreal accel = deceleration/10;
        // + 0.25 to encourage moving at least one item in the flick direction
        qreal dist = qMin(qreal(modelCount-1), qreal(v2 / (accel * 2.0) + 0.25));
        if (haveHighlightRange && highlightRangeMode == QSGPathView::StrictlyEnforceRange) {
            // round to nearest item.
            if (velocity > 0.)
                dist = qRound(dist + offset) - offset;
            else
                dist = qRound(dist - offset) + offset;
            // Calculate accel required to stop on item boundary
            if (dist <= 0.) {
                dist = 0.;
                accel = 0.;
            } else {
                accel = v2 / (2.0f * qAbs(dist));
            }
        }
        offsetAdj = 0.0;
        moveOffset.setValue(offset);
        tl.accel(moveOffset, velocity, accel, dist);
        tl.callback(QDeclarativeTimeLineCallback(&moveOffset, fixOffsetCallback, this));
        if (!flicking) {
            flicking = true;
            emit q->flickingChanged();
            emit q->flickStarted();
        }
    } else {
        fixOffset();
    }

    lastPosTime.invalidate();
    if (!tl.isActive())
        q->movementEnding();
}

bool QSGPathView::sendMouseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_D(QSGPathView);
    QGraphicsSceneMouseEvent mouseEvent(event->type());
    QRectF myRect = mapRectToScene(QRectF(0, 0, width(), height()));
    QSGCanvas *c = canvas();
    QSGItem *grabber = c ? c->mouseGrabberItem() : 0;
    bool stealThisEvent = d->stealMouse;
    if ((stealThisEvent || myRect.contains(event->scenePos().toPoint())) && (!grabber || !grabber->keepMouseGrab())) {
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
            d->handleMouseMoveEvent(&mouseEvent);
            break;
        case QEvent::GraphicsSceneMousePress:
            d->handleMousePressEvent(&mouseEvent);
            stealThisEvent = d->stealMouse;   // Update stealThisEvent in case changed by function call above
            break;
        case QEvent::GraphicsSceneMouseRelease:
            d->handleMouseReleaseEvent(&mouseEvent);
            break;
        default:
            break;
        }
        grabber = c->mouseGrabberItem();
        if (grabber && stealThisEvent && !grabber->keepMouseGrab() && grabber != this)
            grabMouse();

        return d->stealMouse;
    } else if (d->lastPosTime.isValid()) {
        d->lastPosTime.invalidate();
    }
    if (mouseEvent.type() == QEvent::GraphicsSceneMouseRelease)
        d->stealMouse = false;
    return false;
}

bool QSGPathView::childMouseEventFilter(QSGItem *i, QEvent *e)
{
    Q_D(QSGPathView);
    if (!isVisible() || !d->interactive)
        return QSGItem::childMouseEventFilter(i, e);

    switch (e->type()) {
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMouseRelease:
        return sendMouseEvent(static_cast<QGraphicsSceneMouseEvent *>(e));
    default:
        break;
    }

    return QSGItem::childMouseEventFilter(i, e);
}

void QSGPathView::updatePolish()
{
    QSGItem::updatePolish();
    refill();
}

void QSGPathView::componentComplete()
{
    Q_D(QSGPathView);
    QSGItem::componentComplete();
    d->createHighlight();
    // It is possible that a refill has already happended to to Path
    // bindings being handled in the componentComplete().  If so
    // don't do it again.
    if (d->items.count() == 0 && d->model) {
        d->modelCount = d->model->count();
        d->regenerate();
    }
    d->updateHighlight();
}

void QSGPathView::refill()
{
    Q_D(QSGPathView);
    if (!d->isValid() || !isComponentComplete())
        return;

    d->layoutScheduled = false;
    bool currentVisible = false;

    // first move existing items and remove items off path
    int idx = d->firstIndex;
    QList<QSGItem*>::iterator it = d->items.begin();
    while (it != d->items.end()) {
        qreal pos = d->positionOfIndex(idx);
        QSGItem *item = *it;
        if (pos >= 0.0) {
            d->updateItem(item, pos);
            if (idx == d->currentIndex) {
                currentVisible = true;
                d->currentItemOffset = pos;
            }
            ++it;
        } else {
//            qDebug() << "release";
            d->updateItem(item, 1.0);
            d->releaseItem(item);
            if (it == d->items.begin()) {
                if (++d->firstIndex >= d->modelCount)
                    d->firstIndex = 0;
            }
            it = d->items.erase(it);
        }
        ++idx;
        if (idx >= d->modelCount)
            idx = 0;
    }
    if (!d->items.count())
        d->firstIndex = -1;

    if (d->modelCount) {
        // add items to beginning and end
        int count = d->pathItems == -1 ? d->modelCount : qMin(d->pathItems, d->modelCount);
        if (d->items.count() < count) {
            int idx = qRound(d->modelCount - d->offset) % d->modelCount;
            qreal startPos = 0.0;
            if (d->haveHighlightRange && d->highlightRangeMode != QSGPathView::NoHighlightRange)
                startPos = d->highlightRangeStart;
            if (d->firstIndex >= 0) {
                startPos = d->positionOfIndex(d->firstIndex);
                idx = (d->firstIndex + d->items.count()) % d->modelCount;
            }
            qreal pos = d->positionOfIndex(idx);
            while ((pos > startPos || !d->items.count()) && d->items.count() < count) {
    //            qDebug() << "append" << idx;
                QSGItem *item = d->getItem(idx);
                if (d->model->completePending())
                    item->setZ(idx+1);
                if (d->currentIndex == idx) {
                    item->setFocus(true);
                    if (QSGPathViewAttached *att = d->attached(item))
                        att->setIsCurrentItem(true);
                    currentVisible = true;
                    d->currentItemOffset = pos;
                    d->currentItem = item;
                }
                if (d->items.count() == 0)
                    d->firstIndex = idx;
                d->items.append(item);
                d->updateItem(item, pos);
                if (d->model->completePending())
                    d->model->completeItem();
                ++idx;
                if (idx >= d->modelCount)
                    idx = 0;
                pos = d->positionOfIndex(idx);
            }

            idx = d->firstIndex - 1;
            if (idx < 0)
                idx = d->modelCount - 1;
            pos = d->positionOfIndex(idx);
            while (pos >= 0.0 && pos < startPos) {
    //            qDebug() << "prepend" << idx;
                QSGItem *item = d->getItem(idx);
                if (d->model->completePending())
                    item->setZ(idx+1);
                if (d->currentIndex == idx) {
                    item->setFocus(true);
                    if (QSGPathViewAttached *att = d->attached(item))
                        att->setIsCurrentItem(true);
                    currentVisible = true;
                    d->currentItemOffset = pos;
                    d->currentItem = item;
                }
                d->items.prepend(item);
                d->updateItem(item, pos);
                if (d->model->completePending())
                    d->model->completeItem();
                d->firstIndex = idx;
                idx = d->firstIndex - 1;
                if (idx < 0)
                    idx = d->modelCount - 1;
                pos = d->positionOfIndex(idx);
            }
        }
    }

    if (!currentVisible)
        d->currentItemOffset = 1.0;

    if (d->highlightItem && d->haveHighlightRange && d->highlightRangeMode == QSGPathView::StrictlyEnforceRange) {
        d->updateItem(d->highlightItem, d->highlightRangeStart);
        if (QSGPathViewAttached *att = d->attached(d->highlightItem))
            att->setOnPath(true);
    } else if (d->highlightItem && d->moveReason != QSGPathViewPrivate::SetIndex) {
        d->updateItem(d->highlightItem, d->currentItemOffset);
        if (QSGPathViewAttached *att = d->attached(d->highlightItem))
            att->setOnPath(currentVisible);
    }
    while (d->itemCache.count())
        d->releaseItem(d->itemCache.takeLast());
}

void QSGPathView::itemsInserted(int modelIndex, int count)
{
    //XXX support animated insertion
    Q_D(QSGPathView);
    if (!d->isValid() || !isComponentComplete())
        return;

    if (d->modelCount) {
        d->itemCache += d->items;
        d->items.clear();
        if (modelIndex <= d->currentIndex) {
            d->currentIndex += count;
            emit currentIndexChanged();
        } else if (d->offset != 0) {
            d->offset += count;
            d->offsetAdj += count;
        }
    }

    d->modelCount += count;
    if (d->flicking || d->moving) {
        d->regenerate();
        d->updateCurrent();
    } else {
        d->firstIndex = -1;
        d->updateMappedRange();
        d->scheduleLayout();
    }
    emit countChanged();
}

void QSGPathView::itemsRemoved(int modelIndex, int count)
{
    //XXX support animated removal
    Q_D(QSGPathView);
    if (!d->model || !d->modelCount || !d->model->isValid() || !d->path || !isComponentComplete())
        return;

    // fix current
    bool currentChanged = false;
    if (d->currentIndex >= modelIndex + count) {
        d->currentIndex -= count;
        currentChanged = true;
    } else if (d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count) {
        // current item has been removed.
        d->currentIndex = qMin(modelIndex, d->modelCount-count-1);
        if (d->currentItem) {
            if (QSGPathViewAttached *att = d->attached(d->currentItem))
                att->setIsCurrentItem(true);
        }
        currentChanged = true;
    }

    d->itemCache += d->items;
    d->items.clear();

    bool changedOffset = false;
    if (modelIndex > d->currentIndex) {
        if (d->offset >= count) {
            changedOffset = true;
            d->offset -= count;
            d->offsetAdj -= count;
        }
    }

    d->modelCount -= count;
    if (!d->modelCount) {
        while (d->itemCache.count())
            d->releaseItem(d->itemCache.takeLast());
        d->offset = 0;
        changedOffset = true;
        d->tl.reset(d->moveOffset);
    } else {
        d->regenerate();
        d->updateCurrent();
    }
    if (changedOffset)
        emit offsetChanged();
    if (currentChanged)
        emit currentIndexChanged();
    emit countChanged();
}

void QSGPathView::itemsMoved(int /*from*/, int /*to*/, int /*count*/)
{
    Q_D(QSGPathView);
    if (!d->isValid() || !isComponentComplete())
        return;

    QList<QSGItem *> removedItems = d->items;
    d->items.clear();
    d->regenerate();
    while (removedItems.count())
        d->releaseItem(removedItems.takeLast());

    // Fix current index
    if (d->currentIndex >= 0 && d->currentItem) {
        int oldCurrent = d->currentIndex;
        d->currentIndex = d->model->indexOf(d->currentItem, this);
        if (oldCurrent != d->currentIndex)
            emit currentIndexChanged();
    }
    d->updateCurrent();
}

void QSGPathView::modelReset()
{
    Q_D(QSGPathView);
    d->modelCount = d->model->count();
    d->regenerate();
    emit countChanged();
}

void QSGPathView::createdItem(int index, QSGItem *item)
{
    Q_D(QSGPathView);
    if (d->requestedIndex != index) {
        if (!d->attType) {
            // pre-create one metatype to share with all attached objects
            d->attType = new QDeclarativeOpenMetaObjectType(&QSGPathViewAttached::staticMetaObject, qmlEngine(this));
            foreach(const QString &attr, d->path->attributes())
                d->attType->createProperty(attr.toUtf8());
        }
        qPathViewAttachedType = d->attType;
        QSGPathViewAttached *att = static_cast<QSGPathViewAttached *>(qmlAttachedPropertiesObject<QSGPathView>(item));
        qPathViewAttachedType = 0;
        if (att) {
            att->m_view = this;
            att->setOnPath(false);
        }
        item->setParentItem(this);
        d->updateItem(item, index < d->firstIndex ? 0.0 : 1.0);
    }
}

void QSGPathView::destroyingItem(QSGItem *item)
{
    Q_UNUSED(item);
}

void QSGPathView::ticked()
{
    Q_D(QSGPathView);
    d->updateCurrent();
}

void QSGPathView::movementEnding()
{
    Q_D(QSGPathView);
    if (d->flicking) {
        d->flicking = false;
        emit flickingChanged();
        emit flickEnded();
    }
    if (d->moving && !d->stealMouse) {
        d->moving = false;
        emit movingChanged();
        emit movementEnded();
    }
}

// find the item closest to the snap position
int QSGPathViewPrivate::calcCurrentIndex()
{
    int current = -1;
    if (modelCount && model && items.count()) {
        offset = qmlMod(offset, modelCount);
        if (offset < 0)
            offset += modelCount;
        current = qRound(qAbs(qmlMod(modelCount - offset, modelCount)));
        current = current % modelCount;
    }

    return current;
}

void QSGPathViewPrivate::updateCurrent()
{
    Q_Q(QSGPathView);
    if (moveReason != Mouse)
        return;
    if (!modelCount || !haveHighlightRange || highlightRangeMode != QSGPathView::StrictlyEnforceRange)
        return;

    int idx = calcCurrentIndex();
    if (model && idx != currentIndex) {
        int itemIndex = (currentIndex - firstIndex + modelCount) % modelCount;
        if (itemIndex < items.count()) {
            if (QSGItem *item = items.at(itemIndex)) {
                if (QSGPathViewAttached *att = attached(item))
                    att->setIsCurrentItem(false);
            }
        }
        currentIndex = idx;
        currentItem = 0;
        itemIndex = (idx - firstIndex + modelCount) % modelCount;
        if (itemIndex < items.count()) {
            currentItem = items.at(itemIndex);
            currentItem->setFocus(true);
            if (QSGPathViewAttached *att = attached(currentItem))
                att->setIsCurrentItem(true);
        }
        emit q->currentIndexChanged();
    }
}

void QSGPathViewPrivate::fixOffsetCallback(void *d)
{
    ((QSGPathViewPrivate *)d)->fixOffset();
}

void QSGPathViewPrivate::fixOffset()
{
    Q_Q(QSGPathView);
    if (model && items.count()) {
        if (haveHighlightRange && highlightRangeMode == QSGPathView::StrictlyEnforceRange) {
            int curr = calcCurrentIndex();
            if (curr != currentIndex)
                q->setCurrentIndex(curr);
            else
                snapToCurrent();
        }
    }
}

void QSGPathViewPrivate::snapToCurrent()
{
    if (!model || modelCount <= 0)
        return;

    qreal targetOffset = qmlMod(modelCount - currentIndex, modelCount);

    moveReason = Other;
    offsetAdj = 0.0;
    tl.reset(moveOffset);
    moveOffset.setValue(offset);

    const int duration = highlightMoveDuration;

    if (moveDirection == Positive || (moveDirection == Shortest && targetOffset - offset > modelCount/2)) {
        qreal distance = modelCount - targetOffset + offset;
        if (targetOffset > moveOffset) {
            tl.move(moveOffset, 0.0, QEasingCurve(QEasingCurve::InQuad), int(duration * offset / distance));
            tl.set(moveOffset, modelCount);
            tl.move(moveOffset, targetOffset, QEasingCurve(offset == 0.0 ? QEasingCurve::InOutQuad : QEasingCurve::OutQuad), int(duration * (modelCount-targetOffset) / distance));
        } else {
            tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InOutQuad), duration);
        }
    } else if (moveDirection == Negative || targetOffset - offset <= -modelCount/2) {
        qreal distance = modelCount - offset + targetOffset;
        if (targetOffset < moveOffset) {
            tl.move(moveOffset, modelCount, QEasingCurve(targetOffset == 0 ? QEasingCurve::InOutQuad : QEasingCurve::InQuad), int(duration * (modelCount-offset) / distance));
            tl.set(moveOffset, 0.0);
            tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::OutQuad), int(duration * targetOffset / distance));
        } else {
            tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InOutQuad), duration);
        }
    } else {
        tl.move(moveOffset, targetOffset, QEasingCurve(QEasingCurve::InOutQuad), duration);
    }
    moveDirection = Shortest;
}

QSGPathViewAttached *QSGPathView::qmlAttachedProperties(QObject *obj)
{
    return new QSGPathViewAttached(obj);
}

QT_END_NAMESPACE

