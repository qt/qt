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

#ifndef PHONON_MMF_ABSTRACTPLAYER_H
#define PHONON_MMF_ABSTRACTPLAYER_H

#include <QObject>
#include <Phonon/phononnamespace.h>
#include <Phonon/MediaSource.h>

namespace Phonon
{
    namespace MMF
    {
        class AudioOutput;

        class AbstractPlayer : public QObject
        {
        public:
            virtual void play() = 0;
            virtual void pause() = 0;
            virtual void stop() = 0;
            virtual void seek(qint64 milliseconds) = 0;
            virtual qint32 tickInterval() const = 0;
            virtual void setTickInterval() const = 0;
            virtual bool hasVideo() const = 0;
            virtual bool isSeekable() const = 0;
            virtual qint64 currentTime() const = 0;
            virtual Phonon::State state() const = 0;
            virtual QString errorString() const = 0;
            virtual Phonon::ErrorType errorType() const = 0;
            virtual qint64 totalTime() const = 0;
            virtual Phonon::MediaSource source() const = 0;
            virtual void setSource(const Phonon::MediaSource &) = 0;
            virtual void setNextSource(const Phonon::MediaSource &) = 0;

            virtual void setTransitionTime(qint32) = 0;
            virtual qint32 transitionTime() const = 0;
            virtual qint32 prefinishMark() const = 0;
            virtual void setPrefinishMark(qint32) = 0;

            virtual bool setVolume(qreal) = 0;
            virtual qreal volume() const = 0;

            virtual void setAudioOutput(AudioOutput *) = 0;
        };
    }
}

#endif

