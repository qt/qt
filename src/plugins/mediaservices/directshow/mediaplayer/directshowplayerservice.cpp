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

#include "directshowplayerservice.h"

#include "directshowaudioendpointcontrol.h"
#include "directshowiosource.h"
#include "directshowmetadatacontrol.h"
#include "directshowplayercontrol.h"
#include "directshowvideooutputcontrol.h"
#include "directshowvideorenderercontrol.h"
#include "vmr9videowindowcontrol.h"

#include <QtMediaServices/qmediacontent.h>

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qthread.h>
#include <QtCore/qvarlengtharray.h>

Q_GLOBAL_STATIC(DirectShowEventLoop, qt_directShowEventLoop)

QT_BEGIN_NAMESPACE

class DirectShowPlayerServiceThread : public QThread
{
public:
    DirectShowPlayerServiceThread(DirectShowPlayerService *service)
        : m_service(service)
    {
    }

protected:
    void run() { m_service->run(); }

private:
    DirectShowPlayerService *m_service;
};

DirectShowPlayerService::DirectShowPlayerService(QObject *parent)
    : QMediaService(parent)
    , m_playerControl(0)
    , m_metaDataControl(0)
    , m_videoOutputControl(0)
    , m_videoRendererControl(0)
    , m_videoWindowControl(0)
    , m_audioEndpointControl(0)
    , m_taskThread(0)
    , m_loop(qt_directShowEventLoop())
    , m_pendingTasks(0)
    , m_executingTask(0)
    , m_executedTasks(0)
    , m_taskHandle(::CreateEvent(0, 0, 0, 0))
    , m_eventHandle(0)
    , m_graphStatus(NoMedia)
    , m_stream(0)
    , m_graph(0)
    , m_source(0)
    , m_audioOutput(0)
    , m_videoOutput(0)
    , m_rate(1.0)
    , m_position(0)
    , m_duration(0)
    , m_buffering(false)
    , m_seekable(false)
    , m_atEnd(false)
{
    m_playerControl = new DirectShowPlayerControl(this);
    m_metaDataControl = new DirectShowMetaDataControl(this);
    m_videoOutputControl = new DirectShowVideoOutputControl; 
    m_audioEndpointControl = new DirectShowAudioEndpointControl(this);
    m_videoRendererControl = new DirectShowVideoRendererControl(m_loop);
    m_videoWindowControl = new Vmr9VideoWindowControl;

    m_taskThread = new DirectShowPlayerServiceThread(this);
    m_taskThread->start();

    connect(m_videoOutputControl, SIGNAL(outputChanged()), this, SLOT(videoOutputChanged()));
    connect(m_videoRendererControl, SIGNAL(filterChanged()), this, SLOT(videoOutputChanged()));
}

DirectShowPlayerService::~DirectShowPlayerService()
{
    {
        QMutexLocker locker(&m_mutex);

        releaseGraph();

        m_pendingTasks = Shutdown;
        ::SetEvent(m_taskHandle);
    }

    m_taskThread->wait();
    delete m_taskThread;

    if (m_audioOutput) {
        m_audioOutput->Release();
        m_audioOutput = 0;
    }

    if (m_videoOutput) {
        m_videoOutput->Release();
        m_videoOutput = 0;
    }

    delete m_playerControl;
    delete m_audioEndpointControl;
    delete m_metaDataControl;
    delete m_videoOutputControl;
    delete m_videoRendererControl;
    delete m_videoWindowControl;

    ::CloseHandle(m_taskHandle);
}

QMediaControl *DirectShowPlayerService::control(const char *name) const
{
    if (qstrcmp(name, QMediaPlayerControl_iid) == 0)
        return m_playerControl;
    else if (qstrcmp(name, QAudioEndpointSelector_iid) == 0)
        return m_audioEndpointControl;
    else if (qstrcmp(name, QMetaDataControl_iid) == 0)
        return m_metaDataControl;
    else if (qstrcmp(name, QVideoOutputControl_iid) == 0)
        return m_videoOutputControl;
    else if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        return m_videoRendererControl;
    else if (qstrcmp(name, QVideoWindowControl_iid) == 0)
        return m_videoWindowControl;
    else
        return 0;
}

void DirectShowPlayerService::load(const QMediaContent &media, QIODevice *stream)
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks = 0;

    if (m_graph)
        releaseGraph();

    m_resources = media.resources();
    m_stream = stream;
    m_error = QMediaPlayer::NoError;
    m_errorString = QString();
    m_position = 0;
    m_duration = 0;
    m_streamTypes = 0;
    m_executedTasks = 0;
    m_buffering = false;
    m_seekable = false;
    m_atEnd = false;
    m_metaDataControl->updateGraph(0, 0);

    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(VideoOutputChange)));

    if (m_resources.isEmpty() && !stream) {
        m_pendingTasks = 0;
        m_graphStatus = NoMedia;

        m_url.clear();
    } else if (stream && (!stream->isReadable() || stream->isSequential())) {
        m_pendingTasks = 0;
        m_graphStatus = InvalidMedia;
        m_error = QMediaPlayer::ResourceError;
    } else {
        // {36b73882-c2c8-11cf-8b46-00805f6cef60}
        static const GUID iid_IFilterGraph2 = {
            0x36b73882, 0xc2c8, 0x11cf, {0x8b, 0x46, 0x00, 0x80, 0x5f, 0x6c, 0xef, 0x60} };
        m_graphStatus = Loading;

        m_graph = com_new<IFilterGraph2>(CLSID_FilterGraph, iid_IFilterGraph2);

        if (stream)
            m_pendingTasks = SetStreamSource;
        else
            m_pendingTasks = SetUrlSource;

        ::SetEvent(m_taskHandle);
    }

    m_playerControl->updateError(m_error, m_errorString);
    m_playerControl->updateMediaInfo(m_duration, m_streamTypes, m_seekable);
    m_playerControl->updateState(QMediaPlayer::StoppedState);
    m_playerControl->updatePosition(m_position);
    updateStatus();
}

