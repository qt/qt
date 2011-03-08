#ifndef FOLLOWEMITTER_H
#define FOLLOWEMITTER_H
#include "particleemitter.h"
#include "particleaffector.h"

class FollowEmitter : public ParticleEmitter
{
    Q_OBJECT
    Q_PROPERTY(Particle* follow READ follow WRITE setFollow NOTIFY followChanged)
    Q_PROPERTY(int particlesPerParticlePerSecond READ particlesPerParticlePerSecond WRITE setParticlesPerParticlePerSecond NOTIFY particlesPerParticlePerSecondChanged)

    Q_PROPERTY(qreal emitterXVariation READ emitterXVariation WRITE setEmitterXVariation NOTIFY emitterXVariationChanged)
    Q_PROPERTY(qreal emitterYVariation READ emitterYVariation WRITE setEmitterYVariation NOTIFY emitterYVariationChanged)

    Q_PROPERTY(qreal particleSize READ particleSize WRITE setParticleSize NOTIFY particleSizeChanged)
    Q_PROPERTY(qreal particleEndSize READ particleEndSize WRITE setParticleEndSize NOTIFY particleEndSizeChanged)
    Q_PROPERTY(qreal particleSizeVariation READ particleSizeVariation WRITE setParticleSizeVariation NOTIFY particleSizeVariationChanged)

    Q_PROPERTY(qreal xSpeed READ xSpeed WRITE setXSpeed NOTIFY xSpeedChanged)
    Q_PROPERTY(qreal ySpeed READ ySpeed WRITE setYSpeed NOTIFY ySpeedChanged)
    Q_PROPERTY(qreal xSpeedVariation READ xSpeedVariation WRITE setXSpeedVariation NOTIFY xSpeedVariationChanged)
    Q_PROPERTY(qreal ySpeedVariation READ ySpeedVariation WRITE setYSpeedVariation NOTIFY ySpeedVariationChanged)

    Q_PROPERTY(qreal xAccel READ xAccel WRITE setXAccel NOTIFY xAccelChanged)
    Q_PROPERTY(qreal yAccel READ yAccel WRITE setYAccel NOTIFY yAccelChanged)
    Q_PROPERTY(qreal xAccelVariation READ xAccelVariation WRITE setXAccelVariation NOTIFY xAccelVariationChanged)
    Q_PROPERTY(qreal yAccelVariation READ yAccelVariation WRITE setYAccelVariation NOTIFY yAccelVariationChanged)

public:
    explicit FollowEmitter(QSGItem *parent = 0);
    virtual void emitWindow(int timeStamp);

    qreal particleSize() const
    {
        return m_particleSize;
    }

    qreal particleEndSize() const
    {
        return m_particleEndSize;
    }

    qreal particleSizeVariation() const
    {
        return m_particleSizeVariation;
    }

    qreal xSpeed() const
    {
        return m_xSpeed;
    }

    qreal ySpeed() const
    {
        return m_ySpeed;
    }

    qreal xSpeedVariation() const
    {
        return m_xSpeedVariation;
    }

    qreal ySpeedVariation() const
    {
        return m_ySpeedVariation;
    }

    qreal xAccel() const
    {
        return m_xAccel;
    }

    qreal yAccel() const
    {
        return m_yAccel;
    }

    qreal xAccelVariation() const
    {
        return m_xAccelVariation;
    }

    qreal yAccelVariation() const
    {
        return m_yAccelVariation;
    }

    Particle* follow() const
    {
        return m_follow;
    }

    int particlesPerParticlePerSecond() const
    {
        return m_particlesPerParticlePerSecond;
    }

    qreal emitterXVariation() const
    {
        return m_emitterXVariation;
    }

    qreal emitterYVariation() const
    {
        return m_emitterYVariation;
    }

signals:

    void particleSizeChanged(qreal arg);

    void particleEndSizeChanged(qreal arg);

    void particleSizeVariationChanged(qreal arg);

    void xSpeedChanged(qreal arg);

    void ySpeedChanged(qreal arg);

