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

#ifndef QMLGRAPHICSANCHORS_P_H
#define QMLGRAPHICSANCHORS_P_H

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

#include "qmlgraphicsanchors_p.h"
#include "qmlgraphicsitemchangelistener_p.h"
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlGraphicsAnchorLine
{
public:
    QmlGraphicsAnchorLine() : item(0), anchorLine(Invalid)
    {
    }

    enum AnchorLine {
        Invalid = 0x0,
        Left = 0x01,
        Right = 0x02,
        Top = 0x04,
        Bottom = 0x08,
        HCenter = 0x10,
        VCenter = 0x20,
        Baseline = 0x40,
        Horizontal_Mask = Left | Right | HCenter,
        Vertical_Mask = Top | Bottom | VCenter | Baseline
    };

    QmlGraphicsItem *item;
    AnchorLine anchorLine;

    bool operator==(const QmlGraphicsAnchorLine& other) const
    {
        return item == other.item && anchorLine == other.anchorLine;
    }
};

class QmlGraphicsAnchorsPrivate : public QObjectPrivate, public QmlGraphicsItemChangeListener
{
    Q_DECLARE_PUBLIC(QmlGraphicsAnchors)
public:
    QmlGraphicsAnchorsPrivate(QmlGraphicsItem *i)
      : updatingMe(false), updatingHorizontalAnchor(0),
        updatingVerticalAnchor(0), updatingFill(0), updatingCenterIn(0), item(i), usedAnchors(0), fill(0),
        centerIn(0), leftMargin(0), rightMargin(0), topMargin(0), bottomMargin(0),
        margins(0), vCenterOffset(0), hCenterOffset(0), baselineOffset(0),
        componentComplete(true)
    {
    }

    void init()
    {
    }

    void clearItem(QmlGraphicsItem *);

    void addDepend(QmlGraphicsItem *);
    void remDepend(QmlGraphicsItem *);
    bool isItemComplete() const;

    bool updatingMe;
    int updatingHorizontalAnchor;
    int updatingVerticalAnchor;
    int updatingFill;
    int updatingCenterIn;

    void setItemHeight(qreal);
    void setItemWidth(qreal);
    void setItemX(qreal);
    void setItemY(qreal);
    void setItemPos(const QPointF &);

    void updateOnComplete();
    void updateMe();

    // QmlGraphicsItemGeometryListener interface
    void itemGeometryChanged(QmlGraphicsItem *, const QRectF &, const QRectF &);
    QmlGraphicsAnchorsPrivate *anchorPrivate() { return this; }

    bool checkHValid() const;
    bool checkVValid() const;
    bool checkHAnchorValid(QmlGraphicsAnchorLine anchor) const;
    bool checkVAnchorValid(QmlGraphicsAnchorLine anchor) const;
    bool calcStretch(const QmlGraphicsAnchorLine &edge1, const QmlGraphicsAnchorLine &edge2, int offset1, int offset2, QmlGraphicsAnchorLine::AnchorLine line, int &stretch);

    void updateHorizontalAnchors();
    void updateVerticalAnchors();
    void fillChanged();
    void centerInChanged();

    QmlGraphicsItem *item;
    QmlGraphicsAnchors::UsedAnchors usedAnchors;

    QmlGraphicsItem *fill;
    QmlGraphicsItem *centerIn;

    QmlGraphicsAnchorLine left;
    QmlGraphicsAnchorLine right;
    QmlGraphicsAnchorLine top;
    QmlGraphicsAnchorLine bottom;
    QmlGraphicsAnchorLine vCenter;
    QmlGraphicsAnchorLine hCenter;
    QmlGraphicsAnchorLine baseline;

    qreal leftMargin;
    qreal rightMargin;
    qreal topMargin;
    qreal bottomMargin;
    qreal margins;
    qreal vCenterOffset;
    qreal hCenterOffset;
    qreal baselineOffset;

    bool componentComplete;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QmlGraphicsAnchorLine)

#endif
