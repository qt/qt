#include "scalingaffector.h"
#include "spriteparticles.h"
#include "particlesystem.h"

ScalingAffector::ScalingAffector(QObject *parent) :
    ParticleAffector(parent)
{
}

//TODO: get duration better than from vAnimDat
void ScalingAffector::affect(ParticleVertices *p, int idx, qreal dt, QObject* emitter)
{
    Q_UNUSED(idx);
    qreal scale = (p->v1.endSize - p->v1.size) * (dt/(emitter->property("particleDuration").toInt()/1000.0));
    scale *= 0.5;//half back, half forwards to grow from the center

    p->v1.x -= scale;
    p->v1.y -= scale;

    p->v2.x += scale;
    p->v2.y -= scale;

    p->v3.x -= scale;
    p->v3.y += scale;

    p->v4.x += scale;
    p->v4.y += scale;
}
