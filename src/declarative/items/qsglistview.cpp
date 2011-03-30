// Commit: ce38c6e3a9b7eb336cbd9cd1e9520a5000c8f8ac
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

#include "qsglistview_p.h"
#include "qsgflickable_p_p.h"
#include "qsgvisualitemmodel_p.h"

#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeinfo.h>
#include <QtGui/qevent.h>
#include <QtCore/qmath.h>
#include <QtCore/qcoreapplication.h>

#include <private/qdeclarativesmoothedanimation_p_p.h>
#include <private/qlistmodelinterface_p.h>

QT_BEGIN_NAMESPACE

void QSGViewSection::setProperty(const QString &property)
{
    if (property != m_property) {
        m_property = property;
        emit propertyChanged();
    }
}

void QSGViewSection::setCriteria(QSGViewSection::SectionCriteria criteria)
{
    if (criteria != m_criteria) {
        m_criteria = criteria;
        emit criteriaChanged();
    }
}

void QSGViewSection::setDelegate(QDeclarativeComponent *delegate)
{
    if (delegate != m_delegate) {
        m_delegate = delegate;
        emit delegateChanged();
    }
}

QString QSGViewSection::sectionString(const QString &value)
{
    if (m_criteria == FirstCharacter)
        return value.isEmpty() ? QString() : value.at(0);
    else
        return value;
}

//----------------------------------------------------------------------------

class FxListItemSG
{
public:
    FxListItemSG(QSGItem *i, QSGListView *v) : item(i), section(0), view(v) {
        attached = static_cast<QSGListViewAttached*>(qmlAttachedPropertiesObject<QSGListView>(item));
        if (attached)
            attached->setView(view);
    }
    ~FxListItemSG() {}
    qreal position() const {
        if (section) {
            if (view->orientation() == QSGListView::Vertical)
                return section->y();
            else
                return (view->effectiveLayoutDirection() == Qt::RightToLeft ? -section->width()-section->x() : section->x());
        } else {
            return itemPosition();
        }
    }
    qreal itemPosition() const {
        if (view->orientation() == QSGListView::Vertical)
            return item->y();
        else
            return (view->effectiveLayoutDirection() == Qt::RightToLeft ? -item->width()-item->x() : item->x());
    }
    qreal size() const {
        if (section)
            return (view->orientation() == QSGListView::Vertical ? item->height()+section->height() : item->width()+section->width());
        else
            return (view->orientation() == QSGListView::Vertical ? item->height() : item->width());
    }
    qreal itemSize() const {
        return (view->orientation() == QSGListView::Vertical ? item->height() : item->width());
    }
    qreal sectionSize() const {
        if (section)
            return (view->orientation() == QSGListView::Vertical ? section->height() : section->width());
        return 0.0;
    }
    qreal endPosition() const {
        if (view->orientation() == QSGListView::Vertical) {
            return item->y() + (item->height() >= 1.0 ? item->height() : 1) - 1;
        } else {
            return (view->effectiveLayoutDirection() == Qt::RightToLeft
                    ? -item->width()-item->x() + (item->width() >= 1.0 ? item->width() : 1)
                    : item->x() + (item->width() >= 1.0 ? item->width() : 1)) - 1;
        }
    }
    void setPosition(qreal pos) {
        if (view->orientation() == QSGListView::Vertical) {
            if (section) {
                section->setY(pos);
                pos += section->height();
            }
            item->setY(pos);
        } else {
            if (view->effectiveLayoutDirection() == Qt::RightToLeft) {
                if (section) {
                    section->setX(-section->width()-pos);
                    pos += section->width();
                }
                item->setX(-item->width()-pos);
            } else {
                if (section) {
                    section->setX(pos);
                    pos += section->width();
                }
                item->setX(pos);
            }
        }
    }
    void setSize(qreal size) {
        if (view->orientation() == QSGListView::Vertical)
            item->setHeight(size);
        else
            item->setWidth(size);
    }
    bool contains(qreal x, qreal y) const {
        return (x >= item->x() && x < item->x() + item->width() &&
                y >= item->y() && y < item->y() + item->height());
    }

    QSGItem *item;
    QSGItem *section;
    QSGListView *view;
    QSGListViewAttached *attached;
    int index;
};

//----------------------------------------------------------------------------

class QSGListViewPrivate : public QSGFlickablePrivate
{
    Q_DECLARE_PUBLIC(QSGListView)

public:
    QSGListViewPrivate()
        : currentItem(0), orient(QSGListView::Vertical), layoutDirection(Qt::LeftToRight)
        , visiblePos(0), visibleIndex(0)
        , averageSize(100.0), currentIndex(-1), requestedIndex(-1)
        , itemCount(0), highlightRangeStart(0), highlightRangeEnd(0)
        , highlightComponent(0), highlight(0), trackedItem(0)
        , moveReason(Other), buffer(0), highlightPosAnimator(0), highlightSizeAnimator(0)
        , sectionCriteria(0), spacing(0.0)
        , highlightMoveSpeed(400), highlightMoveDuration(-1)
        , highlightResizeSpeed(400), highlightResizeDuration(-1), highlightRange(QSGListView::NoHighlightRange)
        , snapMode(QSGListView::NoSnap), overshootDist(0.0)
        , footerComponent(0), footer(0), headerComponent(0), header(0)
        , bufferMode(BufferBefore | BufferAfter)
        , ownModel(false), wrap(false), autoHighlight(true), haveHighlightRange(false)
        , correctFlick(false), inFlickCorrection(false), lazyRelease(false)
        , deferredRelease(false), layoutScheduled(false), currentIndexCleared(false)
        , inViewportMoved(false)
        , highlightRangeStartValid(false), highlightRangeEndValid(false)
        , minExtentDirty(true), maxExtentDirty(true)
    {}

    void init();
    void clear();
    FxListItemSG *createItem(int modelIndex);
    void releaseItem(FxListItemSG *item);

    FxListItemSG *visibleItem(int modelIndex) const {
        if (modelIndex >= visibleIndex && modelIndex < visibleIndex + visibleItems.count()) {
            for (int i = modelIndex - visibleIndex; i < visibleItems.count(); ++i) {
                FxListItemSG *item = visibleItems.at(i);
                if (item->index == modelIndex)
                    return item;
            }
        }
        return 0;
    }

