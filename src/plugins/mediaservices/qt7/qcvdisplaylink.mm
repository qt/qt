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

#include "qcvdisplaylink.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdebug.h>


QT_USE_NAMESPACE

static CVReturn CVDisplayLinkCallback(CVDisplayLinkRef displayLink,
                                 const CVTimeStamp *inNow,
                                 const CVTimeStamp *inOutputTime,
                                 CVOptionFlags flagsIn,
                                 CVOptionFlags *flagsOut,
                                 void *displayLinkContext)
{
    Q_UNUSED(displayLink);
    Q_UNUSED(inNow);
    Q_UNUSED(flagsIn);
    Q_UNUSED(flagsOut);

    QCvDisplayLink *link = (QCvDisplayLink *)displayLinkContext;

    link->displayLinkEvent(inOutputTime);
    return kCVReturnSuccess;
}


QCvDisplayLink::QCvDisplayLink(QObject *parent)
    :QObject(parent),
    m_pendingDisplayLinkEvent(false),
    m_isActive(false)
{
    // create display link for the main display
    CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &m_displayLink);
    if (m_displayLink) {
        // set the current display of a display link.
        CVDisplayLinkSetCurrentCGDisplay(m_displayLink, kCGDirectMainDisplay);

        // set the renderer output callback function
        CVDisplayLinkSetOutputCallback(m_displayLink, &CVDisplayLinkCallback, this);
    }
}

QCvDisplayLink::~QCvDisplayLink()
{
    if (m_displayLink) {
        CVDisplayLinkStop(m_displayLink);
        CVDisplayLinkRelease(m_displayLink);
        m_displayLink = NULL;
    }
}

bool QCvDisplayLink::isValid()
{
    return m_displayLink != 0;
}

bool QCvDisplayLink::isActive() const
{
    return m_isActive;
}

void QCvDisplayLink::start()
{
     if (m_displayLink && !m_isActive) {
        CVDisplayLinkStart(m_displayLink);
        m_isActive = true;
     }
}

void QCvDisplayLink::stop()
{
    if (m_displayLink && m_isActive) {
        CVDisplayLinkStop(m_displayLink);
        m_isActive = false;
    }
}

void QCvDisplayLink::displayLinkEvent(const CVTimeStamp *ts)
{
    // This function is called from a
    // thread != gui thread. So we post the event.
    // But we need to make sure that we don't post faster
    // than the event loop can eat:
    m_displayLinkMutex.lock();
    bool pending = m_pendingDisplayLinkEvent;
    m_pendingDisplayLinkEvent = true;
    m_frameTimeStamp = *ts;
    m_displayLinkMutex.unlock();

    if (!pending)
        qApp->postEvent(this, new QEvent(QEvent::User), Qt::HighEventPriority);
}

bool QCvDisplayLink::event(QEvent *event)
{
    switch (event->type()){
        case QEvent::User:  {
                m_displayLinkMutex.lock();
                m_pendingDisplayLinkEvent = false;
                CVTimeStamp ts = m_frameTimeStamp;
                m_displayLinkMutex.unlock();

                emit tick(ts);

                return false;
            }
            break;
        default:
            break;
    }
    return QObject::event(event);
}

QT_END_NAMESPACE

#include "moc_qcvdisplaylink.cpp"

