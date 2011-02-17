#include "pluginmain.h"
#include "spriteparticles.h"
#include "spritestate.h"
#include "particleaffector.h"
#include "wanderaffector.h"
#include "V1/qdeclarativeparticles_p.h"

void ParticlesPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.particles"));

    qmlRegisterType<QDeclarativeParticles>(uri, 1, 0, "Particles");
    qmlRegisterType<QDeclarativeParticleMotion>(uri,1,0,"ParticleMotion");
    qmlRegisterType<QDeclarativeParticleMotionGravity>(uri,1,0,"ParticleMotionGravity");
    qmlRegisterType<QDeclarativeParticleMotionLinear>(uri,1,0,"ParticleMotionLinear");
    qmlRegisterType<QDeclarativeParticleMotionWander>(uri,1,0,"ParticleMotionWander");
    qmlRegisterType<SpriteParticles>(uri, 2, 0, "SpriteParticles");
    qmlRegisterType<SpriteState>(uri, 2, 0, "SpriteState");
    qmlRegisterType<ParticleAffector>(uri, 2, 0, "BasicAffector");//###Should this be default? Or Null?
    qmlRegisterType<WanderAffector>(uri, 2, 0, "WanderAffector");
}

Q_EXPORT_PLUGIN2(Particles, ParticlesPlugin);

