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

#include "private/qfxflickable_p.h"
#include "qmlfollow.h"
#include "qlistmodelinterface.h"
#include "qfxvisualitemmodel.h"
#include "qfxlistview.h"
#include <qmlexpression.h>

#include <QKeyEvent>

QT_BEGIN_NAMESPACE
class QFxListViewAttached : public QObject
{
    Q_OBJECT
public:
    QFxListViewAttached(QObject *parent)
        : QObject(parent), m_view(0), m_isCurrent(false), m_delayRemove(false) {}
    ~QFxListViewAttached() {
        attachedProperties.remove(parent());
    }

    Q_PROPERTY(QFxListView *view READ view)
    QFxListView *view() { return m_view; }

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

    static QFxListViewAttached *properties(QObject *obj) {
        QFxListViewAttached *rv = attachedProperties.value(obj);
        if (!rv) {
            rv = new QFxListViewAttached(obj);
            attachedProperties.insert(obj, rv);
        }
        return rv;
    }

    void emitAdd() { emit add(); }
    void emitRemove() { emit remove(); }

signals:
    void currentItemChanged();
    void sectionChanged();
    void prevSectionChanged();
    void delayRemoveChanged();
    void add();
    void remove();

public:
    QFxListView *m_view;
    bool m_isCurrent;
    mutable QString m_section;
    QString m_prevSection;
    bool m_delayRemove;

    static QHash<QObject*, QFxListViewAttached*> attachedProperties;
};

QHash<QObject*, QFxListViewAttached*> QFxListViewAttached::attachedProperties;

//----------------------------------------------------------------------------

class FxListItem
{
public:
    FxListItem(QFxItem *i, QFxListView *v) : item(i), view(v) {
        attached = QFxListViewAttached::properties(item);
        attached->m_view = view;
    }
    ~FxListItem() {}

    qreal position() const { return (view->orientation() == Qt::Vertical ? item->y() : item->x()); }
    int size() const { return (view->orientation() == Qt::Vertical ? item->height() : item->width()); }
    qreal endPosition() const {
        return (view->orientation() == Qt::Vertical
                                        ? item->y() + (item->height() > 0 ? item->height() : 1)
                                        : item->x() + (item->width() > 0 ? item->width() : 1)) - 1;
    }
    void setPosition(qreal pos) {
        if (view->orientation() == Qt::Vertical) {
            item->setY(pos);
        } else {
            item->setX(pos);
        }
    }

    QFxItem *item;
    QFxListView *view;
    QFxListViewAttached *attached;
    int index;
};

//----------------------------------------------------------------------------

class QFxListViewPrivate : public QFxFlickablePrivate
{
    Q_DECLARE_PUBLIC(QFxListView);

public:
    QFxListViewPrivate()
        : model(0), currentItem(0), tmpCurrent(0), orient(Qt::Vertical)
        , visiblePos(0), visibleIndex(0)
        , averageSize(100), currentIndex(-1), requestedIndex(-1)
        , currItemMode(QFxListView::Free), snapPos(0), highlightComponent(0), highlight(0), trackedItem(0)
        , moveReason(Other), buffer(0), highlightPosAnimator(0), highlightSizeAnimator(0)
        , ownModel(false), wrap(false), autoHighlight(true)
        , fixCurrentVisibility(false) {}

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

    qreal position() const {
        Q_Q(const QFxListView);
        return orient == Qt::Vertical ? q->yPosition() : q->xPosition();
    }
    void setPosition(qreal pos) {
        Q_Q(QFxListView);
        if (orient == Qt::Vertical)
            q->setYPosition(pos);
        else
            q->setXPosition(pos);
    }
    int size() const {
        Q_Q(const QFxListView);
        return orient == Qt::Vertical ? q->height() : q->width();
    }

