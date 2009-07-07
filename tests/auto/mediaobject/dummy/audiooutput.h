#ifndef PHONON_DUMMY_AUDIOOUTPUT_H
#define PHONON_DUMMY_AUDIOOUTPUT_H

#include "backend.h"
#include <phonon/audiooutputinterface.h>
#include <phonon/phononnamespace.h>

QT_BEGIN_NAMESPACE

namespace Phonon
{
namespace Dummy
{
class AudioOutput : public QObject, public AudioOutputInterface
{
    Q_OBJECT
    Q_INTERFACES(Phonon::AudioOutputInterface)
public:
    AudioOutput(Backend *backend, QObject *parent);
    ~AudioOutput();

    qreal volume() const;
    int outputDevice() const;
    void setVolume(qreal newVolume);
    bool setOutputDevice(int newDevice);
    bool setOutputDevice(const AudioOutputDevice &newDevice);

Q_SIGNALS:
    void volumeChanged(qreal newVolume);
    void audioDeviceFailed();

private:
    qreal m_volumeLevel;
    int m_device;
};
}
} //namespace Phonon::Dummy

QT_END_NAMESPACE

#endif // PHONON_DUMMY_AUDIOOUTPUT_H
