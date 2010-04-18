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

#ifndef QGSTREAMERPLAYERCONTROL_H
#define QGSTREAMERPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include <QtMediaServices/qmediaplayercontrol.h>
#include <QtMediaServices/qmediaplayer.h>

#include <limits.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QMediaPlaylist;
class QGstreamerPlayerSession;
class QGstreamerPlayerService;
class QMediaPlaylistNavigator;
class QSocketNotifier;

class QGstreamerPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    QGstreamerPlayerControl(QGstreamerPlayerSession *session, QObject *parent = 0);
    ~QGstreamerPlayerControl();

    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    qint64 position() const;
    qint64 duration() const;

    int bufferStatus() const;

    int volume() const;
    bool isMuted() const;

    bool isAudioAvailable() const;
    bool isVideoAvailable() const;
    void setVideoOutput(QObject *output);

    bool isSeekable() const;
    QMediaTimeRange availablePlaybackRanges() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMediaContent media() const;
    const QIODevice *mediaStream() const;
    void setMedia(const QMediaContent&, QIODevice *);

public Q_SLOTS:
    void setPosition(qint64 pos);

    void play();
    void pause();
    void stop();

    void setVolume(int volume);
    void setMuted(bool muted);

private Q_SLOTS:
    void writeFifo();
    void fifoReadyWrite(int socket);

    void updateState(QMediaPlayer::State);
    void processEOS();
    void setBufferProgress(int progress);

private:
    bool openFifo();
    void closeFifo();
    void playOrPause(QMediaPlayer::State state);

    QGstreamerPlayerSession *m_session;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    int m_bufferProgress;
    bool m_seekToStartPending;
    QMediaContent m_currentResource;
    QIODevice *m_stream;
    QSocketNotifier *m_fifoNotifier;
    int m_fifoFd[2];
    bool m_fifoCanWrite;
    int m_bufferSize;
    int m_bufferOffset;
    char m_buffer[PIPE_BUF];
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
