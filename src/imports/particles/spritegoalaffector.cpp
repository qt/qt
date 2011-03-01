#include "spritegoalaffector.h"
#include "spriteparticle.h"
#include "spriteengine.h"
#include "spritestate.h"

SpriteGoalAffector::SpriteGoalAffector(QObject *parent) :
    ParticleAffector(parent), m_goalIdx(-1)
{
}

void SpriteGoalAffector::updateStateIndex(SpriteEngine* e)
{
    m_lastEngine = e;
    for(int i=0; i<e->stateCount(); i++){
        if(e->state(i)->name() == m_goalState){
            m_goalIdx = i;
            return;
        }
    }
    m_goalIdx = -1;//Can't find it
}

void SpriteGoalAffector::setGoalState(QString arg)
{
    if (m_goalState != arg) {
        m_goalState = arg;
        emit goalStateChanged(arg);
        if(m_goalState.isEmpty())
            m_goalIdx = -1;
        else
            m_goalIdx = -2;
    }
}

bool SpriteGoalAffector::affect(ParticleData *d, qreal dt)
{
    Q_UNUSED(dt);
    SpriteEngine *engine = 0;
    if(qobject_cast<SpriteParticle*>(d->p))
        engine = qobject_cast<SpriteParticle*>(d->p)->spriteEngine();
    if(!engine)
        return false;

    if(m_goalIdx == -2 || engine != m_lastEngine)
        updateStateIndex(engine);
    engine->setGoal(m_goalIdx, d->systemIndex);
    return false; //Doesn't affect particle data
}