void DirectShowPlayerService::doSetUrlSource(QMutexLocker *locker)
{
    IBaseFilter *source = 0;

    QMediaResource resource = m_resources.takeFirst();
    QUrl url = resource.url();

    HRESULT hr = E_FAIL;

    if (url.scheme() == QLatin1String("http") || url.scheme() == QLatin1String("https")) {
        static const GUID clsid_WMAsfReader = {
            0x187463a0, 0x5bb7, 0x11d3, {0xac, 0xbe, 0x00, 0x80, 0xc7, 0x5e, 0x24, 0x6e} };

        // {56a868a6-0ad4-11ce-b03a-0020af0ba770}
        static const GUID iid_IFileSourceFilter = {
            0x56a868a6, 0x0ad4, 0x11ce, {0xb0, 0x3a, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70} };

        if (IFileSourceFilter *fileSource = com_new<IFileSourceFilter>(
                clsid_WMAsfReader, iid_IFileSourceFilter)) {
            locker->unlock();
            hr = fileSource->Load(reinterpret_cast<const OLECHAR *>(url.toString().utf16()), 0);
            locker->relock();

            if (SUCCEEDED(hr)) {
                source = com_cast<IBaseFilter>(fileSource, IID_IBaseFilter);

                if (!SUCCEEDED(hr = m_graph->AddFilter(source, L"Source")) && source) {
                    source->Release();
                    source = 0;
                }
            }

            fileSource->Release();
        }
    } else if (url.scheme() == QLatin1String("qrc")) {
        DirectShowRcSource *rcSource = new DirectShowRcSource(m_loop);

        if (rcSource->open(url) && SUCCEEDED(hr = m_graph->AddFilter(rcSource, L"Source")))
            source = rcSource;
        else
            rcSource->Release();
    }

    if (!SUCCEEDED(hr)) {
        locker->unlock();
        hr = m_graph->AddSourceFilter(
                reinterpret_cast<const OLECHAR *>(url.toString().utf16()), L"Source", &source);
        locker->relock();
    }

    if (SUCCEEDED(hr)) {
        m_executedTasks |= SetSource;
        m_pendingTasks |= Render;

        if (m_audioOutput)
            m_pendingTasks |= SetAudioOutput;
        if (m_videoOutput)
            m_pendingTasks |= SetVideoOutput;

        if (m_rate != 1.0 && m_rate != 0.0)
            m_pendingTasks |= SetRate;

        m_source = source;
    } else if (!m_resources.isEmpty()) {
        m_pendingTasks |= SetUrlSource;
    } else {
        m_pendingTasks = 0;
        m_graphStatus = InvalidMedia;

        switch (hr) {
        case VFW_E_UNKNOWN_FILE_TYPE:
            m_error = QMediaPlayer::FormatError;
            m_errorString = QString();
            break;
        case E_OUTOFMEMORY:
        case VFW_E_CANNOT_LOAD_SOURCE_FILTER:
        case VFW_E_NOT_FOUND:
            m_error = QMediaPlayer::ResourceError;
            m_errorString = QString();
        default:
            m_error = QMediaPlayer::ResourceError;
            m_errorString = QString();
            qWarning("DirectShowPlayerService::doSetUrlSource: Unresolved error code %x", uint(hr));
            break;
        }

        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(Error)));
    }
}

void DirectShowPlayerService::doSetStreamSource(QMutexLocker *locker)
{
    DirectShowIOSource *source = new DirectShowIOSource(m_loop);
    source->setDevice(m_stream);

    if (SUCCEEDED(m_graph->AddFilter(source, L"Source"))) {
        m_executedTasks |= SetSource;
        m_pendingTasks |= Render;

        if (m_audioOutput)
            m_pendingTasks |= SetAudioOutput;
        if (m_videoOutput)
            m_pendingTasks |= SetVideoOutput;

        if (m_rate != 1.0 && m_rate != 0.0)
            m_pendingTasks |= SetRate;

        m_source = source;
    } else {
        source->Release();

        m_pendingTasks = 0;
        m_graphStatus = InvalidMedia;

        m_error = QMediaPlayer::ResourceError;
        m_errorString = QString();

        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(Error)));
    }
}

