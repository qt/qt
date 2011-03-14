#ifndef SWARMAFFECTOR_H
#define SWARMAFFECTOR_H
#include "particleaffector.h"
#include <QDeclarativeListProperty>

class ParticleType;

class SwarmAffector : public ParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal strength READ strength WRITE setStrength NOTIFY strengthChanged)
    Q_PROPERTY(QStringList leaders READ leaders WRITE setLeaders NOTIFY leadersChanged)
public:
    explicit SwarmAffector(QSGItem *parent = 0);
    virtual bool affectParticle(ParticleData *d, qreal dt);
    virtual void reset(int systemIdx);

    qreal strength() const
    {
        return m_strength;
    }

    QStringList leaders() const
    {
        return m_leaders;
    }

signals:

    void strengthChanged(qreal arg);

    void leadersChanged(QStringList arg);

public slots:

void setStrength(qreal arg)
{
    if (m_strength != arg) {
        m_strength = arg;
        emit strengthChanged(arg);
    }
}

void setLeaders(QStringList arg)
{
    if (m_leaders != arg) {
        m_leaders = arg;
        emit leadersChanged(arg);
    }
}

private:
    void ensureInit();
    void mapUpdate(int idx, qreal strength);
    QVector<QPointF> m_lastPos;
    qreal m_strength;
    bool m_inited;
    QStringList m_leaders;
    QSet<int> m_leadGroups;
private slots:
    void updateGroupList();
};

#endif // SWARMAFFECTOR_H
