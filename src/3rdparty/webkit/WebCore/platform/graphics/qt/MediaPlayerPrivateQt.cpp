/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2009 Apple Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "MediaPlayerPrivateQt.h"

#include <limits>

// WebCore Includes
#include "CString.h"
#include "FrameView.h"
#include "GraphicsContext.h"
#include "NotImplemented.h"
#include "TimeRanges.h"
#include "Widget.h"

// WebKit Includes
#include <wtf/HashSet.h>

// Qt Includes
#include <QDebug>
#include <QPainter>
#include <QUrl>
#include <QRect>
#include <QTime>
#include <QVideoSurfaceFormat>

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
#include <QGLContext>
#endif

// Qt Multimedia Includes
#include <QMediaService>
#include <QVideoOutputControl>
#include <QVideoRendererControl>
#include <qpaintervideosurface_p.h>


//#define QT_WEBKIT_MEDIA_INSTRUMENTATION

using namespace WTF;

namespace WebCore {

#ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
class MediaPlayerPrivateInstrumentation
{
public:
    MediaPlayerPrivateInstrumentation()
        : update(false)
        , frameCount(0)
        , paintTimeTotal(0)
        , framesPerSecond(0)
        , averagePaintTime(0)
        , preTime(QTime())
        , lastUpdate(QTime::currentTime()) {}

    void prePaint() { preTime = QTime::currentTime(); }

    void postPaint()
    {
        QTime time = QTime::currentTime();

        // Measure seconds different
        int paintTime = (time.second() - preTime.second() - 1) * 1000;
        paintTime = qMax(0, paintTime);

        // Measure milliseconds different
        if(time.msec() < preTime.msec())
            paintTime += time.msec() + (1000 - preTime.msec());
        else
            paintTime += time.msec() - preTime.msec();

        // Add to the total paint time for this second
        paintTimeTotal += paintTime;

        // Check for update time
        if(time.second() != lastUpdate.second()) {
            // Calculate Values
            framesPerSecond = frameCount;
            averagePaintTime = static_cast<qreal>(paintTimeTotal) / static_cast<qreal>(frameCount);

            // Next Update
            update = true;
            frameCount = 1;
            paintTimeTotal = 0;
            lastUpdate = time;
        } else {
            frameCount++;
        }
    }

    bool updateAvailable() { return update; }

    int fps() { update = false; return framesPerSecond; }
    qreal paintTime() { update = false; return averagePaintTime; }

private:
    bool update;

    int frameCount;
    qint64 paintTimeTotal;

    int framesPerSecond;
    qreal averagePaintTime;

    QTime preTime;
    QTime lastUpdate;
};
#endif // QT_WEBKIT_MEDIA_INSTRUMENTATION

class MediaPlayerPrivateDefault : public MediaPlayerPrivate {
public:
    IntSize naturalSize() const;
    void setSize(const IntSize&) {}
    void paint(GraphicsContext*, const IntRect&);
    bool supportsFullscreen() const { return true; }

private slots:
    void repaint();

private:
    friend class MediaPlayerPrivate;
    MediaPlayerPrivateDefault(MediaPlayer*);
    ~MediaPlayerPrivateDefault();

    QPainterVideoSurface* m_videoSurface;
    QVideoOutputControl* m_outputControl;
    QVideoRendererControl* m_rendererControl;