void DirectShowPlayerService::doRender(QMutexLocker *locker)
{
    if (m_executedTasks & Pause)
        m_pendingTasks |= Pause;
    else if (m_executedTasks & Play && m_rate != 0.0)
        m_pendingTasks |= Play;

    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        control->Stop();
        control->Release();
    }

    if (m_pendingTasks & SetAudioOutput) {
        m_graph->AddFilter(m_audioOutput, L"AudioOutput");

        m_pendingTasks ^= SetAudioOutput;
        m_executedTasks |= SetAudioOutput;
    }
    if (m_pendingTasks & SetVideoOutput) {
        m_graph->AddFilter(m_videoOutput, L"VideoOutput");

        m_pendingTasks ^= SetVideoOutput;
        m_executedTasks |= SetVideoOutput;
    }

    IFilterGraph2 *graph = m_graph;
    graph->AddRef();

    QVarLengthArray<IBaseFilter *, 16> filters;
    m_source->AddRef();
    filters.append(m_source);

    bool rendered = false;

    HRESULT renderHr = S_OK;

    while (!filters.isEmpty()) {
        IEnumPins *pins = 0;
        IBaseFilter *filter = filters[filters.size() - 1];
        filters.removeLast();

        if (!(m_pendingTasks & ReleaseFilters) && SUCCEEDED(filter->EnumPins(&pins))) {
            int outputs = 0;
            for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
                PIN_DIRECTION direction;
                if (pin->QueryDirection(&direction) == S_OK && direction == PINDIR_OUTPUT) {
                    ++outputs;

                    IPin *peer = 0;
                    if (pin->ConnectedTo(&peer) == S_OK) {
                        PIN_INFO peerInfo;
                        if (SUCCEEDED(peer->QueryPinInfo(&peerInfo)))
                            filters.append(peerInfo.pFilter);
                        peer->Release();
                    } else {
                        locker->unlock();
                        HRESULT hr;
                        if (SUCCEEDED(hr = graph->RenderEx(
                                pin, /*AM_RENDEREX_RENDERTOEXISTINGRENDERERS*/ 1, 0))) {
                            rendered = true;
                        } else if (renderHr == S_OK || renderHr == VFW_E_NO_DECOMPRESSOR){
                            renderHr = hr;
                        }
                        locker->relock();
                    }
                }
            }

            pins->Release();

            if (outputs == 0)
                rendered = true;
        }
        filter->Release();
    }

    if (m_audioOutput && !isConnected(m_audioOutput, PINDIR_INPUT)) {
        graph->RemoveFilter(m_audioOutput);

        m_executedTasks &= ~SetAudioOutput;
    }

    if (m_videoOutput && !isConnected(m_videoOutput, PINDIR_INPUT)) {
        graph->RemoveFilter(m_videoOutput);

        m_executedTasks &= ~SetVideoOutput;
    }

    graph->Release();

    if (!(m_pendingTasks & ReleaseFilters)) {
        if (rendered) {
            if (!(m_executedTasks & FinalizeLoad))
                m_pendingTasks |= FinalizeLoad;
        } else {
            m_pendingTasks = 0;

            m_graphStatus = InvalidMedia;

            if (!m_audioOutput && !m_videoOutput) {
                m_error = QMediaPlayer::ResourceError;
                m_errorString = QString();
            } else {
                switch (renderHr) {
                case VFW_E_UNSUPPORTED_AUDIO:
                case VFW_E_UNSUPPORTED_VIDEO:
                case VFW_E_UNSUPPORTED_STREAM:
                    m_error = QMediaPlayer::FormatError;
                    m_errorString = QString();
                default:
                    m_error = QMediaPlayer::ResourceError;
                    m_errorString = QString();
                    qWarning("DirectShowPlayerService::doRender: Unresolved error code %x",
                             uint(renderHr));
                }
            }

            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(Error)));
        }

        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(VideoOutputChange)));

        m_executedTasks |= Render;
    }
}

void DirectShowPlayerService::doFinalizeLoad(QMutexLocker *locker)
{
    if (m_graphStatus != Loaded) {
        if (IMediaEvent *event = com_cast<IMediaEvent>(m_graph, IID_IMediaEvent)) {
            event->GetEventHandle(reinterpret_cast<OAEVENT *>(&m_eventHandle));
            event->Release();
        }
        if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
            LONGLONG duration = 0;
            seeking->GetDuration(&duration);
            m_duration = duration / 10;

            DWORD capabilities = 0;
            seeking->GetCapabilities(&capabilities);
            m_seekable = capabilities & AM_SEEKING_CanSeekAbsolute;

            seeking->Release();
        }
    }

    if ((m_executedTasks & SetOutputs) == SetOutputs) {
        m_streamTypes = AudioStream | VideoStream;
    } else {
        m_streamTypes = findStreamTypes(m_source);
    }

    m_executedTasks |= FinalizeLoad;

    m_graphStatus = Loaded;

    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(FinalizedLoad)));
}

