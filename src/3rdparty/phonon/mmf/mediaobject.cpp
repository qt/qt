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

#include "abstractplayer.h"
#include "audioplayer.h"
#include "mmf_videoplayer.h"
#include "mediaobject.h"

using namespace Phonon;
using namespace Phonon::MMF;

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::MediaObject::MediaObject(QObject *parent) : QObject::QObject(parent)
{
}

MMF::MediaObject::~MediaObject()
{
}

//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

void MMF::MediaObject::play()
{
    m_player->play();
}

void MMF::MediaObject::pause()
{
    m_player->pause();
}

void MMF::MediaObject::stop()
{
    m_player->stop();
}

void MMF::MediaObject::seek(qint64 ms)
{
    m_player->seek(ms);
}

qint32 MMF::MediaObject::tickInterval() const
{
    return m_player->tickInterval();
}

void MMF::MediaObject::setTickInterval(qint32 interval)
{
    m_player->setTickInterval(interval);
}

bool MMF::MediaObject::hasVideo() const
{
    return m_player->hasVideo();
}

bool MMF::MediaObject::isSeekable() const
{
    return m_player->isSeekable();
}

Phonon::State MMF::MediaObject::state() const
{
    return m_player->state();
}

qint64 MMF::MediaObject::currentTime() const
{
    return m_player->currentTime();
}

QString MMF::MediaObject::errorString() const
{
    return m_player->errorString();
}

Phonon::ErrorType MMF::MediaObject::errorType() const
{
    return m_player->errorType();
}

qint64 MMF::MediaObject::totalTime() const
{
    return m_player->totalTime();
}

MediaSource MMF::MediaObject::source() const
{
    return m_player->source();
}

void MMF::MediaObject::setSource(const MediaSource &source)
{
    loadPlayer(source);

    return m_player->setSource(source);
}

void MMF::MediaObject::loadPlayer(const MediaSource &source)
{
    disconnect(m_player.data(), 0, this, 0);

    // TODO determine media type
    m_player.reset(new AudioPlayer());

    connect(m_player.data(), SIGNAL(totalTimeChanged()), SIGNAL(totalTimeChanged()));
    connect(m_player.data(), SIGNAL(stateChanged(Phonon::State, Phonon::State)), SIGNAL(stateChanged(Phonon::State, Phonon::State)));
    connect(m_player.data(), SIGNAL(finished()), SIGNAL(finished()));
    connect(m_player.data(), SIGNAL(tick(qint64)), SIGNAL(tick(qint64)));
}

void MMF::MediaObject::setNextSource(const MediaSource &source)
{
    return m_player->setNextSource(source);
}

qint32 MMF::MediaObject::prefinishMark() const
{
    return m_player->prefinishMark();
}

void MMF::MediaObject::setPrefinishMark(qint32 mark)
{
    m_player->setPrefinishMark(mark);
}

qint32 MMF::MediaObject::transitionTime() const
{
    return m_player->transitionTime();
}

void MMF::MediaObject::setTransitionTime(qint32 time)
{
    m_player->setTransitionTime(time);
}

//-----------------------------------------------------------------------------
// Volume
//-----------------------------------------------------------------------------

qreal MMF::MediaObject::volume() const
{
    return m_player->volume();
}

bool MMF::MediaObject::setVolume(qreal volume)
{
    return m_player->setVolume(volume);
}

void MMF::MediaObject::setAudioOutput(AudioOutput* audioOutput)
{
    m_player->setAudioOutput(audioOutput);
}

