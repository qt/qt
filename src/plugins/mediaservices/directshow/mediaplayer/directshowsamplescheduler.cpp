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

#include "directshowsamplescheduler.h"

#include <QtCore/qcoreevent.h>


QT_BEGIN_NAMESPACE

class DirectShowTimedSample
{
public:
    DirectShowTimedSample(IMediaSample *sample)
        : m_next(0)
        , m_sample(sample)
        , m_cookie(0)
        , m_lastSample(false)
    {
        m_sample->AddRef();
    }

    ~DirectShowTimedSample()
    {
        m_sample->Release();
    }

    IMediaSample *sample() const { return m_sample; }

    DirectShowTimedSample *nextSample() const { return m_next; }
    void setNextSample(DirectShowTimedSample *sample) { Q_ASSERT(!m_next); m_next = sample; }

    DirectShowTimedSample *remove() {
        DirectShowTimedSample *next = m_next; delete this; return next; }

    bool schedule(IReferenceClock *clock, REFERENCE_TIME startTime, HANDLE handle);
    void unschedule(IReferenceClock *clock);

    bool isReady(IReferenceClock *clock) const;

    bool isLast() const { return m_lastSample; }
    void setLast() { m_lastSample = true; }

private:
    DirectShowTimedSample *m_next;
    IMediaSample *m_sample;
    DWORD_PTR m_cookie;
    bool m_lastSample;
};

bool DirectShowTimedSample::schedule(
        IReferenceClock *clock, REFERENCE_TIME startTime, HANDLE handle)
{
    REFERENCE_TIME sampleStartTime;
    REFERENCE_TIME sampleEndTime;
    if (m_sample->GetTime(&sampleStartTime, &sampleEndTime) == S_OK) {
        if (clock->AdviseTime(
                startTime, sampleStartTime, reinterpret_cast<HEVENT>(handle), &m_cookie) == S_OK) {
            return true;
        }
    }
    return false;
}

void DirectShowTimedSample::unschedule(IReferenceClock *clock)
{
    clock->Unadvise(m_cookie);
}

bool DirectShowTimedSample::isReady(IReferenceClock *clock) const
{
    REFERENCE_TIME sampleStartTime;
    REFERENCE_TIME sampleEndTime;
    REFERENCE_TIME currentTime;
    if (m_sample->GetTime(&sampleStartTime, &sampleEndTime) == S_OK) {
        if (clock->GetTime(&currentTime) == S_OK)
            return currentTime >= sampleStartTime;
    }
    return true;
}

DirectShowSampleScheduler::DirectShowSampleScheduler(IUnknown *pin, QObject *parent)
    : QObject(parent)
    , m_pin(pin)
    , m_clock(0)
    , m_allocator(0)
    , m_head(0)
    , m_tail(0)
    , m_maximumSamples(2)
    , m_state(Stopped)
    , m_startTime(0)
    , m_timeoutEvent(::CreateEvent(0, 0, 0, 0))
{
    m_semaphore.release(m_maximumSamples);

    m_eventNotifier.setHandle(m_timeoutEvent);
    m_eventNotifier.setEnabled(true);

    connect(&m_eventNotifier, SIGNAL(activated(HANDLE)), this, SIGNAL(sampleReady()));
}

DirectShowSampleScheduler::~DirectShowSampleScheduler()
{
    m_eventNotifier.setEnabled(false);

    ::CloseHandle(m_timeoutEvent);

    Q_ASSERT(!m_clock);
    Q_ASSERT(!m_allocator);
}

HRESULT DirectShowSampleScheduler::QueryInterface(REFIID riid, void **ppvObject)
{
    return m_pin->QueryInterface(riid, ppvObject);
}

ULONG DirectShowSampleScheduler::AddRef()
{
    return m_pin->AddRef();
}

ULONG DirectShowSampleScheduler::Release()
{
    return m_pin->Release();
}

// IMemInputPin
HRESULT DirectShowSampleScheduler::GetAllocator(IMemAllocator **ppAllocator)
{
    if (!ppAllocator) {
        return E_POINTER;
    } else {
        QMutexLocker locker(&m_mutex);

        if (!m_allocator) {
            return VFW_E_NO_ALLOCATOR;
        } else {
            *ppAllocator = m_allocator;

            return S_OK;
        }
    }
}

HRESULT DirectShowSampleScheduler::NotifyAllocator(IMemAllocator *pAllocator, BOOL bReadOnly)
{
    Q_UNUSED(bReadOnly);

    HRESULT hr;
    ALLOCATOR_PROPERTIES properties;

    if (!pAllocator) {
        if (m_allocator)
            m_allocator->Release();

        m_allocator = 0;

        return S_OK;
    } else if ((hr = pAllocator->GetProperties(&properties)) != S_OK) {
        return hr;
    } else {
        if (properties.cBuffers == 1) {
            ALLOCATOR_PROPERTIES actual;

            properties.cBuffers = 2;
            if ((hr = pAllocator->SetProperties(&properties, &actual)) != S_OK)
                return hr;
        }

        QMutexLocker locker(&m_mutex);

        if (m_allocator)
            m_allocator->Release();

        m_allocator = pAllocator;
        m_allocator->AddRef();

        return S_OK;
    }
}

