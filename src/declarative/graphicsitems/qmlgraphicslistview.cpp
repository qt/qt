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

#include "qmlgraphicslistview_p.h"

#include "qmlgraphicsflickable_p_p.h"
#include "qmlgraphicsvisualitemmodel_p.h"

#include <qmleasefollow_p.h>
#include <qmlexpression.h>
#include <qmlengine.h>

#include <qlistmodelinterface_p.h>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE

void QmlGraphicsViewSection::setProperty(const QString &property)
{
    if (property != m_property) {
        m_property = property;
        emit changed();
    }
}

void QmlGraphicsViewSection::setCriteria(QmlGraphicsViewSection::SectionCriteria criteria)
{
    if (criteria != m_criteria) {
        m_criteria = criteria;
        emit changed();
    }
}

void QmlGraphicsViewSection::setDelegate(QmlComponent *delegate)
{
    if (delegate != m_delegate) {
        m_delegate = delegate;
        emit delegateChanged();
    }
}

QString QmlGraphicsViewSection::sectionString(const QString &value)
{
    if (m_criteria == FirstCharacter)
        return value.isEmpty() ? QString() : value.at(0);
    else
        return value;
}

class QmlGraphicsListViewAttached : public QObject
{
    Q_OBJECT
public:
    QmlGraphicsListViewAttached(QObject *parent)
        : QObject(parent), m_view(0), m_isCurrent(false), m_delayRemove(false) {}
    ~QmlGraphicsListViewAttached() {}

    Q_PROPERTY(QmlGraphicsListView *view READ view CONSTANT)
    QmlGraphicsListView *view() { return m_view; }

    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
        }
    }

    Q_PROPERTY(QString prevSection READ prevSection NOTIFY prevSectionChanged)
    QString prevSection() const { return m_prevSection; }
    void setPrevSection(const QString &sect) {
        if (m_prevSection != sect) {
            m_prevSection = sect;
            emit prevSectionChanged();
        }
    }

    Q_PROPERTY(QString section READ section NOTIFY sectionChanged)
    QString section() const { return m_section; }
    void setSection(const QString &sect) {
        if (m_section != sect) {
            m_section = sect;
            emit sectionChanged();
        }
    }

    Q_PROPERTY(bool delayRemove READ delayRemove WRITE setDelayRemove NOTIFY delayRemoveChanged)
    bool delayRemove() const { return m_delayRemove; }
    void setDelayRemove(bool delay) {
        if (m_delayRemove != delay) {
            m_delayRemove = delay;
            emit delayRemoveChanged();
        }
    }

    void emitAdd() { emit add(); }
    void emitRemove() { emit remove(); }

Q_SIGNALS:
    void currentItemChanged();
    void sectionChanged();
    void prevSectionChanged();
    void delayRemoveChanged();
    void add();
    void remove();

public:
    QmlGraphicsListView *m_view;
    bool m_isCurrent;
    mutable QString m_section;
    QString m_prevSection;
    bool m_delayRemove;
};

//----------------------------------------------------------------------------

class FxListItem
{
public:
    FxListItem(QmlGraphicsItem *i, QmlGraphicsListView *v) : item(i), section(0), view(v) {
        attached = static_cast<QmlGraphicsListViewAttached*>(qmlAttachedPropertiesObject<QmlGraphicsListView>(item));
        if (attached)
            attached->m_view = view;
    }
    ~FxListItem() {}
    qreal position() const {
        if (section)
            return (view->orientation() == QmlGraphicsListView::Vertical ? section->y() : section->x());
        else
            return (view->orientation() == QmlGraphicsListView::Vertical ? item->y() : item->x());
    }
    int size() const {
        if (section)
            return (view->orientation() == QmlGraphicsListView::Vertical ? item->height()+section->height() : item->width()+section->height());
        else
            return (view->orientation() == QmlGraphicsListView::Vertical ? item->height() : item->width());
    }
    qreal endPosition() const {
        return (view->orientation() == QmlGraphicsListView::Vertical
                                        ? item->y() + (item->height() > 0 ? item->height() : 1)
                                        : item->x() + (item->width() > 0 ? item->width() : 1)) - 1;
    }
    void setPosition(qreal pos) {
        if (view->orientation() == QmlGraphicsListView::Vertical) {
            if (section) {
                section->setY(pos);
                pos += section->height();
            }
            item->setY(pos);
        } else {
            if (section) {
                section->setX(pos);
                pos += section->width();
            }
            item->setX(pos);
        }
    }

    QmlGraphicsItem *item;
    QmlGraphicsItem *section;
    QmlGraphicsListView *view;
    QmlGraphicsListViewAttached *attached;
    int index;
};

//----------------------------------------------------------------------------

class QmlGraphicsListViewPrivate : public QmlGraphicsFlickablePrivate, private QmlGraphicsItemChangeListener
{
    Q_DECLARE_PUBLIC(QmlGraphicsListView)

public:
    QmlGraphicsListViewPrivate()
        : currentItem(0), orient(QmlGraphicsListView::Vertical)
        , visiblePos(0), visibleIndex(0)
        , averageSize(100.0), currentIndex(-1), requestedIndex(-1)
        , highlightRangeStart(0), highlightRangeEnd(0)
        , highlightComponent(0), highlight(0), trackedItem(0)
        , moveReason(Other), buffer(0), highlightPosAnimator(0), highlightSizeAnimator(0)
        , sectionCriteria(0), spacing(0.0)
        , highlightMoveSpeed(400), highlightResizeSpeed(400), highlightRange(QmlGraphicsListView::NoHighlightRange)
        , snapMode(QmlGraphicsListView::NoSnap), overshootDist(0.0)
        , footerComponent(0), footer(0), headerComponent(0), header(0)
        , bufferMode(NoBuffer)
        , ownModel(false), wrap(false), autoHighlight(true), haveHighlightRange(false)
        , correctFlick(true), inFlickCorrection(false), lazyRelease(false)
        , minExtentDirty(true), maxExtentDirty(true)
    {}

    void init();
    void clear();
    FxListItem *createItem(int modelIndex);
    void releaseItem(FxListItem *item);

    FxListItem *visibleItem(int modelIndex) const {
        if (modelIndex >= visibleIndex && modelIndex < visibleIndex + visibleItems.count()) {
            for (int i = modelIndex - visibleIndex; i < visibleItems.count(); ++i) {
                FxListItem *item = visibleItems.at(i);
                if (item->index == modelIndex)
                    return item;
            }
        }
        return 0;
    }

