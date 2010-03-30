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

#ifndef DIRECTSHOWIOREADER_H
#define DIRECTSHOWIOREADER_H

#include <QtCore/qmutex.h>
#include <QtCore/qobject.h>
#include <QtCore/qwaitcondition.h>

#include <dshow.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QIODevice;

class DirectShowEventLoop;
class DirectShowIOSource;
class DirectShowSampleRequest;

class DirectShowIOReader : public QObject, public IAsyncReader
{
    Q_OBJECT
public:
    DirectShowIOReader(QIODevice *device, DirectShowIOSource *source, DirectShowEventLoop *loop);
    ~DirectShowIOReader();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IAsyncReader
    HRESULT STDMETHODCALLTYPE RequestAllocator(
            IMemAllocator *pPreferred, ALLOCATOR_PROPERTIES *pProps, IMemAllocator **ppActual);

    HRESULT STDMETHODCALLTYPE Request(IMediaSample *pSample, DWORD_PTR dwUser);

    HRESULT STDMETHODCALLTYPE WaitForNext(
            DWORD dwTimeout, IMediaSample **ppSample, DWORD_PTR *pdwUser);

    HRESULT STDMETHODCALLTYPE SyncReadAligned(IMediaSample *pSample);

    HRESULT STDMETHODCALLTYPE SyncRead(LONGLONG llPosition, LONG lLength, BYTE *pBuffer);

    HRESULT STDMETHODCALLTYPE Length(LONGLONG *pTotal, LONGLONG *pAvailable);

    HRESULT STDMETHODCALLTYPE BeginFlush();
    HRESULT STDMETHODCALLTYPE EndFlush();

protected:
    void customEvent(QEvent *event);

private Q_SLOTS:
    void readyRead();

private:
    HRESULT blockingRead(LONGLONG position, LONG length, BYTE *buffer, qint64 *bytesRead);
    bool nonBlockingRead(
            LONGLONG position, LONG length, BYTE *buffer, qint64 *bytesRead, HRESULT *result);
    void flushRequests();

    DirectShowIOSource *m_source;
    QIODevice *m_device;
    DirectShowEventLoop *m_loop;
    DirectShowSampleRequest *m_pendingHead;
    DirectShowSampleRequest *m_pendingTail;
    DirectShowSampleRequest *m_readyHead;
    DirectShowSampleRequest *m_readyTail;
    LONGLONG m_synchronousPosition;
    LONG m_synchronousLength;
    qint64 m_synchronousBytesRead;
    BYTE *m_synchronousBuffer;
    HRESULT m_synchronousResult;
    LONGLONG m_totalLength;
    LONGLONG m_availableLength;
    bool m_flushing;
    QMutex m_mutex;
    QWaitCondition m_wait;
};

QT_END_NAMESPACE

QT_END_HEADER
#endif
