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

#include "mediaobject.h"

using namespace Phonon;
using namespace Phonon::MMF;

MediaObject::MediaObject(QObject *parent) : m_player(0)
                                          , m_error(NoError)
                                          , m_state(StoppedState)
{
    Q_UNUSED(parent);
    m_player = CDrmPlayerUtility::NewL(*this, 0, EMdaPriorityPreferenceNone);
}

MediaObject::~MediaObject()
{
    delete m_player;
}

void MediaObject::play()
{
    m_state = PlayingState;
    m_player->Play();
}

void MediaObject::pause()
{
    m_state = PausedState;
    m_player->Pause();
}

void MediaObject::stop()
{
    m_state = StoppedState;
    m_player->Stop();
}

void MediaObject::seek(qint64 milliseconds)
{
    m_player->SetPosition(toMicroSeconds(milliseconds));
}

qint32 MediaObject::tickInterval() const
{
    return 0;
}

void MediaObject::setTickInterval(qint32 interval)
{
    Q_UNUSED(interval);
}

bool MediaObject::hasVideo() const
{
    return false;
}

bool MediaObject::isSeekable() const
{
    return false;
}

qint64 MediaObject::currentTime() const
{
    TTimeIntervalMicroSeconds mss;
    const TInt retcode = m_player->GetPosition(mss);

    if(retcode == KErrNone)
        return toMilliSeconds(m_player->Duration());
    else {
        m_state = ErrorState;
        m_error = NormalError;
        return -1;
    }
}

Phonon::State MediaObject::state() const
{
    return m_state;
}

QString MediaObject::errorString() const
{
    return QString();
}

Phonon::ErrorType MediaObject::errorType() const
{
    return m_error;
}

qint64 MediaObject::toMilliSeconds(const TTimeIntervalMicroSeconds &in)
{
    return in.Int64() / 1000;
}

TTimeIntervalMicroSeconds MediaObject::toMicroSeconds(qint64 ms)
{
    return TTimeIntervalMicroSeconds(TInt64(ms));
}

qint64 MediaObject::totalTime() const
{
    return toMilliSeconds(m_player->Duration());
}

MediaSource MediaObject::source() const
{
    return MediaSource();
}

void MediaObject::setSource(const MediaSource &source)
{
    stop();
    m_state = LoadingState;

    Q_UNUSED(source);
    // TODO

    emit totalTimeChanged();
}

void MediaObject::setNextSource(const MediaSource &source)
{
    Q_UNUSED(source);
}

qint32 MediaObject::prefinishMark() const
{
    return 0;
}

void MediaObject::setPrefinishMark(qint32)
{
}

qint32 MediaObject::transitionTime() const
{
    return 0;
}

void MediaObject::setTransitionTime(qint32)
{
}

void MediaObject::MaloLoadingComplete()
{
    m_state = StoppedState;
}

void MediaObject::MaloLoadingStarted()
{
    m_state = LoadingState;
}

void MediaObject::MdapcInitComplete(TInt aError,
                                    const TTimeIntervalMicroSeconds &aDuration)
{
    Q_UNUSED(aError);
    Q_UNUSED(aDuration);
}

void MediaObject::MdapcPlayComplete(TInt aError)
{
    Q_UNUSED(aError);
}