    FxListItem *firstVisibleItem() const {
        const qreal pos = position();
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems.at(i);
            if (item->index != -1 && item->endPosition() > pos)
                return item;
        }
        return visibleItems.count() ? visibleItems.first() : 0;
    }

    FxListItem *nextVisibleItem() const {
        const qreal pos = position();
        bool foundFirst = false;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems.at(i);
            if (item->index != -1) {
                if (foundFirst)
                    return item;
                else if (item->position() < pos && item->endPosition() > pos)
                    foundFirst = true;
            }
        }
        return 0;
    }

    qreal position() const {
        Q_Q(const QmlGraphicsListView);
        return orient == QmlGraphicsListView::Vertical ? q->viewportY() : q->viewportX();
    }
    void setPosition(qreal pos) {
        Q_Q(QmlGraphicsListView);
        if (orient == QmlGraphicsListView::Vertical)
            q->setViewportY(pos);
        else
            q->setViewportX(pos);
    }
    qreal size() const {
        Q_Q(const QmlGraphicsListView);
        return orient == QmlGraphicsListView::Vertical ? q->height() : q->width();
    }

    qreal startPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty()) {
            pos = (*visibleItems.constBegin())->position();
            if (visibleIndex > 0)
                pos -= visibleIndex * (averageSize + spacing) - spacing;
        }
        return pos;
    }

    qreal endPosition() const {
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
        }
        return pos;
    }

    qreal positionAt(int modelIndex) const {
        if (FxListItem *item = visibleItem(modelIndex))
            return item->position();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int count = visibleIndex - modelIndex;
                return (*visibleItems.constBegin())->position() - count * (averageSize + spacing);
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

    QString sectionAt(int modelIndex) {
        if (FxListItem *item = visibleItem(modelIndex))
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

    int snapIndex() {
        int index = currentIndex;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->position();
            if (itemTop >= highlight->position()-item->size()/2 && itemTop < highlight->position()+item->size()/2)
                return item->index;
        }
        return index;
    }

    qreal snapPosAt(qreal pos) {
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->position();
            if (item->index == model->count()-1 || (itemTop+item->size()/2 >= pos))
                return item->position();
        }
        if (visibleItems.count()) {
            qreal firstPos = visibleItems.first()->position();
            qreal endPos = visibleItems.last()->position();
            if (pos < firstPos) {
                return firstPos - qRound((firstPos - pos) / averageSize) * averageSize;
            } else if (pos > endPos)
                return endPos + qRound((pos - endPos) / averageSize) * averageSize;
        }
        return qRound((pos - startPosition()) / averageSize) * averageSize + startPosition();
    }

    FxListItem *snapItemAt(qreal pos) {
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems[i];
            if (item->index == -1)
                continue;
            qreal itemTop = item->position();
            if (item->index == model->count()-1 || (itemTop+item->size()/2 >= pos))
                return item;
        }
        if (visibleItems.count() && visibleItems.first()->position() <= pos)
            return visibleItems.first();
        return 0;
    }

    int lastVisibleIndex() const {
        int lastIndex = -1;
        for (int i = visibleItems.count()-1; i >= 0; --i) {
            FxListItem *listItem = visibleItems.at(i);
            if (listItem->index != -1) {
                lastIndex = listItem->index;
                break;
            }
        }
        return lastIndex;
    }

    // map a model index to visibleItems index.
    // These may differ if removed items are still present in the visible list,
    // e.g. doing a removal animation
    int mapFromModel(int modelIndex) const {
        if (modelIndex < visibleIndex || modelIndex >= visibleIndex + visibleItems.count())
            return -1;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *listItem = visibleItems.at(i);
            if (listItem->index == modelIndex)
                return i + visibleIndex;
            if (listItem->index > modelIndex)
                return -1;
        }
        return -1; // Not in visibleList
    }

    bool mapRangeFromModel(int &index, int &count) const {
        if (index + count < visibleIndex)
            return false;

        int lastIndex = -1;
        for (int i = visibleItems.count()-1; i >= 0; --i) {
            FxListItem *listItem = visibleItems.at(i);
            if (listItem->index != -1) {
                lastIndex = listItem->index;
                break;
            }
        }

        if (index > lastIndex)
            return false;

        int last = qMin(index + count - 1, lastIndex);
        index = qMax(index, visibleIndex);
        count = last - index + 1;

        return true;
    }

    void updateViewport() {
        Q_Q(QmlGraphicsListView);
        if (orient == QmlGraphicsListView::Vertical) {
            q->setViewportHeight(q->minYExtent() - q->maxYExtent());
        } else {
            q->setViewportWidth(q->minXExtent() - q->maxXExtent());
        }
    }

    void itemGeometryChanged(QmlGraphicsItem *, const QRectF &newGeometry, const QRectF &oldGeometry) {
        if ((orient == QmlGraphicsListView::Vertical && newGeometry.height() != oldGeometry.height())
            || newGeometry.width() != oldGeometry.width()) {
            layout();
            fixupPosition();
        }
    }

    // for debugging only
    void checkVisible() const {
        int skip = 0;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxListItem *listItem = visibleItems.at(i);
            if (listItem->index == -1) {
                ++skip;
            } else if (listItem->index != visibleIndex + i - skip) {
                qFatal("index %d %d %d", visibleIndex, i, listItem->index);
            }
        }
    }

    void refill(qreal from, qreal to, bool doBuffer = false);
    void layout();
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateTrackedItem();
    void createHighlight();
    void updateHighlight();
    void createSection(FxListItem *);
    void updateSections();
    void updateCurrentSection();
    void updateCurrent(int);
    void updateAverage();
    void updateHeader();
    void updateFooter();
    void fixupPosition();
    virtual void fixupY();
    virtual void fixupX();
    virtual void flickX(qreal velocity);
    virtual void flickY(qreal velocity);

    QGuard<QmlGraphicsVisualModel> model;
    QVariant modelVariant;
    QList<FxListItem*> visibleItems;
    QHash<QmlGraphicsItem*,int> unrequestedItems;
    FxListItem *currentItem;
    QmlGraphicsListView::Orientation orient;
    int visiblePos;
    int visibleIndex;
    qreal averageSize;
    int currentIndex;
    int requestedIndex;
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    QmlComponent *highlightComponent;
    FxListItem *highlight;
    FxListItem *trackedItem;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    int buffer;
    QmlEaseFollow *highlightPosAnimator;
    QmlEaseFollow *highlightSizeAnimator;
    QmlGraphicsViewSection *sectionCriteria;
    QString currentSection;
    static const int sectionCacheSize = 3;
    QmlGraphicsItem *sectionCache[sectionCacheSize];
    qreal spacing;
    qreal highlightMoveSpeed;
    qreal highlightResizeSpeed;
    QmlGraphicsListView::HighlightRangeMode highlightRange;
    QmlGraphicsListView::SnapMode snapMode;
    qreal overshootDist;
    QmlComponent *footerComponent;
    FxListItem *footer;
    QmlComponent *headerComponent;
    FxListItem *header;
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
    mutable bool minExtentDirty : 1;
    mutable bool maxExtentDirty : 1;
};

void QmlGraphicsListViewPrivate::init()
{
    Q_Q(QmlGraphicsListView);
    q->setFlag(QGraphicsItem::ItemIsFocusScope);
    addItemChangeListener(this, Geometry);
    QObject::connect(q, SIGNAL(movementEnded()), q, SLOT(animStopped()));
    q->setFlickDirection(QmlGraphicsFlickable::VerticalFlick);
    ::memset(sectionCache, 0, sizeof(QmlGraphicsItem*) * sectionCacheSize);
}

void QmlGraphicsListViewPrivate::clear()
{
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
}

FxListItem *QmlGraphicsListViewPrivate::createItem(int modelIndex)
{
    Q_Q(QmlGraphicsListView);
    // create object
    requestedIndex = modelIndex;
    FxListItem *listItem = 0;
    if (QmlGraphicsItem *item = model->item(modelIndex, false)) {
        listItem = new FxListItem(item, q);
        listItem->index = modelIndex;
        // initialise attached properties
        if (sectionCriteria) {
            QString propValue = model->stringValue(modelIndex, sectionCriteria->property());
            listItem->attached->m_section = sectionCriteria->sectionString(propValue);
            if (modelIndex > 0) {
                if (FxListItem *item = visibleItem(modelIndex-1))
                    listItem->attached->m_prevSection = item->attached->section();
                else
                    listItem->attached->m_prevSection = sectionAt(modelIndex-1);
            }
        }
        // complete
        model->completeItem();
        listItem->item->setZValue(1);
        listItem->item->setParent(q->viewport());
        QmlGraphicsItemPrivate *itemPrivate = static_cast<QmlGraphicsItemPrivate*>(QGraphicsItemPrivate::get(item));
        itemPrivate->addItemChangeListener(this, QmlGraphicsItemPrivate::Geometry);
        if (sectionCriteria && sectionCriteria->delegate()) {
            if (listItem->attached->m_prevSection != listItem->attached->m_section)
                createSection(listItem);
        }
        unrequestedItems.remove(listItem->item);
    }
    requestedIndex = -1;

    return listItem;
}

