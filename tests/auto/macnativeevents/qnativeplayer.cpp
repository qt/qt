/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include "qnativeplayer.h"

QNativePlayer::QNativePlayer()
{
    currIndex = -1;
    playbackMultiplier = 1.0;
    wait = false;
}

QNativePlayer::~QNativePlayer()
{
    for (int i=0; i<eventList.size(); i++)
        delete eventList.takeAt(i).second;
}

void QNativePlayer::sendNextEvent()
{
    QNativeEvent *e = eventList.at(currIndex).second;
    if (e)
        QNativeInput::sendNativeEvent(*e);
    waitNextEvent();
}

void QNativePlayer::waitNextEvent()
{
    if (++currIndex >= eventList.size()){
        emit done();
        stop();
        return;
    }

    int interval = eventList.at(currIndex).first;
    QTimer::singleShot(interval * playbackMultiplier, this, SLOT(sendNextEvent()));
}

void QNativePlayer::append(int waitMs, QNativeEvent *event)
{
    eventList.append(QPair<int, QNativeEvent *>(waitMs, event));
}

void QNativePlayer::play(Playback playback)
{
    waitNextEvent();

    wait = (playback == WaitUntilFinished);
    while (wait)
        QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
}

void QNativePlayer::stop()
{
    wait = false;
    QAbstractEventDispatcher::instance()->interrupt();
}

// ************************************************************************

QEventOutputList::QEventOutputList()
{
    wait = true;
}

QEventOutputList::~QEventOutputList()
{
    qDeleteAll(*this);
}

bool QEventOutputList::waitUntilEmpty(int maxEventWaitTime)
{
    int currSize = size();
    QTime time;
    time.restart();
    while (wait){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

        if (isEmpty()){
            return true;
        }
        else if (currSize == size()){
            if (time.elapsed() > maxEventWaitTime){
                return false;
            }
        }
        else{
            currSize = size();
            time.restart();
        }
    }
    return false;
}

void QEventOutputList::sleep(int sleepTime)
{
    QTime time;
    time.restart();
    while (time.elapsed() < sleepTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}
