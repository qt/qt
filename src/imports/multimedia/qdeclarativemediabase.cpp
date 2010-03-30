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

#include "qdeclarativemediabase_p.h"

#include <QtCore/qcoreevent.h>
#include <QtCore/qurl.h>

#include <QtMultimedia/qmediaplayercontrol.h>
#include <QtMultimedia/qmediaservice.h>
#include <QtMultimedia/qmediaserviceprovider.h>
#include <QtMultimedia/qmetadatacontrol.h>
#include "qmetadatacontrolmetaobject_p.h"



QT_BEGIN_NAMESPACE


class QDeclarativeMediaBaseObject : public QMediaObject
{
public:
    QDeclarativeMediaBaseObject(QMediaService *service)
        : QMediaObject(0, service)
    {
    }
};

class QDeclarativeMediaBasePlayerControl : public QMediaPlayerControl
{
public:
    QDeclarativeMediaBasePlayerControl(QObject *parent)
        : QMediaPlayerControl(parent)
    {
    }

    QMediaPlayer::State state() const { return QMediaPlayer::StoppedState; }
    QMediaPlayer::MediaStatus mediaStatus() const { return QMediaPlayer::NoMedia; }

    qint64 duration() const { return 0; }
    qint64 position() const { return 0; }
    void setPosition(qint64) {}
    int volume() const { return 0; }
    void setVolume(int) {}
    bool isMuted() const { return false; }
    void setMuted(bool) {}
    int bufferStatus() const { return 0; }
    bool isAudioAvailable() const { return false; }
    bool isVideoAvailable() const { return false; }
    bool isSeekable() const { return false; }
    QMediaTimeRange availablePlaybackRanges() const { return QMediaTimeRange(); }
    qreal playbackRate() const { return 1; }
    void setPlaybackRate(qreal) {}
    QMediaContent media() const { return QMediaContent(); }
    const QIODevice *mediaStream() const { return 0; }
    void setMedia(const QMediaContent &, QIODevice *) {}

    void play() {}
    void pause() {}
    void stop() {}
};

class QDeclarativeMediaBaseAnimation : public QObject
{
public:
    QDeclarativeMediaBaseAnimation(QDeclarativeMediaBase *media)
        : m_media(media)
    {
    }

    void start() { if (!m_timer.isActive()) m_timer.start(500, this); }
    void stop() { m_timer.stop(); }

protected:
    void timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == m_timer.timerId()) {
            event->accept();

            if (m_media->m_state == QMediaPlayer::PlayingState)
                emit m_media->positionChanged();
            if (m_media->m_status == QMediaPlayer::BufferingMedia || QMediaPlayer::StalledMedia)
                emit m_media->bufferProgressChanged();
        } else {
            QObject::timerEvent(event);
        }
    }

private:
    QDeclarativeMediaBase *m_media;
    QBasicTimer m_timer;
};

void QDeclarativeMediaBase::_q_stateChanged(QMediaPlayer::State state)
{
    if (m_state == state)
        return;

    switch (state) {
    case QMediaPlayer::StoppedState: {
            emit stopped();

            if (m_playing) {
                m_playing = false;
                emit playingChanged();
            }
        }
        break;
    case QMediaPlayer::PausedState: {
            emit paused();

            if (!m_paused) {
                m_paused = true;
                emit pausedChanged();
            }

            if (m_state == QMediaPlayer::StoppedState)
                emit started();
        }
        break;
    case QMediaPlayer::PlayingState: {
            if (m_state == QMediaPlayer::PausedState)
                emit resumed();
            else
                emit started();

           if (m_paused) {
               m_paused = false;
               emit pausedChanged();
           }
        }
        break;
    }

    // Check
    if (state == QMediaPlayer::PlayingState
            || m_status == QMediaPlayer::BufferingMedia
            || m_status == QMediaPlayer::StalledMedia) {
        m_animation->start();
    }
    else {
        m_animation->stop();
    }

    m_state = state;
}

void QDeclarativeMediaBase::_q_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status != m_status) {
        m_status = status;

        switch (status) {
        case QMediaPlayer::LoadedMedia:
            emit loaded();
            break;
        case QMediaPlayer::BufferingMedia:
            emit buffering();
            break;
        case QMediaPlayer::BufferedMedia:
            emit buffered();
            break;
        case QMediaPlayer::StalledMedia:
            emit stalled();
            break;
        case QMediaPlayer::EndOfMedia:
            emit endOfMedia();
            break;
        default:
            break;
        }

        emit statusChanged();

        if (m_state == QMediaPlayer::PlayingState
                || m_status == QMediaPlayer::BufferingMedia
                || m_status == QMediaPlayer::StalledMedia) {
            m_animation->start();
        } else {
            m_animation->stop();
        }
    }
}

