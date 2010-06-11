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

    if (m_effect.data()) {
        const EffectParameter& internalParam = internalParameter(param.id());
        int err = parameterChanged(internalParam, newValue);
        // TODO: handle audio effect errors
        Q_UNUSED(err);
    }
}

void AbstractAudioEffect::abstractPlayerChanged(AbstractPlayer *player)
{
    m_player = qobject_cast<AbstractMediaPlayer *>(player);
    m_effect.reset();
}

void AbstractAudioEffect::stateChanged(Phonon::State newState,
                                       Phonon::State oldState)
{
    if (Phonon::LoadingState == oldState
        && Phonon::LoadingState != newState)
        createEffect();
}

void AbstractAudioEffect::connectMediaObject(MediaObject *mediaObject)
{
    Q_ASSERT_X(!m_player, Q_FUNC_INFO, "Player already connected");
    Q_ASSERT_X(!m_effect.data(), Q_FUNC_INFO, "Effect already created");

    abstractPlayerChanged(mediaObject->abstractPlayer());

    connect(mediaObject, SIGNAL(stateChanged(Phonon::State, Phonon::State)),
            SLOT(stateChanged(Phonon::State, Phonon::State)));

    connect(mediaObject, SIGNAL(abstractPlayerChanged(AbstractPlayer *)),
            SLOT(abstractPlayerChanged(AbstractPlayer *)));

    if (mediaObject->state() != Phonon::LoadingState)
        createEffect();
}

void AbstractAudioEffect::disconnectMediaObject(MediaObject *mediaObject)
{
    mediaObject->disconnect(this);
    abstractPlayerChanged(0);
}

void AbstractAudioEffect::setEnabled(bool enabled)
{
    TInt err = KErrNone;

    if (enabled)
        // TODO: handle audio effect errors
        TRAP(err, m_effect->EnableL())
    else
        // TODO: handle audio effect errors
        TRAP(err, m_effect->DisableL())

    Q_UNUSED(err);
}

void AbstractAudioEffect::createEffect()
{
    Q_ASSERT_X(m_player, Q_FUNC_INFO, "Invalid media player pointer");

    if (AudioPlayer *audioPlayer = qobject_cast<AudioPlayer *>(m_player)) {
        createEffect(audioPlayer->nativePlayer());
    }

    if (m_effect.data()) {
        EffectParameter param;
	int err = 0;
        foreach (param, m_params) {
            const QVariant value = parameterValue(param);
            err = parameterChanged(param, value);
        }
	Q_UNUSED(err)
    }
}

const MMF::EffectParameter& AbstractAudioEffect::internalParameter(int id) const
{
    const EffectParameter *result = 0;
    for (int i=0; i<m_params.count() && !result; ++i) {
        if (m_params[i].id() == id)
            result = &m_params[i];
    }
    Q_ASSERT_X(result, Q_FUNC_INFO, "Parameter not found");
    return *result;
}

int AbstractAudioEffect::parameterChanged(const EffectParameter &param,
            const QVariant &value)
{
    int err = 0;

    switch (param.id()) {
    case ParameterEnable:
        setEnabled(value.toBool());
        break;
    default:
        {
        const EffectParameter& internalParam = internalParameter(param.id());
        err = effectParameterChanged(internalParam, value);
        }
        break;
    }

    if (!err)
        TRAP(err, m_effect->ApplyL());

    return err;
}

int AbstractAudioEffect::effectParameterChanged(
    const EffectParameter &param, const QVariant &value)
{
    // Default implementation
    Q_UNUSED(param)
    Q_UNUSED(value)
    Q_ASSERT_X(false, Q_FUNC_INFO, "Effect has no parameters");
    return 0;
}


QT_END_NAMESPACE

