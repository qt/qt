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

#include "qgstreamerplayercontrol.h"
#include "qgstreamerplayersession.h"

#include <qmediaplaylistnavigator.h>

#include <QtCore/qdir.h>
#include <QtCore/qsocketnotifier.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

QT_BEGIN_NAMESPACE

QGstreamerPlayerControl::QGstreamerPlayerControl(QGstreamerPlayerSession *session, QObject *parent)
    : QMediaPlayerControl(parent)
    , m_session(session)
    , m_state(QMediaPlayer::StoppedState)
    , m_mediaStatus(QMediaPlayer::NoMedia)
    , m_bufferProgress(-1)
    , m_seekToStartPending(false)
    , m_stream(0)
    , m_fifoNotifier(0)
    , m_fifoCanWrite(false)
    , m_bufferSize(0)
    , m_bufferOffset(0)
{
    m_fifoFd[0] = -1;
    m_fifoFd[1] = -1;

    connect(m_session, SIGNAL(positionChanged(qint64)),
            this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(durationChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(mutedStateChanged(bool)),
            this, SIGNAL(mutedChanged(bool)));
    connect(m_session, SIGNAL(volumeChanged(int)),
            this, SIGNAL(volumeChanged(int)));
    connect(m_session, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(updateState(QMediaPlayer::State)));
    connect(m_session,SIGNAL(bufferingProgressChanged(int)),
            this, SLOT(setBufferProgress(int)));
    connect(m_session, SIGNAL(playbackFinished()),
            this, SLOT(processEOS()));
    connect(m_session, SIGNAL(audioAvailableChanged(bool)),
            this, SIGNAL(audioAvailableChanged(bool)));
    connect(m_session, SIGNAL(videoAvailableChanged(bool)),
            this, SIGNAL(videoAvailableChanged(bool)));
    connect(m_session, SIGNAL(seekableChanged(bool)),
            this, SIGNAL(seekableChanged(bool)));
    connect(m_session, SIGNAL(error(int,QString)),
            this, SIGNAL(error(int,QString)));
}

QGstreamerPlayerControl::~QGstreamerPlayerControl()
{
    if (m_fifoFd[0] >= 0) {
        ::close(m_fifoFd[0]);
        ::close(m_fifoFd[1]);
        m_fifoFd[0] = -1;
        m_fifoFd[1] = -1;
    }
}

qint64 QGstreamerPlayerControl::position() const
{
    return m_seekToStartPending ? 0 : m_session->position();
}

qint64 QGstreamerPlayerControl::duration() const
{
    return m_session->duration();
}

QMediaPlayer::State QGstreamerPlayerControl::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus QGstreamerPlayerControl::mediaStatus() const
{
    return m_mediaStatus;
}

int QGstreamerPlayerControl::bufferStatus() const
{
    if (m_bufferProgress == -1) {
        return m_session->state() == QMediaPlayer::StoppedState ? 0 : 100;
    } else
        return m_bufferProgress;
}

int QGstreamerPlayerControl::volume() const
{
    return m_session->volume();
}

bool QGstreamerPlayerControl::isMuted() const
{
    return m_session->isMuted();
}

bool QGstreamerPlayerControl::isSeekable() const
{
    return m_session->isSeekable();
}

QMediaTimeRange QGstreamerPlayerControl::availablePlaybackRanges() const
{
    QMediaTimeRange ranges;

     if (m_session->isSeekable())
         ranges.addInterval(0, m_session->duration());

     return ranges;
}

qreal QGstreamerPlayerControl::playbackRate() const
{
    return m_session->playbackRate();
}

void QGstreamerPlayerControl::setPlaybackRate(qreal rate)
{
    m_session->setPlaybackRate(rate);
}

void QGstreamerPlayerControl::setPosition(qint64 pos)
{
    if (m_mediaStatus == QMediaPlayer::EndOfMedia) {
        m_mediaStatus = QMediaPlayer::LoadedMedia;
        emit mediaStatusChanged(m_mediaStatus);
    }

    if (m_session->seek(pos))
        m_seekToStartPending = false;
}

void QGstreamerPlayerControl::play()
{
    playOrPause(QMediaPlayer::PlayingState);
}

void QGstreamerPlayerControl::pause()
{
    playOrPause(QMediaPlayer::PausedState);
}

void QGstreamerPlayerControl::playOrPause(QMediaPlayer::State newState)
{
    QMediaPlayer::State oldState = m_state;
    QMediaPlayer::MediaStatus oldMediaStatus = m_mediaStatus;

    if (m_mediaStatus == QMediaPlayer::EndOfMedia)
        m_mediaStatus = QMediaPlayer::BufferedMedia;

    if (m_seekToStartPending) {
        m_session->pause();
        if (!m_session->seek(0)) {
            m_bufferProgress = -1;
            m_session->stop();
            m_mediaStatus = QMediaPlayer::LoadingMedia;
        }
        m_seekToStartPending = false;
    }

    bool ok = false;
    if (newState == QMediaPlayer::PlayingState)
        ok = m_session->play();
    else
        ok = m_session->pause();

    if (!ok)
        return;

    m_state = newState;

    if (m_mediaStatus == QMediaPlayer::EndOfMedia || m_mediaStatus == QMediaPlayer::LoadedMedia) {
        if (m_bufferProgress == -1 || m_bufferProgress == 100)
            m_mediaStatus = QMediaPlayer::BufferedMedia;
        else
            m_mediaStatus = QMediaPlayer::BufferingMedia;
    }

    if (m_state != oldState)
        emit stateChanged(m_state);
    if (m_mediaStatus != oldMediaStatus)
        emit mediaStatusChanged(m_mediaStatus);

}

void QGstreamerPlayerControl::stop()
{
    if (m_state != QMediaPlayer::StoppedState) {
        m_state = QMediaPlayer::StoppedState;
        m_session->pause();
        m_seekToStartPending = true;
        updateState(m_session->state());
        emit positionChanged(0);
        emit stateChanged(m_state);
    }
}

void QGstreamerPlayerControl::setVolume(int volume)
{
    m_session->setVolume(volume);
}

void QGstreamerPlayerControl::setMuted(bool muted)
{
    m_session->setMuted(muted);
}

QMediaContent QGstreamerPlayerControl::media() const
{
    return m_currentResource;
}

const QIODevice *QGstreamerPlayerControl::mediaStream() const
{
    return m_stream;
}

void QGstreamerPlayerControl::setMedia(const QMediaContent &content, QIODevice *stream)
{
    QMediaPlayer::State oldState = m_state;
    m_state = QMediaPlayer::StoppedState;    
    m_session->stop();

    if (m_bufferProgress != -1) {
        m_bufferProgress = -1;
        emit bufferStatusChanged(0);
    }

    if (m_stream) {
        closeFifo();

        disconnect(m_stream, SIGNAL(readyRead()), this, SLOT(writeFifo()));
        m_stream = 0;
    }

    m_currentResource = content;
    m_stream = stream;
    m_seekToStartPending = false;

    QUrl url;

    if (m_stream) {
        if (m_stream->isReadable() && openFifo()) {
            url = QUrl(QString(QLatin1String("fd://%1")).arg(m_fifoFd[0]));
        }
    } else if (!content.isNull()) {
        url = content.canonicalUrl();
    }

    m_session->load(url);    

    if (m_fifoFd[1] >= 0) {
        m_fifoCanWrite = true;

        writeFifo();
    }

    if (!url.isEmpty()) {
        if (m_mediaStatus != QMediaPlayer::LoadingMedia)
            emit mediaStatusChanged(m_mediaStatus = QMediaPlayer::LoadingMedia);
        m_session->pause();
    } else {
        if (m_mediaStatus != QMediaPlayer::NoMedia)
            emit mediaStatusChanged(m_mediaStatus = QMediaPlayer::NoMedia);
        setBufferProgress(0);
    }

    emit mediaChanged(m_currentResource);
    if (m_state != oldState)
        emit stateChanged(m_state);
}

void QGstreamerPlayerControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

bool QGstreamerPlayerControl::isAudioAvailable() const
{
    return m_session->isAudioAvailable();
}

bool QGstreamerPlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
}

