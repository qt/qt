/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qcoreapplication.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtNetwork>
#include <QTime>

#include "wavedecoder_p.h"

#include "qsoundeffect_pulse_p.h"

#if defined(Q_WS_MAEMO_5)
#include <pulse/ext-stream-restore.h>
#endif

#include <unistd.h>

// Less than ideal
#define PA_SCACHE_ENTRY_SIZE_MAX (1024*1024*16)

QT_BEGIN_NAMESPACE

namespace
{
inline pa_sample_spec audioFormatToSampleSpec(const QAudioFormat &format)
{
    pa_sample_spec  spec;

    spec.rate = format.frequency();
    spec.channels = format.channels();

    if (format.sampleSize() == 8)
        spec.format = PA_SAMPLE_U8;
    else if (format.sampleSize() == 16) {
        switch (format.byteOrder()) {
            case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S16BE; break;
            case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S16LE; break;
        }
    }
    else if (format.sampleSize() == 32) {
        switch (format.byteOrder()) {
            case QAudioFormat::BigEndian: spec.format = PA_SAMPLE_S32BE; break;
            case QAudioFormat::LittleEndian: spec.format = PA_SAMPLE_S32LE; break;
        }
    }

    return spec;
}

class PulseDaemon
{
public:
    PulseDaemon():m_prepared(false)
    {
        prepare();
    }

    ~PulseDaemon()
    {
        if (m_prepared)
            release();
    }

    inline void lock()
    {
        pa_threaded_mainloop_lock(m_mainLoop);
    }

    inline void unlock()
    {
        pa_threaded_mainloop_unlock(m_mainLoop);
    }

    inline pa_context *context() const
    {
        return m_context;
    }

    int volume()
    {
        return m_vol;
    }

private:
    void prepare()
    {
        m_vol = 100;

        m_mainLoop = pa_threaded_mainloop_new();
        if (m_mainLoop == 0) {
            qWarning("PulseAudioService: unable to create pulseaudio mainloop");
            return;
        }

        if (pa_threaded_mainloop_start(m_mainLoop) != 0) {
            qWarning("PulseAudioService: unable to start pulseaudio mainloop");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

        lock();
        m_context = pa_context_new(m_mainLoopApi, QString(QLatin1String("QtPulseAudio:%1")).arg(::getpid()).toAscii().constData());

#if defined(Q_WS_MAEMO_5)
        pa_context_set_state_callback(m_context, context_state_callback, this);
#endif
        if (m_context == 0) {
            qWarning("PulseAudioService: Unable to create new pulseaudio context");
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }

        if (pa_context_connect(m_context, NULL, (pa_context_flags_t)0, NULL) < 0) {
            qWarning("PulseAudioService: pa_context_connect() failed");
            pa_context_unref(m_context);
            pa_threaded_mainloop_free(m_mainLoop);
            return;
        }
        unlock();

        m_prepared = true;
    }

    void release()
    {
        if (!m_prepared) return;
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
        m_prepared = false;
    }

#if defined(Q_WS_MAEMO_5)
    static void context_state_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        switch (pa_context_get_state(c)) {
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
                break;
            case PA_CONTEXT_READY:
                pa_ext_stream_restore_set_subscribe_cb(c, &stream_restore_monitor_callback, self);
                pa_ext_stream_restore_subscribe(c, 1, NULL, self);
                break;
            default:
                break;
        }
    }
    static void stream_restore_monitor_callback(pa_context *c, void *userdata)
    {
        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);
        pa_ext_stream_restore2_read(c, &stream_restore_info_callback, self);
    }
    static void stream_restore_info_callback(pa_context *c, const pa_ext_stream_restore2_info *info,
            int eol, void *userdata)
    {
        Q_UNUSED(c)

        PulseDaemon *self = reinterpret_cast<PulseDaemon*>(userdata);

        if (!eol) {
            if (QString(info->name).startsWith(QLatin1String("sink-input-by-media-role:x-maemo"))) {
                const unsigned str_length = 256;
                char str[str_length];
                pa_cvolume_snprint(str, str_length, &info->volume);
                self->m_vol = QString(str).replace(" ","").replace("%","").mid(2).toInt();
            }
        }
    }
#endif

    int  m_vol;
    bool m_prepared;
    pa_context *m_context;
    pa_threaded_mainloop *m_mainLoop;
    pa_mainloop_api *m_mainLoopApi;
};
}

Q_GLOBAL_STATIC(PulseDaemon, daemon)


QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_retry(false),
    m_muted(false),
    m_playQueued(false),
    m_sampleLoaded(false),
    m_volume(100),
    m_duration(0),
    m_dataUploaded(0),
    m_loopCount(1),
    m_runningCount(0),
    m_reply(0),
    m_stream(0),
    m_networkAccessManager(0)
{
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
    m_reply->deleteLater();
    unloadSample();
}

QUrl QSoundEffectPrivate::source() const
{
    return m_source;
}

void QSoundEffectPrivate::setSource(const QUrl &url)
{
    if (url.isEmpty()) {
        m_source = QUrl();
        unloadSample();
        return;
    }

    m_source = url;

    if (m_networkAccessManager == 0)
        m_networkAccessManager = new QNetworkAccessManager(this);

    m_stream = m_networkAccessManager->get(QNetworkRequest(m_source));

    unloadSample();
    loadSample();
}

int QSoundEffectPrivate::loopCount() const
{
    return m_loopCount;
}

void QSoundEffectPrivate::setLoopCount(int loopCount)
{
    m_loopCount = loopCount;
}

int QSoundEffectPrivate::volume() const
{
    return m_volume;
}

void QSoundEffectPrivate::setVolume(int volume)
{
    m_volume = volume;
}

