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
}

void MMF::MediaObject::pause()
{
}

void MMF::MediaObject::stop()
{
}

void MMF::MediaObject::seek(qint64 ms)
{
    Q_UNUSED(ms);
}

qint32 MMF::MediaObject::tickInterval() const
{
}

void MMF::MediaObject::setTickInterval(qint32 interval)
{
}

bool MMF::MediaObject::hasVideo() const
{
}

bool MMF::MediaObject::isSeekable() const
{
}

Phonon::State MMF::MediaObject::state() const
{
}

qint64 MMF::MediaObject::currentTime() const
{
}

QString MMF::MediaObject::errorString() const
{
}

Phonon::ErrorType MMF::MediaObject::errorType() const
{
}

qint64 MMF::MediaObject::totalTime() const
{
}

MediaSource MMF::MediaObject::source() const
{
}

void MMF::MediaObject::setSource(const MediaSource &source)
{
}

void MMF::MediaObject::setNextSource(const MediaSource &source)
{
}

qint32 MMF::MediaObject::prefinishMark() const
{
}

void MMF::MediaObject::setPrefinishMark(qint32 mark)
{
}

qint32 MMF::MediaObject::transitionTime() const
{
}

void MMF::MediaObject::setTransitionTime(qint32 time)
{
}

//-----------------------------------------------------------------------------
// Volume
//-----------------------------------------------------------------------------

qreal MMF::MediaObject::volume() const
{
}

bool MMF::MediaObject::setVolume(qreal volume)
{
}

void MMF::MediaObject::setAudioOutput(AudioOutput* audioOutput)
{
}

