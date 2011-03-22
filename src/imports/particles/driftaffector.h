#ifndef DRIFTAFFECTOR_H
#define DRIFTAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class DriftAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal xDrift READ xDrift WRITE setXDrift NOTIFY xDriftChanged)
    Q_PROPERTY(qreal yDrift READ yDrift WRITE setYDrift NOTIFY yDriftChanged)
public:
    explicit DriftAffector(QSGItem *parent = 0);
    ~DriftAffector();
    qreal yDrift() const
    {
        return m_yDrift;
    }

    qreal xDrift() const
    {
        return m_xDrift;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);

signals:

    void yDriftChanged(qreal arg);

    void xDriftChanged(qreal arg);

public slots:

void setYDrift(qreal arg)
{
    if (m_yDrift != arg) {
        m_yDrift = arg;
        emit yDriftChanged(arg);
    }
}

void setXDrift(qreal arg)
{
    if (m_xDrift != arg) {
        m_xDrift = arg;
        emit xDriftChanged(arg);
    }
}

private:
    qreal m_yDrift;
    qreal m_xDrift;
};

QT_END_NAMESPACE
#endif // DRIFTAFFECTOR_H
