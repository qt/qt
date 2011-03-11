#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
#include "particlesystem.h"

class ParticleAffector : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(QStringList particles READ particles WRITE setParticles NOTIFY particlesChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    explicit ParticleAffector(QSGItem *parent = 0);
    virtual void affectSystem(qreal dt);
    virtual void reset(int systemIdx);//As some store their own data per idx?
    ParticleSystem* system() const
    {
        return m_system;
    }

    QStringList particles() const
    {
        return m_particles;
    }

    bool active() const
    {
        return m_active;
    }

signals:

    void systemChanged(ParticleSystem* arg);

    void particlesChanged(QStringList arg);

    void activeChanged(bool arg);

public slots:
void setSystem(ParticleSystem* arg)
{
    if (m_system != arg) {
        m_system = arg;
        m_system->registerParticleAffector(this);
        emit systemChanged(arg);
    }
}

void setParticles(QStringList arg)
{
    if (m_particles != arg) {
        m_particles = arg;
        m_updateIntSet = true;
        emit particlesChanged(arg);
    }
}

void setActive(bool arg)
{
    if (m_active != arg) {
        m_active = arg;
        emit activeChanged(arg);
    }
}

protected:
    friend class ParticleSystem;
    virtual bool affectParticle(ParticleData *d, qreal dt);
    bool m_needsReset;
    ParticleSystem* m_system;
    QStringList m_particles;
    bool activeGroup(int g) {return m_groups.isEmpty() || m_groups.contains(g);}
    bool m_active;
private:
    QSet<int> m_groups;
    bool m_updateIntSet;
};

#endif // PARTICLEAFFECTOR_H
