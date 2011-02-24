#ifndef PARTICLE_H
#define PARTICLE_H

#include <QObject>
#include "particlesystem.h"

class Particle : public QObject
{
    Q_OBJECT
public:
    explicit Particle(QObject *parent = 0);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);
signals:

public slots:

private:
};

#endif // PARTICLE_H
