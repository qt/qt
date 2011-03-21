// Commit: 2c7cab4172f1acc86fd49345a2847417e162f2c3
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

#ifndef QSGPOSITIONERS_P_P_H
#define QSGPOSITIONERS_P_P_H

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

#include "qsgpositioners_p.h"
#include "qsgimplicitsizeitem_p_p.h"

#include <private/qdeclarativestate_p.h>
#include <private/qdeclarativetransitionmanager_p_p.h>
#include <private/qdeclarativestateoperations_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtCore/qtimer.h>

QT_BEGIN_NAMESPACE

class QSGBasePositionerPrivate : public QSGImplicitSizeItemPrivate, public QSGItemChangeListener
{
    Q_DECLARE_PUBLIC(QSGBasePositioner)

public:
    QSGBasePositionerPrivate()
        : spacing(0), type(QSGBasePositioner::None)
        , moveTransition(0), addTransition(0), queuedPositioning(false)
        , doingPositioning(false), anchorConflict(false), layoutDirection(Qt::LeftToRight)
    {
    }

    void init(QSGBasePositioner::PositionerType at)
    {
        type = at;
    }

    int spacing;

    QSGBasePositioner::PositionerType type;
    QDeclarativeTransition *moveTransition;
    QDeclarativeTransition *addTransition;
    QDeclarativeStateOperation::ActionList addActions;
    QDeclarativeStateOperation::ActionList moveActions;
    QDeclarativeTransitionManager addTransitionManager;
    QDeclarativeTransitionManager moveTransitionManager;

    void watchChanges(QSGItem *other);
    void unwatchChanges(QSGItem* other);
    bool queuedPositioning : 1;
    bool doingPositioning : 1;
    bool anchorConflict : 1;

    Qt::LayoutDirection layoutDirection;

    void schedulePositioning()
    {
        Q_Q(QSGBasePositioner);
        if(!queuedPositioning){
            QTimer::singleShot(0,q,SLOT(prePositioning()));
            queuedPositioning = true;
        }
    }

    void mirrorChange() {
        Q_Q(QSGBasePositioner);
        if (type != QSGBasePositioner::Vertical)
            q->prePositioning();
    }
    bool isLeftToRight() const {
        if (type == QSGBasePositioner::Vertical)
            return true;
        else
            return effectiveLayoutMirror ? layoutDirection == Qt::RightToLeft : layoutDirection == Qt::LeftToRight;
    }

    virtual void itemSiblingOrderChanged(QSGItem* other)
    {
        Q_UNUSED(other);
        //Delay is due to many children often being reordered at once
        //And we only want to reposition them all once
        schedulePositioning();
    }

    void itemGeometryChanged(QSGItem *, const QRectF &newGeometry, const QRectF &oldGeometry)
    {
        Q_Q(QSGBasePositioner);
        if (newGeometry.size() != oldGeometry.size())
            q->prePositioning();
    }

    virtual void itemVisibilityChanged(QSGItem *)
    {
        schedulePositioning();
    }
    virtual void itemOpacityChanged(QSGItem *)
    {
        Q_Q(QSGBasePositioner);
        q->prePositioning();
    }

    void itemDestroyed(QSGItem *item)
    {
        Q_Q(QSGBasePositioner);
        q->positionedItems.removeOne(QSGBasePositioner::PositionedItem(item));
    }

    static Qt::LayoutDirection getLayoutDirection(const QSGBasePositioner *positioner)
    {
        return positioner->d_func()->layoutDirection;
    }

    static Qt::LayoutDirection getEffectiveLayoutDirection(const QSGBasePositioner *positioner)
    {
        if (positioner->d_func()->effectiveLayoutMirror)
            return positioner->d_func()->layoutDirection == Qt::RightToLeft ? Qt::LeftToRight : Qt::RightToLeft;
        else
            return positioner->d_func()->layoutDirection;
    }
};

QT_END_NAMESPACE

#endif // QSGPOSITIONERS_P_P_H
