#ifndef DIRECTEDEMITTER_H
#define DIRECTEDEMITTER_H
#include "particleemitter.h"

class DirectedEmitter : public ParticleEmitter
{
    Q_OBJECT

    //TODO: Make up one's mind about where size goes
    Q_PROPERTY(qreal particleSize READ particleSize WRITE setParticleSize NOTIFY particleSizeChanged)
    Q_PROPERTY(qreal particleEndSize READ particleEndSize WRITE setParticleEndSize NOTIFY particleEndSizeChanged)
    Q_PROPERTY(qreal particleSizeVariation READ particleSizeVariation WRITE setParticleSizeVariation NOTIFY particleSizeVariationChanged)


    Q_PROPERTY(qreal targetX READ targetX WRITE setTargetX NOTIFY targetXChanged)
    Q_PROPERTY(qreal targetY READ targetY WRITE setTargetY NOTIFY targetYChanged)

    Q_PROPERTY(qreal speed READ speed WRITE setSpeed NOTIFY speedChanged)
    Q_PROPERTY(qreal speedVariation READ speedVariation WRITE setSpeedVariation NOTIFY speedVariation)
    Q_PROPERTY(qreal acceleration READ acceleration WRITE setAcceleration NOTIFY accelerationChanged)
    Q_PROPERTY(qreal accelerationVariation READ accelerationVariation WRITE setAcceleration NOTIFY acclerationChanged)

public:
    explicit DirectedEmitter(QSGItem *parent = 0);
    virtual void emitWindow(int timeStamp);
    virtual void reset();

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

    qreal targetX() const
    {
        return m_targetX;
    }

    qreal targetY() const
    {
        return m_targetY;
    }

    qreal accelerationVariation() const
    {
        return m_accelerationVariation;
    }

    qreal acceleration() const
    {
        return m_acceleration;
    }

    qreal speedVariation() const
    {
        return m_speedVariation;
    }

    qreal speed() const
    {
        return m_speed;
    }

signals:

    void particleSizeChanged(qreal arg);

    void particleEndSizeChanged(qreal arg);

    void particleSizeVariationChanged(qreal arg);

    void targetXChanged(qreal arg);

    void targetYChanged(qreal arg);

    void acclerationChanged(qreal arg);

    void accelerationChanged(qreal arg);

    void speedVariation(qreal arg);

    void speedChanged(qreal arg);

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

    void setTargetX(qreal arg)
    {
        if (m_targetX != arg) {
            m_targetX = arg;
            emit targetXChanged(arg);
        }
    }

    void setTargetY(qreal arg)
    {
        if (m_targetY != arg) {
            m_targetY = arg;
            emit targetYChanged(arg);
        }
    }

    void setAcceleration(qreal arg)
    {
        if (m_accelerationVariation != arg) {
            m_accelerationVariation = arg;
            emit acclerationChanged(arg);
        }
    }

    void setSpeedVariation(qreal arg)
    {
        if (m_speedVariation != arg) {
            m_speedVariation = arg;
            emit speedVariation(arg);
        }
    }

    void setSpeed(qreal arg)
    {
        if (m_speed != arg) {
            m_speed = arg;
            emit speedChanged(arg);
        }
    }

private:
    qreal m_particleSize;
    qreal m_particleEndSize;
    qreal m_particleSizeVariation;
    qreal m_targetX;
    qreal m_targetY;
    qreal m_speed;
    qreal m_speedVariation;
    qreal m_acceleration;
    qreal m_accelerationVariation;
    bool m_resetLast;
    QPointF m_lastTarget;
    qreal m_lastTimestamp;
    int m_lastParticle;

};

#endif // DIRECTEDEMITTER_H