void DirectShowPlayerService::releaseGraph()
{
    if (m_graph) {
        if (m_executingTask != 0) {
            // {8E1C39A1-DE53-11cf-AA63-0080C744528D}
            static const GUID iid_IAMOpenProgress = {
                0x8E1C39A1, 0xDE53, 0x11cf, {0xAA, 0x63, 0x00, 0x80, 0xC7, 0x44, 0x52, 0x8D} };

            if (IAMOpenProgress *progress = com_cast<IAMOpenProgress>(
                    m_graph, iid_IAMOpenProgress)) {
                progress->AbortOperation();
                progress->Release();
            }
            m_graph->Abort();
        }
        
        m_pendingTasks = ReleaseGraph;

        ::SetEvent(m_taskHandle);

        m_loop->wait(&m_mutex);
    }
}

void DirectShowPlayerService::doReleaseGraph(QMutexLocker *locker)
{
    Q_UNUSED(locker);

    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        control->Stop();
        control->Release();
    }

    if (m_source) {
        m_source->Release();
        m_source = 0;
    }

    m_eventHandle = 0;

    m_graph->Release();
    m_graph = 0;

    m_loop->wake();
}

int DirectShowPlayerService::findStreamTypes(IBaseFilter *source) const
{
    QVarLengthArray<IBaseFilter *, 16> filters;
    source->AddRef();
    filters.append(source);

    int streamTypes = 0;

    while (!filters.isEmpty()) {
        IEnumPins *pins = 0;
        IBaseFilter *filter = filters[filters.size() - 1];
        filters.removeLast();

        if (SUCCEEDED(filter->EnumPins(&pins))) {
            for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
                PIN_DIRECTION direction;
                if (pin->QueryDirection(&direction) == S_OK && direction == PINDIR_OUTPUT) {
                    AM_MEDIA_TYPE connectionType;
                    if (SUCCEEDED(pin->ConnectionMediaType(&connectionType))) {
                        IPin *peer = 0;

                        if (connectionType.majortype == MEDIATYPE_Audio) {
                            streamTypes |= AudioStream;
                        } else if (connectionType.majortype == MEDIATYPE_Video) {
                            streamTypes |= VideoStream;
                        } else if (SUCCEEDED(pin->ConnectedTo(&peer))) {
                            PIN_INFO peerInfo;
                            if (SUCCEEDED(peer->QueryPinInfo(&peerInfo)))
                                filters.append(peerInfo.pFilter);
                            peer->Release();
                        }
                    } else {
                        streamTypes |= findStreamType(pin);
                    }
                }
            }
        }
        filter->Release();
    }
    return streamTypes;
}

int DirectShowPlayerService::findStreamType(IPin *pin) const
{
    IEnumMediaTypes *types;

    if (SUCCEEDED(pin->EnumMediaTypes(&types))) {
        bool video = false;
        bool audio = false;
        bool other = false;

        for (AM_MEDIA_TYPE *type = 0;
                types->Next(1, &type, 0) == S_OK;
                DirectShowMediaType::deleteType(type)) {
            if (type->majortype == MEDIATYPE_Audio)
                audio = true;
            else if (type->majortype == MEDIATYPE_Video)
                video = true;
            else
                other = true;
        }
        types->Release();

        if (other)
            return 0;
        else if (audio && !video)
            return AudioStream;
        else if (!audio && video)
            return VideoStream;
        else
            return 0;
    } else {
        return 0;
    }
}

void DirectShowPlayerService::play()
{
    QMutexLocker locker(&m_mutex);

    if (m_rate != 0.0) {
        m_pendingTasks &= ~Pause;
        m_pendingTasks |= Play;
    } else {
        m_pendingTasks |= Pause;
        m_executedTasks |= Play;
    }

    if (m_executedTasks & Render) {
        if (m_executedTasks & Stop) {
            m_atEnd = false;
            m_position = 0;
            m_pendingTasks |= Seek;
            m_executedTasks ^= Stop;
        }

        ::SetEvent(m_taskHandle);
    }
}

void DirectShowPlayerService::doPlay(QMutexLocker *locker)
{
    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        locker->unlock();
        HRESULT hr = control->Run();
        locker->relock();

        control->Release();

        if (SUCCEEDED(hr)) {
            m_executedTasks |= Play;
            m_executedTasks &= ~Pause;

            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StatusChange)));
        } else {
            m_error = QMediaPlayer::ResourceError;
            m_errorString = QString();
            qWarning("DirectShowPlayerService::doPlay: Unresolved error code %x", uint(hr));

            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(Error)));
        }
    }
}

void DirectShowPlayerService::pause()
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks &= ~Play;
    m_pendingTasks |= Pause;

    if (m_executedTasks & Render) {
        if (m_executedTasks & Stop) {
            m_atEnd = false;
            m_position = 0;
            m_pendingTasks |= Seek;
            m_executedTasks ^= Stop;
        }

        ::SetEvent(m_taskHandle);
    }
}

void DirectShowPlayerService::doPause(QMutexLocker *locker)
{
    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        locker->unlock();
        HRESULT hr = control->Pause();
        locker->relock();

        control->Release();

        if (SUCCEEDED(hr)) {
            if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
                LONGLONG position = 0;

                seeking->GetCurrentPosition(&position);
                seeking->Release();

                m_position = position / 10;
            } else {
                m_position = 0;
            }

            m_executedTasks |= Pause;

            if (m_rate != 0.0)
                m_executedTasks &= ~Play;

            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StatusChange)));
        } else {
            m_error = QMediaPlayer::ResourceError;
            m_errorString = QString();
            qWarning("DirectShowPlayerService::doPause: Unresolved error code %x", uint(hr));

            QCoreApplication::postEvent(this, new QEvent(QEvent::Type(Error)));
        }
    }
}

