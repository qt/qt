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

#ifndef DIRECTSHOWPLAYERSERVICE_H
#define DIRECTSHOWPLAYERSERVICE_H

#include <QtMultimedia/qmediaplayer.h>
#include <QtMultimedia/qmediaresource.h>
#include <QtMultimedia/qmediaservice.h>
#include <QtMultimedia/qmediatimerange.h>

#include "directshoweventloop.h"
#include "directshowglobal.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qmutex.h>
#include <QtCore/qurl.h>
#include <QtCore/qwaitcondition.h>

#include <QtCore/private/qwineventnotifier_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class DirectShowAudioEndpointControl;
class DirectShowMetaDataControl;
class DirectShowPlayerControl;
class DirectShowVideoOutputControl;
class DirectShowVideoRendererControl;
class Vmr9VideoWindowControl;

class DirectShowPlayerService : public QMediaService
{
    Q_OBJECT
public:
    enum StreamType
    {
        AudioStream = 0x01,
        VideoStream = 0x02
    };

    DirectShowPlayerService(QObject *parent = 0);
    ~DirectShowPlayerService();

    QMediaControl* control(const char *name) const;

    void load(const QMediaContent &media, QIODevice *stream);
    void play();
    void pause();
    void stop();

    qint64 position() const;
    QMediaTimeRange availablePlaybackRanges() const;

    void seek(qint64 position);
    void setRate(qreal rate);

    int bufferStatus() const;

    void setAudioOutput(IBaseFilter *filter);
    void setVideoOutput(IBaseFilter *filter);

protected:
    void customEvent(QEvent *event);

private Q_SLOTS:
    void videoOutputChanged();

private:
    void releaseGraph();
    void updateStatus();

    int findStreamTypes(IBaseFilter *source) const;
    int findStreamType(IPin *pin) const;

    bool isConnected(IBaseFilter *filter, PIN_DIRECTION direction) const;
    IBaseFilter *getConnected(IBaseFilter *filter, PIN_DIRECTION direction) const;

    void run();

    void doSetUrlSource(QMutexLocker *locker);
    void doSetStreamSource(QMutexLocker *locker);
    void doRender(QMutexLocker *locker);
    void doFinalizeLoad(QMutexLocker *locker);
    void doSetRate(QMutexLocker *locker);
    void doSeek(QMutexLocker *locker);
    void doPlay(QMutexLocker *locker);
    void doPause(QMutexLocker *locker);
    void doStop(QMutexLocker *locker);
    void doReleaseAudioOutput(QMutexLocker *locker);
    void doReleaseVideoOutput(QMutexLocker *locker);
    void doReleaseGraph(QMutexLocker *locker);

    void graphEvent(QMutexLocker *locker);

    enum Task
    {
        Shutdown           = 0x0001,
        SetUrlSource       = 0x0002,
        SetStreamSource    = 0x0004,
        SetSource          = SetUrlSource | SetStreamSource,
        SetAudioOutput     = 0x0008,
        SetVideoOutput     = 0x0010,
        SetOutputs         = SetAudioOutput | SetVideoOutput,
        Render             = 0x0020,
        FinalizeLoad       = 0x0040,
        SetRate            = 0x0080,
        Seek               = 0x0100,
        Play               = 0x0200,
        Pause              = 0x0400,
        Stop               = 0x0800,
        ReleaseGraph       = 0x1000,
        ReleaseAudioOutput = 0x2000,
        ReleaseVideoOutput = 0x4000,
        ReleaseFilters     = ReleaseGraph | ReleaseAudioOutput | ReleaseVideoOutput
    };

    enum Event
    {
        FinalizedLoad = QEvent::User,
        Error,
        RateChange,
        Started,
        Paused,
        DurationChange,
        StatusChange,
        EndOfMedia,
        PositionChange,
        VideoOutputChange
    };

    enum GraphStatus
    {
        NoMedia,
        Loading,
        Loaded,
        InvalidMedia
    };

    DirectShowPlayerControl *m_playerControl;
    DirectShowMetaDataControl *m_metaDataControl;
    DirectShowVideoOutputControl *m_videoOutputControl;
    DirectShowVideoRendererControl *m_videoRendererControl;
    Vmr9VideoWindowControl *m_videoWindowControl;
    DirectShowAudioEndpointControl *m_audioEndpointControl;

    QThread *m_taskThread;
    DirectShowEventLoop *m_loop;
    int m_pendingTasks;
    int m_executingTask;
    int m_executedTasks;
    HANDLE m_taskHandle;
    HANDLE m_eventHandle;
    GraphStatus m_graphStatus;
    QMediaPlayer::Error m_error;
    QIODevice *m_stream;
    IFilterGraph2 *m_graph;
    IBaseFilter *m_source;
    IBaseFilter *m_audioOutput;
    IBaseFilter *m_videoOutput;
    int m_streamTypes;
    qreal m_rate;
    qint64 m_position;
    qint64 m_duration;
    bool m_buffering;
    bool m_seekable;
    bool m_atEnd;
    QMediaTimeRange m_playbackRange;
    QUrl m_url;
    QMediaResourceList m_resources;
    QString m_errorString;
    QMutex m_mutex;

    friend class DirectShowPlayerServiceThread;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
