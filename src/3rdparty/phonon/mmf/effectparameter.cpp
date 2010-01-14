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

#include "effectparameter.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::EffectParameter
  \internal
*/

MMF::EffectParameter::EffectParameter()
    :   m_hasInternalRange(false)
{

}

MMF::EffectParameter::EffectParameter(
            int parameterId, const QString &name, Hints hints,
            const QVariant &defaultValue, const QVariant &min,
            const QVariant &max, const QVariantList &values,
            const QString &description)
    :   Phonon::EffectParameter(parameterId, name, hints, defaultValue,
            min, max, values, description)
    ,   m_hasInternalRange(false)
{

}

void MMF::EffectParameter::setInternalRange(qint32 min, qint32 max)
{
    Q_ASSERT_X(max >= min, Q_FUNC_INFO, "Invalid range");
    m_internalRange = QPair<qint32, qint32>(min, max);
    m_hasInternalRange = true;
}

qint32 MMF::EffectParameter::toInternalValue(qreal external) const
{
    Q_ASSERT_X(m_hasInternalRange, Q_FUNC_INFO, "Does not have internal range");
    const qint32 range = m_internalRange.second - m_internalRange.first;
    return m_internalRange.first + ((1.0 + external) / 2) * range;
}

qreal MMF::EffectParameter::toExternalValue
    (qint32 value, qint32 min, qint32 max)
{
    Q_ASSERT_X(max >= min, Q_FUNC_INFO, "Invalid range");
    const qint32 range = max - min;
    return range == 0 ? 0.0 : ((2.0 * value - min) / range) - 1.0;
}

QT_END_NAMESPACE

