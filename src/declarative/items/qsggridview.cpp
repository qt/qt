// Commit: cc6408ccd5453d1bed9f98b9caa14861cea5742b
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

#include "qsggridview_p.h"
#include "qsgvisualitemmodel_p.h"
#include "qsgflickable_p_p.h"

#include <private/qdeclarativesmoothedanimation_p_p.h>
#include <private/qlistmodelinterface_p.h>

#include <QtGui/qevent.h>
#include <QtCore/qmath.h>
#include <QtCore/qcoreapplication.h>
#include <math.h>

QT_BEGIN_NAMESPACE

//----------------------------------------------------------------------------

class FxGridItemSG
{
public:
    FxGridItemSG(QSGItem *i, QSGGridView *v) : item(i), view(v) {
        attached = static_cast<QSGGridViewAttached*>(qmlAttachedPropertiesObject<QSGGridView>(item));
        if (attached)
            attached->setView(view);
    }
    ~FxGridItemSG() {}

    qreal rowPos() const {
        qreal rowPos = 0;
        if (view->flow() == QSGGridView::LeftToRight) {
            rowPos = item->y();
        } else {
            if (view->effectiveLayoutDirection() == Qt::RightToLeft)
                rowPos = -view->cellWidth()-item->x();
            else
                rowPos = item->x();
        }
        return rowPos;
    }
    qreal colPos() const {
        qreal colPos = 0;
        if (view->flow() == QSGGridView::LeftToRight) {
            if (view->effectiveLayoutDirection() == Qt::RightToLeft) {
                int colSize = view->cellWidth();
                int columns = view->width()/colSize;
                colPos = colSize * (columns-1) - item->x();
            } else {
                colPos = item->x();
            }
        } else {
            colPos = item->y();
        }

        return colPos;
    }
    qreal endRowPos() const {
        if (view->flow() == QSGGridView::LeftToRight) {
            return item->y() + view->cellHeight() - 1;
        } else {
            if (view->effectiveLayoutDirection() == Qt::RightToLeft)
                return -item->x() - 1;
            else
                return item->x() + view->cellWidth() - 1;
        }
    }
    void setPosition(qreal col, qreal row) {
        if (view->effectiveLayoutDirection() == Qt::RightToLeft) {
            if (view->flow() == QSGGridView::LeftToRight) {
                int columns = view->width()/view->cellWidth();
                item->setPos(QPointF((view->cellWidth() * (columns-1) - col), row));
            } else {
                item->setPos(QPointF(-view->cellWidth()-row, col));
            }
        } else {
            if (view->flow() == QSGGridView::LeftToRight)
                item->setPos(QPointF(col, row));
            else
                item->setPos(QPointF(row, col));
        }
    }
    bool contains(qreal x, qreal y) const {
        return (x >= item->x() && x < item->x() + view->cellWidth() &&
                y >= item->y() && y < item->y() + view->cellHeight());
    }

    QSGItem *item;
    QSGGridView *view;
    QSGGridViewAttached *attached;
    int index;
};

//----------------------------------------------------------------------------

class QSGGridViewPrivate : public QSGFlickablePrivate
{
    Q_DECLARE_PUBLIC(QSGGridView)

public:
    QSGGridViewPrivate()
    : currentItem(0), layoutDirection(Qt::LeftToRight), flow(QSGGridView::LeftToRight)
    , visibleIndex(0) , currentIndex(-1)
    , cellWidth(100), cellHeight(100), columns(1), requestedIndex(-1), itemCount(0)
    , highlightRangeStart(0), highlightRangeEnd(0)
    , highlightRange(QSGGridView::NoHighlightRange)
    , highlightComponent(0), highlight(0), trackedItem(0)
    , moveReason(Other), buffer(0), highlightXAnimator(0), highlightYAnimator(0)
    , highlightMoveDuration(150)
    , footerComponent(0), footer(0), headerComponent(0), header(0)
    , bufferMode(BufferBefore | BufferAfter), snapMode(QSGGridView::NoSnap)
    , ownModel(false), wrap(false), autoHighlight(true)
    , fixCurrentVisibility(false), lazyRelease(false), layoutScheduled(false)
    , deferredRelease(false), haveHighlightRange(false), currentIndexCleared(false)
    , highlightRangeStartValid(false), highlightRangeEndValid(false) {}

    void init();
    void clear();
    FxGridItemSG *createItem(int modelIndex);
    void releaseItem(FxGridItemSG *item);
    void refill(qreal from, qreal to, bool doBuffer=false);

    void updateGrid();
    void scheduleLayout();
    void layout();
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateTrackedItem();
    void createHighlight();
    void updateHighlight();
    void updateCurrent(int modelIndex);
    void updateHeader();
    void updateFooter();
    void fixupPosition();

    FxGridItemSG *visibleItem(int modelIndex) const {
        if (modelIndex >= visibleIndex && modelIndex < visibleIndex + visibleItems.count()) {
            for (int i = modelIndex - visibleIndex; i < visibleItems.count(); ++i) {
                FxGridItemSG *item = visibleItems.at(i);
                if (item->index == modelIndex)
                    return item;
            }
        }
        return 0;
    }

    bool isRightToLeftTopToBottom() const {
        Q_Q(const QSGGridView);
        return flow == QSGGridView::TopToBottom && q->effectiveLayoutDirection() == Qt::RightToLeft;
    }

    void regenerate() {
        Q_Q(QSGGridView);
        if (q->isComponentComplete()) {
            clear();
            updateGrid();
            setPosition(0);
            q->refill();
            updateCurrent(currentIndex);
        }
    }

    void mirrorChange() {
        Q_Q(QSGGridView);
        regenerate();
        emit q->effectiveLayoutDirectionChanged();
    }

    qreal position() const {
        Q_Q(const QSGGridView);
        return flow == QSGGridView::LeftToRight ? q->contentY() : q->contentX();
    }
    void setPosition(qreal pos) {
        Q_Q(QSGGridView);
        if (flow == QSGGridView::LeftToRight) {
            q->QSGFlickable::setContentY(pos);
            q->QSGFlickable::setContentX(0);
        } else {
            if (q->effectiveLayoutDirection() == Qt::LeftToRight)
                q->QSGFlickable::setContentX(pos);
            else
                q->QSGFlickable::setContentX(-pos-size());
            q->QSGFlickable::setContentY(0);
        }
    }
    int size() const {
        Q_Q(const QSGGridView);
        return flow == QSGGridView::LeftToRight ? q->height() : q->width();
    }
    qreal originPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty())
            pos = visibleItems.first()->rowPos() - visibleIndex / columns * rowSize();
        return pos;
    }

    qreal lastPosition() const {
        qreal pos = 0;
        if (model && model->count())
            pos = rowPosAt(model->count() - 1) + rowSize();
        return pos;
    }

    qreal startPosition() const {
        return isRightToLeftTopToBottom() ? -lastPosition()+1 : originPosition();
    }

    qreal endPosition() const {
        return isRightToLeftTopToBottom() ? -originPosition()+1 : lastPosition();

    }

    bool isValid() const {
        return model && model->count() && model->isValid();
    }

    int rowSize() const {
        return flow == QSGGridView::LeftToRight ? cellHeight : cellWidth;
    }
    int colSize() const {
        return flow == QSGGridView::LeftToRight ? cellWidth : cellHeight;
    }

    qreal colPosAt(int modelIndex) const {
        if (FxGridItemSG *item = visibleItem(modelIndex))
            return item->colPos();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int count = (visibleIndex - modelIndex) % columns;
                int col = visibleItems.first()->colPos() / colSize();
                col = (columns - count + col) % columns;
                return col * colSize();
            } else {
                int count = columns - 1 - (modelIndex - visibleItems.last()->index - 1) % columns;
                return visibleItems.last()->colPos() - count * colSize();
            }
        } else {
            return (modelIndex % columns) * colSize();
        }
        return 0;
    }
    qreal rowPosAt(int modelIndex) const {
        if (FxGridItemSG *item = visibleItem(modelIndex))
            return item->rowPos();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int firstCol = visibleItems.first()->colPos() / colSize();
                int col = visibleIndex - modelIndex + (columns - firstCol - 1);
                int rows = col / columns;
                return visibleItems.first()->rowPos() - rows * rowSize();
            } else {
                int count = modelIndex - visibleItems.last()->index;
                int col = visibleItems.last()->colPos() + count * colSize();
                int rows = col / (columns * colSize());
                return visibleItems.last()->rowPos() + rows * rowSize();
            }
        } else {
            qreal pos = (modelIndex / columns) * rowSize();
            if (header)
                pos += headerSize();
            return pos;
        }
        return 0;
    }

    FxGridItemSG *firstVisibleItem() const {
        const qreal pos = isRightToLeftTopToBottom() ? -position()-size() : position();
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *item = visibleItems.at(i);
            if (item->index != -1 && item->endRowPos() > pos)
                return item;
        }
        return visibleItems.count() ? visibleItems.first() : 0;
    }

    int lastVisibleIndex() const {
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *item = visibleItems.at(i);
            if (item->index != -1)
                return item->index;
        }
        return -1;
    }

    // Map a model index to visibleItems list index.
    // These may differ if removed items are still present in the visible list,
    // e.g. doing a removal animation
    int mapFromModel(int modelIndex) const {
        if (modelIndex < visibleIndex || modelIndex >= visibleIndex + visibleItems.count())
            return -1;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *listItem = visibleItems.at(i);
            if (listItem->index == modelIndex)
                return i + visibleIndex;
            if (listItem->index > modelIndex)
                return -1;
        }
        return -1; // Not in visibleList
    }

    qreal snapPosAt(qreal pos) const {
        Q_Q(const QSGGridView);
        qreal snapPos = 0;
        if (!visibleItems.isEmpty()) {
            pos += rowSize()/2;
            snapPos = visibleItems.first()->rowPos() - visibleIndex / columns * rowSize();
            snapPos = pos - fmodf(pos - snapPos, qreal(rowSize()));
            qreal maxExtent;
            qreal minExtent;
            if (isRightToLeftTopToBottom()) {
                maxExtent = q->minXExtent();
                minExtent = q->maxXExtent();
            } else {
                maxExtent = flow == QSGGridView::LeftToRight ? -q->maxYExtent() : -q->maxXExtent();
                minExtent = flow == QSGGridView::LeftToRight ? -q->minYExtent() : -q->minXExtent();
            }
            if (snapPos > maxExtent)
                snapPos = maxExtent;
            if (snapPos < minExtent)
                snapPos = minExtent;
        }
        return snapPos;
    }

    FxGridItemSG *snapItemAt(qreal pos) {
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->rowPos();
            if (itemTop+rowSize()/2 >= pos && itemTop - rowSize()/2 <= pos)
                return item;
        }
        return 0;
    }

    int snapIndex() {
        int index = currentIndex;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->rowPos();
            if (itemTop >= highlight->rowPos()-rowSize()/2 && itemTop < highlight->rowPos()+rowSize()/2) {
                index = item->index;
                if (item->colPos() >= highlight->colPos()-colSize()/2 && item->colPos() < highlight->colPos()+colSize()/2)
                    return item->index;
            }
        }
        return index;
    }

    qreal headerSize() const {
        if (!header)
            return 0.0;

        return flow == QSGGridView::LeftToRight
                       ? header->item->height()
                       : header->item->width();
    }


    virtual void itemGeometryChanged(QSGItem *item, const QRectF &newGeometry, const QRectF &oldGeometry) {
        Q_Q(const QSGGridView);
        QSGFlickablePrivate::itemGeometryChanged(item, newGeometry, oldGeometry);
        if (item == q) {
            if (newGeometry.height() != oldGeometry.height()
                || newGeometry.width() != oldGeometry.width()) {
                if (q->isComponentComplete()) {
                    updateGrid();
                    scheduleLayout();
                }
            }
        } else if ((header && header->item == item) || (footer && footer->item == item)) {
            if (header)
                updateHeader();
            if (footer)
                updateFooter();
        }
    }

    void positionViewAtIndex(int index, int mode);
    virtual void fixup(AxisData &data, qreal minExtent, qreal maxExtent);
    virtual void flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity);

    // for debugging only
    void checkVisible() const {
        int skip = 0;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItemSG *listItem = visibleItems.at(i);
            if (listItem->index == -1) {
                ++skip;
            } else if (listItem->index != visibleIndex + i - skip) {
                for (int j = 0; j < visibleItems.count(); j++)
                    qDebug() << " index" << j << "item index" << visibleItems.at(j)->index;
                qFatal("index %d %d %d", visibleIndex, i, listItem->index);
            }
        }
    }

    QDeclarativeGuard<QSGVisualModel> model;
    QVariant modelVariant;
    QList<FxGridItemSG*> visibleItems;
    QHash<QSGItem*,int> unrequestedItems;
    FxGridItemSG *currentItem;
    Qt::LayoutDirection layoutDirection;
    QSGGridView::Flow flow;
    int visibleIndex;
    int currentIndex;
    int cellWidth;
    int cellHeight;
    int columns;
    int requestedIndex;
    int itemCount;
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    QSGGridView::HighlightRangeMode highlightRange;
    QDeclarativeComponent *highlightComponent;
    FxGridItemSG *highlight;
    FxGridItemSG *trackedItem;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    int buffer;
    QSmoothedAnimation *highlightXAnimator;
    QSmoothedAnimation *highlightYAnimator;
    int highlightMoveDuration;
    QDeclarativeComponent *footerComponent;
    FxGridItemSG *footer;
    QDeclarativeComponent *headerComponent;
    FxGridItemSG *header;
    enum BufferMode { NoBuffer = 0x00, BufferBefore = 0x01, BufferAfter = 0x02 };
    int bufferMode;
    QSGGridView::SnapMode snapMode;

    bool ownModel : 1;
    bool wrap : 1;
    bool autoHighlight : 1;
    bool fixCurrentVisibility : 1;
    bool lazyRelease : 1;
    bool layoutScheduled : 1;
    bool deferredRelease : 1;
    bool haveHighlightRange : 1;
    bool currentIndexCleared : 1;
    bool highlightRangeStartValid : 1;
    bool highlightRangeEndValid : 1;
};

