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

#ifndef S60MEDIAPLAYERCONTROL_H
#define S60MEDIAPLAYERCONTROL_H

#include <QtCore/qobject.h>

#include <qmediaplayercontrol.h>

#include "ms60mediaplayerresolver.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QMediaPlayer;
class QMediaTimeRange;
class QMediaContent;


class S60MediaPlayerSession;
class S60MediaPlayerService;

class S60MediaSettings
{

public:
    S60MediaSettings()
        : m_volume(0)
        , m_muted(false)
        , m_playbackRate(0)
        , m_mediaStatus(QMediaPlayer::UnknownMediaStatus)
        , m_audioEndpoint(QString("Default"))
    {
    }

    void setVolume(int volume) { m_volume = volume; }
    void setMuted(bool muted) { m_muted = muted; }
    void setPlaybackRate(int rate) { m_playbackRate = rate; }
    void setMediaStatus(QMediaPlayer::MediaStatus status) {m_mediaStatus=status;}
    void setAudioEndpoint(const QString& audioEndpoint) { m_audioEndpoint = audioEndpoint; }

    int volume() const { return m_volume; }
    bool isMuted() const { return m_muted; }
    qreal playbackRate() const { return m_playbackRate; }
    QMediaPlayer::MediaStatus mediaStatus() const {return m_mediaStatus;}
    QString audioEndpoint() const { return m_audioEndpoint; }

private:
    int m_volume;
    bool m_muted;
    qreal m_playbackRate;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QString m_audioEndpoint;
};

class S60MediaPlayerControl : public QMediaPlayerControl
{
    Q_OBJECT

public:
    S60MediaPlayerControl(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent = 0);
    ~S60MediaPlayerControl();

    // from QMediaPlayerControl
    virtual QMediaPlayer::State state() const;
    virtual QMediaPlayer::MediaStatus mediaStatus() const;
    virtual qint64 duration() const;
    virtual qint64 position() const;
    virtual void setPosition(qint64 pos);
    virtual int volume() const;
    virtual void setVolume(int volume);
    virtual bool isMuted() const;
    virtual void setMuted(bool muted);
    virtual int bufferStatus() const;
    virtual bool isAudioAvailable() const;
    virtual bool isVideoAvailable() const;
    virtual bool isSeekable() const;
    virtual QMediaTimeRange availablePlaybackRanges() const;
    virtual qreal playbackRate() const;
    virtual void setPlaybackRate(qreal rate);
    virtual QMediaContent media() const;
    virtual const QIODevice *mediaStream() const;
    virtual void setMedia(const QMediaContent&, QIODevice *);
    virtual void play();
    virtual void pause();
    virtual void stop();
    S60MediaPlayerSession* session();
    void setAudioEndpoint(const QString& name);

    // Own methods
    void setVideoOutput(QObject *output);
    const S60MediaSettings& mediaControlSettings() const;

private:
    MS60MediaPlayerResolver &m_mediaPlayerResolver;
    S60MediaPlayerSession *m_session;
    QMediaContent m_currentResource;
    QIODevice *m_stream;
    S60MediaSettings m_mediaSettings;
};

QT_END_NAMESPACE

#endif
