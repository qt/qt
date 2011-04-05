#ifndef PARTICLE_H
#define PARTICLE_H

#include <QObject>
#include "particlesystem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleType : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(ParticleSystem* system READ system WRITE setSystem NOTIFY systemChanged)
    Q_PROPERTY(QStringList particles READ particles WRITE setParticles NOTIFY particlesChanged)


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


    QStringList particles() const
    {
        return m_particles;
    }

    int particleTypeIndex(ParticleData*);
signals:
    void countChanged();
    void systemChanged(ParticleSystem* arg);

    void particlesChanged(QStringList arg);

public slots:
void setSystem(ParticleSystem* arg);

void setParticles(QStringList arg)
{
    if (m_particles != arg) {
        m_particles = arg;
        emit particlesChanged(arg);
    }
}
private slots:
    void calcSystemOffset();
protected:
    virtual void reset() {;}

    ParticleSystem* m_system;
    friend class ParticleSystem;
    int m_count;
    bool m_pleaseReset;
    QStringList m_particles;
    QHash<int,int> m_particleStarts;
    int m_lastStart;
    QPointF m_systemOffset;
private:
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // PARTICLE_H
