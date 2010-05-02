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

#ifndef S60MEDIAPLAYERSESSION_H
#define S60MEDIAPLAYERSESSION_H

#include <QtCore/qobject.h>
#include <QtCore/qurl.h>
#include <QtCore/qpair.h>
#include <qmediaplayer.h>
#include <e32cmn.h> // for TDesC
#include <QRect>
#include "s60mediaplayerservice.h"

QT_BEGIN_NAMESPACE

class QMediaTimeRange;

class QTimer;

class S60MediaPlayerSession : public QObject
{
    Q_OBJECT

public:
    S60MediaPlayerSession(QObject *parent);
    virtual ~S60MediaPlayerSession();

    // for player control interface to use
    QMediaPlayer::State state() const;
    QMediaPlayer::MediaStatus mediaStatus() const;
    qint64 duration() const;
    qint64 position() const;
    void setPosition(qint64 pos);
    int volume() const;
    void setVolume(int volume);
    bool isMuted() const;
    void setMuted(bool muted);
    virtual bool isVideoAvailable() const = 0;
    virtual bool isAudioAvailable() const = 0;
    bool isSeekable() const;
    void play();
    void pause();
    void stop();
    void reset();
    bool isMetadataAvailable() const;
    QVariant metaData(const QString &key) const;
    QMap<QString, QVariant> availableMetaData() const;
    void load(QUrl url);
    int bufferStatus();
    virtual void setVideoRenderer(QObject *renderer);
    void setMediaStatus(QMediaPlayer::MediaStatus);
    void setState(QMediaPlayer::State state);
    void setAudioEndpoint(const QString& audioEndpoint);

protected:
    virtual void doLoadL(const TDesC &path) = 0;
    virtual void doLoadUrlL(const TDesC &path) = 0;
    virtual void doPlay() = 0;
    virtual void doStop() = 0;
    virtual void doPauseL() = 0;
    virtual void doSetVolumeL(int volume) = 0;
    virtual void doSetPositionL(qint64 microSeconds) = 0;
    virtual qint64 doGetPositionL() const = 0;
    virtual void updateMetaDataEntriesL() = 0;
    virtual int doGetBufferStatusL() const = 0;
    virtual qint64 doGetDurationL() const = 0;
    virtual void doSetAudioEndpoint(const QString& audioEndpoint) = 0;

public:
    // From S60MediaPlayerAudioEndpointSelector
    virtual QString activeEndpoint() const = 0;
    virtual QString defaultEndpoint() const = 0;
public Q_SLOTS:
    virtual void setActiveEndpoint(const QString& name) = 0;

protected:
    void setError(int error,  const QString &errorString = QString(), bool forceReset = false);
    void loaded();
    void buffering();
    void buffered();
    void endOfMedia();
    QMap<QString, QVariant>& metaDataEntries();
    QMediaPlayer::Error fromSymbianErrorToMultimediaError(int error);
    void startProgressTimer();
    void stopProgressTimer();
    void startStalledTimer();
    void stopStalledTimer();
    QString TDesC2QString(const TDesC& aDescriptor);
	TPtrC QString2TPtrC( const QString& string );
	QRect TRect2QRect(const TRect& tr);
	TRect QRect2TRect(const QRect& qr);


protected slots:
    void tick();
    void stalled();

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus mediaStatus);
    void videoAvailableChanged(bool videoAvailable);
    void audioAvailableChanged(bool audioAvailable);
    void bufferStatusChanged(int percentFilled);
    void seekableChanged(bool);
    void availablePlaybackRangesChanged(const QMediaTimeRange&);
    void metaDataChanged();
    void error(int error, const QString &errorString);
    void activeEndpointChanged(const QString &name);

private:
    qreal m_playbackRate;
    QMap<QString, QVariant> m_metaDataMap;
    bool m_muted;
    int m_volume;
    QMediaPlayer::State m_state;
    QMediaPlayer::MediaStatus m_mediaStatus;
    QTimer *m_progressTimer;
    QTimer *m_stalledTimer;
    int m_error;
    bool m_play_requested;
    bool m_stream;
};

QT_END_NAMESPACE

#endif
