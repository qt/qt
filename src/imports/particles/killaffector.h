#ifndef KILLAFFECTOR_H
#define KILLAFFECTOR_H
#include "particleaffector.h"

class KillAffector : public ParticleAffector
{
    Q_OBJECT
public:
    explicit KillAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
signals:

public slots:

};

#endif // KILLAFFECTOR_H
