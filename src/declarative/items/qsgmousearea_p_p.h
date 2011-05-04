// Commit: 57676c237992e0aa5a93a4e8fa66b3e7b90c2c90
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

#ifndef QSGMOUSEAREA_P_P_H
#define QSGMOUSEAREA_P_P_H

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

#include "qsgitem_p.h"

#include <QtGui/qgraphicssceneevent.h>
#include <QtCore/qbasictimer.h>

QT_BEGIN_NAMESPACE

class QSGMouseEvent;
class QSGMouseArea;
class QSGMouseAreaPrivate : public QSGItemPrivate
{
    Q_DECLARE_PUBLIC(QSGMouseArea)

public:
    QSGMouseAreaPrivate();
    ~QSGMouseAreaPrivate();
    void init();

    void saveEvent(QGraphicsSceneMouseEvent *event);
    enum PropagateType{
        Click,
        DoubleClick,
        PressAndHold
    };
    void propagate(QSGMouseEvent* event, PropagateType);
    bool propagateHelper(QSGMouseEvent*, QSGItem*,const QPointF &, PropagateType);

    bool isPressAndHoldConnected();
    bool isDoubleClickConnected();
    bool isClickConnected();

    bool absorb : 1;
    bool hovered : 1;
    bool pressed : 1;
    bool longPress : 1;
    bool moved : 1;
    bool dragX : 1;
    bool dragY : 1;
    bool stealMouse : 1;
    bool doubleClick : 1;
    bool preventStealing : 1;
    QSGDrag *drag;
    QPointF startScene;
    qreal startX;
    qreal startY;
    QPointF lastPos;
    QDeclarativeNullableValue<QPointF> lastScenePos;
    Qt::MouseButton lastButton;
    Qt::MouseButtons lastButtons;
    Qt::KeyboardModifiers lastModifiers;
    QBasicTimer pressAndHoldTimer;
};

QT_END_NAMESPACE

#endif // QSGMOUSEAREA_P_P_H