void QSGGridViewPrivate::init()
{
    Q_Q(QSGGridView);
    QObject::connect(q, SIGNAL(movementEnded()), q, SLOT(animStopped()));
    q->setFlag(QSGItem::ItemIsFocusScope);
    q->setFlickableDirection(QSGFlickable::VerticalFlick);
    addItemChangeListener(this, Geometry);
}

void QSGGridViewPrivate::clear()
{
    for (int i = 0; i < visibleItems.count(); ++i)
        releaseItem(visibleItems.at(i));
    visibleItems.clear();
    visibleIndex = 0;
    releaseItem(currentItem);
    currentItem = 0;
    createHighlight();
    trackedItem = 0;
    itemCount = 0;
}

FxGridItemSG *QSGGridViewPrivate::createItem(int modelIndex)
{
    Q_Q(QSGGridView);
    // create object
    requestedIndex = modelIndex;
    FxGridItemSG *listItem = 0;
    if (QSGItem *item = model->item(modelIndex, false)) {
        listItem = new FxGridItemSG(item, q);
        listItem->index = modelIndex;
        if (model->completePending()) {
            // complete
            listItem->item->setZ(1);
            listItem->item->setParentItem(q->contentItem());
            model->completeItem();
        } else {
            listItem->item->setParentItem(q->contentItem());
        }
        unrequestedItems.remove(listItem->item);
    }
    requestedIndex = -1;
    return listItem;
}


void QSGGridViewPrivate::releaseItem(FxGridItemSG *item)
{
    Q_Q(QSGGridView);
    if (!item || !model)
        return;
    if (trackedItem == item) {
        QObject::disconnect(trackedItem->item, SIGNAL(yChanged()), q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, SIGNAL(xChanged()), q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }
    if (model->release(item->item) == 0) {
        // item was not destroyed, and we no longer reference it.
        unrequestedItems.insert(item->item, model->indexOf(item->item, q));
    }
    delete item;
}

void QSGGridViewPrivate::refill(qreal from, qreal to, bool doBuffer)
{
    Q_Q(QSGGridView);
    if (!isValid() || !q->isComponentComplete())
        return;
    itemCount = model->count();
    qreal bufferFrom = from - buffer;
    qreal bufferTo = to + buffer;
    qreal fillFrom = from;
    qreal fillTo = to;
    if (doBuffer && (bufferMode & BufferAfter))
        fillTo = bufferTo;
    if (doBuffer && (bufferMode & BufferBefore))
        fillFrom = bufferFrom;

    bool changed = false;

    int colPos = colPosAt(visibleIndex);
    int rowPos = rowPosAt(visibleIndex);
    int modelIndex = visibleIndex;
    if (visibleItems.count()) {
        rowPos = visibleItems.last()->rowPos();
        colPos = visibleItems.last()->colPos() + colSize();
        if (colPos > colSize() * (columns-1)) {
            colPos = 0;
            rowPos += rowSize();
        }
        int i = visibleItems.count() - 1;
        while (i > 0 && visibleItems.at(i)->index == -1)
            --i;
        modelIndex = visibleItems.at(i)->index + 1;
    }
    int colNum = colPos / colSize();

    FxGridItemSG *item = 0;

    // Item creation and release is staggered in order to avoid
    // creating/releasing multiple items in one frame
    // while flicking (as much as possible).
    while (modelIndex < model->count() && rowPos <= fillTo + rowSize()*(columns - colNum)/(columns+1)) {
//        qDebug() << "refill: append item" << modelIndex;
        if (!(item = createItem(modelIndex)))
            break;
        item->setPosition(colPos, rowPos);
        visibleItems.append(item);
        colPos += colSize();
        colNum++;
        if (colPos > colSize() * (columns-1)) {
            colPos = 0;
            colNum = 0;
            rowPos += rowSize();
        }
        ++modelIndex;
        changed = true;
        if (doBuffer) // never buffer more than one item per frame
            break;
    }

    if (visibleItems.count()) {
        rowPos = visibleItems.first()->rowPos();
        colPos = visibleItems.first()->colPos() - colSize();
        if (colPos < 0) {
            colPos = colSize() * (columns - 1);
            rowPos -= rowSize();
        }
    }
    colNum = colPos / colSize();
    while (visibleIndex > 0 && rowPos + rowSize() - 1 >= fillFrom - rowSize()*(colNum+1)/(columns+1)){
//        qDebug() << "refill: prepend item" << visibleIndex-1 << "top pos" << rowPos << colPos;
        if (!(item = createItem(visibleIndex-1)))
            break;
        --visibleIndex;
        item->setPosition(colPos, rowPos);
        visibleItems.prepend(item);
        colPos -= colSize();
        colNum--;
        if (colPos < 0) {
            colPos = colSize() * (columns - 1);
            colNum = columns-1;
            rowPos -= rowSize();
        }
        changed = true;
        if (doBuffer) // never buffer more than one item per frame
            break;
    }

    if (!lazyRelease || !changed || deferredRelease) { // avoid destroying items in the same frame that we create
        while (visibleItems.count() > 1
               && (item = visibleItems.first())
                    && item->rowPos()+rowSize()-1 < bufferFrom - rowSize()*(item->colPos()/colSize()+1)/(columns+1)) {
            if (item->attached->delayRemove())
                break;
//            qDebug() << "refill: remove first" << visibleIndex << "top end pos" << item->endRowPos();
            if (item->index != -1)
                visibleIndex++;
            visibleItems.removeFirst();
            releaseItem(item);
            changed = true;
        }
        while (visibleItems.count() > 1
               && (item = visibleItems.last())
                    && item->rowPos() > bufferTo + rowSize()*(columns - item->colPos()/colSize())/(columns+1)) {
            if (item->attached->delayRemove())
                break;
//            qDebug() << "refill: remove last" << visibleIndex+visibleItems.count()-1;
            visibleItems.removeLast();
            releaseItem(item);
            changed = true;
        }
        deferredRelease = false;
    } else {
        deferredRelease = true;
    }
    if (changed) {
        if (header)
            updateHeader();
        if (footer)
            updateFooter();
        if (flow == QSGGridView::LeftToRight)
            q->setContentHeight(endPosition() - startPosition());
        else
            q->setContentWidth(endPosition() - startPosition());
    } else if (!doBuffer && buffer && bufferMode != NoBuffer) {
        refill(from, to, true);
    }
    lazyRelease = false;
}

void QSGGridViewPrivate::updateGrid()
{
    Q_Q(QSGGridView);
    columns = (int)qMax((flow == QSGGridView::LeftToRight ? q->width() : q->height()) / colSize(), qreal(1.));
    if (isValid()) {
        if (flow == QSGGridView::LeftToRight)
            q->setContentHeight(endPosition() - startPosition());
        else
            q->setContentWidth(lastPosition() - originPosition());
    }
}

void QSGGridViewPrivate::scheduleLayout()
{
    Q_Q(QSGGridView);
    if (!layoutScheduled) {
        layoutScheduled = true;
        q->polish();
    }
}

void QSGGridViewPrivate::layout()
{
    Q_Q(QSGGridView);
    layoutScheduled = false;
    if (!isValid() && !visibleItems.count()) {
        clear();
        return;
    }
    if (visibleItems.count()) {
        qreal rowPos = visibleItems.first()->rowPos();
        qreal colPos = visibleItems.first()->colPos();
        int col = visibleIndex % columns;
        if (colPos != col * colSize()) {
            colPos = col * colSize();
            visibleItems.first()->setPosition(colPos, rowPos);
        }
        for (int i = 1; i < visibleItems.count(); ++i) {
            FxGridItemSG *item = visibleItems.at(i);
            colPos += colSize();
            if (colPos > colSize() * (columns-1)) {
                colPos = 0;
                rowPos += rowSize();
            }
            item->setPosition(colPos, rowPos);
        }
    }
    if (header)
        updateHeader();
    if (footer)
        updateFooter();
    q->refill();
    updateHighlight();
    moveReason = Other;
    if (flow == QSGGridView::LeftToRight) {
        q->setContentHeight(endPosition() - startPosition());
        fixupY();
    } else {
        q->setContentWidth(endPosition() - startPosition());
        fixupX();
    }
    updateUnrequestedPositions();
}

void QSGGridViewPrivate::updateUnrequestedIndexes()
{
    Q_Q(QSGGridView);
    QHash<QSGItem*,int>::iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it)
        *it = model->indexOf(it.key(), q);
}

