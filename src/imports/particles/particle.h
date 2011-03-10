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
    virtual int count();
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);
    ParticleSystem* m_system; //set by classless function
    bool wantsReset;
signals:
    void countChanged();
public slots:
protected:
    int m_count;
};

#endif // PARTICLE_H
