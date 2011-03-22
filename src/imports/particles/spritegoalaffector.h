#ifndef SPRITEGOALAFFECTOR_H
#define SPRITEGOALAFFECTOR_H
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class SpriteEngine;

class SpriteGoalAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(QString goalState READ goalState WRITE setGoalState NOTIFY goalStateChanged)
    Q_PROPERTY(bool jump READ jump WRITE setJump NOTIFY jumpChanged)
public:
    explicit SpriteGoalAffector(QSGItem *parent = 0);

    QString goalState() const
    {
        return m_goalState;
    }

    bool jump() const
    {
        return m_jump;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void goalStateChanged(QString arg);

    void jumpChanged(bool arg);

    void affected(const QPointF &pos);
public slots:

void setGoalState(QString arg);

void setJump(bool arg)
{
    if (m_jump != arg) {
        m_jump = arg;
        emit jumpChanged(arg);
    }
}

private:
    void updateStateIndex(SpriteEngine* e);
    QString m_goalState;
    int m_goalIdx;
    SpriteEngine* m_lastEngine;
    bool m_jump;
};

QT_END_NAMESPACE
#endif // SPRITEGOALAFFECTOR_H