void QSGGridViewPrivate::updateUnrequestedPositions()
{
    QHash<QSGItem*,int>::const_iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it) {
        QSGItem *item = it.key();
        if (flow == QSGGridView::LeftToRight) {
            item->setPos(QPointF(colPosAt(*it), rowPosAt(*it)));
        } else {
            if (isRightToLeftTopToBottom())
                item->setPos(QPointF(-rowPosAt(*it)-item->width(), colPosAt(*it)));
            else
                item->setPos(QPointF(rowPosAt(*it), colPosAt(*it)));
        }
    }
}

void QSGGridViewPrivate::updateTrackedItem()
{
    Q_Q(QSGGridView);
    FxGridItemSG *item = currentItem;
    if (highlight)
        item = highlight;

    if (trackedItem && item != trackedItem) {
        QObject::disconnect(trackedItem->item, SIGNAL(yChanged()), q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, SIGNAL(xChanged()), q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }

    if (!trackedItem && item) {
        trackedItem = item;
        QObject::connect(trackedItem->item, SIGNAL(yChanged()), q, SLOT(trackedPositionChanged()));
        QObject::connect(trackedItem->item, SIGNAL(xChanged()), q, SLOT(trackedPositionChanged()));
    }
    if (trackedItem)
        q->trackedPositionChanged();
}

void QSGGridViewPrivate::createHighlight()
{
    Q_Q(QSGGridView);
    bool changed = false;
    if (highlight) {
        if (trackedItem == highlight)
            trackedItem = 0;
        delete highlight->item;
        delete highlight;
        highlight = 0;
        delete highlightXAnimator;
        delete highlightYAnimator;
        highlightXAnimator = 0;
        highlightYAnimator = 0;
        changed = true;
    }

    if (currentItem) {
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
            QDeclarative_setParent_noEvent(item, q->contentItem());
            item->setParentItem(q->contentItem());
        }
        if (item) {
            QDeclarative_setParent_noEvent(item, q->contentItem());
            item->setParentItem(q->contentItem());
            highlight = new FxGridItemSG(item, q);
            if (currentItem && autoHighlight)
                highlight->setPosition(currentItem->colPos(), currentItem->rowPos());
            highlightXAnimator = new QSmoothedAnimation(q);
            highlightXAnimator->target = QDeclarativeProperty(highlight->item, QLatin1String("x"));
            highlightXAnimator->userDuration = highlightMoveDuration;
            highlightYAnimator = new QSmoothedAnimation(q);
            highlightYAnimator->target = QDeclarativeProperty(highlight->item, QLatin1String("y"));
            highlightYAnimator->userDuration = highlightMoveDuration;
            if (autoHighlight) {
                highlightXAnimator->restart();
                highlightYAnimator->restart();
            }
            changed = true;
        }
    }
    if (changed)
        emit q->highlightItemChanged();
}

void QSGGridViewPrivate::updateHighlight()
{
    if ((!currentItem && highlight) || (currentItem && !highlight))
        createHighlight();
    if (currentItem && autoHighlight && highlight && !movingHorizontally && !movingVertically) {
        // auto-update highlight
        highlightXAnimator->to = currentItem->item->x();
        highlightYAnimator->to = currentItem->item->y();
        highlight->item->setWidth(currentItem->item->width());
        highlight->item->setHeight(currentItem->item->height());
        highlightXAnimator->restart();
        highlightYAnimator->restart();
    }
    updateTrackedItem();
}

void QSGGridViewPrivate::updateCurrent(int modelIndex)
{
    Q_Q(QSGGridView);
    if (!q->isComponentComplete() || !isValid() || modelIndex < 0 || modelIndex >= model->count()) {
        if (currentItem) {
            currentItem->attached->setIsCurrentItem(false);
            releaseItem(currentItem);
            currentItem = 0;
            currentIndex = modelIndex;
            emit q->currentIndexChanged();
            updateHighlight();
        } else if (currentIndex != modelIndex) {
            currentIndex = modelIndex;
            emit q->currentIndexChanged();
        }
        return;
    }

    if (currentItem && currentIndex == modelIndex) {
        updateHighlight();
        return;
    }

    FxGridItemSG *oldCurrentItem = currentItem;
    currentIndex = modelIndex;
    currentItem = createItem(modelIndex);
    fixCurrentVisibility = true;
    if (oldCurrentItem && (!currentItem || oldCurrentItem->item != currentItem->item))
        oldCurrentItem->attached->setIsCurrentItem(false);
    if (currentItem) {
        currentItem->setPosition(colPosAt(modelIndex), rowPosAt(modelIndex));
        currentItem->item->setFocus(true);
        currentItem->attached->setIsCurrentItem(true);
    }
    updateHighlight();
    emit q->currentIndexChanged();
    releaseItem(oldCurrentItem);
}

void QSGGridViewPrivate::updateFooter()
{
    Q_Q(QSGGridView);
    if (!footer && footerComponent) {
        QSGItem *item = 0;
        QDeclarativeContext *context = new QDeclarativeContext(qmlContext(q));
        QObject *nobj = footerComponent->create(context);
        if (nobj) {
            QDeclarative_setParent_noEvent(context, nobj);
            item = qobject_cast<QSGItem *>(nobj);
            if (!item)
                delete nobj;
        } else {
            delete context;
        }
        if (item) {
            QDeclarative_setParent_noEvent(item, q->contentItem());
            item->setParentItem(q->contentItem());
            item->setZ(1);
            QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
            itemPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
            footer = new FxGridItemSG(item, q);
        }
    }
    if (footer) {
        qreal colOffset = 0;
        qreal rowOffset;
        if (isRightToLeftTopToBottom()) {
            rowOffset = footer->item->width()-cellWidth;
        } else {
            rowOffset = 0;
            if (q->effectiveLayoutDirection() == Qt::RightToLeft)
                colOffset = footer->item->width()-cellWidth;
        }
        if (visibleItems.count()) {
            qreal endPos = lastPosition();
            if (lastVisibleIndex() == model->count()-1) {
                footer->setPosition(colOffset, endPos + rowOffset);
            } else {
                qreal visiblePos = isRightToLeftTopToBottom() ? -position() : position() + size();
                if (endPos <= visiblePos || footer->endRowPos() < endPos + rowOffset)
                    footer->setPosition(colOffset, endPos + rowOffset);
            }
        } else {
            qreal endPos = 0;
            if (header) {
                endPos += (flow == QSGGridView::LeftToRight) ? header->item->height() : header->item->width();
            }
            footer->setPosition(colOffset, endPos);
        }
    }
}

void QSGGridViewPrivate::updateHeader()
{
    Q_Q(QSGGridView);
    if (!header && headerComponent) {
        QSGItem *item = 0;
        QDeclarativeContext *context = new QDeclarativeContext(qmlContext(q));
        QObject *nobj = headerComponent->create(context);
        if (nobj) {
            QDeclarative_setParent_noEvent(context, nobj);
            item = qobject_cast<QSGItem *>(nobj);
            if (!item)
                delete nobj;
        } else {
            delete context;
        }
        if (item) {
            QDeclarative_setParent_noEvent(item, q->contentItem());
            item->setParentItem(q->contentItem());
            item->setZ(1);
            QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
            itemPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
            header = new FxGridItemSG(item, q);
        }
    }
    if (header) {
        qreal colOffset = 0;
        qreal rowOffset;
        if (isRightToLeftTopToBottom()) {
            rowOffset = -cellWidth;
        } else {
            rowOffset = -headerSize();
            if (q->effectiveLayoutDirection() == Qt::RightToLeft)
                colOffset = header->item->width()-cellWidth;
        }
        if (visibleItems.count()) {
            qreal startPos = originPosition();
            if (visibleIndex == 0) {
                header->setPosition(colOffset, startPos + rowOffset);
            } else {
                qreal tempPos = isRightToLeftTopToBottom() ? -position()-size() : position();
                qreal headerPos = isRightToLeftTopToBottom() ? header->rowPos() + cellWidth - headerSize() : header->rowPos();
                if (tempPos <= startPos || headerPos > startPos + rowOffset)
                    header->setPosition(colOffset, startPos + rowOffset);
            }
        } else {
            header->setPosition(colOffset, 0);
        }
    }
}

