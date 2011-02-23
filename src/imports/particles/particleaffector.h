#ifndef PARTICLEAFFECTOR_H
#define PARTICLEAFFECTOR_H

#include <QObject>
class ParticleVertices;
class SpriteEmitter;

class ParticleAffector : public QObject
{
    Q_OBJECT
public:
    explicit ParticleAffector(QObject *parent = 0);
    //###Pass the system in? Or register it? Or becomes moot once everything's moved out?
    virtual void affect(ParticleVertices *p, int idx, qreal dt, QObject* emitter);
    virtual void reset(int idx);//As some store their own data per idx
signals:

public slots:

};

#endif // PARTICLEAFFECTOR_H
