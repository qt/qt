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

#include <QCoreApplication>

#include "audiooutput.h"
#include "defs.h"
#include "mediaobject.h"
#include "utils.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AudioOutput
  \internal
*/

//-----------------------------------------------------------------------------
// Constructor / destructor
//-----------------------------------------------------------------------------

MMF::AudioOutput::AudioOutput(Backend *, QObject *parent) : MediaNode(parent)
        , m_volume(InitialVolume)
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
    TRACE_ENTRY("volume %f", volume);

    if (volume != m_volume) {

        m_volume = volume;
        TRACE("emit volumeChanged(%f)", volume)
        emit volumeChanged(volume);
    }

    TRACE_EXIT_0();
}

int MMF::AudioOutput::outputDevice() const
{
    return AudioOutputDeviceID;
}

bool MMF::AudioOutput::setOutputDevice(int index)
{
    Q_ASSERT_X(index == AudioOutputDeviceID, Q_FUNC_INFO,
               "We only support one output device, with id 0");
#ifdef QT_NO_DEBUG
    Q_UNUSED(index)
#endif
    return true;
}

void MMF::AudioOutput::connectMediaObject(MediaObject *mediaObject)
{
    // Ensure that the MediaObject has the correct initial volume
    mediaObject->volumeChanged(m_volume);
    // Connect MediaObject to receive future volume changes
    connect(this, SIGNAL(volumeChanged(qreal)), mediaObject, SLOT(volumeChanged(qreal)));
}

void MMF::AudioOutput::disconnectMediaObject(MediaObject *mediaObject)
{
    // Disconnect all signal-slot connections
    disconnect(this, 0, mediaObject, 0);
}

QHash<QByteArray, QVariant> MMF::AudioOutput::audioOutputDescription(int index)
{
    QHash<QByteArray, QVariant> retval;

    if (index == AudioOutputDeviceID) {
        retval.insert("name", QCoreApplication::translate("Phonon::MMF", "Audio Output"));
        retval.insert("description", QCoreApplication::translate("Phonon::MMF", "The audio output device"));
        retval.insert("available", true);
    }

    return retval;
}

QT_END_NAMESPACE
