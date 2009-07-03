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

#include <DrmAudioSamplePlayer.h>

#include "mediaobject.h"
#include "audiooutput.h"

using namespace Phonon;
using namespace Phonon::MMF;

AudioOutput::AudioOutput(Backend *, QObject *parent) : m_mediaObject(0)
                                                     , m_volume(0)
                                                     , m_maxVolume(-1)
{
    setParent(parent);
}

qreal AudioOutput::volume() const
{
    return 0;
}

void AudioOutput::setVolume(qreal newVolume)
{
    if(!m_mediaObject)
        return;

    Q_ASSERT(m_mediaObject->m_player);

    if (newVolume == m_volume)
        return;

    m_volume = newVolume;
    m_mediaObject->m_player->SetVolume(newVolume * m_maxVolume);
    emit volumeChanged(m_volume);
}

int AudioOutput::outputDevice() const
{
    return 0;
}

bool AudioOutput::setOutputDevice(int)
{
    return true;
}

bool AudioOutput::setOutputDevice(const Phonon::AudioOutputDevice &)
{
    return true;
}

void AudioOutput::setMediaObject(MediaObject *mo)
{
    Q_ASSERT(m_mediaObject);
    m_mediaObject = mo;

    Q_ASSERT(m_mediaObject->m_player);
    m_maxVolume = m_mediaObject->m_player->MaxVolume();

    TInt mmfVolume = 0;
    const TInt errorCode = m_mediaObject->m_player->GetVolume(mmfVolume);

    if(errorCode == KErrNone)
        return;

    m_volume = mmfVolume / m_maxVolume;
    emit volumeChanged(m_volume);
}

