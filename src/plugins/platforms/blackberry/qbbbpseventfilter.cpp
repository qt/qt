/****************************************************************************
**
** Copyright (C) 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

//#define QBBBPSEVENTFILTER_DEBUG

#include "qbbbpseventfilter.h"

#include <QAbstractEventDispatcher>
#include <QDebug>

#include <bps/event.h>

QT_BEGIN_NAMESPACE

static QBBBpsEventFilter *sInstance;

QBBBpsEventFilter::QBBBpsEventFilter(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(sInstance == 0);

    sInstance = this;
}

QBBBpsEventFilter::~QBBBpsEventFilter()
{
    Q_ASSERT(sInstance == this);

    sInstance = 0;
}

void QBBBpsEventFilter::installOnEventDispatcher(QAbstractEventDispatcher *dispatcher)
{
#if defined(QBBBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO << "dispatcher=" << dispatcher;
#endif

    QAbstractEventDispatcher::EventFilter previousEventFilter = dispatcher->setEventFilter(dispatcherEventFilter);

    // the QPA plugin in created in the QApplication constructor which indirectly also creates
    // the event dispatcher so we are the first event filter.
    // assert on that just in case somebody adds another event filter
    // in the QBBIntegration constructor instead of adding a new section in here
    Q_ASSERT(previousEventFilter == 0);
    Q_UNUSED(previousEventFilter);
}

bool QBBBpsEventFilter::dispatcherEventFilter(void *message)
{
#if defined(QBBBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO;
#endif
    if (sInstance == 0)
        return false;

    bps_event_t *event = static_cast<bps_event_t *>(message);
    return sInstance->bpsEventFilter(event);
}

bool QBBBpsEventFilter::bpsEventFilter(bps_event_t *event)
{
#if defined(QBBBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO << "event=" << event << "domain=" << bps_event_get_domain(event);
#else
    Q_UNUSED(event);
#endif

    return false;
}

QT_END_NAMESPACE