void QDeclarativeMediaBase::_q_metaDataChanged()
{
    m_metaObject->metaDataChanged();
}

QDeclarativeMediaBase::QDeclarativeMediaBase()
    : m_paused(false)
    , m_playing(false)
    , m_autoLoad(true)
    , m_loaded(false)
    , m_muted(false)
    , m_position(0)
    , m_vol(1.0)
    , m_playbackRate(1.0)
    , m_mediaService(0)
    , m_playerControl(0)
    , m_mediaObject(0)
    , m_mediaProvider(0)
    , m_metaDataControl(0)
    , m_metaObject(0)
    , m_animation(0)
    , m_state(QMediaPlayer::StoppedState)
    , m_status(QMediaPlayer::NoMedia)
    , m_error(QMediaPlayer::ServiceMissingError)
{
}

QDeclarativeMediaBase::~QDeclarativeMediaBase()
{
}

void QDeclarativeMediaBase::shutdown()
{
    delete m_metaObject;
    delete m_mediaObject;

    if (m_mediaProvider)
        m_mediaProvider->releaseService(m_mediaService);

    delete m_animation;

}

void QDeclarativeMediaBase::setObject(QObject *object)
{
    if ((m_mediaProvider = QMediaServiceProvider::defaultServiceProvider())) {
        if ((m_mediaService = m_mediaProvider->requestService(Q_MEDIASERVICE_MEDIAPLAYER))) {
            m_playerControl = qobject_cast<QMediaPlayerControl *>(
                    m_mediaService->control(QMediaPlayerControl_iid));
            m_metaDataControl = qobject_cast<QMetaDataControl *>(
                    m_mediaService->control(QMetaDataControl_iid));
            m_mediaObject = new QDeclarativeMediaBaseObject(m_mediaService);
        }
    }

    if (m_playerControl) {
        QObject::connect(m_playerControl, SIGNAL(stateChanged(QMediaPlayer::State)),
                object, SLOT(_q_stateChanged(QMediaPlayer::State)));
        QObject::connect(m_playerControl, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                object, SLOT(_q_mediaStatusChanged(QMediaPlayer::MediaStatus)));
        QObject::connect(m_playerControl, SIGNAL(mediaChanged(QMediaContent)),
                object, SIGNAL(sourceChanged()));
        QObject::connect(m_playerControl, SIGNAL(durationChanged(qint64)),
                object, SIGNAL(durationChanged()));
        QObject::connect(m_playerControl, SIGNAL(positionChanged(qint64)),
                object, SIGNAL(positionChanged()));
        QObject::connect(m_playerControl, SIGNAL(volumeChanged(int)),
                object, SIGNAL(volumeChanged()));
        QObject::connect(m_playerControl, SIGNAL(mutedChanged(bool)),
                object, SIGNAL(mutedChanged()));
        QObject::connect(m_playerControl, SIGNAL(bufferStatusChanged(int)),
                object, SIGNAL(bufferProgressChanged()));
        QObject::connect(m_playerControl, SIGNAL(seekableChanged(bool)),
                object, SIGNAL(seekableChanged()));
        QObject::connect(m_playerControl, SIGNAL(playbackRateChanged(qreal)),
                object, SIGNAL(playbackRateChanged()));
        QObject::connect(m_playerControl, SIGNAL(error(int,QString)),
                object, SLOT(_q_error(int,QString)));

        m_animation = new QDeclarativeMediaBaseAnimation(this);
        m_error = QMediaPlayer::NoError;
    } else {
        m_playerControl = new QDeclarativeMediaBasePlayerControl(object);
    }

    if (m_metaDataControl) {
        m_metaObject = new QMetaDataControlMetaObject(m_metaDataControl, object);

        QObject::connect(m_metaDataControl, SIGNAL(metaDataChanged()),
                object, SLOT(_q_metaDataChanged()));
    }

    // Init
    m_playerControl->setVolume(m_vol * 100);
    m_playerControl->setMuted(m_muted);
    m_playerControl->setPlaybackRate(m_playbackRate);

    if (!m_source.isEmpty() && (m_autoLoad || m_playing)) // Override autoLoad if playing set
        m_playerControl->setMedia(m_source, 0);

    if (m_paused)
        m_playerControl->pause();
    else if (m_playing)
        m_playerControl->play();

    if ((m_playing || m_paused) && m_position > 0)
        m_playerControl->setPosition(m_position);
}


