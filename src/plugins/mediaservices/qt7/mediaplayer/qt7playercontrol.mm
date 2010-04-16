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

#include "qt7playercontrol.h"
#include "qt7playersession.h"

#include <QtMediaServices/qmediaplaylistnavigator.h>

#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>


QT_BEGIN_NAMESPACE

QT7PlayerControl::QT7PlayerControl(QObject *parent)
   : QMediaPlayerControl(parent)
{    
}

QT7PlayerControl::~QT7PlayerControl()
{
}

void QT7PlayerControl::setSession(QT7PlayerSession *session)
{
    m_session = session;

    connect(m_session, SIGNAL(positionChanged(qint64)), this, SIGNAL(positionChanged(qint64)));
    connect(m_session, SIGNAL(durationChanged(qint64)), this, SIGNAL(durationChanged(qint64)));
    connect(m_session, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SIGNAL(stateChanged(QMediaPlayer::State)));
    connect(m_session, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_session, SIGNAL(volumeChanged(int)), this, SIGNAL(volumeChanged(int)));
    connect(m_session, SIGNAL(mutedChanged(bool)), this, SIGNAL(mutedChanged(bool)));
    connect(m_session, SIGNAL(audioAvailableChanged(bool)), this, SIGNAL(audioAvailableChanged(bool)));
    connect(m_session, SIGNAL(videoAvailableChanged(bool)), this, SIGNAL(videoAvailableChanged(bool)));
    connect(m_session, SIGNAL(error(int,QString)), this, SIGNAL(error(int,QString)));
}

qint64 QT7PlayerControl::position() const
{
    return m_session->position();
}

qint64 QT7PlayerControl::duration() const
{
    return m_session->duration();
}

QMediaPlayer::State QT7PlayerControl::state() const
{
    return m_session->state();
}

QMediaPlayer::MediaStatus QT7PlayerControl::mediaStatus() const
{
    return m_session->mediaStatus();
}

int QT7PlayerControl::bufferStatus() const
{
    return m_session->bufferStatus();
}

int QT7PlayerControl::volume() const
{
    return m_session->volume();
}

bool QT7PlayerControl::isMuted() const
{
    return m_session->isMuted();
}

bool QT7PlayerControl::isSeekable() const
{
    return m_session->isSeekable();
}

QMediaTimeRange QT7PlayerControl::availablePlaybackRanges() const
{
    return isSeekable() ? QMediaTimeRange(0, duration()) : QMediaTimeRange();
}

qreal QT7PlayerControl::playbackRate() const
{
    return m_session->playbackRate();
}

void QT7PlayerControl::setPlaybackRate(qreal rate)
{
    m_session->setPlaybackRate(rate);
}

void QT7PlayerControl::setPosition(qint64 pos)
{
    m_session->setPosition(pos);
}

void QT7PlayerControl::play()
{
    m_session->play();
}

void QT7PlayerControl::pause()
{
    m_session->pause();
}

void QT7PlayerControl::stop()
{
    m_session->stop();
}

void QT7PlayerControl::setVolume(int volume)
{
    m_session->setVolume(volume);
}

void QT7PlayerControl::setMuted(bool muted)
{
    m_session->setMuted(muted);
}

QMediaContent QT7PlayerControl::media() const
{
    return m_session->media();
}

const QIODevice *QT7PlayerControl::mediaStream() const
{
    return m_session->mediaStream();
}

void QT7PlayerControl::setMedia(const QMediaContent &content, QIODevice *stream)
{
    m_session->setMedia(content, stream);

    emit mediaChanged(content);
}

bool QT7PlayerControl::isAudioAvailable() const
{
    return m_session->isAudioAvailable();
}

bool QT7PlayerControl::isVideoAvailable() const
{
    return m_session->isVideoAvailable();
}

#include "moc_qt7playercontrol.cpp"

QT_END_NAMESPACE
