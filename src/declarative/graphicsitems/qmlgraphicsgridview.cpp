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

#include "qmlgraphicsgridview_p.h"

#include "qmlgraphicsvisualitemmodel_p.h"
#include "qmlgraphicsflickable_p_p.h"

#include <qmleasefollow_p.h>

#include <qlistmodelinterface_p.h>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE

class QmlGraphicsGridViewAttached : public QObject
{
    Q_OBJECT
public:
    QmlGraphicsGridViewAttached(QObject *parent)
        : QObject(parent), m_isCurrent(false), m_delayRemove(false) {}
    ~QmlGraphicsGridViewAttached() {
        attachedProperties.remove(parent());
    }

    Q_PROPERTY(QmlGraphicsGridView *view READ view CONSTANT)
    QmlGraphicsGridView *view() { return m_view; }

    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
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

    static QmlGraphicsGridViewAttached *properties(QObject *obj) {
        QmlGraphicsGridViewAttached *rv = attachedProperties.value(obj);
        if (!rv) {
            rv = new QmlGraphicsGridViewAttached(obj);
            attachedProperties.insert(obj, rv);
        }
        return rv;
    }

    void emitAdd() { emit add(); }
    void emitRemove() { emit remove(); }

Q_SIGNALS:
    void currentItemChanged();
    void delayRemoveChanged();
    void add();
    void remove();

public:
    QmlGraphicsGridView *m_view;
    bool m_isCurrent;
    bool m_delayRemove;

    static QHash<QObject*, QmlGraphicsGridViewAttached*> attachedProperties;
};

QHash<QObject*, QmlGraphicsGridViewAttached*> QmlGraphicsGridViewAttached::attachedProperties;


//----------------------------------------------------------------------------

class FxGridItem
{
public:
    FxGridItem(QmlGraphicsItem *i, QmlGraphicsGridView *v) : item(i), view(v) {
        attached = QmlGraphicsGridViewAttached::properties(item);
        attached->m_view = view;
    }
    ~FxGridItem() {}

    qreal rowPos() const { return (view->flow() == QmlGraphicsGridView::LeftToRight ? item->y() : item->x()); }
    qreal colPos() const { return (view->flow() == QmlGraphicsGridView::LeftToRight ? item->x() : item->y()); }
    qreal endRowPos() const {
        return view->flow() == QmlGraphicsGridView::LeftToRight
                                ? item->y() + view->cellHeight() - 1
                                : item->x() + view->cellWidth() - 1;
    }
    void setPosition(qreal col, qreal row) {
        if (view->flow() == QmlGraphicsGridView::LeftToRight) {
            item->setPos(QPointF(col, row));
        } else {
            item->setPos(QPointF(row, col));
        }
    }

    QmlGraphicsItem *item;
    QmlGraphicsGridView *view;
    QmlGraphicsGridViewAttached *attached;
    int index;
};

//----------------------------------------------------------------------------

class QmlGraphicsGridViewPrivate : public QmlGraphicsFlickablePrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsGridView)