    qreal startPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty())
            pos = visibleItems.first()->position() - visibleIndex * averageSize;
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
            pos = visibleItems.last()->endPosition() + invisibleCount * averageSize;
        }
        return pos;
    }

    qreal positionAt(int modelIndex) const {
        if (FxListItem *item = visibleItem(modelIndex))
            return item->position();
        if (!visibleItems.isEmpty()) {
            if (modelIndex < visibleIndex) {
                int count = visibleIndex - modelIndex;
                return visibleItems.first()->position() - count * averageSize;
            } else {
                int idx = visibleItems.count() - 1;
                while (idx >= 0 && visibleItems.at(idx)->index == -1)
                    --idx;
                if (idx < 0)
                    idx = visibleIndex;
                else
                    idx = visibleItems.at(idx)->index;
                int count = modelIndex - idx - 1;
                return visibleItems.last()->endPosition() + count * averageSize + 1;
            }
        }
        return 0;
    }

    QString sectionAt(int modelIndex) {
        Q_Q(QFxListView);
        if (FxListItem *item = visibleItem(modelIndex))
            return item->attached->section();
        QString section;
        if (!sectionExpression.isEmpty())
            section = model->evaluate(modelIndex, sectionExpression, q).toString();
        return section;
    }

    bool isValid() const {
        return model && model->count() && (!ownModel || model->delegate());
    }

    int snapIndex() {
        qreal pos = position();
        for (int i = 0; i < visibleItems.count(); ++i) {
            qreal itemTop = visibleItems[i]->position() - pos;
            if (itemTop >= snapPos-averageSize/2 && itemTop < snapPos+averageSize/2)
                return visibleItems[i]->index;
        }
        return -1;
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

    void refill(qreal from, qreal to);
    void layout();
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateTrackedItem();
    void createHighlight();
    void updateHighlight();
    void updateSections();
    void updateCurrentSection();
    void updateCurrent(int);
    void updateAverage();
    void fixupPosition();
    virtual void fixupY();
    virtual void fixupX();

    QFxVisualItemModel *model;
    QVariant modelVariant;
    QList<FxListItem*> visibleItems;
    QHash<QFxItem*,int> unrequestedItems;
    FxListItem *currentItem;
    QFxItem *tmpCurrent;
    Qt::Orientation orient;
    int visiblePos;
    int visibleIndex;
    qreal averageSize;
    int currentIndex;
    int requestedIndex;
    QFxListView::CurrentItemPositioning currItemMode;
    int snapPos;
    QmlComponent *highlightComponent;
    FxListItem *highlight;
    FxListItem *trackedItem;
    QFxItem *activeItem; //XXX fix
    enum MovementReason { Other, Key, Mouse };
    MovementReason moveReason;
    int buffer;
    QmlFollow *highlightPosAnimator;
    QmlFollow *highlightSizeAnimator;
    QString sectionExpression;
    QString currentSection;

    int ownModel : 1;
    int wrap : 1;
    int autoHighlight : 1;
    int fixCurrentVisibility : 1;
};

void QFxListViewPrivate::init()
{
    Q_Q(QFxListView);
    q->setOptions(QFxListView::IsFocusRealm);
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(refill()));
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(refill()));
}

void QFxListViewPrivate::clear()
{
    for (int i = 0; i < visibleItems.count(); ++i)
        releaseItem(visibleItems.at(i));
    visibleItems.clear();
    visiblePos = 0;
    visibleIndex = 0;
    releaseItem(currentItem);
    currentItem = 0;
    currentIndex = -1;
    createHighlight();
    trackedItem = 0;
}

FxListItem *QFxListViewPrivate::createItem(int modelIndex)
{
    Q_Q(QFxListView);
    // create object
    requestedIndex = modelIndex;
    FxListItem *listItem = 0;
    if (QFxItem *item = model->item(modelIndex, false)) {
        listItem = new FxListItem(item, q);
        listItem->index = modelIndex;
        // initialise attached properties
        if (!sectionExpression.isEmpty()) {
            QmlExpression e(qmlContext(listItem->item), sectionExpression, q);
            e.setTrackChange(false);
            listItem->attached->m_section = e.value().toString();
            if (modelIndex > 0) {
                if (FxListItem *item = visibleItem(modelIndex-1))
                    listItem->attached->m_prevSection = item->attached->section();
                else
                    listItem->attached->m_prevSection = sectionAt(modelIndex-1);
            }
        }
        // complete
        model->completeItem();
        listItem->item->setZValue(modelIndex + 1);
        listItem->item->setParent(q->viewport());
        if (orient == Qt::Vertical)
            QObject::connect(listItem->item, SIGNAL(heightChanged()), q, SLOT(itemResized()));
        else
            QObject::connect(listItem->item, SIGNAL(widthChanged()), q, SLOT(itemResized()));
    }
    requestedIndex = -1;

    return listItem;
}

void QFxListViewPrivate::releaseItem(FxListItem *item)
{
    Q_Q(QFxListView);
    if (!item)
        return;
    if (trackedItem == item) {
        const char *notifier1 = orient == Qt::Vertical ? SIGNAL(yChanged()) : SIGNAL(xChanged());
        const char *notifier2 = orient == Qt::Vertical ? SIGNAL(heightChanged()) : SIGNAL(widthChanged());
        QObject::disconnect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }
    if (model->release(item->item) == 0) {
        // item was not destroyed, and we no longer reference it.
        unrequestedItems.insert(item->item, model->indexOf(item->item, q));
        if (orient == Qt::Vertical)
            QObject::disconnect(item->item, SIGNAL(heightChanged()), q, SLOT(itemResized()));
        else
            QObject::disconnect(item->item, SIGNAL(widthChanged()), q, SLOT(itemResized()));
    }
    delete item;
}