    void xSpeedVariationChanged(qreal arg);

    void ySpeedVariationChanged(qreal arg);

    void xAccelChanged(qreal arg);

    void yAccelChanged(qreal arg);

    void xAccelVariationChanged(qreal arg);

    void yAccelVariationChanged(qreal arg);

    void followChanged(Particle* arg);

    void particlesPerParticlePerSecondChanged(int arg);

    void emitterXVariationChanged(qreal arg);

    void emitterYVariationChanged(qreal arg);

public slots:

    void setParticleSize(qreal arg)
    {
        if (m_particleSize != arg) {
            m_particleSize = arg;
            emit particleSizeChanged(arg);
        }
    }
    void setParticleEndSize(qreal arg)
    {
        if (m_particleEndSize != arg) {
            m_particleEndSize = arg;
            emit particleEndSizeChanged(arg);
        }
    }

    void setParticleSizeVariation(qreal arg)
    {
        if (m_particleSizeVariation != arg) {
            m_particleSizeVariation = arg;
            emit particleSizeVariationChanged(arg);
        }
    }

    void setXSpeed(qreal arg)
    {
        if (m_xSpeed != arg) {
            m_xSpeed = arg;
            emit xSpeedChanged(arg);
        }
    }

    void setYSpeed(qreal arg)
    {
        if (m_ySpeed != arg) {
            m_ySpeed = arg;
            emit ySpeedChanged(arg);
        }
    }

    void setXSpeedVariation(qreal arg)
    {
        if (m_xSpeedVariation != arg) {
            m_xSpeedVariation = arg;
            emit xSpeedVariationChanged(arg);
        }
    }

    void setYSpeedVariation(qreal arg)
    {
        if (m_ySpeedVariation != arg) {
            m_ySpeedVariation = arg;
            emit ySpeedVariationChanged(arg);
        }
    }

    void setXAccel(qreal arg)
    {
        if (m_xAccel != arg) {
            m_xAccel = arg;
            emit xAccelChanged(arg);
        }
    }

    void setYAccel(qreal arg)
    {
        if (m_yAccel != arg) {
            m_yAccel = arg;
            emit yAccelChanged(arg);
        }
    }

    void setXAccelVariation(qreal arg)
    {
        if (m_xAccelVariation != arg) {
            m_xAccelVariation = arg;
            emit xAccelVariationChanged(arg);
        }
    }

    void setYAccelVariation(qreal arg)
    {
        if (m_yAccelVariation != arg) {
            m_yAccelVariation = arg;
            emit yAccelVariationChanged(arg);
        }
    }

    void setFollow(Particle* arg);


    void setParticlesPerParticlePerSecond(int arg)
    {
        if (m_particlesPerParticlePerSecond != arg) {
            m_particlesPerParticlePerSecond = arg;
            emit particlesPerParticlePerSecondChanged(arg);
        }
    }
    void setEmitterXVariation(qreal arg)
    {
        if (m_emitterXVariation != arg) {
            m_emitterXVariation = arg;
            emit emitterXVariationChanged(arg);
        }
    }

    void setEmitterYVariation(qreal arg)
    {
        if (m_emitterYVariation != arg) {
            m_emitterYVariation = arg;
            emit emitterYVariationChanged(arg);
        }
    }

private slots:
    void recalcParticlesPerSecond();

private:
    friend class FollowAffector;
    qreal m_particleSize;
    qreal m_particleEndSize;
    qreal m_particleSizeVariation;
    qreal m_xSpeed;
    qreal m_ySpeed;
    qreal m_xSpeedVariation;
    qreal m_ySpeedVariation;
    qreal m_xAccel;
    qreal m_yAccel;
    qreal m_xAccelVariation;
    qreal m_yAccelVariation;
    Particle* m_follow;
    QSet<ParticleData*> m_pending;
    int m_particlesPerParticlePerSecond;
    qreal m_lastTimeStamp;
    qreal m_emitterXVariation;
    qreal m_emitterYVariation;
};

#endif // FOLLOWEMITTER_H
