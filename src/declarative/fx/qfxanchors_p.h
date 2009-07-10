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

#ifndef QFXANCHORS_P_H
#define QFXANCHORS_P_H

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

#include "qfxanchors.h"
#include "private/qobject_p.h"

QT_BEGIN_NAMESPACE

class QFxAnchorsPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QFxAnchors)
public:
    QFxAnchorsPrivate()
      : updatingMe(false), updatingHorizontalAnchor(0),
        updatingVerticalAnchor(0), item(0), usedAnchors(0), fill(0),
        centeredIn(0), leftMargin(0), rightMargin(0), topMargin(0), 
        bottomMargin(0), vCenterOffset(0), hCenterOffset(0)
    {
    }

    void init()
    {
    }

    void clearItem(QFxItem *);

    void addDepend(QFxItem *);
    void remDepend(QFxItem *);
    bool isItemComplete() const;

    bool updatingMe;
    int updatingHorizontalAnchor;
    int updatingVerticalAnchor;

    void setItemHeight(qreal);
    void setItemWidth(qreal);
    void setItemX(qreal);
    void setItemY(qreal);
    void setItemPos(const QPointF &);

    void updateOnComplete();
    void updateMe();
    void update(QFxItem *, const QRectF &, const QRectF &);

    bool checkHValid() const;
    bool checkVValid() const;
    bool checkHAnchorValid(QFxAnchorLine anchor) const;
    bool checkVAnchorValid(QFxAnchorLine anchor) const;
    bool calcStretch(const QFxAnchorLine &edge1, const QFxAnchorLine &edge2, int offset1, int offset2, QFxAnchorLine::AnchorLine line, int &stretch);

    void updateHorizontalAnchors();
    void updateVerticalAnchors();
    void fillChanged();
    void centeredInChanged();

    QFxItem *item;
    QFxAnchors::UsedAnchors usedAnchors;

    QFxItem *fill;
    QFxItem *centeredIn;

    QFxAnchorLine left;
    QFxAnchorLine right;
    QFxAnchorLine top;
    QFxAnchorLine bottom;
    QFxAnchorLine vCenter;
    QFxAnchorLine hCenter;
    QFxAnchorLine baseline;

    qreal leftMargin;
    qreal rightMargin;
    qreal topMargin;
    qreal bottomMargin;
    qreal vCenterOffset;
    qreal hCenterOffset;

};

QT_END_NAMESPACE
#endif
