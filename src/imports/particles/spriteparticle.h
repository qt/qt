#ifndef SPRITEPARTICLE_H
#define SPRITEPARTICLE_H
#include "particle.h"
class SpriteState;
class SpriteEngine;
class GeometryNode;
class QSGContext;
class SpriteParticlesMaterial;

class SpriteParticle : public Particle
{
    Q_OBJECT
    Q_PROPERTY(SpriteState* sprite READ sprite WRITE setSprite NOTIFY spriteChanged)
    Q_PROPERTY(SpriteEngine* spriteEngine READ spriteEngine WRITE setSpriteEngine NOTIFY spriteEngineChanged)

public:
    explicit SpriteParticle(QObject *parent = 0);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);

    SpriteEngine* spriteEngine() const
    {
        return m_spriteEngine;
    }

    SpriteState* sprite() const
    {
        return m_sprite;
    }

signals:
    void spriteEngineChanged(SpriteEngine* arg);

    void spriteChanged(SpriteState* arg);

public slots:

    void setSpriteEngine(SpriteEngine* arg)
    {
        if (m_spriteEngine != arg) {
            m_spriteEngine = arg;
            emit spriteEngineChanged(arg);
        }
    }

    void setSprite(SpriteState* arg)
    {
        if (m_sprite != arg) {
            m_sprite = arg;
            emit spriteChanged(arg);
        }
    }

private:
    GeometryNode *m_node;
    SpriteParticlesMaterial *m_material;

    int m_particle_count;
    int m_particle_duration;
    int m_last_particle;
    QTime m_timestamp;
    int m_maxFrames;

    bool buildParticleTexture(QSGContext *sg);

    SpriteState* m_sprite;
    SpriteEngine* m_spriteEngine;
};

#endif // SPRITEPARTICLE_H
