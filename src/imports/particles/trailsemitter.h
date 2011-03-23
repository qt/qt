#ifndef TRAILSEMITTER_H
#define TRAILSEMITTER_H

#include <QtCore>
#include <QtGui>

#include "particleemitter.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class ParticleTrailsMaterial;
class GeometryNode;

class TrailsEmitter : public ParticleEmitter
{
    Q_OBJECT

    Q_PROPERTY(qreal speedFromMovement READ speedFromMovement WRITE setSpeedFromMovement NOTIFY speedFromMovementChanged)

public:
    explicit TrailsEmitter(QSGItem* parent=0);
    virtual ~TrailsEmitter(){}
    virtual void emitWindow(int timeStamp);


    qreal speedFromMovement() const { return m_speed_from_movement; }
    void setSpeedFromMovement(qreal s);

    qreal renderOpacity() const { return m_render_opacity; }

signals:

    void speedFromMovementChanged();

public slots:
public:
    virtual void reset();
protected:

private:

    qreal m_speed_from_movement;

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

QT_END_NAMESPACE
#endif // TRAILSEMITTER_H
