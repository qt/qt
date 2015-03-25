/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qeventdispatcher_blackberry_qpa_p.h"

#include "qapplication_p.h"

#include <QWindowSystemInterface>

QT_BEGIN_NAMESPACE

static bool sendWindowSystemEvents(QAbstractEventDispatcher *eventDispatcher, QEventLoop::ProcessEventsFlags flags)
{
    int nevents = 0;

    // handle gui and posted events
    QCoreApplication::sendPostedEvents();

    while (true) {
        QWindowSystemInterfacePrivate::WindowSystemEvent *event;
        if (!(flags & QEventLoop::ExcludeUserInputEvents)
            && QWindowSystemInterfacePrivate::windowSystemEventsQueued() > 0) {
            // process a pending user input event
            event = QWindowSystemInterfacePrivate::getWindowSystemEvent();
            if (!event)
                break;
        } else {
            break;
        }

        // in contrast to the common QPA version we don't put window system events
        // into the event filter here, instead native bps events are filtered in
        // QEventDispatcherBlackberry::select()

        nevents++;

        QApplicationPrivate::processWindowSystemEvent(event);
        delete event;
    }

    return (nevents > 0);
}

QEventDispatcherBlackberryQPA::QEventDispatcherBlackberryQPA(QObject *parent)
    : QEventDispatcherBlackberry(parent)
{
}

QEventDispatcherBlackberryQPA::~QEventDispatcherBlackberryQPA()
{
}

bool QEventDispatcherBlackberryQPA::processEvents(QEventLoop::ProcessEventsFlags flags)
{
    bool didSendEvents = sendWindowSystemEvents(this, flags);

    if (QEventDispatcherBlackberry::processEvents(flags))
        return true;

    return didSendEvents;
}

bool QEventDispatcherBlackberryQPA::hasPendingEvents()
{
    return QEventDispatcherBlackberry::hasPendingEvents() || QWindowSystemInterfacePrivate::windowSystemEventsQueued();
}

void QEventDispatcherBlackberryQPA::flush()
{
    if (qApp)
        qApp->sendPostedEvents();
}

QT_END_NAMESPACE