    FxListItemSG *firstVisibleItem() const {
        const qreal pos = isRightToLeft() ? -position()-size() : position();
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItemSG *item = visibleItems.at(i);
            if (item->index != -1 && item->endPosition() > pos)
                return item;
        }
        return visibleItems.count() ? visibleItems.first() : 0;
    }

    FxListItemSG *nextVisibleItem() const {
        const qreal pos = isRightToLeft() ? -position()-size() : position();
        bool foundFirst = false;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItemSG *item = visibleItems.at(i);
            if (item->index != -1) {
                if (foundFirst)
                    return item;
                else if (item->position() < pos && item->endPosition() > pos)
                    foundFirst = true;
            }
        }
        return 0;
    }

    // Returns the item before modelIndex, if created.
    // May return an item marked for removal.
    FxListItemSG *itemBefore(int modelIndex) const {
        if (modelIndex < visibleIndex)
            return 0;
        int idx = 1;
        int lastIndex = -1;
        while (idx < visibleItems.count()) {
            FxListItemSG *item = visibleItems.at(idx);
            if (item->index != -1)
                lastIndex = item->index;
            if (item->index == modelIndex)
                return visibleItems.at(idx-1);
            ++idx;
        }
        if (lastIndex == modelIndex-1)
            return visibleItems.last();
        return 0;
    }

    void regenerate() {
        Q_Q(QSGListView);
        if (q->isComponentComplete()) {
            if (header) {
                // XXX todo - the original did scene()->removeItem().  Why?
                header->item->setParentItem(0);
                header->item->deleteLater();
                delete header;
                header = 0;
            }
            if (footer) {
                // XXX todo - the original did scene()->removeItem().  Why?
                footer->item->setParentItem(0);
                footer->item->deleteLater();
                delete footer;
                footer = 0;
            }
            updateHeader();
            updateFooter();
            clear();
            setPosition(0);
            q->refill();
            updateCurrent(currentIndex);
        }
    }

    void mirrorChange() {
        Q_Q(QSGListView);
        regenerate();
        emit q->effectiveLayoutDirectionChanged();
    }

    bool isRightToLeft() const {
        Q_Q(const QSGListView);
        return orient == QSGListView::Horizontal && q->effectiveLayoutDirection() == Qt::RightToLeft;
    }

    qreal position() const {
        Q_Q(const QSGListView);
        return orient == QSGListView::Vertical ? q->contentY() : q->contentX();
    }
    void setPosition(qreal pos) {
        Q_Q(QSGListView);
        if (orient == QSGListView::Vertical) {
            q->QSGFlickable::setContentY(pos);
        } else {
            if (isRightToLeft())
                q->QSGFlickable::setContentX(-pos-size());
            else
                q->QSGFlickable::setContentX(pos);
        }
    }
    qreal size() const {
        Q_Q(const QSGListView);
        return orient == QSGListView::Vertical ? q->height() : q->width();
    }

    qreal originPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty()) {
            pos = (*visibleItems.constBegin())->position();
            if (visibleIndex > 0)
                pos -= visibleIndex * (averageSize + spacing);
        }
        return pos;
    }

    qreal lastPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty()) {
            int invisibleCount = visibleItems.count() - visibleIndex;
            for (int i = visibleItems.count()-1; i >= 0; --i) {
                if (visibleItems.at(i)->index != -1) {
                    invisibleCount = model->count() - visibleItems.at(i)->index - 1;
                    break;
                }
            }
            pos = (*(--visibleItems.constEnd()))->endPosition() + invisibleCount * (averageSize + spacing);
        } else if (model && model->count()) {
            pos = model->count() * averageSize + (model->count()-1) * spacing;
        }
        return pos;
    }

    qreal startPosition() const {
        return isRightToLeft() ? -lastPosition()-1 : originPosition();
    }

    qreal endPosition() const {
        return isRightToLeft() ? -originPosition()-1 : lastPosition();
    }

    qreal positionAt(int modelIndex) const {
        if (FxListItemSG *item = visibleItem(modelIndex))
            return item->position();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int count = visibleIndex - modelIndex;
                qreal cs = 0;
                if (modelIndex == currentIndex && currentItem) {
                    cs = currentItem->size() + spacing;
                    --count;
                }
                return (*visibleItems.constBegin())->position() - count * (averageSize + spacing) - cs;
            } else {
                int idx = visibleItems.count() - 1;
                while (idx >= 0 && visibleItems.at(idx)->index == -1)
                    --idx;
                if (idx < 0)
                    idx = visibleIndex;
                else
                    idx = visibleItems.at(idx)->index;
                int count = modelIndex - idx - 1;
                return (*(--visibleItems.constEnd()))->endPosition() + spacing + count * (averageSize + spacing) + 1;
            }
        }
        return 0;
    }

    qreal endPositionAt(int modelIndex) const {
        if (FxListItemSG *item = visibleItem(modelIndex))
            return item->endPosition();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int count = visibleIndex - modelIndex;
                return (*visibleItems.constBegin())->position() - (count - 1) * (averageSize + spacing) - spacing - 1;
            } else {
                int idx = visibleItems.count() - 1;
                while (idx >= 0 && visibleItems.at(idx)->index == -1)
                    --idx;
                if (idx < 0)
                    idx = visibleIndex;
                else
                    idx = visibleItems.at(idx)->index;
                int count = modelIndex - idx - 1;
                return (*(--visibleItems.constEnd()))->endPosition() + count * (averageSize + spacing);
            }
        }
        return 0;
    }

    QString sectionAt(int modelIndex) {
        if (FxListItemSG *item = visibleItem(modelIndex))
            return item->attached->section();

        QString section;
        if (sectionCriteria) {
            QString propValue = model->stringValue(modelIndex, sectionCriteria->property());
            section = sectionCriteria->sectionString(propValue);
        }

        return section;
    }

    bool isValid() const {
        return model && model->count() && model->isValid();
    }

    qreal snapPosAt(qreal pos) {
        if (FxListItemSG *snapItem = snapItemAt(pos))
            return snapItem->position();
        if (visibleItems.count()) {
            qreal firstPos = visibleItems.first()->position();
            qreal endPos = visibleItems.last()->position();
            if (pos < firstPos) {
                return firstPos - qRound((firstPos - pos) / averageSize) * averageSize;
            } else if (pos > endPos)
                return endPos + qRound((pos - endPos) / averageSize) * averageSize;
        }
        return qRound((pos - originPosition()) / averageSize) * averageSize + originPosition();
    }

    FxListItemSG *snapItemAt(qreal pos) {
        FxListItemSG *snapItem = 0;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItemSG *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->position();
            if (highlight && itemTop >= pos && item->endPosition() <= pos + highlight->size() - 1)
                return item;
            if (itemTop+item->size()/2 >= pos && itemTop-item->size()/2 < pos)
                snapItem = item;
        }
        return snapItem;
    }

    int lastVisibleIndex() const {
        int lastIndex = -1;
        for (int i = visibleItems.count()-1; i >= 0; --i) {
            FxListItemSG *listItem = visibleItems.at(i);
            if (listItem->index != -1) {
                lastIndex = listItem->index;
                break;
            }
        }
        return lastIndex;
    }

    // map a model index to visibleItems index.
    int mapFromModel(int modelIndex) const {
        if (modelIndex < visibleIndex || modelIndex >= visibleIndex + visibleItems.count())
            return -1;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItemSG *listItem = visibleItems.at(i);
            if (listItem->index == modelIndex)
                return i;
            if (listItem->index > modelIndex)
                return -1;
        }
        return -1; // Not in visibleList
    }

    void updateViewport() {
        Q_Q(QSGListView);
        if (orient == QSGListView::Vertical) {
            q->setContentHeight(endPosition() - startPosition() + 1);
        } else {
            q->setContentWidth(endPosition() - startPosition() + 1);
        }
    }

    void itemGeometryChanged(QSGItem *item, const QRectF &newGeometry, const QRectF &oldGeometry) {
        Q_Q(QSGListView);
        QSGFlickablePrivate::itemGeometryChanged(item, newGeometry, oldGeometry);
        if (!q->isComponentComplete())
            return;
        if (item != contentItem && (!highlight || item != highlight->item)) {
            if ((orient == QSGListView::Vertical && newGeometry.height() != oldGeometry.height())
                || (orient == QSGListView::Horizontal && newGeometry.width() != oldGeometry.width())) {
                scheduleLayout();
            }
        }
        if ((header && header->item == item) || (footer && footer->item == item)) {
            if (header)
                updateHeader();
            if (footer)
                updateFooter();
        }
        if (currentItem && currentItem->item == item)
            updateHighlight();
        if (trackedItem && trackedItem->item == item)
            q->trackedPositionChanged();
    }

    // for debugging only
    void checkVisible() const {
        int skip = 0;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItemSG *listItem = visibleItems.at(i);
            if (listItem->index == -1) {
                ++skip;
            } else if (listItem->index != visibleIndex + i - skip) {
                qFatal("index %d %d %d", visibleIndex, i, listItem->index);
            }
        }
    }

    void refill(qreal from, qreal to, bool doBuffer = false);
    void scheduleLayout();
    void layout();
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateTrackedItem();
    void createHighlight();
    void updateHighlight();
    void createSection(FxListItemSG *);
    void updateSections();
    void updateCurrentSection();
    void updateCurrent(int);
    void updateAverage();
    void updateHeader();
    void updateFooter();
    void fixupPosition();
    void positionViewAtIndex(int index, int mode);
    virtual void fixup(AxisData &data, qreal minExtent, qreal maxExtent);
    virtual void flick(QSGFlickablePrivate::AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity);

    QDeclarativeGuard<QSGVisualModel> model;
    QVariant modelVariant;
    QList<FxListItemSG*> visibleItems;
    QHash<QSGItem*,int> unrequestedItems;
    FxListItemSG *currentItem;
    QSGListView::Orientation orient;
    Qt::LayoutDirection layoutDirection;
    qreal visiblePos;
    int visibleIndex;
    qreal averageSize;
    int currentIndex;
    int requestedIndex;
    int itemCount;
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    QDeclarativeComponent *highlightComponent;
    FxListItemSG *highlight;
    FxListItemSG *trackedItem;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    int buffer;
    QSmoothedAnimation *highlightPosAnimator;
    QSmoothedAnimation *highlightSizeAnimator;
    QSGViewSection *sectionCriteria;
    QString currentSection;
    static const int sectionCacheSize = 4;
    QSGItem *sectionCache[sectionCacheSize];
    qreal spacing;
    qreal highlightMoveSpeed;
    int highlightMoveDuration;
    qreal highlightResizeSpeed;
    int highlightResizeDuration;
    QSGListView::HighlightRangeMode highlightRange;
    QSGListView::SnapMode snapMode;
    qreal overshootDist;
    QDeclarativeComponent *footerComponent;
    FxListItemSG *footer;
    QDeclarativeComponent *headerComponent;
    FxListItemSG *header;
    enum BufferMode { NoBuffer = 0x00, BufferBefore = 0x01, BufferAfter = 0x02 };
    int bufferMode;
    mutable qreal minExtent;
    mutable qreal maxExtent;

    bool ownModel : 1;
    bool wrap : 1;
    bool autoHighlight : 1;
    bool haveHighlightRange : 1;
    bool correctFlick : 1;
    bool inFlickCorrection : 1;
    bool lazyRelease : 1;
    bool deferredRelease : 1;
    bool layoutScheduled : 1;
    bool currentIndexCleared : 1;
    bool inViewportMoved : 1;
    bool highlightRangeStartValid : 1;
    bool highlightRangeEndValid : 1;
    mutable bool minExtentDirty : 1;
    mutable bool maxExtentDirty : 1;
};

void QSGListViewPrivate::init()
{
    Q_Q(QSGListView);
    q->setFlag(QSGItem::ItemIsFocusScope);
    addItemChangeListener(this, Geometry);
    QObject::connect(q, SIGNAL(movementEnded()), q, SLOT(animStopped()));
    q->setFlickableDirection(QSGFlickable::VerticalFlick);
    ::memset(sectionCache, 0, sizeof(QSGItem*) * sectionCacheSize);
}

void QSGListViewPrivate::clear()
{
    timeline.clear();
    for (int i = 0; i < visibleItems.count(); ++i)
        releaseItem(visibleItems.at(i));
    visibleItems.clear();
    for (int i = 0; i < sectionCacheSize; ++i) {
        delete sectionCache[i];
        sectionCache[i] = 0;
    }
    visiblePos = header ? header->size() : 0;
    visibleIndex = 0;
    releaseItem(currentItem);
    currentItem = 0;
    createHighlight();
    trackedItem = 0;
    minExtentDirty = true;
    maxExtentDirty = true;
    itemCount = 0;
}

FxListItemSG *QSGListViewPrivate::createItem(int modelIndex)
{
    Q_Q(QSGListView);
    // create object
    requestedIndex = modelIndex;
    FxListItemSG *listItem = 0;
    if (QSGItem *item = model->item(modelIndex, false)) {
        listItem = new FxListItemSG(item, q);
        listItem->index = modelIndex;
        // initialise attached properties
        if (sectionCriteria) {
            QString propValue = model->stringValue(modelIndex, sectionCriteria->property());
            listItem->attached->m_section = sectionCriteria->sectionString(propValue);
            if (modelIndex > 0) {
                if (FxListItemSG *item = itemBefore(modelIndex))
                    listItem->attached->m_prevSection = item->attached->section();
                else
                    listItem->attached->m_prevSection = sectionAt(modelIndex-1);
            }
            if (modelIndex < model->count()-1) {
                if (FxListItemSG *item = visibleItem(modelIndex+1))
                    listItem->attached->m_nextSection = item->attached->section();
                else
                    listItem->attached->m_nextSection = sectionAt(modelIndex+1);
            }
        }
        if (model->completePending()) {
            // complete
            listItem->item->setZ(1);
            listItem->item->setParentItem(q->contentItem());
            model->completeItem();
        } else {
            listItem->item->setParentItem(q->contentItem());
        }
        QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
        itemPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
        if (sectionCriteria && sectionCriteria->delegate()) {
            if (listItem->attached->m_prevSection != listItem->attached->m_section)
                createSection(listItem);
        }
        unrequestedItems.remove(listItem->item);
    }
    requestedIndex = -1;

    return listItem;
}

void QSGListViewPrivate::releaseItem(FxListItemSG *item)
{
    Q_Q(QSGListView);
    if (!item || !model)
        return;
    if (trackedItem == item)
        trackedItem = 0;
    QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item->item);
    itemPrivate->removeItemChangeListener(this, QSGItemPrivate::Geometry);
    if (model->release(item->item) == 0) {
        // item was not destroyed, and we no longer reference it.
        unrequestedItems.insert(item->item, model->indexOf(item->item, q));
    }
    if (item->section) {
        int i = 0;
        do {
            if (!sectionCache[i]) {
                sectionCache[i] = item->section;
                sectionCache[i]->setVisible(false);
                item->section = 0;
                break;
            }
            ++i;
        } while (i < sectionCacheSize);
        delete item->section;
    }
    delete item;
}

