#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <QSGItem>
#include <QTime>
#include <QHash>

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

protected:
    Node *updatePaintNode(Node *, UpdatePaintNodeData *);

private slots:
    void countChanged();
private:
    void buildParticleNodes();
    void dataStore(ParticleData* data);
    int m_next_particle;
    int m_particle_count;
    bool m_running;
    bool m_do_reset;
    Node* m_node;
    QTime m_timestamp;
    QList<ParticleEmitter*> m_emitters;
    QList<ParticleAffector*> m_affectors;
    QList<Particle*> m_particles;
    QHash<int, ParticleData*> d;
};

//TODO: Clean up all this into ParticleData
struct Color4ub {
    uchar r;
    uchar g;
    uchar b;
    uchar a;
};
struct ParticleVertex {
    float x;
    float y;
    float tx;
    float ty;
    float t;
    float size;
    float endSize;
    float dt;
    float sx;
    float sy;
    float ax;
    float ay;
    float animIdx;
    float frameDuration;
    float frameCount;
    float animT;
    Color4ub color;
};

struct ParticleVertices {
    ParticleVertex v1;
    ParticleVertex v2;
    ParticleVertex v3;
    ParticleVertex v4;
};

class ParticleData{
public:
    ParticleData();

    ParticleVertices pv;
    Particle* p;
    ParticleEmitter* e;
    int emitterIndex;
    int systemIndex;
};

#endif // PARTICLESYSTEM_H

