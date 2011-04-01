#ifndef SPRITEIMAGE_H
#define SPRITEIMAGE_H

#include <QSGItem>
#include <QTime>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGContext;
class SpriteState;
class SpriteEngine;
class QSGGeometryNode;
class SpriteMaterial;
class SpriteImage : public QSGItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    //###try to share similar spriteEngines for less overhead?
    Q_PROPERTY(QDeclarativeListProperty<SpriteState> sprites READ sprites)
    Q_CLASSINFO("DefaultProperty", "sprites")

public:
    explicit SpriteImage(QSGItem *parent = 0);

    QDeclarativeListProperty<SpriteState> sprites();

    bool running() const
    {
        return m_running;
    }

signals:


    void runningChanged(bool arg);

public slots:

void setRunning(bool arg)
{
    if (m_running != arg) {
        m_running = arg;
        emit runningChanged(arg);
    }
}

private slots:
    void createEngine();
protected:
    void reset();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
private:
    void prepareNextFrame();
    QSGGeometryNode* buildNode();
    QSGGeometryNode *m_node;
    SpriteMaterial *m_material;
    QList<SpriteState*> m_sprites;
    SpriteEngine* m_spriteEngine;
    QTime m_timestamp;
    int m_maxFrames;
    bool m_pleaseReset;
    bool m_running;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // SPRITEIMAGE_H