HRESULT DirectShowSampleScheduler::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
    if (!pProps)
        return E_POINTER;

    pProps->cBuffers = 2;

    return S_OK;
}

HRESULT DirectShowSampleScheduler::Receive(IMediaSample *pSample)
{
    if (!pSample)
        return E_POINTER;

    m_semaphore.acquire(1);

    QMutexLocker locker(&m_mutex);

    if (m_state & Flushing) {
        m_semaphore.release(1);

        return S_FALSE;
    } else if (m_state == Stopped) {
        m_semaphore.release();

        return VFW_E_WRONG_STATE;
    } else {
        DirectShowTimedSample *timedSample = new DirectShowTimedSample(pSample);

        if (m_tail)
            m_tail->setNextSample(timedSample);
        else
            m_head = timedSample;

        m_tail = timedSample;

        if (m_state == Running) {
            if (!timedSample->schedule(m_clock, m_startTime, m_timeoutEvent)) {
                // Timing information is unavailable, so schedule frames immediately.
               QMetaObject::invokeMethod(this, "sampleReady", Qt::QueuedConnection);
            }
        } else if (m_tail == m_head) {
            // If this is the first frame make is available.
            QMetaObject::invokeMethod(this, "sampleReady", Qt::QueuedConnection);
        }

        return S_OK;
    }
}

HRESULT DirectShowSampleScheduler::ReceiveMultiple(
        IMediaSample **pSamples, long nSamples, long *nSamplesProcessed)
{
    if (!pSamples || !nSamplesProcessed)
        return E_POINTER;

    for (*nSamplesProcessed = 0; *nSamplesProcessed < nSamples; ++(*nSamplesProcessed)) {
        HRESULT hr = Receive(pSamples[*nSamplesProcessed]);

        if (hr != S_OK)
            return hr;
    }
    return S_OK;
}

HRESULT DirectShowSampleScheduler::ReceiveCanBlock()
{
    return S_OK;
}

void DirectShowSampleScheduler::run(REFERENCE_TIME startTime)
{
    QMutexLocker locker(&m_mutex);

    m_state = (m_state & Flushing) | Running;
    m_startTime = startTime;

    for (DirectShowTimedSample *sample = m_head; sample; sample = sample->nextSample()) {
        sample->schedule(m_clock, m_startTime, m_timeoutEvent);
    }
}

void DirectShowSampleScheduler::pause()
{
    QMutexLocker locker(&m_mutex);

    m_state = (m_state & Flushing) | Paused;

    for (DirectShowTimedSample *sample = m_head; sample; sample = sample->nextSample())
        sample->unschedule(m_clock);
}

void DirectShowSampleScheduler::stop()
{
    QMutexLocker locker(&m_mutex);

    m_state = m_state & Flushing;

    for (DirectShowTimedSample *sample = m_head; sample; sample = sample->remove()) {
        sample->unschedule(m_clock);
        
        m_semaphore.release(1);
    }

    m_head = 0;
    m_tail = 0;
}

void DirectShowSampleScheduler::setFlushing(bool flushing)
{
    QMutexLocker locker(&m_mutex);

    const bool isFlushing = m_state & Flushing;

    if (isFlushing != flushing) {
        if (flushing) {
            m_state |= Flushing;

            for (DirectShowTimedSample *sample = m_head; sample; sample = sample->remove()) {
                sample->unschedule(m_clock);

                m_semaphore.release(1);
            }
            m_head = 0;
            m_tail = 0;
        } else {
            m_state &= ~Flushing;
        }
    }
}

void DirectShowSampleScheduler::setClock(IReferenceClock *clock)
{
    QMutexLocker locker(&m_mutex);

    if (m_clock)
        m_clock->Release();

    m_clock = clock;

    if (m_clock)
        m_clock->AddRef();
}

IMediaSample *DirectShowSampleScheduler::takeSample(bool *eos)
{
    QMutexLocker locker(&m_mutex);

    if (m_head && m_head->isReady(m_clock)) {
        IMediaSample *sample = m_head->sample();
        sample->AddRef();

        if (m_state == Running) {
            *eos =  m_head->isLast();

            m_head = m_head->remove();

            if (!m_head)
                m_tail = 0;

            m_semaphore.release(1);
        }

        return sample;
    } else {
        return 0;
    }
}

bool DirectShowSampleScheduler::scheduleEndOfStream()
{
    QMutexLocker locker(&m_mutex);

    if (m_tail) {
        m_tail->setLast();

        return true;
    } else {
        return false;
    }
}

QT_END_NAMESPACE
