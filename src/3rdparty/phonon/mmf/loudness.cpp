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

#include <LoudnessBase.h>
#include "loudness.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

// Define functions which depend on concrete native effect class name
PHONON_MMF_DEFINE_EFFECT_FUNCTIONS(Loudness)

/*! \class MMF::Loudness
  \internal
*/

Loudness::Loudness(QObject *parent, const QList<EffectParameter>& parameters)
    :   AbstractAudioEffect::AbstractAudioEffect(parent, parameters)
{

}

//-----------------------------------------------------------------------------
// Static functions
//-----------------------------------------------------------------------------

const char* Loudness::description()
{
    return "Loudness";
}

bool Loudness::getParameters(CMdaAudioOutputStream *stream,
    QList<EffectParameter> &parameters)
{
    Q_UNUSED(parameters)
    QScopedPointer<CLoudness> effect;
    TRAPD(err, effect.reset(CLoudness::NewL(*stream)));
    return (KErrNone == err);
}

QT_END_NAMESPACE