void QSGListViewPrivate::refill(qreal from, qreal to, bool doBuffer)
{
    Q_Q(QSGListView);
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

    int modelIndex = visibleIndex;
    qreal itemEnd = visiblePos-1;
    if (!visibleItems.isEmpty()) {
        visiblePos = (*visibleItems.constBegin())->position();
        itemEnd = (*(--visibleItems.constEnd()))->endPosition() + spacing;
        int i = visibleItems.count() - 1;
        while (i > 0 && visibleItems.at(i)->index == -1)
            --i;
        if (visibleItems.at(i)->index != -1)
            modelIndex = visibleItems.at(i)->index + 1;
    }

    bool changed = false;
    FxListItemSG *item = 0;
    qreal pos = itemEnd + 1;
    while (modelIndex < model->count() && pos <= fillTo) {
//        qDebug() << "refill: append item" << modelIndex << "pos" << pos;
        if (!(item = createItem(modelIndex)))
            break;
        item->setPosition(pos);
        pos += item->size() + spacing;
        visibleItems.append(item);
        ++modelIndex;
        changed = true;
        if (doBuffer) // never buffer more than one item per frame
            break;
    }
    while (visibleIndex > 0 && visibleIndex <= model->count() && visiblePos-1 >= fillFrom) {
//        qDebug() << "refill: prepend item" << visibleIndex-1 << "current top pos" << visiblePos;
        if (!(item = createItem(visibleIndex-1)))
            break;
        --visibleIndex;
        visiblePos -= item->size() + spacing;
        item->setPosition(visiblePos);
        visibleItems.prepend(item);
        changed = true;
        if (doBuffer) // never buffer more than one item per frame
            break;
    }

    if (!lazyRelease || !changed || deferredRelease) { // avoid destroying items in the same frame that we create
        while (visibleItems.count() > 1 && (item = visibleItems.first()) && item->endPosition() < bufferFrom) {
            if (item->attached->delayRemove())
                break;
//            qDebug() << "refill: remove first" << visibleIndex << "top end pos" << item->endPosition();
            if (item->index != -1)
                visibleIndex++;
            visibleItems.removeFirst();
            releaseItem(item);
            changed = true;
        }
        while (visibleItems.count() > 1 && (item = visibleItems.last()) && item->position() > bufferTo) {
            if (item->attached->delayRemove())
                break;
//            qDebug() << "refill: remove last" << visibleIndex+visibleItems.count()-1 << item->position();
            visibleItems.removeLast();
            releaseItem(item);
            changed = true;
        }
        deferredRelease = false;
    } else {
        deferredRelease = true;
    }
    if (changed) {
        minExtentDirty = true;
        maxExtentDirty = true;
        if (visibleItems.count())
            visiblePos = (*visibleItems.constBegin())->position();
        updateAverage();
        if (currentIndex >= 0 && currentItem && !visibleItem(currentIndex)) {
            currentItem->setPosition(positionAt(currentIndex));
            updateHighlight();
        }

        if (sectionCriteria)
            updateCurrentSection();
        if (header)
            updateHeader();
        if (footer)
            updateFooter();
        updateViewport();
        updateUnrequestedPositions();
    } else if (!doBuffer && buffer && bufferMode != NoBuffer) {
        refill(from, to, true);
    }
    lazyRelease = false;
}

void QSGListViewPrivate::scheduleLayout()
{
    Q_Q(QSGListView);
    if (!layoutScheduled) {
        layoutScheduled = true;
        q->polish();
    }
}

void QSGListViewPrivate::layout()
{
    Q_Q(QSGListView);
    layoutScheduled = false;
    if (!isValid() && !visibleItems.count()) {
        clear();
        setPosition(0);
        return;
    }
    if (!visibleItems.isEmpty()) {
        bool fixedCurrent = currentItem && visibleItems.first()->item == currentItem->item;
        qreal sum = visibleItems.first()->size();
        qreal pos = visibleItems.first()->position() + visibleItems.first()->size() + spacing;
        for (int i=1; i < visibleItems.count(); ++i) {
            FxListItemSG *item = visibleItems.at(i);
            item->setPosition(pos);
            pos += item->size() + spacing;
            sum += item->size();
            fixedCurrent = fixedCurrent || (currentItem && item->item == currentItem->item);
        }
        averageSize = qRound(sum / visibleItems.count());
        // move current item if it is not a visible item.
        if (currentIndex >= 0 && currentItem && !fixedCurrent)
            currentItem->setPosition(positionAt(currentIndex));
    }
    q->refill();
    minExtentDirty = true;
    maxExtentDirty = true;
    updateHighlight();
    if (!q->isMoving() && !q->isFlicking()) {
        fixupPosition();
        q->refill();
    }
    if (header)
        updateHeader();
    if (footer)
        updateFooter();
    updateViewport();
}

void QSGListViewPrivate::updateUnrequestedIndexes()
{
    Q_Q(QSGListView);
    QHash<QSGItem*,int>::iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it)
        *it = model->indexOf(it.key(), q);
}

void QSGListViewPrivate::updateUnrequestedPositions()
{
    Q_Q(QSGListView);
    if (unrequestedItems.count()) {
        qreal pos = position();
        QHash<QSGItem*,int>::const_iterator it;
        for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it) {
            QSGItem *item = it.key();
            if (orient == QSGListView::Vertical) {
                if (item->y() + item->height() > pos && item->y() < pos + q->height())
                    item->setY(positionAt(*it));
            } else {
                if (item->x() + item->width() > pos && item->x() < pos + q->width()) {
                    if (isRightToLeft())
                        item->setX(-positionAt(*it)-item->width());
                    else
                        item->setX(positionAt(*it));
                }
            }
        }
    }
}

void QSGListViewPrivate::updateTrackedItem()
{
    Q_Q(QSGListView);
    FxListItemSG *item = currentItem;
    if (highlight)
        item = highlight;
    trackedItem = item;
    if (trackedItem)
        q->trackedPositionChanged();
}

void QSGListViewPrivate::createHighlight()
{
    Q_Q(QSGListView);
    bool changed = false;
    if (highlight) {
        if (trackedItem == highlight)
            trackedItem = 0;
        delete highlight->item;
        delete highlight;
        highlight = 0;
        delete highlightPosAnimator;
        delete highlightSizeAnimator;
        highlightPosAnimator = 0;
        highlightSizeAnimator = 0;
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
        }
        if (item) {
            QDeclarative_setParent_noEvent(item, q->contentItem());
            item->setParentItem(q->contentItem());
            highlight = new FxListItemSG(item, q);
            if (currentItem && autoHighlight) {
                if (orient == QSGListView::Vertical) {
                    highlight->item->setHeight(currentItem->item->height());
                } else {
                    highlight->item->setWidth(currentItem->item->width());
                }
                highlight->setPosition(currentItem->itemPosition());
            }
            QSGItemPrivate *itemPrivate = QSGItemPrivate::get(item);
            itemPrivate->addItemChangeListener(this, QSGItemPrivate::Geometry);
            const QLatin1String posProp(orient == QSGListView::Vertical ? "y" : "x");
            highlightPosAnimator = new QSmoothedAnimation(q);
            highlightPosAnimator->target = QDeclarativeProperty(highlight->item, posProp);
            highlightPosAnimator->velocity = highlightMoveSpeed;
            highlightPosAnimator->userDuration = highlightMoveDuration;
            const QLatin1String sizeProp(orient == QSGListView::Vertical ? "height" : "width");
            highlightSizeAnimator = new QSmoothedAnimation(q);
            highlightSizeAnimator->velocity = highlightResizeSpeed;
            highlightSizeAnimator->userDuration = highlightResizeDuration;
            highlightSizeAnimator->target = QDeclarativeProperty(highlight->item, sizeProp);
            if (autoHighlight) {
                highlightPosAnimator->restart();
                highlightSizeAnimator->restart();
            }
            changed = true;
        }
    }
    if (changed)
        emit q->highlightItemChanged();
}

void QSGListViewPrivate::updateHighlight()
{
    if ((!currentItem && highlight) || (currentItem && !highlight))
        createHighlight();
    if (currentItem && autoHighlight && highlight && !movingHorizontally && !movingVertically) {
        // auto-update highlight
        highlightPosAnimator->to = isRightToLeft()
                ? -currentItem->itemPosition()-currentItem->itemSize()
                : currentItem->itemPosition();
        highlightSizeAnimator->to = currentItem->itemSize();
        if (orient == QSGListView::Vertical) {
            if (highlight->item->width() == 0)
                highlight->item->setWidth(currentItem->item->width());
        } else {
            if (highlight->item->height() == 0)
                highlight->item->setHeight(currentItem->item->height());
        }
        highlightPosAnimator->restart();
        highlightSizeAnimator->restart();
    }
    updateTrackedItem();
}

void QSGListViewPrivate::createSection(FxListItemSG *listItem)
{
    Q_Q(QSGListView);
    if (!sectionCriteria || !sectionCriteria->delegate())
        return;
    if (listItem->attached->m_prevSection != listItem->attached->m_section) {
        if (!listItem->section) {
            qreal pos = listItem->position();
            int i = sectionCacheSize-1;
            while (i >= 0 && !sectionCache[i])
                --i;
            if (i >= 0) {
                listItem->section = sectionCache[i];
                sectionCache[i] = 0;
                listItem->section->setVisible(true);
                QDeclarativeContext *context = QDeclarativeEngine::contextForObject(listItem->section)->parentContext();
                context->setContextProperty(QLatin1String("section"), listItem->attached->m_section);
            } else {
                QDeclarativeContext *context = new QDeclarativeContext(qmlContext(q));
                context->setContextProperty(QLatin1String("section"), listItem->attached->m_section);
                QObject *nobj = sectionCriteria->delegate()->beginCreate(context);
                if (nobj) {
                    QDeclarative_setParent_noEvent(context, nobj);
                    listItem->section = qobject_cast<QSGItem *>(nobj);
                    if (!listItem->section) {
                        delete nobj;
                    } else {
                        listItem->section->setZ(1);
                        QDeclarative_setParent_noEvent(listItem->section, q->contentItem());
                        listItem->section->setParentItem(q->contentItem());
                    }
                } else {
                    delete context;
                }
                sectionCriteria->delegate()->completeCreate();
            }
            listItem->setPosition(pos);
        } else {
            QDeclarativeContext *context = QDeclarativeEngine::contextForObject(listItem->section)->parentContext();
            context->setContextProperty(QLatin1String("section"), listItem->attached->m_section);
        }
    } else if (listItem->section) {
        qreal pos = listItem->position();
        int i = 0;
        do {
            if (!sectionCache[i]) {
                sectionCache[i] = listItem->section;
                sectionCache[i]->setVisible(false);
                listItem->section = 0;
                return;
            }
            ++i;
        } while (i < sectionCacheSize);
        delete listItem->section;
        listItem->section = 0;
        listItem->setPosition(pos);
    }
}

void QSGListViewPrivate::updateSections()
{
    if (sectionCriteria && !visibleItems.isEmpty()) {
        QString prevSection;
        if (visibleIndex > 0)
            prevSection = sectionAt(visibleIndex-1);
        QSGListViewAttached *prevAtt = 0;
        int idx = -1;
        for (int i = 0; i < visibleItems.count(); ++i) {
            QSGListViewAttached *attached = visibleItems.at(i)->attached;
            attached->setPrevSection(prevSection);
            if (visibleItems.at(i)->index != -1) {
                QString propValue = model->stringValue(visibleItems.at(i)->index, sectionCriteria->property());
                attached->setSection(sectionCriteria->sectionString(propValue));
                idx = visibleItems.at(i)->index;
            }
            createSection(visibleItems.at(i));
            if (prevAtt)
                prevAtt->setNextSection(attached->section());
            prevSection = attached->section();
            prevAtt = attached;
        }
        if (prevAtt) {
            if (idx > 0 && idx < model->count()-1)
                prevAtt->setNextSection(sectionAt(idx+1));
            else
                prevAtt->setNextSection(QString());
        }
    }
}

