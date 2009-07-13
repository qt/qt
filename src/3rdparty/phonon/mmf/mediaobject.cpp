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
#include <private/qcore_symbian_p.h>

#include "mediaobject.h"

using namespace Phonon;
using namespace Phonon::MMF;

MMF::MediaObject::MediaObject(QObject *parent) : m_player(0)
                                               , m_error(NoError)
                                               , m_state(StoppedState)
{
    Q_UNUSED(parent);
    m_player = CDrmPlayerUtility::NewL(*this, 0, EMdaPriorityPreferenceNone);

    m_player->RegisterForAudioLoadingNotification(*this);
}

MMF::MediaObject::~MediaObject()
{
    delete m_player;
}

void MMF::MediaObject::play()
{
    transitTo(PlayingState);
    m_player->Play();
}

void MMF::MediaObject::pause()
{
    transitTo(PausedState);
    m_player->Pause();
}

void MMF::MediaObject::stop()
{
    transitTo(StoppedState);
    m_player->Stop();
}

void MMF::MediaObject::seek(qint64 milliseconds)
{
    m_player->SetPosition(toMicroSeconds(milliseconds));
}

qint32 MMF::MediaObject::tickInterval() const
{
    return 0;
}

void MMF::MediaObject::setTickInterval(qint32 interval)
{
    Q_UNUSED(interval);
}

bool MMF::MediaObject::hasVideo() const
{
    return false;
}

bool MMF::MediaObject::isSeekable() const
{
    return false;
}

qint64 MMF::MediaObject::currentTime() const
{
    TTimeIntervalMicroSeconds mss;
    const TInt retcode = m_player->GetPosition(mss);

    if(retcode == KErrNone)
        return toMilliSeconds(m_player->Duration());
    else {
        // TODO Should we enter/emit error state? Tricky
        // since we're in a const function.
        return -1;
    }
}

QString MMF::MediaObject::errorString() const
{
    return QString();
}

Phonon::ErrorType MMF::MediaObject::errorType() const
{
    return m_error;
}

qint64 MMF::MediaObject::toMilliSeconds(const TTimeIntervalMicroSeconds &in)
{
    return in.Int64() / 1000;
}

TTimeIntervalMicroSeconds MMF::MediaObject::toMicroSeconds(qint64 ms)
{
    return TTimeIntervalMicroSeconds(TInt64(ms));
}

qint64 MMF::MediaObject::totalTime() const
{
    return toMilliSeconds(m_player->Duration());
}

MediaSource MMF::MediaObject::source() const
{
    return m_mediaSource;
}

void MMF::MediaObject::setSource(const MediaSource &source)
{
    stop();
    m_mediaSource = source;

    switch(m_mediaSource.type())
    {
        case MediaSource::LocalFile:
        {
            const QHBufC filename(source.fileName());
            m_player->OpenFileL(*filename);
            break;
        }
        case MediaSource::Url:
        {
            const QHBufC filename(source.url().toString());
            m_player->OpenUrlL(*filename);
            break;
        }
        case MediaSource::Invalid:
        /* Fallthrough. */
        case MediaSource::Disc:
        /* Fallthrough. */
        case MediaSource::Stream:
        /* Fallthrough. */
        case MediaSource::Empty:
        {
            transitTo(ErrorState);
            return;
        }
    }

    transitTo(LoadingState);
}

void MMF::MediaObject::setNextSource(const MediaSource &source)
{
    m_nextSource = source;
    Q_UNUSED(source);
}

qint32 MMF::MediaObject::prefinishMark() const
{
    return 0;
}

void MMF::MediaObject::setPrefinishMark(qint32)
{
}

qint32 MMF::MediaObject::transitionTime() const
{
    return 0;
}

void MMF::MediaObject::setTransitionTime(qint32)
{
}

void MMF::MediaObject::MaloLoadingComplete()
{
    transitTo(StoppedState);
}

void MMF::MediaObject::MaloLoadingStarted()
{
    transitTo(LoadingState);
}

void MMF::MediaObject::MdapcInitComplete(TInt aError,
                                         const TTimeIntervalMicroSeconds &)
{
    if(aError == KErrNone) {
        m_error = NormalError;
        m_state = ErrorState;
    }

    emit totalTimeChanged();
    transitTo(StoppedState);
}

void MMF::MediaObject::MdapcPlayComplete(TInt aError)
{
    if(aError == KErrNone) {
        if(m_nextSource.type() == MediaSource::Empty) {
            emit finished();
        } else {
            setSource(m_nextSource);
            m_nextSource = MediaSource();
        }

        transitTo(StoppedState);
    }
    else {
        m_error = NormalError;
        transitTo(ErrorState);
    }
}

void MMF::MediaObject::transitTo(Phonon::State newState)
{
    emit stateChanged(m_state, newState);
    m_state = newState;
}

Phonon::State MMF::MediaObject::state() const
{
    return m_state;
}

