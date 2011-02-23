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

    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl colortable READ colortable WRITE setColortable NOTIFY colortableChanged)

    Q_PROPERTY(qreal emitterX READ emitterX WRITE setEmitterX NOTIFY emitterXChanged)
    Q_PROPERTY(qreal emitterY READ emitterY WRITE setEmitterY NOTIFY emitterYChanged)
    Q_PROPERTY(qreal emitterXVariation READ emitterXVariation WRITE setEmitterXVariation NOTIFY emitterXVariationChanged)
    Q_PROPERTY(qreal emitterYVariation READ emitterYVariation WRITE setEmitterYVariation NOTIFY emitterYVariationChanged)

    Q_PROPERTY(bool emitting READ emitting WRITE setEmitting NOTIFY emittingChanged)

    Q_PROPERTY(int particlesPerSecond READ particlesPerSecond WRITE setParticlesPerSecond NOTIFY particlesPerSecondChanged)
    Q_PROPERTY(int particleDuration READ particleDuration WRITE setParticleDuration NOTIFY particleDurationChanged)

    Q_PROPERTY(qreal particleSize READ particleSize WRITE setParticleSize NOTIFY particleSizeChanged)
    Q_PROPERTY(qreal particleEndSize READ particleEndSize WRITE setParticleEndSize NOTIFY particleEndSizeChanged)
    Q_PROPERTY(qreal particleSizeVariation READ particleSizeVariation WRITE setParticleSizeVariation NOTIFY particleSizeVariationChanged)

    Q_PROPERTY(qreal xSpeed READ xSpeed WRITE setXSpeed NOTIFY xSpeedChanged)
    Q_PROPERTY(qreal ySpeed READ ySpeed WRITE setYSpeed NOTIFY ySpeedChanged)
    Q_PROPERTY(qreal xSpeedVariation READ xSpeedVariation WRITE setXSpeedVariation NOTIFY xSpeedVariationChanged)
    Q_PROPERTY(qreal ySpeedVariation READ ySpeedVariation WRITE setYSpeedVariation NOTIFY ySpeedVariationChanged)
    Q_PROPERTY(qreal speedFromMovement READ speedFromMovement WRITE setSpeedFromMovement NOTIFY speedFromMovementChanged)

    Q_PROPERTY(qreal xAccel READ xAccel WRITE setXAccel NOTIFY xAccelChanged)
    Q_PROPERTY(qreal yAccel READ yAccel WRITE setYAccel NOTIFY yAccelChanged)
    Q_PROPERTY(qreal xAccelVariation READ xAccelVariation WRITE setXAccelVariation NOTIFY xAccelVariationChanged)
    Q_PROPERTY(qreal yAccelVariation READ yAccelVariation WRITE setYAccelVariation NOTIFY yAccelVariationChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal colorVariation READ colorVariation WRITE setColorVariation NOTIFY colorVariationChanged)

    Q_PROPERTY(qreal additive READ additive WRITE setAdditive NOTIFY additiveChanged)

public:
    explicit TrailsEmitter(QObject* parent=0);
    virtual ~TrailsEmitter(){}

    bool isRunning() const { return m_running; }
    void setRunning(bool r);


    QUrl image() const { return m_image_name; }
    void setImage(const QUrl &image);

    QUrl colortable() const { return m_colortable_name; }
    void setColortable(const QUrl &table);

    int particlesPerSecond() const { return m_particles_per_second; }
    void setParticlesPerSecond(int pps);

    int particleDuration() const { return m_particle_duration; }
    void setParticleDuration(int dur);

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

    bool emitting() const { return m_emitting; }
    void setEmitting(bool emitting);


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


    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    qreal colorVariation() const { return m_color_variation; }
    void setColorVariation(qreal var);

    qreal additive() const { return m_additive; }
    void setAdditive(qreal additive);

    qreal renderOpacity() const { return m_render_opacity; }


    virtual Node* buildParticleNode();
    virtual void prepareNextFrame(uint timestamp);
    virtual void reset();

    virtual uint particleCount();
    virtual ParticleVertices* particles();
signals:
    void runningChanged();

    void imageChanged();
    void colortableChanged();

    void particlesPerSecondChanged();
    void particleDurationChanged();

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

    void colorChanged();
    void colorVariationChanged();
    void additiveChanged();
public slots:

protected:

private:

    bool m_running;
    bool m_do_reset;

    QUrl m_image_name;
    QUrl m_colortable_name;

    int m_particles_per_second;
    int m_particle_duration;

    qreal m_particle_size;
    qreal m_particle_end_size;
    qreal m_particle_size_variation;

    qreal m_emitter_x;
    qreal m_emitter_y;
    qreal m_emitter_x_variation;
    qreal m_emitter_y_variation;
    bool m_emitting;

    qreal m_x_speed;
    qreal m_y_speed;
    qreal m_x_speed_variation;
    qreal m_y_speed_variation;
    qreal m_speed_from_movement;

    qreal m_x_accel;
    qreal m_y_accel;
    qreal m_x_accel_variation;
    qreal m_y_accel_variation;

    QColor m_color;
    qreal m_color_variation;
    qreal m_additive;

    GeometryNode *m_node;
    ParticleTrailsMaterial *m_material;

    // derived values...
    int m_particle_count;
    int m_last_particle;

    QPointF m_last_emitter;
    QPointF m_last_last_emitter;
    QPointF m_last_last_last_emitter;

    bool m_reset_last;

    qreal m_last_timestamp;

    qreal m_render_opacity;
};

#endif // TRAILSEMITTER_H
