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
#include "defs.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AbstractPlayer::AbstractPlayer()
						:	m_videoOutput(NULL)
						,	m_tickInterval(DefaultTickInterval)
						,	m_transitionTime(0)
						,	m_prefinishMark(0)
{

}

MMF::AbstractPlayer::AbstractPlayer(const AbstractPlayer& player)
						:	m_videoOutput(player.m_videoOutput)
						,	m_tickInterval(player.tickInterval())
						,	m_transitionTime(player.transitionTime())
						,	m_prefinishMark(player.prefinishMark())
{

}

//-----------------------------------------------------------------------------
// MediaObjectInterface
//-----------------------------------------------------------------------------

qint32 MMF::AbstractPlayer::tickInterval() const
{
    return m_tickInterval;
}

void MMF::AbstractPlayer::setTickInterval(qint32 interval)
{
    m_tickInterval = interval;
    doSetTickInterval(interval);
}

qint32 MMF::AbstractPlayer::prefinishMark() const
{
	return m_prefinishMark;
}

void MMF::AbstractPlayer::setPrefinishMark(qint32 mark)
{
	m_prefinishMark = mark;
}

qint32 MMF::AbstractPlayer::transitionTime() const
{
	return m_transitionTime;
}

void MMF::AbstractPlayer::setTransitionTime(qint32 time)
{
    m_transitionTime = time;
}


//-----------------------------------------------------------------------------
// Video output
//-----------------------------------------------------------------------------

void MMF::AbstractPlayer::setVideoOutput(VideoOutput* videoOutput)
{
    m_videoOutput = videoOutput;
    videoOutputChanged();
}

void MMF::AbstractPlayer::videoOutputChanged()
{
	// Default behaviour is empty - overridden by VideoPlayer
}


QT_END_NAMESPACE

