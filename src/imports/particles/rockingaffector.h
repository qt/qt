#ifndef ROCKINGAFFECTOR_H
#define ROCKINGAFFECTOR_H
#include "particleaffector.h"
#include <QHash>

class SpriteEmitter;
/*
  The inputs seem quite sensitive and the effect seems quite limited.
  it's not clear if this class should stick around, or be kicked out.

  Especially since proper documentation would require a lot of diagrams
*/
struct RockerData{
    qreal peakAngle;//angles in radians
    qreal curAngle;
    qreal dir;
    bool initialAngleSet;
};

class RockingAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal initialAngle READ initialAngle WRITE setInitialAngle NOTIFY initialAngleChanged)//degrees
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)//degrees
    Q_PROPERTY(qreal angleVariance READ angleVariance WRITE setAngleVariance NOTIFY angleVarianceChanged)//degrees
    Q_PROPERTY(qreal pace READ pace WRITE setPace NOTIFY paceChanged)
    Q_PROPERTY(qreal minSpeed READ minSpeed WRITE setMinSpeed NOTIFY minSpeedChanged)//degrees per second
public:
    explicit RockingAffector(QObject *parent = 0);
    ~RockingAffector();
    virtual void affect(ParticleVertices *p, int idx, qreal dt, QObject*);
    virtual void reset(int idx);
    qreal angle() const
    {
        return m_angle;
    }

    qreal angleVariance() const
    {
        return m_angleVariance;
    }

    qreal pace() const
    {
        return m_pace;
    }

    qreal minSpeed() const
    {
        return m_minSpeed;
    }

    qreal initialAngle() const
    {
        return m_initialAngle;
    }

signals:

    void angleChanged(qreal arg);

    void angleVarianceChanged(qreal arg);

    void paceChanged(qreal arg);

    void minSpeedChanged(qreal arg);

    void initialAngleChanged(qreal arg);

public slots:
void setAngle(qreal arg)
{
    if (m_angle != arg) {
        m_angle = arg;
        emit angleChanged(arg);
    }
}

void setAngleVariance(qreal arg)
{
    if (m_angleVariance != arg) {
        m_angleVariance = arg;
        emit angleVarianceChanged(arg);
    }
}

void setPace(qreal arg)
{
    if (m_pace != arg) {
        m_pace = arg;
        emit paceChanged(arg);
    }
}

void setMinSpeed(qreal arg)
{
    if (m_minSpeed != arg) {
        m_minSpeed = arg;
        emit minSpeedChanged(arg);
    }
}

void setInitialAngle(qreal arg)
{
    if (m_initialAngle != arg) {
        m_initialAngle = arg;
        emit initialAngleChanged(arg);
    }
}

private:
    QHash<int, RockerData*> m_rockerData;
    RockerData* getData(int idx);
    qreal m_angle;
    qreal m_angleVariance;
    qreal m_pace;
    qreal m_minSpeed;
    qreal m_initialAngle;
};

#endif // ROCKINGAFFECTOR_H
