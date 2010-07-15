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

#include <audioequalizerbase.h>
#include "audioequalizer.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AudioEqualizer
  \internal
*/

// Define functions which depend on concrete native effect class name
PHONON_MMF_DEFINE_EFFECT_FUNCTIONS(AudioEqualizer)

AudioEqualizer::AudioEqualizer(QObject *parent, const QList<EffectParameter>& parameters)
    :   AbstractAudioEffect::AbstractAudioEffect(parent, parameters)
{

}

int AudioEqualizer::effectParameterChanged(const EffectParameter &param,
                                      const QVariant &value)
{
    const int band = param.id() - ParameterBase + 1;

    const qreal externalLevel = value.toReal();
    const int internalLevel = param.toInternalValue(externalLevel);

    TRAPD(err, concreteEffect()->SetBandLevelL(band, internalLevel));
    return err;
}


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

const char* AudioEqualizer::description()
{
    return "Audio equalizer";
}

bool AudioEqualizer::getParameters(CMdaAudioOutputStream *stream,
    QList<EffectParameter>& parameters)
{
    bool supported = false;

    QScopedPointer<CAudioEqualizer> effect;
    TRAPD(err, effect.reset(CAudioEqualizer::NewL(*stream)));

    if (KErrNone == err) {
        supported = true;

        TInt32 dbMin;
        TInt32 dbMax;
        effect->DbLevelLimits(dbMin, dbMax);

        const int bandCount = effect->NumberOfBands();

        for (int i = 0; i < bandCount; ++i) {
            // For some reason, band IDs are 1-based, as opposed to the
            // 0-based indices used in just about other Symbian API...!
            const int band = i + 1;

            const qint32 hz = effect->CenterFrequency(band);

            // We pass a floating-point parameter range of -1.0 to +1.0 for
            // each band in order to work around a limitation in
            // Phonon::EffectWidget.  See documentation of EffectParameter
            // for more details.
            EffectParameter param(
                 /* parameterId */        ParameterBase + i,
                 /* name */               tr("%1 Hz").arg(hz),
                 /* hints */              EffectParameter::LogarithmicHint,
                 /* defaultValue */       QVariant(qreal(0.0)),
                 /* minimumValue */       QVariant(qreal(-1.0)),
                 /* maximumValue */       QVariant(qreal(+1.0)));

            param.setInternalRange(dbMin, dbMax);
            parameters.append(param);
        }
    }

    return supported;
}

QT_END_NAMESPACE
