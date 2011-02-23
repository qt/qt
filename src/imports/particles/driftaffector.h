#ifndef DRIFTAFFECTOR_H
#define DRIFTAFFECTOR_H
#include "particleaffector.h"
#include <QHash>

struct DriftData{
    qreal xVel;
    qreal yVel;
};

class DriftAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal xDrift READ xDrift WRITE setXDrift NOTIFY xDriftChanged)
    Q_PROPERTY(qreal yDrift READ yDrift WRITE setYDrift NOTIFY yDriftChanged)
public:
    explicit DriftAffector(QObject *parent = 0);
    ~DriftAffector();
    virtual void affect(ParticleVertices *p, int idx, qreal dt, QObject* emitter);
    virtual void reset(int idx);
    qreal yDrift() const
    {
        return m_yDrift;
    }

    qreal xDrift() const
    {
        return m_xDrift;
    }

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
    DriftData* getData(int idx);
    QHash<int, DriftData*> m_driftData;
    qreal m_yDrift;
    qreal m_xDrift;
};

#endif // DRIFTAFFECTOR_H
