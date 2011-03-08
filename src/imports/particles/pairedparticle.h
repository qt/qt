#ifndef PAIREDPARTICLE_H
#define PAIREDPARTICLE_H
#include "particle.h"
#include <QDeclarativeParserStatus>
#include <QDeclarativeListProperty>

class PairedParticle : public Particle, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(Particle* particle READ particle WRITE particle NOTIFY particleChanged)
    Q_PROPERTY(QDeclarativeListProperty<PairedParticle> pairs READ pairs)
    Q_CLASSINFO("DefaultProperty", "particle")
public:
    explicit PairedParticle(QObject *parent = 0);
    virtual ~PairedParticle();

    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);
    virtual void componentComplete();
    virtual void classBegin(){}

    Particle* particle() const
    {
        return m_particle;
    }

    QDeclarativeListProperty<PairedParticle> pairs(){ return QDeclarativeListProperty<PairedParticle>(this, m_pairs); }
signals:

    void particleChanged(Particle* arg);

public slots:

void particle(Particle* arg)
{
    if (m_particle != arg) {
        m_particle = arg;
        emit particleChanged(arg);
    }
}

private:
    int m_count;
    Particle* m_particle;
    QList<PairedParticle*> m_pairs;
    QList<QPointF> m_differences;

    ParticleData* translate(ParticleData*, const QPointF &difference, bool forwards = true);
};

#endif // PAIREDPARTICLE_H