void QmlGraphicsListViewPrivate::releaseItem(FxListItem *item)
{
    Q_Q(QmlGraphicsListView);
    if (!item || !model)
        return;
    if (trackedItem == item) {
        const char *notifier1 = orient == QmlGraphicsListView::Vertical ? SIGNAL(yChanged()) : SIGNAL(xChanged());
        const char *notifier2 = orient == QmlGraphicsListView::Vertical ? SIGNAL(heightChanged()) : SIGNAL(widthChanged());
        QObject::disconnect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }
    QmlGraphicsItemPrivate *itemPrivate = static_cast<QmlGraphicsItemPrivate*>(QGraphicsItemPrivate::get(item->item));
    itemPrivate->removeItemChangeListener(this, QmlGraphicsItemPrivate::Geometry);
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

void QmlGraphicsListViewPrivate::refill(qreal from, qreal to, bool doBuffer)
{
    Q_Q(QmlGraphicsListView);
    if (!isValid() || !q->isComponentComplete())
        return;
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
        modelIndex = visibleItems.at(i)->index + 1;
    }

    bool changed = false;
    FxListItem *item = 0;
    int pos = itemEnd + 1;
    while (modelIndex < model->count() && pos <= fillTo) {
        //qDebug() << "refill: append item" << modelIndex << "pos" << pos;
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
    while (visibleIndex > 0 && visibleIndex <= model->count() && visiblePos > fillFrom) {
        //qDebug() << "refill: prepend item" << visibleIndex-1 << "current top pos" << visiblePos;
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

    if (!lazyRelease || !changed) { // avoid destroying items in the same frame that we create
        while (visibleItems.count() > 1 && (item = visibleItems.first()) && item->endPosition() < bufferFrom) {
            if (item->attached->delayRemove())
                break;
            //qDebug() << "refill: remove first" << visibleIndex << "top end pos" << item->endPosition();
            if (item->index != -1)
                visibleIndex++;
            visibleItems.removeFirst();
            releaseItem(item);
            changed = true;
        }
        while (visibleItems.count() > 1 && (item = visibleItems.last()) && item->position() > bufferTo) {
            if (item->attached->delayRemove())
                break;
            //qDebug() << "refill: remove last" << visibleIndex+visibleItems.count()-1;
            visibleItems.removeLast();
            releaseItem(item);
            changed = true;
        }
    }
    if (changed) {
        minExtentDirty = true;
        maxExtentDirty = true;
        if (visibleItems.count())
            visiblePos = (*visibleItems.constBegin())->position();
        updateAverage();
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

void QmlGraphicsListViewPrivate::layout()
{
    Q_Q(QmlGraphicsListView);
    updateSections();
    if (!visibleItems.isEmpty()) {
        int oldEnd = visibleItems.last()->endPosition();
        int pos = visibleItems.first()->endPosition() + spacing + 1;
        for (int i=1; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems.at(i);
            item->setPosition(pos);
            pos += item->size() + spacing;
        }
        // move current item if it is after the visible items.
        if (currentItem && currentIndex > lastVisibleIndex())
            currentItem->setPosition(currentItem->position() + (visibleItems.last()->endPosition() - oldEnd));
    }
    if (!isValid())
        return;
    q->refill();
    minExtentDirty = true;
    maxExtentDirty = true;
    updateHighlight();
    fixupPosition();
    q->refill();
    if (header)
        updateHeader();
    if (footer)
        updateFooter();
    updateViewport();
}

void QmlGraphicsListViewPrivate::updateUnrequestedIndexes()
{
    Q_Q(QmlGraphicsListView);
    QHash<QmlGraphicsItem*,int>::iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it)
        *it = model->indexOf(it.key(), q);
}

void QmlGraphicsListViewPrivate::updateUnrequestedPositions()
{
    Q_Q(QmlGraphicsListView);
    if (unrequestedItems.count()) {
        qreal pos = position();
        QHash<QmlGraphicsItem*,int>::const_iterator it;
        for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it) {
            QmlGraphicsItem *item = it.key();
            if (orient == QmlGraphicsListView::Vertical) {
                if (item->y() + item->height() > pos && item->y() < pos + q->height())
                    item->setY(positionAt(*it));
            } else {
                if (item->x() + item->width() > pos && item->x() < pos + q->width())
                    item->setX(positionAt(*it));
            }
        }
    }
}

void QmlGraphicsListViewPrivate::updateTrackedItem()
{
    Q_Q(QmlGraphicsListView);
    FxListItem *item = currentItem;
    if (highlight)
        item = highlight;

    FxListItem *oldTracked = trackedItem;

    const char *notifier1 = orient == QmlGraphicsListView::Vertical ? SIGNAL(yChanged()) : SIGNAL(xChanged());
    const char *notifier2 = orient == QmlGraphicsListView::Vertical ? SIGNAL(heightChanged()) : SIGNAL(widthChanged());

    if (trackedItem && item != trackedItem) {
        QObject::disconnect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }

    if (!trackedItem && item) {
        trackedItem = item;
        QObject::connect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::connect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
    }
    if (trackedItem && trackedItem != oldTracked)
        q->trackedPositionChanged();
}

void QmlGraphicsListViewPrivate::createHighlight()
{
    Q_Q(QmlGraphicsListView);
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
        QmlGraphicsItem *item = 0;
        if (highlightComponent) {
            QmlContext *highlightContext = new QmlContext(qmlContext(q));
            QObject *nobj = highlightComponent->create(highlightContext);
            if (nobj) {
                highlightContext->setParent(nobj);
                item = qobject_cast<QmlGraphicsItem *>(nobj);
                if (!item)
                    delete nobj;
            } else {
                delete highlightContext;
            }
        } else {
            item = new QmlGraphicsItem;
        }
        if (item) {
            item->setParent(q->viewport());
            highlight = new FxListItem(item, q);
            if (orient == QmlGraphicsListView::Vertical)
                highlight->item->setHeight(currentItem->item->height());
            else
                highlight->item->setWidth(currentItem->item->width());
            const QLatin1String posProp(orient == QmlGraphicsListView::Vertical ? "y" : "x");
            highlightPosAnimator = new QmlEaseFollow(q);
            highlightPosAnimator->setTarget(QmlMetaProperty(highlight->item, posProp));
            highlightPosAnimator->setVelocity(highlightMoveSpeed);
            highlightPosAnimator->setEnabled(autoHighlight);
            const QLatin1String sizeProp(orient == QmlGraphicsListView::Vertical ? "height" : "width");
            highlightSizeAnimator = new QmlEaseFollow(q);
            highlightSizeAnimator->setVelocity(highlightResizeSpeed);
            highlightSizeAnimator->setTarget(QmlMetaProperty(highlight->item, sizeProp));
            highlightSizeAnimator->setEnabled(autoHighlight);
            changed = true;
        }
    }
    if (changed)
        emit q->highlightChanged();
}

void QmlGraphicsListViewPrivate::updateHighlight()
{
    if ((!currentItem && highlight) || (currentItem && !highlight))
        createHighlight();
    if (currentItem && autoHighlight && highlight && !moving) {
        // auto-update highlight
        highlightPosAnimator->setSourceValue(currentItem->position());
        highlightSizeAnimator->setSourceValue(currentItem->size());
        if (orient == QmlGraphicsListView::Vertical) {
            if (highlight->item->width() == 0)
                highlight->item->setWidth(currentItem->item->width());
        } else {
            if (highlight->item->height() == 0)
                highlight->item->setHeight(currentItem->item->height());
        }
    }
    updateTrackedItem();
}

void QmlGraphicsListViewPrivate::createSection(FxListItem *listItem)
{
    Q_Q(QmlGraphicsListView);
    if (!sectionCriteria || !sectionCriteria->delegate())
        return;
    if (listItem->attached->m_prevSection != listItem->attached->m_section) {
        if (!listItem->section) {
            int i = sectionCacheSize-1;
            while (i >= 0 && !sectionCache[i])
                --i;
            if (i >= 0) {
                listItem->section = sectionCache[i];
                sectionCache[i] = 0;
                listItem->section->setVisible(true);
                QmlContext *context = QmlEngine::contextForObject(listItem->section)->parentContext();
                context->setContextProperty(QLatin1String("section"), listItem->attached->m_section);
            } else {
                QmlContext *context = new QmlContext(qmlContext(q));
                context->setContextProperty(QLatin1String("section"), listItem->attached->m_section);
                QObject *nobj = sectionCriteria->delegate()->create(context);
                if (nobj) {
                    context->setParent(nobj);
                    listItem->section = qobject_cast<QmlGraphicsItem *>(nobj);
                    if (!listItem->section) {
                        delete nobj;
                    } else {
                        listItem->section->setZValue(1);
                        listItem->section->setParent(q->viewport());
                    }
                } else {
                    delete context;
                }
            }
        }
    } else if (listItem->section) {
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
    }
}

void QmlGraphicsListViewPrivate::updateSections()
{
    if (sectionCriteria) {
        QString prevSection;
        if (visibleIndex > 0)
            prevSection = sectionAt(visibleIndex-1);
        for (int i = 0; i < visibleItems.count(); ++i) {
            if (visibleItems.at(i)->index != -1) {
                QmlGraphicsListViewAttached *attached = visibleItems.at(i)->attached;
                attached->setPrevSection(prevSection);
                createSection(visibleItems.at(i));
                prevSection = attached->section();
            }
        }
    }
}

void QmlGraphicsListViewPrivate::updateCurrentSection()
{
    if (!sectionCriteria || visibleItems.isEmpty()) {
        currentSection = QString();
        return;
    }
    int index = 0;
    while (visibleItems.at(index)->endPosition() < position() && index < visibleItems.count())
        ++index;

    if (index < visibleItems.count())
        currentSection = visibleItems.at(index)->attached->section();
    else
        currentSection = visibleItems.first()->attached->section();
}

void QmlGraphicsListViewPrivate::updateCurrent(int modelIndex)
{
    Q_Q(QmlGraphicsListView);
    if (!q->isComponentComplete() || !isValid() || modelIndex < 0 || modelIndex >= model->count()) {
        if (currentItem) {
            currentItem->attached->setIsCurrentItem(false);
            releaseItem(currentItem);
            currentItem = 0;
            currentIndex = -1;
            updateHighlight();
            emit q->currentIndexChanged();
        }
        return;
    }

    if (currentItem && currentIndex == modelIndex) {
        updateHighlight();
        return;
    }
    FxListItem *oldCurrentItem = currentItem;
    currentIndex = modelIndex;
    currentItem = createItem(modelIndex);
    if (oldCurrentItem && (!currentItem || oldCurrentItem->item != currentItem->item))
        oldCurrentItem->attached->setIsCurrentItem(false);
    if (currentItem) {
        if (modelIndex == visibleIndex - 1) {
            // We can calculate exact postion in this case
            currentItem->setPosition(visibleItems.first()->position() - currentItem->size() - spacing);
        } else {
            // Create current item now and position as best we can.
            // Its position will be corrected when it becomes visible.
            currentItem->setPosition(positionAt(modelIndex));
        }
        currentItem->item->setFocus(true);
        currentItem->attached->setIsCurrentItem(true);
    }
    updateHighlight();
    emit q->currentIndexChanged();
    // Release the old current item
    releaseItem(oldCurrentItem);
}

void QmlGraphicsListViewPrivate::updateAverage()
{
    if (!visibleItems.count())
        return;
    qreal sum = 0.0;
    for (int i = 0; i < visibleItems.count(); ++i)
        sum += visibleItems.at(i)->size();
    averageSize = sum / visibleItems.count();
}

void QmlGraphicsListViewPrivate::updateFooter()
{
    Q_Q(QmlGraphicsListView);
    if (!footer && footerComponent) {
        QmlGraphicsItem *item = 0;
        QmlContext *context = new QmlContext(qmlContext(q));
        QObject *nobj = footerComponent->create(context);
        if (nobj) {
            context->setParent(nobj);
            item = qobject_cast<QmlGraphicsItem *>(nobj);
            if (!item)
                delete nobj;
        } else {
            delete context;
        }
        if (item) {
            item->setParent(q->viewport());
            item->setZValue(1);
            footer = new FxListItem(item, q);
        }
    }
    if (footer) {
        if (visibleItems.count()) {
            qreal endPos = endPosition();
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

void QmlGraphicsListViewPrivate::updateHeader()
{
    Q_Q(QmlGraphicsListView);
    if (!header && headerComponent) {
        QmlGraphicsItem *item = 0;
        QmlContext *context = new QmlContext(qmlContext(q));
        QObject *nobj = headerComponent->create(context);
        if (nobj) {
            context->setParent(nobj);
            item = qobject_cast<QmlGraphicsItem *>(nobj);
            if (!item)
                delete nobj;
        } else {
            delete context;
        }
        if (item) {
            item->setParent(q->viewport());
            item->setZValue(1);
            header = new FxListItem(item, q);
            if (visibleItems.isEmpty())
                visiblePos = header->size();
        }
    }
    if (header) {
        if (visibleItems.count()) {
            qreal startPos = startPosition();
            if (visibleIndex == 0) {
                header->setPosition(startPos - header->size());
            } else {
                if (position() <= startPos || header->position() > startPos - header->size())
                    header->setPosition(startPos - header->size());
            }
        } else {
            header->setPosition(0);
        }
    }
}

void QmlGraphicsListViewPrivate::fixupPosition()
{
    moveReason = Other;
    if (orient == QmlGraphicsListView::Vertical)
        fixupY();
    else
        fixupX();
}

void QmlGraphicsListViewPrivate::fixupY()
{
    Q_Q(QmlGraphicsListView);
    if (orient == QmlGraphicsListView::Horizontal)
        return;
    if (!q->yflick() || _moveY.timeLine())
        return;

    int oldDuration = fixupDuration;
    fixupDuration = moveReason == Mouse ? fixupDuration : 0;

    if (haveHighlightRange && highlightRange == QmlGraphicsListView::StrictlyEnforceRange) {
        if (currentItem && highlight && currentItem->position() != highlight->position()) {
            timeline.reset(_moveY);
            timeline.move(_moveY, -(currentItem->position() - highlightRangeStart), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
            vTime = timeline.time();
        }
    } else if (snapMode != QmlGraphicsListView::NoSnap) {
        if (FxListItem *item = snapItemAt(position())) {
            qreal pos = qMin(item->position() - highlightRangeStart, -q->maxYExtent());
            qreal dist = qAbs(_moveY + pos);
            if (dist > 0) {
                timeline.reset(_moveY);
                timeline.move(_moveY, -pos, QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
                vTime = timeline.time();
            }
        }
    } else {
        QmlGraphicsFlickablePrivate::fixupY();
    }
    fixupDuration = oldDuration;
}

void QmlGraphicsListViewPrivate::fixupX()
{
    Q_Q(QmlGraphicsListView);
    if (orient == QmlGraphicsListView::Vertical)
        return;
    if (!q->xflick() || _moveX.timeLine())
        return;

    int oldDuration = fixupDuration;
    fixupDuration = moveReason == Mouse ? fixupDuration : 0;

    if (haveHighlightRange && highlightRange == QmlGraphicsListView::StrictlyEnforceRange) {
        if (currentItem && highlight && currentItem->position() != highlight->position()) {
            timeline.reset(_moveX);
            timeline.move(_moveX, -(currentItem->position() - highlightRangeStart), QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
            vTime = timeline.time();
        }
    } else if (snapMode != QmlGraphicsListView::NoSnap) {
        if (FxListItem *item = snapItemAt(position())) {
            qreal pos = qMin(item->position() - highlightRangeStart, -q->maxXExtent());
            qreal dist = qAbs(_moveX + pos);
            if (dist > 0) {
                timeline.reset(_moveX);
                timeline.move(_moveX, -pos, QEasingCurve(QEasingCurve::InOutQuad), fixupDuration);
                vTime = timeline.time();
            }
        }
    } else {
        QmlGraphicsFlickablePrivate::fixupX();
    }
    fixupDuration = oldDuration;
}

void QmlGraphicsListViewPrivate::flickX(qreal velocity)
{
    Q_Q(QmlGraphicsListView);

    moveReason = Mouse;
    if ((!haveHighlightRange || highlightRange != QmlGraphicsListView::StrictlyEnforceRange) && snapMode == QmlGraphicsListView::NoSnap) {
        QmlGraphicsFlickablePrivate::flickX(velocity);
        return;
    }
    qreal maxDistance = -1;
    const qreal maxX = q->maxXExtent();
    const qreal minX = q->minXExtent();
    // -ve velocity means list is moving up
    if (velocity > 0) {
        if (snapMode == QmlGraphicsListView::SnapOneItem) {
            if (FxListItem *item = firstVisibleItem())
                maxDistance = qAbs(item->position() + _moveX.value());
        } else if (_moveX.value() < minX) {
            maxDistance = qAbs(minX -_moveX.value() + (overShoot?30:0));
        }
        if (snapMode != QmlGraphicsListView::SnapToItem && highlightRange != QmlGraphicsListView::StrictlyEnforceRange)
            flickTargetX = minX;
    } else {
        if (snapMode == QmlGraphicsListView::SnapOneItem) {
            if (FxListItem *item = nextVisibleItem())
                maxDistance = qAbs(item->position() + _moveX.value());
        } else if (_moveX.value() > maxX) {
            maxDistance = qAbs(maxX - _moveX.value()) + (overShoot?30:0);
        }
        if (snapMode != QmlGraphicsListView::SnapToItem && highlightRange != QmlGraphicsListView::StrictlyEnforceRange)
            flickTargetX = maxX;
    }
    if (maxDistance > 0 && (snapMode != QmlGraphicsListView::NoSnap || highlightRange == QmlGraphicsListView::StrictlyEnforceRange)) {
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
        if (!flicked) {
            // the initial flick - estimate boundary
            qreal accel = deceleration;
            qreal v2 = v * v;
            qreal maxAccel = v2 / (2.0f * maxDistance);
            if (maxAccel < accel) {
                qreal dist = v2 / (accel * 2.0);
                if (v > 0)
                    dist = -dist;
                flickTargetX = -snapPosAt(-(_moveX.value() - highlightRangeStart) + dist) + highlightRangeStart;
                dist = -flickTargetX + _moveX.value();
                accel = v2 / (2.0f * qAbs(dist));
                overshootDist = 0.0;
            } else {
                flickTargetX = velocity > 0 ? minX : maxX;
                overshootDist = overShoot ? 30 : 0;
            }
            timeline.reset(_moveX);
            timeline.accel(_moveX, v, accel, maxDistance + overshootDist);
            timeline.execute(fixupXEvent);
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
            correctFlick = true;
        } else {
            // reevaluate the target boundary.
            qreal newtarget = flickTargetX;
            if (snapMode != QmlGraphicsListView::NoSnap || highlightRange == QmlGraphicsListView::StrictlyEnforceRange)
                newtarget = -snapPosAt(-(flickTargetX - highlightRangeStart)) + highlightRangeStart;
            if (velocity < 0 && newtarget < maxX)
                newtarget = maxX;
            else if (velocity > 0 && newtarget > minX)
                newtarget = minX;
            if (newtarget == flickTargetX) // boundary unchanged - nothing to do
                return;
            flickTargetX = newtarget;
            qreal dist = -newtarget + _moveX.value();
            if ((v < 0 && dist < 0) || (v > 0 && dist > 0)) {
                correctFlick = false;
                timeline.reset(_moveX);
                fixupX();
                return;
            }
            timeline.reset(_moveX);
            timeline.accelDistance(_moveX, v, -dist + (v < 0 ? -overshootDist : overshootDist));
            timeline.execute(fixupXEvent);
        }
    } else {
        correctFlick = false;
        timeline.reset(_moveX);
        fixupX();
    }
}

void QmlGraphicsListViewPrivate::flickY(qreal velocity)
{
    Q_Q(QmlGraphicsListView);

    moveReason = Mouse;
    if ((!haveHighlightRange || highlightRange != QmlGraphicsListView::StrictlyEnforceRange) && snapMode == QmlGraphicsListView::NoSnap) {
        QmlGraphicsFlickablePrivate::flickY(velocity);
        return;
    }
    qreal maxDistance = -1;
    const qreal maxY = q->maxYExtent();
    const qreal minY = q->minYExtent();
    // -ve velocity means list is moving up
    if (velocity > 0) {
        if (snapMode == QmlGraphicsListView::SnapOneItem) {
            if (FxListItem *item = firstVisibleItem())
                maxDistance = qAbs(item->position() + _moveY.value());
        } else if (_moveY.value() < minY) {
            maxDistance = qAbs(minY -_moveY.value() + (overShoot?30:0));
        }
        if (snapMode != QmlGraphicsListView::SnapToItem && highlightRange != QmlGraphicsListView::StrictlyEnforceRange)
            flickTargetY = minY;
    } else {
        if (snapMode == QmlGraphicsListView::SnapOneItem) {
            if (FxListItem *item = nextVisibleItem())
                maxDistance = qAbs(item->position() + _moveY.value());
        } else if (_moveY.value() > maxY) {
            maxDistance = qAbs(maxY - _moveY.value()) + (overShoot?30:0);
        }
        if (snapMode != QmlGraphicsListView::SnapToItem && highlightRange != QmlGraphicsListView::StrictlyEnforceRange)
            flickTargetY = maxY;
    }
    if (maxDistance > 0 && (snapMode != QmlGraphicsListView::NoSnap || highlightRange == QmlGraphicsListView::StrictlyEnforceRange)) {
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
        if (!flicked) {
            // the initial flick - estimate boundary
            qreal accel = deceleration;
            qreal v2 = v * v;
            qreal maxAccel = v2 / (2.0f * maxDistance);
            if (maxAccel < accel) {
                qreal dist = v2 / (accel * 2.0);
                if (v > 0)
                    dist = -dist;
                flickTargetY = -snapPosAt(-(_moveY.value() - highlightRangeStart) + dist) + highlightRangeStart;
                dist = -flickTargetY + _moveY.value();
                accel = v2 / (2.0f * qAbs(dist));
                overshootDist = 0.0;
            } else {
                flickTargetY = velocity > 0 ? minY : maxY;
                overshootDist = overShoot ? 30 : 0;
            }
            timeline.reset(_moveY);
            timeline.accel(_moveY, v, accel, maxDistance + overshootDist);
            timeline.execute(fixupYEvent);
            flicked = true;
            emit q->flickingChanged();
            emit q->flickStarted();
            correctFlick = true;
        } else {
            // reevaluate the target boundary.
            qreal newtarget = flickTargetY;
            if (snapMode != QmlGraphicsListView::NoSnap || highlightRange == QmlGraphicsListView::StrictlyEnforceRange)
                newtarget = -snapPosAt(-(flickTargetY - highlightRangeStart)) + highlightRangeStart;
            if (velocity < 0 && newtarget < maxY)
                newtarget = maxY;
            else if (velocity > 0 && newtarget > minY)
                newtarget = minY;
            if (newtarget == flickTargetY) // boundary unchanged - nothing to do
                return;
            flickTargetY = newtarget;
            qreal dist = -newtarget + _moveY.value();
            if ((v < 0 && dist < 0) || (v > 0 && dist > 0)) {
                correctFlick = false;
                timeline.reset(_moveY);
                fixupY();
                return;
            }
            timeline.reset(_moveY);
            timeline.accelDistance(_moveY, v, -dist + (v < 0 ? -overshootDist : overshootDist));
            timeline.execute(fixupYEvent);
        }
    } else {
        correctFlick = false;
        timeline.reset(_moveY);
        fixupY();
    }
}

//----------------------------------------------------------------------------

/*!
    \qmlclass ListView QmlGraphicsListView
    \inherits Flickable
    \brief The ListView item provides a list view of items provided by a model.

    The model is typically provided by a QAbstractListModel "C++ model object",
    but can also be created directly in QML. The items are laid out vertically
    or horizontally and may be flicked to scroll.

    The below example creates a very simple vertical list, using a QML model.
    \image trivialListView.png

    The user interface defines a delegate to display an item, a highlight,
    and the ListView which uses the above.

    \snippet doc/src/snippets/declarative/listview/listview.qml 3

    The model is defined as a ListModel using QML:
    \quotefile doc/src/snippets/declarative/listview/dummydata/ContactModel.qml

    In this case ListModel is a handy way for us to test our UI.  In practice
    the model would be implemented in C++, or perhaps via a SQL data source.
*/

QmlGraphicsListView::QmlGraphicsListView(QmlGraphicsItem *parent)
    : QmlGraphicsFlickable(*(new QmlGraphicsListViewPrivate), parent)
{
    Q_D(QmlGraphicsListView);
    d->init();
}

QmlGraphicsListView::~QmlGraphicsListView()
{
    Q_D(QmlGraphicsListView);
    d->clear();
    if (d->ownModel)
        delete d->model;
    delete d->header;
    delete d->footer;
}

/*!
    \qmlattachedproperty bool ListView::isCurrentItem
    This attached property is true if this delegate is the current item; otherwise false.

    It is attached to each instance of the delegate.

    This property may be used to adjust the appearance of the current item, for example:

    \snippet doc/src/snippets/declarative/listview/highlight.qml 0
*/

/*!
    \qmlattachedproperty ListView ListView::view
    This attached property holds the view that manages this delegate instance.

    It is attached to each instance of the delegate.
*/

/*!
    \qmlattachedproperty string ListView::prevSection
    This attached property holds the section of the previous element.

    It is attached to each instance of the delegate.

    The section is evaluated using the \l {ListView::section.property}{section} properties.
*/

/*!
    \qmlattachedproperty string ListView::section
    This attached property holds the section of this element.

    It is attached to each instance of the delegate.

    The section is evaluated using the \l {ListView::section.property}{section} properties.
*/

/*!
    \qmlattachedproperty bool ListView::delayRemove
    This attached property holds whether the delegate may be destroyed.

    It is attached to each instance of the delegate.

    It is sometimes necessary to delay the destruction of an item
    until an animation completes.

    The example below ensures that the animation completes before
    the item is removed from the list.

    \code
    Component {
        id: myDelegate
        Item {
            id: wrapper
            ListView.onRemove: SequentialAnimation {
                PropertyAction { target: wrapper.ListView; property: "delayRemove"; value: true }
                NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing: "easeInOutQuad" }
                PropertyAction { target: wrapper.ListView; property: "delayRemove"; value: false }
            }
        }
    }
    \endcode
*/

/*!
    \qmlattachedsignal ListView::onAdd()
    This attached handler is called immediately after an item is added to the view.
*/

/*!
    \qmlattachedsignal ListView::onRemove()
    This attached handler is called immediately before an item is removed from the view.
*/

/*!
    \qmlproperty model ListView::model
    This property holds the model providing data for the list.

    The model provides a set of data that is used to create the items
    for the view.  For large or dynamic datasets the model is usually
    provided by a C++ model object.  The C++ model object must be a \l
    {QAbstractItemModel} subclass or a simple list.

    Models can also be created directly in QML, using a \l{ListModel},
    \l{XmlListModel} or \l{VisualItemModel}.

    \sa {qmlmodels}{Data Models}
*/
QVariant QmlGraphicsListView::model() const
{
    Q_D(const QmlGraphicsListView);
    return d->modelVariant;
}

void QmlGraphicsListView::setModel(const QVariant &model)
{
    Q_D(QmlGraphicsListView);
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        disconnect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QmlGraphicsItem*)), this, SLOT(destroyingItem(QmlGraphicsItem*)));
    }
    d->clear();
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
        if (isComponentComplete()) {
            refill();
            if (d->currentIndex >= d->model->count() || d->currentIndex < 0) {
                setCurrentIndex(0);
            } else {
                d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
                d->updateCurrent(d->currentIndex);
            }
        }
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(itemsMoved(int,int,int)), this, SLOT(itemsMoved(int,int,int)));
        connect(d->model, SIGNAL(createdItem(int, QmlGraphicsItem*)), this, SLOT(createdItem(int,QmlGraphicsItem*)));
        connect(d->model, SIGNAL(destroyingItem(QmlGraphicsItem*)), this, SLOT(destroyingItem(QmlGraphicsItem*)));
        emit countChanged();
    }
}

/*!
    \qmlproperty component ListView::delegate

    The delegate provides a template defining each item instantiated by the view.
    The index is exposed as an accessible \c index property.  Properties of the
    model are also available depending upon the type of \l {qmlmodels}{Data Model}.

    Note that the ListView will layout the items based on the size of the root item
    in the delegate.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/listview/listview.qml 0
*/
QmlComponent *QmlGraphicsListView::delegate() const
{
    Q_D(const QmlGraphicsListView);
    if (d->model) {
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QmlGraphicsListView::setDelegate(QmlComponent *delegate)
{
    Q_D(QmlGraphicsListView);
    if (delegate == this->delegate())
        return;
    if (!d->ownModel) {
        d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        if (isComponentComplete()) {
            for (int i = 0; i < d->visibleItems.count(); ++i)
                d->releaseItem(d->visibleItems.at(i));
            d->visibleItems.clear();
            refill();
            d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
            d->updateCurrent(d->currentIndex);
        }
    }
}

/*!
    \qmlproperty int ListView::currentIndex
    \qmlproperty Item ListView::currentItem

    \c currentIndex holds the index of the current item.
    \c currentItem is the current item.  Note that the position of the current item
    may only be approximate until it becomes visible in the view.
*/
int QmlGraphicsListView::currentIndex() const
{
    Q_D(const QmlGraphicsListView);
    return d->currentIndex;
}

void QmlGraphicsListView::setCurrentIndex(int index)
{
    Q_D(QmlGraphicsListView);
    if (isComponentComplete() && d->isValid() && index != d->currentIndex && index < d->model->count() && index >= 0) {
        d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
        cancelFlick();
        d->updateCurrent(index);
    } else {
        d->currentIndex = index;
    }
}

QmlGraphicsItem *QmlGraphicsListView::currentItem()
{
    Q_D(QmlGraphicsListView);
    if (!d->currentItem)
        return 0;
    return d->currentItem->item;
}

/*!
  \qmlproperty Item ListView::highlightItem

  \c highlightItem holds the highlight item, which was created
  from the \l highlight component.

  The highlightItem is managed by the view unless
  \l highlightFollowsCurrentItem is set to false.

  \sa highlight, highlightFollowsCurrentItem
*/
QmlGraphicsItem *QmlGraphicsListView::highlightItem()
{
    Q_D(QmlGraphicsListView);
    if (!d->highlight)
        return 0;
    return d->highlight->item;
}

/*!
  \qmlproperty int ListView::count
  This property holds the number of items in the view.
*/
int QmlGraphicsListView::count() const
{
    Q_D(const QmlGraphicsListView);
    if (d->model)
        return d->model->count();
    return 0;
}

/*!
    \qmlproperty component ListView::highlight
    This property holds the component to use as the highlight.

    An instance of the highlight component will be created for each list.
    The geometry of the resultant component instance will be managed by the list
    so as to stay with the current item, unless the highlightFollowsCurrentItem
    property is false.

    The below example demonstrates how to make a simple highlight
    for a vertical list.

    \snippet doc/src/snippets/declarative/listview/listview.qml 1
    \image trivialListView.png

    \sa highlightItem, highlightFollowsCurrentItem
*/
QmlComponent *QmlGraphicsListView::highlight() const
{
    Q_D(const QmlGraphicsListView);
    return d->highlightComponent;
}

void QmlGraphicsListView::setHighlight(QmlComponent *highlight)
{
    Q_D(QmlGraphicsListView);
    if (highlight != d->highlightComponent) {
        d->highlightComponent = highlight;
        d->createHighlight();
        if (d->currentItem)
            d->updateHighlight();
    }
}

/*!
    \qmlproperty bool ListView::highlightFollowsCurrentItem
    This property holds whether the highlight is managed by the view.

    If highlightFollowsCurrentItem is true, the highlight will be moved smoothly
    to follow the current item.  If highlightFollowsCurrentItem is false, the
    highlight will not be moved by the view, and must be implemented
    by the highlight.  The following example creates a highlight with
    its motion defined by the spring \l {SpringFollow}:

    \snippet doc/src/snippets/declarative/listview/highlight.qml 1

    Note that the highlight animation also affects the way that the view
    is scrolled.  This is because the view moves to maintain the
    highlight within the preferred highlight range (or visible viewport).

    \sa highlight, highlightMoveSpeed
*/
bool QmlGraphicsListView::highlightFollowsCurrentItem() const
{
    Q_D(const QmlGraphicsListView);
    return d->autoHighlight;
}

void QmlGraphicsListView::setHighlightFollowsCurrentItem(bool autoHighlight)
{
    Q_D(QmlGraphicsListView);
    if (d->autoHighlight != autoHighlight) {
        d->autoHighlight = autoHighlight;
        if (d->highlightPosAnimator) {
            d->highlightPosAnimator->setEnabled(d->autoHighlight);
            d->highlightSizeAnimator->setEnabled(d->autoHighlight);
        }
        d->updateHighlight();
    }
}

/*!
    \qmlproperty real ListView::preferredHighlightBegin
    \qmlproperty real ListView::preferredHighlightEnd
    \qmlproperty enumeration ListView::highlightRangeMode

    These properties set the preferred range of the highlight (current item)
    within the view.

    If highlightRangeMode is set to \e ApplyRange the view will
    attempt to maintain the highlight within the range, however
    the highlight can move outside of the range at the ends of the list
    or due to a mouse interaction.

    If highlightRangeMode is set to \e StrictlyEnforceRange the highlight will never
    move outside of the range.  This means that the current item will change
    if a keyboard or mouse action would cause the highlight to move
    outside of the range.

    The default value is \e NoHighlightRange.

    Note that a valid range requires preferredHighlightEnd to be greater
    than or equal to preferredHighlightBegin.
*/
qreal QmlGraphicsListView::preferredHighlightBegin() const
{
    Q_D(const QmlGraphicsListView);
    return d->highlightRangeStart;
}

void QmlGraphicsListView::setPreferredHighlightBegin(qreal start)
{
    Q_D(QmlGraphicsListView);
    d->highlightRangeStart = start;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
}

qreal QmlGraphicsListView::preferredHighlightEnd() const
{
    Q_D(const QmlGraphicsListView);
    return d->highlightRangeEnd;
}

void QmlGraphicsListView::setPreferredHighlightEnd(qreal end)
{
    Q_D(QmlGraphicsListView);
    d->highlightRangeEnd = end;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
}

QmlGraphicsListView::HighlightRangeMode QmlGraphicsListView::highlightRangeMode() const
{
    Q_D(const QmlGraphicsListView);
    return d->highlightRange;
}

void QmlGraphicsListView::setHighlightRangeMode(HighlightRangeMode mode)
{
    Q_D(QmlGraphicsListView);
    d->highlightRange = mode;
    d->haveHighlightRange = d->highlightRange != NoHighlightRange && d->highlightRangeStart <= d->highlightRangeEnd;
}

/*!
    \qmlproperty real ListView::spacing

    This property holds the spacing to leave between items.
*/
qreal QmlGraphicsListView::spacing() const
{
    Q_D(const QmlGraphicsListView);
    return d->spacing;
}

void QmlGraphicsListView::setSpacing(qreal spacing)
{
    Q_D(QmlGraphicsListView);
    if (spacing != d->spacing) {
        d->spacing = spacing;
        d->layout();
        emit spacingChanged();
    }
}

/*!
    \qmlproperty enumeration ListView::orientation
    This property holds the orientation of the list.

    Possible values are \c Vertical (default) and \c Horizontal.

    Vertical Example:
    \image trivialListView.png
    Horizontal Example:
    \image ListViewHorizontal.png
*/
QmlGraphicsListView::Orientation QmlGraphicsListView::orientation() const
{
    Q_D(const QmlGraphicsListView);
    return d->orient;
}

void QmlGraphicsListView::setOrientation(QmlGraphicsListView::Orientation orientation)
{
    Q_D(QmlGraphicsListView);
    if (d->orient != orientation) {
        d->orient = orientation;
        if (d->orient == QmlGraphicsListView::Vertical) {
            setViewportWidth(-1);
            setFlickDirection(VerticalFlick);
        } else {
            setViewportHeight(-1);
            setFlickDirection(HorizontalFlick);
        }
        d->clear();
        refill();
        emit orientationChanged();
        d->updateCurrent(d->currentIndex);
    }
}

/*!
    \qmlproperty bool ListView::keyNavigationWraps
    This property holds whether the list wraps key navigation

    If this property is true then key presses to move off of one end of the list will cause the
    current item to jump to the other end.
*/
bool QmlGraphicsListView::isWrapEnabled() const
{
    Q_D(const QmlGraphicsListView);
    return d->wrap;
}

void QmlGraphicsListView::setWrapEnabled(bool wrap)
{
    Q_D(QmlGraphicsListView);
    d->wrap = wrap;
}

/*!
    \qmlproperty int ListView::cacheBuffer
    This property holds the number of off-screen pixels to cache.

    This property determines the number of pixels above the top of the list
    and below the bottom of the list to cache.  Setting this value can make
    scrolling the list smoother at the expense of additional memory usage.
*/
int QmlGraphicsListView::cacheBuffer() const
{
    Q_D(const QmlGraphicsListView);
    return d->buffer;
}

void QmlGraphicsListView::setCacheBuffer(int b)
{
    Q_D(QmlGraphicsListView);
    if (d->buffer != b) {
        d->buffer = b;
        if (isComponentComplete()) {
            d->bufferMode = QmlGraphicsListViewPrivate::BufferBefore | QmlGraphicsListViewPrivate::BufferAfter;
            refill();
        }
    }
}

/*!
    \qmlproperty string ListView::section.property
    \qmlproperty enumeration ListView::section.criteria
    These properties hold the expression to be evaluated for the section attached property.

    section.property hold the name of the property to use to determine
    the section the item is in.

    section.criteria holds the criteria to use to get the section. It
    can be either:
    \list
    \o ViewSection.FullString (default) - section is the value of the property.
    \o ViewSection.FirstCharacter - section is the first character of the property value.
    \endlist

    Each item in the list has attached properties named \c ListView.section and
    \c ListView.prevSection.  These may be used to place a section header for
    related items.  The example below assumes that the model is sorted by size of
    pet.  The section expression is the size property.  If \c ListView.section and
    \c ListView.prevSection differ, the item will display a section header.

    \snippet examples/declarative/listview/sections.qml 0

    \image ListViewSections.png
*/
QmlGraphicsViewSection *QmlGraphicsListView::sectionCriteria()
{
    Q_D(QmlGraphicsListView);
    if (!d->sectionCriteria)
        d->sectionCriteria = new QmlGraphicsViewSection(this);
    return d->sectionCriteria;
}

/*!
    \qmlproperty string ListView::currentSection
    This property holds the section that is currently at the beginning of the view.
*/
QString QmlGraphicsListView::currentSection() const
{
    Q_D(const QmlGraphicsListView);
    return d->currentSection;
}

/*!
    \qmlproperty real ListView::highlightMoveSpeed
    \qmlproperty real ListView::highlightResizeSpeed
    These properties hold the move and resize animation speed of the highlight delegate.

    highlightFollowsCurrentItem must be true for these properties
    to have effect.

    The default value for these properties is 400 pixels/second.

    \sa highlightFollowsCurrentItem
*/
qreal QmlGraphicsListView::highlightMoveSpeed() const
{
    Q_D(const QmlGraphicsListView);\
    return d->highlightMoveSpeed;
}

void QmlGraphicsListView::setHighlightMoveSpeed(qreal speed)
{
    Q_D(QmlGraphicsListView);\
    if (d->highlightMoveSpeed != speed) {
        d->highlightMoveSpeed = speed;
        if (d->highlightPosAnimator)
            d->highlightPosAnimator->setVelocity(d->highlightMoveSpeed);
        emit highlightMoveSpeedChanged();
    }
}

qreal QmlGraphicsListView::highlightResizeSpeed() const
{
    Q_D(const QmlGraphicsListView);\
    return d->highlightResizeSpeed;
}

void QmlGraphicsListView::setHighlightResizeSpeed(qreal speed)
{
    Q_D(QmlGraphicsListView);\
    if (d->highlightResizeSpeed != speed) {
        d->highlightResizeSpeed = speed;
        if (d->highlightSizeAnimator)
            d->highlightSizeAnimator->setVelocity(d->highlightResizeSpeed);
        emit highlightResizeSpeedChanged();
    }
}

/*!
    \qmlproperty enumeration ListView::snapMode

    This property determines where the view will settle following a drag or flick.
    The allowed values are:

    \list
    \o NoSnap (default) - the view will stop anywhere within the visible area.
    \o SnapToItem - the view will settle with an item aligned with the start of
    the view.
    \o SnapOneItem - the view will settle no more than one item away from the first
    visible item at the time the mouse button is released.  This mode is particularly
    useful for moving one page at a time.
    \endlist
*/
QmlGraphicsListView::SnapMode QmlGraphicsListView::snapMode() const
{
    Q_D(const QmlGraphicsListView);
    return d->snapMode;
}

void QmlGraphicsListView::setSnapMode(SnapMode mode)
{
    Q_D(QmlGraphicsListView);
    if (d->snapMode != mode) {
        d->snapMode = mode;
    }
}

QmlComponent *QmlGraphicsListView::footer() const
{
    Q_D(const QmlGraphicsListView);
    return d->footerComponent;
}

void QmlGraphicsListView::setFooter(QmlComponent *footer)
{
    Q_D(QmlGraphicsListView);
    if (d->footerComponent != footer) {
        if (d->footer) {
            delete d->footer;
            d->footer = 0;
        }
        d->footerComponent = footer;
        d->minExtentDirty = true;
        d->maxExtentDirty = true;
        d->updateFooter();
        d->updateViewport();
    }
}

QmlComponent *QmlGraphicsListView::header() const
{
    Q_D(const QmlGraphicsListView);
    return d->headerComponent;
}

void QmlGraphicsListView::setHeader(QmlComponent *header)
{
    Q_D(QmlGraphicsListView);
    if (d->headerComponent != header) {
        if (d->header) {
            delete d->header;
            d->header = 0;
        }
        d->headerComponent = header;
        d->minExtentDirty = true;
        d->maxExtentDirty = true;
        d->updateHeader();
        d->updateFooter();
        d->updateViewport();
    }
}

void QmlGraphicsListView::viewportMoved()
{
    Q_D(QmlGraphicsListView);
    QmlGraphicsFlickable::viewportMoved();
    d->lazyRelease = true;
    refill();
    if (isFlicking() || d->moving)
        d->moveReason = QmlGraphicsListViewPrivate::Mouse;
    if (d->moveReason != QmlGraphicsListViewPrivate::SetIndex) {
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange && d->highlight) {
            // reposition highlight
            qreal pos = d->highlight->position();
            if (pos > d->position() + d->highlightRangeEnd - 1 - d->highlight->size())
                pos = d->position() + d->highlightRangeEnd - 1 - d->highlight->size();
            if (pos < d->position() + d->highlightRangeStart)
                pos = d->position() + d->highlightRangeStart;
            d->highlight->setPosition(pos);

            // update current index
            int idx = d->snapIndex();
            if (idx >= 0 && idx != d->currentIndex)
                d->updateCurrent(idx);
        }
    }

    if (d->flicked && d->correctFlick && !d->inFlickCorrection) {
        d->inFlickCorrection = true;
        // Near an end and it seems that the extent has changed?
        // Recalculate the flick so that we don't end up in an odd position.
        if (yflick()) {
            if (d->velocityY > 0) {
                const qreal minY = minYExtent();
                if ((minY - d->_moveY.value() < height()/2 || d->flickTargetY - d->_moveY.value() < height()/2)
                    && minY != d->flickTargetY)
                    d->flickY(-d->verticalVelocity.value());
                d->bufferMode = QmlGraphicsListViewPrivate::BufferBefore;
            } else if (d->velocityY < 0) {
                const qreal maxY = maxYExtent();
                if ((d->_moveY.value() - maxY < height()/2 || d->_moveY.value() - d->flickTargetY < height()/2)
                    && maxY != d->flickTargetY)
                    d->flickY(-d->verticalVelocity.value());
                d->bufferMode = QmlGraphicsListViewPrivate::BufferAfter;
            }
        }

        if (xflick()) {
            if (d->velocityX > 0) {
                const qreal minX = minXExtent();
                if ((minX - d->_moveX.value() < height()/2 || d->flickTargetX - d->_moveX.value() < height()/2)
                    && minX != d->flickTargetX)
                    d->flickX(-d->horizontalVelocity.value());
                d->bufferMode = QmlGraphicsListViewPrivate::BufferBefore;
            } else if (d->velocityX < 0) {
                const qreal maxX = maxXExtent();
                if ((d->_moveX.value() - maxX < height()/2 || d->_moveX.value() - d->flickTargetX < height()/2)
                    && maxX != d->flickTargetX)
                    d->flickX(-d->horizontalVelocity.value());
                d->bufferMode = QmlGraphicsListViewPrivate::BufferAfter;
            }
        }
        d->inFlickCorrection = false;
    }
}

qreal QmlGraphicsListView::minYExtent() const
{
    Q_D(const QmlGraphicsListView);
    if (d->orient == QmlGraphicsListView::Horizontal)
        return QmlGraphicsFlickable::minYExtent();
    if (d->minExtentDirty) {
        d->minExtent = -d->startPosition();
        if (d->header && d->visibleItems.count())
            d->minExtent += d->header->size();
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange)
            d->minExtent += d->highlightRangeStart;
        d->minExtentDirty = false;
    }

    return d->minExtent;
}

qreal QmlGraphicsListView::maxYExtent() const
{
    Q_D(const QmlGraphicsListView);
    if (d->orient == QmlGraphicsListView::Horizontal)
        return height();
    if (d->maxExtentDirty) {
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange)
            d->maxExtent = -(d->positionAt(count()-1) - d->highlightRangeEnd);
        else
            d->maxExtent = -(d->endPosition() - height() + 1);
        if (d->footer)
            d->maxExtent -= d->footer->size();
        qreal minY = minYExtent();
        if (d->maxExtent > minY)
            d->maxExtent = minY;
        d->maxExtentDirty = false;
    }
    return d->maxExtent;
}

