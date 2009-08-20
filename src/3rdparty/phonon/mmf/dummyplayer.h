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

#ifndef PHONON_MMF_DUMMYPLAYER_H
#define PHONON_MMF_DUMMYPLAYER_H

#include "abstractplayer.h"

namespace Phonon
{
    namespace MMF
    {
        class AudioOutput;

        /**
         * @short Stub implementation of AbstractPlayer.
         * 
         * The functions of this class are:
         * -	Allow MediaObject to call a subset of the MediaObjectInterface
         * 		API, before SetSource has been called.
         * -	Cache any parameters which are set in this state (e.g.
         * 		prefinish mark), so that they can be copied into the 'real'
         * 		AbstractPlayer implementation once a source has been loaded.
         */
        class DummyPlayer : public AbstractPlayer
        {
        public:
            // MediaObjectInterface
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
           // virtual void setSource(const MediaSource &);
            virtual void setFileSource(const Phonon::MediaSource&, RFile&);
            virtual void setNextSource(const MediaSource &source);
            virtual qint32 prefinishMark() const;
            virtual void setPrefinishMark(qint32);
            virtual qint32 transitionTime() const;
            virtual void setTransitionTime(qint32);
            
            // VolumeControlInterface
            virtual qreal volume() const;
            virtual bool setVolume(qreal volume);
            
        Q_SIGNALS:
            void totalTimeChanged();
            void stateChanged(Phonon::State oldState,
                              Phonon::State newState);
            void finished();
            void tick(qint64 time);
        };
    }
}

#endif
