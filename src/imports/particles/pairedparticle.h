#ifndef PAIREDPARTICLE_H
#define PAIREDPARTICLE_H
#include "particle.h"
#include <QDeclarativeParserStatus>
#include <QDeclarativeListProperty>

class PairedParticle : public ParticleType, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(ParticleType* particle READ particle WRITE particle NOTIFY particleChanged)
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

    ParticleType* particle() const
    {
        return m_particle;
    }

    QDeclarativeListProperty<PairedParticle> pairs(){ return QDeclarativeListProperty<PairedParticle>(this, m_pairs); }
signals:

    void particleChanged(ParticleType* arg);

public slots:

void particle(ParticleType* arg)
{
    if (m_particle != arg) {
        m_particle = arg;
        emit particleChanged(arg);
    }
}

private:
    int m_count;
    ParticleType* m_particle;
    QList<PairedParticle*> m_pairs;
    QList<QPointF> m_differences;

    ParticleData* translate(ParticleData*, const QPointF &difference, bool forwards = true);
};

#endif // PAIREDPARTICLE_H