void QFxListViewPrivate::refill(qreal from, qreal to)
{
    Q_Q(QFxListView);
    if (!isValid() || !q->isComponentComplete())
        return;
    from -= buffer;
    to += buffer;
    int modelIndex = 0;
    qreal itemEnd = visiblePos-1;
    if (!visibleItems.isEmpty()) {
        visiblePos = visibleItems.first()->position();
        itemEnd = visibleItems.last()->endPosition();
        int i = visibleItems.count() - 1;
        while (i > 0 && visibleItems.at(i)->index == -1)
            --i;
        modelIndex = visibleItems.at(i)->index + 1;
    }

    bool changed = false;
    FxListItem *item = 0;
    int pos = itemEnd + 1;
    while (modelIndex < model->count() && pos <= to) {
        //qDebug() << "refill: append item" << modelIndex;
        if (!(item = createItem(modelIndex)))
            break;
        item->setPosition(pos);
        pos += item->size();
        visibleItems.append(item);
        ++modelIndex;
        changed = true;
    }
    while (visibleIndex > 0 && visibleIndex <= model->count() && visiblePos > from) {
        //qDebug() << "refill: prepend item" << visibleIndex-1 << "current top pos" << visiblePos;
        if (!(item = createItem(visibleIndex-1)))
            break;
        --visibleIndex;
        visiblePos -= item->size();
        item->setPosition(visiblePos);
        visibleItems.prepend(item);
        changed = true;
    }

    while (visibleItems.count() > 1 && (item = visibleItems.first()) && item->endPosition() < from) {
        if (item->attached->delayRemove())
            break;
        //qDebug() << "refill: remove first" << visibleIndex << "top end pos" << item->endPosition();
        if (item->index != -1)
            visibleIndex++;
        visibleItems.removeFirst();
        releaseItem(item);
        changed = true;
    }
    while (visibleItems.count() > 1 && (item = visibleItems.last()) && item->position() > to) {
        if (item->attached->delayRemove())
            break;
        //qDebug() << "refill: remove last" << visibleIndex+visibleItems.count()-1;
        visibleItems.removeLast();
        releaseItem(item);
        changed = true;
    }
    if (changed) {
        if (visibleItems.count())
            visiblePos = visibleItems.first()->position();
        updateAverage();
        if (!sectionExpression.isEmpty())
            updateCurrentSection();
        if (orient == Qt::Vertical)
            q->setViewportHeight(endPosition() - startPosition());
        else
            q->setViewportWidth(endPosition() - startPosition());
    }
}

void QFxListViewPrivate::layout()
{
    Q_Q(QFxListView);
    if (!visibleItems.isEmpty()) {
        int oldEnd = visibleItems.last()->endPosition();
        int pos = visibleItems.first()->endPosition() + 1;
        for (int i=1; i < visibleItems.count(); ++i) {
            FxListItem *item = visibleItems.at(i);
            item->setPosition(pos);
            pos += item->size();
        }
        // move current item if it is after the visible items.
        if (currentItem && currentIndex > visibleItems.last()->index)
            currentItem->setPosition(currentItem->position() + (visibleItems.last()->endPosition() - oldEnd));
    }
    if (!isValid())
        return;
    q->refill();
    q->trackedPositionChanged();
    updateHighlight();
    if (orient == Qt::Vertical) {
        fixupY();
        q->setViewportHeight(endPosition() - startPosition());
    } else {
        fixupX();
        q->setViewportWidth(endPosition() - startPosition());
    }
    updateUnrequestedPositions();
}

void QFxListViewPrivate::updateUnrequestedIndexes()
{
    Q_Q(QFxListView);
    QHash<QFxItem*,int>::iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it)
        *it = model->indexOf(it.key(), q);
}

void QFxListViewPrivate::updateUnrequestedPositions()
{
    QHash<QFxItem*,int>::const_iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it) {
        if (visibleItem(*it))
            continue;
        if (orient == Qt::Vertical)
            it.key()->setY(positionAt(*it));
        else
            it.key()->setX(positionAt(*it));
    }
}

void QFxListViewPrivate::updateTrackedItem()
{
    Q_Q(QFxListView);
    FxListItem *item = currentItem;
    if (highlight)
        item = highlight;

    const char *notifier1 = orient == Qt::Vertical ? SIGNAL(yChanged()) : SIGNAL(xChanged());
    const char *notifier2 = orient == Qt::Vertical ? SIGNAL(heightChanged()) : SIGNAL(widthChanged());

    if (trackedItem && item != trackedItem) {
        QObject::disconnect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::disconnect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
        trackedItem = 0;
    }

    if (!trackedItem && item) {
        trackedItem = item;
        QObject::connect(trackedItem->item, notifier1, q, SLOT(trackedPositionChanged()));
        QObject::connect(trackedItem->item, notifier2, q, SLOT(trackedPositionChanged()));
        q->trackedPositionChanged();
    }
    if (trackedItem)
        q->trackedPositionChanged();
}

void QFxListViewPrivate::createHighlight()
{
    Q_Q(QFxListView);
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
    }

    if (currentItem) {
        QFxItem *item = 0;
        if (highlightComponent) {
            QmlContext *highlightContext = new QmlContext(qmlContext(q));
            QObject *nobj = highlightComponent->create(highlightContext);
            if (nobj) {
                highlightContext->setParent(nobj);
                item = qobject_cast<QFxItem *>(nobj);
                if (!item) {
                    delete nobj;
                } else {
                    item->setParent(q->viewport());
                }
            } else {
                delete highlightContext;
            }
        } else {
            item = new QFxItem;
            item->setParent(q->viewport());
        }
        if (item) {
            highlight = new FxListItem(item, q);
            const QLatin1String posProp(orient == Qt::Vertical ? "y" : "x");
            highlightPosAnimator = new QmlFollow(q);
            highlightPosAnimator->setTarget(QmlMetaProperty(highlight->item, posProp));
            highlightPosAnimator->setSpring(3);
            highlightPosAnimator->setDamping(0.3);
            highlightPosAnimator->setEnabled(autoHighlight);
            const QLatin1String sizeProp(orient == Qt::Vertical ? "height" : "width");
            highlightSizeAnimator = new QmlFollow(q);
            highlightSizeAnimator->setTarget(QmlMetaProperty(highlight->item, sizeProp));
            highlightSizeAnimator->setEnabled(autoHighlight);
        }
    }
}

