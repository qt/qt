/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
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

#include "qcocoaeventloopintegration.h"

#import <Cocoa/Cocoa.h>

#include "qcocoaautoreleasepool.h"

#include <QtCore/QElapsedTimer>

#include <QDebug>

@implementation OurApplication

- (void) run
{
    QCocoaAutoReleasePool pool;
    [self finishLaunching];

    shouldKeepRunning = YES;
}

- (void) processEvents : (int) msec
{
    QCocoaAutoReleasePool pool;
    Q_UNUSED(pool);

    QElapsedTimer timer;
    timer.start();

    NSTimeInterval seconds = NSTimeInterval(msec)/1000;
    id untilDate = [NSDate dateWithTimeIntervalSinceNow:seconds];
    bool continueLooping = true;
    while ((timer.elapsed() < (msec-1)) && continueLooping) {
        NSEvent *event =
            [self nextEventMatchingMask:NSAnyEventMask
            untilDate:untilDate
            inMode:NSDefaultRunLoopMode
            dequeue:YES];
        if ([event type] == NSApplicationDefined
                && [event subtype] ==  QCocoaEventLoopIntegration::wakeupEventId) {
            continueLooping = false;
        } else {
            [self sendEvent:event];
        }

    }
    [self updateWindows];
}

@end

int QCocoaEventLoopIntegration::wakeupEventId = SHRT_MAX;

QCocoaEventLoopIntegration::QCocoaEventLoopIntegration() :
    QPlatformEventLoopIntegration()
{
    app = (OurApplication *)[OurApplication sharedApplication];
    [app run];
}

void QCocoaEventLoopIntegration::processEvents(qint64 msec)
{
    [app processEvents:msec];
}

void QCocoaEventLoopIntegration::wakeup()
{
    QCocoaAutoReleasePool pool;
    Q_UNUSED(pool);

    NSPoint p = NSMakePoint(0,0);
    NSWindow *nswin = [app keyWindow];
    double timestamp = (double)(AbsoluteToDuration(UpTime())) / 1000.0;
    NSEvent *event = [NSEvent
            otherEventWithType:NSApplicationDefined
            location:NSZeroPoint
            modifierFlags:0
            timestamp: timestamp
            windowNumber:[nswin windowNumber]
            context:0
            subtype:QCocoaEventLoopIntegration::wakeupEventId
            data1:0
            data2:0
            ];
    [app postEvent:event atStart:NO];

}