public:
    QmlGraphicsGridViewPrivate()
    : currentItem(0), flow(QmlGraphicsGridView::LeftToRight)
    , visiblePos(0), visibleIndex(0) , currentIndex(-1)
    , cellWidth(100), cellHeight(100), columns(1), requestedIndex(-1)
    , highlightComponent(0), highlight(0), trackedItem(0)
    , moveReason(Other), buffer(0), highlightXAnimator(0), highlightYAnimator(0)
    , bufferMode(NoBuffer)
    , ownModel(false), wrap(false), autoHighlight(true)
    , fixCurrentVisibility(false), lazyRelease(false), layoutScheduled(false)
    , deferredRelease(false) {}

    void init();
    void clear();
    FxGridItem *createItem(int modelIndex);
    void releaseItem(FxGridItem *item);
    void refill(qreal from, qreal to, bool doBuffer=false);

    void updateGrid();
    void scheduleLayout();
    void layout(bool removed=false);
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateTrackedItem();
    void createHighlight();
    void updateHighlight();
    void updateCurrent(int modelIndex);

    FxGridItem *visibleItem(int modelIndex) const {
        if (modelIndex >= visibleIndex && modelIndex < visibleIndex + visibleItems.count()) {
            for (int i = modelIndex - visibleIndex; i < visibleItems.count(); ++i) {
                FxGridItem *item = visibleItems.at(i);
                if (item->index == modelIndex)
                    return item;
            }
        }
        return 0;
    }

    qreal position() const {
        Q_Q(const QmlGraphicsGridView);
        return flow == QmlGraphicsGridView::LeftToRight ? q->viewportY() : q->viewportX();
    }
    void setPosition(qreal pos) {
        Q_Q(QmlGraphicsGridView);
        if (flow == QmlGraphicsGridView::LeftToRight)
            q->setViewportY(pos);
        else
            q->setViewportX(pos);
    }
    int size() const {
        Q_Q(const QmlGraphicsGridView);
        return flow == QmlGraphicsGridView::LeftToRight ? q->height() : q->width();
    }
    qreal startPosition() const {
        qreal pos = 0;
        if (!visibleItems.isEmpty())
            pos = visibleItems.first()->rowPos() - visibleIndex / columns * rowSize();
        return pos;
    }

    qreal endPosition() const {
        qreal pos = 0;
        if (model && model->count())
            pos = rowPosAt(model->count() - 1) + rowSize();
        return pos;
    }

    bool isValid() const {
        return model && model->count() && model->isValid();
    }

    int rowSize() const {
        return flow == QmlGraphicsGridView::LeftToRight ? cellHeight : cellWidth;
    }
    int colSize() const {
        return flow == QmlGraphicsGridView::LeftToRight ? cellWidth : cellHeight;
    }

    qreal colPosAt(int modelIndex) const {
        if (FxGridItem *item = visibleItem(modelIndex))
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
        if (FxGridItem *item = visibleItem(modelIndex))
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
             return (modelIndex / columns) * rowSize();
        }
        return 0;
    }

    FxGridItem *firstVisibleItem() const {
        const qreal pos = position();
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItem *item = visibleItems.at(i);
            if (item->index != -1 && item->endRowPos() > pos)
                return item;
        }
        return visibleItems.count() ? visibleItems.first() : 0;
    }

    // Map a model index to visibleItems list index.
    // These may differ if removed items are still present in the visible list,
    // e.g. doing a removal animation
    int mapFromModel(int modelIndex) const {
        if (modelIndex < visibleIndex || modelIndex >= visibleIndex + visibleItems.count())
            return -1;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItem *listItem = visibleItems.at(i);
            if (listItem->index == modelIndex)
                return i + visibleIndex;
            if (listItem->index > modelIndex)
                return -1;
        }
        return -1; // Not in visibleList
    }

    // for debugging only
    void checkVisible() const {
        int skip = 0;
        for (int i = 0; i < visibleItems.count(); ++i) {
            FxGridItem *listItem = visibleItems.at(i);
            if (listItem->index == -1) {
                ++skip;
            } else if (listItem->index != visibleIndex + i - skip) {
                for (int j = 0; j < visibleItems.count(); j++)
                    qDebug() << " index" << j << "item index" << visibleItems.at(j)->index;
                qFatal("index %d %d %d", visibleIndex, i, listItem->index);
            }
        }
    }

    QGuard<QmlGraphicsVisualModel> model;
    QVariant modelVariant;
    QList<FxGridItem*> visibleItems;
    QHash<QmlGraphicsItem*,int> unrequestedItems;
    FxGridItem *currentItem;
    QmlGraphicsGridView::Flow flow;
    int visiblePos;
    int visibleIndex;
    int currentIndex;
    int cellWidth;
    int cellHeight;
    int columns;
    int requestedIndex;
    QmlComponent *highlightComponent;
    FxGridItem *highlight;
    FxGridItem *trackedItem;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    int buffer;
    QmlEaseFollow *highlightXAnimator;
    QmlEaseFollow *highlightYAnimator;
    enum BufferMode { NoBuffer = 0x00, BufferBefore = 0x01, BufferAfter = 0x02 };
    BufferMode bufferMode;

    bool ownModel : 1;
    bool wrap : 1;
    bool autoHighlight : 1;
    bool fixCurrentVisibility : 1;
    bool lazyRelease : 1;
    bool layoutScheduled : 1;
    bool deferredRelease : 1;
};

void QmlGraphicsGridViewPrivate::init()
{
    Q_Q(QmlGraphicsGridView);
    q->setFlag(QGraphicsItem::ItemIsFocusScope);
    QObject::connect(q, SIGNAL(widthChanged()), q, SLOT(sizeChange()));
    QObject::connect(q, SIGNAL(heightChanged()), q, SLOT(sizeChange()));
    q->setFlickDirection(QmlGraphicsFlickable::VerticalFlick);
}

void QmlGraphicsGridViewPrivate::clear()
{
    for (int i = 0; i < visibleItems.count(); ++i)
        releaseItem(visibleItems.at(i));
    visibleItems.clear();
    visiblePos = 0;
    visibleIndex = 0;
    releaseItem(currentItem);
    currentItem = 0;
    createHighlight();
    trackedItem = 0;
}

FxGridItem *QmlGraphicsGridViewPrivate::createItem(int modelIndex)
{
    Q_Q(QmlGraphicsGridView);
    // create object
    requestedIndex = modelIndex;
    FxGridItem *listItem = 0;
    if (QmlGraphicsItem *item = model->item(modelIndex, false)) {
        listItem = new FxGridItem(item, q);
        listItem->index = modelIndex;
        // complete
        model->completeItem();
        listItem->item->setZValue(1);
        listItem->item->setParent(q->viewport());
        unrequestedItems.remove(listItem->item);
    }
    requestedIndex = 0;
    return listItem;
}


