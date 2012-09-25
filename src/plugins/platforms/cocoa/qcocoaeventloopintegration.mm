/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcocoaeventloopintegration.h"

#import <Cocoa/Cocoa.h>

#include "qcocoaautoreleasepool.h"

#include <QtCore/QElapsedTimer>

#include <QDebug>
#include <QApplication>

void wakeupCallback ( void * ) {
    QPlatformEventLoopIntegration::processEvents();
}

void timerCallback( CFRunLoopTimerRef timer, void *info)
{
    QPlatformEventLoopIntegration::processEvents();
    QCocoaEventLoopIntegration *eventLoopIntegration =
            static_cast<QCocoaEventLoopIntegration *>(info);
    qint64 nextTime = eventLoopIntegration->nextTimerEvent();
    CFAbsoluteTime nexttime = CFAbsoluteTimeGetCurrent();
    nexttime = nexttime + (double(nextTime)/1000);
    CFRunLoopTimerSetNextFireDate(timer,nexttime);
}

QCocoaEventLoopIntegration::QCocoaEventLoopIntegration() :
    QPlatformEventLoopIntegration()
{
    [NSApplication sharedApplication];
    m_sourceContext.version = 0;
    m_sourceContext.info = this;
    m_sourceContext.retain = 0;
    m_sourceContext.release = 0;
    m_sourceContext.copyDescription = 0;
    m_sourceContext.equal = 0;
    m_sourceContext.hash = 0;
    m_sourceContext.schedule = 0;
    m_sourceContext.cancel = 0;
    m_sourceContext.perform = wakeupCallback;

    m_source = CFRunLoopSourceCreate(0,0,&m_sourceContext);
    CFRunLoopAddSource(CFRunLoopGetMain(),m_source,kCFRunLoopCommonModes);

    m_timerContext.version = 0;
    m_timerContext.info = this;
    m_timerContext.retain = 0;
    m_timerContext.release = 0;
    m_timerContext.copyDescription = 0;
    CFAbsoluteTime fireDate = CFAbsoluteTimeGetCurrent ();
    CFTimeInterval interval = 30;

    CFRunLoopTimerRef m_timerSource = CFRunLoopTimerCreate(0,fireDate,interval,0,0,timerCallback,&m_timerContext);
    CFRunLoopAddTimer(CFRunLoopGetMain(),m_timerSource,kCFRunLoopCommonModes);
}

void QCocoaEventLoopIntegration::startEventLoop()
{
    [[NSApplication sharedApplication] run];
}

void QCocoaEventLoopIntegration::quitEventLoop()
{
    [[NSApplication sharedApplication] terminate:nil];
}

void QCocoaEventLoopIntegration::qtNeedsToProcessEvents()
{
    CFRunLoopSourceSignal(m_source);
}

