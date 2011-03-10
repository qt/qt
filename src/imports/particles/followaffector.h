#ifndef FOLLOWAFFECTOR_H
#define FOLLOWAFFECTOR_H
#include "particleaffector.h"

class FollowEmitter;
class FollowAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(FollowEmitter* emitter READ emitter WRITE setEmitter NOTIFY emitterChanged)
public:
    explicit FollowAffector(QObject *parent = 0);
    virtual bool affect(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);
    FollowEmitter* emitter() const
    {
        return m_emitter;
    }

signals:

    void emitterChanged(FollowEmitter* arg);

public slots:
void setEmitter(FollowEmitter* arg)
{
    if (m_emitter != arg) {
        m_emitter = arg;
        emit emitterChanged(arg);
    }
}

private:
FollowEmitter* m_emitter;
};

#endif // FOLLOWAFFECTOR_H
