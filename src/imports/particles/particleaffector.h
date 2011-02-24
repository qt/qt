#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
#include "particlesystem.h"

class ParticleAffector : public QObject
{
    Q_OBJECT
public:
    explicit ParticleAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);//As some store their own data per idx
signals:

public slots:

};

#endif // PARTICLEAFFECTOR_H
