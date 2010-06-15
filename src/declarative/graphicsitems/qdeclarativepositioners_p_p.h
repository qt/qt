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

#ifndef QDECLARATIVELAYOUTS_P_H
#define QDECLARATIVELAYOUTS_P_H

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

#include "private/qdeclarativepositioners_p.h"

#include "private/qdeclarativeitem_p.h"

#include <qdeclarativestate_p.h>
#include <qdeclarativetransitionmanager_p_p.h>
#include <qdeclarativestateoperations_p.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QDeclarativeBasePositionerPrivate : public QDeclarativeItemPrivate, public QDeclarativeItemChangeListener
{
    Q_DECLARE_PUBLIC(QDeclarativeBasePositioner)

public:
    QDeclarativeBasePositionerPrivate()
        : spacing(0), type(QDeclarativeBasePositioner::None)
        , moveTransition(0), addTransition(0), queuedPositioning(false)
        , doingPositioning(false), anchorConflict(false)
    {
    }

    void init(QDeclarativeBasePositioner::PositionerType at)
    {
        type = at;
    }

    int spacing;

    QDeclarativeBasePositioner::PositionerType type;
    QDeclarativeTransition *moveTransition;
    QDeclarativeTransition *addTransition;
    QDeclarativeStateOperation::ActionList addActions;
    QDeclarativeStateOperation::ActionList moveActions;
    QDeclarativeTransitionManager addTransitionManager;
    QDeclarativeTransitionManager moveTransitionManager;

    void watchChanges(QDeclarativeItem *other);
    void unwatchChanges(QDeclarativeItem* other);
    bool queuedPositioning : 1;
    bool doingPositioning : 1;
    bool anchorConflict : 1;

    void schedulePositioning()
    {
        Q_Q(QDeclarativeBasePositioner);
        if(!queuedPositioning){
            QTimer::singleShot(0,q,SLOT(prePositioning()));
            queuedPositioning = true;
        }
    }

    virtual void itemSiblingOrderChanged(QDeclarativeItem* other)
    {
        Q_UNUSED(other);
        //Delay is due to many children often being reordered at once
        //And we only want to reposition them all once
        schedulePositioning();
    }

    void itemGeometryChanged(QDeclarativeItem *, const QRectF &newGeometry, const QRectF &oldGeometry)
    {
        Q_Q(QDeclarativeBasePositioner);
        if (newGeometry.size() != oldGeometry.size())
            q->prePositioning();
    }
    virtual void itemVisibilityChanged(QDeclarativeItem *)
    {
        schedulePositioning();
    }
    virtual void itemOpacityChanged(QDeclarativeItem *)
    {
        Q_Q(QDeclarativeBasePositioner);
        q->prePositioning();
    }

    void itemDestroyed(QDeclarativeItem *item)
    {
        Q_Q(QDeclarativeBasePositioner);
        q->positionedItems.removeOne(QDeclarativeBasePositioner::PositionedItem(item));
    }
};

QT_END_NAMESPACE
#endif