void QGstreamerPlayerControl::updateState(QMediaPlayer::State state)
{
    QMediaPlayer::MediaStatus oldStatus = m_mediaStatus;
    QMediaPlayer::State oldState = m_state;

    switch (state) {
    case QMediaPlayer::StoppedState:
        m_state = QMediaPlayer::StoppedState;
        if (m_currentResource.isNull())
            m_mediaStatus = QMediaPlayer::NoMedia;
        else
            m_mediaStatus = QMediaPlayer::LoadingMedia;
        break;

    case QMediaPlayer::PlayingState:
    case QMediaPlayer::PausedState:
        if (m_state == QMediaPlayer::StoppedState) {
            m_mediaStatus = QMediaPlayer::LoadedMedia;
        } else {
            if (m_bufferProgress == -1 || m_bufferProgress == 100)
                m_mediaStatus = QMediaPlayer::BufferedMedia;
        }
        break;
    }

    //EndOfMedia status should be kept, until reset by pause, play or setMedia
    if (oldStatus == QMediaPlayer::EndOfMedia)
        m_mediaStatus = QMediaPlayer::EndOfMedia;

    if (m_state != oldState)
        emit stateChanged(m_state);
    if (m_mediaStatus != oldStatus)
        emit mediaStatusChanged(m_mediaStatus);
}

