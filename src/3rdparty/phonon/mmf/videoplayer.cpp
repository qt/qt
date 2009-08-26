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

#include <QUrl>
#include <QTimer>
#include <QWidget>

#include <coemain.h>	// For CCoeEnv
#include <coecntrl.h>

#include "videoplayer.h"
#include "utils.h"

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoPlayer::VideoPlayer()
								:	m_wsSession(NULL)
								,	m_screenDevice(NULL)
								,	m_window(NULL)
								,   m_totalTime(0)
{
	construct();
}

MMF::VideoPlayer::VideoPlayer(const AbstractPlayer& player)
								:	AbstractMediaPlayer(player)
								,	m_wsSession(NULL)
								,	m_screenDevice(NULL)
								,	m_window(NULL)
                                ,   m_totalTime(0)
{
	construct();
}

void MMF::VideoPlayer::construct()
{
	TRACE_CONTEXT(VideoPlayer::VideoPlayer, EVideoApi);
	TRACE_ENTRY_0();
	
	if(!m_videoOutput)
	{
		m_dummyVideoOutput.reset(new VideoOutput(NULL));
	}
	
	videoOutput().setObserver(this);
	
	const TInt priority = 0;
	const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
	
	getNativeWindowSystemHandles();
	
	// TODO: is this the correct way to handle errors which occur when
	// creating a Symbian object in the constructor of a Qt object?
	TRAPD(err,
		m_player = CVideoPlayerUtility::NewL
			(
			*this, 
			priority, preference,
			*m_wsSession, *m_screenDevice,
			*m_window,
			m_windowRect, m_clipRect
			)
		);
	
	if(KErrNone != err)
	{
		changeState(ErrorState);
	}
	
	TRACE_EXIT_0();
}

MMF::VideoPlayer::~VideoPlayer()
{
	TRACE_CONTEXT(VideoPlayer::~VideoPlayer, EVideoApi);
    TRACE_ENTRY_0();
		
	delete m_player;
	
	TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::doPlay()
{    
	m_player->Play();
}

void MMF::VideoPlayer::doPause()
{
	TRACE_CONTEXT(VideoPlayer::doPause, EVideoApi);

	TRAPD(err, m_player->PauseL());
	if(KErrNone != err)
	{
		TRACE("PauseL error %d", err);
		setError(NormalError);
	}
}

void MMF::VideoPlayer::doStop()
{
	m_player->Stop();
}

void MMF::VideoPlayer::doSeek(qint64 ms)
{
    TRACE_CONTEXT(VideoPlayer::doSeek, EVideoApi);
    
    TRAPD(err, m_player->SetPositionL(TTimeIntervalMicroSeconds(ms * 1000)));

    if(KErrNone != err)
    {
        TRACE("SetPositionL error %d", err);
        setError(NormalError);
    }
}

int MMF::VideoPlayer::setDeviceVolume(int mmfVolume)
{
	TRAPD(err, m_player->SetVolumeL(mmfVolume));
	return err;
}

int MMF::VideoPlayer::openFile(RFile& file)
{
	TRAPD(err, m_player->OpenFileL(file));
	return err;
}

void MMF::VideoPlayer::close()
{
	m_player->Close();
}

bool MMF::VideoPlayer::hasVideo() const
{
	return true;
}

qint64 MMF::VideoPlayer::currentTime() const
{
	TRACE_CONTEXT(VideoPlayer::currentTime, EVideoApi);

	TTimeIntervalMicroSeconds us;
    TRAPD(err, us = m_player->PositionL())

    qint64 result = 0;

    if(KErrNone == err)
    {
        result = toMilliSeconds(us);
    }
    else
    {
		TRACE("PositionL error %d", err);
		
		// If we don't cast away constness here, we simply have to ignore 
		// the error.
    	const_cast<VideoPlayer*>(this)->setError(NormalError);
    }
    
    return result;
}

qint64 MMF::VideoPlayer::totalTime() const
{
    return m_totalTime;
}


//-----------------------------------------------------------------------------
// MVideoPlayerUtilityObserver callbacks
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::MvpuoOpenComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoOpenComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

	if(KErrNone == aError)
	{
		m_player->Prepare();
	}
	else
	{	
		// TODO: set different error states according to value of aError?
		setError(NormalError);
	}
    
    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPrepareComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoPrepareComplete, EVideoApi);
	TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));
    
    TRAPD(err, doPrepareCompleteL(aError));
    
	if(KErrNone == err)
	{
		maxVolumeChanged(m_player->MaxVolume());
		
		videoOutput().setFrameSize(m_frameSize);	    

        emit totalTimeChanged(totalTime());
        changeState(StoppedState);
	}
    else
	{
	    // TODO: set different error states according to value of aError?
	    setError(NormalError);   
	}
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::doPrepareCompleteL(TInt aError)
{
    User::LeaveIfError(aError);
    
    // Get frame size
    TSize size;
    m_player->VideoFrameSizeL(size);
    m_frameSize = QSize(size.iWidth, size.iHeight);
    
    // Get duration
    m_totalTime = toMilliSeconds(m_player->DurationL());
}


