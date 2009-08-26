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

AudioEqualizer::AudioEqualizer(QObject *parent) : AbstractAudioEffect::AbstractAudioEffect(parent, createParams())
{
}

void AudioEqualizer::parameterChanged(const int pid,
                                      const QVariant &value)
{
    // There is no way to return an error from this function, so we just
    // have to trap and ignore exceptions.
    TRAP_IGNORE(eq()->SetBandLevelL(pid, value.toInt()));
}

void AudioEqualizer::activateOn(CPlayerType *player)
{
    m_effect.reset(CAudioEqualizer::NewL(*player));
}

CAudioEqualizer *AudioEqualizer::eq() const
{
    return static_cast<CAudioEqualizer *>(m_effect.data());
}

QList<EffectParameter> AudioEqualizer::createParams()
{
    QList<EffectParameter> retval;

    // We temporarily create an AudioPlayer, and run the effect on it, so
    // we can extract the readonly data we need.
    AudioPlayer dummyPlayer;
    activateOn(dummyPlayer.player());

    TInt32 dbMin;
    TInt32 dbMax;
    eq()->DbLevelLimits(dbMin, dbMax);

    const int bandCount = eq()->NumberOfBands();

    for (int i = 0; i < bandCount; ++i) {
        const qint32 hz = eq()->CenterFrequency(i);

        const qint32 defVol = eq()->BandLevel(i);

        retval.append(EffectParameter(i,
                                      tr("Frequency band, %1 Hz").arg(hz),
                                      EffectParameter::LogarithmicHint,
                                      QVariant(qint32(defVol)),
                                      QVariant(qint32(dbMin)),
                                      QVariant(qint32(dbMax)),
                                      QVariantList(),
                                      QString()));
    }

    m_effect.reset();

    return retval;
}

QT_END_NAMESPACE

