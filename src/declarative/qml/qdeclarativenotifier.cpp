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

#include "private/qdeclarativenotifier_p.h"
#include "private/qdeclarativeproperty_p.h"

QT_BEGIN_NAMESPACE

void QDeclarativeNotifier::emitNotify(QDeclarativeNotifierEndpoint *endpoint)
{
    QDeclarativeNotifierEndpoint::Notifier *n = endpoint->asNotifier();

    QDeclarativeNotifierEndpoint **oldDisconnected = n->disconnected;
    n->disconnected = &endpoint;

    if (n->next)
        emitNotify(n->next);

    if (endpoint) {
        void *args[] = { 0 };

        QMetaObject::metacall(endpoint->target, QMetaObject::InvokeMetaMethod, 
                              endpoint->targetMethod, args);

        if (endpoint)
            endpoint->asNotifier()->disconnected = oldDisconnected;
    } 

    if (oldDisconnected) *oldDisconnected = endpoint;
}

void QDeclarativeNotifierEndpoint::connect(QObject *source, int sourceSignal)
{
    Signal *s = toSignal();
    
    if (s->source == source && s->sourceSignal == sourceSignal)
        return;

    disconnect();

    QDeclarativePropertyPrivate::connect(source, sourceSignal, target, targetMethod);

    s->source = source;
    s->sourceSignal = sourceSignal;
}

void QDeclarativeNotifierEndpoint::copyAndClear(QDeclarativeNotifierEndpoint &other)
{
    other.disconnect();

    other.target = target;
    other.targetMethod = targetMethod;

    if (!isConnected())
        return;

    if (SignalType == type) {
        Signal *other_s = other.toSignal();
        Signal *s = asSignal();

        other_s->source = s->source;
        other_s->sourceSignal = s->sourceSignal;
        s->source = 0;
    } else if(NotifierType == type) {
        Notifier *other_n = other.toNotifier();
        Notifier *n = asNotifier();

        other_n->notifier = n->notifier;
        other_n->disconnected = n->disconnected;
        if (other_n->disconnected) *other_n->disconnected = &other;

        if (n->next) {
            other_n->next = n->next;
            n->next->asNotifier()->prev = &other_n->next;
        }
        other_n->prev = n->prev;
        *other_n->prev = &other;

        n->prev = 0;
        n->next = 0;
        n->disconnected = 0;
        n->notifier = 0;
    } 
}


QT_END_NAMESPACE

