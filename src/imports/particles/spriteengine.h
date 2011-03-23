#ifndef SPRITEENGINE_H
#define SPRITEENGINE_H

#include <QObject>
#include <QVector>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QDeclarativeListProperty>
#include <QImage>
#include <QPair>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class SpriteState;

class SpriteEngine : public QObject
{
    Q_OBJECT
    //TODO: Optimize single sprite case
    Q_PROPERTY(QDeclarativeListProperty<SpriteState> sprites READ sprites)
    Q_PROPERTY(QString globalGoal READ globalGoal WRITE setGlobalGoal NOTIFY globalGoalChanged)
public:
    explicit SpriteEngine(QObject *parent = 0);
    SpriteEngine(QList<SpriteState*> sprites, QObject *parent=0);
    ~SpriteEngine();

    QDeclarativeListProperty<SpriteState> sprites()
    {
        return QDeclarativeListProperty<SpriteState>(this, m_states);
    }
    QString globalGoal() const
    {
        return m_globalGoal;
    }

    int count() const {return m_sprites.count();}
    void setCount(int c);

    int spriteState(int sprite=0) {return m_sprites[sprite];}
    int spriteStart(int sprite=0) {return m_startTimes[sprite];}
    int stateIndex(SpriteState* s){return m_states.indexOf(s);}
    SpriteState* state(int idx){return m_states[idx];}
    int stateCount() {return m_states.count();}
    int maxFrames();

    void setGoal(int state, int sprite=0, bool jump=false);
    QImage assembledImage();

    void startSprite(int index=0);

signals:

    void globalGoalChanged(QString arg);

public slots:
    void setGlobalGoal(QString arg)
    {
        if (m_globalGoal != arg) {
            m_globalGoal = arg;
            emit globalGoalChanged(arg);
        }
    }

    uint updateSprites(uint time);

private:
    void restartSprite(int sprite);
    void addToUpdateList(uint t, int idx);
    int goalSeek(int curState, int spriteIdx, int dist=-1);
    QList<SpriteState*> m_states;
    QVector<int> m_sprites;//int is the index in m_states of the current state
    QVector<int> m_goals;
    QVector<int> m_startTimes;
    QList<QPair<uint, QList<int> > > m_stateUpdates;//### This could be done faster

    QTime m_advanceTime;
    uint m_timeOffset;
    QString m_globalGoal;
    int m_maxFrames;
};

//Common use is to have your own list property which is transparently an engine
inline void spriteAppend(QDeclarativeListProperty<SpriteState> *p, SpriteState* s)
{
    reinterpret_cast<QList<SpriteState *> *>(p->data)->append(s);
    p->object->metaObject()->invokeMethod(p->object, "createEngine");
}

inline SpriteState* spriteAt(QDeclarativeListProperty<SpriteState> *p, int idx)
{
    return reinterpret_cast<QList<SpriteState *> *>(p->data)->at(idx);
}

inline void spriteClear(QDeclarativeListProperty<SpriteState> *p)
{
    reinterpret_cast<QList<SpriteState *> *>(p->data)->clear();
    p->object->metaObject()->invokeMethod(p->object, "createEngine");
}

inline int spriteCount(QDeclarativeListProperty<SpriteState> *p)
{
    return reinterpret_cast<QList<SpriteState *> *>(p->data)->count();
}


QT_END_NAMESPACE
#endif // SPRITEENGINE_H
