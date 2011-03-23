#ifndef KILLAFFECTOR_H
#define KILLAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


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

QT_END_NAMESPACE
QT_END_HEADER
#endif // KILLAFFECTOR_H
