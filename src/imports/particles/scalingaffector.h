#ifndef SCALINGAFFECTOR_H
#define SCALINGAFFECTOR_H
#include "particleaffector.h"
class SpriteParticles;

class ScalingAffector : public ParticleAffector
{
    Q_OBJECT
public:
    explicit ScalingAffector(QObject *parent = 0);
    virtual void affect(ParticleVertices *p, int idx, qreal dt, SpriteParticles* sp);

signals:

public slots:

};

#endif // SCALINGAFFECTOR_H
