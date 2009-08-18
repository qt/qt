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

#include <coemain.h>	// For CCoeEnv

#include "videoplayer.h"
#include "utils.h"

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::VideoPlayer::VideoPlayer()
{
    TRACE_CONTEXT(VideoPlayer::VideoPlayer, EAudioApi);
    TRACE_ENTRY_0();
   
    const TInt priority = 0;
    const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
    CCoeEnv* coeEnv = CCoeEnv::Static();
    RWsSession& wsSession = coeEnv->WsSession();
    CWsScreenDevice& screenDevice = *(coeEnv->ScreenDevice());
    /* DUMMY */ RWindow window;
    /* DUMMY */ TRect screenRect;
    /* DUMMY */ TRect clipRect;
    
    // TODO: is this the correct way to handle errors in constructing Symbian objects?
    TRAPD(err,
    	m_player = CVideoPlayerUtility::NewL
			(
			*this, 
			priority, preference,
			wsSession, screenDevice,
			window,
			screenRect, clipRect
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
	// TODO
}

void MMF::VideoPlayer::doPause()
{
	// TODO
}

void MMF::VideoPlayer::doStop()
{
	// TODO
}

int MMF::VideoPlayer::doSetVolume(int mmfVolume)
{
	// TODO
	Q_UNUSED(mmfVolume);
	return KErrNotSupported;
}

int MMF::VideoPlayer::openFile(RFile& file)
{
	// TODO
	Q_UNUSED(file);
	return KErrNotSupported;
}

void MMF::VideoPlayer::close()
{

}

void MMF::VideoPlayer::seek(qint64 ms)
{
	// TODO
	Q_UNUSED(ms);
}

bool MMF::VideoPlayer::hasVideo() const
{
	return true;
}

qint64 MMF::VideoPlayer::currentTime() const
{
	// TODO 
	return 0;
}

qint64 MMF::VideoPlayer::totalTime() const
{
	// TODO 
	return 0;
}


//-----------------------------------------------------------------------------
// MVideoPlayerUtilityObserver callbacks
//-----------------------------------------------------------------------------

void MMF::VideoPlayer::MvpuoOpenComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoOpenComplete, EVideoApi);
    TRACE_ENTRY("state %d error %d", state(), aError);

    // TODO
    
    TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPrepareComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoPrepareComplete, EVideoApi);
	TRACE_ENTRY("state %d error %d", state(), aError);

	// TODO
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoFrameReady(CFbsBitmap &aFrame, TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoFrameReady, EVideoApi);
	TRACE_ENTRY("state %d error %d", state(), aError);

	// TODO
	Q_UNUSED(aFrame);
	
	TRACE_EXIT_0();
}

void MMF::VideoPlayer::MvpuoPlayComplete(TInt aError)
{
	TRACE_CONTEXT(VideoPlayer::MvpuoPlayComplete, EVideoApi)
	TRACE_ENTRY("state %d error %d", state(), aError);

	// TODO
	
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



