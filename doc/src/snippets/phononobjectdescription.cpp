#include <QtGui>

#include <phonon/phononnamespace.h>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
#include <phonon/effect.h>
#include <phonon/effectparameter.h>
#include <phonon/objectdescriptionmodel.h>
#include <QList>

int main(int argv, char **args)
{
    QApplication app(argv, args);
    
//![0]
    QList<Phonon::EffectDescription> effectDescriptions =
            Phonon::BackendCapabilities::availableAudioEffects();

//![1]
    QList<Phonon::AudioOutputDevice> audioOutputDevices =
            Phonon::BackendCapabilities::availableAudioOutputDevices();

//![1]
    foreach (Phonon::EffectDescription effectDescription, effectDescriptions) {
        Phonon::Effect *effect = new Phonon::Effect(effectDescription);

        // ... Do something with the effect, like insert it into a media graph
    }

    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput;

    audioOutput->setOutputDevice(audioOutputDevices[0]);
//![0]

    return app.exec();

}
