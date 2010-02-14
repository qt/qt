/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLGRAPHICSLAYOUTS_P_H
#define QMLGRAPHICSLAYOUTS_P_H

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

#include "qmlgraphicspositioners_p.h"

#include "qmlgraphicsitem_p.h"

#include <qmlstate_p.h>
#include <qmltransitionmanager_p_p.h>
#include <qmlstateoperations_p.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QmlGraphicsBasePositionerPrivate : public QmlGraphicsItemPrivate, public QmlGraphicsItemChangeListener
{
    Q_DECLARE_PUBLIC(QmlGraphicsBasePositioner)

public:
    QmlGraphicsBasePositionerPrivate()
        : spacing(0), type(QmlGraphicsBasePositioner::None), moveTransition(0), addTransition(0),
          queuedPositioning(false)
    {
    }

    void init(QmlGraphicsBasePositioner::PositionerType at)
    {
        type = at;
    }

    int spacing;
    QmlGraphicsBasePositioner::PositionerType type;
    QmlTransition *moveTransition;
    QmlTransition *addTransition;
    QmlStateOperation::ActionList addActions;
    QmlStateOperation::ActionList moveActions;
    QmlTransitionManager addTransitionManager;
    QmlTransitionManager moveTransitionManager;

    void watchChanges(QmlGraphicsItem *other);
    void unwatchChanges(QmlGraphicsItem* other);
    bool queuedPositioning;

    virtual void itemSiblingOrderChanged(QmlGraphicsItem* other)
    {
        Q_Q(QmlGraphicsBasePositioner);
        Q_UNUSED(other);
        if(!queuedPositioning){
            //Delay is due to many children often being reordered at once
            //And we only want to reposition them all once
            QTimer::singleShot(0,q,SLOT(prePositioning()));
            queuedPositioning = true;
        }
    }

    void itemGeometryChanged(QmlGraphicsItem *, const QRectF &newGeometry, const QRectF &oldGeometry)
    {
        Q_Q(QmlGraphicsBasePositioner);
        if (newGeometry.size() != oldGeometry.size())
            q->prePositioning();
    }
    virtual void itemVisibilityChanged(QmlGraphicsItem *)
    {
        Q_Q(QmlGraphicsBasePositioner);
        q->prePositioning();
    }
    virtual void itemOpacityChanged(QmlGraphicsItem *)
    {
        Q_Q(QmlGraphicsBasePositioner);
        q->prePositioning();
    }

    void itemDestroyed(QmlGraphicsItem *item)
    {
        Q_Q(QmlGraphicsBasePositioner);
        q->positionedItems.removeOne(QmlGraphicsBasePositioner::PositionedItem(item));
    }
};

QT_END_NAMESPACE
#endif