    bool m_isActive;

#ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
    MediaPlayerPrivateInstrumentation* m_instruments;
#endif
};

MediaPlayerPrivateInterface* MediaPlayerPrivate::create(MediaPlayer* player)
{
    return new MediaPlayerPrivateDefault(player);
}

void MediaPlayerPrivate::registerMediaEngine(MediaEngineRegistrar registrar)
{
    registrar(create, getSupportedTypes, supportsType);
}

void MediaPlayerPrivate::getSupportedTypes(HashSet<String> &supported)
{
    QStringList types = QMediaPlayer::supportedMimeTypes();

    for (int i = 0; i < types.size(); i++) {
        supported.add(types.at(i));
    }
}

MediaPlayer::SupportsType MediaPlayerPrivate::supportsType(const String& mime, const String& codec)
{
    if(QMediaPlayer::hasSupport(mime, QStringList(codec)) >= QtMedia::ProbablySupported) {
        return MediaPlayer::IsSupported;
    }

    return MediaPlayer::MayBeSupported;
}

MediaPlayerPrivate::MediaPlayerPrivate(MediaPlayer* player)
    : m_player(player)
    , m_mediaPlayer(new QMediaPlayer)
    , m_networkState(MediaPlayer::Empty)
    , m_readyState(MediaPlayer::HaveNothing)
    , m_isVisible(false)
    , m_endTime(std::numeric_limits<qint64>::max())
{
    // Signal Handlers
    connect(m_mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(stateChanged(QMediaPlayer::State)));
    connect(m_mediaPlayer, SIGNAL(error(QMediaPlayer::Error)),
            this, SLOT(handleError(QMediaPlayer::Error)));
    connect(m_mediaPlayer, SIGNAL(durationChanged(qint64)),
            this, SLOT(durationChanged(qint64)));
    connect(m_mediaPlayer, SIGNAL(positionChanged(qint64)),
            this, SLOT(positionChanged(qint64)));

    // Grab the player control
    if (m_mediaPlayer) {
        QMediaService *service = m_mediaPlayer->service();
        if (service) {
            m_mediaPlayerControl = qobject_cast<QMediaPlayerControl *>(
                    service->control(QMediaPlayerControl_iid));
        }
    }
}

MediaPlayerPrivate::~MediaPlayerPrivate()
{
    delete m_mediaPlayer;
    m_mediaPlayer = 0;
}

bool MediaPlayerPrivate::hasVideo() const
{
    return m_mediaPlayer->isVideoAvailable();
}

bool MediaPlayerPrivate::hasAudio() const
{
    return true;
}

void MediaPlayerPrivate::load(const String& url)
{
    // We are now loading
    if (m_networkState != MediaPlayer::Loading) {
        m_networkState = MediaPlayer::Loading;
        m_player->networkStateChanged();
    }

    // And we don't have any data yet
    if (m_readyState != MediaPlayer::HaveNothing) {
        m_readyState = MediaPlayer::HaveNothing;
        m_player->readyStateChanged();
    }

    m_mediaPlayer->setMedia(QUrl(url));
}

void MediaPlayerPrivate::cancelLoad()
{
    m_mediaPlayer->setMedia(QMediaContent());
    updateStates();
}

void MediaPlayerPrivate::play()
{
    if(m_mediaPlayer->state() != QMediaPlayer::PlayingState) {
        m_mediaPlayer->play();
    }
}

void MediaPlayerPrivate::pause()
{
    if(m_mediaPlayer->state() == QMediaPlayer::PlayingState) {
        m_mediaPlayer->pause();
    }
}

bool MediaPlayerPrivate::paused() const
{
    return m_mediaPlayer->state() != QMediaPlayer::PlayingState;
}

void MediaPlayerPrivate::seek(float position)
{
    if (!m_mediaPlayer->isSeekable())
        return;

    if (m_mediaPlayerControl != 0 &&
        !m_mediaPlayerControl->availablePlaybackRanges().contains(position * 1000))
        return;

    if (position > duration())
        position = duration();

    m_mediaPlayer->setPosition(position * 1000.0f);
}

bool MediaPlayerPrivate::seeking() const
{
    return false;
}

void MediaPlayerPrivate::setEndTime(float time)
{
    // Set the end time in milliseconds
    m_endTime = static_cast<qint64>(time * 1000.0f);

    // Duration may return a smaller value as of now
    m_player->durationChanged();
}

float MediaPlayerPrivate::duration() const
{
    if (m_readyState < MediaPlayer::HaveMetadata)
        return 0.0f;

    float duration = qMin(m_mediaPlayer->duration(), m_endTime) / 1000.0f;

    // We are streaming
    if (duration <= 0.0f)
        duration = std::numeric_limits<float>::infinity();

    return duration;
}

float MediaPlayerPrivate::currentTime() const
{
    float currentTime = m_mediaPlayer->position() / 1000.0f;
    return currentTime;
}

PassRefPtr<TimeRanges> MediaPlayerPrivate::buffered() const
{
    RefPtr<TimeRanges> buffered = TimeRanges::create();

    if(m_mediaPlayerControl == 0)
        return buffered;

    QMediaTimeRange playbackRanges = m_mediaPlayerControl->availablePlaybackRanges();

    foreach(const QMediaTimeInterval interval, playbackRanges.intervals()) {
        float rangeMin = static_cast<float>(interval.start()) / 1000.0f;
        float rangeMax = static_cast<float>(interval.end()) / 1000.0f;
        buffered->add(rangeMin, rangeMax);
    }

    return buffered.release();
}

float MediaPlayerPrivate::maxTimeSeekable() const
{
    if(m_mediaPlayerControl == 0)
        return 0;

    return static_cast<float>(m_mediaPlayerControl->availablePlaybackRanges().latestTime()) / 1000.0f;
}

unsigned MediaPlayerPrivate::bytesLoaded() const
{
    unsigned percentage = m_mediaPlayer->bufferStatus();

    if(percentage == 100) {
        if(m_networkState != MediaPlayer::Idle) {
            m_networkState = MediaPlayer::Idle;
            m_player->networkStateChanged();
        }
        if(m_readyState != MediaPlayer::HaveEnoughData) {
            m_readyState = MediaPlayer::HaveEnoughData;
            m_player->readyStateChanged();
        }
    }

    QLatin1String blKey("bytes-loaded");
    if(m_mediaPlayer->availableExtendedMetaData().contains(blKey))
        return m_mediaPlayer->extendedMetaData(blKey).toInt();

    return percentage;
}

unsigned MediaPlayerPrivate::totalBytes() const
{
    if(m_mediaPlayer->availableMetaData().contains(QtMedia::Size))
        return m_mediaPlayer->metaData(QtMedia::Size).toInt();

    return 100;
}

bool MediaPlayerPrivate::totalBytesKnown() const
{
    return m_mediaPlayer->availableMetaData().contains(QtMedia::Size);
}

void MediaPlayerPrivate::setRate(float rate)
{
    m_mediaPlayer->setPlaybackRate(rate);
}

void MediaPlayerPrivate::setVolume(float volume)
{
    m_mediaPlayer->setVolume(static_cast<int>(volume * 100.0));
}

void MediaPlayerPrivate::setMuted(bool muted)
{
    m_mediaPlayer->setMuted(muted);
}

int MediaPlayerPrivate::dataRate() const
{
    return 0;
}

MediaPlayer::NetworkState MediaPlayerPrivate::networkState() const
{
    return m_networkState;
}

MediaPlayer::ReadyState MediaPlayerPrivate::readyState() const
{
    return m_readyState;
}

void MediaPlayerPrivate::setVisible(bool visible)
{
    m_isVisible = visible;
}

void MediaPlayerPrivate::mediaStatusChanged(QMediaPlayer::MediaStatus)
{
    updateStates();
}

void MediaPlayerPrivate::handleError(QMediaPlayer::Error)
{
    updateStates();
}

void MediaPlayerPrivate::durationChanged(qint64)
{
    m_player->durationChanged();
}

void MediaPlayerPrivate::updateStates()
{
    // Store the old states so that we can detect a change and raise change events
    MediaPlayer::NetworkState oldNetworkState = m_networkState;
    MediaPlayer::ReadyState oldReadyState = m_readyState;

    QMediaPlayer::MediaStatus currentStatus = m_mediaPlayer->mediaStatus();
    QMediaPlayer::Error currentError = m_mediaPlayer->error();

    if(currentError != QMediaPlayer::NoError) {
        m_readyState = MediaPlayer::HaveNothing;
        if(currentError == QMediaPlayer::FormatError) {
            m_networkState = MediaPlayer::FormatError;
        } else {
            // ResourceError, NetworkError, AccessDeniedError, ServiceMissingError
            m_networkState = MediaPlayer::NetworkError;
        }
    } else if(currentStatus == QMediaPlayer::UnknownMediaStatus ||
            currentStatus == QMediaPlayer::NoMedia) {
        m_networkState = MediaPlayer::Idle;
        m_readyState = MediaPlayer::HaveNothing;
    } else if(currentStatus == QMediaPlayer::LoadingMedia) {
        m_networkState = MediaPlayer::Loading;
        m_readyState = MediaPlayer::HaveNothing;
    } else if(currentStatus == QMediaPlayer::LoadedMedia) {
        m_networkState = MediaPlayer::Loading;
        m_readyState = MediaPlayer::HaveMetadata;
    } else if(currentStatus == QMediaPlayer::BufferingMedia) {
        m_networkState = MediaPlayer::Loading;
        m_readyState = MediaPlayer::HaveFutureData;
    } else if(currentStatus == QMediaPlayer::StalledMedia) {
        m_networkState = MediaPlayer::Loading;
        m_readyState = MediaPlayer::HaveCurrentData;
    } else if(currentStatus == QMediaPlayer::BufferedMedia ||
            currentStatus == QMediaPlayer::EndOfMedia) {
        m_networkState = MediaPlayer::Idle;
        m_readyState = MediaPlayer::HaveEnoughData;
    } else if(currentStatus == QMediaPlayer::InvalidMedia) {
        m_networkState = MediaPlayer::FormatError;
        m_readyState = MediaPlayer::HaveMetadata;
    }

    // Log the state changes and raise the state change events
    if (m_networkState != oldNetworkState) {
        m_player->networkStateChanged();
    }

    if (m_readyState != oldReadyState) {
        m_player->readyStateChanged();
    }
}

MediaPlayerPrivateDefault::MediaPlayerPrivateDefault(MediaPlayer *player)
    : MediaPlayerPrivate(player)
    , m_videoSurface(new QPainterVideoSurface(m_mediaPlayer))
{
#ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
    m_instruments = new MediaPlayerPrivateInstrumentation;
#endif

    if (m_mediaPlayer) {
        QMediaService *service = m_mediaPlayer->service();
        if (service) {
            m_outputControl = qobject_cast<QVideoOutputControl *>(
                    service->control(QVideoOutputControl_iid));
            m_rendererControl = qobject_cast<QVideoRendererControl *>(
                    service->control(QVideoRendererControl_iid));

            if (m_outputControl != 0 && m_rendererControl != 0) {
                connect(m_videoSurface, SIGNAL(frameChanged()), this, SLOT(repaint()));

                m_rendererControl->setSurface(m_videoSurface);
                m_outputControl->setOutput(QVideoOutputControl::RendererOutput);
            }
        }
    }
}

MediaPlayerPrivateDefault::~MediaPlayerPrivateDefault()
{
#ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
    delete m_instruments;
    m_instruments = 0;
#endif
}

IntSize MediaPlayerPrivateDefault::naturalSize() const
{
    if (!hasVideo() || m_readyState < MediaPlayer::HaveMetadata)
        return IntSize();

    QSize videoSize = m_videoSurface->surfaceFormat().sizeHint();
    return IntSize(videoSize.width(), videoSize.height());
}

void MediaPlayerPrivateDefault::paint(GraphicsContext* context, const IntRect& rect)
{
    if (context->paintingDisabled())
        return;

    if (!m_isVisible)
        return;

#ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
    m_instruments->prePaint();
#endif

    // Grab the painter
    QPainter* painter = context->platformContext();

    if(!m_isActive) {
        // Attempt GL Setup
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
        if ((painter->paintEngine()->type() == QPaintEngine::OpenGL ||
             painter->paintEngine()->type() == QPaintEngine::OpenGL2)) {
            // Set GL Context
            m_videoSurface->setGLContext(const_cast<QGLContext *>(QGLContext::currentContext()));

            // Check Shader Types
            if (m_videoSurface->supportedShaderTypes() & QPainterVideoSurface::GlslShader) {
                m_videoSurface->setShaderType(QPainterVideoSurface::GlslShader);
            } else {
                m_videoSurface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
            }
        }
#endif

        // Setup Completed
        m_isActive = true;
    }

    // Paint the frame
    m_videoSurface->paint(painter, QRect(rect.x(), rect.y(),
    rect.width(), rect.height()));
    m_videoSurface->setReady(true);

    #ifdef QT_WEBKIT_MEDIA_INSTRUMENTATION
    m_instruments->postPaint();

    if(m_instruments->updateAvailable()) {
        qDebug("FPS: %d, Average Paint Time: %f", m_instruments->fps(), m_instruments->paintTime());
    }
    #endif
}

void MediaPlayerPrivateDefault::repaint()
{
    m_player->repaint();
}

} // namespace WebCore

#include "moc_MediaPlayerPrivateQt.cpp"
