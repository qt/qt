/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSSCENE_P_H
#define QGRAPHICSSCENE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include "qgraphicsscene.h"

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include "qgraphicssceneevent.h"
#include "qgraphicsview.h"
#include "qgraphicsscene_bsp_p.h"
#include "qgraphicsitem_p.h"

#include <private/qobject_p.h>
#include <QtCore/qbitarray.h>
#include <QtCore/qlist.h>
#include <QtCore/qmap.h>
#include <QtCore/qset.h>
#include <QtGui/qfont.h>
#include <QtGui/qpalette.h>
#include <QtGui/qstyle.h>
#include <QtGui/qstyleoption.h>

static const int QGRAPHICSSCENE_INDEXTIMER_TIMEOUT = 2000;

QT_BEGIN_NAMESPACE

class QGraphicsView;
class QGraphicsWidget;

class QGraphicsScenePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsScene)
public:
    QGraphicsScenePrivate();
    void init();

    quint32 changedSignalMask;

    QGraphicsScene::ItemIndexMethod indexMethod;
    int bspTreeDepth;

    QList<QGraphicsItem *> estimateItemsInRect(const QRectF &rect) const;
    void addToIndex(QGraphicsItem *item);
    void removeFromIndex(QGraphicsItem *item);
    void resetIndex();

    QGraphicsSceneBspTree bspTree;
    void _q_updateIndex();
    int lastItemCount;

    QRectF sceneRect;
    bool hasSceneRect;
    QRectF growingItemsBoundingRect;
    QRectF largestUntransformableItem;

    void _q_emitUpdated();
    QList<QRectF> updatedRects;
    bool updateAll;
    bool calledEmitUpdated;
    bool processDirtyItemsEmitted;

    QPainterPath selectionArea;
    int selectionChanging;
    QSet<QGraphicsItem *> selectedItems;
    QList<QGraphicsItem *> unindexedItems;
    QList<QGraphicsItem *> indexedItems;
    QList<QGraphicsItem *> pendingUpdateItems;
    QList<QGraphicsItem *> unpolishedItems;
    QList<QGraphicsItem *> topLevelItems;
    bool needSortTopLevelItems;
    QMap<QGraphicsItem *, QPointF> movingItemsInitialPositions;
    void registerTopLevelItem(QGraphicsItem *item);
    void unregisterTopLevelItem(QGraphicsItem *item);
    void _q_updateLater();
    void _q_polishItems();

    void _q_processDirtyItems();

    QList<int> freeItemIndexes;
    bool regenerateIndex;

    bool purgePending;
    void removeItemHelper(QGraphicsItem *item);
    QSet<QGraphicsItem *> removedItems;
    void purgeRemovedItems();

    QBrush backgroundBrush;
    QBrush foregroundBrush;

    int indexTimerId;
    bool restartIndexTimer;
    void startIndexTimer(int interval = QGRAPHICSSCENE_INDEXTIMER_TIMEOUT);

    bool stickyFocus;
    bool hasFocus;
    QGraphicsItem *focusItem;
    QGraphicsItem *lastFocusItem;
    QGraphicsWidget *tabFocusFirst;
    QGraphicsWidget *activeWindow;
    int activationRefCount;

    QList<QGraphicsWidget *> popupWidgets;
    void addPopup(QGraphicsWidget *widget);
    void removePopup(QGraphicsWidget *widget, bool itemIsDying = false);

    QGraphicsItem *lastMouseGrabberItem;
    bool lastMouseGrabberItemHasImplicitMouseGrab;
    QList<QGraphicsItem *> mouseGrabberItems;
    void grabMouse(QGraphicsItem *item, bool implicit = false);
    void ungrabMouse(QGraphicsItem *item, bool itemIsDying = false);
    void clearMouseGrabber();

    QList<QGraphicsItem *> keyboardGrabberItems;
    void grabKeyboard(QGraphicsItem *item);
    void ungrabKeyboard(QGraphicsItem *item, bool itemIsDying = false);
    void clearKeyboardGrabber();

    QGraphicsItem *dragDropItem;
    QGraphicsWidget *enterWidget;
    Qt::DropAction lastDropAction;
    QList<QGraphicsItem *> cachedItemsUnderMouse;
    QList<QGraphicsItem *> hoverItems;
    bool allItemsIgnoreHoverEvents;
    bool allItemsUseDefaultCursor;
    void enableMouseTrackingOnViews();
    QMap<Qt::MouseButton, QPointF> mouseGrabberButtonDownPos;
    QMap<Qt::MouseButton, QPointF> mouseGrabberButtonDownScenePos;
    QMap<Qt::MouseButton, QPoint> mouseGrabberButtonDownScreenPos;
    QList<QGraphicsItem *> itemsAtPosition(const QPoint &screenPos,
                                           const QPointF &scenePos,
                                           QWidget *widget) const;
    static bool itemCollidesWithPath(QGraphicsItem *item, const QPainterPath &path, Qt::ItemSelectionMode mode);
    void storeMouseButtonsForMouseGrabber(QGraphicsSceneMouseEvent *event);

    QList<QGraphicsView *> views;
    void addView(QGraphicsView *view);
    void removeView(QGraphicsView *view);

    bool painterStateProtection;

    QMultiMap<QGraphicsItem *, QGraphicsItem *> sceneEventFilters;
    void installSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter);
    void removeSceneEventFilter(QGraphicsItem *watched, QGraphicsItem *filter);
    bool filterEvent(QGraphicsItem *item, QEvent *event);
    bool sendEvent(QGraphicsItem *item, QEvent *event);

    bool dispatchHoverEvent(QGraphicsSceneHoverEvent *hoverEvent);
    bool itemAcceptsHoverEvents_helper(const QGraphicsItem *item) const;
    void leaveScene();

    void cloneDragDropEvent(QGraphicsSceneDragDropEvent *dest,
                           QGraphicsSceneDragDropEvent *source);
    void sendDragDropEvent(QGraphicsItem *item,
                           QGraphicsSceneDragDropEvent *dragDropEvent);
    void sendHoverEvent(QEvent::Type type, QGraphicsItem *item,
                        QGraphicsSceneHoverEvent *hoverEvent);
    void sendMouseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEventHandler(QGraphicsSceneMouseEvent *mouseEvent);
    QGraphicsWidget *windowForItem(const QGraphicsItem *item) const;

    QList<QGraphicsItem *> topLevelItemsInStackingOrder(const QTransform *const, QRegion *);
    void recursive_items_helper(QGraphicsItem *item, QRectF rect, QList<QGraphicsItem *> *items,
                                const QTransform &parentTransform, const QTransform &viewTransform,
                                Qt::ItemSelectionMode mode, Qt::SortOrder order, qreal parentOpacity = 1.0) const;

    QList<QGraphicsItem *> items_helper(const QPointF &pos) const;
    QList<QGraphicsItem *> items_helper(const QRectF &rect,
                                        Qt::ItemSelectionMode mode,
                                        Qt::SortOrder order) const;
    QList<QGraphicsItem *> items_helper(const QPolygonF &rect,
                                        Qt::ItemSelectionMode mode,
                                        Qt::SortOrder order) const;
    QList<QGraphicsItem *> items_helper(const QPainterPath &rect,
                                        Qt::ItemSelectionMode mode,
                                        Qt::SortOrder order) const;
    void childItems_helper(QList<QGraphicsItem *> *items,
                           const QGraphicsItem *parent,
                           const QPointF &pos) const;
    void childItems_helper(QList<QGraphicsItem *> *items,
                           const QGraphicsItem *parent,
                           const QRectF &rect,
                           Qt::ItemSelectionMode mode) const;
    void childItems_helper(QList<QGraphicsItem *> *items,
                           const QGraphicsItem *parent,
                           const QPolygonF &polygon,
                           Qt::ItemSelectionMode mode) const;
    void childItems_helper(QList<QGraphicsItem *> *items,
                           const QGraphicsItem *parent,
                           const QPainterPath &path,
                           Qt::ItemSelectionMode mode) const;

    bool sortCacheEnabled;
    bool updatingSortCache;
    void invalidateSortCache();
    static void climbTree(QGraphicsItem *item, int *stackingOrder);
    void _q_updateSortCache();

    static bool closestItemFirst_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2);
    static bool closestItemLast_withoutCache(const QGraphicsItem *item1, const QGraphicsItem *item2);

    static inline bool closestItemFirst_withCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
    {
        return item1->d_ptr->globalStackingOrder < item2->d_ptr->globalStackingOrder;
    }
    static inline bool closestItemLast_withCache(const QGraphicsItem *item1, const QGraphicsItem *item2)
    {
        return item1->d_ptr->globalStackingOrder >= item2->d_ptr->globalStackingOrder;
    }

    static void sortItems(QList<QGraphicsItem *> *itemList, Qt::SortOrder order, bool cached);

    void drawItemHelper(QGraphicsItem *item, QPainter *painter,
                        const QStyleOptionGraphicsItem *option, QWidget *widget,
                        bool painterStateProtection);

    inline void drawItems(QPainter *painter, const QTransform *const viewTransform,
                          QRegion *exposedRegion, QWidget *widget)
    {
        const QList<QGraphicsItem *> tli = topLevelItemsInStackingOrder(viewTransform, exposedRegion);
        for (int i = 0; i < tli.size(); ++i)
            drawSubtreeRecursive(tli.at(i), painter, viewTransform, exposedRegion, widget);
        return;
    }

    void drawSubtreeRecursive(QGraphicsItem *item, QPainter *painter, const QTransform *const,
                              QRegion *exposedRegion, QWidget *widget, qreal parentOpacity = qreal(1.0));
    void markDirty(QGraphicsItem *item, const QRectF &rect = QRectF(), bool invalidateChildren = false,
                   bool maybeDirtyClipPath = false, bool force = false, bool ignoreOpacity = false,
                   bool removingItemFromScene = false);
    void processDirtyItemsRecursive(QGraphicsItem *item, bool dirtyAncestorContainsChildren = false,
                                    qreal parentOpacity = qreal(1.0));

    inline void resetDirtyItem(QGraphicsItem *item)
    {
        Q_ASSERT(item);
        item->d_ptr->dirty = 0;
        item->d_ptr->paintedViewBoundingRectsNeedRepaint = 0;
        item->d_ptr->geometryChanged = 0;
        item->d_ptr->dirtyChildren = 0;
        item->d_ptr->needsRepaint = QRectF();
        item->d_ptr->allChildrenDirty = 0;
        item->d_ptr->fullUpdatePending = 0;
        item->d_ptr->ignoreVisible = 0;
        item->d_ptr->ignoreOpacity = 0;
    }

    QStyle *style;
    QFont font;
    void setFont_helper(const QFont &font);
    void resolveFont();
    void updateFont(const QFont &font);
    QPalette palette;
    void setPalette_helper(const QPalette &palette);
    void resolvePalette();
    void updatePalette(const QPalette &palette);

    QStyleOptionGraphicsItem styleOptionTmp;

    QMap<int, QTouchEvent::TouchPoint> sceneCurrentTouchPoints;
    QMap<int, QGraphicsItem *> itemForTouchPointId;
    static void updateTouchPointsForItem(QGraphicsItem *item, QTouchEvent *touchEvent);
    int findClosestTouchPointId(const QPointF &scenePos);
    void touchEventHandler(QTouchEvent *touchEvent);
    bool sendTouchBeginEvent(QGraphicsItem *item, QTouchEvent *touchEvent);
    bool allItemsIgnoreTouchEvents;
    void enableTouchEventsOnViews();

    void updateInputMethodSensitivityInViews();
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif
