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

#ifndef QGRAPHICSSCENELINEARINDEX_P_H
#define QGRAPHICSSCENELINEARINDEX_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qlist.h>

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

#include <QtCore/qrect.h>
#include <QtCore/qlist.h>
#include <QtGui/qgraphicssceneindex.h>
#include <QtGui/qgraphicsitem.h>

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QGraphicsSceneLinearIndex : public QGraphicsSceneIndex
{
    Q_OBJECT

private:
    QRectF m_sceneRect;
    QList<QGraphicsItem*> m_items;

public:
    QGraphicsSceneLinearIndex(QObject *parent = 0): QGraphicsSceneIndex(parent) {
    }

    virtual void setRect(const QRectF &rect) {
        m_sceneRect = rect;
    }

    virtual QRectF rect() const {
        return m_sceneRect;
    }

    virtual void clear() {
        m_items.clear();
    }

    virtual void insertItem(QGraphicsItem *item) {
        m_items << item;
    }

    virtual void removeItem(QGraphicsItem *item) {
        m_items.removeAll(item);
    }

    virtual QList<QGraphicsItem *> items(const QPointF &point) {
        QList<QGraphicsItem*> result;
        foreach (QGraphicsItem *item, m_items)
            if (item->sceneBoundingRect().contains(point))
                result << item;
        return result;
    }

    virtual QList<QGraphicsItem *> items(const QRectF &rect) {
        QList<QGraphicsItem*> result;
        foreach (QGraphicsItem *item, m_items)
            if (item->sceneBoundingRect().intersects(rect))
                result << item;
        return result;
    }
};

QT_END_NAMESPACE

#endif // QT_NO_GRAPHICSVIEW

#endif // QGRAPHICSSCENELINEARINDEX_P_H
