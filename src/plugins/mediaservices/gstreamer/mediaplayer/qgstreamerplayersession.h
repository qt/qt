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

#ifndef QGSTREAMERPLAYERSESSION_H
#define QGSTREAMERPLAYERSESSION_H

#include <QObject>
#include <QUrl>
#include "qgstreamerplayercontrol.h"
#include "qgstreamerbushelper.h"
#include <QtMediaServices/qmediaplayer.h>
//#include <qmediastreamscontrol.h>

#include <gst/gst.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QGstreamerBusHelper;
class QGstreamerMessage;
class QGstreamerVideoRendererInterface;

class QGstreamerPlayerSession : public QObject, public QGstreamerSyncEventFilter
{
Q_OBJECT

public:
    QGstreamerPlayerSession(QObject *parent);
    virtual ~QGstreamerPlayerSession();

    QUrl url() const;

    QMediaPlayer::State state() const { return m_state; }

    qint64 duration() const;
    qint64 position() const;

    bool isBuffering() const;

    int bufferingProgress() const;

    int volume() const;
    bool isMuted() const;

    void setVideoRenderer(QObject *renderer);
    bool isAudioAvailable() const;
    bool isVideoAvailable() const;

    bool isSeekable() const;

    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);

    QMap<QByteArray ,QVariant> tags() const { return m_tags; }
    QMap<QtMediaServices::MetaData,QVariant> streamProperties(int streamNumber) const { return m_streamProperties[streamNumber]; }
//    int streamCount() const { return m_streamProperties.count(); }
//    QMediaStreamsControl::StreamType streamType(int streamNumber) { return m_streamTypes.value(streamNumber, QMediaStreamsControl::UnknownStream); }
//
//    int activeStream(QMediaStreamsControl::StreamType streamType) const;
//    void setActiveStream(QMediaStreamsControl::StreamType streamType, int streamNumber);

    bool processSyncMessage(const QGstreamerMessage &message);    

public slots:
    void load(const QUrl &url);

    bool play();
    bool pause();
    void stop();

    bool seek(qint64 pos);

    void setVolume(int volume);
    void setMuted(bool muted);

signals:
    void durationChanged(qint64 duration);
    void positionChanged(qint64 position);
    void stateChanged(QMediaPlayer::State state);
    void volumeChanged(int volume);
    void mutedStateChanged(bool muted);
    void audioAvailableChanged(bool audioAvailable);
    void videoAvailableChanged(bool videoAvailable);
    void bufferingChanged(bool buffering);
    void bufferingProgressChanged(int percentFilled);
    void playbackFinished();
    void tagsChanged();
    void streamsChanged();
    void seekableChanged(bool);
    void error(int error, const QString &errorString);

private slots:
    void busMessage(const QGstreamerMessage &message);
    void getStreamsInfo();
    void setSeekable(bool);
    void finishVideoOutputChange();

private:
    QUrl m_url;
    QMediaPlayer::State m_state;
    QGstreamerBusHelper* m_busHelper;
    GstElement* m_playbin;

    GstElement* m_videoOutputBin;
    GstElement* m_videoIdentity;
    GstElement* m_colorSpace;
    GstElement* m_videoScale;
    GstElement* m_videoSink;
    GstElement* m_pendingVideoSink;
    GstElement* m_nullVideoSink;

    GstBus* m_bus;
    QGstreamerVideoRendererInterface *m_renderer;

    QMap<QByteArray, QVariant> m_tags;
    QList< QMap<QtMediaServices::MetaData,QVariant> > m_streamProperties;
//    QList<QMediaStreamsControl::StreamType> m_streamTypes;
//    QMap<QMediaStreamsControl::StreamType, int> m_playbin2StreamOffset;


    int m_volume;
    qreal m_playbackRate;
    bool m_muted;
    bool m_audioAvailable;
    bool m_videoAvailable;
    bool m_seekable;

    qint64 m_lastPosition;
    qint64 m_duration;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGSTREAMERPLAYERSESSION_H
