#ifndef TURBULENCEAFFECTOR_H
#define TURBULENCEAFFECTOR_H
#include "particleaffector.h"
#include <QDeclarativeListProperty>

class ParticleType;

class TurbulenceAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(int strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(int frequency READ frequency WRITE setFrequency NOTIFY frequencyChanged)
    Q_PROPERTY(int gridSize READ size WRITE setSize NOTIFY sizeChanged)
public:
    explicit TurbulenceAffector(QSGItem *parent = 0);
    ~TurbulenceAffector();
    virtual void affectSystem(qreal dt);

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
        return m_gridSize;
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

void setSize(int arg);

private:
    void ensureInit();
    void mapUpdate();
    int m_strength;
    qreal m_lastT;
    int m_frequency;
    int m_gridSize;
    QPointF** m_field;
    QPointF m_spacing;
    qreal m_magSum;
    bool m_inited;
};

#endif // TURBULENCEAFFECTOR_H
