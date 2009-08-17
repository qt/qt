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
#include <private/qcore_symbian_p.h>

#include "audioplayer.h"
#include "audiooutput.h"
#include "utils.h"

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const qint32    DefaultTickInterval = 20;
const int        NullMaxVolume = -1;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioPlayer::AudioPlayer(QObject *parent) : m_player(NULL)
                                               , m_audioOutput(NULL)
                                               , m_error(NoError)
                                               , m_state(GroundState)
                                               , m_tickInterval(DefaultTickInterval)
                                               , m_tickTimer(NULL)
                                               , m_volume(0.0)
                                               , m_maxVolume(NullMaxVolume)
{
    TRACE_CONTEXT(AudioPlayer::AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    Q_UNUSED(parent);

    // TODO: should leaves be trapped in the constructor?
    m_player = CPlayerType::NewL(*this, 0, EMdaPriorityPreferenceNone);

    m_tickTimer = new QTimer(this);
    connect(m_tickTimer, SIGNAL(timeout()), this, SLOT(tick()));

    TRACE_EXIT_0();
}

MMF::AudioPlayer::~AudioPlayer()
{
    TRACE_CONTEXT(AudioPlayer::~AudioPlayer, EAudioApi);
    TRACE_ENTRY_0();

    delete m_tickTimer;
    delete m_player;

    TRACE_EXIT_0();
}

//-----------------------------------------------------------------------------
// AudioPlayerInterface
//-----------------------------------------------------------------------------

void MMF::AudioPlayer::play()
{
    TRACE_CONTEXT(AudioPlayer::play, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    switch(m_state)
    {
        case GroundState:
        case LoadingState:
            // Is this the correct error?  Really we want 'NotReadyError'
            m_error = NormalError;
            changeState(ErrorState);
            break;

        case StoppedState:
        case PausedState:
            m_player->Play();
            m_tickTimer->start(m_tickInterval);
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

void MMF::AudioPlayer::pause()
{
    TRACE_CONTEXT(AudioPlayer::pause, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

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
            m_player->Pause();
            m_tickTimer->stop();
            changeState(PausedState);
            break;

        // Protection against adding new states and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", m_state);
}

void MMF::AudioPlayer::stop()
{
    TRACE_CONTEXT(AudioPlayer::stop, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

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
            m_player->Stop();
            m_tickTimer->stop();
            changeState(StoppedState);
            break;

        // Protection against adding new states and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_EXIT("state %d", m_state);
}

void MMF::AudioPlayer::seek(qint64 ms)
{
    TRACE_CONTEXT(AudioPlayer::seek, EAudioApi);
    TRACE_ENTRY("state %d pos %Ld", m_state, ms);

    m_player->SetPosition(TTimeIntervalMicroSeconds(ms));

    TRACE_EXIT_0();
}

qint32 MMF::AudioPlayer::tickInterval() const
{
    TRACE_CONTEXT(AudioPlayer::tickInterval, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    TRACE_RETURN("%d", m_tickInterval);
}

void MMF::AudioPlayer::setTickInterval(qint32 interval)
{
    TRACE_CONTEXT(AudioPlayer::setTickInterval, EAudioApi);
    TRACE_ENTRY("state %d m_interval %d interval %d", m_state, m_tickInterval, interval);

    m_tickInterval = interval;
    m_tickTimer->setInterval(interval);

    TRACE_EXIT_0();
}

bool MMF::AudioPlayer::hasVideo() const
{
    TRACE_CONTEXT(AudioPlayer::hasVideo, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    // TODO: re-factor this class so that audio playback and video playback are
    // delegated to separate classes, which internally hoat a
    // CMdaAudioPlayerUtility / CVideoPlayerUtility instance as appropriate.

    TRACE_RETURN("%d", false);
}

bool MMF::AudioPlayer::isSeekable() const
{
    TRACE_CONTEXT(AudioPlayer::isSeekable, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    TRACE_RETURN("%d", true);
}

Phonon::State MMF::AudioPlayer::state() const
{
    TRACE_CONTEXT(AudioPlayer::state, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    const Phonon::State result = phononState(m_state);

    TRACE_RETURN("%d", result);
}

qint64 MMF::AudioPlayer::currentTime() const
{
    TRACE_CONTEXT(AudioPlayer::currentTime, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    TTimeIntervalMicroSeconds us;
    const TInt err = m_player->GetPosition(us);

    qint64 result = -1;

    if(KErrNone == err) {
        result = toMilliSeconds(us);
    }

    TRACE_RETURN("%Ld", result);
}

QString MMF::AudioPlayer::errorString() const
{
    TRACE_CONTEXT(AudioPlayer::errorString, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    TRACE_EXIT_0();
    // TODO: put in proper error strings
    QString result;
    return result;
}

Phonon::ErrorType MMF::AudioPlayer::errorType() const
{
    TRACE_CONTEXT(AudioPlayer::errorType, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    const Phonon::ErrorType result = (ErrorState == m_state)
        ? m_error : NoError;

    TRACE_RETURN("%d", result);
}

qint64 MMF::AudioPlayer::totalTime() const
{
    TRACE_CONTEXT(AudioPlayer::totalTime, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    const qint64 result = toMilliSeconds(m_player->Duration());

    TRACE_RETURN("%Ld", result);
}

MediaSource MMF::AudioPlayer::source() const
{
    TRACE_CONTEXT(AudioPlayer::source, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    TRACE_EXIT_0();
    return m_mediaSource;
}

void MMF::AudioPlayer::setSource(const MediaSource &source)
{
    TRACE_CONTEXT(AudioPlayer::setSource, EAudioApi);
    TRACE_ENTRY("state %d source.type %d", m_state, source.type());

    m_player->Close();
    changeState(GroundState);

    // TODO: is it correct to assign even if the media type is not supported in
    // the switch statement below?
    m_mediaSource = source;

    TInt symbianErr = KErrNone;

    switch(m_mediaSource.type())
    {
        case MediaSource::LocalFile:
        {
            // TODO: work out whose responsibility it is to ensure that paths
            // are Symbian-style, i.e. have backslashes for path delimiters.
            // Until then, use this utility function...
            const QHBufC filename = Utils::symbianFilename(m_mediaSource.fileName());
            TRAP(symbianErr, m_player->OpenFileL(*filename));
            break;
        }

        case MediaSource::Url:
        {
            const QHBufC filename(m_mediaSource.url().toString());
            TRAP(symbianErr, m_player->OpenUrlL(*filename));
            break;
        }

        case MediaSource::Invalid:
        case MediaSource::Disc:
        case MediaSource::Stream:
            symbianErr = KErrNotSupported;
            break;

        case MediaSource::Empty:
            TRACE_EXIT_0();
            return;

        // Protection against adding new media types and forgetting to update this switch
        default:
            TRACE_PANIC(InvalidMediaTypePanic);
    }

    if(KErrNone == symbianErr)
    {
#ifdef QT_PHONON_MMF_AUDIO_DRM
        // There appears to be a bug in the CDrmPlayerUtility implementation (at least
        // in S60 5.x) whereby the player does not check whether the loading observer
        // pointer is null before dereferencing it.  Therefore we must register for
        // loading notification, even though we do nothing in the callback functions.
        m_player->RegisterForAudioLoadingNotification(*this);
#endif
        changeState(LoadingState);
    }
    else
    {
        // TODO: do something with the value of symbianErr?
        m_error = NormalError;
        changeState(ErrorState);
    }

    TRACE_EXIT_0();
}

void MMF::AudioPlayer::setNextSource(const MediaSource &source)
{
    TRACE_CONTEXT(AudioPlayer::setNextSource, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    // TODO: handle 'next source'

    m_nextSource = source;
    Q_UNUSED(source);

    TRACE_EXIT_0();
}

qint32 MMF::AudioPlayer::prefinishMark() const
{
    TRACE_CONTEXT(AudioPlayer::prefinishMark, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    // TODO: implement prefinish mark
    const qint32 result = 0;
    TRACE_RETURN("%d", result);
}

void MMF::AudioPlayer::setPrefinishMark(qint32 mark)
{
    TRACE_CONTEXT(AudioPlayer::setPrefinishMark, EAudioApi);
    TRACE_ENTRY("state %d mark %d", m_state, mark);
    Q_UNUSED(mark); // to silence warnings in release builds

    // TODO: implement prefinish mark

    TRACE_EXIT_0();
}

qint32 MMF::AudioPlayer::transitionTime() const
{
    TRACE_CONTEXT(AudioPlayer::transitionTime, EAudioApi);
    TRACE_ENTRY("state %d", m_state);

    // TODO: implement transition time
    const qint32 result = 0;
    TRACE_RETURN("%d", result);
}

void MMF::AudioPlayer::setTransitionTime(qint32 time)
{
    TRACE_CONTEXT(AudioPlayer::setTransitionTime, EAudioApi);
    TRACE_ENTRY("state %d time %d", m_state, time);
    Q_UNUSED(time); // to silence warnings in release builds

    // TODO: implement transition time

    TRACE_EXIT_0();
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
    TRACE_ENTRY("state %d error %d", m_state, aError);

    __ASSERT_ALWAYS(LoadingState == m_state, Utils::panic(InvalidStatePanic));

    if(KErrNone == aError)
    {
        TInt volume = 0;
        aError = m_player->GetVolume(volume);
        if(KErrNone == aError)
        {
            m_maxVolume = m_player->MaxVolume();
            m_volume = static_cast<qreal>(volume) / m_maxVolume;

            if(m_audioOutput)
            {
                // Trigger AudioOutput signal
                m_audioOutput->triggerVolumeChanged(m_volume);
            }

            emit totalTimeChanged();
            changeState(StoppedState);
        }
    }
    else
    {
        // TODO: set different error states according to value of aError?
        m_error = NormalError;
        changeState(ErrorState);
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
    TRACE_ENTRY("state %d error %d", m_state, aError);

    m_tickTimer->stop();

    if(KErrNone == aError)
    {
        changeState(StoppedState);
        // TODO: move on to m_nextSource
    }
    else
    {
        // TODO: do something with aError?
        m_error = NormalError;
        changeState(ErrorState);
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


//-----------------------------------------------------------------------------
// Volume
//-----------------------------------------------------------------------------

qreal MMF::AudioPlayer::volume() const
{
    return m_volume;
}

bool MMF::AudioPlayer::setVolume(qreal volume)
{
    TRACE_CONTEXT(AudioPlayer::setVolume, EAudioInternal);
    TRACE_ENTRY("state %d", m_state);

    bool volumeChanged = false;

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
            if(volume != m_volume)
            {
                const int err = m_player->SetVolume(volume * m_maxVolume);
                if(KErrNone == err)
                {
                    m_volume = volume;
                    volumeChanged = true;
                }
                else
                {
                    m_error = NormalError;
                    changeState(ErrorState);
                }
            }
            break;
        }

        // Protection against adding new states and forgetting to update this
        // switch
        default:
            TRACE_PANIC(InvalidStatePanic);
    }

    TRACE_RETURN("%d", volumeChanged);
}

void MMF::AudioPlayer::setAudioOutput(AudioOutput* audioOutput)
{
    m_audioOutput = audioOutput;
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

qint64 MMF::AudioPlayer::toMilliSeconds(const TTimeIntervalMicroSeconds &in)
{
    return in.Int64() / 1000;
}

Phonon::State MMF::AudioPlayer::phononState(PrivateState state)
{
    const Phonon::State phononState =
        GroundState == state
        ?    Phonon::StoppedState
        :    static_cast<Phonon::State>(state);

    return phononState;
}

void MMF::AudioPlayer::changeState(PrivateState newState)
{
    TRACE_CONTEXT(AudioPlayer::changeState, EAudioInternal);
    TRACE_ENTRY("state %d newState %d", m_state, newState);

    // TODO: add some invariants to check that the transition is valid

    const Phonon::State currentPhononState = phononState(m_state);
    const Phonon::State newPhononState = phononState(newState);
    if(currentPhononState != newPhononState)
    {
        TRACE("emit stateChanged(%d, %d)", newPhononState, currentPhononState);
        emit stateChanged(newPhononState, currentPhononState);
    }

    m_state = newState;

    TRACE_EXIT_0();
}

void MMF::AudioPlayer::tick()
{
    TRACE_CONTEXT(AudioPlayer::tick, EAudioInternal);
    TRACE_ENTRY("state %d", m_state);

    emit tick(currentTime());

    TRACE_EXIT_0();
}




