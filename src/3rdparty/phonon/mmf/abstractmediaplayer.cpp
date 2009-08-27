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

#include "abstractmediaplayer.h"
#include "defs.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int		NullMaxVolume = -1;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AbstractMediaPlayer::AbstractMediaPlayer() :
							m_state(GroundState)
						,	m_error(NoError)
						,	m_playPending(false)
						,	m_tickTimer(new QTimer(this))
						,	m_volume(InitialVolume)
						,	m_mmfMaxVolume(NullMaxVolume)
{
	connect(m_tickTimer.data(), SIGNAL(timeout()), this, SLOT(tick()));
}

MMF::AbstractMediaPlayer::AbstractMediaPlayer(const AbstractPlayer& player) :
							AbstractPlayer(player)
						,	m_state(GroundState)
						,	m_error(NoError)
						,	m_playPending(false)
						,	m_tickTimer(new QTimer(this))
						,	m_volume(InitialVolume)
						,	m_mmfMaxVolume(NullMaxVolume)
{
	connect(m_tickTimer.data(), SIGNAL(timeout()), this, SLOT(tick()));
}

MMF::AbstractMediaPlayer::~AbstractMediaPlayer()
{
	
}


//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::play()
{
    TRACE_CONTEXT(AbstractMediaPlayer::play, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    switch(m_state)
    {
        case GroundState:
            // Is this the correct error?  Really we want 'NotReadyError'
            m_error = NormalError;
            changeState(ErrorState);
            break;
            
        case LoadingState:
			m_playPending = true;
			break;
        	
        case StoppedState:
        case PausedState:
            doPlay();
            startTickTimer();
            changeState(PlayingState);
            break;

        case PlayingState:
        case BufferingState:
        case ErrorState:
            // Do nothing
            break;

        // Protection against adding new states and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", m_state);
}

void MMF::AbstractMediaPlayer::pause()
{
    TRACE_CONTEXT(AbstractMediaPlayer::pause, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    m_playPending = false;
    
    switch(m_state)
    {
        case GroundState:
        case LoadingState:
        case StoppedState:
        case PausedState:
        case ErrorState:
            // Do nothing
            break;

        case PlayingState:
        case BufferingState:
            doPause();
            stopTickTimer();
            changeState(PausedState);
            break;

        // Protection against adding new states and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", m_state);
}

void MMF::AbstractMediaPlayer::stop()
{
    TRACE_CONTEXT(AbstractMediaPlayer::stop, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    m_playPending = false;
    
    switch(m_state)
    {
        case GroundState:
        case LoadingState:
        case StoppedState:
        case ErrorState:
            // Do nothing
            break;

        case PlayingState:
        case BufferingState:
        case PausedState:
            doStop();
            stopTickTimer();
            changeState(StoppedState);
            break;

        // Protection against adding new states and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", m_state);
}

void MMF::AbstractMediaPlayer::seek(qint64 ms)
{
    TRACE_CONTEXT(AbstractMediaPlayer::seek, EAudioApi);
    TRACE_ENTRY("state %d pos %Ld", state(), ms);

    // TODO: put a state guard in here
    
    const bool tickTimerWasRunning = m_tickTimer->isActive();
    stopTickTimer();
        
    doSeek(ms);
    
    if(tickTimerWasRunning)
    {
        startTickTimer();
    }
    
    TRACE_EXIT_0();
}

bool MMF::AbstractMediaPlayer::isSeekable() const
{
	return true;
}

void MMF::AbstractMediaPlayer::doSetTickInterval(qint32 interval)
{
    TRACE_CONTEXT(AbstractMediaPlayer::doSetTickInterval, EAudioApi);
    TRACE_ENTRY("state %d m_interval %d interval %d", m_state, tickInterval(), interval);

    m_tickTimer->setInterval(interval);

    TRACE_EXIT_0();
}

Phonon::ErrorType MMF::AbstractMediaPlayer::errorType() const
{
    const Phonon::ErrorType result = (ErrorState == m_state)
        ? m_error : NoError;
    return result;
}

QString MMF::AbstractMediaPlayer::errorString() const
{
    // TODO: put in proper error strings
    QString result;
    return result;
}

Phonon::State MMF::AbstractMediaPlayer::state() const
{
    return phononState(m_state);
}

MediaSource MMF::AbstractMediaPlayer::source() const
{
    return m_source;
}

void MMF::AbstractMediaPlayer::setFileSource(const MediaSource &source, RFile& file)
{
    TRACE_CONTEXT(AbstractMediaPlayer::setFileSource, EAudioApi);
    TRACE_ENTRY("state %d source.type %d", m_state, source.type());

    close();
    changeState(GroundState);

    // TODO: is it correct to assign even if the media type is not supported in
    // the switch statement below?
    m_source = source;

    TInt symbianErr = KErrNone;

    switch(m_source.type())
    {
        case MediaSource::LocalFile:
        {
            // TODO: work out whose responsibility it is to ensure that paths
            // are Symbian-style, i.e. have backslashes for path delimiters.
            // Until then, use this utility function...
            //const QHBufC filename = Utils::symbianFilename(m_source.fileName());
            //TRAP(symbianErr, m_player->OpenFileL(*filename));
            
            // Open using shared filehandle
            // This is a temporary hack to work around KErrInUse from MMF
            // client utility OpenFileL calls
            //TRAP(symbianErr, m_player->OpenFileL(file));
            
			symbianErr = openFile(file);    
            break;
        }

        case MediaSource::Url:
        {
			TRACE_0("Source type not supported");
			// TODO: support opening URLs
			symbianErr = KErrNotSupported;
			break;
        }

        case MediaSource::Invalid:
        case MediaSource::Disc:
        case MediaSource::Stream:
        	TRACE_0("Source type not supported");
            symbianErr = KErrNotSupported;
            break;

        case MediaSource::Empty:
        	TRACE_0("Empty source - doing nothing");
            TRACE_EXIT_0();
            return;

        // Protection against adding new media types and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidMediaTypePanic);
    }

    if(KErrNone == symbianErr)
    {
        changeState(LoadingState);
    }
    else
    {
		TRACE("error %d", symbianErr)
    
        // TODO: do something with the value of symbianErr?
        m_error = NormalError;
        changeState(ErrorState);
    }

    TRACE_EXIT_0();
}

void MMF::AbstractMediaPlayer::setNextSource(const MediaSource &source)
{
    TRACE_CONTEXT(AbstractMediaPlayer::setNextSource, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    // TODO: handle 'next source'

    m_nextSource = source;
    Q_UNUSED(source);

    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VolumeObserver
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::volumeChanged(qreal volume)
{
    TRACE_CONTEXT(AbstractMediaPlayer::volumeChanged, EAudioInternal);
    TRACE_ENTRY("state %d", m_state);
    
    m_volume = volume;
    doVolumeChanged();
    
    TRACE_EXIT_0();
}


void MMF::AbstractMediaPlayer::doVolumeChanged()
{
	switch(m_state)
	{
		case GroundState:
		case LoadingState:
		case ErrorState:
			// Do nothing
			break;

		case StoppedState:
		case PausedState:
		case PlayingState:
		case BufferingState:
		{
			const int err = setDeviceVolume(m_volume * m_mmfMaxVolume);
			
			if(KErrNone != err)
			{
				m_error = NormalError;
				changeState(ErrorState);
			}
			break;
		}

		// Protection against adding new states and forgetting to update this
		// switch
		default:
			Utils::panic(InvalidStatePanic);
	}
}


//-----------------------------------------------------------------------------
// Protected functions
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::startTickTimer()
{
	m_tickTimer->start(tickInterval());
}

void MMF::AbstractMediaPlayer::stopTickTimer()
{
	m_tickTimer->stop();
}

void MMF::AbstractMediaPlayer::maxVolumeChanged(int mmfMaxVolume)
{
	m_mmfMaxVolume = mmfMaxVolume;
	doVolumeChanged();
}

Phonon::State MMF::AbstractMediaPlayer::phononState() const
{
	return phononState(m_state);
}

Phonon::State MMF::AbstractMediaPlayer::phononState(PrivateState state)
{
    const Phonon::State phononState =
        GroundState == state
        ?    Phonon::LoadingState
        :    static_cast<Phonon::State>(state);

    return phononState;
}

void MMF::AbstractMediaPlayer::changeState(PrivateState newState)
{
    TRACE_CONTEXT(AbstractMediaPlayer::changeState, EAudioInternal);
    TRACE_ENTRY("state %d newState %d", m_state, newState);

    // TODO: add some invariants to check that the transition is valid

    const Phonon::State oldPhononState = phononState(m_state);
    const Phonon::State newPhononState = phononState(newState);
    if(oldPhononState != newPhononState)
    {
        TRACE("emit stateChanged(%d, %d)", newPhononState, oldPhononState);
        emit stateChanged(newPhononState, oldPhononState);
    }

    m_state = newState;
    
    // Check whether play() was called while clip was being loaded.  If so,
    // playback should be started now
    if(
    		LoadingState == oldPhononState
    	and	StoppedState == newPhononState
    	and m_playPending
    )
    {
		TRACE_0("play was called while loading; starting playback now");
		m_playPending = false;
		play();
    }
    
    TRACE_EXIT_0();
}

void MMF::AbstractMediaPlayer::setError(Phonon::ErrorType error)
{
    TRACE_CONTEXT(AbstractMediaPlayer::setError, EAudioInternal);
    TRACE_ENTRY("state %d error %d", m_state, error);

    m_error = error;
    changeState(ErrorState);

    TRACE_EXIT_0();
}

qint64 MMF::AbstractMediaPlayer::toMilliSeconds(const TTimeIntervalMicroSeconds &in)
{
    return in.Int64() / 1000;
}


//-----------------------------------------------------------------------------
// Slots
//-----------------------------------------------------------------------------

void MMF::AbstractMediaPlayer::tick()
{
    emit tick(currentTime());
}

QT_END_NAMESPACE

