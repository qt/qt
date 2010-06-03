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

#include <QObject>
#include <QCoreApplication>

#include <mdaaudiooutputstream.h>

#include "audioequalizer.h"
#include "bassboost.h"
#include "environmentalreverb.h"
#include "loudness.h"
#include "stereowidening.h"

#include "effectfactory.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

/*! \class MMF::EffectFactory
  \internal
*/

EffectFactory::EffectFactory(QObject *parent)
    :   QObject(parent)
    ,   m_initialized(false)
{

}

EffectFactory::~EffectFactory()
{

}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

AbstractAudioEffect *EffectFactory::createAudioEffect(Type type,
                                                      QObject *parent)
{
    // Lazily initialize
    if (!m_initialized)
        initialize();

    Q_ASSERT(parent);

    const QList<EffectParameter>& parameters = data(type).m_parameters;

    AbstractAudioEffect *effect = 0;

    switch (type)
    {
    case TypeBassBoost:
        effect = new BassBoost(parent, parameters);
        break;
    case TypeAudioEqualizer:
        effect = new AudioEqualizer(parent, parameters);
        break;
    case TypeEnvironmentalReverb:
        effect = new EnvironmentalReverb(parent, parameters);
        break;
    case TypeLoudness:
        effect = new Loudness(parent, parameters);
        break;
    case TypeStereoWidening:
        effect = new StereoWidening(parent, parameters);
        break;

    // Not implemented
    case TypeDistanceAttenuation:
    case TypeListenerOrientation:
    case TypeSourceOrientation:
    // Fall through
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Unknown effect");
    }

    return effect;
}

QHash<QByteArray, QVariant> EffectFactory::audioEffectDescriptions(Type type)
{
    // Lazily initialize
    if (!m_initialized)
        initialize();

    return data(type).m_descriptions;
}

QList<int> EffectFactory::effectIndexes()
{
    // Lazily initialize
    if (!m_initialized)
        initialize();

    QList<int> result;

    QHash<Type, EffectData>::const_iterator i = m_effectData.begin();
    for ( ; i != m_effectData.end(); ++i)
        if (i.value().m_supported)
            result.append(i.key());

    return result;
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

#define INITIALIZE_EFFECT(Effect) \
    { \
    EffectData data = getData<Effect>(); \
    m_effectData.insert(Type##Effect, data); \
    }

void EffectFactory::initialize()
{
    Q_ASSERT_X(!m_initialized, Q_FUNC_INFO, "Already initialized");

    INITIALIZE_EFFECT(AudioEqualizer)
    INITIALIZE_EFFECT(BassBoost)
    INITIALIZE_EFFECT(EnvironmentalReverb)
    INITIALIZE_EFFECT(Loudness)
    INITIALIZE_EFFECT(StereoWidening)

    m_initialized = true;
}

// This class is just a wrapper which allows us to instantiate a
// CMdaAudioOutputStream object.  This is done in order to allow the
// effects API to query the DevSound implementation, to discover
// which effects are supported and what parameters they take.
// Ideally, we would use CMMFDevSound directly, but this class is not
// available in the public S60 SDK.
class OutputStreamFactory : public MMdaAudioOutputStreamCallback
{
public:
    CMdaAudioOutputStream* create()
    {
        CMdaAudioOutputStream* stream = 0;
        QT_TRAP_THROWING(stream = CMdaAudioOutputStream::NewL(*this));
        return stream;
    }
private:
    void MaoscOpenComplete(TInt /*aError*/) { }
    void MaoscBufferCopied(TInt /*aError*/, const TDesC8& /*aBuffer*/) { }
    void MaoscPlayComplete(TInt /*aError*/) { }
};

template<typename BackendNode>
EffectFactory::EffectData EffectFactory::getData()
{
    EffectData data;

    // Create a temporary CMdaAudioOutputStream object, so that the effects
    // API can query DevSound to discover which effects are supported.
    OutputStreamFactory streamFactory;
    QScopedPointer<CMdaAudioOutputStream> stream(streamFactory.create());

    EffectParameter param(
         /* parameterId */        AbstractAudioEffect::ParameterEnable,
         /* name */               tr("Enabled"),
         /* hints */              EffectParameter::ToggledHint,
         /* defaultValue */       QVariant(bool(true)));
    data.m_parameters.append(param);

    data.m_supported = BackendNode::getParameters(stream.data(),
                                                  data.m_parameters);
    if (data.m_supported) {
        const QString description = QCoreApplication::translate
            ("Phonon::MMF::EffectFactory", BackendNode::description());
        data.m_descriptions.insert("name", description);
        data.m_descriptions.insert("description", description);
        data.m_descriptions.insert("available", true);
    }

    // Sanity check to ensure that all parameter IDs are unique
    QSet<int> ids;
    foreach (param, data.m_parameters) {
        Q_ASSERT_X(ids.find(param.id()) == ids.end(), Q_FUNC_INFO,
            "Parameter list contains duplicates");
        ids.insert(param.id());
    }

    return data;
}

const EffectFactory::EffectData& EffectFactory::data(Type type) const
{
    QHash<Type, EffectData>::const_iterator i = m_effectData.find(type);
    Q_ASSERT_X(i != m_effectData.end(), Q_FUNC_INFO, "Effect data not found");
    return i.value();
}

QT_END_NAMESPACE

