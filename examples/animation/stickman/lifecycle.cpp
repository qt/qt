#include "lifecycle.h"
#include "stickman.h"
#include "node.h"
#include "animation.h"
#include "graphicsview.h"

#include <QtCore>
#include <QtGui>

class KeyPressTransition: public QSignalTransition
{
public:
    KeyPressTransition(GraphicsView *receiver, Qt::Key key)
        : QSignalTransition(receiver, SIGNAL(keyPressed(int))), m_key(key)
    {
    }
    KeyPressTransition(GraphicsView *receiver, Qt::Key key, QAbstractState *target)
        : QSignalTransition(receiver, SIGNAL(keyPressed(int)), QList<QAbstractState*>() << target), m_key(key)
    {
    }

    virtual bool eventTest(QEvent *e) const
    {
        if (QSignalTransition::eventTest(e)) {
            QVariant key = static_cast<QSignalEvent*>(e)->arguments().at(0);
            return (key.toInt() == int(m_key));
        } 

        return false;
    }
private:
    Qt::Key m_key;
};

class LightningStrikesTransition: public QEventTransition
{
public:
    LightningStrikesTransition(QAbstractState *target)
        : QEventTransition(this, QEvent::Timer, QList<QAbstractState*>() << target)
    {
        qsrand((uint)QDateTime::currentDateTime().toTime_t());
        startTimer(1000);
    }

    virtual bool eventTest(QEvent *e) const
    {
        return QEventTransition::eventTest(e) && ((qrand() % 50) == 0);
    }
};

LifeCycle::LifeCycle(StickMan *stickMan, GraphicsView *keyReceiver)
    : m_stickMan(stickMan), m_keyReceiver(keyReceiver)
{
    // Create animation group to be used for all transitions
    m_animationGroup = new QParallelAnimationGroup();
    const int stickManNodeCount = m_stickMan->nodeCount();
    for (int i=0; i<stickManNodeCount; ++i) {
        QPropertyAnimation *pa = new QPropertyAnimation(m_stickMan->node(i), "position");
        m_animationGroup->addAnimation(pa);    
    }

    // Set up intial state graph
    m_machine = new QStateMachine();
    m_machine->setGlobalRestorePolicy(QState::RestoreProperties);

    m_alive = new QState(m_machine->rootState());
    m_alive->setObjectName("alive");
    
    // Make it blink when lightning strikes before entering dead animation
    QState *lightningBlink = new QState(m_machine->rootState());    
    lightningBlink->setRestorePolicy(QState::DoNotRestoreProperties);
    lightningBlink->setPropertyOnEntry(m_stickMan->scene(), "backgroundBrush", Qt::white);
    lightningBlink->setPropertyOnEntry(m_stickMan, "penColor", Qt::black);
    lightningBlink->setPropertyOnEntry(m_stickMan, "fillColor", Qt::white);
    lightningBlink->setPropertyOnEntry(m_stickMan, "isDead", true);
  
    m_dead = new QState(m_machine->rootState());
    m_dead->setRestorePolicy(QState::DoNotRestoreProperties);
    m_dead->setPropertyOnEntry(m_stickMan->scene(), "backgroundBrush", Qt::black);
    m_dead->setPropertyOnEntry(m_stickMan, "penColor", Qt::white);
    m_dead->setPropertyOnEntry(m_stickMan, "fillColor", Qt::black);
    m_dead->setObjectName("dead");
           
    // Idle state (sets no properties)
    m_idle = new QState(m_alive);
    m_idle->setObjectName("idle");
    m_alive->setInitialState(m_idle);

    // Lightning strikes at random
    m_alive->addTransition(new LightningStrikesTransition(lightningBlink));
    m_alive->addTransition(new KeyPressTransition(m_keyReceiver, Qt::Key_L, lightningBlink));
    connectByAnimation(m_machine->rootState(), lightningBlink, m_dead);

    m_machine->setInitialState(m_alive);
}

void LifeCycle::setResetKey(Qt::Key resetKey)
{
    // When resetKey is pressed, enter the idle state and do a restoration animation
    // (requires no animation pointer, since no property is being set in the idle state)
    m_alive->addAnimatedTransition(new KeyPressTransition(m_keyReceiver, resetKey, m_idle));
}

void LifeCycle::setDeathAnimation(const QString &fileName)
{
    QState *deathAnimation = makeState(m_dead, fileName);
    m_dead->setInitialState(deathAnimation);
}

void LifeCycle::start()
{
    m_machine->start();
}

void LifeCycle::connectByAnimation(QState *parentState, 
                                   QState *s1, QAbstractState *s2, 
                                   QAbstractTransition *transition)
{
    QAnimationState *animationState = new QAnimationState(m_animationGroup, parentState);
    
    if (transition == 0)
        s1->addTransition(animationState);
    else {
        transition->setTargetStates(QList<QAbstractState*>() << animationState);
        s1->addTransition(transition);
    }

    animationState->addFinishedTransition(s2);
}

void LifeCycle::addActivity(const QString &fileName, Qt::Key key)
{
    QState *state = makeState(m_alive, fileName);
    connectByAnimation(m_alive, m_alive, state, new KeyPressTransition(m_keyReceiver, key));
}

QState *LifeCycle::makeState(QState *parentState, const QString &animationFileName)
{
    QState *topLevel = new QState(parentState);
    
    Animation animation;
    {
        QFile file(animationFileName);
        if (file.open(QIODevice::ReadOnly))
            animation.load(&file);
    }

    const int frameCount = animation.totalFrames();
    QState *previousState = 0;
    for (int i=0; i<frameCount; ++i) {
        QState *frameState = new QState(topLevel);
        
        animation.setCurrentFrame(i);
        const int nodeCount = animation.nodeCount();
        for (int j=0; j<nodeCount; ++j)
            frameState->setPropertyOnEntry(m_stickMan->node(j), "position", animation.nodePos(j));

        if (previousState == 0)
            topLevel->setInitialState(frameState);
        else
            connectByAnimation(topLevel, previousState, frameState);
        previousState = frameState;
    }

    // Loop
    connectByAnimation(topLevel, previousState, topLevel->initialState());

    return topLevel;

}

LifeCycle::~LifeCycle()
{
    delete m_machine;
    delete m_animationGroup;
}
