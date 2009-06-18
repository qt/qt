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

#ifndef QGRAPHICSSCENEINDEX_H
#define QGRAPHICSSCENEINDEX_H

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

#include <QtCore/qnamespace.h>
#include <QtCore/qobject.h>
#include <QtGui/qtransform.h>
#include <QtGui/qgraphicsitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsSceneIndexPrivate;
class QGraphicsScene;
class QRectF;
class QPointF;
template<typename T> class QList;

class Q_AUTOTEST_EXPORT QGraphicsSceneIndex: public QObject
{
    Q_OBJECT

public:
    QGraphicsSceneIndex(QGraphicsScene *scene = 0);
    virtual ~QGraphicsSceneIndex();

    QGraphicsScene *scene() const;

    virtual QRectF indexedRect() const;

    virtual QList<QGraphicsItem *> items(Qt::SortOrder order = Qt::AscendingOrder) const  = 0;
    virtual QList<QGraphicsItem *> items(const QPointF &pos, Qt::ItemSelectionMode mode,
                                         Qt::SortOrder order, const QTransform &deviceTransform = QTransform()) const;
    virtual QList<QGraphicsItem *> items(const QRectF &rect, Qt::ItemSelectionMode mode,
                                         Qt::SortOrder order, const QTransform &deviceTransform = QTransform()) const;
    virtual QList<QGraphicsItem *> items(const QPolygonF &polygon, Qt::ItemSelectionMode mode,
                                         Qt::SortOrder order, const QTransform &deviceTransform = QTransform()) const;
    virtual QList<QGraphicsItem *> items(const QPainterPath &path, Qt::ItemSelectionMode mode,
                                         Qt::SortOrder order, const QTransform &deviceTransform = QTransform()) const;
    virtual QList<QGraphicsItem *> estimateItems(const QPointF &point,
                                                 Qt::SortOrder order, const QTransform &deviceTransform) const;
    virtual QList<QGraphicsItem *> estimateItems(const QRectF &rect,
                                                 Qt::SortOrder order, const QTransform &deviceTransform) const = 0;

protected:
    virtual void clear();
    virtual void addItem(QGraphicsItem *item) = 0;
    virtual void removeItem(QGraphicsItem *item) = 0;
    virtual void deleteItem(QGraphicsItem *item);

    virtual void itemChange(const QGraphicsItem *item, QGraphicsItem::GraphicsItemChange, const QVariant &value);
    virtual void prepareBoundingRectChange(const QGraphicsItem *item);
    virtual void sceneRectChanged(const QRectF &rect);

    QGraphicsSceneIndex(QObjectPrivate &dd, QGraphicsScene *scene);

    friend class QGraphicsScene;
    friend class QGraphicsScenePrivate;
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    friend class QGraphicsSceneBspTreeIndex;
private:
    Q_DISABLE_COPY(QGraphicsSceneIndex)
    Q_DECLARE_PRIVATE(QGraphicsSceneIndex)
};

#endif // QT_NO_GRAPHICSVIEW

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGRAPHICSSCENEINDEX_H
