#include "pluginmain.h"
#include "spriteparticles.h"
#include "spritestate.h"

void ParticlesPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("Qt.labs.particles"));

    qmlRegisterType<SpriteParticles>(uri, 2, 0, "SpriteParticles");
    qmlRegisterType<SpriteState>(uri, 2, 0, "SpriteState");
}

Q_EXPORT_PLUGIN2(Particles, ParticlesPlugin);