void QSGGridViewPrivate::fixupPosition()
{
    moveReason = Other;
    if (flow == QSGGridView::LeftToRight)
        fixupY();
    else
        fixupX();
}

void QSGGridViewPrivate::fixup(AxisData &data, qreal minExtent, qreal maxExtent)
{
    if ((flow == QSGGridView::TopToBottom && &data == &vData)
        || (flow == QSGGridView::LeftToRight && &data == &hData))
        return;

    fixupMode = moveReason == Mouse ? fixupMode : Immediate;

    qreal highlightStart;
    qreal highlightEnd;
    qreal viewPos;
    if (isRightToLeftTopToBottom()) {
        // Handle Right-To-Left exceptions
        viewPos = -position()-size();
        highlightStart = highlightRangeStartValid ? size()-highlightRangeEnd : highlightRangeStart;
        highlightEnd = highlightRangeEndValid ? size()-highlightRangeStart : highlightRangeEnd;
    } else {
        viewPos = position();
        highlightStart = highlightRangeStart;
        highlightEnd = highlightRangeEnd;
    }

    if (snapMode != QSGGridView::NoSnap) {
        qreal tempPosition = isRightToLeftTopToBottom() ? -position()-size() : position();
        FxGridItemSG *topItem = snapItemAt(tempPosition+highlightStart);
        FxGridItemSG *bottomItem = snapItemAt(tempPosition+highlightEnd);
        qreal pos;
        if (topItem && bottomItem && haveHighlightRange && highlightRange == QSGGridView::StrictlyEnforceRange) {
            qreal topPos = qMin(topItem->rowPos() - highlightStart, -maxExtent);
            qreal bottomPos = qMax(bottomItem->rowPos() - highlightEnd, -minExtent);
            pos = qAbs(data.move + topPos) < qAbs(data.move + bottomPos) ? topPos : bottomPos;
        } else if (topItem) {
            qreal headerPos = 0;
            if (header)
                headerPos = isRightToLeftTopToBottom() ? header->rowPos() + cellWidth - headerSize() : header->rowPos();
            if (topItem->index == 0 && header && tempPosition+highlightStart < headerPos+headerSize()/2) {
                pos = isRightToLeftTopToBottom() ? - headerPos + highlightStart - size() : headerPos - highlightStart;
            } else {
                if (isRightToLeftTopToBottom())
                    pos = qMax(qMin(-topItem->rowPos() + highlightStart - size(), -maxExtent), -minExtent);
                else
                    pos = qMax(qMin(topItem->rowPos() - highlightStart, -maxExtent), -minExtent);
            }
        } else if (bottomItem) {
            if (isRightToLeftTopToBottom())
                pos = qMax(qMin(-bottomItem->rowPos() + highlightStart - size(), -maxExtent), -minExtent);
            else
                pos = qMax(qMin(bottomItem->rowPos() - highlightStart, -maxExtent), -minExtent);
        } else {
            QSGFlickablePrivate::fixup(data, minExtent, maxExtent);
            return;
        }
        if (currentItem && haveHighlightRange && highlightRange == QSGGridView::StrictlyEnforceRange) {
            updateHighlight();
            qreal currPos = currentItem->rowPos();
            if (isRightToLeftTopToBottom())
                pos = -pos-size(); // Transform Pos if required
            if (pos < currPos + rowSize() - highlightEnd)
                pos = currPos + rowSize() - highlightEnd;
            if (pos > currPos - highlightStart)
                pos = currPos - highlightStart;
            if (isRightToLeftTopToBottom())
                pos = -pos-size(); // Untransform
        }

        qreal dist = qAbs(data.move + pos);
        if (dist > 0) {
            timeline.reset(data.move);
            if (fixupMode != Immediate) {
                timeline.move(data.move, -pos, QEasingCurve(QEasingCurve::InOutQuad), fixupDuration/2);
                data.fixingUp = true;
            } else {
                timeline.set(data.move, -pos);
            }
            vTime = timeline.time();
        }
    } else if (haveHighlightRange && highlightRange == QSGGridView::StrictlyEnforceRange) {
        if (currentItem) {
            updateHighlight();
            qreal pos = currentItem->rowPos();
            if (viewPos < pos + rowSize() - highlightEnd)
                viewPos = pos + rowSize() - highlightEnd;
            if (viewPos > pos - highlightStart)
                viewPos = pos - highlightStart;
            if (isRightToLeftTopToBottom())
                viewPos = -viewPos-size();
            timeline.reset(data.move);
            if (viewPos != position()) {
                if (fixupMode != Immediate) {
                    timeline.move(data.move, -viewPos, QEasingCurve(QEasingCurve::InOutQuad), fixupDuration/2);
                    data.fixingUp = true;
                } else {
                    timeline.set(data.move, -viewPos);
                }
            }
            vTime = timeline.time();
        }
    } else {
        QSGFlickablePrivate::fixup(data, minExtent, maxExtent);
    }
    fixupMode = Normal;
}

void QSGGridViewPrivate::flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity)
{
    Q_Q(QSGGridView);
    data.fixingUp = false;
    moveReason = Mouse;
    if ((!haveHighlightRange || highlightRange != QSGGridView::StrictlyEnforceRange)
        && snapMode == QSGGridView::NoSnap) {
        QSGFlickablePrivate::flick(data, minExtent, maxExtent, vSize, fixupCallback, velocity);
        return;
    }
    qreal maxDistance = 0;
    qreal dataValue = isRightToLeftTopToBottom() ? -data.move.value()+size() : data.move.value();
    // -ve velocity means list is moving up/left
    if (velocity > 0) {
        if (data.move.value() < minExtent) {
            if (snapMode == QSGGridView::SnapOneRow) {
                if (FxGridItemSG *item = firstVisibleItem())
                    maxDistance = qAbs(item->rowPos() + dataValue);
            } else {
                maxDistance = qAbs(minExtent - data.move.value());
            }
        }
        if (snapMode == QSGGridView::NoSnap && highlightRange != QSGGridView::StrictlyEnforceRange)
            data.flickTarget = minExtent;
    } else {
        if (data.move.value() > maxExtent) {
            if (snapMode == QSGGridView::SnapOneRow) {
                qreal pos = snapPosAt(-dataValue) + (isRightToLeftTopToBottom() ? 0 : rowSize());
                maxDistance = qAbs(pos + dataValue);
            } else {
                maxDistance = qAbs(maxExtent - data.move.value());
            }
        }
        if (snapMode == QSGGridView::NoSnap && highlightRange != QSGGridView::StrictlyEnforceRange)
            data.flickTarget = maxExtent;
    }
    bool overShoot = boundsBehavior == QSGFlickable::DragAndOvershootBounds;
    qreal highlightStart = isRightToLeftTopToBottom() && highlightRangeStartValid ? size()-highlightRangeEnd : highlightRangeStart;
    if (maxDistance > 0 || overShoot) {
        // This mode requires the grid to stop exactly on a row boundary.
        qreal v = velocity;
        if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
            if (v < 0)
                v = -maxVelocity;
            else
                v = maxVelocity;
        }
        qreal accel = deceleration;
        qreal v2 = v * v;
        qreal overshootDist = 0.0;
        if ((maxDistance > 0.0 && v2 / (2.0f * maxDistance) < accel) || snapMode == QSGGridView::SnapOneRow) {
            // + rowSize()/4 to encourage moving at least one item in the flick direction
            qreal dist = v2 / (accel * 2.0) + rowSize()/4;
            dist = qMin(dist, maxDistance);
            if (v > 0)
                dist = -dist;
            qreal distTemp = isRightToLeftTopToBottom() ? -dist : dist;
            data.flickTarget = -snapPosAt(-(dataValue - highlightStart) + distTemp) + highlightStart;
            data.flickTarget = isRightToLeftTopToBottom() ? -data.flickTarget+size() : data.flickTarget;
            qreal adjDist = -data.flickTarget + data.move.value();
            if (qAbs(adjDist) > qAbs(dist)) {
                // Prevent painfully slow flicking - adjust velocity to suit flickDeceleration
                qreal adjv2 = accel * 2.0f * qAbs(adjDist);
                if (adjv2 > v2) {
                    v2 = adjv2;
                    v = qSqrt(v2);
                    if (dist > 0)
                        v = -v;
                }
            }
            dist = adjDist;
            accel = v2 / (2.0f * qAbs(dist));
        } else {
            data.flickTarget = velocity > 0 ? minExtent : maxExtent;
            overshootDist = overShoot ? overShootDistance(v, vSize) : 0;
        }
        timeline.reset(data.move);
        timeline.accel(data.move, v, accel, maxDistance + overshootDist);
        timeline.callback(QDeclarativeTimeLineCallback(&data.move, fixupCallback, this));
        if (!flickingHorizontally && q->xflick()) {
            flickingHorizontally = true;
            emit q->flickingChanged();
            emit q->flickingHorizontallyChanged();
            emit q->flickStarted();
        }
        if (!flickingVertically && q->yflick()) {
            flickingVertically = true;
            emit q->flickingChanged();
            emit q->flickingVerticallyChanged();
            emit q->flickStarted();
        }
    } else {
        timeline.reset(data.move);
        fixup(data, minExtent, maxExtent);
    }
}


//----------------------------------------------------------------------------

QSGGridView::QSGGridView(QSGItem *parent)
    : QSGFlickable(*(new QSGGridViewPrivate), parent)
{
    Q_D(QSGGridView);
    d->init();
}

QSGGridView::~QSGGridView()
{
    Q_D(QSGGridView);
    d->clear();
    if (d->ownModel)
        delete d->model;
    delete d->header;
    delete d->footer;
}

// For internal use
int QSGGridView::modelCount() const
{
    Q_D(const QSGGridView);
    return d->model->count();
}

QVariant QSGGridView::model() const
{
    Q_D(const QSGGridView);
    return d->modelVariant;
}

