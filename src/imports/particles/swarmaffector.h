#ifndef SWARMAFFECTOR_H
#define SWARMAFFECTOR_H
#include "particleaffector.h"
#include <QDeclarativeListProperty>

class Particle;

class SwarmAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(QDeclarativeListProperty<Particle> leaders READ leaders)
public:
    explicit SwarmAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);

    QDeclarativeListProperty<Particle> leaders(){return QDeclarativeListProperty<Particle>(this, m_leaders);}
    qreal strength() const
    {
        return m_strength;
    }

signals:

    void strengthChanged(qreal arg);

public slots:

void setStrength(qreal arg)
{
    if (m_strength != arg) {
        m_strength = arg;
        emit strengthChanged(arg);
    }
}

private:
    void ensureInit(ParticleData* d);
    void mapUpdate(int idx, qreal strength);
    QList<Particle*> m_leaders;
    ParticleSystem* m_system;
    QVector<QPointF> m_lastPos;
    qreal m_strength;
};

#endif // SWARMAFFECTOR_H