void QSGListViewPrivate::updateCurrentSection()
{
    Q_Q(QSGListView);
    if (!sectionCriteria || visibleItems.isEmpty()) {
        if (!currentSection.isEmpty()) {
            currentSection.clear();
            emit q->currentSectionChanged();
        }
        return;
    }
    int index = 0;
    while (index < visibleItems.count() && visibleItems.at(index)->endPosition() < position())
        ++index;

    QString newSection = currentSection;
    if (index < visibleItems.count())
        newSection = visibleItems.at(index)->attached->section();
    else
        newSection = visibleItems.first()->attached->section();
    if (newSection != currentSection) {
        currentSection = newSection;
        emit q->currentSectionChanged();
    }
}

void QSGListViewPrivate::updateCurrent(int modelIndex)
{
    Q_Q(QSGListView);
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
    FxListItemSG *oldCurrentItem = currentItem;
    currentIndex = modelIndex;
    currentItem = createItem(modelIndex);
    if (oldCurrentItem && (!currentItem || oldCurrentItem->item != currentItem->item))
        oldCurrentItem->attached->setIsCurrentItem(false);
    if (currentItem) {
        if (modelIndex == visibleIndex - 1 && visibleItems.count()) {
            // We can calculate exact postion in this case
            currentItem->setPosition(visibleItems.first()->position() - currentItem->size() - spacing);
        } else {
            // Create current item now and position as best we can.
            // Its position will be corrected when it becomes visible.
            currentItem->setPosition(positionAt(modelIndex));
        }
        currentItem->item->setFocus(true);
        currentItem->attached->setIsCurrentItem(true);
        // Avoid showing section delegate twice.  We still need the section heading so that
        // currentItem positioning works correctly.
        // This is slightly sub-optimal, but section heading caching minimizes the impact.
        if (currentItem->section)
            currentItem->section->setVisible(false);
        if (visibleItems.isEmpty())
            averageSize = currentItem->size();
    }
    updateHighlight();
    emit q->currentIndexChanged();
    // Release the old current item
    releaseItem(oldCurrentItem);
}

void QSGListViewPrivate::updateAverage()
{
    if (!visibleItems.count())
        return;
    qreal sum = 0.0;
    for (int i = 0; i < visibleItems.count(); ++i)
        sum += visibleItems.at(i)->size();
    averageSize = qRound(sum / visibleItems.count());
}

void QSGListViewPrivate::updateFooter()
{
    Q_Q(QSGListView);
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
            footer = new FxListItemSG(item, q);
        }
    }
    if (footer) {
        if (visibleItems.count()) {
            qreal endPos = lastPosition() + 1;
            if (lastVisibleIndex() == model->count()-1) {
                footer->setPosition(endPos);
            } else {
                qreal visiblePos = position() + q->height();
                if (endPos <= visiblePos || footer->position() < endPos)
                    footer->setPosition(endPos);
            }
        } else {
            footer->setPosition(visiblePos);
        }
    }
}

void QSGListViewPrivate::updateHeader()
{
    Q_Q(QSGListView);
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
            header = new FxListItemSG(item, q);
        }
    }
    if (header) {
        if (visibleItems.count()) {
            qreal startPos = originPosition();
            if (visibleIndex == 0) {
                header->setPosition(startPos - header->size());
            } else {
                if (position() <= startPos || header->position() > startPos - header->size())
                    header->setPosition(startPos - header->size());
            }
        } else {
            if (itemCount == 0)
                visiblePos = header->size();
            header->setPosition(0);
        }
    }
}

void QSGListViewPrivate::fixupPosition()
{
    if ((haveHighlightRange && highlightRange == QSGListView::StrictlyEnforceRange)
        || snapMode != QSGListView::NoSnap)
        moveReason = Other;
    if (orient == QSGListView::Vertical)
        fixupY();
    else
        fixupX();
}

void QSGListViewPrivate::fixup(AxisData &data, qreal minExtent, qreal maxExtent)
{
    if ((orient == QSGListView::Horizontal && &data == &vData)
        || (orient == QSGListView::Vertical && &data == &hData))
        return;

    correctFlick = false;
    fixupMode = moveReason == Mouse ? fixupMode : Immediate;

    qreal highlightStart;
    qreal highlightEnd;
    qreal viewPos;
    if (isRightToLeft()) {
        // Handle Right-To-Left exceptions
        viewPos = -position()-size();
        highlightStart = highlightRangeStartValid ? size() - highlightRangeEnd : highlightRangeStart;
        highlightEnd = highlightRangeEndValid ? size() - highlightRangeStart : highlightRangeEnd;
    } else {
        viewPos = position();
        highlightStart = highlightRangeStart;
        highlightEnd = highlightRangeEnd;
    }

    if (currentItem && haveHighlightRange && highlightRange == QSGListView::StrictlyEnforceRange
            && moveReason != QSGListViewPrivate::SetIndex) {
        updateHighlight();
        qreal pos = currentItem->itemPosition();
        if (viewPos < pos + currentItem->itemSize() - highlightEnd)
            viewPos = pos + currentItem->itemSize() - highlightEnd;
        if (viewPos > pos - highlightStart)
            viewPos = pos - highlightStart;
        if (isRightToLeft())
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
    } else if (snapMode != QSGListView::NoSnap && moveReason != QSGListViewPrivate::SetIndex) {
        qreal tempPosition = isRightToLeft() ? -position()-size() : position();
        FxListItemSG *topItem = snapItemAt(tempPosition+highlightStart);
        FxListItemSG *bottomItem = snapItemAt(tempPosition+highlightEnd);
        qreal pos;
        bool isInBounds = -position() > maxExtent && -position() < minExtent;
        if (topItem && isInBounds) {
            if (topItem->index == 0 && header && tempPosition+highlightStart < header->position()+header->size()/2) {
                pos = isRightToLeft() ? - header->position() + highlightStart - size() : header->position() - highlightStart;
            } else {
                if (isRightToLeft())
                    pos = qMax(qMin(-topItem->position() + highlightStart - size(), -maxExtent), -minExtent);
                else
                    pos = qMax(qMin(topItem->position() - highlightStart, -maxExtent), -minExtent);
            }
        } else if (bottomItem && isInBounds) {
            if (isRightToLeft())
                pos = qMax(qMin(-bottomItem->position() + highlightStart - size(), -maxExtent), -minExtent);
            else
                pos = qMax(qMin(bottomItem->position() - highlightStart, -maxExtent), -minExtent);
        } else {
            QSGFlickablePrivate::fixup(data, minExtent, maxExtent);
            return;
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
    } else {
        QSGFlickablePrivate::fixup(data, minExtent, maxExtent);
    }
    fixupMode = Normal;
}

void QSGListViewPrivate::flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity)
{
    Q_Q(QSGListView);

    data.fixingUp = false;
    moveReason = Mouse;
    if ((!haveHighlightRange || highlightRange != QSGListView::StrictlyEnforceRange) && snapMode == QSGListView::NoSnap) {
        correctFlick = true;
        QSGFlickablePrivate::flick(data, minExtent, maxExtent, vSize, fixupCallback, velocity);
        return;
    }
    qreal maxDistance = 0;
    qreal dataValue = isRightToLeft() ? -data.move.value()+size() : data.move.value();
    // -ve velocity means list is moving up/left
    if (velocity > 0) {
        if (data.move.value() < minExtent) {
            if (snapMode == QSGListView::SnapOneItem) {
                if (FxListItemSG *item = isRightToLeft() ? nextVisibleItem() : firstVisibleItem())
                    maxDistance = qAbs(item->position() + dataValue);
            } else {
                maxDistance = qAbs(minExtent - data.move.value());
            }
        }
        if (snapMode == QSGListView::NoSnap && highlightRange != QSGListView::StrictlyEnforceRange)
            data.flickTarget = minExtent;
    } else {
        if (data.move.value() > maxExtent) {
            if (snapMode == QSGListView::SnapOneItem) {
                if (FxListItemSG *item = isRightToLeft() ? firstVisibleItem() : nextVisibleItem())
                    maxDistance = qAbs(item->position() + dataValue);
            } else {
                maxDistance = qAbs(maxExtent - data.move.value());
            }
        }
        if (snapMode == QSGListView::NoSnap && highlightRange != QSGListView::StrictlyEnforceRange)
            data.flickTarget = maxExtent;
    }
    bool overShoot = boundsBehavior == QSGFlickable::DragAndOvershootBounds;
    qreal highlightStart = isRightToLeft() && highlightRangeStartValid ? size()-highlightRangeEnd : highlightRangeStart;
    if (maxDistance > 0 || overShoot) {
        // These modes require the list to stop exactly on an item boundary.
        // The initial flick will estimate the boundary to stop on.
        // Since list items can have variable sizes, the boundary will be
        // reevaluated and adjusted as we approach the boundary.
        qreal v = velocity;
        if (maxVelocity != -1 && maxVelocity < qAbs(v)) {
            if (v < 0)
                v = -maxVelocity;
            else
                v = maxVelocity;
        }
        if (!flickingHorizontally && !flickingVertically) {
            // the initial flick - estimate boundary
            qreal accel = deceleration;
            qreal v2 = v * v;
            overshootDist = 0.0;
            // + averageSize/4 to encourage moving at least one item in the flick direction
            qreal dist = v2 / (accel * 2.0) + averageSize/4;
            if (maxDistance > 0)
                dist = qMin(dist, maxDistance);
            if (v > 0)
                dist = -dist;
            if ((maxDistance > 0.0 && v2 / (2.0f * maxDistance) < accel) || snapMode == QSGListView::SnapOneItem) {
                qreal distTemp = isRightToLeft() ? -dist : dist;
                data.flickTarget = -snapPosAt(-(dataValue - highlightStart) + distTemp) + highlightStart;
                data.flickTarget = isRightToLeft() ? -data.flickTarget+size() : data.flickTarget;
                if (overShoot) {
                    if (data.flickTarget >= minExtent) {
                        overshootDist = overShootDistance(v, vSize);
                        data.flickTarget += overshootDist;
                    } else if (data.flickTarget <= maxExtent) {
                        overshootDist = overShootDistance(v, vSize);
                        data.flickTarget -= overshootDist;
                    }
                }
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
            } else if (overShoot) {
                data.flickTarget = data.move.value() - dist;
                if (data.flickTarget >= minExtent) {
                    overshootDist = overShootDistance(v, vSize);
                    data.flickTarget += overshootDist;
                } else if (data.flickTarget <= maxExtent) {
                    overshootDist = overShootDistance(v, vSize);
                    data.flickTarget -= overshootDist;
                }
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
            correctFlick = true;
        } else {
            // reevaluate the target boundary.
            qreal newtarget = data.flickTarget;
            if (snapMode != QSGListView::NoSnap || highlightRange == QSGListView::StrictlyEnforceRange) {
                qreal tempFlickTarget = isRightToLeft() ? -data.flickTarget+size() : data.flickTarget;
                newtarget = -snapPosAt(-(tempFlickTarget - highlightStart)) + highlightStart;
                newtarget = isRightToLeft() ? -newtarget+size() : newtarget;
            }
            if (velocity < 0 && newtarget <= maxExtent)
                newtarget = maxExtent - overshootDist;
            else if (velocity > 0 && newtarget >= minExtent)
                newtarget = minExtent + overshootDist;
            if (newtarget == data.flickTarget) { // boundary unchanged - nothing to do
                if (qAbs(velocity) < MinimumFlickVelocity)
                    correctFlick = false;
                return;
            }
            data.flickTarget = newtarget;
            qreal dist = -newtarget + data.move.value();
            if ((v < 0 && dist < 0) || (v > 0 && dist > 0)) {
                correctFlick = false;
                timeline.reset(data.move);
                fixup(data, minExtent, maxExtent);
                return;
            }
            timeline.reset(data.move);
            timeline.accelDistance(data.move, v, -dist);
            timeline.callback(QDeclarativeTimeLineCallback(&data.move, fixupCallback, this));
        }
    } else {
        correctFlick = false;
        timeline.reset(data.move);
        fixup(data, minExtent, maxExtent);
    }
}

//----------------------------------------------------------------------------

QSGListView::QSGListView(QSGItem *parent)
    : QSGFlickable(*(new QSGListViewPrivate), parent)
{
    Q_D(QSGListView);
    d->init();
}

QSGListView::~QSGListView()
{
    Q_D(QSGListView);
    d->clear();
    if (d->ownModel)
        delete d->model;
    delete d->header;
    delete d->footer;
}

QVariant QSGListView::model() const
{
    Q_D(const QSGListView);
    return d->modelVariant;
}

void QSGListView::setModel(const QVariant &model)
{
    Q_D(QSGListView);
    if (d->modelVariant == model)
        return;
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(itemsChanged(int,int)), this, SLOT(itemsChanged(int,int)));
        disconnect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        disconnect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
    }
    d->clear();
    QSGVisualModel *oldModel = d->model;
    d->model = 0;
    d->setPosition(0);
    d->modelVariant = model;
    QObject *object = qvariant_cast<QObject*>(model);
    QSGVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QSGVisualModel *>(object))) {
        if (d->ownModel) {
            delete oldModel;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QSGVisualDataModel(qmlContext(this), this);
            d->ownModel = true;
        } else {
            d->model = oldModel;
        }
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        d->bufferMode = QSGListViewPrivate::BufferBefore | QSGListViewPrivate::BufferAfter;
        if (isComponentComplete()) {
            updateSections();
            refill();
            if ((d->currentIndex >= d->model->count() || d->currentIndex < 0) && !d->currentIndexCleared) {
                setCurrentIndex(0);
            } else {
                d->moveReason = QSGListViewPrivate::SetIndex;
                d->updateCurrent(d->currentIndex);
                if (d->highlight && d->currentItem) {
                    if (d->autoHighlight)
                        d->highlight->setPosition(d->currentItem->position());
                    d->updateTrackedItem();
                }
            }
            d->updateViewport();
        }
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(itemsChanged(int,int)), this, SLOT(itemsChanged(int,int)));
        connect(d->model, SIGNAL(modelReset()), this, SLOT(modelReset()));
        connect(d->model, SIGNAL(createdItem(int,QSGItem*)), this, SLOT(createdItem(int,QSGItem*)));
        connect(d->model, SIGNAL(destroyingItem(QSGItem*)), this, SLOT(destroyingItem(QSGItem*)));
        emit countChanged();
    }
    emit modelChanged();
}