void DirectShowPlayerService::stop()
{
    QMutexLocker locker(&m_mutex);

    m_pendingTasks &= ~(Play | Pause | Seek);

    if ((m_executingTask | m_executedTasks) & (Play | Pause | Seek)) {
        m_pendingTasks |= Stop;

        ::SetEvent(m_taskHandle);

        m_loop->wait(&m_mutex);
    }

}

void DirectShowPlayerService::doStop(QMutexLocker *locker)
{
    if (m_executedTasks & (Play | Pause)) {
        if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
            control->Stop();
            control->Release();
        }

        if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
            LONGLONG position = 0;

            seeking->GetCurrentPosition(&position);
            seeking->Release();

            m_position = position / 10;
        } else {
            m_position = 0;
        }

        m_executedTasks &= ~(Play | Pause);
        
        QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StatusChange)));
    }

    m_executedTasks |= Stop;

    m_loop->wake();
}

void DirectShowPlayerService::setRate(qreal rate)
{
    QMutexLocker locker(&m_mutex);

    if (m_rate == rate)
        return;

    if (rate == 0.0) {
        if (m_pendingTasks & Play) {
            m_executedTasks |= Play;
            m_pendingTasks &= ~(Play | SetRate);

            if (!((m_executingTask | m_executedTasks) & Pause))
                m_pendingTasks |= Pause;
        } else if ((m_executingTask | m_executedTasks) & Play) {
            m_pendingTasks |= Pause;
        }
    } else {
        m_pendingTasks |= SetRate;

        if (m_rate == 0.0 && (m_executedTasks & Play) && !(m_executingTask & Play))
            m_pendingTasks |= Play;
    }

    m_rate = rate;

    if (m_executedTasks & FinalizeLoad)
        ::SetEvent(m_taskHandle);
}

void DirectShowPlayerService::doSetRate(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
        // Cache current values as we can't query IMediaSeeking during a seek due to the
        // possibility of a deadlock when flushing the VideoSurfaceFilter.
        LONGLONG currentPosition = 0;
        seeking->GetCurrentPosition(&currentPosition);
        m_position = currentPosition / 10;

        LONGLONG minimum = 0;
        LONGLONG maximum = 0;
        m_playbackRange = SUCCEEDED(seeking->GetAvailable(&minimum, &maximum))
                ? QMediaTimeRange(minimum / 10, maximum / 10)
                : QMediaTimeRange();

        locker->unlock();
        HRESULT hr = seeking->SetRate(m_rate);
        locker->relock();

        if (!SUCCEEDED(hr)) {
            double rate = 0.0;
            m_rate = seeking->GetRate(&rate)
                    ? rate
                    : 1.0;
        }

        seeking->Release();
    } else if (m_rate != 1.0) {
        m_rate = 1.0;
    }

    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(RateChange)));
}

qint64 DirectShowPlayerService::position() const
{
    QMutexLocker locker(const_cast<QMutex *>(&m_mutex));

    if (m_graphStatus == Loaded) {
        if (m_executingTask == Seek || m_executingTask == SetRate) {
            return m_position;
        } else if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
            LONGLONG position = 0;

            seeking->GetCurrentPosition(&position);
            seeking->Release();

            const_cast<qint64 &>(m_position) = position / 10;

            return m_position;
        }
    }
    return 0;
}

QMediaTimeRange DirectShowPlayerService::availablePlaybackRanges() const
{
    QMutexLocker locker(const_cast<QMutex *>(&m_mutex));

    if (m_graphStatus == Loaded) {
        if (m_executingTask == Seek || m_executingTask == SetRate) {
            return m_playbackRange;
        } else if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
            LONGLONG minimum = 0;
            LONGLONG maximum = 0;

            HRESULT hr = seeking->GetAvailable(&minimum, &maximum);
            seeking->Release();

            if (SUCCEEDED(hr))
                return QMediaTimeRange(minimum, maximum);
        }
    }
    return QMediaTimeRange();
}

void DirectShowPlayerService::seek(qint64 position)
{
    QMutexLocker locker(&m_mutex);

    m_position = position;

    m_pendingTasks |= Seek;

    if (m_executedTasks & FinalizeLoad)
        ::SetEvent(m_taskHandle);
}

