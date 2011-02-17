#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
class ParticleVertices;

class ParticleAffector : public QObject
{
    Q_OBJECT
public:
    explicit ParticleAffector(QObject *parent = 0);
    virtual void affect(ParticleVertices *p, int idx, qreal dt);
signals:

public slots:

};

#endif // PARTICLEAFFECTOR_H
