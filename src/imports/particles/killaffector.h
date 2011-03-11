#ifndef KILLAFFECTOR_H
#define KILLAFFECTOR_H
#include "particleaffector.h"

class KillAffector : public ParticleAffector
{
    Q_OBJECT
public:
    explicit KillAffector(QSGItem *parent = 0);
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

public slots:

};

#endif // KILLAFFECTOR_H
