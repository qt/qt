#ifndef FOLLOWEMITTER_H
#define FOLLOWEMITTER_H
#include "particleemitter.h"
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class FollowEmitter : public ParticleEmitter
{
    Q_OBJECT
    Q_PROPERTY(QString follow READ follow WRITE setFollow NOTIFY followChanged)
    Q_PROPERTY(int particlesPerParticlePerSecond READ particlesPerParticlePerSecond WRITE setParticlesPerParticlePerSecond NOTIFY particlesPerParticlePerSecondChanged)

    //TODO: Document that FollowEmitter's box is where it follows. It emits in a rect centered on the followed particle
    //TODO: A set of properties that can involve the particle size of the followed
    Q_PROPERTY(qreal emitterXVariation READ emitterXVariation WRITE setEmitterXVariation NOTIFY emitterXVariationChanged)
    Q_PROPERTY(qreal emitterYVariation READ emitterYVariation WRITE setEmitterYVariation NOTIFY emitterYVariationChanged)

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

    QString follow() const
    {
        return m_follow;
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

    void particlesPerParticlePerSecondChanged(int arg);

    void emitterXVariationChanged(qreal arg);

    void emitterYVariationChanged(qreal arg);

    void followChanged(QString arg);

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

    void setFollow(QString arg)
    {
        if (m_follow != arg) {
            m_follow = arg;
            emit followChanged(arg);
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
    QSet<ParticleData*> m_pending;
    QVector<qreal> m_lastEmission;
    int m_particlesPerParticlePerSecond;
    qreal m_lastTimeStamp;
    qreal m_emitterXVariation;
    qreal m_emitterYVariation;
    QString m_follow;
    int m_followCount;
};

QT_END_NAMESPACE
#endif // FOLLOWEMITTER_H
