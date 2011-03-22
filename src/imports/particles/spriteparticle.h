#ifndef SPRITEPARTICLE_H
#define SPRITEPARTICLE_H
#include "particle.h"
#include <QDeclarativeListProperty>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class SpriteState;
class SpriteEngine;
class GeometryNode;
class SpriteParticlesMaterial;
class SpriteParticleVertex;

class SpriteParticle : public ParticleType
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<SpriteState> sprites READ sprites)
    Q_CLASSINFO("DefaultProperty", "sprites")
public:
    explicit SpriteParticle(QSGItem *parent = 0);
    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);

    QDeclarativeListProperty<SpriteState> sprites();
    SpriteEngine* spriteEngine() {return m_spriteEngine;}
signals:

public slots:
protected:
    Node *updatePaintNode(Node *, UpdatePaintNodeData *);
    void reset();
    void prepareNextFrame();
    GeometryNode* buildParticleNode();
private slots:
    void createEngine();
private:
    GeometryNode *m_node;
    SpriteParticlesMaterial *m_material;

    int m_particle_duration;
    int m_last_particle;
    QTime m_timestamp;

    QList<SpriteState*> m_sprites;
    SpriteEngine* m_spriteEngine;

    void vertexCopy(SpriteParticleVertex &b,const ParticleVertex& a);
};

QT_END_NAMESPACE
#endif // SPRITEPARTICLE_H
