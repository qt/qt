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

#ifndef PHONON_MMF_MEDIAOBJECT_H
#define PHONON_MMF_MEDIAOBJECT_H

#include <DrmAudioSamplePlayer.h>

#include <Phonon/MediaSource>
#include <Phonon/mediaobjectinterface.h>

class CDrmPlayerUtility;
class TTimeIntervalMicroSeconds;

namespace Phonon
{
    namespace MMF
    {
        class MediaObject : public QObject
                          , public MediaObjectInterface
                          , public MDrmAudioPlayerCallback
        {
            Q_OBJECT
            Q_INTERFACES(Phonon::MediaObjectInterface)
        public:
            MediaObject(QObject *parent);
            virtual ~MediaObject();
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

            // MAudioLoadingObserver
            virtual void MaloLoadingComplete();
            virtual void MaloLoadingStarted();
            
            // MDrmAudioPlayerCallback
            virtual void MdapcInitComplete(TInt aError,
                                            const TTimeIntervalMicroSeconds &aDuration);
            virtual void MdapcPlayComplete(TInt aError);

        Q_SIGNALS:
            void totalTimeChanged();

        private:
            static inline qint64 toMilliSeconds(const TTimeIntervalMicroSeconds &);
            static inline TTimeIntervalMicroSeconds toMicroSeconds(qint64 ms);

            CDrmPlayerUtility * m_player;
            mutable ErrorType   m_error;
            mutable State       m_state;
        };
    }
}

#endif
