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

#include <QDir>
#include <QUrl>

#include "audioplayer.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AudioPlayer
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioPlayer::AudioPlayer(MediaObject *parent, const AbstractPlayer *player)
        :   AbstractMediaPlayer(parent, player)
        ,   m_totalTime(0)
{
    construct();
}

void MMF::AudioPlayer::construct()
{
    TRACE_CONTEXT(AudioPlayer::AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    NativePlayer *player = 0;
    QT_TRAP_THROWING(player = NativePlayer::NewL(*this, 0, EMdaPriorityPreferenceNone));
    m_player.reset(player);
    m_player->RegisterForAudioLoadingNotification(*this);

    TRACE_EXIT_0();
}

MMF::AudioPlayer::~AudioPlayer()
{
    TRACE_CONTEXT(AudioPlayer::~AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    TRACE_EXIT_0();
}

MMF::AudioPlayer::NativePlayer *MMF::AudioPlayer::nativePlayer() const
{
    return m_player.data();
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

void MMF::AudioPlayer::doSeek(qint64 ms)
{
    m_player->SetPosition(TTimeIntervalMicroSeconds(ms * 1000));
}

int MMF::AudioPlayer::setDeviceVolume(int mmfVolume)
{
    /* In SDK 3.1, SetVolume() returns void. If we're compiling against
     * 3.1, we handle it with ifdefs. However, if we compile against a later
     * SDK but are _running_ against 3.1, we avoid returning from an undefined
     * stack by doing a runtime check of the SDK version. */
#if !defined(__SERIES60_31__)
    const int err = m_player->SetVolume(mmfVolume);
    if (QSysInfo::s60Version() >= QSysInfo::SV_S60_3_2)
        return err;
    else
        return KErrNone;
 #else
     m_player->SetVolume(mmfVolume);
     return KErrNone;
#endif
}

int MMF::AudioPlayer::openFile(const QString &fileName)
{
    const QHBufC nativeFileName(QDir::toNativeSeparators(fileName));
    TRAPD(err, m_player->OpenFileL(*nativeFileName));
    return err;
}

int MMF::AudioPlayer::openFile(RFile& file)
{
    TRAPD(err, m_player->OpenFileL(file));

#ifdef QT_PHONON_MMF_AUDIO_DRM
    if (KErrNone == err) {
        // There appears to be a bug in the CDrmPlayerUtility implementation (at least
        // in S60 5.x) whereby the player does not check whether the loading observer
        // pointer is null before dereferencing it.  Therefore we must register for
        // loading notification, even though we do nothing in the callback functions.
        m_player->RegisterForAudioLoadingNotification(*this);
    }
#endif

    return err;
}

int MMF::AudioPlayer::openUrl(const QString& /*url*/, int /*iap*/)
{
    // Streaming playback is generally not supported by the implementation
    // of the audio player API, so we use CVideoPlayerUtility for both
    // audio and video streaming.
    Utils::panic(AudioUtilityUrlNotSupported);

    // Silence warning
    return 0;
}

int MMF::AudioPlayer::openDescriptor(const TDesC8 &des)
{
    TRAPD(err, m_player->OpenDesL(des));
    return err;
}

int MMF::AudioPlayer::bufferStatus() const
{
    int result = 0;
    TRAP_IGNORE(m_player->GetAudioLoadingProgressL(result));
    return result;
}

void MMF::AudioPlayer::doClose()
{
    m_player->Close();
}

bool MMF::AudioPlayer::hasVideo() const
{
    return false;
}

qint64 MMF::AudioPlayer::getCurrentTime() const
{
    TRACE_CONTEXT(AudioPlayer::getCurrentTime, EAudioApi);

    TTimeIntervalMicroSeconds us;
    const TInt err = m_player->GetPosition(us);

    qint64 result = 0;

    if (KErrNone == err) {
        result = toMilliSeconds(us);
    } else {
        TRACE("GetPosition err %d", err);

        // If we don't cast away constness here, we simply have to ignore
        // the error.
        const_cast<AudioPlayer*>(this)->setError(tr("Getting position failed"), err);
    }

    return result;
}

qint64 MMF::AudioPlayer::totalTime() const
{
    return m_totalTime;
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

    __ASSERT_ALWAYS(LoadingState == state() ||
                    progressiveDownloadStalled() && BufferingState == state(),
                    Utils::panic(InvalidStatePanic));

    if (KErrNone == aError) {
        maxVolumeChanged(m_player->MaxVolume());
        m_totalTime = toMilliSeconds(m_player->Duration());
        emit totalTimeChanged(m_totalTime);
    }

    loadingComplete(aError);

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

    // Call base class function which handles end of playback for both
    // audio and video clips.
    playbackComplete(aError);

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


//-----------------------------------------------------------------------------
// MAudioLoadingObserver callbacks
//-----------------------------------------------------------------------------

void MMF::AudioPlayer::MaloLoadingStarted()
{
    bufferingStarted();
}

void MMF::AudioPlayer::MaloLoadingComplete()
{
    bufferingComplete();
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

int MMF::AudioPlayer::numberOfMetaDataEntries() const
{
    int numberOfEntries = 0;
    m_player->GetNumberOfMetaDataEntries(numberOfEntries); // ignoring return code
    return numberOfEntries;
}

QPair<QString, QString> MMF::AudioPlayer::metaDataEntry(int index) const
{
    CMMFMetaDataEntry *entry = 0;
    QT_TRAP_THROWING(entry = m_player->GetMetaDataEntryL(index));
    return QPair<QString, QString>(qt_TDesC2QString(entry->Name()), qt_TDesC2QString(entry->Value()));
}


QT_END_NAMESPACE

