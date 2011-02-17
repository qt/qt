// Commit: ac5c099cc3c5b8c7eec7a49fdeb8a21037230350
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

#ifndef QDECLARATIVEPATHVIEW_P_H
#define QDECLARATIVEPATHVIEW_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qsgpathview_p.h"
#include "qsgitem_p.h"
#include "qsgvisualitemmodel_p.h"

#include <QtDeclarative/qdeclarative.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qcoreapplication.h>

#include <private/qdeclarativeanimation_p_p.h>
#include <private/qdeclarativeguard_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeOpenMetaObjectType;
class QSGPathViewAttached;
class QSGPathViewPrivate : public QSGItemPrivate, public QSGItemChangeListener
{
    Q_DECLARE_PUBLIC(QSGPathView)

public:
    QSGPathViewPrivate()
      : path(0), currentIndex(0), currentItemOffset(0.0), startPc(0), lastDist(0)
        , lastElapsed(0), offset(0.0), offsetAdj(0.0), mappedRange(1.0)
        , stealMouse(false), ownModel(false), interactive(true), haveHighlightRange(true)
        , autoHighlight(true), highlightUp(false), layoutScheduled(false)
        , moving(false), flicking(false)
        , dragMargin(0), deceleration(100)
        , moveOffset(this, &QSGPathViewPrivate::setAdjustedOffset)
        , firstIndex(-1), pathItems(-1), requestedIndex(-1)
        , moveReason(Other), moveDirection(Shortest), attType(0), highlightComponent(0), highlightItem(0)
        , moveHighlight(this, &QSGPathViewPrivate::setHighlightPosition)
        , highlightPosition(0)
        , highlightRangeStart(0), highlightRangeEnd(0)
        , highlightRangeMode(QSGPathView::StrictlyEnforceRange)
        , highlightMoveDuration(300), modelCount(0)
    {
    }

    void init();

    void itemGeometryChanged(QSGItem *item, const QRectF &newGeometry, const QRectF &oldGeometry) {
        if ((newGeometry.size() != oldGeometry.size())
            && (!highlightItem || item != highlightItem)) {
            if (QSGPathViewAttached *att = attached(item))
                att->m_percent = -1;
            scheduleLayout();
        }
    }

    void scheduleLayout() {
        Q_Q(QSGPathView);
        if (!layoutScheduled) {
            layoutScheduled = true;
            q->polish();
        }
    }

    QSGItem *getItem(int modelIndex);
    void releaseItem(QSGItem *item);
    QSGPathViewAttached *attached(QSGItem *item);
    void clear();
    void updateMappedRange();
    qreal positionOfIndex(qreal index) const;
    void createHighlight();
    void updateHighlight();
    void setHighlightPosition(qreal pos);
    bool isValid() const {
        return model && model->count() > 0 && model->isValid() && path;
    }

    void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    int calcCurrentIndex();
    void updateCurrent();
    static void fixOffsetCallback(void*);
    void fixOffset();
    void setOffset(qreal offset);
    void setAdjustedOffset(qreal offset);
    void regenerate();
    void updateItem(QSGItem *, qreal);
    void snapToCurrent();
    QPointF pointNear(const QPointF &point, qreal *nearPercent=0) const;

    QDeclarativePath *path;
    int currentIndex;
    QDeclarativeGuard<QSGItem> currentItem;
    qreal currentItemOffset;
    qreal startPc;
    QPointF startPoint;
    qreal lastDist;
    int lastElapsed;
    qreal offset;
    qreal offsetAdj;
    qreal mappedRange;
    bool stealMouse : 1;
    bool ownModel : 1;
    bool interactive : 1;
    bool haveHighlightRange : 1;
    bool autoHighlight : 1;
    bool highlightUp : 1;
    bool layoutScheduled : 1;
    bool moving : 1;
    bool flicking : 1;
    QElapsedTimer lastPosTime;
    QPointF lastPos;
    qreal dragMargin;
    qreal deceleration;
    QDeclarativeTimeLine tl;
    QDeclarativeTimeLineValueProxy<QSGPathViewPrivate> moveOffset;
    int firstIndex;
    int pathItems;
    int requestedIndex;
    QList<QSGItem *> items;
    QList<QSGItem *> itemCache;
    QDeclarativeGuard<QSGVisualModel> model;
    QVariant modelVariant;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    enum MovementDirection { Shortest, Negative, Positive };
    MovementDirection moveDirection;
    QDeclarativeOpenMetaObjectType *attType;
    QDeclarativeComponent *highlightComponent;
    QSGItem *highlightItem;
    QDeclarativeTimeLineValueProxy<QSGPathViewPrivate> moveHighlight;
    qreal highlightPosition;
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    QSGPathView::HighlightRangeMode highlightRangeMode;
    int highlightMoveDuration;
    int modelCount;
};

QT_END_NAMESPACE

#endif
