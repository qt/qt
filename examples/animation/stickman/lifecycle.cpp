#include "lifecycle.h"
#include "stickman.h"
#include "node.h"
#include "animation.h"
#include "graphicsview.h"

#include <QtCore>
#include <QtGui>
#if defined(QT_EXPERIMENTAL_SOLUTION)
#include "qstatemachine.h"
#include "qstate.h"
#include "qeventtransition.h"
#include "qsignaltransition.h"
#include "qsignalevent.h"
#include "qpropertyanimation.h"
#include "qparallelanimationgroup.h"
#endif

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
    lightningBlink->assignProperty(m_stickMan->scene(), "backgroundBrush", Qt::white);
    lightningBlink->assignProperty(m_stickMan, "penColor", Qt::black);
    lightningBlink->assignProperty(m_stickMan, "fillColor", Qt::white);
    lightningBlink->assignProperty(m_stickMan, "isDead", true);
    
    QTimer *timer = new QTimer(lightningBlink);
    timer->setSingleShot(true);
    timer->setInterval(100);
    QObject::connect(lightningBlink, SIGNAL(entered()), timer, SLOT(start()));
    QObject::connect(lightningBlink, SIGNAL(exited()), timer, SLOT(stop()));
  
    m_dead = new QState(m_machine->rootState());
    m_dead->setRestorePolicy(QState::DoNotRestoreProperties);
    m_dead->assignProperty(m_stickMan->scene(), "backgroundBrush", Qt::black);
    m_dead->assignProperty(m_stickMan, "penColor", Qt::white);
    m_dead->assignProperty(m_stickMan, "fillColor", Qt::black);
    m_dead->setObjectName("dead");
           
    // Idle state (sets no properties)
    m_idle = new QState(m_alive);
    m_idle->setObjectName("idle");

    m_alive->setInitialState(m_idle);

    // Lightning strikes at random
    m_alive->addTransition(new LightningStrikesTransition(lightningBlink));
    //m_alive->addTransition(new KeyPressTransition(m_keyReceiver, Qt::Key_L, lightningBlink));
    connectByAnimation(lightningBlink, m_dead, new QSignalTransition(timer, SIGNAL(timeout())));

    m_machine->setInitialState(m_alive);
}

void LifeCycle::setResetKey(Qt::Key resetKey)
{
    // When resetKey is pressed, enter the idle state and do a restoration animation
    // (requires no animation pointer, since no property is being set in the idle state)
    KeyPressTransition *trans = new KeyPressTransition(m_keyReceiver, resetKey, m_idle);
    trans->addAnimation(m_animationGroup);
    m_alive->addTransition(trans);
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

void LifeCycle::connectByAnimation(QState *s1, QAbstractState *s2, 
                                   QAbstractTransition *transition)
{
    if (transition == 0) {
        transition = s1->addTransition(s2);
    } else {
        transition->setTargetState(s2);
        s1->addTransition(transition);
    }
    transition->addAnimation(m_animationGroup);
}

void LifeCycle::addActivity(const QString &fileName, Qt::Key key)
{
    QState *state = makeState(m_alive, fileName);
    connectByAnimation(m_alive, state, new KeyPressTransition(m_keyReceiver, key));
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
        frameState->setObjectName(QString::fromLatin1("frame %0").arg(i));

        animation.setCurrentFrame(i);
        const int nodeCount = animation.nodeCount();
        for (int j=0; j<nodeCount; ++j)
            frameState->assignProperty(m_stickMan->node(j), "position", animation.nodePos(j));

        if (previousState == 0) {
            topLevel->setInitialState(frameState);
        } else {
            connectByAnimation(previousState, frameState, 
                new QSignalTransition(m_machine, SIGNAL(animationsFinished())));
        }
        previousState = frameState;
    }

    // Loop
    connectByAnimation(previousState, topLevel->initialState(), 
        new QSignalTransition(m_machine, SIGNAL(animationsFinished())));

    return topLevel;

}

LifeCycle::~LifeCycle()
{
    delete m_machine;
    delete m_animationGroup;
}