QDeclarativeComponent *QSGListView::delegate() const
{
    Q_D(const QSGListView);
    if (d->model) {
        if (QSGVisualDataModel *dataModel = qobject_cast<QSGVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QSGListView::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QSGListView);
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
            updateSections();
            refill();
            d->moveReason = QSGListViewPrivate::SetIndex;
            d->updateCurrent(d->currentIndex);
            if (d->highlight && d->currentItem) {
                if (d->autoHighlight)
                    d->highlight->setPosition(d->currentItem->position());
                d->updateTrackedItem();
            }
            d->updateViewport();
        }
    }
    emit delegateChanged();
}

int QSGListView::currentIndex() const
{
    Q_D(const QSGListView);
    return d->currentIndex;
}

void QSGListView::setCurrentIndex(int index)
{
    Q_D(QSGListView);
    if (d->requestedIndex >= 0)  // currently creating item
        return;
    d->currentIndexCleared = (index == -1);
    if (index == d->currentIndex)
        return;
    if (isComponentComplete() && d->isValid()) {
        d->moveReason = QSGListViewPrivate::SetIndex;
        d->updateCurrent(index);
    } else if (d->currentIndex != index) {
        d->currentIndex = index;
        emit currentIndexChanged();
    }
}

QSGItem *QSGListView::currentItem()
{
    Q_D(QSGListView);
    if (!d->currentItem)
        return 0;
    return d->currentItem->item;
}

QSGItem *QSGListView::highlightItem()
{
    Q_D(QSGListView);
    if (!d->highlight)
        return 0;
    return d->highlight->item;
}

int QSGListView::count() const
{
    Q_D(const QSGListView);
    if (d->model)
        return d->model->count();
    return 0;
}

QDeclarativeComponent *QSGListView::highlight() const
{
    Q_D(const QSGListView);
    return d->highlightComponent;
}

void QSGListView::setHighlight(QDeclarativeComponent *highlight)
{
    Q_D(QSGListView);
    if (highlight != d->highlightComponent) {
        d->highlightComponent = highlight;
        d->createHighlight();
        if (d->currentItem)
            d->updateHighlight();
        emit highlightChanged();
    }
}

bool QSGListView::highlightFollowsCurrentItem() const
{
    Q_D(const QSGListView);
    return d->autoHighlight;
}

void QSGListView::setHighlightFollowsCurrentItem(bool autoHighlight)
{
    Q_D(QSGListView);
    if (d->autoHighlight != autoHighlight) {
        d->autoHighlight = autoHighlight;
        if (autoHighlight) {
            d->updateHighlight();
        } else {
            if (d->highlightPosAnimator)
                d->highlightPosAnimator->stop();
            if (d->highlightSizeAnimator)
                d->highlightSizeAnimator->stop();
        }
        emit highlightFollowsCurrentItemChanged();
    }
}

//###Possibly rename these properties, since they are very useful even without a highlight?
qreal QSGListView::preferredHighlightBegin() const
{
    Q_D(const QSGListView);
    return d->highlightRangeStart;
}

void QSGListView::setPreferredHighlightBegin(qreal start)
{
    Q_D(QSGListView);
    d->highlightRangeStartValid = true;
    if (d->highlightRangeStart == start)
        return;
    d->highlightRangeStart = start;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit preferredHighlightBeginChanged();
}

void QSGListView::resetPreferredHighlightBegin()
{
    Q_D(QSGListView);
    d->highlightRangeStartValid = false;
    if (d->highlightRangeStart == 0)
        return;
    d->highlightRangeStart = 0;
    emit preferredHighlightBeginChanged();
}

qreal QSGListView::preferredHighlightEnd() const
{
    Q_D(const QSGListView);
    return d->highlightRangeEnd;
}

void QSGListView::setPreferredHighlightEnd(qreal end)
{
    Q_D(QSGListView);
    d->highlightRangeEndValid = true;
    if (d->highlightRangeEnd == end)
        return;
    d->highlightRangeEnd = end;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit preferredHighlightEndChanged();
}

void QSGListView::resetPreferredHighlightEnd()
{
    Q_D(QSGListView);
    d->highlightRangeEndValid = false;
    if (d->highlightRangeEnd == 0)
        return;
    d->highlightRangeEnd = 0;
    emit preferredHighlightEndChanged();
}

QSGListView::HighlightRangeMode QSGListView::highlightRangeMode() const
{
    Q_D(const QSGListView);
    return d->highlightRange;
}

void QSGListView::setHighlightRangeMode(HighlightRangeMode mode)
{
    Q_D(QSGListView);
    if (d->highlightRange == mode)
        return;
    d->highlightRange = mode;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
    emit highlightRangeModeChanged();
}

qreal QSGListView::spacing() const
{
    Q_D(const QSGListView);
    return d->spacing;
}

void QSGListView::setSpacing(qreal spacing)
{
    Q_D(QSGListView);
    if (spacing != d->spacing) {
        d->spacing = spacing;
        d->layout();
        emit spacingChanged();
    }
}

QSGListView::Orientation QSGListView::orientation() const
{
    Q_D(const QSGListView);
    return d->orient;
}

void QSGListView::setOrientation(QSGListView::Orientation orientation)
{
    Q_D(QSGListView);
    if (d->orient != orientation) {
        d->orient = orientation;
        if (d->orient == QSGListView::Vertical) {
            setContentWidth(-1);
            setFlickableDirection(VerticalFlick);
        } else {
            setContentHeight(-1);
            setFlickableDirection(HorizontalFlick);
        }
        d->regenerate();
        emit orientationChanged();
    }
}

Qt::LayoutDirection QSGListView::layoutDirection() const
{
    Q_D(const QSGListView);
    return d->layoutDirection;
}

void QSGListView::setLayoutDirection(Qt::LayoutDirection layoutDirection)
{
    Q_D(QSGListView);
    if (d->layoutDirection != layoutDirection) {
        d->layoutDirection = layoutDirection;
        d->regenerate();
        emit layoutDirectionChanged();
        emit effectiveLayoutDirectionChanged();
    }
}

Qt::LayoutDirection QSGListView::effectiveLayoutDirection() const
{
    Q_D(const QSGListView);
    if (d->effectiveLayoutMirror)
        return d->layoutDirection == Qt::RightToLeft ? Qt::LeftToRight : Qt::RightToLeft;
    else
        return d->layoutDirection;
}

bool QSGListView::isWrapEnabled() const
{
    Q_D(const QSGListView);
    return d->wrap;
}

void QSGListView::setWrapEnabled(bool wrap)
{
    Q_D(QSGListView);
    if (d->wrap == wrap)
        return;
    d->wrap = wrap;
    emit keyNavigationWrapsChanged();
}

int QSGListView::cacheBuffer() const
{
    Q_D(const QSGListView);
    return d->buffer;
}

void QSGListView::setCacheBuffer(int b)
{
    Q_D(QSGListView);
    if (d->buffer != b) {
        d->buffer = b;
        if (isComponentComplete()) {
            d->bufferMode = QSGListViewPrivate::BufferBefore | QSGListViewPrivate::BufferAfter;
            refill();
        }
        emit cacheBufferChanged();
    }
}

QSGViewSection *QSGListView::sectionCriteria()
{
    Q_D(QSGListView);
    if (!d->sectionCriteria) {
        d->sectionCriteria = new QSGViewSection(this);
        connect(d->sectionCriteria, SIGNAL(propertyChanged()), this, SLOT(updateSections()));
    }
    return d->sectionCriteria;
}

QString QSGListView::currentSection() const
{
    Q_D(const QSGListView);
    return d->currentSection;
}

qreal QSGListView::highlightMoveSpeed() const
{
    Q_D(const QSGListView);\
    return d->highlightMoveSpeed;
}

void QSGListView::setHighlightMoveSpeed(qreal speed)
{
    Q_D(QSGListView);\
    if (d->highlightMoveSpeed != speed) {
        d->highlightMoveSpeed = speed;
        if (d->highlightPosAnimator)
            d->highlightPosAnimator->velocity = d->highlightMoveSpeed;
        emit highlightMoveSpeedChanged();
    }
}

