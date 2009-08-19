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

#include <include/videoplayer.h>

#include "abstractmediaplayer.h"

class CVideoPlayerUtility;
class QSymbianControl;

namespace Phonon
{
    namespace MMF
    {
        /**
         *
         * See
         * <a href="http://wiki.forum.nokia.com/index.php/How_to_play_a_video_file_using_CVideoPlayerUtility">How to
         * play a video file using CVideoPlayerUtility</a>
         */
        class VideoPlayer	:	public AbstractMediaPlayer
							,	public MVideoPlayerUtilityObserver
        {
            Q_OBJECT
        public:
            VideoPlayer();
            virtual ~VideoPlayer();

            // AbstractPlayer
            virtual void doPlay();
            virtual void doPause();
            virtual void doStop();
            virtual int doSetVolume(int mmfVolume);
            virtual int openFile(RFile& file);
            virtual void close();
            
            virtual void seek(qint64 milliseconds);
            virtual bool hasVideo() const;
            virtual qint64 currentTime() const;
            virtual qint64 totalTime() const;
            
            // MVideoPlayerUtilityObserver
            virtual void MvpuoOpenComplete(TInt aError);
            virtual void MvpuoPrepareComplete(TInt aError);
            virtual void MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError);
            virtual void MvpuoPlayComplete(TInt aError);
            virtual void MvpuoEvent(const TMMFEvent &aEvent);
            
        Q_SIGNALS:
            void totalTimeChanged();
            void stateChanged(Phonon::State oldState,
                              Phonon::State newState);
            void finished();
            
        private:
        	CVideoPlayerUtility*				m_player;
        	QScopedPointer<QWidget>				m_widget;

        };
    }
}

#endif
