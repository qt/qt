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

#ifndef QBBBPSEVENTFILTER_H
#define QBBBPSEVENTFILTER_H

#include <QObject>

struct bps_event_t;

QT_BEGIN_NAMESPACE

class QAbstractEventDispatcher;

class QBBBpsEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit QBBBpsEventFilter(QObject *parent = 0);
    ~QBBBpsEventFilter();

    void installOnEventDispatcher(QAbstractEventDispatcher *dispatcher);

private:
    static bool dispatcherEventFilter(void *message);
    bool bpsEventFilter(bps_event_t *event);
};

QT_END_NAMESPACE

#endif // QBBBPSEVENTFILTER_H
