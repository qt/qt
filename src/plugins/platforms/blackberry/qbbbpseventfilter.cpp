/****************************************************************************
**
** Copyright (C) 2012 Research In Motion <blackberry-qt@qnx.com>
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
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

//#define QBBBPSEVENTFILTER_DEBUG

#include "qbbbpseventfilter.h"
#include "qbbnavigatoreventhandler.h"
#include "qbbscreen.h"
#include "qbbscreeneventhandler.h"
#include "qbbvirtualkeyboardbps.h"

#include <QCoreApplication>
#include <QAbstractEventDispatcher>
#include <QDebug>

#include <bps/event.h>
#include <bps/navigator.h>
#include <bps/screen.h>

QT_BEGIN_NAMESPACE

static QBBBpsEventFilter *sInstance;

QBBBpsEventFilter::QBBBpsEventFilter(QBBNavigatorEventHandler *navigatorEventHandler,
                                     QBBScreenEventHandler *screenEventHandler,
                                     QBBVirtualKeyboardBps *virtualKeyboard, QObject *parent)
    : QObject(parent)
    , mNavigatorEventHandler(navigatorEventHandler)
    , mScreenEventHandler(screenEventHandler)
    , mVirtualKeyboard(virtualKeyboard)
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
    if (navigator_request_events(0) != BPS_SUCCESS)
        qWarning("QBB: failed to register for navigator events");

    QAbstractEventDispatcher::EventFilter previousEventFilter = dispatcher->setEventFilter(dispatcherEventFilter);

    // the QPA plugin in created in the QApplication constructor which indirectly also creates
    // the event dispatcher so we are the first event filter.
    // assert on that just in case somebody adds another event filter
    // in the QBBIntegration constructor instead of adding a new section in here
    Q_ASSERT(previousEventFilter == 0);
    Q_UNUSED(previousEventFilter);
}

void QBBBpsEventFilter::registerForScreenEvents(QBBScreen *screen)
{
    if (!mScreenEventHandler) {
        qWarning("QBB: trying to register for screen events, but no handler provided");
        return;
    }

    int attached;
    if (screen_get_display_property_iv(screen->nativeDisplay(), SCREEN_PROPERTY_ATTACHED, &attached) != BPS_SUCCESS) {
        qWarning() << "QBB: unable to query display attachment";
        return;
    }

    if (!attached) {
#if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << Q_FUNC_INFO << "skipping event registration for non-attached screen";
#endif
        return;
    }

    if (screen_request_events(screen->nativeContext()) != BPS_SUCCESS)
        qWarning("QBB: failed to register for screen events on screen %p", screen->nativeContext());
}

void QBBBpsEventFilter::unregisterForScreenEvents(QBBScreen *screen)
{
    if (!mScreenEventHandler) {
        qWarning("QBB: trying to unregister for screen events, but no handler provided");
        return;
    }

    if (screen_stop_events(screen->nativeContext()) != BPS_SUCCESS)
        qWarning("QBB: failed to unregister for screen events on screen %p", screen->nativeContext());
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
    const int eventDomain = bps_event_get_domain(event);

#if defined(QBBBPSEVENTFILTER_DEBUG)
    qDebug() << Q_FUNC_INFO << "event=" << event << "domain=" << eventDomain;
#endif

    if (eventDomain == screen_get_domain()) {
        if (!mScreenEventHandler) {
            qWarning("QBB: registered for screen events, but no handler provided");
            return false;
        }

        screen_event_t screenEvent = screen_event_get_event(event);
        return mScreenEventHandler->handleEvent(screenEvent);
    }

    if (eventDomain == navigator_get_domain())
        return handleNavigatorEvent(event);

    if (mVirtualKeyboard->handleEvent(event))
        return true;

    return false;
}

bool QBBBpsEventFilter::handleNavigatorEvent(bps_event_t *event)
{
    switch (bps_event_get_code(event)) {
    case NAVIGATOR_ORIENTATION_CHECK: {
        const int angle = navigator_event_get_orientation_angle(event);

        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator ORIENTATION CHECK event. angle=" << angle;
        #endif

        const bool result = mNavigatorEventHandler->handleOrientationCheck(angle);

        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator ORIENTATION CHECK event. result=" << result;
        #endif

        // reply to navigator whether orientation is acceptable
        navigator_orientation_check_response(event, result);
        break;
    }

    case NAVIGATOR_ORIENTATION: {
        const int angle = navigator_event_get_orientation_angle(event);

        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator ORIENTATION event. angle=" << angle;
        #endif

        mNavigatorEventHandler->handleOrientationChange(angle);

        navigator_done_orientation(event);
        break;
    }

    case NAVIGATOR_SWIPE_DOWN:
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator SWIPE DOWN event";
        #endif

        mNavigatorEventHandler->handleSwipeDown();
        break;

    case NAVIGATOR_EXIT:
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator EXIT event";
        #endif

        mNavigatorEventHandler->handleExit();
        break;

    case NAVIGATOR_WINDOW_STATE: {
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << Q_FUNC_INFO << "WINDOW STATE event";
        #endif

        const navigator_window_state_t state = navigator_event_get_window_state(event);
        const QByteArray id(navigator_event_get_groupid(event));

        switch (state) {
        case NAVIGATOR_WINDOW_FULLSCREEN:
            mNavigatorEventHandler->handleWindowGroupStateChanged(id, Qt::WindowFullScreen);
            break;
        case NAVIGATOR_WINDOW_THUMBNAIL:
            mNavigatorEventHandler->handleWindowGroupStateChanged(id, Qt::WindowMinimized);
            break;
        case NAVIGATOR_WINDOW_INVISIBLE:
            mNavigatorEventHandler->handleWindowGroupDeactivated(id);
            break;
        }

        break;
    }

    case NAVIGATOR_WINDOW_ACTIVE: {
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator WINDOW ACTIVE event";
        #endif

        const QByteArray id(navigator_event_get_groupid(event));
        mNavigatorEventHandler->handleWindowGroupActivated(id);
        break;
    }

    case NAVIGATOR_WINDOW_INACTIVE: {
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Navigator WINDOW INACTIVE event";
        #endif

        const QByteArray id(navigator_event_get_groupid(event));
        mNavigatorEventHandler->handleWindowGroupDeactivated(id);
        break;
    }

    case NAVIGATOR_LOW_MEMORY:
        qWarning() << "QApplication based process" << QCoreApplication::applicationPid()
                   << "received \"NAVIGATOR_LOW_MEMORY\" event";
        return false;

    default:
        #if defined(QBBBPSEVENTFILTER_DEBUG)
        qDebug() << "QBB: Unhandled navigator event. code=" << bps_event_get_code(event);
        #endif
        return false;
    }

    return true;
}

QT_END_NAMESPACE