int QSGListView::highlightMoveDuration() const
{
    Q_D(const QSGListView);
    return d->highlightMoveDuration;
}

void QSGListView::setHighlightMoveDuration(int duration)
{
    Q_D(QSGListView);\
    if (d->highlightMoveDuration != duration) {
        d->highlightMoveDuration = duration;
        if (d->highlightPosAnimator)
            d->highlightPosAnimator->userDuration = d->highlightMoveDuration;
        emit highlightMoveDurationChanged();
    }
}

qreal QSGListView::highlightResizeSpeed() const
{
    Q_D(const QSGListView);\
    return d->highlightResizeSpeed;
}

void QSGListView::setHighlightResizeSpeed(qreal speed)
{
    Q_D(QSGListView);\
    if (d->highlightResizeSpeed != speed) {
        d->highlightResizeSpeed = speed;
        if (d->highlightSizeAnimator)
            d->highlightSizeAnimator->velocity = d->highlightResizeSpeed;
        emit highlightResizeSpeedChanged();
    }
}

int QSGListView::highlightResizeDuration() const
{
    Q_D(const QSGListView);
    return d->highlightResizeDuration;
}

void QSGListView::setHighlightResizeDuration(int duration)
{
    Q_D(QSGListView);\
    if (d->highlightResizeDuration != duration) {
        d->highlightResizeDuration = duration;
        if (d->highlightSizeAnimator)
            d->highlightSizeAnimator->userDuration = d->highlightResizeDuration;
        emit highlightResizeDurationChanged();
    }
}

QSGListView::SnapMode QSGListView::snapMode() const
{
    Q_D(const QSGListView);
    return d->snapMode;
}

void QSGListView::setSnapMode(SnapMode mode)
{
    Q_D(QSGListView);
    if (d->snapMode != mode) {
        d->snapMode = mode;
        emit snapModeChanged();
    }
}

QDeclarativeComponent *QSGListView::footer() const
{
    Q_D(const QSGListView);
    return d->footerComponent;
}

void QSGListView::setFooter(QDeclarativeComponent *footer)
{
    Q_D(QSGListView);
    if (d->footerComponent != footer) {
        if (d->footer) {
            // XXX todo - the original did scene()->removeItem().  Why?
            d->footer->item->setParentItem(0);
            d->footer->item->deleteLater();
            delete d->footer;
            d->footer = 0;
        }
        d->footerComponent = footer;
        d->minExtentDirty = true;
        d->maxExtentDirty = true;
        if (isComponentComplete()) {
            d->updateFooter();
            d->updateViewport();
            d->fixupPosition();
        }
        emit footerChanged();
    }
}

QDeclarativeComponent *QSGListView::header() const
{
    Q_D(const QSGListView);
    return d->headerComponent;
}

void QSGListView::setHeader(QDeclarativeComponent *header)
{
    Q_D(QSGListView);
    if (d->headerComponent != header) {
        if (d->header) {
            // XXX todo - the original did scene()->removeItem().  Why?
            d->header->item->setParentItem(0);
            d->header->item->deleteLater();
            delete d->header;
            d->header = 0;
        }
        d->headerComponent = header;
        d->minExtentDirty = true;
        d->maxExtentDirty = true;
        if (isComponentComplete()) {
            d->updateHeader();
            d->updateFooter();
            d->updateViewport();
            d->fixupPosition();
        }
        emit headerChanged();
    }
}

void QSGListView::setContentX(qreal pos)
{
    Q_D(QSGListView);
    // Positioning the view manually should override any current movement state
    d->moveReason = QSGListViewPrivate::Other;
    QSGFlickable::setContentX(pos);
}

void QSGListView::setContentY(qreal pos)
{
    Q_D(QSGListView);
    // Positioning the view manually should override any current movement state
    d->moveReason = QSGListViewPrivate::Other;
    QSGFlickable::setContentY(pos);
}

void QSGListView::updatePolish()
{
    Q_D(QSGListView);
    QSGFlickable::updatePolish();
    d->layout();
}

void QSGListView::viewportMoved()
{
    Q_D(QSGListView);
    QSGFlickable::viewportMoved();
    if (!d->itemCount)
        return;
    // Recursion can occur due to refill changing the content size.
    if (d->inViewportMoved)
        return;
    d->inViewportMoved = true;
    d->lazyRelease = true;
    refill();
    if (d->flickingHorizontally || d->flickingVertically || d->movingHorizontally || d->movingVertically)
        d->moveReason = QSGListViewPrivate::Mouse;
    if (d->moveReason != QSGListViewPrivate::SetIndex) {
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange && d->highlight) {
            // reposition highlight
            qreal pos = d->highlight->position();
            qreal viewPos;
            qreal highlightStart;
            qreal highlightEnd;
            if (d->isRightToLeft()) {
                // Handle Right-To-Left exceptions
                viewPos = -d->position()-d->size();
                highlightStart = d->highlightRangeStartValid ? d->size()-d->highlightRangeEnd : d->highlightRangeStart;
                highlightEnd = d->highlightRangeEndValid ? d->size()-d->highlightRangeStart : d->highlightRangeEnd;
            } else {
                viewPos = d->position();
                highlightStart = d->highlightRangeStart;
                highlightEnd = d->highlightRangeEnd;
            }
            if (pos > viewPos + highlightEnd - d->highlight->size())
                pos = viewPos + highlightEnd - d->highlight->size();
            if (pos < viewPos + highlightStart)
                pos = viewPos + highlightStart;
            d->highlightPosAnimator->stop();
            d->highlight->setPosition(qRound(pos));

            // update current index
            if (FxListItemSG *snapItem = d->snapItemAt(d->highlight->position())) {
                if (snapItem->index >= 0 && snapItem->index != d->currentIndex)
                    d->updateCurrent(snapItem->index);
            }
        }
    }

    if ((d->flickingHorizontally || d->flickingVertically) && d->correctFlick && !d->inFlickCorrection) {
        d->inFlickCorrection = true;
        // Near an end and it seems that the extent has changed?
        // Recalculate the flick so that we don't end up in an odd position.
        if (yflick()) {
            if (d->vData.velocity > 0) {
                const qreal minY = minYExtent();
                if ((minY - d->vData.move.value() < height()/2 || d->vData.flickTarget - d->vData.move.value() < height()/2)
                    && minY != d->vData.flickTarget)
                    d->flickY(-d->vData.smoothVelocity.value());
                d->bufferMode = QSGListViewPrivate::BufferBefore;
            } else if (d->vData.velocity < 0) {
                const qreal maxY = maxYExtent();
                if ((d->vData.move.value() - maxY < height()/2 || d->vData.move.value() - d->vData.flickTarget < height()/2)
                    && maxY != d->vData.flickTarget)
                    d->flickY(-d->vData.smoothVelocity.value());
                d->bufferMode = QSGListViewPrivate::BufferAfter;
            }
        }

        if (xflick()) {
            if (d->hData.velocity > 0) {
                const qreal minX = minXExtent();
                if ((minX - d->hData.move.value() < width()/2 || d->hData.flickTarget - d->hData.move.value() < width()/2)
                    && minX != d->hData.flickTarget)
                    d->flickX(-d->hData.smoothVelocity.value());
                d->bufferMode = d->isRightToLeft() ? QSGListViewPrivate::BufferAfter : QSGListViewPrivate::BufferBefore;
            } else if (d->hData.velocity < 0) {
                const qreal maxX = maxXExtent();
                if ((d->hData.move.value() - maxX < width()/2 || d->hData.move.value() - d->hData.flickTarget < width()/2)
                    && maxX != d->hData.flickTarget)
                    d->flickX(-d->hData.smoothVelocity.value());
                d->bufferMode = d->isRightToLeft() ? QSGListViewPrivate::BufferBefore : QSGListViewPrivate::BufferAfter;
            }
        }
        d->inFlickCorrection = false;
    }
    d->inViewportMoved = false;
}

qreal QSGListView::minYExtent() const
{
    Q_D(const QSGListView);
    if (d->orient == QSGListView::Horizontal)
        return QSGFlickable::minYExtent();
    if (d->minExtentDirty) {
        d->minExtent = -d->startPosition();
        if (d->header && d->visibleItems.count())
            d->minExtent += d->header->size();
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
            d->minExtent += d->highlightRangeStart;
            if (d->sectionCriteria) {
                if (d->visibleItem(0))
                    d->minExtent -= d->visibleItem(0)->sectionSize();
            }
            d->minExtent = qMax(d->minExtent, -(d->endPositionAt(0) - d->highlightRangeEnd + 1));
        }
        d->minExtentDirty = false;
    }

    return d->minExtent;
}

qreal QSGListView::maxYExtent() const
{
    Q_D(const QSGListView);
    if (d->orient == QSGListView::Horizontal)
        return height();
    if (d->maxExtentDirty) {
        if (!d->model || !d->model->count()) {
            d->maxExtent = d->header ? -d->header->size() : 0;
            d->maxExtent += height();
        } else if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
            d->maxExtent = -(d->positionAt(d->model->count()-1) - d->highlightRangeStart);
            if (d->highlightRangeEnd != d->highlightRangeStart)
                d->maxExtent = qMin(d->maxExtent, -(d->endPosition() - d->highlightRangeEnd + 1));
        } else {
            d->maxExtent = -(d->endPosition() - height() + 1);
        }
        if (d->footer)
            d->maxExtent -= d->footer->size();
        qreal minY = minYExtent();
        if (d->maxExtent > minY)
            d->maxExtent = minY;
        d->maxExtentDirty = false;
    }
    return d->maxExtent;
}

qreal QSGListView::minXExtent() const
{
    Q_D(const QSGListView);
    if (d->orient == QSGListView::Vertical)
        return QSGFlickable::minXExtent();
    if (d->minExtentDirty) {
        d->minExtent = -d->startPosition();
        qreal highlightStart;
        qreal highlightEnd;
        qreal endPositionFirstItem = 0;
        if (d->isRightToLeft()) {
            if (d->model && d->model->count())
                endPositionFirstItem = d->positionAt(d->model->count()-1);
            else if (d->header)
                d->minExtent += d->header->size();
            highlightStart = d->highlightRangeStartValid
                    ? d->highlightRangeStart - (d->lastPosition()-endPositionFirstItem)
                    : d->size() - (d->lastPosition()-endPositionFirstItem);
            highlightEnd = d->highlightRangeEndValid ? d->highlightRangeEnd : d->size();
            if (d->footer)
                d->minExtent += d->footer->size();
            qreal maxX = maxXExtent();
            if (d->minExtent < maxX)
                d->minExtent = maxX;
        } else {
            endPositionFirstItem = d->endPositionAt(0);
            highlightStart = d->highlightRangeStart;
            highlightEnd = d->highlightRangeEnd;
            if (d->header && d->visibleItems.count())
                d->minExtent += d->header->size();
        }
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
            d->minExtent += highlightStart;
            d->minExtent = qMax(d->minExtent, -(endPositionFirstItem - highlightEnd + 1));
        }
        d->minExtentDirty = false;
    }

    return d->minExtent;
}

