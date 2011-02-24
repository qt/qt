#ifndef ZONEAFFECTOR_H
#define ZONEAFFECTOR_H
#include "particleaffector.h"

class ZoneAffector : public ParticleAffector
{
    Q_OBJECT
    //TODO: Can we get anchors in here? consider becoming an un-parented QSGItem?
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged);
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged);
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged);
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged);
    Q_PROPERTY(ParticleAffector* affector READ affector)
public:
    explicit ZoneAffector(QObject *parent = 0);

    virtual bool affect(ParticleData *d, qreal dt);
    int x() const
    {
        return m_x;
    }

    int y() const
    {
        return m_y;
    }

    int width() const
    {
        return m_width;
    }

    int height() const
    {
        return m_height;
    }

    ParticleAffector* affector() const
    {
        return m_affector;
    }

signals:

    void xChanged(int arg);

    void yChanged(int arg);

    void widthChanged(int arg);

    void heightChanged(int arg);

public slots:

void setX(int arg)
{
    if (m_x != arg) {
        m_x = arg;
        emit xChanged(arg);
    }
}

void setY(int arg)
{
    if (m_y != arg) {
        m_y = arg;
        emit yChanged(arg);
    }
}

void setWidth(int arg)
{
    if (m_width != arg) {
        m_width = arg;
        emit widthChanged(arg);
    }
}

void setHeight(int arg)
{
    if (m_height != arg) {
        m_height = arg;
        emit heightChanged(arg);
    }
}

private:
int m_x;
int m_y;
int m_width;
int m_height;
ParticleAffector* m_affector;
};

#endif // ZONEAFFECTOR_H