void QFxListViewPrivate::updateHighlight()
{
    if ((!currentItem && highlight) || (currentItem && !highlight))
        createHighlight();
    updateTrackedItem();
    if (currentItem && autoHighlight && highlight) {
        // auto-update highlight
        highlightPosAnimator->setSourceValue(currentItem->position());
        highlightSizeAnimator->setSourceValue(currentItem->size());
        if (orient == Qt::Vertical) {
            if (highlight->item->width() == 0)
                highlight->item->setWidth(currentItem->item->width());
        } else {
            if (highlight->item->height() == 0)
                highlight->item->setHeight(currentItem->item->height());
        }
    }
}

void QFxListViewPrivate::updateSections()
{
    if (!sectionExpression.isEmpty()) {
        QString prevSection;
        if (visibleIndex > 0)
            prevSection = sectionAt(visibleIndex-1);
        for (int i = 0; i < visibleItems.count(); ++i) {
            QFxListViewAttached *attached = visibleItems.at(i)->attached;
            attached->setPrevSection(prevSection);
            prevSection = attached->section();
        }
    }
}

void QFxListViewPrivate::updateCurrentSection()
{
    if (sectionExpression.isEmpty() || visibleItems.isEmpty()) {
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

void QFxListViewPrivate::updateCurrent(int modelIndex)
{
    Q_Q(QFxListView);
    if (!isValid() || modelIndex < 0 || modelIndex >= model->count()) {
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

    if (tmpCurrent) {
        delete tmpCurrent;
        tmpCurrent = 0;
    }
    FxListItem *oldCurrentItem = currentItem;
    currentIndex = modelIndex;
    currentItem = createItem(modelIndex);
    fixCurrentVisibility = true;
    if (oldCurrentItem && (!currentItem || oldCurrentItem->item != currentItem->item))
        oldCurrentItem->attached->setIsCurrentItem(false);
    if (currentItem) {
        if (modelIndex == visibleIndex - 1) {
            // We can calculate exact postion in this case
            currentItem->setPosition(visibleItems.first()->position() - currentItem->size());
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

void QFxListViewPrivate::updateAverage()
{
    if (!visibleItems.count())
        return;
    qreal sum = 0.0;
    for (int i = 0; i < visibleItems.count(); ++i)
        sum += visibleItems.at(i)->size();
    averageSize = sum / visibleItems.count();
}

void QFxListViewPrivate::fixupPosition()
{
    if (orient == Qt::Vertical)
        fixupY();
    else
        fixupX();
}

void QFxListViewPrivate::fixupY()
{
    Q_Q(QFxListView);
    QFxFlickablePrivate::fixupY();
    if (orient == Qt::Horizontal)
        return;
    if (currItemMode == QFxListView::SnapAuto) {
        if (currentItem) {
            moveReason = Mouse;
            _tl.clear();
            _tl.move(_moveY, -(currentItem->position() - snapPos), QEasingCurve(QEasingCurve::InOutQuad), 200);
        }
    } else if (currItemMode == QFxListView::Snap) {
        moveReason = Mouse;
        int idx = snapIndex();
        if (FxListItem *snapItem = visibleItem(idx)) {
            int pos = snapItem->position() - snapPos;
            if (pos > -q->maxYExtent())
                pos = -q->maxYExtent();
            else if (pos < -q->minYExtent())
                pos = -q->minYExtent();
            _tl.clear();
            _tl.move(_moveY, -(pos), QEasingCurve(QEasingCurve::InOutQuad), 200);
        }
    }
}

void QFxListViewPrivate::fixupX()
{
    Q_Q(QFxListView);
    QFxFlickablePrivate::fixupX();
    if (orient == Qt::Vertical)
        return;
    if (currItemMode == QFxListView::SnapAuto) {
        if (currentItem) {
            moveReason = Mouse;
            _tl.clear();
            _tl.move(_moveX, -(currentItem->position() - snapPos), QEasingCurve(QEasingCurve::InOutQuad), 200);
        }
    } else if (currItemMode == QFxListView::Snap) {
        moveReason = Mouse;
        int idx = snapIndex();
        if (FxListItem *snapItem = visibleItem(idx)) {
            int pos = snapItem->position() - snapPos;
            if (pos > -q->maxXExtent())
                pos = -q->maxXExtent();
            else if (pos < -q->minXExtent())
                pos = -q->minXExtent();
            _tl.clear();
            _tl.move(_moveX, -(pos), QEasingCurve(QEasingCurve::InOutQuad), 200);
        }
    }
}

//----------------------------------------------------------------------------

/*!
    \qmlclass ListView
    \inherits Flickable
    \brief The ListView item provides a list view of items provided by a model.

    The model is typically provided by a QAbstractListModel "C++ model object", but can also be created directly in QML.
    The items are laid out vertically or horizontally and may be flicked to scroll.

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

QFxListView::QFxListView(QFxItem *parent)
    : QFxFlickable(*(new QFxListViewPrivate), parent)
{
    Q_D(QFxListView);
    d->init();
}

QFxListView::~QFxListView()
{
    Q_D(QFxListView);
    if (d->ownModel)
        delete d->model;
}

/*!
    \qmlproperty model ListView::model
    This property holds the model providing data for the list.

    The model provides a set of data that is used to create the items
    for the view.  For large or dynamic datasets the model is usually
    provided by a C++ model object.  The C++ model object must be a \l
    {QAbstractItemModel} subclass, a VisualModel, or a simple list.

    Models can also be created directly in QML, using a \l{ListModel}.
*/
QVariant QFxListView::model() const
{
    Q_D(const QFxListView);
    return d->modelVariant;
}

void QFxListView::setModel(const QVariant &model)
{
    Q_D(QFxListView);
    if (d->model) {
        disconnect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        disconnect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        disconnect(d->model, SIGNAL(createdItem(int, QFxItem*)), this, SLOT(createdItem(int,QFxItem*)));
        disconnect(d->model, SIGNAL(destroyingItem(QFxItem*)), this, SLOT(destroyingItem(QFxItem*)));
    }
    d->clear();
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
        if (d->currentIndex >= d->model->count() || d->currentIndex < 0)
            setCurrentIndex(0);
        else
            d->updateCurrent(d->currentIndex);
        connect(d->model, SIGNAL(itemsInserted(int,int)), this, SLOT(itemsInserted(int,int)));
        connect(d->model, SIGNAL(itemsRemoved(int,int)), this, SLOT(itemsRemoved(int,int)));
        connect(d->model, SIGNAL(createdItem(int, QFxItem*)), this, SLOT(createdItem(int,QFxItem*)));
        connect(d->model, SIGNAL(destroyingItem(QFxItem*)), this, SLOT(destroyingItem(QFxItem*)));
        refill();
        emit countChanged();
    }
}

/*!
    \qmlproperty component ListView::delegate

    The delegate provides a template describing what each item in the view should look and act like.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/listview/listview.qml 0
*/
QmlComponent *QFxListView::delegate() const
{
    Q_D(const QFxListView);
    return d->model ? d->model->delegate() : 0;
}

void QFxListView::setDelegate(QmlComponent *delegate)
{
    Q_D(QFxListView);
    if (!d->ownModel) {
        d->model = new QFxVisualItemModel(qmlContext(this));
        d->ownModel = true;
    }
    d->model->setDelegate(delegate);
    d->updateCurrent(d->currentIndex);
    refill();
}

/*!
    \qmlproperty int ListView::currentIndex
    \qmlproperty Item ListView::current

    \c currentIndex holds the index of the current item.
    \c current is the current item.  Note that the position of the current item
    may only be approximate until it becomes visible in the view.
*/
int QFxListView::currentIndex() const
{
    Q_D(const QFxListView);
    return d->currentIndex;
}

void QFxListView::setCurrentIndex(int index)
{
    Q_D(QFxListView);
    d->moveReason = QFxListViewPrivate::Other;
    if (d->isValid() && index != d->currentIndex && index < d->model->count() && index >= 0)
        d->updateCurrent(index);
    else
        d->currentIndex = index;
}

QFxItem *QFxListView::currentItem()
{
    Q_D(QFxListView);
    if (!d->currentItem) {
        // Always return something valid
        if (!d->tmpCurrent) {
            d->tmpCurrent = new QFxItem;
            d->tmpCurrent->setParent(viewport());
        }
        return d->tmpCurrent;
    }
    return d->currentItem->item;
}

/*!
  \qmlproperty int ListView::count
  This property holds the number of items in the view.
*/
int QFxListView::count() const
{
    Q_D(const QFxListView);
    if (d->model)
        return d->model->count();
    return 0;
}

/*!
    \qmlproperty component ListView::highlight
    This property holds the component to use as the highlight.

    An instance of the highlight component will be created for each list.
    The geometry of the resultant component instance will be managed by the list
    so as to stay with the current item, unless the autoHighlight property is false.

    The below example demonstrates how to make a simple highlight
    for a vertical list.

    \snippet doc/src/snippets/declarative/listview/listview.qml 1
    \image trivialListView.png

    \sa autoHighlight
*/
QmlComponent *QFxListView::highlight() const
{
    Q_D(const QFxListView);
    return d->highlightComponent;
}

void QFxListView::setHighlight(QmlComponent *highlight)
{
    Q_D(QFxListView);
    delete d->highlightComponent;
    d->highlightComponent = highlight;
    d->updateCurrent(d->currentIndex);
}

/*!
    \qmlproperty bool ListView::autoHighlight
    This property holds whether the highlight is managed by the view.

    If autoHighlight is true, the highlight will be moved smoothly
    to follow the current item.  If autoHighlight is false, the
    highlight will not be moved by the view, and must be implemented
    by the highlight.  The following example creates a highlight with
    its motion defined by the spring \l {Follow}:

    \snippet doc/src/snippets/declarative/listview/highlight.qml 1

    \sa highlight
*/
bool QFxListView::autoHighlight() const
{
    Q_D(const QFxListView);
    return d->autoHighlight;
}

void QFxListView::setAutoHighlight(bool autoHighlight)
{
    Q_D(QFxListView);
    d->autoHighlight = autoHighlight;
    if (d->highlightPosAnimator) {
        d->highlightPosAnimator->setEnabled(d->autoHighlight);
        d->highlightSizeAnimator->setEnabled(d->autoHighlight);
    }
    d->updateHighlight();
}

/*!
    \qmlproperty enumeration ListView::currentItemPositioning
    This property determines the current item positioning and selection characteristics.

    The modes supported are:
    \list
    \i Free - For Mouse, the current item may be positioned anywhere,
    whether within the visible area, or outside.  During Keyboard interaction,
    the current item can move within the visible area, and the view will
    scroll to keep the highlight visible.
    \i Snap - For mouse, the current item may be positioned anywhere,
    whether within the visible area, or outside.  During keyboard interaction,
    the current item will be kept in the visible area and will prefer to be
    positioned at the \l snapPosition, however the view will never scroll
    beyond the beginning or end of the view.
    \i SnapAuto - For both mouse and keyboard, the current item will be
    kept at the \l {snapPosition}. Additionally, if the view is dragged or
    flicked, the current item will be automatically updated to be the item
    currently at the snapPosition.
    \endlist
*/
QFxListView::CurrentItemPositioning QFxListView::currentItemPositioning() const
{
    Q_D(const QFxListView);
    return d->currItemMode;
}

void QFxListView::setCurrentItemPositioning(CurrentItemPositioning mode)
{
    Q_D(QFxListView);
    d->currItemMode = mode;
}

/*!
    \qmlproperty int ListView::snapPosition

    When currentItemPositioning is set to Snap or SnapAuto, the
    \c snapPosition determines where the top of the items will
    snap to.
*/
int QFxListView::snapPosition() const
{
    Q_D(const QFxListView);
    return d->snapPos;
}

void QFxListView::setSnapPosition(int pos)
{
    Q_D(QFxListView);
    d->snapPos = pos;
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
Qt::Orientation QFxListView::orientation() const
{
    Q_D(const QFxListView);
    return d->orient;
}

void QFxListView::setOrientation(Qt::Orientation orientation)
{
    Q_D(QFxListView);
    if (d->orient != orientation) {
        d->orient = orientation;
        if (d->orient == Qt::Vertical)
            setViewportWidth(-1);
        else
            setViewportHeight(-1);
        d->clear();
        refill();
        d->updateCurrent(d->currentIndex);
    }
}

/*!
    \qmlproperty bool ListView::wrap
    This property holds whether the list wraps key navigation

    If this property is true then key presses to move off of one end of the list will cause the
    selection to jump to the other side.
*/
bool QFxListView::isWrapEnabled() const
{
    Q_D(const QFxListView);
    return d->wrap;
}

void QFxListView::setWrapEnabled(bool wrap)
{
    Q_D(QFxListView);
    d->wrap = wrap;
}

/*!
    \qmlproperty int ListView::cacheBuffer
    This property holds the number of off-screen pixels to cache.

    This property determines the number of pixels above the top of the list
    and below the bottom of the list to cache.  Setting this value can make
    scrolling the list smoother at the expense of additional memory usage.
*/

/*!
    \property QFxListView::cacheBuffer
    \brief sets the number of off-screen pixels to cache.

    This property determines the number of pixels above the top of the list
    and below the bottom of the list to cache.  Setting this value can make
    scrolling the list smoother at the expense of additional memory usage.
*/
int QFxListView::cacheBuffer() const
{
    Q_D(const QFxListView);
    return d->buffer;
}

void QFxListView::setCacheBuffer(int b)
{
    Q_D(QFxListView);
    if (d->buffer != b) {
        d->buffer = b;
        if (isComponentComplete())
            refill();
    }
}

/*!
    \qmlproperty string ListView::sectionExpression
    This property holds the expression to be evaluated for the section attached property.

    Each item in the list has attached properties named \c ListView.section and
    \c ListView.prevSection.  These may be used to place a section header for
    related items.  The example below assumes that the model is sorted by size of
    pet.  The section expression is the size property.  If \c ListView.section and
    \c ListView.prevSection differ, the item will display a section header.

    \snippet examples/declarative/listview/sections.qml 0

    \image ListViewSections.png
*/
QString QFxListView::sectionExpression() const
{
    Q_D(const QFxListView);
    return d->sectionExpression;
}

void QFxListView::setSectionExpression(const QString &expression)
{
    Q_D(QFxListView);
    if (d->sectionExpression != expression) {
        d->sectionExpression = expression;
        emit sectionExpressionChanged();
    }
}

QString QFxListView::currentSection() const
{
    Q_D(const QFxListView);
    return d->currentSection;
}

void QFxListView::viewportMoved()
{
    Q_D(QFxListView);
    QFxFlickable::viewportMoved();
    refill();
    if (isFlicking() || d->pressed)
        d->moveReason = QFxListViewPrivate::Mouse;
    if (d->currItemMode == SnapAuto && d->moveReason == QFxListViewPrivate::Mouse) {
        // Update current index
        int idx = d->snapIndex();
        if (idx >= 0 && idx != d->currentIndex)
            d->updateCurrent(idx);
    }
}

/*!
    \reimp
*/
qreal QFxListView::minYExtent() const
{
    Q_D(const QFxListView);
    if (d->orient == Qt::Horizontal)
        return QFxFlickable::minYExtent();
    qreal extent = -d->startPosition();
    if (d->currItemMode == SnapAuto)
        extent += d->snapPos;

    return extent;
}

/*!
    \reimp
*/
qreal QFxListView::maxYExtent() const
{
    Q_D(const QFxListView);
    if (d->orient == Qt::Horizontal)
        return QFxFlickable::maxYExtent();
    qreal extent;
    if (d->currItemMode == SnapAuto)
        extent = -(d->positionAt(count()-1) - d->snapPos);
    else
        extent = -(d->endPosition() - height());
    if (extent > 0)
        extent = 0;
    return extent;
}

/*!
    \reimp
*/
qreal QFxListView::minXExtent() const
{
    Q_D(const QFxListView);
    if (d->orient == Qt::Vertical)
        return QFxFlickable::minXExtent();
    qreal extent = -d->startPosition();
    if (d->currItemMode == SnapAuto)
        extent += d->snapPos;

    return extent;
}

/*!
    \reimp
*/
qreal QFxListView::maxXExtent() const
{
    Q_D(const QFxListView);
    if (d->orient == Qt::Vertical)
        return QFxFlickable::maxXExtent();
    qreal extent;
    if (d->currItemMode == SnapAuto)
        extent = -(d->positionAt(count()-1) - d->snapPos);
    else
        extent = -(d->endPosition() - width());
    if (extent > 0)
        extent = 0;
    return extent;
}

void QFxListView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QFxListView);
    if (d->model && d->model->count() && !d->locked) {
        if ((d->orient == Qt::Horizontal && event->key() == Qt::Key_Left)
                    || (d->orient == Qt::Vertical && event->key() == Qt::Key_Up)) {
            if (currentIndex() > 0 || d->wrap) {
                d->moveReason = QFxListViewPrivate::Key;
                int index = currentIndex()-1;
                d->updateCurrent(index >= 0 ? index : d->model->count()-1);
                event->accept();
            }
            return;
        } else if ((d->orient == Qt::Horizontal && event->key() == Qt::Key_Right)
                    || (d->orient == Qt::Vertical && event->key() == Qt::Key_Down)) {
            if (currentIndex() < d->model->count() - 1 || d->wrap) {
                d->moveReason = QFxListViewPrivate::Key;
                int index = currentIndex()+1;
                d->updateCurrent(index < d->model->count() ? index : 0);
                event->accept();
            }
            return;
        }
    }
    d->moveReason = QFxListViewPrivate::Other;
    QFxFlickable::keyPressEvent(event);
}

void QFxListView::componentComplete()
{
    Q_D(QFxListView);
    QFxFlickable::componentComplete();
    if (d->currentIndex < 0)
        d->updateCurrent(0);
    refill();
    d->fixupPosition();
}

void QFxListView::refill()
{
    Q_D(QFxListView);
    d->refill(d->position(), d->position()+d->size()-1);
}

void QFxListView::trackedPositionChanged()
{
    Q_D(QFxListView);
    if (!d->trackedItem)
        return;
    if (!isFlicking() && !d->pressed && d->moveReason != QFxListViewPrivate::Mouse) {
        switch (d->currItemMode) {
        case Free:
            if (d->trackedItem->position() < d->position()) {
                d->setPosition(d->trackedItem->position());
            } else if (d->trackedItem->endPosition() > d->position() + d->size()) {
                qreal pos = d->trackedItem->endPosition() - d->size();
                if (d->trackedItem->size() > d->size())
                    pos = d->trackedItem->position();
                d->setPosition(pos);
            }
            d->fixupPosition();
            break;
        case Snap:
            if (d->trackedItem->position() < d->startPosition() + d->snapPos)
                d->setPosition(d->startPosition());
            else if (d->trackedItem->endPosition() > d->endPosition() - d->size() + d->snapPos + d->trackedItem->size())
                d->setPosition(d->endPosition() - d->size());
            else
                d->setPosition(d->trackedItem->position() - d->snapPos);
            break;
        case SnapAuto:
            d->setPosition(d->trackedItem->position() - d->snapPos);
            break;
        }
    } else if (d->fixCurrentVisibility && d->currentItem && !d->pressed) {
        /*
        if (d->trackedItem->position() < d->position()
                && d->currentItem->position() < d->position()) {
            d->setPosition(d->trackedItem->position());
        } else if (d->size() && d->trackedItem->size() <= d->size()
                && d->trackedItem->endPosition() > d->position() + d->size()
                && d->currentItem->endPosition() > d->position() + d->size()) {
            d->setPosition(d->trackedItem->endPosition() - d->size());
        }
        if (d->trackedItem->position() == d->currentItem->position())
            d->fixCurrentVisibility = false;
            */
    }
}

void QFxListView::itemResized()
{
    Q_D(QFxListView);
    QFxItem *item = qobject_cast<QFxItem*>(sender());
    if (item) {
        d->activeItem = item; // Ick - don't delete the sender
        d->layout();
        d->activeItem = 0;
        d->fixupPosition();
    }
}

void QFxListView::itemsInserted(int modelIndex, int count)
{
    Q_D(QFxListView);
    d->updateUnrequestedIndexes();
    if (!d->visibleItems.count() || d->model->count() <= 1) {
        d->layout();
        d->updateCurrent(qMax(0, qMin(d->currentIndex, d->model->count()-1)));
        emit countChanged();
        return;
    }

    if (!d->mapRangeFromModel(modelIndex, count)) {
        int i = d->visibleItems.count() - 1;
        while (i > 0 && d->visibleItems.at(i)->index == -1)
            --i;
        if (d->visibleItems.at(i)->index + 1 == modelIndex) {
            // Special case of appending an item to the model.
            modelIndex = d->visibleIndex + d->visibleItems.count();
        } else {
            if (modelIndex + count - 1 < d->visibleIndex) {
                // Insert before visible items
                d->visibleIndex += count;
                for (int i = 0; i < d->visibleItems.count(); ++i) {
                    FxListItem *listItem = d->visibleItems.at(i);
                    if (listItem->index != -1)
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
    int to = d->buffer+d->position()+d->size()-1;
    // index can be the next item past the end of the visible items list (i.e. appended)
    int pos = index < d->visibleItems.count() ? d->visibleItems.at(index)->position()
                                                : d->visibleItems.at(index-1)->endPosition()+1;
    int initialPos = pos;
    QList<FxListItem*> added;
    for (int i = 0; i < count && pos <= to; ++i) {
        FxListItem *item = d->createItem(modelIndex + i);
        d->visibleItems.insert(index, item);
        item->setPosition(pos);
        added.append(item);
        pos += item->size();
        ++index;
    }
    if (d->currentIndex >= modelIndex) {
        // adjust current item index
        d->currentIndex += count;
        if (d->currentItem) {
            d->currentItem->index = d->currentIndex;
            d->currentItem->setPosition(d->currentItem->position() + (pos - initialPos));
        }
    }
    if (pos > to) {
        // We didn't insert all our new items, which means anything
        // beyond the current index is not visible - remove it.
        while (d->visibleItems.count() > index)
            d->releaseItem(d->visibleItems.takeLast());
    } else {
        // Update the indexes of the following visible items.
        for (; index < d->visibleItems.count(); ++index) {
            FxListItem *listItem = d->visibleItems.at(index);
            listItem->setPosition(listItem->position() + (pos - initialPos));
            if (listItem->index != -1)
                listItem->index += count;
        }
    }
    // everything is in order now - emit add() signal
    for (int j = 0; j < added.count(); ++j)
        added.at(j)->attached->emitAdd();
    d->updateUnrequestedPositions();
    emit countChanged();
}

void QFxListView::itemsRemoved(int modelIndex, int count)
{
    Q_D(QFxListView);
    d->updateUnrequestedIndexes();
    if (!d->mapRangeFromModel(modelIndex, count)) {
        if (modelIndex + count - 1 < d->visibleIndex) {
            // Items removed before our visible items.
            d->visibleIndex -= count;
            for (int i = 0; i < d->visibleItems.count(); ++i) {
                FxListItem *listItem = d->visibleItems.at(i);
                if (listItem->index != -1)
                    listItem->index -= count;
            }
        }
        if (d->currentIndex >= modelIndex + count) {
            d->currentIndex -= count;
            if (d->currentItem)
                d->currentItem->index -= count;
        } else if (d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count) {
            // current item has been removed.
            d->releaseItem(d->currentItem);
            d->currentItem = 0;
            d->currentIndex = -1;
            d->updateCurrent(qMin(modelIndex, d->model->count()-1));
        }
        d->layout();
        emit countChanged();
        return;
    }

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
    } else if (d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count) {
        // current item has been removed.
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

    if (d->visibleItems.isEmpty()) {
        d->visibleIndex = 0;
        d->visiblePos = 0;
        d->_tl.clear();
        d->setPosition(0);
        if (d->model->count() == 0)
            update();
        else
            refill();
    } else {
        // Correct the positioning of the items
        d->layout();
    }

    emit countChanged();
}

void QFxListView::destroyRemoved()
{
    Q_D(QFxListView);
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

void QFxListView::createdItem(int index, QFxItem *item)
{
    Q_D(QFxListView);
    if (d->requestedIndex != index) {
        item->setParentItem(viewport());
        d->unrequestedItems.insert(item, index);
        if (d->orient == Qt::Vertical)
            item->setY(d->positionAt(index));
        else
            item->setX(d->positionAt(index));
    }
}

void QFxListView::destroyingItem(QFxItem *item)
{
    Q_D(QFxListView);
    d->unrequestedItems.remove(item);
}

QFxListViewAttached *QFxListView::qmlAttachedProperties(QObject *obj)
{
    return QFxListViewAttached::properties(obj);
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ListView,QFxListView)

QT_END_NAMESPACE
#include "qfxlistview.moc"
