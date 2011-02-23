#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <QObject>

class ParticleSystem;
struct ParticleVertices;
class Node;

class ParticleEmitter : public QObject
{
    Q_OBJECT
public:
    explicit ParticleEmitter(QObject *parent = 0);
    virtual Node* buildParticleNode();
    virtual void prepareNextFrame(uint timestamp);
    virtual void reset();

    virtual uint particleCount();
    virtual ParticleVertices* particles();


    ParticleSystem* m_system;//###Needs to be set from classless function?
signals:

public slots:

protected:
};

#endif // PARTICLEEMITTER_H