void QSGGridView::setModel(const QVariant &model)
{
    Q_D(QSGGridView);
    if (d->modelVariant == model)
        return;
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        disconnect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
    }
    d->clear();
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
    if (d->model) {
        d->bufferMode = QSGGridViewPrivate::BufferBefore | QSGGridViewPrivate::BufferAfter;
        if (isComponentComplete()) {
            refill();
            if ((d->currentIndex >= d->model->count() || d->currentIndex < 0) && !d->currentIndexCleared) {
                setCurrentIndex(0);
            } else {
                d->moveReason = QSGGridViewPrivate::SetIndex;
                d->updateCurrent(d->currentIndex);
                if (d->highlight && d->currentItem) {
                    if (d->autoHighlight)
                        d->highlight->setPosition(d->currentItem->colPos(), d->currentItem->rowPos());
                    d->updateTrackedItem();
                }
                d->moveReason = QSGGridViewPrivate::Other;
            }
        }
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        connect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        connect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
        emit countChanged();
    }
    emit modelChanged();
}

QDeclarativeComponent *QSGGridView::delegate() const
{
    Q_D(const QSGGridView);
    if (d->model) {
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QSGGridView::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QSGGridView);
    if (delegate == this->delegate())
        return;

    if (!d->ownModel) {
        d->model = new QSGVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        if (isComponentComplete()) {
            for (int i = 0; i < d->visibleItems.count(); ++i)
                d->releaseItem(d->visibleItems.at(i));
            d->visibleItems.clear();
            d->releaseItem(d->currentItem);
            d->currentItem = 0;
            refill();
            d->moveReason = QSGGridViewPrivate::SetIndex;
            d->updateCurrent(d->currentIndex);
            if (d->highlight && d->currentItem) {
                if (d->autoHighlight)
                    d->highlight->setPosition(d->currentItem->colPos(), d->currentItem->rowPos());
                d->updateTrackedItem();
            }
            d->moveReason = QSGGridViewPrivate::Other;
        }
        emit delegateChanged();
    }
}

int QSGGridView::currentIndex() const
{
    Q_D(const QSGGridView);
    return d->currentIndex;
}

void QSGGridView::setCurrentIndex(int index)
{
    Q_D(QSGGridView);
    if (d->requestedIndex >= 0) // currently creating item
        return;
    d->currentIndexCleared = (index == -1);
    if (index == d->currentIndex)
        return;
    if (isComponentComplete() && d->isValid()) {
        d->moveReason = QSGGridViewPrivate::SetIndex;
        d->updateCurrent(index);
    } else {
        d->currentIndex = index;
        emit currentIndexChanged();
    }
}

QSGItem *QSGGridView::currentItem()
{
    Q_D(QSGGridView);
    if (!d->currentItem)
        return 0;
    return d->currentItem->item;
}

QSGItem *QSGGridView::highlightItem()
{
    Q_D(QSGGridView);
    if (!d->highlight)
        return 0;
    return d->highlight->item;
}

int QSGGridView::count() const
{
    Q_D(const QSGGridView);
    if (d->model)
        return d->model->count();
    return 0;
}

QDeclarativeComponent *QSGGridView::highlight() const
{
    Q_D(const QSGGridView);
    return d->highlightComponent;
}

void QSGGridView::setHighlight(QDeclarativeComponent *highlight)
{
    Q_D(QSGGridView);
    if (highlight != d->highlightComponent) {
        d->highlightComponent = highlight;
        d->updateCurrent(d->currentIndex);
        emit highlightChanged();
    }
}

bool QSGGridView::highlightFollowsCurrentItem() const
{
    Q_D(const QSGGridView);
    return d->autoHighlight;
}

void QSGGridView::setHighlightFollowsCurrentItem(bool autoHighlight)
{
    Q_D(QSGGridView);
    if (d->autoHighlight != autoHighlight) {
        d->autoHighlight = autoHighlight;
        if (autoHighlight) {
            d->updateHighlight();
        } else if (d->highlightXAnimator) {
            d->highlightXAnimator->stop();
            d->highlightYAnimator->stop();
        }
    }
}

int QSGGridView::highlightMoveDuration() const
{
    Q_D(const QSGGridView);
    return d->highlightMoveDuration;
}

void QSGGridView::setHighlightMoveDuration(int duration)
{
    Q_D(QSGGridView);
    if (d->highlightMoveDuration != duration) {
        d->highlightMoveDuration = duration;
        if (d->highlightYAnimator) {
            d->highlightXAnimator->userDuration = d->highlightMoveDuration;
            d->highlightYAnimator->userDuration = d->highlightMoveDuration;
        }
        emit highlightMoveDurationChanged();
    }
}

qreal QSGGridView::preferredHighlightBegin() const
{
    Q_D(const QSGGridView);
    return d->highlightRangeStart;
}

void QSGGridView::setPreferredHighlightBegin(qreal start)
{
    Q_D(QSGGridView);
    d->highlightRangeStartValid = true;
    if (d->highlightRangeStart == start)
        return;
    d->highlightRangeStart = start;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit preferredHighlightBeginChanged();
}

void QSGGridView::resetPreferredHighlightBegin()
{
    Q_D(QSGGridView);
    d->highlightRangeStartValid = false;
    if (d->highlightRangeStart == 0)
        return;
    d->highlightRangeStart = 0;
    emit preferredHighlightBeginChanged();
}

qreal QSGGridView::preferredHighlightEnd() const
{
    Q_D(const QSGGridView);
    return d->highlightRangeEnd;
}

void QSGGridView::setPreferredHighlightEnd(qreal end)
{
    Q_D(QSGGridView);
    d->highlightRangeEndValid = true;
    if (d->highlightRangeEnd == end)
        return;
    d->highlightRangeEnd = end;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit preferredHighlightEndChanged();
}

void QSGGridView::resetPreferredHighlightEnd()
{
    Q_D(QSGGridView);
    d->highlightRangeEndValid = false;
    if (d->highlightRangeEnd == 0)
        return;
    d->highlightRangeEnd = 0;
    emit preferredHighlightEndChanged();
}

QSGGridView::HighlightRangeMode QSGGridView::highlightRangeMode() const
{
    Q_D(const QSGGridView);
    return d->highlightRange;
}

void QSGGridView::setHighlightRangeMode(HighlightRangeMode mode)
{
    Q_D(QSGGridView);
    if (d->highlightRange == mode)
        return;
    d->highlightRange = mode;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit highlightRangeModeChanged();
}

Qt::LayoutDirection QSGGridView::layoutDirection() const
{
    Q_D(const QSGGridView);
    return d->layoutDirection;
}

void QSGGridView::setLayoutDirection(Qt::LayoutDirection layoutDirection)
{
    Q_D(QSGGridView);
    if (d->layoutDirection != layoutDirection) {
        d->layoutDirection = layoutDirection;
        d->regenerate();
        emit layoutDirectionChanged();
        emit effectiveLayoutDirectionChanged();
    }
}

Qt::LayoutDirection QSGGridView::effectiveLayoutDirection() const
{
    Q_D(const QSGGridView);
    if (d->effectiveLayoutMirror)
        return d->layoutDirection == Qt::RightToLeft ? Qt::LeftToRight : Qt::RightToLeft;
    else
        return d->layoutDirection;
}

QSGGridView::Flow QSGGridView::flow() const
{
    Q_D(const QSGGridView);
    return d->flow;
}

void QSGGridView::setFlow(Flow flow)
{
    Q_D(QSGGridView);
    if (d->flow != flow) {
        d->flow = flow;
        if (d->flow == LeftToRight) {
            setContentWidth(-1);
            setFlickableDirection(QSGFlickable::VerticalFlick);
        } else {
            setContentHeight(-1);
            setFlickableDirection(QSGFlickable::HorizontalFlick);
        }
        setContentX(0);
        setContentY(0);
        d->regenerate();
        emit flowChanged();
    }
}

bool QSGGridView::isWrapEnabled() const
{
    Q_D(const QSGGridView);
    return d->wrap;
}

void QSGGridView::setWrapEnabled(bool wrap)
{
    Q_D(QSGGridView);
    if (d->wrap == wrap)
        return;
    d->wrap = wrap;
    emit keyNavigationWrapsChanged();
}

int QSGGridView::cacheBuffer() const
{
    Q_D(const QSGGridView);
    return d->buffer;
}

void QSGGridView::setCacheBuffer(int buffer)
{
    Q_D(QSGGridView);
    if (d->buffer != buffer) {
        d->buffer = buffer;
        if (isComponentComplete())
            refill();
        emit cacheBufferChanged();
    }
}

int QSGGridView::cellWidth() const
{
    Q_D(const QSGGridView);
    return d->cellWidth;
}

void QSGGridView::setCellWidth(int cellWidth)
{
    Q_D(QSGGridView);
    if (cellWidth != d->cellWidth && cellWidth > 0) {
        d->cellWidth = qMax(1, cellWidth);
        d->updateGrid();
        emit cellWidthChanged();
        d->layout();
    }
}

int QSGGridView::cellHeight() const
{
    Q_D(const QSGGridView);
    return d->cellHeight;
}

void QSGGridView::setCellHeight(int cellHeight)
{
    Q_D(QSGGridView);
    if (cellHeight != d->cellHeight && cellHeight > 0) {
        d->cellHeight = qMax(1, cellHeight);
        d->updateGrid();
        emit cellHeightChanged();
        d->layout();
    }
}

QSGGridView::SnapMode QSGGridView::snapMode() const
{
    Q_D(const QSGGridView);
    return d->snapMode;
}

void QSGGridView::setSnapMode(SnapMode mode)
{
    Q_D(QSGGridView);
    if (d->snapMode != mode) {
        d->snapMode = mode;
        emit snapModeChanged();
    }
}

QDeclarativeComponent *QSGGridView::footer() const
{
    Q_D(const QSGGridView);
    return d->footerComponent;
}

void QSGGridView::setFooter(QDeclarativeComponent *footer)
{
    Q_D(QSGGridView);
    if (d->footerComponent != footer) {
        if (d->footer) {
            // XXX todo - the original did scene()->removeItem().  Why?
            d->footer->item->setParentItem(0);
            d->footer->item->deleteLater();
            delete d->footer;
            d->footer = 0;
        }
        d->footerComponent = footer;
        if (isComponentComplete()) {
            d->updateFooter();
            d->updateGrid();
            d->fixupPosition();
        }
        emit footerChanged();
    }
}

QDeclarativeComponent *QSGGridView::header() const
{
    Q_D(const QSGGridView);
    return d->headerComponent;
}