void QGstreamerPlayerControl::processEOS()
{
    m_mediaStatus = QMediaPlayer::EndOfMedia;
    stop();
    emit mediaStatusChanged(m_mediaStatus);
}

void QGstreamerPlayerControl::setBufferProgress(int progress)
{
    if (m_bufferProgress == progress || m_mediaStatus == QMediaPlayer::NoMedia)
        return;

    QMediaPlayer::MediaStatus oldStatus = m_mediaStatus;

    m_bufferProgress = progress;

    if (m_state == QMediaPlayer::StoppedState) {
        m_mediaStatus = QMediaPlayer::LoadedMedia;
    } else {
        if (m_bufferProgress < 100) {
            m_mediaStatus = QMediaPlayer::StalledMedia;
            m_session->pause();
        } else {
            m_mediaStatus = QMediaPlayer::BufferedMedia;
            if (m_state == QMediaPlayer::PlayingState)
                m_session->play();
        }
    }

    if (m_mediaStatus != oldStatus)
        emit mediaStatusChanged(m_mediaStatus);

    emit bufferStatusChanged(m_bufferProgress);
}

void QGstreamerPlayerControl::writeFifo()
{
    if (m_fifoCanWrite) {
        qint64 bytesToRead = qMin<qint64>(
                m_stream->bytesAvailable(), PIPE_BUF - m_bufferSize);

        if (bytesToRead > 0) {
            int bytesRead = m_stream->read(&m_buffer[m_bufferOffset + m_bufferSize], bytesToRead);

            if (bytesRead > 0)
                m_bufferSize += bytesRead;
        }

        if (m_bufferSize > 0) {
            int bytesWritten = ::write(m_fifoFd[1], &m_buffer[m_bufferOffset], size_t(m_bufferSize));

            if (bytesWritten > 0) {
                m_bufferOffset += bytesWritten;
                m_bufferSize -= bytesWritten;

                if (m_bufferSize == 0)
                    m_bufferOffset = 0;
            } else if (errno == EAGAIN) {
                m_fifoCanWrite = false;
            } else {
                closeFifo();
            }
        }
    }

    m_fifoNotifier->setEnabled(m_stream->bytesAvailable() > 0);
}

void QGstreamerPlayerControl::fifoReadyWrite(int socket)
{
    if (socket == m_fifoFd[1]) {
        m_fifoCanWrite = true;

        writeFifo();
    }
}

bool QGstreamerPlayerControl::openFifo()
{
    Q_ASSERT(m_fifoFd[0] < 0);
    Q_ASSERT(m_fifoFd[1] < 0);

    if (::pipe(m_fifoFd) == 0) {
        int flags = ::fcntl(m_fifoFd[1], F_GETFD);

        if (::fcntl(m_fifoFd[1], F_SETFD, flags | O_NONBLOCK) >= 0) {
            m_fifoNotifier = new QSocketNotifier(m_fifoFd[1], QSocketNotifier::Write);

            connect(m_fifoNotifier, SIGNAL(activated(int)), this, SLOT(fifoReadyWrite(int)));

            return true;
        } else {
            qWarning("Failed to make pipe non blocking %d", errno);

            ::close(m_fifoFd[0]);
            ::close(m_fifoFd[1]);

            m_fifoFd[0] = -1;
            m_fifoFd[1] = -1;

            return false;
        }
    } else {
        qWarning("Failed to create pipe %d", errno);

        return false;
    }
}

void QGstreamerPlayerControl::closeFifo()
{
    if (m_fifoFd[0] >= 0) {
        delete m_fifoNotifier;
        m_fifoNotifier = 0;

        ::close(m_fifoFd[0]);
        ::close(m_fifoFd[1]);
        m_fifoFd[0] = -1;
        m_fifoFd[1] = -1;

        m_fifoCanWrite = false;

        m_bufferSize = 0;
        m_bufferOffset = 0;
    }
}

QT_END_NAMESPACE