void QmlGraphicsGridViewPrivate::releaseItem(FxGridItem *item)
{
    Q_Q(QmlGraphicsGridView);
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

void QmlGraphicsGridViewPrivate::refill(qreal from, qreal to, bool doBuffer)
{
    Q_Q(QmlGraphicsGridView);
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

    FxGridItem *item = 0;

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
                    && item->endRowPos() < bufferFrom - rowSize()*(item->colPos()/colSize()+1)/(columns+1)) {
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
        if (flow == QmlGraphicsGridView::LeftToRight)
            q->setViewportHeight(endPosition() - startPosition());
        else
            q->setViewportWidth(endPosition() - startPosition());
    } else if (!doBuffer && buffer && bufferMode != NoBuffer) {
        refill(from, to, true);
    }
    lazyRelease = false;
}

void QmlGraphicsGridViewPrivate::updateGrid()
{
    Q_Q(QmlGraphicsGridView);
    columns = (int)qMax((flow == QmlGraphicsGridView::LeftToRight ? q->width() : q->height()) / colSize(), qreal(1.));
    if (isValid()) {
        if (flow == QmlGraphicsGridView::LeftToRight)
            q->setViewportHeight(endPosition() - startPosition());
        else
            q->setViewportWidth(endPosition() - startPosition());
    }
}

void QmlGraphicsGridViewPrivate::scheduleLayout()
{
    Q_Q(QmlGraphicsGridView);
    if (!layoutScheduled) {
        layoutScheduled = true;
        QMetaObject::invokeMethod(q, "layout", Qt::QueuedConnection);
    }
}

void QmlGraphicsGridViewPrivate::layout(bool removed)
{
    Q_Q(QmlGraphicsGridView);
    layoutScheduled = false;
    if (visibleItems.count()) {
        qreal rowPos = visibleItems.first()->rowPos();
        qreal colPos = visibleItems.first()->colPos();
        int col = visibleIndex % columns;
        if (colPos != col * colSize()) {
            if (removed)
                rowPos -= rowSize();
            colPos = col * colSize();
            visibleItems.first()->setPosition(colPos, rowPos);
        }
        for (int i = 1; i < visibleItems.count(); ++i) {
            FxGridItem *item = visibleItems.at(i);
            colPos += colSize();
            if (colPos > colSize() * (columns-1)) {
                colPos = 0;
                rowPos += rowSize();
            }
            item->setPosition(colPos, rowPos);
        }
    }
    q->refill();
    updateHighlight();
    moveReason = Other;
    if (flow == QmlGraphicsGridView::LeftToRight) {
        q->setViewportHeight(endPosition() - startPosition());
        fixupY();
    } else {
        q->setViewportWidth(endPosition() - startPosition());
        fixupX();
    }
    updateUnrequestedPositions();
}

void QmlGraphicsGridViewPrivate::updateUnrequestedIndexes()
{
    Q_Q(QmlGraphicsGridView);
    QHash<QmlGraphicsItem*,int>::iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it)
        *it = model->indexOf(it.key(), q);
}

void QmlGraphicsGridViewPrivate::updateUnrequestedPositions()
{
    QHash<QmlGraphicsItem*,int>::const_iterator it;
    for (it = unrequestedItems.begin(); it != unrequestedItems.end(); ++it) {
        if (flow == QmlGraphicsGridView::LeftToRight) {
            it.key()->setPos(QPointF(colPosAt(*it), rowPosAt(*it)));
        } else {
            it.key()->setPos(QPointF(rowPosAt(*it), colPosAt(*it)));
        }
    }
}

void QmlGraphicsGridViewPrivate::updateTrackedItem()
{
    Q_Q(QmlGraphicsGridView);
    FxGridItem *item = currentItem;
    if (highlight)
        item = highlight;

    FxGridItem *oldTracked = trackedItem;

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
    if (trackedItem && trackedItem != oldTracked)
        q->trackedPositionChanged();
}

void QmlGraphicsGridViewPrivate::createHighlight()
{
    Q_Q(QmlGraphicsGridView);
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
            item->setParent(q->viewport());
        }
        if (item) {
            item->setParent(q->viewport());
            highlight = new FxGridItem(item, q);
            highlightXAnimator = new QmlEaseFollow(q);
            highlightXAnimator->setTarget(QmlMetaProperty(highlight->item, QLatin1String("x")));
            highlightXAnimator->setDuration(150);
            highlightXAnimator->setEnabled(autoHighlight);
            highlightYAnimator = new QmlEaseFollow(q);
            highlightYAnimator->setTarget(QmlMetaProperty(highlight->item, QLatin1String("y")));
            highlightYAnimator->setDuration(150);
            highlightYAnimator->setEnabled(autoHighlight);
            changed = true;
        }
    }
    if (changed)
        emit q->highlightChanged();
}

void QmlGraphicsGridViewPrivate::updateHighlight()
{
    if ((!currentItem && highlight) || (currentItem && !highlight))
        createHighlight();
    if (currentItem && autoHighlight && highlight && !moving) {
        // auto-update highlight
        highlightXAnimator->setSourceValue(currentItem->item->x());
        highlightYAnimator->setSourceValue(currentItem->item->y());
        highlight->item->setWidth(currentItem->item->width());
        highlight->item->setHeight(currentItem->item->height());
    }
    updateTrackedItem();
}

