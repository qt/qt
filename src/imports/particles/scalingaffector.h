#ifndef SCALINGAFFECTOR_H
#define SCALINGAFFECTOR_H
#include "particleaffector.h"
class SpriteEmitter;

class ScalingAffector : public ParticleAffector
{
    Q_OBJECT
public:
    explicit ScalingAffector(QObject *parent = 0);
    virtual void affect(ParticleVertices *p, int idx, qreal dt, QObject* emitter);

signals:

public slots:

};

#endif // SCALINGAFFECTOR_H