void DirectShowPlayerService::doSeek(QMutexLocker *locker)
{
    if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
        LONGLONG seekPosition = LONGLONG(m_position) * 10;

        // Cache current values as we can't query IMediaSeeking during a seek due to the
        // possibility of a deadlock when flushing the VideoSurfaceFilter.
        LONGLONG currentPosition = 0;
        seeking->GetCurrentPosition(&currentPosition);
        m_position = currentPosition / 10;

        LONGLONG minimum = 0;
        LONGLONG maximum = 0;
        m_playbackRange = SUCCEEDED(seeking->GetAvailable(&minimum, &maximum))
                ? QMediaTimeRange(minimum / 10, maximum / 10)
                : QMediaTimeRange();

        locker->unlock();
        seeking->SetPositions(
                &seekPosition, AM_SEEKING_AbsolutePositioning, 0, AM_SEEKING_NoPositioning);
        locker->relock();

        seeking->GetCurrentPosition(&currentPosition);
        m_position = currentPosition / 10;

        seeking->Release();
    } else {
        m_position = 0;
    }

    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(PositionChange)));
}

int DirectShowPlayerService::bufferStatus() const
{
#ifndef QT_NO_WMSDK
    QMutexLocker locker(const_cast<QMutex *>(&m_mutex));

    if (IWMReaderAdvanced2 *reader = com_cast<IWMReaderAdvanced2>(
            m_source, IID_IWMReaderAdvanced2)) {
        DWORD percentage = 0;

        reader->GetBufferProgress(&percentage, 0);
        reader->Release();

        return percentage;
    } else {
        return 0;
    }
#else
    return 0;
#endif
}

void DirectShowPlayerService::setAudioOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_audioOutput) {
            if (m_executedTasks & SetAudioOutput) {
                m_pendingTasks |= ReleaseAudioOutput;

                ::SetEvent(m_taskHandle);

                m_loop->wait(&m_mutex);
            }
            m_audioOutput->Release();
        } 

        m_audioOutput = filter;

        if (m_audioOutput) {
            m_audioOutput->AddRef();

            m_pendingTasks |= SetAudioOutput;

            if (m_executedTasks & SetSource) {
                m_pendingTasks |= Render;

                ::SetEvent(m_taskHandle);
            }
        } else {
            m_pendingTasks &= ~ SetAudioOutput;
        }
    } else {
        if (m_audioOutput)
            m_audioOutput->Release();

        m_audioOutput = filter;

        if (m_audioOutput)
            m_audioOutput->AddRef();
    }

    m_playerControl->updateAudioOutput(m_audioOutput);
}

void DirectShowPlayerService::doReleaseAudioOutput(QMutexLocker *locker)
{
    m_pendingTasks |= m_executedTasks & (Play | Pause);

    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        control->Stop();
        control->Release();
    }

    IBaseFilter *decoder = getConnected(m_audioOutput, PINDIR_INPUT);
    if (!decoder) {
        decoder = m_audioOutput;
        decoder->AddRef();
    }

    // {DCFBDCF6-0DC2-45f5-9AB2-7C330EA09C29}
    static const GUID iid_IFilterChain = {
        0xDCFBDCF6, 0x0DC2, 0x45f5, {0x9A, 0xB2, 0x7C, 0x33, 0x0E, 0xA0, 0x9C, 0x29} };

    if (IFilterChain *chain = com_cast<IFilterChain>(m_graph, iid_IFilterChain)) {
        chain->RemoveChain(decoder, m_audioOutput);
        chain->Release();
    } else {
        m_graph->RemoveFilter(m_audioOutput);
    }

    decoder->Release();

    m_executedTasks &= ~SetAudioOutput;

    m_loop->wake();
}

void DirectShowPlayerService::setVideoOutput(IBaseFilter *filter)
{
    QMutexLocker locker(&m_mutex);

    if (m_graph) {
        if (m_videoOutput) {
            if (m_executedTasks & SetVideoOutput) {
                m_pendingTasks |= ReleaseVideoOutput;

                ::SetEvent(m_taskHandle);

                m_loop->wait(&m_mutex);
            }
            m_videoOutput->Release();
        }

        m_videoOutput = filter;

        if (m_videoOutput) {
            m_videoOutput->AddRef();

            m_pendingTasks |= SetVideoOutput;

            if (m_executedTasks & SetSource) {
                m_pendingTasks |= Render;

                ::SetEvent(m_taskHandle);
            }
        }
    } else {
        if (m_videoOutput)
            m_videoOutput->Release();

        m_videoOutput = filter;

        if (m_videoOutput)
            m_videoOutput->AddRef();
    }
}

void DirectShowPlayerService::doReleaseVideoOutput(QMutexLocker *locker)
{
    m_pendingTasks |= m_executedTasks & (Play | Pause);

    if (IMediaControl *control = com_cast<IMediaControl>(m_graph, IID_IMediaControl)) {
        control->Stop();
        control->Release();
    }

    IBaseFilter *intermediate = 0;
    if (!SUCCEEDED(m_graph->FindFilterByName(L"Color Space Converter", &intermediate))) {
        intermediate = m_videoOutput;
        intermediate->AddRef();
    }

    IBaseFilter *decoder = getConnected(intermediate, PINDIR_INPUT);
    if (!decoder) {
        decoder = intermediate;
        decoder->AddRef();
    }

    // {DCFBDCF6-0DC2-45f5-9AB2-7C330EA09C29}
    static const GUID iid_IFilterChain = {
        0xDCFBDCF6, 0x0DC2, 0x45f5, {0x9A, 0xB2, 0x7C, 0x33, 0x0E, 0xA0, 0x9C, 0x29} };

    if (IFilterChain *chain = com_cast<IFilterChain>(m_graph, iid_IFilterChain)) {
        chain->RemoveChain(decoder, m_videoOutput);
        chain->Release();
    } else {
        m_graph->RemoveFilter(m_videoOutput);
    }

    intermediate->Release();
    decoder->Release();

    m_executedTasks &= ~SetVideoOutput;

    m_loop->wake();
}