void QSGGridView::setHeader(QDeclarativeComponent *header)
{
    Q_D(QSGGridView);
    if (d->headerComponent != header) {
        if (d->header) {
            // XXX todo - the original did scene()->removeItem().  Why?
            d->header->item->setParentItem(0);
            d->header->item->deleteLater();
            delete d->header;
            d->header = 0;
        }
        d->headerComponent = header;
        if (isComponentComplete()) {
            d->updateHeader();
            d->updateFooter();
            d->updateGrid();
            d->fixupPosition();
        }
        emit headerChanged();
    }
}

void QSGGridView::setContentX(qreal pos)
{
    Q_D(QSGGridView);
    // Positioning the view manually should override any current movement state
    d->moveReason = QSGGridViewPrivate::Other;
    QSGFlickable::setContentX(pos);
}

void QSGGridView::setContentY(qreal pos)
{
    Q_D(QSGGridView);
    // Positioning the view manually should override any current movement state
    d->moveReason = QSGGridViewPrivate::Other;
    QSGFlickable::setContentY(pos);
}

void QSGGridView::updatePolish() 
{
    Q_D(QSGGridView);
    QSGFlickable::updatePolish();
    d->layout();
}

void QSGGridView::viewportMoved()
{
    Q_D(QSGGridView);
    QSGFlickable::viewportMoved();
    if (!d->itemCount)
        return;
    d->lazyRelease = true;
    if (d->flickingHorizontally || d->flickingVertically) {
        if (yflick()) {
            if (d->vData.velocity > 0)
                d->bufferMode = QSGGridViewPrivate::BufferBefore;
            else if (d->vData.velocity < 0)
                d->bufferMode = QSGGridViewPrivate::BufferAfter;
        }

        if (xflick()) {
            if (d->hData.velocity > 0)
                d->bufferMode = QSGGridViewPrivate::BufferBefore;
            else if (d->hData.velocity < 0)
                d->bufferMode = QSGGridViewPrivate::BufferAfter;
        }
    }
    refill();
    if (d->flickingHorizontally || d->flickingVertically || d->movingHorizontally || d->movingVertically)
        d->moveReason = QSGGridViewPrivate::Mouse;
    if (d->moveReason != QSGGridViewPrivate::SetIndex) {
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange && d->highlight) {
            // reposition highlight
            qreal pos = d->highlight->rowPos();
            qreal viewPos;
            qreal highlightStart;
            qreal highlightEnd;
            if (d->isRightToLeftTopToBottom()) {
                highlightStart = d->highlightRangeStartValid ? d->size()-d->highlightRangeEnd : d->highlightRangeStart;
                highlightEnd = d->highlightRangeEndValid ? d->size()-d->highlightRangeStart : d->highlightRangeEnd;
                viewPos = -d->position()-d->size();
            } else {
                highlightStart = d->highlightRangeStart;
                highlightEnd = d->highlightRangeEnd;
                viewPos = d->position();
            }
            if (pos > viewPos + highlightEnd - d->rowSize())
                pos = viewPos + highlightEnd - d->rowSize();
            if (pos < viewPos + highlightStart)
                pos = viewPos + highlightStart;
            d->highlight->setPosition(d->highlight->colPos(), qRound(pos));

            // update current index
            int idx = d->snapIndex();
            if (idx >= 0 && idx != d->currentIndex) {
                d->updateCurrent(idx);
                if (d->currentItem && d->currentItem->colPos() != d->highlight->colPos() && d->autoHighlight) {
                    if (d->flow == LeftToRight)
                        d->highlightXAnimator->to = d->currentItem->item->x();
                    else
                        d->highlightYAnimator->to = d->currentItem->item->y();
                }
            }
        }
    }
}

qreal QSGGridView::minYExtent() const
{
    Q_D(const QSGGridView);
    if (d->flow == QSGGridView::TopToBottom)
        return QSGFlickable::minYExtent();
    qreal extent = -d->startPosition();
    if (d->header && d->visibleItems.count())
        extent += d->header->item->height();
    if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
        extent += d->highlightRangeStart;
        extent = qMax(extent, -(d->rowPosAt(0) + d->rowSize() - d->highlightRangeEnd));
    }
    return extent;
}

qreal QSGGridView::maxYExtent() const
{
    Q_D(const QSGGridView);
    if (d->flow == QSGGridView::TopToBottom)
        return QSGFlickable::maxYExtent();
    qreal extent;
    if (!d->model || !d->model->count()) {
        extent = 0;
    } else if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
        extent = -(d->rowPosAt(d->model->count()-1) - d->highlightRangeStart);
        if (d->highlightRangeEnd != d->highlightRangeStart)
            extent = qMin(extent, -(d->endPosition() - d->highlightRangeEnd + 1));
    } else {
        extent = -(d->endPosition() - height());
    }
    if (d->footer)
        extent -= d->footer->item->height();
    const qreal minY = minYExtent();
    if (extent > minY)
        extent = minY;
    return extent;
}

qreal QSGGridView::minXExtent() const
{
    Q_D(const QSGGridView);
    if (d->flow == QSGGridView::LeftToRight)
        return QSGFlickable::minXExtent();
    qreal extent = -d->startPosition();
    qreal highlightStart;
    qreal highlightEnd;
    qreal endPositionFirstItem;
    if (d->isRightToLeftTopToBottom()) {
        endPositionFirstItem = d->rowPosAt(d->model->count()-1);
        highlightStart = d->highlightRangeStartValid
                ? d->highlightRangeStart - (d->lastPosition()-endPositionFirstItem)
                : d->size() - (d->lastPosition()-endPositionFirstItem);
        highlightEnd = d->highlightRangeEndValid ? d->highlightRangeEnd : d->size();
        if (d->footer && d->visibleItems.count())
            extent += d->footer->item->width();
    } else {
        endPositionFirstItem = d->rowPosAt(0)+d->rowSize();
        highlightStart = d->highlightRangeStart;
        highlightEnd = d->highlightRangeEnd;
        if (d->header && d->visibleItems.count())
            extent += d->header->item->width();
    }
    if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
        extent += highlightStart;
        extent = qMax(extent, -(endPositionFirstItem - highlightEnd));
    }
    return extent;
}

qreal QSGGridView::maxXExtent() const
{
    Q_D(const QSGGridView);
    if (d->flow == QSGGridView::LeftToRight)
        return QSGFlickable::maxXExtent();
    qreal extent;
    qreal highlightStart;
    qreal highlightEnd;
    qreal lastItemPosition;
    if (d->isRightToLeftTopToBottom()){
        highlightStart = d->highlightRangeStartValid ? d->highlightRangeEnd : d->size();
        highlightEnd = d->highlightRangeEndValid ? d->highlightRangeStart : d->size();
        lastItemPosition = d->endPosition();
    } else {
        highlightStart = d->highlightRangeStart;
        highlightEnd = d->highlightRangeEnd;
        if (d->model && d->model->count())
            lastItemPosition = d->rowPosAt(d->model->count()-1);
    }
    if (!d->model || !d->model->count()) {
        extent = 0;
    } else if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
        extent = -(lastItemPosition - highlightStart);
        if (highlightEnd != highlightStart)
            extent = d->isRightToLeftTopToBottom()
                    ? qMax(extent, -(d->endPosition() - highlightEnd + 1))
                    : qMin(extent, -(d->endPosition() - highlightEnd + 1));
    } else {
        extent = -(d->endPosition() - width());
    }
    if (d->isRightToLeftTopToBottom()) {
        if (d->header)
            extent -= d->header->item->width();
    } else {
        if (d->footer)
            extent -= d->footer->item->width();
    }

    const qreal minX = minXExtent();
    if (extent > minX)
        extent = minX;
    return extent;
}

void QSGGridView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QSGGridView);
    if (d->model && d->model->count() && d->interactive) {
        d->moveReason = QSGGridViewPrivate::SetIndex;
        int oldCurrent = currentIndex();
        switch (event->key()) {
        case Qt::Key_Up:
            moveCurrentIndexUp();
            break;
        case Qt::Key_Down:
            moveCurrentIndexDown();
            break;
        case Qt::Key_Left:
            moveCurrentIndexLeft();
            break;
        case Qt::Key_Right:
            moveCurrentIndexRight();
            break;
        default:
            break;
        }
        if (oldCurrent != currentIndex()) {
            event->accept();
            return;
        }
    }
    d->moveReason = QSGGridViewPrivate::Other;
    event->ignore();
    QSGFlickable::keyPressEvent(event);
}

void QSGGridView::moveCurrentIndexUp()
{
    Q_D(QSGGridView);
    const int count = d->model ? d->model->count() : 0;
    if (!count)
        return;
    if (d->flow == QSGGridView::LeftToRight) {
        if (currentIndex() >= d->columns || d->wrap) {
            int index = currentIndex() - d->columns;
            setCurrentIndex((index >= 0 && index < count) ? index : count-1);
        }
    } else {
        if (currentIndex() > 0 || d->wrap) {
            int index = currentIndex() - 1;
            setCurrentIndex((index >= 0 && index < count) ? index : count-1);
        }
    }
}

void QSGGridView::moveCurrentIndexDown()
{
    Q_D(QSGGridView);
    const int count = d->model ? d->model->count() : 0;
    if (!count)
        return;
    if (d->flow == QSGGridView::LeftToRight) {
        if (currentIndex() < count - d->columns || d->wrap) {
            int index = currentIndex()+d->columns;
            setCurrentIndex((index >= 0 && index < count) ? index : 0);
        }
    } else {
        if (currentIndex() < count - 1 || d->wrap) {
            int index = currentIndex() + 1;
            setCurrentIndex((index >= 0 && index < count) ? index : 0);
        }
    }
}

void QSGGridView::moveCurrentIndexLeft()
{
    Q_D(QSGGridView);
    const int count = d->model ? d->model->count() : 0;
    if (!count)
        return;
    if (effectiveLayoutDirection() == Qt::LeftToRight) {
        if (d->flow == QSGGridView::LeftToRight) {
            if (currentIndex() > 0 || d->wrap) {
                int index = currentIndex() - 1;
                setCurrentIndex((index >= 0 && index < count) ? index : count-1);
            }
        } else {
            if (currentIndex() >= d->columns || d->wrap) {
                int index = currentIndex() - d->columns;
                setCurrentIndex((index >= 0 && index < count) ? index : count-1);
            }
        }
    } else {
        if (d->flow == QSGGridView::LeftToRight) {
            if (currentIndex() < count - 1 || d->wrap) {
                int index = currentIndex() + 1;
                setCurrentIndex((index >= 0 && index < count) ? index : 0);
            }
        } else {
            if (currentIndex() < count - d->columns || d->wrap) {
                int index = currentIndex() + d->columns;
                setCurrentIndex((index >= 0 && index < count) ? index : 0);
            }
        }
    }
}

