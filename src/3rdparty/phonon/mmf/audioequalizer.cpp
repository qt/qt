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

#include <AudioEqualizerBase.h>
#include "audioequalizer.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AudioEqualizer
  \internal
*/

AudioEqualizer::AudioEqualizer(QObject *parent, const QList<EffectParameter> &parameters)
    :   AbstractAudioEffect::AbstractAudioEffect(parent, parameters)
{

}

void AudioEqualizer::parameterChanged(const int pid,
                                      const QVariant &value)
{
    if (m_effect.data()) {
        const int band = pid;
        const qreal level = value.toReal();
        setBandLevel(band, level);
    }
}

void AudioEqualizer::createEffect(AudioPlayer::NativePlayer *player)
{
    CAudioEqualizer *ptr = 0;
    QT_TRAP_THROWING(ptr = CAudioEqualizer::NewL(*player));
    m_effect.reset(ptr);
}

void AudioEqualizer::applyParameters()
{
    if (m_effect.data()) {
	Phonon::EffectParameter param;
        foreach (param, parameters()) {
            const int band = param.id();
            const qreal level = parameterValue(param).toReal();
            setBandLevel(band, level);
        }
    }
}

void AudioEqualizer::setBandLevel(int band, qreal externalLevel)
{
    const EffectParameter &param = m_params[band-1]; // Band IDs are 1-based
    const int internalLevel = param.toInternalValue(externalLevel);

    CAudioEqualizer *const effect = static_cast<CAudioEqualizer *>(m_effect.data());
    // TODO: handle audio effect errors
    TRAP_IGNORE(effect->SetBandLevelL(band, internalLevel));
}

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

const char* AudioEqualizer::description()
{
    return "Audio equalizer";
}

void AudioEqualizer::getParameters(NativeEffect *effect,
    QList<EffectParameter> &parameters)
{
    TInt32 dbMin;
    TInt32 dbMax;
    effect->DbLevelLimits(dbMin, dbMax);

    const int bandCount = effect->NumberOfBands();

    // For some reason, band IDs are 1-based, as opposed to the
    // 0-based indices used in just about other Symbian API...!
    for (int i = 1; i <= bandCount; ++i) {
        const qint32 hz = effect->CenterFrequency(i);

        // We pass a floating-point parameter range of -1.0 to +1.0 for
        // each band in order to work around a limitation in
        // Phonon::EffectWidget.  See documentation of EffectParameter
        // for more details.
        EffectParameter param(
             /* parameterId */        i,
             /* name */               tr("%1 Hz").arg(hz),
             /* hints */              EffectParameter::LogarithmicHint,
             /* defaultValue */       QVariant(qreal(0.0)),
             /* minimumValue */       QVariant(qreal(-1.0)),
             /* maximumValue */       QVariant(qreal(+1.0)),
             /* values */             QVariantList(),
             /* description */        QString());

        param.setInternalRange(dbMin, dbMax);
        parameters.append(param);
    }
}

QT_END_NAMESPACE
