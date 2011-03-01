#ifndef SPRITEIMAGE_H
#define SPRITEIMAGE_H

#include <QSGItem>
#include <QTime>

class QSGContext;
class SpriteState;
class SpriteEngine;
class GeometryNode;
class SpriteMaterial;
class SpriteImage : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    //###Fold SpriteEngine/Sprite back into sprites from a user perspective?
    Q_PROPERTY(SpriteState* sprite READ sprite WRITE setSprite NOTIFY spriteChanged)
    Q_PROPERTY(SpriteEngine* spriteEngine READ spriteEngine WRITE setSpriteEngine NOTIFY spriteEngineChanged)

public:
    explicit SpriteImage(QSGItem *parent = 0);

    SpriteState* sprite() const
    {
        return m_sprite;
    }

    SpriteEngine* spriteEngine() const
    {
        return m_spriteEngine;
    }

    bool running() const
    {
        return m_running;
    }

signals:

    void spriteChanged(SpriteState* arg);

    void spriteEngineChanged(SpriteEngine* arg);

    void runningChanged(bool arg);

public slots:

void setSprite(SpriteState* arg)
{
    if (m_sprite != arg) {
        m_sprite = arg;
        emit spriteChanged(arg);
    }
}

void setSpriteEngine(SpriteEngine* arg)
{
    if (m_spriteEngine != arg) {
        m_spriteEngine = arg;
        emit spriteEngineChanged(arg);
    }
}

void setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        emit runningChanged(arg);
    }
}

protected:
    void reset();
    Node *updatePaintNode(Node *, UpdatePaintNodeData *);
private:
    void prepareNextFrame();
    GeometryNode* buildNode();
    bool buildParticleTexture(QSGContext *sg);
    GeometryNode *m_node;
    SpriteMaterial *m_material;
    SpriteState* m_sprite;
    SpriteEngine* m_spriteEngine;
    QTime m_timestamp;
    int m_maxFrames;
    bool m_pleaseReset;
    bool m_running;
};

#endif // SPRITEIMAGE_H