qreal QSGListView::maxXExtent() const
{
    Q_D(const QSGListView);
    if (d->orient == QSGListView::Vertical)
        return width();
    if (d->maxExtentDirty) {
        qreal highlightStart;
        qreal highlightEnd;
        qreal lastItemPosition = 0;
        d->maxExtent = 0;
        if (d->isRightToLeft()) {
            highlightStart = d->highlightRangeStartValid ? d->highlightRangeEnd : d->size();
            highlightEnd = d->highlightRangeEndValid ? d->highlightRangeStart : d->size();
            lastItemPosition = d->endPosition();
        } else {
            highlightStart = d->highlightRangeStart;
            highlightEnd = d->highlightRangeEnd;
            if (d->model && d->model->count())
                lastItemPosition = d->positionAt(d->model->count()-1);
        }
        if (!d->model || !d->model->count()) {
            if (!d->isRightToLeft())
                d->maxExtent = d->header ? -d->header->size() : 0;
            d->maxExtent += width();
        } else if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange) {
            d->maxExtent = -(lastItemPosition - highlightStart);
            if (highlightEnd != highlightStart) {
                d->maxExtent = d->isRightToLeft()
                        ? qMax(d->maxExtent, -(d->endPosition() - highlightEnd + 1))
                        : qMin(d->maxExtent, -(d->endPosition() - highlightEnd + 1));
            }
        } else {
            d->maxExtent = -(d->endPosition() - width() + 1);
        }
        if (d->isRightToLeft()) {
            if (d->header && d->visibleItems.count())
                d->maxExtent -= d->header->size();
        } else {
            if (d->footer)
                d->maxExtent -= d->footer->size();
            qreal minX = minXExtent();
            if (d->maxExtent > minX)
                d->maxExtent = minX;
        }
        d->maxExtentDirty = false;
    }

    return d->maxExtent;
}

void QSGListView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QSGListView);
    if (d->model && d->model->count() && d->interactive) {
        if ((!d->isRightToLeft() && event->key() == Qt::Key_Left)
                    || (d->orient == QSGListView::Horizontal && d->isRightToLeft() && event->key() == Qt::Key_Right)
                    || (d->orient == QSGListView::Vertical && event->key() == Qt::Key_Up)) {
            if (currentIndex() > 0 || (d->wrap && !event->isAutoRepeat())) {
                decrementCurrentIndex();
                event->accept();
                return;
            } else if (d->wrap) {
                event->accept();
                return;
            }
        } else if ((!d->isRightToLeft() && event->key() == Qt::Key_Right)
                    || (d->orient == QSGListView::Horizontal && d->isRightToLeft() && event->key() == Qt::Key_Left)
                    || (d->orient == QSGListView::Vertical && event->key() == Qt::Key_Down)) {
            if (currentIndex() < d->model->count() - 1 || (d->wrap && !event->isAutoRepeat())) {
                incrementCurrentIndex();
                event->accept();
                return;
            } else if (d->wrap) {
                event->accept();
                return;
            }
        }
    }
    event->ignore();
    QSGFlickable::keyPressEvent(event);
}

void QSGListView::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry)
{
    Q_D(QSGListView);
    d->maxExtentDirty = true;
    d->minExtentDirty = true;
    if (d->isRightToLeft() && d->orient == QSGListView::Horizontal) {
        // maintain position relative to the right edge
        int dx = newGeometry.width() - oldGeometry.width();
        setContentX(contentX() - dx);
    }
    QSGFlickable::geometryChanged(newGeometry, oldGeometry);
}


void QSGListView::incrementCurrentIndex()
{
    Q_D(QSGListView);
    int count = d->model ? d->model->count() : 0;
    if (count && (currentIndex() < count - 1 || d->wrap)) {
        d->moveReason = QSGListViewPrivate::SetIndex;
        int index = currentIndex()+1;
        setCurrentIndex((index >= 0 && index < count) ? index : 0);
    }
}

void QSGListView::decrementCurrentIndex()
{
    Q_D(QSGListView);
    int count = d->model ? d->model->count() : 0;
    if (count && (currentIndex() > 0 || d->wrap)) {
        d->moveReason = QSGListViewPrivate::SetIndex;
        int index = currentIndex()-1;
        setCurrentIndex((index >= 0 && index < count) ? index : count-1);
    }
}

void QSGListViewPrivate::positionViewAtIndex(int index, int mode)
{
    Q_Q(QSGListView);
    if (!isValid())
        return;
    if (mode < QSGListView::Beginning || mode > QSGListView::Contain)
        return;
    int idx = qMax(qMin(index, model->count()-1), 0);

    if (layoutScheduled)
        layout();
    qreal pos = isRightToLeft() ? -position() - size() : position();
    FxListItemSG *item = visibleItem(idx);
    qreal maxExtent;
    if (orient == QSGListView::Vertical)
        maxExtent = -q->maxYExtent();
    else
        maxExtent = isRightToLeft() ? q->minXExtent()-size(): -q->maxXExtent();
    if (!item) {
        int itemPos = positionAt(idx);
        // save the currently visible items in case any of them end up visible again
        QList<FxListItemSG*> oldVisible = visibleItems;
        visibleItems.clear();
        visiblePos = itemPos;
        visibleIndex = idx;
        setPosition(qMin(qreal(itemPos), maxExtent));
        // now release the reference to all the old visible items.
        for (int i = 0; i < oldVisible.count(); ++i)
            releaseItem(oldVisible.at(i));
        item = visibleItem(idx);
    }
    if (item) {
        const qreal itemPos = item->position();
        switch (mode) {
        case QSGListView::Beginning:
            pos = itemPos;
            if (index < 0 && header)
                pos -= header->size();
            break;
        case QSGListView::Center:
            pos = itemPos - (size() - item->size())/2;
            break;
        case QSGListView::End:
            pos = itemPos - size() + item->size();
            if (index >= model->count() && footer)
                pos += footer->size();
            break;
        case QSGListView::Visible:
            if (itemPos > pos + size())
                pos = itemPos - size() + item->size();
            else if (item->endPosition() < pos)
                pos = itemPos;
            break;
        case QSGListView::Contain:
            if (item->endPosition() > pos + size())
                pos = itemPos - size() + item->size();
            if (itemPos < pos)
                pos = itemPos;
        }
        pos = qMin(pos, maxExtent);
        qreal minExtent;
        if (orient == QSGListView::Vertical) {
            minExtent = -q->minYExtent();
        } else {
            minExtent = isRightToLeft() ? q->maxXExtent()-size(): -q->minXExtent();
        }
        pos = qMax(pos, minExtent);
        moveReason = QSGListViewPrivate::Other;
        q->cancelFlick();
        setPosition(pos);
        if (highlight) {
            if (autoHighlight) {
                highlight->setPosition(currentItem->itemPosition());
                highlight->setSize(currentItem->itemSize());
            }
            updateHighlight();
        }
    }
    fixupPosition();
}

void QSGListView::positionViewAtIndex(int index, int mode)
{
    Q_D(QSGListView);
    if (!d->isValid() || index < 0 || index >= d->model->count())
        return;
    d->positionViewAtIndex(index, mode);
}

void QSGListView::positionViewAtBeginning()
{
    Q_D(QSGListView);
    if (!d->isValid())
        return;
    d->positionViewAtIndex(-1, Beginning);
}

void QSGListView::positionViewAtEnd()
{
    Q_D(QSGListView);
    if (!d->isValid())
        return;
    d->positionViewAtIndex(d->model->count(), End);
}

int QSGListView::indexAt(qreal x, qreal y) const
{
    Q_D(const QSGListView);
    for (int i = 0; i < d->visibleItems.count(); ++i) {
        const FxListItemSG *listItem = d->visibleItems.at(i);
        if(listItem->contains(x, y))
            return listItem->index;
    }

    return -1;
}

void QSGListView::componentComplete()
{
    Q_D(QSGListView);
    QSGFlickable::componentComplete();
    updateSections();
    d->updateHeader();
    d->updateFooter();
    if (d->isValid()) {
        refill();
        d->moveReason = QSGListViewPrivate::SetIndex;
        if (d->currentIndex < 0 && !d->currentIndexCleared)
            d->updateCurrent(0);
        else
            d->updateCurrent(d->currentIndex);
        if (d->highlight && d->currentItem) {
            if (d->autoHighlight)
                d->highlight->setPosition(d->currentItem->position());
            d->updateTrackedItem();
        }
        d->moveReason = QSGListViewPrivate::Other;
        d->fixupPosition();
    }
}

void QSGListView::updateSections()
{
    Q_D(QSGListView);
    if (isComponentComplete() && d->model) {
        QList<QByteArray> roles;
        if (d->sectionCriteria && !d->sectionCriteria->property().isEmpty())
            roles << d->sectionCriteria->property().toUtf8();
        d->model->setWatchedRoles(roles);
        d->updateSections();
        if (d->itemCount)
            d->layout();
    }
}

void QSGListView::refill()
{
    Q_D(QSGListView);
    if (d->isRightToLeft())
        d->refill(-d->position()-d->size()+1, -d->position());
    else
        d->refill(d->position(), d->position()+d->size()-1);
}

