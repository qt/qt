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

#include "mediaobject.h"
#include "audiooutput.h"
#include "utils.h"
#include "volumecontrolinterface.h"

using namespace Phonon;
using namespace Phonon::MMF;


//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

static const qreal InitialVolume = 0.5;


//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioOutput::AudioOutput(Backend *, QObject *parent)	: QObject(parent)
															, m_volume(InitialVolume)
															, m_volumeControl(NULL)
{

}


//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------

qreal MMF::AudioOutput::volume() const
{
    TRACE_CONTEXT(AudioOutput::volume, EAudioApi);
    TRACE_ENTRY("control 0x%08x ", m_volumeControl);

    const qreal result = m_volumeControl ? m_volumeControl->volume() : m_volume;

    TRACE_RETURN("%f", result);
}

void MMF::AudioOutput::setVolume(qreal volume)
{
    TRACE_CONTEXT(AudioOutput::setVolume, EAudioApi);
    TRACE_ENTRY("control 0x%08x volume %f", m_volumeControl, volume);

    if(m_volumeControl)
    {
		if(m_volumeControl->setVolume(volume))
		{
			TRACE("emit volumeChanged(%f)", volume)
			emit volumeChanged(volume);
		}
    }
    else
    {
		m_volume = volume;
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

void MMF::AudioOutput::setVolumeControl(VolumeControlInterface *volumeControl)
{
    Q_ASSERT(!m_volumeControl);
    m_volumeControl = volumeControl;
    m_volumeControl->setVolume(m_volume);
}