void QmlGraphicsGridViewPrivate::updateCurrent(int modelIndex)
{
    Q_Q(QmlGraphicsGridView);
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

    FxGridItem *oldCurrentItem = currentItem;
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

//----------------------------------------------------------------------------

/*!
    \qmlclass GridView QmlGraphicsGridView
    \inherits Flickable
    \brief The GridView item provides a grid view of items provided by a model.

    The model is typically provided by a QAbstractListModel "C++ model object",
    but can also be created directly in QML.

    The items are laid out top to bottom (vertically) or left to right (horizontally)
    and may be flicked to scroll.

    The below example creates a very simple grid, using a QML model.

    \image gridview.png

    \snippet doc/src/snippets/declarative/gridview/gridview.qml 3

    The model is defined as a ListModel using QML:
    \quotefile doc/src/snippets/declarative/gridview/dummydata/ContactModel.qml

    In this case ListModel is a handy way for us to test our UI.  In practice
    the model would be implemented in C++, or perhaps via a SQL data source.
*/
QmlGraphicsGridView::QmlGraphicsGridView(QmlGraphicsItem *parent)
    : QmlGraphicsFlickable(*(new QmlGraphicsGridViewPrivate), parent)
{
    Q_D(QmlGraphicsGridView);
    d->init();
}

QmlGraphicsGridView::~QmlGraphicsGridView()
{
    Q_D(QmlGraphicsGridView);
    d->clear();
    if (d->ownModel)
        delete d->model;
}

/*!
    \qmlattachedproperty bool GridView::isCurrentItem
    This attched property is true if this delegate is the current item; otherwise false.

    It is attached to each instance of the delegate.
*/

/*!
    \qmlattachedproperty GridView GridView::view
    This attached property holds the view that manages this delegate instance.

    It is attached to each instance of the delegate.
*/

/*!
    \qmlattachedproperty bool GridView::delayRemove
    This attached property holds whether the delegate may be destroyed.

    It is attached to each instance of the delegate.

    It is sometimes necessary to delay the destruction of an item
    until an animation completes.

    The example below ensures that the animation completes before
    the item is removed from the grid.

    \code
    Component {
        id: myDelegate
        Item {
            id: wrapper
            GridView.onRemove: SequentialAnimation {
                PropertyAction { target: wrapper.GridView; property: "delayRemove"; value: true }
                NumberAnimation { target: wrapper; property: "scale"; to: 0; duration: 250; easing: "easeInOutQuad" }
                PropertyAction { target: wrapper.GridView; property: "delayRemove"; value: false }
            }
        }
    }
    \endcode
*/

/*!
    \qmlattachedsignal GridView::onAdd()
    This attached handler is called immediately after an item is added to the view.
*/

/*!
    \qmlattachedsignal GridView::onRemove()
    This attached handler is called immediately before an item is removed from the view.
*/


/*!
  \qmlproperty model GridView::model
  This property holds the model providing data for the grid.

  The model provides a set of data that is used to create the items
  for the view.  For large or dynamic datasets the model is usually
  provided by a C++ model object.  The C++ model object must be a \l
  {QAbstractItemModel} subclass, a VisualModel, or a simple list.

  \sa {qmlmodels}{Data Models}
*/
QVariant QmlGraphicsGridView::model() const
{
    Q_D(const QmlGraphicsGridView);
    return d->modelVariant;
}

void QmlGraphicsGridView::setModel(const QVariant &model)
{
    Q_D(QmlGraphicsGridView);
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
                d->moveReason = QmlGraphicsGridViewPrivate::SetIndex;
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
    \qmlproperty component GridView::delegate

    The delegate provides a template defining each item instantiated by the view.
    The index is exposed as an accessible \c index property.  Properties of the
    model are also available depending upon the type of \l {qmlmodels}{Data Model}.

    Note that the GridView will layout the items based on the size of the root item
    in the delegate.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/gridview/gridview.qml 0
*/
QmlComponent *QmlGraphicsGridView::delegate() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->model) {
        if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QmlGraphicsGridView::setDelegate(QmlComponent *delegate)
{
    Q_D(QmlGraphicsGridView);
    if (delegate == this->delegate())
        return;

    if (!d->ownModel) {
        d->model = new QmlGraphicsVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QmlGraphicsVisualDataModel *dataModel = qobject_cast<QmlGraphicsVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        if (isComponentComplete()) {
            refill();
            d->moveReason = QmlGraphicsGridViewPrivate::SetIndex;
            d->updateCurrent(d->currentIndex);
        }
    }
}

/*!
  \qmlproperty int GridView::currentIndex
  \qmlproperty Item GridView::currentItem

  \c currentIndex holds the index of the current item.
  \c currentItem is the current item.  Note that the position of the current item
  may only be approximate until it becomes visible in the view.
*/
int QmlGraphicsGridView::currentIndex() const
{
    Q_D(const QmlGraphicsGridView);
    return d->currentIndex;
}

void QmlGraphicsGridView::setCurrentIndex(int index)
{
    Q_D(QmlGraphicsGridView);
    if (isComponentComplete() && d->isValid() && index != d->currentIndex && index < d->model->count() && index >= 0) {
        d->moveReason = QmlGraphicsGridViewPrivate::SetIndex;
        cancelFlick();
        d->updateCurrent(index);
    } else {
        d->currentIndex = index;
    }
}

QmlGraphicsItem *QmlGraphicsGridView::currentItem()
{
    Q_D(QmlGraphicsGridView);
    if (!d->currentItem)
        return 0;
    return d->currentItem->item;
}

/*!
  \qmlproperty Item GridView::highlightItem

  \c highlightItem holds the highlight item, which was created
  from the \l highlight component.

  The highlightItem is managed by the view unless
  \l highlightFollowsCurrentItem is set to false.

  \sa highlight, highlightFollowsCurrentItem
*/
QmlGraphicsItem *QmlGraphicsGridView::highlightItem()
{
    Q_D(QmlGraphicsGridView);
    if (!d->highlight)
        return 0;
    return d->highlight->item;
}

/*!
  \qmlproperty int GridView::count
  This property holds the number of items in the view.
*/
int QmlGraphicsGridView::count() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->model)
        return d->model->count();
    return 0;
}

