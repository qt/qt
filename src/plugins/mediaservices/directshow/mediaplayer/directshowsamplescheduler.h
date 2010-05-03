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

#ifndef DIRECTSHOWSAMPLESCHEDULER_H
#define DIRECTSHOWSAMPLESCHEDULER_H

#include <QtCore/qmutex.h>
#include <QtCore/qobject.h>
#include <QtCore/qsemaphore.h>

#include <QtCore/private/qwineventnotifier_p.h>

#include <dshow.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class DirectShowTimedSample;

class DirectShowSampleScheduler : public QObject, public IMemInputPin
{
    Q_OBJECT
public:

    enum State
    {
        Stopped  = 0x00,
        Running  = 0x01,
        Paused   = 0x02,
        RunMask  = 0x03,
        Flushing = 0x04
    };

    DirectShowSampleScheduler(IUnknown *pin, QObject *parent = 0);
    ~DirectShowSampleScheduler();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IMemInputPin
    HRESULT STDMETHODCALLTYPE GetAllocator(IMemAllocator **ppAllocator);
    HRESULT STDMETHODCALLTYPE NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly);
    HRESULT STDMETHODCALLTYPE GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps);

    HRESULT STDMETHODCALLTYPE Receive(IMediaSample *pSample);
    HRESULT STDMETHODCALLTYPE ReceiveMultiple(IMediaSample **pSamples, long nSamples, long *nSamplesProcessed);
    HRESULT STDMETHODCALLTYPE ReceiveCanBlock();

    void run(REFERENCE_TIME startTime);
    void pause();
    void stop();
    void setFlushing(bool flushing);

    IReferenceClock *clock() const { return m_clock; }
    void setClock(IReferenceClock *clock);

    bool schedule(IMediaSample *sample);
    bool scheduleEndOfStream();

    IMediaSample *takeSample(bool *eos);

Q_SIGNALS:
    void sampleReady();

private:
    IUnknown *m_pin;
    IReferenceClock *m_clock;
    IMemAllocator *m_allocator;
    DirectShowTimedSample *m_head;
    DirectShowTimedSample *m_tail;
    int m_maximumSamples;
    int m_state;
    REFERENCE_TIME m_startTime;
    HANDLE m_timeoutEvent;
    QSemaphore m_semaphore;
    QMutex m_mutex;
    QWinEventNotifier m_eventNotifier;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
