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

#ifndef PHONON_MMF_ABSTRACTMEDIAPLAYER_H
#define PHONON_MMF_ABSTRACTMEDIAPLAYER_H

#include <QTimer>
#include <QScopedPointer>
#include <e32std.h>
#include "abstractplayer.h"

class RFile;

namespace Phonon
{
    namespace MMF
    {
		class AudioOutput;
    
		/**
		 * Interface via which MMF client APIs for both audio and video can be
		 * accessed.
		 */
        class AbstractMediaPlayer : public AbstractPlayer
        {
			Q_OBJECT
        
        protected:
        	AbstractMediaPlayer();
        	explicit AbstractMediaPlayer(const AbstractPlayer& player);
        	~AbstractMediaPlayer();
        	
        public:
        	// MediaObjectInterface
        	virtual void play();
        	virtual void pause();
        	virtual void stop();
        	virtual bool isSeekable() const;
            virtual Phonon::ErrorType errorType() const;
            virtual QString errorString() const;
            virtual Phonon::State state() const;
            virtual MediaSource source() const;
            virtual void setFileSource(const Phonon::MediaSource&, RFile&);
            virtual void setNextSource(const MediaSource &source);

        protected:
        	// AbstractPlayer
        	virtual void doSetTickInterval(qint32 interval);
        	virtual bool doSetVolume(qreal volume);
        	
        protected:
        	virtual void doPlay() = 0;
        	virtual void doPause() = 0;
        	virtual void doStop() = 0;
        	virtual int doSetMmfVolume(int mmfVolume) = 0;
        	virtual int openFile(RFile& file) = 0;
        	virtual void close() = 0;
        	
        protected:
        	void startTickTimer();
        	void stopTickTimer();
        	void initVolume(int maxVolume);
        	
        	/**
			 * Defined private state enumeration in order to add GroundState
			 */
			enum PrivateState
				{
				LoadingState    = Phonon::LoadingState,
				StoppedState    = Phonon::StoppedState,
				PlayingState    = Phonon::PlayingState,
				BufferingState	= Phonon::BufferingState,
				PausedState		= Phonon::PausedState,
				ErrorState		= Phonon::ErrorState,
				GroundState
				};

			/**
			 * Converts PrivateState into the corresponding Phonon::State
			 */
			Phonon::State phononState() const;
			
			/**
			 * Converts PrivateState into the corresponding Phonon::State
			 */
			static Phonon::State phononState(PrivateState state);

			/**
			 * Changes state and emits stateChanged()
			 */
			void changeState(PrivateState newState);
			
			/**
			 * Records error and changes state to ErrorState
			 */
			void setError(Phonon::ErrorType error);
			
            static qint64 toMilliSeconds(const TTimeIntervalMicroSeconds &);
        	
        Q_SIGNALS:
			void tick(qint64 time);
            void stateChanged(Phonon::State oldState,
                              Phonon::State newState);
        
        private Q_SLOTS:
			/**
			 * Receives signal from m_tickTimer
			 */
			void tick();

        private:
        	PrivateState				m_state;
        	Phonon::ErrorType			m_error;
        	
        	QScopedPointer<QTimer>		m_tickTimer;
            int							m_mmfMaxVolume;
            
            MediaSource					m_source;
            MediaSource					m_nextSource;
  
        };
    }
}

#endif

