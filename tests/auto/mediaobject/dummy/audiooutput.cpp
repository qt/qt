#include "audiooutput.h"
#include "backend.h"
#include <phonon/audiooutput.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Dummy
{
AudioOutput::AudioOutput(Backend *backend, QObject *parent)
        : QObject(parent)
{
    Q_UNUSED(backend)

    m_volumeLevel = 100;
}

AudioOutput::~AudioOutput()
{
}

qreal AudioOutput::volume() const
{
    return m_volumeLevel;
}

int AudioOutput::outputDevice() const
{
    return m_device;
}

void AudioOutput::setVolume(qreal newVolume)
{
    m_volumeLevel = newVolume;
    emit volumeChanged(newVolume);
}

bool AudioOutput::setOutputDevice(int newDevice)
{
    return (newDevice == 0);
}

bool AudioOutput::setOutputDevice(const AudioOutputDevice &newDevice)
{
    return setOutputDevice(newDevice.index());
}

}
} //namespace Phonon::Dummy

QT_END_NAMESPACE
#include "moc_audiooutput.cpp"