void DirectShowPlayerService::customEvent(QEvent *event)
{
    if (event->type() == QEvent::Type(FinalizedLoad)) {
        QMutexLocker locker(&m_mutex);

        m_playerControl->updateMediaInfo(m_duration, m_streamTypes, m_seekable);
        m_metaDataControl->updateGraph(m_graph, m_source);

        updateStatus();
    } else if (event->type() == QEvent::Type(Error)) {
        QMutexLocker locker(&m_mutex);

        if (m_error != QMediaPlayer::NoError) {
            m_playerControl->updateError(m_error, m_errorString);
            m_playerControl->updateMediaInfo(m_duration, m_streamTypes, m_seekable);
            m_playerControl->updateState(QMediaPlayer::StoppedState);
            updateStatus();
        }
    } else if (event->type() == QEvent::Type(RateChange)) {
        QMutexLocker locker(&m_mutex);

        m_playerControl->updatePlaybackRate(m_rate);
    } else if (event->type() == QEvent::Type(StatusChange)) {
        QMutexLocker locker(&m_mutex);

        updateStatus();
        m_playerControl->updatePosition(m_position);
    } else if (event->type() == QEvent::Type(DurationChange)) {
        QMutexLocker locker(&m_mutex);

        m_playerControl->updateMediaInfo(m_duration, m_streamTypes, m_seekable);
    } else if (event->type() == QEvent::Type(EndOfMedia)) {
        QMutexLocker locker(&m_mutex);

        if (m_atEnd) {
            m_playerControl->updateState(QMediaPlayer::StoppedState);
            m_playerControl->updateStatus(QMediaPlayer::EndOfMedia);
            m_playerControl->updatePosition(m_position);
        }
    } else if (event->type() == QEvent::Type(PositionChange)) {
        QMutexLocker locker(&m_mutex);

        m_playerControl->updatePosition(m_position);
    } else if (event->type() == QEvent::Type(VideoOutputChange)) {
        if (m_videoWindowControl)
            m_videoWindowControl->updateNativeSize();
    } else {
        QMediaService::customEvent(event);
    }
}

void DirectShowPlayerService::videoOutputChanged()
{
    IBaseFilter *videoOutput = 0;

    switch (m_videoOutputControl->output()) {
    case QVideoOutputControl::RendererOutput:
        videoOutput = m_videoRendererControl->filter();
        break;
    case QVideoOutputControl::WindowOutput:
        videoOutput = m_videoWindowControl->filter();
        break;
    default:
        break;
    }

    setVideoOutput(videoOutput);
}

void DirectShowPlayerService::graphEvent(QMutexLocker *locker)
{
    if (IMediaEvent *event = com_cast<IMediaEvent>(m_graph, IID_IMediaEvent)) {
        long eventCode;
        LONG_PTR param1;
        LONG_PTR param2;

        while (event->GetEvent(&eventCode, &param1, &param2, 0) == S_OK) {
            switch (eventCode) {
            case EC_BUFFERING_DATA:
                m_buffering = param1;

                QCoreApplication::postEvent(this, new QEvent(QEvent::Type(StatusChange)));
                break;
            case EC_COMPLETE:
                m_executedTasks &= ~(Play | Pause);
                m_executedTasks |= Stop;

                m_buffering = false;
                m_atEnd = true;

                if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
                    LONGLONG position = 0;

                    seeking->GetCurrentPosition(&position);
                    seeking->Release();

                    m_position = position / 10;
                }

                QCoreApplication::postEvent(this, new QEvent(QEvent::Type(EndOfMedia)));
                break;
            case EC_LENGTH_CHANGED:
                if (IMediaSeeking *seeking = com_cast<IMediaSeeking>(m_graph, IID_IMediaSeeking)) {
                    LONGLONG duration = 0;
                    seeking->GetDuration(&duration);
                    m_duration = duration / 10;

                    DWORD capabilities = 0;
                    seeking->GetCapabilities(&capabilities);
                    m_seekable = capabilities & AM_SEEKING_CanSeekAbsolute;

                    seeking->Release();

                    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(DurationChange)));
                }
                break;
            default:
                break;
            }

            event->FreeEventParams(eventCode, param1, param2);
        }
        event->Release();
    }
}

void DirectShowPlayerService::updateStatus()
{
    switch (m_graphStatus) {
    case NoMedia:
        m_playerControl->updateStatus(QMediaPlayer::NoMedia);
        break;
    case Loading:
        m_playerControl->updateStatus(QMediaPlayer::LoadingMedia);
        break;
    case Loaded:
        if ((m_pendingTasks | m_executingTask | m_executedTasks) & (Play | Pause)) {
            if (m_buffering)
                m_playerControl->updateStatus(QMediaPlayer::BufferingMedia);
            else
                m_playerControl->updateStatus(QMediaPlayer::BufferedMedia);
        } else {
            m_playerControl->updateStatus(QMediaPlayer::LoadedMedia);
        }
        break;
    case InvalidMedia:
        m_playerControl->updateStatus(QMediaPlayer::InvalidMedia);
        break;
    default:
        m_playerControl->updateStatus(QMediaPlayer::UnknownMediaStatus);
    }
}

