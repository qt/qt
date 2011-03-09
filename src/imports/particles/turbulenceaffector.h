#ifndef TURBULENCEAFFECTOR_H
#define TURBULENCEAFFECTOR_H
#include "particleaffector.h"
#include <QDeclarativeListProperty>

class Particle;

class TurbulenceAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(int strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
public:
    explicit TurbulenceAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);

    int strength() const
    {
        return m_strength;
    }

    int frequency() const
    {
        return m_frequency;
    }

    int size() const
    {
        return m_size;
    }

signals:

    void strengthChanged(int arg);

    void frequencyChanged(int arg);

    void sizeChanged(int arg);

public slots:

void setStrength(int arg)
{
    if (m_strength != arg) {
        m_strength = arg;
        emit strengthChanged(arg);
    }
}

void setFrequency(int arg)
{
    if (m_frequency != arg) {
        m_frequency = arg;
        emit frequencyChanged(arg);
    }
}

void setSize(int arg)
{
    if (m_size != arg) {
        m_size = arg;
        emit sizeChanged(arg);
    }
}

private:
    void ensureInit(ParticleData* d);
    void tickAdvance(ParticleData* d);
    void mapUpdate();
    ParticleSystem* m_system;
    QVector<QVector<QPointF> > m_field;
    int m_strength;
    qreal m_lastT;
    int m_frequency;
    int m_size;
    QPointF m_spacing;
    qreal m_magSum;
};

#endif // TURBULENCEAFFECTOR_H
