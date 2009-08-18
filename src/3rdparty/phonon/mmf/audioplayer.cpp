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

#include "audioplayer.h"
#include "utils.h"

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioPlayer::AudioPlayer() : m_player(NULL)                             
{
    TRACE_CONTEXT(AudioPlayer::AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    // TODO: is this the correct way to handle errors in constructing Symbian objects?
    TRAPD(err, m_player = CPlayerType::NewL(*this, 0, EMdaPriorityPreferenceNone));
    if(KErrNone != err)
    {
		changeState(ErrorState);
    }
    
    TRACE_EXIT_0();
}

MMF::AudioPlayer::~AudioPlayer()
{
    TRACE_CONTEXT(AudioPlayer::~AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    delete m_player;

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

void MMF::AudioPlayer::doPlay()
{
	m_player->Play();
}

void MMF::AudioPlayer::doPause()
{
	m_player->Pause();
}

void MMF::AudioPlayer::doStop()
{
	m_player->Stop();
}

int MMF::AudioPlayer::doSetVolume(int mmfVolume)
{
	return m_player->SetVolume(mmfVolume);
}

int MMF::AudioPlayer::openFile(RFile& file)
{
	TRAPD(err, m_player->OpenFileL(file));
	
#ifdef QT_PHONON_MMF_AUDIO_DRM
	if(KErrNone == err)
	{
        // There appears to be a bug in the CDrmPlayerUtility implementation (at least
        // in S60 5.x) whereby the player does not check whether the loading observer
        // pointer is null before dereferencing it.  Therefore we must register for
        // loading notification, even though we do nothing in the callback functions.
        m_player->RegisterForAudioLoadingNotification(*this);
	}
#endif
	
	return err;
}

void MMF::AudioPlayer::close()
{
	m_player->Close();
}

void MMF::AudioPlayer::seek(qint64 ms)
{
    TRACE_CONTEXT(AudioPlayer::seek, EAudioApi);
    TRACE_ENTRY("state %d pos %Ld", state(), ms);

    m_player->SetPosition(TTimeIntervalMicroSeconds(ms));

    TRACE_EXIT_0();
}

bool MMF::AudioPlayer::hasVideo() const
{
    TRACE_CONTEXT(AudioPlayer::hasVideo, EAudioApi);
    TRACE_ENTRY("state %d", state());
    TRACE_RETURN("%d", false);
}

qint64 MMF::AudioPlayer::currentTime() const
{
    TRACE_CONTEXT(AudioPlayer::currentTime, EAudioApi);
    TRACE_ENTRY("state %d", state());

    TTimeIntervalMicroSeconds us;
    const TInt err = m_player->GetPosition(us);

    qint64 result = -1;

    if(KErrNone == err) {
        result = toMilliSeconds(us);
    }

    TRACE_RETURN("%Ld", result);
}

qint64 MMF::AudioPlayer::totalTime() const
{
    TRACE_CONTEXT(AudioPlayer::totalTime, EAudioApi);
    TRACE_ENTRY("state %d", state());

    const qint64 result = toMilliSeconds(m_player->Duration());

    TRACE_RETURN("%Ld", result);
}


//-----------------------------------------------------------------------------
// Symbian multimedia client observer callbacks
//-----------------------------------------------------------------------------

#ifdef QT_PHONON_MMF_AUDIO_DRM
void MMF::AudioPlayer::MdapcInitComplete(TInt aError,
                                         const TTimeIntervalMicroSeconds &)
#else
void MMF::AudioPlayer::MapcInitComplete(TInt aError,
                                         const TTimeIntervalMicroSeconds &)
#endif
{
    TRACE_CONTEXT(AudioPlayer::MapcInitComplete, EAudioInternal);
    TRACE_ENTRY("state %d error %d", state(), aError);

    __ASSERT_ALWAYS(LoadingState == state(), Utils::panic(InvalidStatePanic));

    if(KErrNone == aError)
    {
        TInt volume = 0;
        aError = m_player->GetVolume(volume);
        if(KErrNone == aError)
        {
            initVolume(volume, m_player->MaxVolume());

            emit totalTimeChanged();
            changeState(StoppedState);
        }
    }
    else
    {
        // TODO: set different error states according to value of aError?
        setError(NormalError);
    }

    TRACE_EXIT_0();
}

#ifdef QT_PHONON_MMF_AUDIO_DRM
void MMF::AudioPlayer::MdapcPlayComplete(TInt aError)
#else
void MMF::AudioPlayer::MapcPlayComplete(TInt aError)
#endif
{
    TRACE_CONTEXT(AudioPlayer::MapcPlayComplete, EAudioInternal);
    TRACE_ENTRY("state %d error %d", state(), aError);

    stopTickTimer();

    if(KErrNone == aError)
    {
        changeState(StoppedState);
        // TODO: move on to m_nextSource
    }
    else
    {
        // TODO: do something with aError?
        setError(NormalError);
    }

/*
    if(aError == KErrNone) {
        if(m_nextSource.type() == MediaSource::Empty) {
            emit finished();
        } else {
            setSource(m_nextSource);
            m_nextSource = MediaSource();
        }

        changeState(StoppedState);
    }
    else {
        m_error = NormalError;
        changeState(ErrorState);
    }
*/

    TRACE_EXIT_0();
}

#ifdef QT_PHONON_MMF_AUDIO_DRM
void MMF::AudioPlayer::MaloLoadingStarted()
{

}

void MMF::AudioPlayer::MaloLoadingComplete()
{

}
#endif // QT_PHONON_MMF_AUDIO_DRM
















