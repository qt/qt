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

#ifndef QMLSTATE_P_H
#define QMLSTATE_P_H

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

#include "qmlstate_p.h"

#include "qmlanimation_p_p.h"
#include "qmltransitionmanager_p_p.h"

#include <qmlguard_p.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlSimpleAction
{
public:
    enum State { StartState, EndState };
    QmlSimpleAction(const QmlAction &a, State state = StartState) 
    {
        property = a.property;
        specifiedObject = a.specifiedObject;
        specifiedProperty = a.specifiedProperty;
        event = a.event;
        if (state == StartState) {
            value = a.fromValue;
            binding = property.binding();
            reverseEvent = true;
        } else {
            value = a.toValue;
            binding = a.toBinding;
            reverseEvent = false;
        }
    }

    QmlMetaProperty property;
    QVariant value;
    QmlAbstractBinding *binding;
    QObject *specifiedObject;
    QString specifiedProperty;
    QmlActionEvent *event;
    bool reverseEvent;
};

class QmlStatePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlState)

public:
    QmlStatePrivate()
    : when(0), inState(false), group(0) {}

    typedef QList<QmlSimpleAction> SimpleActionList;

    QString name;
    QmlBinding *when;

    class OperationList;
    struct OperationGuard : public QmlGuard<QmlStateOperation>
    {
        OperationGuard(QObject *obj, OperationList *l) : list(l) { (QmlGuard<QObject>&)*this = obj; }
        OperationList *list;
        void objectDestroyed(QmlStateOperation *) {
            // we assume priv will always be destroyed after objectDestroyed calls
            list->removeOne(*this);
        }
    };

    class OperationList : public QList<OperationGuard>, public QmlList<QmlStateOperation*>
    {
    public:
        virtual void append(QmlStateOperation* v) { QList<OperationGuard>::append(OperationGuard(v, this)); }
        virtual void insert(int i, QmlStateOperation* v) { QList<OperationGuard>::insert(i, OperationGuard(v, this)); }
        virtual void clear() { QList<OperationGuard>::clear(); }
        virtual QmlStateOperation* at(int i) const { return QList<OperationGuard>::at(i); }
        virtual void removeAt(int i) { QList<OperationGuard>::removeAt(i); }
        virtual int count() const { return QList<OperationGuard>::count(); }
    };
    OperationList operations;

    QmlTransitionManager transitionManager;

    SimpleActionList revertList;
    QList<QmlMetaProperty> reverting;
    QString extends;
    mutable bool inState;
    QmlStateGroup *group;

    QmlStateOperation::ActionList generateActionList(QmlStateGroup *) const;
    void complete();
};

QT_END_NAMESPACE

#endif // QMLSTATE_P_H
