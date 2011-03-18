#ifndef TRAILSEMITTER_H
#define TRAILSEMITTER_H

#include <QtCore>
#include <QtGui>

#include "particleemitter.h"

class ParticleTrailsMaterial;
class GeometryNode;

class TrailsEmitter : public ParticleEmitter
{
    Q_OBJECT

    //XXXCurrently these are added to item props
    Q_PROPERTY(qreal emitterX READ emitterX WRITE setEmitterX NOTIFY emitterXChanged)
    Q_PROPERTY(qreal emitterY READ emitterY WRITE setEmitterY NOTIFY emitterYChanged)
    Q_PROPERTY(qreal emitterXVariation READ emitterXVariation WRITE setEmitterXVariation NOTIFY emitterXVariationChanged)
    Q_PROPERTY(qreal emitterYVariation READ emitterYVariation WRITE setEmitterYVariation NOTIFY emitterYVariationChanged)

    Q_PROPERTY(qreal speedFromMovement READ speedFromMovement WRITE setSpeedFromMovement NOTIFY speedFromMovementChanged)
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
    explicit TrailsEmitter(QSGItem* parent=0);
    virtual ~TrailsEmitter(){}
    virtual void emitWindow(int timeStamp);


    qreal particleSize() const { return m_particle_size; }
    void setParticleSize(qreal size);

    qreal particleEndSize() const { return m_particle_end_size; }
    void setParticleEndSize(qreal size);

    qreal particleSizeVariation() const { return m_particle_size_variation; }
    void setParticleSizeVariation(qreal var);


    qreal emitterX() const { return m_emitter_x; }
    void setEmitterX(qreal x);

    qreal emitterY() const { return m_emitter_y; }
    void setEmitterY(qreal y);

    qreal emitterXVariation() const { return m_emitter_x_variation; }
    void setEmitterXVariation(qreal var);

    qreal emitterYVariation() const { return m_emitter_y_variation; }
    void setEmitterYVariation(qreal var);

    qreal xSpeed() const { return m_x_speed; }
    void setXSpeed(qreal x);

    qreal ySpeed() const { return m_y_speed; }
    void setYSpeed(qreal y);

    qreal xSpeedVariation() const { return m_x_speed_variation; }
    void setXSpeedVariation(qreal x);

    qreal ySpeedVariation() const { return m_y_speed_variation; }
    void setYSpeedVariation(qreal y);

    qreal speedFromMovement() const { return m_speed_from_movement; }
    void setSpeedFromMovement(qreal s);


    qreal xAccel() const { return m_x_accel; }
    void setXAccel(qreal x);

    qreal yAccel() const { return m_y_accel; }
    void setYAccel(qreal y);

    qreal xAccelVariation() const { return m_x_accel_variation; }
    void setXAccelVariation(qreal x);

    qreal yAccelVariation() const { return m_y_accel_variation; }
    void setYAccelVariation(qreal y);


    qreal renderOpacity() const { return m_render_opacity; }

signals:

    void particleSizeChanged();
    void particleEndSizeChanged();
    void particleSizeVariationChanged();

    void emitterXChanged();
    void emitterYChanged();
    void emitterXVariationChanged();
    void emitterYVariationChanged();
    void emittingChanged();

    void xSpeedChanged();
    void ySpeedChanged();
    void xSpeedVariationChanged();
    void ySpeedVariationChanged();
    void speedFromMovementChanged();

    void xAccelChanged();
    void yAccelChanged();
    void xAccelVariationChanged();
    void yAccelVariationChanged();

public slots:
public:
    virtual void reset();
protected:

private:

    qreal m_particle_size;
    qreal m_particle_end_size;
    qreal m_particle_size_variation;

    qreal m_emitter_x;
    qreal m_emitter_y;
    qreal m_emitter_x_variation;
    qreal m_emitter_y_variation;

    qreal m_x_speed;
    qreal m_y_speed;
    qreal m_x_speed_variation;
    qreal m_y_speed_variation;
    qreal m_speed_from_movement;

    qreal m_x_accel;
    qreal m_y_accel;
    qreal m_x_accel_variation;
    qreal m_y_accel_variation;

    // derived values...
    int m_particle_count;
    bool m_reset_last;
    qreal m_last_timestamp;
    int m_last_particle;

    QPointF m_last_emitter;
    QPointF m_last_last_emitter;
    QPointF m_last_last_last_emitter;

    qreal m_render_opacity;
};

#endif // TRAILSEMITTER_H