/*!
  \qmlproperty component GridView::highlight
  This property holds the component to use as the highlight.

  An instance of the highlight component will be created for each view.
  The geometry of the resultant component instance will be managed by the view
  so as to stay with the current item, unless the highlightFollowsCurrentItem property is false.

  The below example demonstrates how to make a simple highlight:
  \snippet doc/src/snippets/declarative/gridview/gridview.qml 1

  \sa highlightItem, highlightFollowsCurrentItem
*/
QmlComponent *QmlGraphicsGridView::highlight() const
{
    Q_D(const QmlGraphicsGridView);
    return d->highlightComponent;
}

void QmlGraphicsGridView::setHighlight(QmlComponent *highlight)
{
    Q_D(QmlGraphicsGridView);
    if (highlight != d->highlightComponent) {
        d->highlightComponent = highlight;
        d->updateCurrent(d->currentIndex);
    }
}

/*!
  \qmlproperty bool GridView::highlightFollowsCurrentItem
  This property sets whether the highlight is managed by the view.

  If highlightFollowsCurrentItem is true, the highlight will be moved smoothly
  to follow the current item.  If highlightFollowsCurrentItem is false, the
  highlight will not be moved by the view, and must be implemented
  by the highlight component, for example:

  \code
  Component {
      id: myHighlight
      Rectangle {
          id: wrapper; color: "lightsteelblue"; radius: 4; width: 320; height: 60 >
          y: SpringFollow { source: Wrapper.GridView.view.currentItem.y; spring: 3; damping: 0.2 }
          x: SpringFollow { source: Wrapper.GridView.view.currentItem.x; spring: 3; damping: 0.2 }
      }
  }
  \endcode
*/
bool QmlGraphicsGridView::highlightFollowsCurrentItem() const
{
    Q_D(const QmlGraphicsGridView);
    return d->autoHighlight;
}

void QmlGraphicsGridView::setHighlightFollowsCurrentItem(bool autoHighlight)
{
    Q_D(QmlGraphicsGridView);
    if (d->autoHighlight != autoHighlight) {
        d->autoHighlight = autoHighlight;
        if (d->highlightXAnimator) {
            d->highlightXAnimator->setEnabled(d->autoHighlight);
            d->highlightYAnimator->setEnabled(d->autoHighlight);
        }
        d->updateHighlight();
    }
}

/*!
  \qmlproperty enumeration GridView::flow
  This property holds the flow of the grid.

  Possible values are \c LeftToRight (default) and \c TopToBottom.

  If \a flow is \c LeftToRight, the view will scroll vertically.
  If \a flow is \c TopToBottom, the view will scroll horizontally.
*/
QmlGraphicsGridView::Flow QmlGraphicsGridView::flow() const
{
    Q_D(const QmlGraphicsGridView);
    return d->flow;
}

void QmlGraphicsGridView::setFlow(Flow flow)
{
    Q_D(QmlGraphicsGridView);
    if (d->flow != flow) {
        d->flow = flow;
        if (d->flow == LeftToRight) {
            setViewportWidth(-1);
            setFlickDirection(QmlGraphicsFlickable::VerticalFlick);
        } else {
            setViewportHeight(-1);
            setFlickDirection(QmlGraphicsFlickable::HorizontalFlick);
        }
        d->clear();
        d->updateGrid();
        refill();
        d->updateCurrent(d->currentIndex);
    }
}

/*!
  \qmlproperty bool GridView::keyNavigationWraps
  This property holds whether the grid wraps key navigation

  If this property is true then key presses to move off of one end of the grid will cause the
  selection to jump to the other side.
*/
bool QmlGraphicsGridView::isWrapEnabled() const
{
    Q_D(const QmlGraphicsGridView);
    return d->wrap;
}

void QmlGraphicsGridView::setWrapEnabled(bool wrap)
{
    Q_D(QmlGraphicsGridView);
    d->wrap = wrap;
}

/*!
  \qmlproperty int GridView::cacheBuffer
  This property holds the number of off-screen pixels to cache.

  This property determines the number of pixels above the top of the view
  and below the bottom of the view to cache.  Setting this value can make
  scrolling the view smoother at the expense of additional memory usage.
*/
int QmlGraphicsGridView::cacheBuffer() const
{
    Q_D(const QmlGraphicsGridView);
    return d->buffer;
}

