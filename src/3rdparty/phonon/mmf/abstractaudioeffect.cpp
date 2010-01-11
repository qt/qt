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

#include "mediaobject.h"

#include "abstractaudioeffect.h"
#include "audioplayer.h"
#include "mmf_videoplayer.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::AbstractAudioEffect
  \internal
*/

/*! \namespace Phonon::MMF
  \internal
*/

AbstractAudioEffect::AbstractAudioEffect(QObject *parent,
                                         const QList<EffectParameter> &params)
    :   MediaNode(parent)
    ,   m_params(params)
    ,   m_player(0)
{

}

QList<Phonon::EffectParameter> AbstractAudioEffect::parameters() const
{
    // Convert from QList<MMF::EffectParameter> to QList<Phonon::EffectParameter>
    QList<Phonon::EffectParameter> result;
    EffectParameter param;
    foreach (param, m_params)
        result += param;
    return result;
}

QVariant AbstractAudioEffect::parameterValue(const Phonon::EffectParameter &queriedParam) const
{
    const QVariant &val = m_values.value(queriedParam.id());

    if (val.isNull())
        return queriedParam.defaultValue();
    else
        return val;
}

void AbstractAudioEffect::setParameterValue(const Phonon::EffectParameter &param,
                                            const QVariant &newValue)
{
    m_values.insert(param.id(), newValue);
    parameterChanged(param.id(), newValue);

    if (m_effect.data())
        // TODO: handle audio effect errors
        TRAP_IGNORE(m_effect->ApplyL());
}

void AbstractAudioEffect::connectMediaObject(MediaObject *mediaObject)
{
    Q_ASSERT_X(!m_player, Q_FUNC_INFO, "Player already connected");
    Q_ASSERT_X(!m_effect.data(), Q_FUNC_INFO, "Effect already created");

    AbstractMediaPlayer *const player =
        qobject_cast<AbstractMediaPlayer *>(mediaObject->abstractPlayer());

    if (player) {
        m_player = player;

        if (AudioPlayer *audioPlayer = qobject_cast<AudioPlayer *>(player)) {
            connectAudioPlayer(audioPlayer->nativePlayer());
            applyParameters();
            // TODO: handle audio effect errors
            TRAP_IGNORE(m_effect->EnableL());
        }
    }
}

void AbstractAudioEffect::disconnectMediaObject(MediaObject * /*mediaObject*/)
{
    m_player = 0;
    m_effect.reset();
}

QT_END_NAMESPACE

