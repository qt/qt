#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QSGItem>
#include <QTime>
#include <QVector>

class ParticleAffector;
class ParticleEmitter;
class Particle;
class ParticleData;

class ParticleSystem : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(QDeclarativeListProperty<ParticleAffector> affectors READ affectors)
    Q_PROPERTY(QDeclarativeListProperty<ParticleEmitter> emitters READ emitters)
    Q_PROPERTY(QDeclarativeListProperty<Particle> particles READ particles)

public:
    explicit ParticleSystem(QSGItem *parent = 0);

bool isRunning() const
{
    return m_running;
}

QDeclarativeListProperty<ParticleEmitter> emitters();

QDeclarativeListProperty<Particle> particles()
{
    return QDeclarativeListProperty<Particle>(this, m_particles);
}

QDeclarativeListProperty<ParticleAffector> affectors()
{
    return QDeclarativeListProperty<ParticleAffector>(this, m_affectors);
}
signals:

void runningChanged(bool arg);

public slots:
void pleaseUpdate(){if(this)update();}//XXX
void reset();
void prepareNextFrame();
void setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        emit runningChanged(arg);
    }
}

void emitParticle(ParticleData* p);
ParticleData* newDatum();

protected:
    Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private slots:
    void countChanged();
private:
    void buildParticleNodes();
    int m_next_particle;
    int m_particle_count;
    bool m_running;
    bool m_do_reset;
    Node* m_node;
    QTime m_timestamp;
    QList<ParticleEmitter*> m_emitters;
    QList<ParticleAffector*> m_affectors;
    QList<Particle*> m_particles;
    QVector<ParticleData*> d;
};

//TODO: Clean up all this into ParticleData

struct ParticleVertex {
    float x;
    float y;
    float t;
    float size;
    float endSize;
    float dt;
    float sx;
    float sy;
    float ax;
    float ay;
};

class ParticleData{
public:
    ParticleData();

    ParticleVertex pv;
    Particle* p;
    ParticleEmitter* e;
    int emitterIndex;
    int systemIndex;
};

#endif // PARTICLESYSTEM_H