// Properties

QUrl QDeclarativeMediaBase::source() const
{
    return m_source;
}

void QDeclarativeMediaBase::setSource(const QUrl &url)
{
    if (url == m_source)
        return;

    m_source = url;
    m_loaded = false;
    if (m_playerControl != 0 && m_autoLoad) {
        if (m_error != QMediaPlayer::ServiceMissingError && m_error != QMediaPlayer::NoError) {
            m_error = QMediaPlayer::NoError;
            m_errorString = QString();

            emit errorChanged();
        }

        m_playerControl->setMedia(m_source, 0);
        m_loaded = true;
    }
    else
        emit sourceChanged();
}

bool QDeclarativeMediaBase::isAutoLoad() const
{
    return m_autoLoad;
}

void QDeclarativeMediaBase::setAutoLoad(bool autoLoad)
{
    if (m_autoLoad == autoLoad)
        return;

    m_autoLoad = autoLoad;
    emit autoLoadChanged();
}

bool QDeclarativeMediaBase::isPlaying() const
{
    return m_playing;
}

void QDeclarativeMediaBase::setPlaying(bool playing)
{
    if (playing == m_playing)
        return;

    m_playing = playing;
    if (m_playerControl != 0) {
        if (m_playing) {
            if (!m_autoLoad && !m_loaded) {
                m_playerControl->setMedia(m_source, 0);
                m_playerControl->setPosition(m_position);
                m_loaded = true;
            }

            if (!m_paused)
                m_playerControl->play();
            else
                m_playerControl->pause();
        }
        else if (m_state != QMediaPlayer::StoppedState)
            m_playerControl->stop();
    }

    emit playingChanged();
}

bool QDeclarativeMediaBase::isPaused() const
{
    return m_paused;
}

void QDeclarativeMediaBase::setPaused(bool paused)
{
    if (m_paused == paused)
        return;

    m_paused = paused;
    if (m_playerControl != 0) {
        if (!m_autoLoad && !m_loaded) {
            m_playerControl->setMedia(m_source, 0);
            m_playerControl->setPosition(m_position);
            m_loaded = true;
        }

        if (m_paused && m_state == QMediaPlayer::PlayingState) {
            m_playerControl->pause();
        }
        else if (!m_paused && m_playing) {
            m_playerControl->play();
        }
    }

    emit pausedChanged();
}

int QDeclarativeMediaBase::duration() const
{
    return m_playerControl == 0 ? 0 : m_playerControl->duration();
}

int QDeclarativeMediaBase::position() const
{
    return m_playerControl == 0 ? m_position : m_playerControl->position();
}

void QDeclarativeMediaBase::setPosition(int position)
{
    if (m_position == position)
        return;

    m_position = position;
    if (m_playerControl != 0)
        m_playerControl->setPosition(m_position);
    else
        emit positionChanged();
}

qreal QDeclarativeMediaBase::volume() const
{
    return m_playerControl == 0 ? m_vol : qreal(m_playerControl->volume()) / 100;
}

void QDeclarativeMediaBase::setVolume(qreal volume)
{
    if (m_vol == volume)
        return;

    m_vol = volume;

    if (m_playerControl != 0)
        m_playerControl->setVolume(qRound(volume * 100));
    else
        emit volumeChanged();
}

bool QDeclarativeMediaBase::isMuted() const
{
    return m_playerControl == 0 ? m_muted : m_playerControl->isMuted();
}

void QDeclarativeMediaBase::setMuted(bool muted)
{
    if (m_muted == muted)
        return;

    m_muted = muted;

    if (m_playerControl != 0)
        m_playerControl->setMuted(muted);
    else
        emit mutedChanged();
}

qreal QDeclarativeMediaBase::bufferProgress() const
{
    return m_playerControl == 0 ? 0 : qreal(m_playerControl->bufferStatus()) / 100;
}

bool QDeclarativeMediaBase::isSeekable() const
{
    return m_playerControl == 0 ? false : m_playerControl->isSeekable();
}

qreal QDeclarativeMediaBase::playbackRate() const
{
    return m_playbackRate;
}

void QDeclarativeMediaBase::setPlaybackRate(qreal rate)
{
    if (m_playbackRate == rate)
        return;

    m_playbackRate = rate;

    if (m_playerControl != 0)
        m_playerControl->setPlaybackRate(m_playbackRate);
    else
        emit playbackRateChanged();
}

QString QDeclarativeMediaBase::errorString() const
{
    return m_errorString;
}

QT_END_NAMESPACE