bool DirectShowPlayerService::isConnected(IBaseFilter *filter, PIN_DIRECTION direction) const
{
    bool connected = false;

    IEnumPins *pins = 0;

    if (SUCCEEDED(filter->EnumPins(&pins))) {
        for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
            PIN_DIRECTION dir;
            if (SUCCEEDED(pin->QueryDirection(&dir)) && dir == direction) {
                IPin *peer = 0;
                if (SUCCEEDED(pin->ConnectedTo(&peer))) {
                    connected = true;

                    peer->Release();
                }
            }
        }
        pins->Release();
    }
    return connected;
}

IBaseFilter *DirectShowPlayerService::getConnected(
        IBaseFilter *filter, PIN_DIRECTION direction) const
{
    IBaseFilter *connected = 0;

    IEnumPins *pins = 0;

    if (SUCCEEDED(filter->EnumPins(&pins))) {
        for (IPin *pin = 0; pins->Next(1, &pin, 0) == S_OK; pin->Release()) {
            PIN_DIRECTION dir;
            if (SUCCEEDED(pin->QueryDirection(&dir)) && dir == direction) {
                IPin *peer = 0;
                if (SUCCEEDED(pin->ConnectedTo(&peer))) {
                    PIN_INFO info;

                    if (SUCCEEDED(peer->QueryPinInfo(&info))) {
                        if (connected) {
                            qWarning("DirectShowPlayerService::getConnected: "
                                "Multiple connected filters");
                            connected->Release();
                        }
                        connected = info.pFilter;
                    }
                    peer->Release();
                }
            }
        }
        pins->Release();
    }
    return connected;
}

void DirectShowPlayerService::run()
{
    QMutexLocker locker(&m_mutex);

    for (;;) {
        ::ResetEvent(m_taskHandle);

        while (m_pendingTasks == 0) {
            DWORD result = 0;

            locker.unlock();
            if (m_eventHandle) {
                HANDLE handles[] = { m_taskHandle, m_eventHandle };

                result = ::WaitForMultipleObjects(2, handles, false, INFINITE);
            } else {
                result = ::WaitForSingleObject(m_taskHandle, INFINITE);
            }
            locker.relock();

            if (result == WAIT_OBJECT_0 + 1) {
                graphEvent(&locker);
            }
        }

        if (m_pendingTasks & ReleaseGraph) {
            m_pendingTasks ^= ReleaseGraph;
            m_executingTask = ReleaseGraph;

            doReleaseGraph(&locker);
        } else if (m_pendingTasks & Shutdown) {
            return;
        } else if (m_pendingTasks & ReleaseAudioOutput) {
            m_pendingTasks ^= ReleaseAudioOutput;
            m_executingTask = ReleaseAudioOutput;

            doReleaseAudioOutput(&locker);
        } else if (m_pendingTasks & ReleaseVideoOutput) {
            m_pendingTasks ^= ReleaseVideoOutput;
            m_executingTask = ReleaseVideoOutput;

            doReleaseVideoOutput(&locker);
        } else if (m_pendingTasks & SetUrlSource) {
            m_pendingTasks ^= SetUrlSource;
            m_executingTask = SetUrlSource;

            doSetUrlSource(&locker);
        } else if (m_pendingTasks & SetStreamSource) {
            m_pendingTasks ^= SetStreamSource;
            m_executingTask = SetStreamSource;

            doSetStreamSource(&locker);
        } else if (m_pendingTasks & Render) {
            m_pendingTasks ^= Render;
            m_executingTask = Render;

            doRender(&locker);
        } else if (!(m_executedTasks & Render)) {
            m_pendingTasks &= ~(FinalizeLoad | SetRate | Stop | Pause | Seek | Play);
        } else if (m_pendingTasks & FinalizeLoad) {
            m_pendingTasks ^= FinalizeLoad;
            m_executingTask = FinalizeLoad;

            doFinalizeLoad(&locker);
        } else if (m_pendingTasks & Stop) {
            m_pendingTasks ^= Stop;
            m_executingTask = Stop;

            doStop(&locker);
        } else if (m_pendingTasks & Pause) {
            m_pendingTasks ^= Pause;
            m_executingTask = Pause;

            doPause(&locker);
        } else if (m_pendingTasks & SetRate) {
            m_pendingTasks ^= SetRate;
            m_executingTask = SetRate;

            doSetRate(&locker);
        } else if (m_pendingTasks & Seek) {
            m_pendingTasks ^= Seek;
            m_executingTask = Seek;

            doSeek(&locker);
        } else if (m_pendingTasks & Play) {
            m_pendingTasks ^= Play;
            m_executingTask = Play;

            doPlay(&locker);
        }
        m_executingTask = 0;
    }
}

QT_END_NAMESPACE
