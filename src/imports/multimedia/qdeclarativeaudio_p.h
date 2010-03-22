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

#ifndef QDECLARATIVEAUDIO_P_H
#define QDECLARATIVEAUDIO_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativemediabase_p.h"

#include <QtCore/qbasictimer.h>
#include <QtDeclarative/qdeclarativeitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QTimerEvent;

class QDeclarativeAudio : public QObject, public QDeclarativeMediaBase, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool autoLoad READ isAutoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(bool playing READ isPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(int bufferProgress READ bufferProgress NOTIFY bufferProgressChanged)
    Q_PROPERTY(bool seekable READ isSeekable NOTIFY seekableChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(Error error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged)
    Q_ENUMS(Status)
    Q_ENUMS(Error)
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    enum Status
    {
        UnknownStatus = QMediaPlayer::UnknownMediaStatus,
        NoMedia       = QMediaPlayer::NoMedia,
        Loading       = QMediaPlayer::LoadingMedia,
        Loaded        = QMediaPlayer::LoadedMedia,
        Stalled       = QMediaPlayer::StalledMedia,
        Buffering     = QMediaPlayer::BufferingMedia,
        Buffered      = QMediaPlayer::BufferedMedia,
        EndOfMedia    = QMediaPlayer::EndOfMedia,
        InvalidMedia  = QMediaPlayer::InvalidMedia
    };

    enum Error
    {
        NoError        = QMediaPlayer::NoError,
        ResourceError  = QMediaPlayer::ResourceError,
        FormatError    = QMediaPlayer::FormatError,
        NetworkError   = QMediaPlayer::NetworkError,
        AccessDenied   = QMediaPlayer::AccessDeniedError,
        ServiceMissing = QMediaPlayer::ServiceMissingError
    };

    QDeclarativeAudio(QObject *parent = 0);
    ~QDeclarativeAudio();

    Status status() const;
    Error error() const;

    void componentComplete();

public Q_SLOTS:
    void play();
    void pause();
    void stop();

Q_SIGNALS:
    void sourceChanged();
    void autoLoadChanged();
    void playingChanged();
    void pausedChanged();

    void started();
    void resumed();
    void paused();
    void stopped();

    void statusChanged();

    void loaded();
    void buffering();
    void stalled();
    void buffered();
    void endOfMedia();

    void durationChanged();
    void positionChanged();

    void volumeChanged();
    void mutedChanged();

    void bufferProgressChanged();

    void seekableChanged();
    void playbackRateChanged();

    void errorChanged();
    void error(QDeclarativeAudio::Error error, const QString &errorString);

private Q_SLOTS:
    void _q_error(int, const QString &);

private:
    Q_DISABLE_COPY(QDeclarativeAudio)
    Q_PRIVATE_SLOT(mediaBase(), void _q_stateChanged(QMediaPlayer::State))
    Q_PRIVATE_SLOT(mediaBase(), void _q_mediaStatusChanged(QMediaPlayer::MediaStatus))
    Q_PRIVATE_SLOT(mediaBase(), void _q_metaDataChanged())

    inline QDeclarativeMediaBase *mediaBase() { return this; }
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QT_PREPEND_NAMESPACE(QDeclarativeAudio))

QT_END_HEADER

#endif
