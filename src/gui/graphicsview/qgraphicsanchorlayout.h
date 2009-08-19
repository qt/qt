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

#ifndef QGRAPHICSANCHORLAYOUT_H
#define QGRAPHICSANCHORLAYOUT_H

#include <QtGui/qgraphicsitem.h>
#include <QtGui/qgraphicslayout.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#if !defined(QT_NO_GRAPHICSVIEW) || (QT_EDITION & QT_MODULE_GRAPHICSVIEW) != QT_MODULE_GRAPHICSVIEW

class QGraphicsAnchorLayoutPrivate;

class Q_GUI_EXPORT QGraphicsAnchorLayout : public QGraphicsLayout
{
public:
    QGraphicsAnchorLayout(QGraphicsLayoutItem *parent = 0);
    virtual ~QGraphicsAnchorLayout();

    void addAnchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                   QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge);

    void addCornerAnchors(QGraphicsLayoutItem *firstItem, Qt::Corner firstCorner,
                          QGraphicsLayoutItem *secondItem, Qt::Corner secondCorner);

    inline void addLeftAndRightAnchors(QGraphicsLayoutItem *firstItem,
                                       QGraphicsLayoutItem *secondItem);

    inline void addTopAndBottomAnchors(QGraphicsLayoutItem *firstItem,
                                       QGraphicsLayoutItem *secondItem);

    inline void addAllAnchors(QGraphicsLayoutItem *firstItem,
                              QGraphicsLayoutItem *secondItem);

    void setAnchorSpacing(const QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                          const QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge,
                          qreal spacing);

    qreal anchorSpacing(const QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                        const QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge) const;

    void unsetAnchorSpacing(const QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                            const QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge);

    void removeAnchor(QGraphicsLayoutItem *firstItem, Qt::AnchorPoint firstEdge,
                      QGraphicsLayoutItem *secondItem, Qt::AnchorPoint secondEdge);

    void setHorizontalSpacing(qreal spacing);
    void setVerticalSpacing(qreal spacing);
    void setSpacing(qreal spacing);
    qreal horizontalSpacing() const;
    qreal verticalSpacing() const;

    void removeAt(int index);
    void setGeometry(const QRectF &rect);
    int count() const;
    QGraphicsLayoutItem *itemAt(int index) const;

    void invalidate();
protected:
    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

private:
    Q_DISABLE_COPY(QGraphicsAnchorLayout)
    Q_DECLARE_PRIVATE(QGraphicsAnchorLayout)
};


void QGraphicsAnchorLayout::addLeftAndRightAnchors(QGraphicsLayoutItem *firstItem,
                                                   QGraphicsLayoutItem *secondItem)
{
    addAnchor(secondItem, Qt::AnchorLeft, firstItem, Qt::AnchorLeft);
    addAnchor(firstItem, Qt::AnchorRight, secondItem, Qt::AnchorRight);
}

void QGraphicsAnchorLayout::addTopAndBottomAnchors(QGraphicsLayoutItem *firstItem,
                                                   QGraphicsLayoutItem *secondItem)
{
    addAnchor(secondItem, Qt::AnchorTop, firstItem, Qt::AnchorTop);
    addAnchor(firstItem, Qt::AnchorBottom, secondItem, Qt::AnchorBottom);
}

void QGraphicsAnchorLayout::addAllAnchors(QGraphicsLayoutItem *firstItem,
                                          QGraphicsLayoutItem *secondItem)
{
    addLeftAndRightAnchors(firstItem, secondItem);
    addTopAndBottomAnchors(firstItem, secondItem);
}

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