qreal QmlGraphicsListView::minXExtent() const
{
    Q_D(const QmlGraphicsListView);
    if (d->orient == QmlGraphicsListView::Vertical)
        return QmlGraphicsFlickable::minXExtent();
    if (d->minExtentDirty) {
        d->minExtent = -d->startPosition();
        if (d->header)
            d->minExtent += d->header->size();
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange)
            d->minExtent += d->highlightRangeStart;
        d->minExtentDirty = false;
    }

    return d->minExtent;
}

qreal QmlGraphicsListView::maxXExtent() const
{
    Q_D(const QmlGraphicsListView);
    if (d->orient == QmlGraphicsListView::Vertical)
        return width();
    if (d->maxExtentDirty) {
        if (d->haveHighlightRange && d->highlightRange == StrictlyEnforceRange)
            d->maxExtent = -(d->positionAt(count()-1) - d->highlightRangeEnd);
        else
            d->maxExtent = -(d->endPosition() - width() + 1);
        if (d->footer)
            d->maxExtent -= d->footer->size();
        qreal minX = minXExtent();
        if (d->maxExtent > minX)
            d->maxExtent = minX;
        d->maxExtentDirty = false;
    }

    return d->maxExtent;
}

void QmlGraphicsListView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsListView);
    QmlGraphicsFlickable::keyPressEvent(event);
    if (event->isAccepted())
        return;

    if (d->model && d->model->count() && d->interactive) {
        if ((d->orient == QmlGraphicsListView::Horizontal && event->key() == Qt::Key_Left)
                    || (d->orient == QmlGraphicsListView::Vertical && event->key() == Qt::Key_Up)) {
            if (currentIndex() > 0 || (d->wrap && !event->isAutoRepeat())) {
                decrementCurrentIndex();
                event->accept();
                return;
            } else if (d->wrap) {
                event->accept();
                return;
            }
        } else if ((d->orient == QmlGraphicsListView::Horizontal && event->key() == Qt::Key_Right)
                    || (d->orient == QmlGraphicsListView::Vertical && event->key() == Qt::Key_Down)) {
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
}

/*!
    \qmlmethod ListView::incrementCurrentIndex()

    Increments the current index.  The current index will wrap
    if keyNavigationWraps is true and it is currently at the end.
*/
void QmlGraphicsListView::incrementCurrentIndex()
{
    Q_D(QmlGraphicsListView);
    if (currentIndex() < d->model->count() - 1 || d->wrap) {
        d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
        int index = currentIndex()+1;
        cancelFlick();
        d->updateCurrent(index < d->model->count() ? index : 0);
    }
}

/*!
    \qmlmethod ListView::decrementCurrentIndex()

    Decrements the current index.  The current index will wrap
    if keyNavigationWraps is true and it is currently at the beginning.
*/
void QmlGraphicsListView::decrementCurrentIndex()
{
    Q_D(QmlGraphicsListView);
    if (currentIndex() > 0 || d->wrap) {
        d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
        int index = currentIndex()-1;
        cancelFlick();
        d->updateCurrent(index >= 0 ? index : d->model->count()-1);
    }
}

/*!
    \qmlmethod ListView::positionViewAtIndex(int index)

    Positions the view such that the \a index is at the top (or left for horizontal orientation) of the view.
    If positioning the view at the index would cause empty space to be displayed at
    the end of the view, the view will be positioned at the end.
*/
void QmlGraphicsListView::positionViewAtIndex(int index)
{
    Q_D(QmlGraphicsListView);
    if (!d->isValid() || index < 0 || index >= d->model->count())
        return;

    qreal maxExtent = d->orient == QmlGraphicsListView::Vertical ? -maxYExtent() : -maxXExtent();
    FxListItem *item = d->visibleItem(index);
    if (item) {
        // Already created - just move to top of view
        int pos = qMin(item->position(), maxExtent);
        d->setPosition(pos);
    } else {
        int pos = d->positionAt(index);
        // save the currently visible items in case any of them end up visible again
        QList<FxListItem*> oldVisible = d->visibleItems;
        d->visibleItems.clear();
        d->visiblePos = pos;
        d->visibleIndex = index;
        d->setPosition(pos);
        // setPosition() will cause refill.  Adjust if we have moved beyond range.
        if (d->position() > maxExtent)
            d->setPosition(maxExtent);
        // now release the reference to all the old visible items.
        for (int i = 0; i < oldVisible.count(); ++i)
            d->releaseItem(oldVisible.at(i));
    }
}


void QmlGraphicsListView::componentComplete()
{
    Q_D(QmlGraphicsListView);
    QmlGraphicsFlickable::componentComplete();
    refill();
    d->moveReason = QmlGraphicsListViewPrivate::SetIndex;
    if (d->currentIndex < 0)
        d->updateCurrent(0);
    else
        d->updateCurrent(d->currentIndex);
    d->fixupPosition();
}

void QmlGraphicsListView::refill()
{
    Q_D(QmlGraphicsListView);
    d->refill(d->position(), d->position()+d->size()-1);
}

void QmlGraphicsListView::trackedPositionChanged()
{
    Q_D(QmlGraphicsListView);
    if (!d->trackedItem || !d->currentItem)
        return;
    if (!isFlicking() && !d->moving && d->moveReason == QmlGraphicsListViewPrivate::SetIndex) {
        const qreal trackedPos = d->trackedItem->position();
        const qreal viewPos = d->position();
        if (d->haveHighlightRange) {
            if (d->highlightRange == StrictlyEnforceRange) {
                qreal pos = viewPos;
                if (trackedPos > pos + d->highlightRangeEnd - d->trackedItem->size())
                    pos = trackedPos - d->highlightRangeEnd + d->trackedItem->size();
                if (trackedPos < pos + d->highlightRangeStart)
                    pos = trackedPos - d->highlightRangeStart;
                d->setPosition(pos);
            } else {
                qreal pos = viewPos;
                if (trackedPos < d->startPosition() + d->highlightRangeStart) {
                    pos = d->startPosition();
                } else if (d->trackedItem->endPosition() > d->endPosition() - d->size() + d->highlightRangeEnd) {
                    pos = d->endPosition() - d->size();
                    if (pos < d->startPosition())
                        pos = d->startPosition();
                } else {
                    if (trackedPos < viewPos + d->highlightRangeStart) {
                        pos = trackedPos - d->highlightRangeStart;
                    } else if (trackedPos > viewPos + d->highlightRangeEnd - d->trackedItem->size()) {
                        pos = trackedPos - d->highlightRangeEnd + d->trackedItem->size();
                    }
                }
                d->setPosition(pos);
            }
        } else {
            if (trackedPos < viewPos && d->currentItem->position() < viewPos) {
                d->setPosition(d->currentItem->position() < trackedPos ? trackedPos : d->currentItem->position());
            } else if (d->trackedItem->endPosition() > viewPos + d->size()
                        && d->currentItem->endPosition() > viewPos + d->size()) {
                qreal pos;
                if (d->trackedItem->endPosition() < d->currentItem->endPosition()) {
                    pos = d->trackedItem->endPosition() - d->size();
                    if (d->trackedItem->size() > d->size())
                        pos = trackedPos;
                } else {
                    pos = d->currentItem->endPosition() - d->size();
                    if (d->currentItem->size() > d->size())
                        pos = d->currentItem->position();
                }
                d->setPosition(pos);
            }
        }
    }
}

void QmlGraphicsListView::itemsInserted(int modelIndex, int count)
{
    Q_D(QmlGraphicsListView);
    d->updateUnrequestedIndexes();
    d->moveReason = QmlGraphicsListViewPrivate::Other;
    if (!d->visibleItems.count() || d->model->count() <= 1) {
        d->layout();
        d->updateCurrent(qMax(0, qMin(d->currentIndex, d->model->count()-1)));
        emit countChanged();
        return;
    }

    int overlapCount = count;
    if (!d->mapRangeFromModel(modelIndex, overlapCount)) {
        int i = d->visibleItems.count() - 1;
        while (i > 0 && d->visibleItems.at(i)->index == -1)
            --i;
        if (d->visibleItems.at(i)->index + 1 == modelIndex) {
            // Special case of appending an item to the model.
            modelIndex = d->visibleIndex + d->visibleItems.count();
        } else {
            if (modelIndex < d->visibleIndex) {
                // Insert before visible items
                d->visibleIndex += count;
                for (int i = 0; i < d->visibleItems.count(); ++i) {
                    FxListItem *listItem = d->visibleItems.at(i);
                    if (listItem->index != -1 && listItem->index >= modelIndex)
                        listItem->index += count;
                }
            }
            if (d->currentIndex >= modelIndex) {
                // adjust current item index
                d->currentIndex += count;
                if (d->currentItem)
                    d->currentItem->index = d->currentIndex;
            }
            d->layout();
            emit countChanged();
            return;
        }
    }

    // At least some of the added items will be visible

    int index = modelIndex - d->visibleIndex;
    // index can be the next item past the end of the visible items list (i.e. appended)
    int pos = index < d->visibleItems.count() ? d->visibleItems.at(index)->position()
                                                : d->visibleItems.at(index-1)->endPosition()+d->spacing+1;
    int initialPos = pos;
    int diff = 0;
    QList<FxListItem*> added;
    bool addedVisible = false;
    FxListItem *firstVisible = d->firstVisibleItem();
    if (firstVisible && pos < firstVisible->position()) {
        // Insert items before the visible item.
        int insertionIdx = index;
        int i = 0;
        int from = d->position() - d->buffer;
        for (i = count-1; i >= 0 && pos > from; --i) {
            addedVisible = true;
            FxListItem *item = d->createItem(modelIndex + i);
            d->visibleItems.insert(insertionIdx, item);
            pos -= item->size() + d->spacing;
            item->setPosition(pos);
            index++;
        }
        if (i >= 0) {
            // If we didn't insert all our new items - anything
            // before the current index is not visible - remove it.
            while (insertionIdx--) {
                FxListItem *item = d->visibleItems.takeFirst();
                if (item->index != -1)
                    d->visibleIndex++;
                d->releaseItem(item);
            }
        } else {
            // adjust pos of items before inserted items.
            for (int i = insertionIdx-1; i >= 0; i--) {
                FxListItem *listItem = d->visibleItems.at(i);
                listItem->setPosition(listItem->position() - (initialPos - pos));
            }
        }
    } else {
        int i = 0;
        int to = d->buffer+d->position()+d->size()-1;
        for (i = 0; i < count && pos <= to; ++i) {
            addedVisible = true;
            FxListItem *item = d->createItem(modelIndex + i);
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
    if (d->currentIndex >= modelIndex) {
        // adjust current item index
        d->currentIndex += count;
        if (d->currentItem) {
            d->currentItem->index = d->currentIndex;
            d->currentItem->setPosition(d->currentItem->position() + diff);
        }
    }
    // Update the indexes of the following visible items.
    for (; index < d->visibleItems.count(); ++index) {
        FxListItem *listItem = d->visibleItems.at(index);
        if (d->currentItem && listItem->item != d->currentItem->item)
            listItem->setPosition(listItem->position() + diff);
        if (listItem->index != -1)
            listItem->index += count;
    }
    // everything is in order now - emit add() signal
    for (int j = 0; j < added.count(); ++j)
        added.at(j)->attached->emitAdd();

    if (addedVisible)
        d->layout();
    emit countChanged();
}

void QmlGraphicsListView::itemsRemoved(int modelIndex, int count)
{
    Q_D(QmlGraphicsListView);
    d->moveReason = QmlGraphicsListViewPrivate::Other;
    d->updateUnrequestedIndexes();

    FxListItem *firstVisible = d->firstVisibleItem();
    int preRemovedSize = 0;
    bool removedVisible = false;
    // Remove the items from the visible list, skipping anything already marked for removal
    QList<FxListItem*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxListItem *item = *it;
        if (item->index == -1 || item->index < modelIndex) {
            // already removed, or before removed items
            ++it;
        } else if (item->index >= modelIndex + count) {
            // after removed items
            item->index -= count;
            ++it;
        } else {
            // removed item
            removedVisible = true;
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
    } else if (d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count) {
        // current item has been removed.
        d->currentItem->attached->setIsCurrentItem(false);
        d->releaseItem(d->currentItem);
        d->currentItem = 0;
        d->currentIndex = -1;
        d->updateCurrent(qMin(modelIndex, d->model->count()-1));
    }

    // update visibleIndex
    for (it = d->visibleItems.begin(); it != d->visibleItems.end(); ++it) {
        if ((*it)->index != -1) {
            d->visibleIndex = (*it)->index;
            break;
        }
    }

    if (removedVisible) {
        if (d->visibleItems.isEmpty()) {
            d->visibleIndex = 0;
            d->visiblePos = d->header ? d->header->size() : 0;
            d->timeline.clear();
            d->setPosition(0);
            if (d->model->count() == 0)
                update();
            else
                refill();
        } else {
            // Correct the positioning of the items
            d->layout();
        }
    }

    emit countChanged();
}

void QmlGraphicsListView::destroyRemoved()
{
    Q_D(QmlGraphicsListView);
    for (QList<FxListItem*>::Iterator it = d->visibleItems.begin();
            it != d->visibleItems.end();) {
        FxListItem *listItem = *it;
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

void QmlGraphicsListView::itemsMoved(int from, int to, int count)
{
    Q_D(QmlGraphicsListView);
    d->updateUnrequestedIndexes();

    if (d->visibleItems.isEmpty()) {
        refill();
        return;
    }

    d->moveReason = QmlGraphicsListViewPrivate::Other;
    FxListItem *firstVisible = d->firstVisibleItem();
    qreal firstItemPos = firstVisible->position();
    QHash<int,FxListItem*> moved;
    int moveBy = 0;

    QList<FxListItem*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxListItem *item = *it;
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
        FxListItem *item = *it;
        if (remaining && item->index >= to && item->index < to + count) {
            // place items in the target position, reusing any existing items
            FxListItem *movedItem = moved.take(item->index);
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
    while (FxListItem *item = moved.take(endIndex+1)) {
        d->visibleItems.append(item);
        ++endIndex;
    }

    // Whatever moved items remain are no longer visible items.
    while (moved.count())
        d->releaseItem(moved.take(moved.begin().key()));

    // Ensure we don't cause an ugly list scroll.
    d->visibleItems.first()->setPosition(d->visibleItems.first()->position() + moveBy);

    d->layout();
}

void QmlGraphicsListView::createdItem(int index, QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsListView);
    if (d->requestedIndex != index) {
        item->setParentItem(viewport());
        d->unrequestedItems.insert(item, index);
        if (d->orient == QmlGraphicsListView::Vertical)
            item->setY(d->positionAt(index));
        else
            item->setX(d->positionAt(index));
    }
}

void QmlGraphicsListView::destroyingItem(QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsListView);
    d->unrequestedItems.remove(item);
}

void QmlGraphicsListView::animStopped()
{
    Q_D(QmlGraphicsListView);
    d->moveReason = QmlGraphicsListViewPrivate::Other;
    d->bufferMode = QmlGraphicsListViewPrivate::NoBuffer;
}

QmlGraphicsListViewAttached *QmlGraphicsListView::qmlAttachedProperties(QObject *obj)
{
    return new QmlGraphicsListViewAttached(obj);
}

QML_DEFINE_TYPE(Qt,4,6,ListView,QmlGraphicsListView)
QML_DEFINE_TYPE(Qt,4,6,ViewSection,QmlGraphicsViewSection)

QT_END_NAMESPACE

#include <qmlgraphicslistview.moc>