void QmlGraphicsGridView::setCacheBuffer(int buffer)
{
    Q_D(QmlGraphicsGridView);
    if (d->buffer != buffer) {
        d->buffer = buffer;
        if (isComponentComplete())
            refill();
    }
}

/*!
  \qmlproperty int GridView::cellWidth
  \qmlproperty int GridView::cellHeight

  These properties holds the width and height of each cell in the grid

  The default cell size is 100x100.
*/
int QmlGraphicsGridView::cellWidth() const
{
    Q_D(const QmlGraphicsGridView);
    return d->cellWidth;
}

void QmlGraphicsGridView::setCellWidth(int cellWidth)
{
    Q_D(QmlGraphicsGridView);
    if (cellWidth != d->cellWidth && cellWidth > 0) {
        d->cellWidth = qMax(1, cellWidth);
        d->updateGrid();
        emit cellWidthChanged();
        d->layout();
    }
}

int QmlGraphicsGridView::cellHeight() const
{
    Q_D(const QmlGraphicsGridView);
    return d->cellHeight;
}

void QmlGraphicsGridView::setCellHeight(int cellHeight)
{
    Q_D(QmlGraphicsGridView);
    if (cellHeight != d->cellHeight && cellHeight > 0) {
        d->cellHeight = qMax(1, cellHeight);
        d->updateGrid();
        emit cellHeightChanged();
        d->layout();
    }
}

void QmlGraphicsGridView::sizeChange()
{
    Q_D(QmlGraphicsGridView);
    if (isComponentComplete()) {
        d->updateGrid();
        d->layout();
    }
}

void QmlGraphicsGridView::viewportMoved()
{
    Q_D(QmlGraphicsGridView);
    QmlGraphicsFlickable::viewportMoved();
    d->lazyRelease = true;
    if (d->flicked) {
        if (yflick()) {
            if (d->velocityY > 0)
                d->bufferMode = QmlGraphicsGridViewPrivate::BufferBefore;
            else if (d->velocityY < 0)
                d->bufferMode = QmlGraphicsGridViewPrivate::BufferAfter;
        }

        if (xflick()) {
            if (d->velocityX > 0)
                d->bufferMode = QmlGraphicsGridViewPrivate::BufferBefore;
            else if (d->velocityX < 0)
                d->bufferMode = QmlGraphicsGridViewPrivate::BufferAfter;
        }
    }
    refill();
}

qreal QmlGraphicsGridView::minYExtent() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::TopToBottom)
        return QmlGraphicsFlickable::minYExtent();
    return -d->startPosition();
}

qreal QmlGraphicsGridView::maxYExtent() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::TopToBottom)
        return QmlGraphicsFlickable::maxYExtent();
    qreal extent = -(d->endPosition() - height());
    const qreal minY = minYExtent();
    if (extent > minY)
        extent = minY;
    return extent;
}

qreal QmlGraphicsGridView::minXExtent() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight)
        return QmlGraphicsFlickable::minXExtent();
    return -d->startPosition();
}

qreal QmlGraphicsGridView::maxXExtent() const
{
    Q_D(const QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight)
        return QmlGraphicsFlickable::maxXExtent();
    qreal extent = -(d->endPosition() - width());
    const qreal minX = minXExtent();
    if (extent > minX)
        extent = minX;
    return extent;
}

