#include "pluginmain.h"
//#include "spriteparticles.h"
#include "spritestate.h"
#include "particleaffector.h"
#include "wanderaffector.h"
//#include "rockingaffector.h"
//#include "scalingaffector.h"
#include "driftaffector.h"
#include "particlesystem.h"
#include "particleemitter.h"
//#include "spriteemitter.h"
#include "trailsemitter.h"
#include "particle.h"
#include "coloredparticle.h"
#include "spriteparticle.h"
#include "V1/qdeclarativeparticles_p.h"

void ParticlesPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.particles"));

    qmlRegisterType<QDeclarativeParticles>(uri, 1, 0, "Particles");
    qmlRegisterType<QDeclarativeParticleMotion>(uri,1,0,"ParticleMotion");
    qmlRegisterType<QDeclarativeParticleMotionGravity>(uri,1,0,"ParticleMotionGravity");
    qmlRegisterType<QDeclarativeParticleMotionLinear>(uri,1,0,"ParticleMotionLinear");
    qmlRegisterType<QDeclarativeParticleMotionWander>(uri,1,0,"ParticleMotionWander");
    qmlRegisterType<SpriteState>(uri, 2, 0, "SpriteState");

    qmlRegisterType<ParticleSystem>(uri, 2, 0, "ParticleSystem");

    qmlRegisterType<Particle>(uri, 2, 0, "Particle");
    qmlRegisterType<ColoredParticle>(uri, 2, 0, "ColoredParticle");
    qmlRegisterType<SpriteParticle>(uri, 2, 0, "SpriteParticle");

    qmlRegisterType<ParticleEmitter>(uri, 2, 0, "ParticleEmitter");
    qmlRegisterType<TrailsEmitter>(uri, 2, 0, "TrailEmitter");

    qmlRegisterType<ParticleAffector>(uri, 2, 0, "ParticleAffector");
    qmlRegisterType<WanderAffector>(uri, 2, 0, "Wander");
    //qmlRegisterType<ScalingAffector>(uri, 2, 0, "Scale");
    //qmlRegisterType<RockingAffector>(uri, 2, 0, "Rocking");
    qmlRegisterType<DriftAffector>(uri, 2, 0, "Drift");
}

Q_EXPORT_PLUGIN2(Particles, ParticlesPlugin);

