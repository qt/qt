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

#include <stereowideningbase.h>
#include "stereowidening.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

// Define functions which depend on concrete native effect class name
PHONON_MMF_DEFINE_EFFECT_FUNCTIONS(StereoWidening)

/*! \class MMF::StereoWidening
  \internal
*/

StereoWidening::StereoWidening(QObject *parent, const QList<EffectParameter>& parameters)
    :   AbstractAudioEffect::AbstractAudioEffect(parent, parameters)
{

}

int StereoWidening::parameterChanged(const EffectParameter &param,
                                 const QVariant &value)
{
    Q_ASSERT_X(param.id() == ParameterBase, Q_FUNC_INFO, "Invalid parameter ID");

    const qreal externalLevel = value.toReal();
    const int internalLevel = param.toInternalValue(externalLevel);

    TRAPD(err, concreteEffect()->SetStereoWideningLevelL(internalLevel));

    return err;
}

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

const char* StereoWidening::description()
{
    return "Stereo widening";
}

bool StereoWidening::getParameters(CMdaAudioOutputStream *stream,
    QList<EffectParameter> &parameters)
{
    bool supported = false;

    QScopedPointer<CStereoWidening> effect;
    TRAPD(err, effect.reset(CStereoWidening::NewL(*stream)));

    if (KErrNone == err) {
        supported = true;

        const qreal defaultValue =
            Phonon::MMF::EffectParameter::toExternalValue
                (effect->StereoWideningLevel(), 0, 100);

        EffectParameter param(
             /* parameterId */        ParameterBase,
             /* name */               tr("Level (%)"),
             /* hints */              EffectParameter::IntegerHint,
             /* defaultValue */       QVariant(defaultValue),
             /* minimumValue */       QVariant(qreal(-1.0)),
             /* maximumValue */       QVariant(qreal(+1.0)));

        param.setInternalRange(0, 100);
        parameters.append(param);
    }

    return supported;
}

QT_END_NAMESPACE