bool QSoundEffectPrivate::isMuted() const
{
    return m_muted;
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_muted = muted;
}

void QSoundEffectPrivate::play()
{
    if (m_retry) {
        m_retry = false;
        setSource(m_source);
        return;
    }

    if (!m_sampleLoaded) {
        m_playQueued = true;
        return;
    }

    m_runningCount += m_loopCount;

    playSample();
}

void QSoundEffectPrivate::decoderReady()
{
    if (m_waveDecoder->size() >= PA_SCACHE_ENTRY_SIZE_MAX) {
        m_waveDecoder->deleteLater();
        qWarning("QSoundEffect(pulseaudio): Attempting to load to large a sample");
        return;
    }

    if (m_name.isNull())
        m_name = QString(QLatin1String("QtPulseSample-%1-%2")).arg(::getpid()).arg(quintptr(this)).toUtf8();

    pa_sample_spec spec = audioFormatToSampleSpec(m_waveDecoder->audioFormat());

    daemon()->lock();
    pa_stream *stream = pa_stream_new(daemon()->context(), m_name.constData(), &spec, 0);
    pa_stream_set_state_callback(stream, stream_state_callback, this);
    pa_stream_set_write_callback(stream, stream_write_callback, this);
    pa_stream_connect_upload(stream, (size_t)m_waveDecoder->size());
    m_pulseStream = stream;
    daemon()->unlock();
}

void QSoundEffectPrivate::decoderError()
{
    qWarning("QSoundEffect(pulseaudio): Error decoding source");
}

void QSoundEffectPrivate::checkPlayTime()
{
    int elapsed = m_playbackTime.elapsed();

    if (elapsed < m_duration)
        startTimer(m_duration - elapsed);
}

void QSoundEffectPrivate::loadSample()
{
    m_sampleLoaded = false;
    m_dataUploaded = 0;
    m_waveDecoder = new WaveDecoder(m_stream);
    connect(m_waveDecoder, SIGNAL(formatKnown()), SLOT(decoderReady()));
    connect(m_waveDecoder, SIGNAL(invalidFormat()), SLOT(decoderError()));
}

void QSoundEffectPrivate::unloadSample()
{
    if (!m_sampleLoaded)
        return;

    daemon()->lock();
    pa_context_remove_sample(daemon()->context(), m_name.constData(), NULL, NULL);
    daemon()->unlock();

    m_duration = 0;
    m_dataUploaded = 0;
    m_sampleLoaded = false;
}

void QSoundEffectPrivate::uploadSample()
{
    daemon()->lock();

    size_t bufferSize = qMin(pa_stream_writable_size(m_pulseStream),
            size_t(m_waveDecoder->bytesAvailable()));
    char buffer[bufferSize];

    size_t len = 0;
    while (len < (m_waveDecoder->size())) {
        qint64 read = m_waveDecoder->read(buffer, qMin((int)bufferSize,
                      (int)(m_waveDecoder->size()-len)));
        if (read > 0) {
            if (pa_stream_write(m_pulseStream, buffer, size_t(read), 0, 0, PA_SEEK_RELATIVE) == 0)
                len += size_t(read);
            else
                break;
        }
    }

    m_dataUploaded += len;
    pa_stream_set_write_callback(m_pulseStream, NULL, NULL);

    if (m_waveDecoder->size() == m_dataUploaded) {
        int err = pa_stream_finish_upload(m_pulseStream);
        if(err != 0) {
            qWarning("pa_stream_finish_upload() err=%d",err);
            pa_stream_disconnect(m_pulseStream);
            m_retry = true;
            m_playQueued = false;
            QMetaObject::invokeMethod(this, "play");
            daemon()->unlock();
            return;
        }
        m_duration = m_waveDecoder->duration();
        m_waveDecoder->deleteLater();
        m_stream->deleteLater();
        m_sampleLoaded = true;
        if (m_playQueued) {
            m_playQueued = false;
            QMetaObject::invokeMethod(this, "play");
        }
    }
    daemon()->unlock();
}

void QSoundEffectPrivate::playSample()
{
    pa_volume_t volume = PA_VOLUME_NORM;

    daemon()->lock();
#ifdef Q_WS_MAEMO_5
    volume = PA_VOLUME_NORM / 100 * ((daemon()->volume() + m_volume) / 2);
#endif
    pa_operation_unref(
            pa_context_play_sample(daemon()->context(),
                m_name.constData(),
                0,
                volume,
                play_callback,
                this)
            );
    daemon()->unlock();

    m_playbackTime.start();
}

void QSoundEffectPrivate::timerEvent(QTimerEvent *event)
{
    if (m_runningCount > 0)
        playSample();

    killTimer(event->timerId());
}

void QSoundEffectPrivate::stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
    Q_UNUSED(length);

    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);

    QMetaObject::invokeMethod(self, "uploadSample", Qt::QueuedConnection);
}

void QSoundEffectPrivate::stream_state_callback(pa_stream *s, void *userdata)
{
    switch (pa_stream_get_state(s)) {
        case PA_STREAM_CREATING:
        case PA_STREAM_READY:
        case PA_STREAM_TERMINATED:
            break;

        case PA_STREAM_FAILED:
        default:
            qWarning("QSoundEffect(pulseaudio): Error in pulse audio stream");
            break;
    }
}

void QSoundEffectPrivate::play_callback(pa_context *c, int success, void *userdata)
{
    Q_UNUSED(c);

    QSoundEffectPrivate *self = reinterpret_cast<QSoundEffectPrivate*>(userdata);

    if (success == 1) {
        self->m_runningCount--;
        QMetaObject::invokeMethod(self, "checkPlayTime", Qt::QueuedConnection);
    }
}

QT_END_NAMESPACE
