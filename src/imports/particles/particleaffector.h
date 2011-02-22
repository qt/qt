#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>

struct ParticleVertices;
class SpriteParticles;

class ParticleAffector : public QObject
{
    Q_OBJECT
public:
    explicit ParticleAffector(QObject *parent = 0);
    //###Pass the system in? Or register it? Or becomes moot once everything's moved out?
    virtual void affect(ParticleVertices *p, int idx, qreal dt, SpriteParticles* sp);
    virtual void reset(int idx);//As some store their own data per idx
signals:

public slots:

};

#endif // PARTICLEAFFECTOR_H