void MMF::VideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoFrameReady, EVideoApi);
	TRACE_ENTRY("state %d error %d", state(), aError);

	// TODO
	Q_UNUSED(aFrame);
	Q_UNUSED(aError);	// suppress warnings in release builds
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPlayComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoPlayComplete, EVideoApi)
	TRACE_ENTRY("state %d error %d", state(), aError);

	// TODO
	Q_UNUSED(aError);	// suppress warnings in release builds
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoEvent(const TMMFEvent &aEvent)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoEvent, EVideoApi);
	TRACE_ENTRY("state %d", state());

	// TODO
	Q_UNUSED(aEvent);
	
	TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// VideoOutputObserver
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::videoOutputRegionChanged()
{
    TRACE_CONTEXT(VideoPlayer::videoOutputRegionChanged, EVideoInternal);
    TRACE_ENTRY_0();
    
#ifdef PHONON_MMF_DEBUG_VIDEO_OUTPUT
    videoOutput().dump();
#endif
    
    getNativeWindowSystemHandles();
        
    TRAPD(err,
        m_player->SetDisplayWindowL
            (
            *m_wsSession, *m_screenDevice,
            *m_window,
            m_windowRect, m_clipRect
            )
        );
    
    if(KErrNone != err)
    {
        TRACE("SetDisplayWindowL error %d", err);
        setError(NormalError);
    }
    
    TRACE_EXIT_0();
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

VideoOutput& MMF::VideoPlayer::videoOutput()
{
	TRACE_CONTEXT(VideoPlayer::videoOutput, EVideoInternal);
	TRACE("videoOutput 0x%08x dummy 0x%08x", m_videoOutput, m_dummyVideoOutput.data());

	return m_videoOutput ? *m_videoOutput : *m_dummyVideoOutput;
}

void MMF::VideoPlayer::videoOutputChanged()
{
	TRACE_CONTEXT(VideoPlayer::videoOutputChanged, EVideoInternal);
	TRACE_ENTRY_0();
	
	// Lazily construct a dummy output if needed here
	if(!m_videoOutput and m_dummyVideoOutput.isNull())
	{
		m_dummyVideoOutput.reset(new VideoOutput(NULL));
	}
	
	videoOutput().setObserver(this);
	
	videoOutput().setFrameSize(m_frameSize);
	
	videoOutputRegionChanged();
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::getNativeWindowSystemHandles()
{
	TRACE_CONTEXT(VideoPlayer::getNativeWindowSystemHandles, EVideoInternal);

	VideoOutput& output = videoOutput();
	CCoeControl* const control = output.winId();
	
	TRACE("control 0x%08x", control);
	TRACE("control isVisible %d", control->IsVisible());
	TRACE("control isDimmed  %d", control->IsDimmed());
	TRACE("control hasBorder %d", control->HasBorder());
	TRACE("control position  %d %d",
		control->Position().iX, control->Position().iY);
	TRACE("control rect      %d %d - %d %d",
		control->Rect().iTl.iX, control->Rect().iTl.iY,
		control->Rect().iBr.iX, control->Rect().iBr.iY);
	
	CCoeEnv* const coeEnv = control->ControlEnv();
	
	m_wsSession = &(coeEnv->WsSession());
	
	TRACE("session handle    0x%08x", m_wsSession->Handle());
	
	m_screenDevice = coeEnv->ScreenDevice();
	
	TRACE("device srv handle 0x%08x", m_screenDevice->WsHandle());
	
	m_window = control->DrawableWindow();
	
	TRACE("window cli handle 0x%08x", m_window->ClientHandle());
	TRACE("window srv handle 0x%08x", m_window->WsHandle());
	TRACE("window group      %d", m_window->WindowGroupId());
	TRACE("window position   %d %d",
		m_window->Position().iX, m_window->Position().iY);
	TRACE("window abs_pos    %d %d",
	    m_window->AbsPosition().iX, m_window->AbsPosition().iY);
	TRACE("window size       %d %d",
        m_window->Size().iWidth, m_window->Size().iHeight);
	
#ifdef PHONON_MMF_HARD_CODE_VIDEO_RECT
	// HACK: why isn't control->Rect updated following a call to 
	// updateGeometry on the parent widget?
	m_windowRect = TRect(0,100,320,250);
#else
	m_windowRect = control->Rect();
#endif
	
	m_clipRect = m_windowRect;
}
