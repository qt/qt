#ifndef WANDERAFFECTOR_H
#define WANDERAFFECTOR_H
#include <QHash>
#include "particleaffector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class SpriteEmitter;

struct WanderData{
    qreal x_vel;
    qreal y_vel;
    qreal x_peak;
    qreal x_var;
    qreal y_peak;
    qreal y_var;
};

class WanderAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal xVariance READ xVariance WRITE setXVariance NOTIFY xVarianceChanged)
    Q_PROPERTY(qreal yVariance READ yVariance WRITE setYVariance NOTIFY yVarianceChanged)
    Q_PROPERTY(qreal pace READ pace WRITE setPace NOTIFY paceChanged)

public:
    explicit WanderAffector(QSGItem *parent = 0);
    ~WanderAffector();
    virtual void reset(int systemIdx);

    qreal xVariance() const
    {
        return m_xVariance;
    }

    qreal yVariance() const
    {
        return m_yVariance;
    }

    qreal pace() const
    {
        return m_pace;
    }
protected:
    virtual bool affectParticle(ParticleData *d, qreal dt);
signals:

    void xVarianceChanged(qreal arg);

    void yVarianceChanged(qreal arg);

    void paceChanged(qreal arg);

public slots:
void setXVariance(qreal arg)
{
    if (m_xVariance != arg) {
        m_xVariance = arg;
        emit xVarianceChanged(arg);
    }
}

void setYVariance(qreal arg)
{
    if (m_yVariance != arg) {
        m_yVariance = arg;
        emit yVarianceChanged(arg);
    }
}

void setPace(qreal arg)
{
    if (m_pace != arg) {
        m_pace = arg;
        emit paceChanged(arg);
    }
}

private:
    WanderData* getData(int idx);
    QHash<int, WanderData*> m_wanderData;
    qreal m_xVariance;
    qreal m_yVariance;
    qreal m_pace;
};

QT_END_NAMESPACE
#endif // WANDERAFFECTOR_H
