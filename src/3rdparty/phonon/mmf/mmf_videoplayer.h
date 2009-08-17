/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PHONON_MMF_VIDEOPLAYER_H
#define PHONON_MMF_VIDEOPLAYER_H

/* We use the extra qualification include/ to avoid picking up the include
 * Phonon has. */
#include <include/videoplayer.h>
#include "abstractplayer.h"

#include <Phonon/MediaSource>

class CDrmPlayerUtility;
class TTimeIntervalMicroSeconds;
class QTimer;

namespace Phonon
{
    namespace MMF
    {
        class AudioOutput;

        /**
         *
         * See
         * <a href="http://wiki.forum.nokia.com/index.php/How_to_play_a_video_file_using_CVideoPlayerUtility">How to
         * play a video file using CVideoPlayerUtility</a>
         */
        class VideoPlayer : public AbstractPlayer
        {
            Q_OBJECT
        public:
            VideoPlayer(QObject *parent);
            virtual ~VideoPlayer();

            // AbstractPlayer
            virtual void play();
            virtual void pause();
            virtual void stop();
            virtual void seek(qint64 milliseconds);
            virtual qint32 tickInterval() const;
            virtual void setTickInterval(qint32 interval);
            virtual bool hasVideo() const;
            virtual bool isSeekable() const;
            virtual qint64 currentTime() const;
            virtual Phonon::State state() const;
            virtual QString errorString() const;
            virtual Phonon::ErrorType errorType() const;
            virtual qint64 totalTime() const;
            virtual MediaSource source() const;
            virtual void setSource(const MediaSource &);
            virtual void setNextSource(const MediaSource &source);
            virtual qint32 prefinishMark() const;
            virtual void setPrefinishMark(qint32);
            virtual qint32 transitionTime() const;
            virtual void setTransitionTime(qint32);

#ifdef QT_PHONON_MMF_AUDIO_DRM
            // MDrmVideoPlayerCallback
            virtual void MdapcInitComplete(TInt aError,
                                           const TTimeIntervalMicroSeconds &aDuration);
            virtual void MdapcPlayComplete(TInt aError);

            // MAudioLoadingObserver
            virtual void MaloLoadingStarted();
            virtual void MaloLoadingComplete();
#else
            // MMdaVideoPlayerCallback
            virtual void MapcInitComplete(TInt aError,
                                                       const TTimeIntervalMicroSeconds &aDuration);
            virtual void MapcPlayComplete(TInt aError);
#endif

            qreal volume() const;
            bool setVolume(qreal volume);

            void setAudioOutput(AudioOutput* audioOutput);

        Q_SIGNALS:
            void totalTimeChanged();
            void stateChanged(Phonon::State oldState,
                              Phonon::State newState);
            void finished();
            void tick(qint64 time);

        private Q_SLOTS:
            /**
             * Receives signal from m_tickTimer
             */
            void tick();

        private:
            static qint64 toMilliSeconds(const TTimeIntervalMicroSeconds &);

            /**
             * Defined private state enumeration in order to add GroundState
             */
            enum PrivateState
                {
                LoadingState    = Phonon::LoadingState,
                StoppedState    = Phonon::StoppedState,
                PlayingState    = Phonon::PlayingState,
                BufferingState    = Phonon::BufferingState,
                PausedState        = Phonon::PausedState,
                ErrorState        = Phonon::ErrorState,
                GroundState
                };

            /**
             * Converts PrivateState into the corresponding Phonon::State
             */
            static Phonon::State phononState(PrivateState state);

            /**
             * Changes state and emits stateChanged()
             */
            void changeState(PrivateState newState);

            /**
             * Using CPlayerType typedef in order to be able to easily switch between
             * CMdaVideoPlayerUtility and CDrmPlayerUtility
             */
            // CPlayerType*         m_player; TODO

            AudioOutput*        m_audioOutput;

            ErrorType           m_error;

            /**
             * Do not set this directly - call changeState() instead.
             */
            PrivateState        m_state;

            qint32                m_tickInterval;

            QTimer*                m_tickTimer;

            MediaSource         m_mediaSource;
            MediaSource         m_nextSource;

            qreal                m_volume;
            int                    m_maxVolume;
        };
    }
}

#endif