void QSGGridView::moveCurrentIndexRight()
{
    Q_D(QSGGridView);
    const int count = d->model ? d->model->count() : 0;
    if (!count)
        return;
    if (effectiveLayoutDirection() == Qt::LeftToRight) {
        if (d->flow == QSGGridView::LeftToRight) {
            if (currentIndex() < count - 1 || d->wrap) {
                int index = currentIndex() + 1;
                setCurrentIndex((index >= 0 && index < count) ? index : 0);
            }
        } else {
            if (currentIndex() < count - d->columns || d->wrap) {
                int index = currentIndex()+d->columns;
                setCurrentIndex((index >= 0 && index < count) ? index : 0);
            }
        }
    } else {
        if (d->flow == QSGGridView::LeftToRight) {
            if (currentIndex() > 0 || d->wrap) {
                int index = currentIndex() - 1;
                setCurrentIndex((index >= 0 && index < count) ? index : count-1);
            }
        } else {
            if (currentIndex() >= d->columns || d->wrap) {
                int index = currentIndex() - d->columns;
                setCurrentIndex((index >= 0 && index < count) ? index : count-1);
            }
        }
    }
}

void QSGGridViewPrivate::positionViewAtIndex(int index, int mode)
{
    Q_Q(QSGGridView);
    if (!isValid())
        return;
    if (mode < QSGGridView::Beginning || mode > QSGGridView::Contain)
        return;

    int idx = qMax(qMin(index, model->count()-1), 0);

    if (layoutScheduled)
        layout();
    qreal pos = isRightToLeftTopToBottom() ? -position() - size() : position();
    FxGridItemSG *item = visibleItem(idx);
    qreal maxExtent;
    if (flow == QSGGridView::LeftToRight)
        maxExtent = -q->maxYExtent();
    else
        maxExtent = isRightToLeftTopToBottom() ? q->minXExtent()-size() : -q->maxXExtent();
    if (!item) {
        int itemPos = rowPosAt(idx);
        // save the currently visible items in case any of them end up visible again
        QList<FxGridItemSG*> oldVisible = visibleItems;
        visibleItems.clear();
        visibleIndex = idx - idx % columns;
        if (flow == QSGGridView::LeftToRight)
            maxExtent = -q->maxYExtent();
        else
            maxExtent = isRightToLeftTopToBottom() ? q->minXExtent()-size() : -q->maxXExtent();
        setPosition(qMin(qreal(itemPos), maxExtent));
        // now release the reference to all the old visible items.
        for (int i = 0; i < oldVisible.count(); ++i)
            releaseItem(oldVisible.at(i));
        item = visibleItem(idx);
    }
    if (item) {
        qreal itemPos = item->rowPos();
        switch (mode) {
        case QSGGridView::Beginning:
            pos = itemPos;
            if (index < 0 && header) {
                pos -= flow == QSGGridView::LeftToRight
                            ? header->item->height()
                            : header->item->width();
            }
            break;
        case QSGGridView::Center:
            pos = itemPos - (size() - rowSize())/2;
            break;
        case QSGGridView::End:
            pos = itemPos - size() + rowSize();
            if (index >= model->count() && footer) {
                pos += flow == QSGGridView::LeftToRight
                            ? footer->item->height()
                            : footer->item->width();
            }
            break;
        case QSGGridView::Visible:
            if (itemPos > pos + size())
                pos = itemPos - size() + rowSize();
            else if (item->endRowPos() < pos)
                pos = itemPos;
            break;
        case QSGGridView::Contain:
            if (item->endRowPos() > pos + size())
                pos = itemPos - size() + rowSize();
            if (itemPos < pos)
                pos = itemPos;
        }
        pos = qMin(pos, maxExtent);
        qreal minExtent;
        if (flow == QSGGridView::LeftToRight)
            minExtent = -q->minYExtent();
        else
            minExtent = isRightToLeftTopToBottom() ? q->maxXExtent()-size() : -q->minXExtent();
        pos = qMax(pos, minExtent);
        moveReason = QSGGridViewPrivate::Other;
        q->cancelFlick();
        setPosition(pos);
    }
    fixupPosition();
}

void QSGGridView::positionViewAtIndex(int index, int mode)
{
    Q_D(QSGGridView);
    if (!d->isValid() || index < 0 || index >= d->model->count())
        return;
    d->positionViewAtIndex(index, mode);
}

void QSGGridView::positionViewAtBeginning()
{
    Q_D(QSGGridView);
    if (!d->isValid())
        return;
    d->positionViewAtIndex(-1, Beginning);
}

void QSGGridView::positionViewAtEnd()
{
    Q_D(QSGGridView);
    if (!d->isValid())
        return;
    d->positionViewAtIndex(d->model->count(), End);
}

int QSGGridView::indexAt(qreal x, qreal y) const
{
    Q_D(const QSGGridView);
    for (int i = 0; i < d->visibleItems.count(); ++i) {
        const FxGridItemSG *listItem = d->visibleItems.at(i);
        if(listItem->contains(x, y))
            return listItem->index;
    }

    return -1;
}

void QSGGridView::componentComplete()
{
    Q_D(QSGGridView);
    QSGFlickable::componentComplete();
    d->updateHeader();
    d->updateFooter();
    d->updateGrid();
    if (d->isValid()) {
        refill();
        d->moveReason = QSGGridViewPrivate::SetIndex;
        if (d->currentIndex < 0 && !d->currentIndexCleared)
            d->updateCurrent(0);
        else
            d->updateCurrent(d->currentIndex);
        if (d->highlight && d->currentItem) {
            if (d->autoHighlight)
                d->highlight->setPosition(d->currentItem->colPos(), d->currentItem->rowPos());
            d->updateTrackedItem();
        }
        d->moveReason = QSGGridViewPrivate::Other;
        d->fixupPosition();
    }
}

void QSGGridView::trackedPositionChanged()
{
    Q_D(QSGGridView);
    if (!d->trackedItem || !d->currentItem)
        return;
    if (d->moveReason == QSGGridViewPrivate::SetIndex) {
        const qreal trackedPos = d->trackedItem->rowPos();
        qreal viewPos;
        qreal highlightStart;
        qreal highlightEnd;
        if (d->isRightToLeftTopToBottom()) {
            viewPos = -d->position()-d->size();
            highlightStart = d->highlightRangeStartValid ? d->size()-d->highlightRangeEnd : d->highlightRangeStart;
            highlightEnd = d->highlightRangeEndValid ? d->size()-d->highlightRangeStart : d->highlightRangeEnd;
        } else {
            viewPos = d->position();
            highlightStart = d->highlightRangeStart;
            highlightEnd = d->highlightRangeEnd;
        }
        qreal pos = viewPos;
        if (d->haveHighlightRange) {
            if (d->highlightRange == StrictlyEnforceRange) {
                if (trackedPos > pos + highlightEnd - d->rowSize())
                    pos = trackedPos - highlightEnd + d->rowSize();
                if (trackedPos < pos + highlightStart)
                    pos = trackedPos - highlightStart;
            } else {
                if (trackedPos < d->startPosition() + highlightStart) {
                    pos = d->startPosition();
                } else if (d->trackedItem->endRowPos() > d->endPosition() - d->size() + highlightEnd) {
                    pos = d->endPosition() - d->size() + 1;
                    if (pos < d->startPosition())
                        pos = d->startPosition();
                } else {
                    if (trackedPos < viewPos + highlightStart) {
                        pos = trackedPos - highlightStart;
                    } else if (trackedPos > viewPos + highlightEnd - d->rowSize()) {
                        pos = trackedPos - highlightEnd + d->rowSize();
                    }
                }
            }
        } else {
            if (trackedPos < viewPos && d->currentItem->rowPos() < viewPos) {
                pos = qMax(trackedPos, d->currentItem->rowPos());
            } else if (d->trackedItem->endRowPos() >= viewPos + d->size()
                && d->currentItem->endRowPos() >= viewPos + d->size()) {
                if (d->trackedItem->endRowPos() <= d->currentItem->endRowPos()) {
                    pos = d->trackedItem->endRowPos() - d->size() + 1;
                    if (d->rowSize() > d->size())
                        pos = trackedPos;
                } else {
                    pos = d->currentItem->endRowPos() - d->size() + 1;
                    if (d->rowSize() > d->size())
                        pos = d->currentItem->rowPos();
                }
            }
        }
        if (viewPos != pos) {
            cancelFlick();
            d->calcVelocity = true;
            d->setPosition(pos);
            d->calcVelocity = false;
        }
    }
}

