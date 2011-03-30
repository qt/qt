#ifndef ETERNALAFFECTOR_H
#define ETERNALAFFECTOR_H
#include "particleaffector.h"

class EternalAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(int targetLife READ targetLife WRITE setTargetLife NOTIFY targetLifeChanged)

public:
    explicit EternalAffector(QSGItem *parent = 0);
    int targetLife() const
    {
        return m_targetLife;
    }

protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);

signals:

    void targetLifeChanged(int arg);

public slots:

    void setTargetLife(int arg)
    {
        if (m_targetLife != arg) {
            m_targetLife = arg;
            emit targetLifeChanged(arg);
        }
    }
private:
    int m_targetLife;
};

#endif // ETERNALAFFECTOR_H