void QSGListView::trackedPositionChanged()
{
    Q_D(QSGListView);
    if (!d->trackedItem || !d->currentItem)
        return;
    if (d->moveReason == QSGListViewPrivate::SetIndex) {
        qreal trackedPos = qCeil(d->trackedItem->position());
        qreal trackedSize = d->trackedItem->size();
        if (d->trackedItem != d->currentItem) {
            trackedPos -= d->currentItem->sectionSize();
            trackedSize += d->currentItem->sectionSize();
        }
        qreal viewPos;
        qreal highlightStart;
        qreal highlightEnd;
        if (d->isRightToLeft()) {
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
                if (trackedPos > pos + highlightEnd - d->trackedItem->size())
                    pos = trackedPos - highlightEnd + d->trackedItem->size();
                if (trackedPos < pos + highlightStart)
                    pos = trackedPos - highlightStart;
            } else {
                if (trackedPos < d->startPosition() + highlightStart) {
                    pos = d->startPosition();
                } else if (d->trackedItem->endPosition() > d->endPosition() - d->size() + highlightEnd) {
                    pos = d->endPosition() - d->size() + 1;
                    if (pos < d->startPosition())
                        pos = d->startPosition();
                } else {
                    if (trackedPos < viewPos + highlightStart) {
                        pos = trackedPos - highlightStart;
                    } else if (trackedPos > viewPos + highlightEnd - trackedSize) {
                        pos = trackedPos - highlightEnd + trackedSize;
                    }
                }
            }
        } else {
            if (trackedPos < viewPos && d->currentItem->position() < viewPos) {
                pos = d->currentItem->position() < trackedPos ? trackedPos : d->currentItem->position();
            } else if (d->trackedItem->endPosition() >= viewPos + d->size()
                        && d->currentItem->endPosition() >= viewPos + d->size()) {
                if (d->trackedItem->endPosition() <= d->currentItem->endPosition()) {
                    pos = d->trackedItem->endPosition() - d->size() + 1;
                     if (trackedSize > d->size())
                        pos = trackedPos;
                } else {
                    pos = d->currentItem->endPosition() - d->size() + 1;
                    if (d->currentItem->size() > d->size())
                        pos = d->currentItem->position();
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

void QSGListView::itemsInserted(int modelIndex, int count)
{
    Q_D(QSGListView);
    if (!isComponentComplete())
        return;
    d->updateUnrequestedIndexes();
    d->moveReason = QSGListViewPrivate::Other;

    qreal tempPos = d->isRightToLeft() ? -d->position()-d->size() : d->position();
    int index = d->visibleItems.count() ? d->mapFromModel(modelIndex) : 0;
    if (index < 0) {
        int i = d->visibleItems.count() - 1;
        while (i > 0 && d->visibleItems.at(i)->index == -1)
            --i;
        if (i == 0 && d->visibleItems.first()->index == -1) {
            // there are no visible items except items marked for removal
            index = d->visibleItems.count();
        } else if (d->visibleItems.at(i)->index + 1 == modelIndex
            && d->visibleItems.at(i)->endPosition() < d->buffer+tempPos+d->size()-1) {
            // Special case of appending an item to the model.
            index = d->visibleItems.count();
        } else {
            if (modelIndex < d->visibleIndex) {
                // Insert before visible items
                d->visibleIndex += count;
                for (int i = 0; i < d->visibleItems.count(); ++i) {
                    FxListItemSG *listItem = d->visibleItems.at(i);
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

    // index can be the next item past the end of the visible items list (i.e. appended)
    int pos = 0;
    if (d->visibleItems.count()) {
        pos = index < d->visibleItems.count() ? d->visibleItems.at(index)->position()
                                                : d->visibleItems.last()->endPosition()+d->spacing+1;
    } else if (d->itemCount == 0 && d->header) {
        pos = d->header->size();
    }

    int initialPos = pos;
    int diff = 0;
    QList<FxListItemSG*> added;
    bool addedVisible = false;
    FxListItemSG *firstVisible = d->firstVisibleItem();
    if (firstVisible && pos < firstVisible->position()) {
        // Insert items before the visible item.
        int insertionIdx = index;
        int i = 0;
        int from = tempPos - d->buffer;
        for (i = count-1; i >= 0 && pos > from; --i) {
            if (!addedVisible) {
                d->scheduleLayout();
                addedVisible = true;
            }
            FxListItemSG *item = d->createItem(modelIndex + i);
            d->visibleItems.insert(insertionIdx, item);
            pos -= item->size() + d->spacing;
            item->setPosition(pos);
            index++;
        }
        if (i >= 0) {
            // If we didn't insert all our new items - anything
            // before the current index is not visible - remove it.
            while (insertionIdx--) {
                FxListItemSG *item = d->visibleItems.takeFirst();
                if (item->index != -1)
                    d->visibleIndex++;
                d->releaseItem(item);
            }
        } else {
            // adjust pos of items before inserted items.
            for (int i = insertionIdx-1; i >= 0; i--) {
                FxListItemSG *listItem = d->visibleItems.at(i);
                listItem->setPosition(listItem->position() - (initialPos - pos));
            }
        }
    } else {
        int i = 0;
        int to = d->buffer+tempPos+d->size()-1;
        for (i = 0; i < count && pos <= to; ++i) {
            if (!addedVisible) {
                d->scheduleLayout();
                addedVisible = true;
            }
            FxListItemSG *item = d->createItem(modelIndex + i);
            d->visibleItems.insert(index, item);
            item->setPosition(pos);
            added.append(item);
            pos += item->size() + d->spacing;
            ++index;
        }
        if (i != count) {
            // We didn't insert all our new items, which means anything
            // beyond the current index is not visible - remove it.
            while (d->visibleItems.count() > index)
                d->releaseItem(d->visibleItems.takeLast());
        }
        diff = pos - initialPos;
    }
    if (d->itemCount && d->currentIndex >= modelIndex) {
        // adjust current item index
        d->currentIndex += count;
        if (d->currentItem) {
            d->currentItem->index = d->currentIndex;
            d->currentItem->setPosition(d->currentItem->position() + diff);
        }
        emit currentIndexChanged();
    } else if (!d->itemCount && (!d->currentIndex || (d->currentIndex < 0 && !d->currentIndexCleared))) {
        d->updateCurrent(0);
    }
    // Update the indexes of the following visible items.
    for (; index < d->visibleItems.count(); ++index) {
        FxListItemSG *listItem = d->visibleItems.at(index);
        if (d->currentItem && listItem->item != d->currentItem->item)
            listItem->setPosition(listItem->position() + diff);
        if (listItem->index != -1)
            listItem->index += count;
    }
    // everything is in order now - emit add() signal
    for (int j = 0; j < added.count(); ++j)
        added.at(j)->attached->emitAdd();

    d->updateSections();
    d->itemCount += count;
    emit countChanged();
}

void QSGListView::itemsRemoved(int modelIndex, int count)
{
    Q_D(QSGListView);
    if (!isComponentComplete())
        return;
    d->moveReason = QSGListViewPrivate::Other;
    d->updateUnrequestedIndexes();
    d->itemCount -= count;

    FxListItemSG *firstVisible = d->firstVisibleItem();
    int preRemovedSize = 0;
    bool removedVisible = false;
    // Remove the items from the visible list, skipping anything already marked for removal
    QList<FxListItemSG*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxListItemSG *item = *it;
        if (item->index == -1 || item->index < modelIndex) {
            // already removed, or before removed items
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
                if (item == firstVisible)
                    firstVisible = 0;
                if (firstVisible && item->position() < firstVisible->position())
                    preRemovedSize += item->size();
                it = d->visibleItems.erase(it);
                d->releaseItem(item);
            }
        }
    }

    if (firstVisible && d->visibleItems.first() != firstVisible)
        d->visibleItems.first()->setPosition(d->visibleItems.first()->position() + preRemovedSize);

    // fix current
    if (d->currentIndex >= modelIndex + count) {
        d->currentIndex -= count;
        if (d->currentItem)
            d->currentItem->index -= count;
        emit currentIndexChanged();
    } else if (d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count) {
        // current item has been removed.
        if (d->currentItem) {
            d->currentItem->attached->setIsCurrentItem(false);
            d->releaseItem(d->currentItem);
            d->currentItem = 0;
        }
        d->currentIndex = -1;
        if (d->itemCount)
            d->updateCurrent(qMin(modelIndex, d->itemCount-1));
        else
            emit currentIndexChanged();
    }

    // update visibleIndex
    bool haveVisibleIndex = false;
    for (it = d->visibleItems.begin(); it != d->visibleItems.end(); ++it) {
        if ((*it)->index != -1) {
            d->visibleIndex = (*it)->index;
            haveVisibleIndex = true;
            break;
        }
    }

    if (removedVisible && !haveVisibleIndex) {
        d->timeline.clear();
        if (d->itemCount == 0) {
            d->visibleIndex = 0;
            d->visiblePos = d->header ? d->header->size() : 0;
            d->setPosition(0);
            d->updateHeader();
            d->updateFooter();
        } else {
            if (modelIndex < d->visibleIndex)
                d->visibleIndex = modelIndex+1;
            d->visibleIndex = qMax(qMin(d->visibleIndex, d->itemCount-1), 0);
        }
    }

    d->updateSections();
    emit countChanged();
}

void QSGListView::destroyRemoved()
{
    Q_D(QSGListView);
    for (QList<FxListItemSG*>::Iterator it = d->visibleItems.begin();
            it != d->visibleItems.end();) {
        FxListItemSG *listItem = *it;
        if (listItem->index == -1 && listItem->attached->delayRemove() == false) {
            d->releaseItem(listItem);
            it = d->visibleItems.erase(it);
        } else {
            ++it;
        }
    }

    // Correct the positioning of the items
    d->updateSections();
    d->layout();
}

void QSGListView::itemsMoved(int from, int to, int count)
{
    Q_D(QSGListView);
    if (!isComponentComplete())
        return;
    d->updateUnrequestedIndexes();

    if (d->visibleItems.isEmpty()) {
        refill();
        return;
    }

    d->moveReason = QSGListViewPrivate::Other;
    FxListItemSG *firstVisible = d->firstVisibleItem();
    qreal firstItemPos = firstVisible->position();
    QHash<int,FxListItemSG*> moved;
    int moveBy = 0;

    QList<FxListItemSG*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxListItemSG *item = *it;
        if (item->index >= from && item->index < from + count) {
            // take the items that are moving
            item->index += (to-from);
            moved.insert(item->index, item);
            if (item->position() < firstItemPos)
                moveBy += item->size();
            it = d->visibleItems.erase(it);
        } else {
            // move everything after the moved items.
            if (item->index > from && item->index != -1)
                item->index -= count;
            ++it;
        }
    }

    int remaining = count;
    int endIndex = d->visibleIndex;
    it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxListItemSG *item = *it;
        if (remaining && item->index >= to && item->index < to + count) {
            // place items in the target position, reusing any existing items
            FxListItemSG *movedItem = moved.take(item->index);
            if (!movedItem)
                movedItem = d->createItem(item->index);
            if (item->index <= firstVisible->index)
                moveBy -= movedItem->size();
            it = d->visibleItems.insert(it, movedItem);
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
    while (FxListItemSG *item = moved.take(endIndex+1)) {
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
        FxListItemSG *item = moved.take(idx);
        if (d->currentItem && item->item == d->currentItem->item)
            item->setPosition(d->positionAt(idx));
        d->releaseItem(item);
    }

    // Ensure we don't cause an ugly list scroll.
    d->visibleItems.first()->setPosition(d->visibleItems.first()->position() + moveBy);

    d->updateSections();
    d->layout();
}

void QSGListView::itemsChanged(int, int)
{
    Q_D(QSGListView);
    d->updateSections();
    d->layout();
}

void QSGListView::modelReset()
{
    Q_D(QSGListView);
    d->moveReason = QSGListViewPrivate::SetIndex;
    d->regenerate();
    if (d->highlight && d->currentItem) {
        if (d->autoHighlight)
            d->highlight->setPosition(d->currentItem->position());
        d->updateTrackedItem();
    }
    d->moveReason = QSGListViewPrivate::Other;
    emit countChanged();
}

void QSGListView::createdItem(int index, QSGItem *item)
{
    Q_D(QSGListView);
    if (d->requestedIndex != index) {
        item->setParentItem(contentItem());
        d->unrequestedItems.insert(item, index);
        if (d->orient == QSGListView::Vertical) {
            item->setY(d->positionAt(index));
        } else {
            if (d->isRightToLeft())
                item->setX(-d->positionAt(index)-item->width());
            else
                item->setX(d->positionAt(index));
        }
    }
}

void QSGListView::destroyingItem(QSGItem *item)
{
    Q_D(QSGListView);
    d->unrequestedItems.remove(item);
}

void QSGListView::animStopped()
{
    Q_D(QSGListView);
    d->bufferMode = QSGListViewPrivate::NoBuffer;
    if (d->haveHighlightRange && d->highlightRange == QSGListView::StrictlyEnforceRange)
        d->updateHighlight();
}

QSGListViewAttached *QSGListView::qmlAttachedProperties(QObject *obj)
{
    return new QSGListViewAttached(obj);
}

QT_END_NAMESPACE
