/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSSCENEINDEX_P_H
#define QGRAPHICSSCENEINDEX_P_H

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

#include "qgraphicssceneindex.h"

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include <private/qobject_p.h>
#include <private/qgraphicsitem_p.h>

QT_BEGIN_NAMESPACE

class QGraphicsScene;
class QGraphicsSceneIndexIntersector;
class QGraphicsSceneIndexRectIntersector;
class QGraphicsSceneIndexPointIntersector;
class QGraphicsSceneIndexPathIntersector;

class QGraphicsSceneIndexPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QGraphicsSceneIndex)
public:
    QGraphicsSceneIndexPrivate(QGraphicsScene *scene);
    ~QGraphicsSceneIndexPrivate();

    static bool itemCollidesWithPath(const QGraphicsItem *item, const QPainterPath &path, Qt::ItemSelectionMode mode);

    void recursive_items_helper(QGraphicsItem *item, QRectF exposeRect,
                                QGraphicsSceneIndexIntersector *intersector, QList<QGraphicsItem *> *items,
                                const QTransform &parentTransform, const QTransform &viewTransform,
                                Qt::ItemSelectionMode mode, Qt::SortOrder order, qreal parentOpacity = 1.0) const;
   QGraphicsScene *scene;
   QGraphicsSceneIndexPointIntersector *pointIntersector;
   QGraphicsSceneIndexRectIntersector *rectIntersector;
   QGraphicsSceneIndexPathIntersector *pathIntersector;
};

class QGraphicsSceneIndexIntersector
{
public:
    QGraphicsSceneIndexIntersector() { }
    virtual ~QGraphicsSceneIndexIntersector() { }
    virtual bool intersect(const QGraphicsItem *item, const QRectF &exposeRect, Qt::ItemSelectionMode mode,
                           const QTransform &transform, const QTransform &deviceTransform) const = 0;
};

QT_END_NAMESPACE

#endif // QGRAPHICSSCENEINDEX_P_H

#endif
