#ifndef LIFECYCLE_H
#define LIFECYCLE_H

#include <Qt>

class StickMan;
class QStateMachine;
class QAnimationGroup;
class QState;
class QAbstractState;
class QAbstractTransition;
class GraphicsView;
class LifeCycle
{
public:
    LifeCycle(StickMan *stickMan, GraphicsView *keyEventReceiver);
    ~LifeCycle();

    void setDeathAnimation(const QString &fileName);
    void setResetKey(Qt::Key key);
    void addActivity(const QString &fileName, Qt::Key key);    

    void start();

private:
    void connectByAnimation(QState *s1, QAbstractState *s2, 
                            QAbstractTransition *transition = 0);
    QState *makeState(QState *parentState, const QString &animationFileName);

    StickMan *m_stickMan;
    QStateMachine *m_machine;
    QAnimationGroup *m_animationGroup;
    GraphicsView *m_keyReceiver;

    QState *m_alive;
    QState *m_dead;
    QState *m_idle;
};

#endif
