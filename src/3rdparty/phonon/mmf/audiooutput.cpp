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

#include <e32debug.h>

#include "audiooutput.h"
#include "defs.h"
#include "utils.h"
#include "volumeobserver.h"

using namespace Phonon;
using namespace Phonon::MMF;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioOutput::AudioOutput(Backend *, QObject *parent)	: QObject(parent)
															, m_volume(InitialVolume)
															, m_observer(NULL)
{

}


//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

qreal MMF::AudioOutput::volume() const
{
	return m_volume;
}

void MMF::AudioOutput::setVolume(qreal volume)
{
    TRACE_CONTEXT(AudioOutput::setVolume, EAudioApi);
    TRACE_ENTRY("observer 0x%08x volume %f", m_observer, volume);

    if(volume != m_volume)
    {
		if(m_observer)
		{
			m_observer->volumeChanged(volume);
		}
		
		m_volume = volume;
		TRACE("emit volumeChanged(%f)", volume)
		emit volumeChanged(volume);
    }
    
    TRACE_EXIT_0();
}

int MMF::AudioOutput::outputDevice() const
{
    return 0;
}

bool MMF::AudioOutput::setOutputDevice(int)
{
    return true;
}

bool MMF::AudioOutput::setOutputDevice(const Phonon::AudioOutputDevice &)
{
    return true;
}

void MMF::AudioOutput::setVolumeObserver(VolumeObserver& observer)
{
    Q_ASSERT(!m_observer);
    m_observer = &observer;
    m_observer->volumeChanged(m_volume);
}

