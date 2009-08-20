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

MMF::VideoPlayer::VideoPlayer() : m_widget(new QWidget())
{
	construct();
}

MMF::VideoPlayer::VideoPlayer(const AbstractPlayer& player)
								: AbstractMediaPlayer(player)
								, m_widget(new QWidget())
{
	construct();
}

void MMF::VideoPlayer::construct()
{
	TRACE_CONTEXT(VideoPlayer::VideoPlayer, EAudioApi);
	TRACE_ENTRY_0();
	
	CCoeControl* control = m_widget->winId();
	CCoeEnv* coeEnv = control->ControlEnv();
	
	const TInt priority = 0;
	const TMdaPriorityPreference preference = EMdaPriorityPreferenceNone;
	RWsSession& wsSession = coeEnv->WsSession();
	CWsScreenDevice& screenDevice = *(coeEnv->ScreenDevice());
	RDrawableWindow& window = *(control->DrawableWindow());
	const TRect screenRect = control->Rect();
	const TRect clipRect = control->Rect();
	
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
	m_player->Play();
}

void MMF::VideoPlayer::doPause()
{
	TRAPD(err, m_player->PauseL());
	if(KErrNone != err)
	{
		setError(NormalError);
	}
}

void MMF::VideoPlayer::doStop()
{
	m_player->Stop();
}

int MMF::VideoPlayer::doSetMmfVolume(int mmfVolume)
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

void MMF::VideoPlayer::seek(qint64 ms)
{
	TRACE_CONTEXT(VideoPlayer::seek, EAudioApi);
    TRACE_ENTRY("state %d pos %Ld", state(), ms);

    TRAPD(err, m_player->SetPositionL(TTimeIntervalMicroSeconds(ms)));

    if(KErrNone != err)
	{
		setError(NormalError);
	}
    
    TRACE_EXIT_0();
}

bool MMF::VideoPlayer::hasVideo() const
{
	return true;
}

qint64 MMF::VideoPlayer::currentTime() const
{
	TTimeIntervalMicroSeconds us;
    TRAPD(err, us = m_player->PositionL())

    qint64 result = 0;

    if(KErrNone == err)
    {
        result = toMilliSeconds(us);
    }
    
    return result;
}

qint64 MMF::VideoPlayer::totalTime() const
{
	qint64 result = 0;
	TRAPD(err, result = toMilliSeconds(m_player->DurationL()));
	
	if(KErrNone != err)
	{
		// If we don't cast away constness here, we simply have to ignore 
		// the error.
		const_cast<VideoPlayer*>(this)->setError(NormalError);
	}
	
	return result;
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

	if(KErrNone == aError)
	{
		initVolume(m_player->MaxVolume());

		emit totalTimeChanged();
		changeState(StoppedState);
	}
	else
	{
		// TODO: set different error states according to value of aError?
		setError(NormalError);
	}
	
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