void QSGGridView::itemsInserted(int modelIndex, int count)
{
    Q_D(QSGGridView);
    if (!isComponentComplete())
        return;

    int index = d->visibleItems.count() ? d->mapFromModel(modelIndex) : 0;
    if (index < 0) {
        int i = d->visibleItems.count() - 1;
        while (i > 0 && d->visibleItems.at(i)->index == -1)
            --i;
        if (d->visibleItems.at(i)->index + 1 == modelIndex) {
            // Special case of appending an item to the model.
            index = d->visibleIndex + d->visibleItems.count();
        } else {
            if (modelIndex <= d->visibleIndex) {
                // Insert before visible items
                d->visibleIndex += count;
                for (int i = 0; i < d->visibleItems.count(); ++i) {
                    FxGridItemSG *listItem = d->visibleItems.at(i);
                    if (listItem->index != -1 && listItem->index >= modelIndex)
                        listItem->index += count;
                }
            }
            if (d->currentIndex >= modelIndex) {
                // adjust current item index
                d->currentIndex += count;
                if (d->currentItem)
                    d->currentItem->index = d->currentIndex;
                emit currentIndexChanged();
            }
            d->scheduleLayout();
            d->itemCount += count;
            emit countChanged();
            return;
        }
    }

    int insertCount = count;
    if (index < d->visibleIndex && d->visibleItems.count()) {
        insertCount -= d->visibleIndex - index;
        index = d->visibleIndex;
        modelIndex = d->visibleIndex;
    }

    qreal tempPos = d->isRightToLeftTopToBottom() ? -d->position()-d->size()+width()+1 : d->position();
    int to = d->buffer+tempPos+d->size()-1;
    int colPos = 0;
    int rowPos = 0;
    if (d->visibleItems.count()) {
        index -= d->visibleIndex;
        if (index < d->visibleItems.count()) {
            colPos = d->visibleItems.at(index)->colPos();
            rowPos = d->visibleItems.at(index)->rowPos();
        } else {
            // appending items to visible list
            colPos = d->visibleItems.at(index-1)->colPos() + d->colSize();
            rowPos = d->visibleItems.at(index-1)->rowPos();
            if (colPos > d->colSize() * (d->columns-1)) {
                colPos = 0;
                rowPos += d->rowSize();
            }
        }
    } else if (d->itemCount == 0 && d->header) {
        rowPos = d->headerSize();
    }

    // Update the indexes of the following visible items.
    for (int i = 0; i < d->visibleItems.count(); ++i) {
        FxGridItemSG *listItem = d->visibleItems.at(i);
        if (listItem->index != -1 && listItem->index >= modelIndex)
            listItem->index += count;
    }

    bool addedVisible = false;
    QList<FxGridItemSG*> added;
    int i = 0;
    while (i < insertCount && rowPos <= to + d->rowSize()*(d->columns - (colPos/d->colSize()))/qreal(d->columns)) {
        if (!addedVisible) {
            d->scheduleLayout();
            addedVisible = true;
        }
        FxGridItemSG *item = d->createItem(modelIndex + i);
        d->visibleItems.insert(index, item);
        item->setPosition(colPos, rowPos);
        added.append(item);
        colPos += d->colSize();
        if (colPos > d->colSize() * (d->columns-1)) {
            colPos = 0;
            rowPos += d->rowSize();
        }
        ++index;
        ++i;
    }
    if (i < insertCount) {
        // We didn't insert all our new items, which means anything
        // beyond the current index is not visible - remove it.
        while (d->visibleItems.count() > index) {
            d->releaseItem(d->visibleItems.takeLast());
        }
    }

    // update visibleIndex
    d->visibleIndex = 0;
    for (QList<FxGridItemSG*>::Iterator it = d->visibleItems.begin(); it != d->visibleItems.end(); ++it) {
        if ((*it)->index != -1) {
            d->visibleIndex = (*it)->index;
            break;
        }
    }

    if (d->itemCount && d->currentIndex >= modelIndex) {
        // adjust current item index
        d->currentIndex += count;
        if (d->currentItem) {
            d->currentItem->index = d->currentIndex;
            d->currentItem->setPosition(d->colPosAt(d->currentIndex), d->rowPosAt(d->currentIndex));
        } else if (!d->currentIndex || (d->currentIndex < 0 && !d->currentIndexCleared)) {
            d->updateCurrent(0);
        }
        emit currentIndexChanged();
    } else if (d->itemCount == 0 && d->currentIndex == -1) {
        setCurrentIndex(0);
    }

    // everything is in order now - emit add() signal
    for (int j = 0; j < added.count(); ++j)
        added.at(j)->attached->emitAdd();

    d->itemCount += count;
    emit countChanged();
}

void QSGGridView::itemsRemoved(int modelIndex, int count)
{
    Q_D(QSGGridView);
    if (!isComponentComplete())
        return;

    d->itemCount -= count;
    bool currentRemoved = d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count;
    bool removedVisible = false;

    // Remove the items from the visible list, skipping anything already marked for removal
    QList<FxGridItemSG*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItemSG *item = *it;
        if (item->index == -1 || item->index < modelIndex) {
            // already removed, or before removed items
            if (item->index < modelIndex && !removedVisible) {
                d->scheduleLayout();
                removedVisible = true;
            }
            ++it;
        } else if (item->index >= modelIndex + count) {
            // after removed items
            item->index -= count;
            ++it;
        } else {
            // removed item
            if (!removedVisible) {
                d->scheduleLayout();
                removedVisible = true;
            }
            item->attached->emitRemove();
            if (item->attached->delayRemove()) {
                item->index = -1;
                connect(item->attached, SIGNAL(delayRemoveChanged()), this, SLOT(destroyRemoved()), Qt::QueuedConnection);
                ++it;
            } else {
                it = d->visibleItems.erase(it);
                d->releaseItem(item);
            }
        }
    }

    // fix current
    if (d->currentIndex >= modelIndex + count) {
        d->currentIndex -= count;
        if (d->currentItem)
            d->currentItem->index -= count;
        emit currentIndexChanged();
    } else if (currentRemoved) {
        // current item has been removed.
        d->releaseItem(d->currentItem);
        d->currentItem = 0;
        d->currentIndex = -1;
        if (d->itemCount)
            d->updateCurrent(qMin(modelIndex, d->itemCount-1));
    }

    // update visibleIndex
    d->visibleIndex = 0;
    for (it = d->visibleItems.begin(); it != d->visibleItems.end(); ++it) {
        if ((*it)->index != -1) {
            d->visibleIndex = (*it)->index;
            break;
        }
    }

    if (removedVisible && d->visibleItems.isEmpty()) {
        d->timeline.clear();
        if (d->itemCount == 0) {
            d->setPosition(0);
            d->updateHeader();
            d->updateFooter();
        }
    }

    emit countChanged();
}

void QSGGridView::destroyRemoved()
{
    Q_D(QSGGridView);
    for (QList<FxGridItemSG*>::Iterator it = d->visibleItems.begin();
            it != d->visibleItems.end();) {
        FxGridItemSG *listItem = *it;
        if (listItem->index == -1 && listItem->attached->delayRemove() == false) {
            d->releaseItem(listItem);
            it = d->visibleItems.erase(it);
        } else {
            ++it;
        }
    }

    // Correct the positioning of the items
    d->layout();
}

void QSGGridView::itemsMoved(int from, int to, int count)
{
    Q_D(QSGGridView);
    if (!isComponentComplete())
        return;
    QHash<int,FxGridItemSG*> moved;

    FxGridItemSG *firstItem = d->firstVisibleItem();

    QList<FxGridItemSG*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItemSG *item = *it;
        if (item->index >= from && item->index < from + count) {
            // take the items that are moving
            item->index += (to-from);
            moved.insert(item->index, item);
            it = d->visibleItems.erase(it);
        } else {
            if (item->index > from && item->index != -1) {
                // move everything after the moved items.
                item->index -= count;
                if (item->index < d->visibleIndex)
                    d->visibleIndex = item->index;
            }
            ++it;
        }
    }

    int remaining = count;
    int endIndex = d->visibleIndex;
    it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItemSG *item = *it;
        if (remaining && item->index >= to && item->index < to + count) {
            // place items in the target position, reusing any existing items
            FxGridItemSG *movedItem = moved.take(item->index);
            if (!movedItem)
                movedItem = d->createItem(item->index);
            it = d->visibleItems.insert(it, movedItem);
            if (it == d->visibleItems.begin() && firstItem)
                movedItem->setPosition(firstItem->colPos(), firstItem->rowPos());
            ++it;
            --remaining;
        } else {
            if (item->index != -1) {
                if (item->index >= to) {
                    // update everything after the moved items.
                    item->index += count;
                }
                endIndex = item->index;
            }
            ++it;
        }
    }

    // If we have moved items to the end of the visible items
    // then add any existing moved items that we have
    while (FxGridItemSG *item = moved.take(endIndex+1)) {
        d->visibleItems.append(item);
        ++endIndex;
    }

    // update visibleIndex
    for (it = d->visibleItems.begin(); it != d->visibleItems.end(); ++it) {
        if ((*it)->index != -1) {
            d->visibleIndex = (*it)->index;
            break;
        }
    }

    // Fix current index
    if (d->currentIndex >= 0 && d->currentItem) {
        int oldCurrent = d->currentIndex;
        d->currentIndex = d->model->indexOf(d->currentItem->item, this);
        if (oldCurrent != d->currentIndex) {
            d->currentItem->index = d->currentIndex;
            emit currentIndexChanged();
        }
    }

    // Whatever moved items remain are no longer visible items.
    while (moved.count()) {
        int idx = moved.begin().key();
        FxGridItemSG *item = moved.take(idx);
        if (d->currentItem && item->item == d->currentItem->item)
            item->setPosition(d->colPosAt(idx), d->rowPosAt(idx));
        d->releaseItem(item);
    }

    d->layout();
}

void QSGGridView::modelReset()
{
    Q_D(QSGGridView);
    d->clear();
    refill();
    d->moveReason = QSGGridViewPrivate::SetIndex;
    d->updateCurrent(d->currentIndex);
    if (d->highlight && d->currentItem) {
        if (d->autoHighlight)
            d->highlight->setPosition(d->currentItem->colPos(), d->currentItem->rowPos());
        d->updateTrackedItem();
    }
    d->moveReason = QSGGridViewPrivate::Other;

    emit countChanged();
}

void QSGGridView::createdItem(int index, QSGItem *item)
{
    Q_D(QSGGridView);
    if (d->requestedIndex != index) {
        item->setParentItem(this);
        d->unrequestedItems.insert(item, index);
        if (d->flow == QSGGridView::LeftToRight) {
            item->setPos(QPointF(d->colPosAt(index), d->rowPosAt(index)));
        } else {
            item->setPos(QPointF(d->rowPosAt(index), d->colPosAt(index)));
        }
    }
}

void QSGGridView::destroyingItem(QSGItem *item)
{
    Q_D(QSGGridView);
    d->unrequestedItems.remove(item);
}

void QSGGridView::animStopped()
{
    Q_D(QSGGridView);
    d->bufferMode = QSGGridViewPrivate::NoBuffer;
    if (d->haveHighlightRange && d->highlightRange == QSGGridView::StrictlyEnforceRange)
        d->updateHighlight();
}

void QSGGridView::refill()
{
    Q_D(QSGGridView);
    if (d->isRightToLeftTopToBottom())
        d->refill(-d->position()-d->size()+1, -d->position());
    else
        d->refill(d->position(), d->position()+d->size()-1);
}


QSGGridViewAttached *QSGGridView::qmlAttachedProperties(QObject *obj)
{
    return new QSGGridViewAttached(obj);
}

QT_END_NAMESPACE
