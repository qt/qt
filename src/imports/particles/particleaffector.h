#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
#include "particlesystem.h"

class ParticleAffector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)

public:
    explicit ParticleAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);//As some store their own data per idx?
    ParticleSystem* system() const
    {
        return m_system;
    }

signals:

    void systemChanged(ParticleSystem* arg);

public slots:
void setSystem(ParticleSystem* arg)
{
    if (m_system != arg) {
        m_system = arg;
        m_system->registerParticleAffector(this);
        emit systemChanged(arg);
    }
}

protected:
    ParticleSystem* m_system;

};

#endif // PARTICLEAFFECTOR_H
