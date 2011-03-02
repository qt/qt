#ifndef TOGGLEAFFECTOR_H
#define TOGGLEAFFECTOR_H
#include "particleaffector.h"

class ToggleAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(bool affecting READ affecting WRITE setAffecting NOTIFY affectingChanged)
    Q_PROPERTY(ParticleAffector* affector READ affector WRITE affector NOTIFY affectorChanged)
    Q_CLASSINFO("DefaultProperty", "affector")

public:
    explicit ToggleAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    bool affecting() const
    {
        return m_affecting;
    }

    ParticleAffector* affector() const
    {
        return m_affector;
    }

signals:

    void affectingChanged(bool arg);

    void affectorChanged(ParticleAffector* arg);

public slots:
void setAffecting(bool arg)
{
    if (m_affecting != arg) {
        m_affecting = arg;
        emit affectingChanged(arg);
    }
}

void affector(ParticleAffector* arg)
{
    if (m_affector != arg) {
        m_affector = arg;
        emit affectorChanged(arg);
    }
}

private:
bool m_affecting;
ParticleAffector* m_affector;
};

#endif // TOGGLEAFFECTOR_H
