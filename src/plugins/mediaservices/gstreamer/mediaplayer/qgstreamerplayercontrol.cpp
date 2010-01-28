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

QGstreamerPlayerControl::QGstreamerPlayerControl(QGstreamerPlayerSession *session, QObject *parent)
    : QMediaPlayerControl(parent)
    , m_session(session)
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
            this, SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(m_session, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_session,SIGNAL(bufferingProgressChanged(int)),
            this, SIGNAL(bufferStatusChanged(int)));
    connect(m_session, SIGNAL(videoAvailableChanged(bool)),
            this, SIGNAL(videoAvailableChanged(bool)));
    connect(m_session, SIGNAL(seekableChanged(bool)),
            this, SIGNAL(seekableChanged(bool)));
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
    return m_session->position();
}

qint64 QGstreamerPlayerControl::duration() const
{
    return m_session->duration();
}

QMediaPlayer::State QGstreamerPlayerControl::state() const
{
    return m_session->state();
}

QMediaPlayer::MediaStatus QGstreamerPlayerControl::mediaStatus() const
{
    return m_session->mediaStatus();
}

int QGstreamerPlayerControl::bufferStatus() const
{
    return 100;
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
    m_session->seek(pos);
}

void QGstreamerPlayerControl::play()
{
    m_session->play();

    if (m_fifoFd[1] >= 0) {
        m_fifoCanWrite = true;

        writeFifo();
    }
}

void QGstreamerPlayerControl::pause()
{
    m_session->pause();
}

void QGstreamerPlayerControl::stop()
{
    m_session->stop();
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
    m_session->stop();

    if (m_stream) {
        closeFifo();

        disconnect(m_stream, SIGNAL(readyRead()), this, SLOT(writeFifo()));
        m_stream = 0;
    }

    m_currentResource = content;
    m_stream = stream;

    QUrl url;

    if (m_stream) {
        if (m_stream->isReadable() && openFifo()) {
            url = QUrl(QString(QLatin1String("fd://%1")).arg(m_fifoFd[0]));
        }
    } else if (!content.isNull()) {
        url = content.canonicalUrl();
    }

    m_session->load(url);

    emit mediaChanged(m_currentResource);
}

void QGstreamerPlayerControl::setVideoOutput(QObject *output)
{
    m_session->setVideoRenderer(output);
}

bool QGstreamerPlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
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
