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
        const int level = value.toInt();
        setBandLevel(band, level);
    }
}

void AudioEqualizer::connectAudioPlayer(AudioPlayer::NativePlayer *player)
{
    CAudioEqualizer *ptr = 0;
    QT_TRAP_THROWING(ptr = CAudioEqualizer::NewL(*player));
    m_effect.reset(ptr);
}

void AudioEqualizer::applyParameters()
{
    if (m_effect.data()) {
        EffectParameter param;
        foreach (param, parameters()) {
            const int band = param.id();
            const int level = parameterValue(param).toInt();
            setBandLevel(band, level);
        }
    }
}

void AudioEqualizer::setBandLevel(int band, int level)
{
    CAudioEqualizer *const effect = static_cast<CAudioEqualizer *>(m_effect.data());
    // TODO: handle audio effect errors
    TRAP_IGNORE(effect->SetBandLevelL(band, level));
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

        const qint32 defVol = effect->BandLevel(i);

        parameters.append(EffectParameter(i,
                                      tr("%1 Hz").arg(hz),
                                      EffectParameter::LogarithmicHint,
                                      QVariant(qint32(defVol)),
                                      QVariant(qint32(dbMin)),
                                      QVariant(qint32(dbMax)),
                                      QVariantList(),
                                      QString()));
    }
}

QT_END_NAMESPACE
