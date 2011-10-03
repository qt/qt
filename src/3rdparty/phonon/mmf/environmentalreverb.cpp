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

#include <EnvironmentalReverbBase.h>
#include "environmentalreverb.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::EnvironmentalReverb
  \internal
*/

// Define functions which depend on concrete native effect class name
PHONON_MMF_DEFINE_EFFECT_FUNCTIONS(EnvironmentalReverb)

enum Parameters
{
    DecayHFRatio = AbstractAudioEffect::ParameterBase,
    DecayTime,
    Density,
    Diffusion,
    ReflectionsDelay,
    ReflectionsLevel,
    ReverbDelay,
    ReverbLevel,
    RoomHFLevel,
    RoomLevel
};

EnvironmentalReverb::EnvironmentalReverb(QObject *parent, const QList<EffectParameter>& parameters)
    :   AbstractAudioEffect::AbstractAudioEffect(parent, parameters)
{

}

int EnvironmentalReverb::effectParameterChanged(const EffectParameter &param,
                                      const QVariant &value)
{
    const qreal externalLevel = value.toReal();
    const int internalLevel = param.toInternalValue(externalLevel);

    TInt err = 0;

    switch(param.id()) {
    case DecayHFRatio:
        TRAP(err, concreteEffect()->SetDecayHFRatioL(internalLevel));
        break;
    case DecayTime:
        TRAP(err, concreteEffect()->SetDecayTimeL(internalLevel));
        break;
    case Density:
        TRAP(err, concreteEffect()->SetDensityL(internalLevel));
        break;
    case Diffusion:
        TRAP(err, concreteEffect()->SetDiffusionL(internalLevel));
        break;
    case ReflectionsDelay:
        TRAP(err, concreteEffect()->SetReflectionsDelayL(internalLevel));
        break;
    case ReflectionsLevel:
        TRAP(err, concreteEffect()->SetReflectionsLevelL(internalLevel));
        break;
    case ReverbDelay:
        TRAP(err, concreteEffect()->SetReverbDelayL(internalLevel));
        break;
    case ReverbLevel:
        TRAP(err, concreteEffect()->SetReverbLevelL(internalLevel));
        break;
    case RoomHFLevel:
        TRAP(err, concreteEffect()->SetRoomHFLevelL(internalLevel));
        break;
    case RoomLevel:
        TRAP(err, concreteEffect()->SetRoomLevelL(internalLevel));
        break;
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown parameter");
    }

    return err;
}


//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

const char* EnvironmentalReverb::description()
{
    return "Reverb";
}

// Internal helper function
Phonon::MMF::EffectParameter createParameter(int id, const QString &name,
    int defaultValue, int minValue, int maxValue,
    Phonon::EffectParameter::Hint hint = Phonon::EffectParameter::IntegerHint)
{
    const qreal externalDefaultValue =
        Phonon::MMF::EffectParameter::toExternalValue
            (defaultValue, minValue, maxValue);

    Phonon::MMF::EffectParameter param(id, name, hint,
        /* defaultValue */       QVariant(externalDefaultValue),
        /* minimumValue */       QVariant(qreal(-1.0)),
        /* maximumValue */       QVariant(qreal(+1.0)));

    param.setInternalRange(minValue, maxValue);
    return param;
}

bool EnvironmentalReverb::getParameters(CMdaAudioOutputStream *stream,
    QList<EffectParameter>& parameters)
{
    bool supported = false;

    QScopedPointer<CEnvironmentalReverb> effect;
    TRAPD(err, effect.reset(CEnvironmentalReverb::NewL(*stream)));

    if (KErrNone == err) {
        supported = true;

        TInt32 min, max;
        TUint32 umin, umax;

        effect->DecayHFRatioRange(umin, umax);
        //: DecayHFRatio: Ratio of high-frequency decay time to the value specified by
        //: DecayTime.
        parameters.append(createParameter(
            DecayHFRatio, tr("Decay HF ratio (%)"), effect->DecayHFRatio(),
            umin, umax));

        effect->DecayTimeRange(umin, umax);
        //: DecayTime: Time over which reverberation is diminished.
        parameters.append(createParameter(
            DecayTime, tr("Decay time (ms)"), effect->DecayTime(),
            umin, umax));

        //: Density Delay between first and subsequent reflections.
        //: Note that the S60 platform documentation does not make clear
        //: the distinction between this value and the Diffusion value.
        parameters.append(createParameter(
            Density, tr("Density (%)"), effect->Density(), 0, 100));

        //: Diffusion: Delay between first and subsequent reflections.
        //: Note that the S60 platform documentation does not make clear
        //: the distinction between this value and the Density value.
        parameters.append(createParameter(
            Diffusion, tr("Diffusion (%)"), effect->Diffusion(), 0, 100));

        //: ReflectionsDelay: Amount of delay between the arrival of the direct
        //: path from the source and the arrival of the first reflection.
        parameters.append(createParameter(
            ReflectionsDelay, tr("Reflections delay (ms)"),
            effect->ReflectionsDelay(), 0, effect->ReflectionsDelayMax()));

        effect->ReflectionLevelRange(min, max);
        //: ReflectionsLevel: Amplitude of reflections. This value is
        //: corrected by the RoomLevel to give the final reflection amplitude.
        parameters.append(createParameter(
            ReflectionsLevel, tr("Reflections level (mB)"),
            effect->ReflectionsLevel(),
            min, max, EffectParameter::LogarithmicHint));

        //: ReverbDelay: Amount of time between arrival of the first
        //: reflection and start of the late reverberation.
        parameters.append(createParameter(
            ReverbDelay, tr("Reverb delay (ms)"), effect->ReverbDelay(),
            0, effect->ReverbDelayMax()));

        effect->ReverbLevelRange(min, max);
        //: ReverbLevel: Amplitude of reverberations.  This value is
        //: corrected by the RoomLevel to give the final reverberation
        //: amplitude.
        parameters.append(createParameter(
            ReverbLevel, tr("Reverb level (mB)"), effect->ReverbLevel(),
            min, max, EffectParameter::LogarithmicHint));

        effect->RoomHFLevelRange(min, max);
        //: RoomHFLevel: Amplitude of low-pass filter used to attenuate the
        //: high frequency component of reflected sound.
        parameters.append(createParameter(
            RoomHFLevel, tr("Room HF level"), effect->RoomHFLevel(),
            min, max));

        effect->RoomLevelRange(min, max);
        //: RoomLevel: Master volume control for all reflected sound.
        parameters.append(createParameter(
            RoomLevel, tr("Room level (mB)"), effect->RoomLevel(),
            min, max, EffectParameter::LogarithmicHint));
    }

    return supported;
}

QT_END_NAMESPACE
