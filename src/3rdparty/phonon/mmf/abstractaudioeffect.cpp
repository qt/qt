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

#include <AudioEffectBase.h>
#include "mediaobject.h"

#include "abstractaudioeffect.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

AbstractAudioEffect::AbstractAudioEffect(QObject *parent,
                                         const QList<EffectParameter> &params) : MediaNode::MediaNode(parent)
                                                                               , m_params(params)
                                                                               , m_isApplied(false)
{
}

bool AbstractAudioEffect::disconnectMediaNode(MediaNode *target)
{
    MediaNode::disconnectMediaNode(target);
    m_effect.reset();
    return true;
}

QList<EffectParameter> AbstractAudioEffect::parameters() const
{
    return m_params;
}

QVariant AbstractAudioEffect::parameterValue(const EffectParameter &queriedParam) const
{
    const QVariant &val = m_values.value(queriedParam.id());

    if (val.isNull())
        return queriedParam.defaultValue();
    else
        return val;
}

void AbstractAudioEffect::setParameterValue(const EffectParameter &param,
                                            const QVariant &newValue)
{
    m_values.insert(param.id(), newValue);
    parameterChanged(param.id(), newValue);
}

bool AbstractAudioEffect::activateBackwardsInChain(MediaNode *target)
{
    // TODO we need to walk forward too.
    MediaNode *current = target;

    while (current) {
        MMF::MediaObject *const mo = qobject_cast<MMF::MediaObject *>(current);
        if(!mo)
            continue;

        AudioPlayer *const ap = qobject_cast<AudioPlayer *>(mo->abstractPlayer());

        if (ap) {
            activateOn(ap->player());
            // There might be stuff before the mediaobject, but
            // that's undefined for us.
            return true;
        }
        else
            current = current->source();
    }

    return false;
}

bool AbstractAudioEffect::connectMediaNode(MediaNode *target)
{
    /**
     * We first call this function, so source() and target()
     * is properly set up.
     */
    MediaNode::connectMediaNode(target);

    if (!m_isApplied && activateBackwardsInChain(target))
        m_isApplied = true;

    return true;
}

QT_END_NAMESPACE