void QmlGraphicsGridView::keyPressEvent(QKeyEvent *event)
{
    Q_D(QmlGraphicsGridView);
    QmlGraphicsFlickable::keyPressEvent(event);
    if (event->isAccepted())
        return;
    if (d->model && d->model->count() && d->interactive) {
        d->moveReason = QmlGraphicsGridViewPrivate::SetIndex;
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
    d->moveReason = QmlGraphicsGridViewPrivate::Other;
    event->ignore();
}

/*!
    \qmlmethod GridView::moveCurrentIndexUp()

    Move the currentIndex up one item in the view.
    The current index will wrap if keyNavigationWraps is true and it
    is currently at the end.
*/
void QmlGraphicsGridView::moveCurrentIndexUp()
{
    Q_D(QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight) {
        if (currentIndex() >= d->columns || d->wrap) {
            int index = currentIndex() - d->columns;
            setCurrentIndex(index >= 0 ? index : d->model->count()-1);
        }
    } else {
        if (currentIndex() > 0 || d->wrap) {
            int index = currentIndex() - 1;
            setCurrentIndex(index >= 0 ? index : d->model->count()-1);
        }
    }
}

/*!
    \qmlmethod GridView::moveCurrentIndexDown()

    Move the currentIndex down one item in the view.
    The current index will wrap if keyNavigationWraps is true and it
    is currently at the end.
*/
void QmlGraphicsGridView::moveCurrentIndexDown()
{
    Q_D(QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight) {
        if (currentIndex() < d->model->count() - d->columns || d->wrap) {
            int index = currentIndex()+d->columns;
            setCurrentIndex(index < d->model->count() ? index : 0);
        }
    } else {
        if (currentIndex() < d->model->count() - 1 || d->wrap) {
            int index = currentIndex() + 1;
            setCurrentIndex(index < d->model->count() ? index : 0);
        }
    }
}

/*!
    \qmlmethod GridView::moveCurrentIndexLeft()

    Move the currentIndex left one item in the view.
    The current index will wrap if keyNavigationWraps is true and it
    is currently at the end.
*/
void QmlGraphicsGridView::moveCurrentIndexLeft()
{
    Q_D(QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight) {
        if (currentIndex() > 0 || d->wrap) {
            int index = currentIndex() - 1;
            setCurrentIndex(index >= 0 ? index : d->model->count()-1);
        }
    } else {
        if (currentIndex() >= d->columns || d->wrap) {
            int index = currentIndex() - d->columns;
            setCurrentIndex(index >= 0 ? index : d->model->count()-1);
        }
    }
}

/*!
    \qmlmethod GridView::moveCurrentIndexRight()

    Move the currentIndex right one item in the view.
    The current index will wrap if keyNavigationWraps is true and it
    is currently at the end.
*/
void QmlGraphicsGridView::moveCurrentIndexRight()
{
    Q_D(QmlGraphicsGridView);
    if (d->flow == QmlGraphicsGridView::LeftToRight) {
        if (currentIndex() < d->model->count() - 1 || d->wrap) {
            int index = currentIndex() + 1;
            setCurrentIndex(index < d->model->count() ? index : 0);
        }
    } else {
        if (currentIndex() < d->model->count() - d->columns || d->wrap) {
            int index = currentIndex()+d->columns;
            setCurrentIndex(index < d->model->count() ? index : 0);
        }
    }
}

void QmlGraphicsGridView::positionViewAtIndex(int index)
{
    Q_D(QmlGraphicsGridView);
    if (!d->isValid() || index < 0 || index >= d->model->count())
        return;

    qreal maxExtent = d->flow == QmlGraphicsGridView::LeftToRight ? -maxYExtent() : -maxXExtent();
    FxGridItem *item = d->visibleItem(index);
    if (item) {
        // Already created - just move to top of view
        int pos = qMin(item->rowPos(), maxExtent);
        d->setPosition(pos);
    } else {
        int pos = d->rowPosAt(index);
        // save the currently visible items in case any of them end up visible again
        QList<FxGridItem*> oldVisible = d->visibleItems;
        d->visibleItems.clear();
        d->visibleIndex = index - index % d->columns;
        d->setPosition(pos);
        // setPosition() will cause refill.  Adjust if we have moved beyond range
        if (d->position() > maxExtent)
            d->setPosition(maxExtent);
        // now release the reference to all the old visible items.
        for (int i = 0; i < oldVisible.count(); ++i)
            d->releaseItem(oldVisible.at(i));
    }
}


void QmlGraphicsGridView::componentComplete()
{
    Q_D(QmlGraphicsGridView);
    QmlGraphicsFlickable::componentComplete();
    d->updateGrid();
    refill();
    if (d->currentIndex < 0)
        d->updateCurrent(0);
    else
        d->updateCurrent(d->currentIndex);
}

void QmlGraphicsGridView::trackedPositionChanged()
{
    Q_D(QmlGraphicsGridView);
    if (!d->trackedItem || !d->currentItem)
        return;
    if (!isFlicking() && !d->moving && d->moveReason == QmlGraphicsGridViewPrivate::SetIndex) {
        const qreal viewPos = d->position();
        if (d->trackedItem->rowPos() < viewPos && d->currentItem->rowPos() < viewPos) {
            d->setPosition(d->currentItem->rowPos() < d->trackedItem->rowPos() ? d->trackedItem->rowPos() : d->currentItem->rowPos());
        } else if (d->trackedItem->endRowPos() > viewPos + d->size()
                && d->currentItem->endRowPos() > viewPos + d->size()) {
            qreal pos;
            if (d->trackedItem->endRowPos() < d->currentItem->endRowPos()) {
                pos = d->trackedItem->endRowPos() - d->size();
                if (d->rowSize() > d->size())
                    pos = d->trackedItem->rowPos();
            } else {
                pos = d->currentItem->endRowPos() - d->size();
                if (d->rowSize() > d->size())
                    pos = d->currentItem->rowPos();
            }
            d->setPosition(pos);
        }
    }
}

void QmlGraphicsGridView::itemsInserted(int modelIndex, int count)
{
    Q_D(QmlGraphicsGridView);
    if (!d->visibleItems.count() || d->model->count() <= 1) {
        refill();
        d->updateCurrent(qMax(0, qMin(d->currentIndex, d->model->count()-1)));
        emit countChanged();
        return;
    }

    int index = d->mapFromModel(modelIndex);
    if (index == -1) {
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
                    FxGridItem *listItem = d->visibleItems.at(i);
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
    int insertCount = count;
    if (index < d->visibleIndex) {
        insertCount -= d->visibleIndex - index;
        index = d->visibleIndex;
        modelIndex = d->visibleIndex;
    }

    index -= d->visibleIndex;
    int to = d->buffer+d->position()+d->size()-1;
    int colPos, rowPos;
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

    QList<FxGridItem*> added;
    int i = 0;
    while (i < insertCount && rowPos <= to + d->rowSize()*(d->columns - (colPos/d->colSize()))/qreal(d->columns)) {
        FxGridItem *item = d->createItem(modelIndex + i);
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

    if (d->currentIndex >= modelIndex) {
        // adjust current item index
        d->currentIndex += count;
        if (d->currentItem) {
            d->currentItem->index = d->currentIndex;
            d->currentItem->setPosition(d->colPosAt(d->currentIndex), d->rowPosAt(d->currentIndex));
        }
    }
    // Update the indexes of the following visible items.
    for (; index < d->visibleItems.count(); ++index) {
        FxGridItem *listItem = d->visibleItems.at(index);
        if (listItem->index != -1)
            listItem->index += count;
    }

    // everything is in order now - emit add() signal
    for (int j = 0; j < added.count(); ++j)
        added.at(j)->attached->emitAdd();
    d->layout();
    emit countChanged();
}

void QmlGraphicsGridView::itemsRemoved(int modelIndex, int count)
{
    Q_D(QmlGraphicsGridView);
    bool currentRemoved = d->currentIndex >= modelIndex && d->currentIndex < modelIndex + count;
    bool removedVisible = false;

    // Remove the items from the visible list, skipping anything already marked for removal
    QList<FxGridItem*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItem *item = *it;
        if (item->index == -1 || item->index < modelIndex) {
            // already removed, or before removed items
            if (item->index < modelIndex)
                removedVisible = true;
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
    } else if (currentRemoved) {
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

    if (removedVisible) {
        if (d->visibleItems.isEmpty()) {
            d->visibleIndex = 0;
            d->setPosition(0);
            refill();
        } else {
            // Correct the positioning of the items
            d->scheduleLayout();
        }
    }

    emit countChanged();
}

void QmlGraphicsGridView::layout()
{
    Q_D(QmlGraphicsGridView);
    if (d->layoutScheduled)
        d->layout();
}

void QmlGraphicsGridView::destroyRemoved()
{
    Q_D(QmlGraphicsGridView);
    for (QList<FxGridItem*>::Iterator it = d->visibleItems.begin();
            it != d->visibleItems.end();) {
        FxGridItem *listItem = *it;
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

void QmlGraphicsGridView::itemsMoved(int from, int to, int count)
{
    Q_D(QmlGraphicsGridView);
    QHash<int,FxGridItem*> moved;

    bool removedBeforeVisible = false;
    FxGridItem *firstItem = d->firstVisibleItem();

    if (from < to && from < d->visibleIndex && to > d->visibleIndex)
        removedBeforeVisible = true;

    QList<FxGridItem*>::Iterator it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItem *item = *it;
        if (item->index >= from && item->index < from + count) {
            // take the items that are moving
            item->index += (to-from);
            moved.insert(item->index, item);
            it = d->visibleItems.erase(it);
            if (item->rowPos() < firstItem->rowPos())
                removedBeforeVisible = true;
        } else {
            if (item->index > from && item->index != -1) {
                // move everything after the moved items.
                item->index -= count;
                if (item->index < d->visibleIndex)
                    d->visibleIndex = item->index;
            } else if (item->index != -1) {
                removedBeforeVisible = true;
            }
            ++it;
        }
    }

    int remaining = count;
    int endIndex = d->visibleIndex;
    it = d->visibleItems.begin();
    while (it != d->visibleItems.end()) {
        FxGridItem *item = *it;
        if (remaining && item->index >= to && item->index < to + count) {
            // place items in the target position, reusing any existing items
            FxGridItem *movedItem = moved.take(item->index);
            if (!movedItem)
                movedItem = d->createItem(item->index);
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
    while (FxGridItem *item = moved.take(endIndex+1)) {
        d->visibleItems.append(item);
        ++endIndex;
    }

    // Whatever moved items remain are no longer visible items.
    while (moved.count())
        d->releaseItem(moved.take(moved.begin().key()));

    d->layout(removedBeforeVisible);
}

void QmlGraphicsGridView::createdItem(int index, QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsGridView);
    item->setParentItem(this);
    if (d->requestedIndex != index) {
        item->setParentItem(this);
        d->unrequestedItems.insert(item, index);
        if (d->flow == QmlGraphicsGridView::LeftToRight) {
            item->setPos(QPointF(d->colPosAt(index), d->rowPosAt(index)));
        } else {
            item->setPos(QPointF(d->rowPosAt(index), d->colPosAt(index)));
        }
    }
}

void QmlGraphicsGridView::destroyingItem(QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsGridView);
    d->unrequestedItems.remove(item);
}


void QmlGraphicsGridView::refill()
{
    Q_D(QmlGraphicsGridView);
    d->refill(d->position(), d->position()+d->size()-1);
}


QmlGraphicsGridViewAttached *QmlGraphicsGridView::qmlAttachedProperties(QObject *obj)
{
    return QmlGraphicsGridViewAttached::properties(obj);
}

QML_DEFINE_TYPE(Qt, 4,6, GridView, QmlGraphicsGridView)

QT_END_NAMESPACE

#include <qmlgraphicsgridview.moc>
