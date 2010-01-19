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

#ifndef MediaPlayerPrivateQt_h
#define MediaPlayerPrivateQt_h

#include "MediaPlayerPrivate.h"
#include <QObject>
#include <qmediaplayer.h>
#include <qmediaplayercontrol.h>

namespace WebCore {

    class MediaPlayerPrivate : public QObject, public MediaPlayerPrivateInterface {

        Q_OBJECT

    public:
        static MediaPlayerPrivateInterface* create(MediaPlayer* player);
        virtual ~MediaPlayerPrivate();

        static void registerMediaEngine(MediaEngineRegistrar);
        static void getSupportedTypes(HashSet<String>&);
        static MediaPlayer::SupportsType supportsType(const String& type, const String& codecs);
        static bool isAvailable() { return true; }

        bool hasVideo() const;
        bool hasAudio() const;

        void load(const String &url);
        void cancelLoad();

        void play();
        void pause();

        bool paused() const;
        bool seeking() const;

        float duration() const;
        float currentTime() const;
        void seek(float);
        void setEndTime(float);

        void setRate(float);
        void setVolume(float);
        void setMuted(bool);

        // State Information
        MediaPlayer::NetworkState networkState() const;
        MediaPlayer::ReadyState readyState() const;

        // Network Information
        PassRefPtr<TimeRanges> buffered() const;
        float maxTimeSeekable() const;
        unsigned bytesLoaded() const;
        unsigned totalBytes() const;
        bool totalBytesKnown() const;
        int dataRate() const;

        virtual void setVisible(bool);

        virtual IntSize naturalSize() const = 0;
        virtual void setSize(const IntSize&) = 0;
        virtual void paint(GraphicsContext*, const IntRect&) = 0;
        virtual bool supportsFullscreen() const = 0;

    protected slots:
        virtual void mediaStatusChanged(QMediaPlayer::MediaStatus);
        virtual void handleError(QMediaPlayer::Error);
        virtual void stateChanged(QMediaPlayer::State) {}
        virtual void positionChanged(qint64) {}
        virtual void durationChanged(qint64);
        virtual void repaint() {}

    protected:
        MediaPlayerPrivate(MediaPlayer*);

        MediaPlayer* m_player;
        QMediaPlayer* m_mediaPlayer;
        QMediaPlayerControl* m_mediaPlayerControl;

        mutable MediaPlayer::NetworkState m_networkState;
        mutable MediaPlayer::ReadyState m_readyState;

        bool m_isVisible;
        qint64 m_endTime;

    private:
        void updateStates();
    };
}

#endif // MediaPlayerPrivateQt_h
