#ifndef SPRITEGOALAFFECTOR_H
#define SPRITEGOALAFFECTOR_H
#include "particleaffector.h"
class SpriteEngine;

class SpriteGoalAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(QString goalState READ goalState WRITE setGoalState NOTIFY goalStateChanged)
public:
    explicit SpriteGoalAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);

    QString goalState() const
    {
        return m_goalState;
    }

signals:

    void goalStateChanged(QString arg);

public slots:

    void setGoalState(QString arg);

private:
    void updateStateIndex(SpriteEngine* e);
    QString m_goalState;
    int m_goalIdx;
    SpriteEngine* m_lastEngine;
};

#endif // SPRITEGOALAFFECTOR_H
