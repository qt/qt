#ifndef PARTICLE_H
#define PARTICLE_H

#include <QObject>
#include "particlesystem.h"

class ParticleType : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
public:
    explicit ParticleType(QSGItem *parent = 0);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual int count();
    ParticleSystem* system() const
    {
        return m_system;
    }

signals:
    void countChanged();
    void systemChanged(ParticleSystem* arg);

public slots:
void setSystem(ParticleSystem* arg)
{
    if (m_system != arg) {
        m_system = arg;
        m_system->registerParticleType(this);
        emit systemChanged(arg);
    }
}

protected:
    ParticleSystem* m_system;
    friend class ParticleSystem;
    int m_count;
    bool m_pleaseReset;
};

#endif // PARTICLE_H
