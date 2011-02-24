#ifndef SPRITEPARTICLE_H
#define SPRITEPARTICLE_H
#include "particle.h"
class SpriteState;
class GeometryNode;
class QSGContext;
class SpriteParticlesMaterial;

class SpriteParticle : public Particle
{
    Q_OBJECT
    Q_PROPERTY(int particleDuration READ particleDuration WRITE setParticleDuration NOTIFY particleDurationChanged)

    Q_PROPERTY(QDeclarativeListProperty<SpriteState> states READ states)
    Q_PROPERTY(QString goalState READ goalState WRITE setGoalState NOTIFY goalStateChanged)
    Q_PROPERTY(int frameDuration READ frameDuration WRITE setFrameDuration NOTIFY frameDurationChanged)

public:
    explicit SpriteParticle(QObject *parent = 0);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);

    int particleDuration() const { return m_particle_duration; }
    void setParticleDuration(int dur);
    int frames() const
    {
        return m_frames;
    }

    int frameDuration() const
    {
        return m_frameDuration;
    }

    QDeclarativeListProperty<SpriteState> states()
    {
        return QDeclarativeListProperty<SpriteState>(this, m_states);
    }

    QString goalState() const
    {
        return m_goalState;
    }
signals:
    void particleDurationChanged();
    void framesChanged(int arg);

    void frameDurationChanged(int arg);

    void goalStateChanged(QString arg);

public slots:

    void setFrames(int arg)
    {
        if (m_frames != arg) {
            m_frames = arg;
            emit framesChanged(arg);
        }
    }

    void setFrameDuration(int arg)
    {
        if (m_frameDuration != arg) {
            m_frameDuration = arg;
            emit frameDurationChanged(arg);
        }
    }

    void setGoalState(QString arg)
    {
        if (m_goalState != arg) {
            m_goalState = arg;
            emit goalStateChanged(arg);
        }
    }

private:
    int m_frames;
    int m_frameDuration;
    QList<SpriteState*> m_states;

    GeometryNode *m_node;
    SpriteParticlesMaterial *m_material;

    int m_particle_count;
    int m_particle_duration;
    int m_last_particle;
    QTime m_timestamp;
    int m_maxFrames;

    bool buildParticleTexture(QSGContext *sg);

    //TODO: Need to move to affector and engine-like
    void addToUpdateList(uint t, int idx);
    int goalSeek(int curState, int dist=-1);
    QList<QPair<uint, QList<int> > > m_stateUpdates;//### This could be done faster
    QString m_goalState;
};

#endif // SPRITEPARTICLE_H
