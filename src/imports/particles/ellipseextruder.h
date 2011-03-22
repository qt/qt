#ifndef ELLIPSEEXTRUDER_H
#define ELLIPSEEXTRUDER_H
#include "particleextruder.h"

class EllipseExtruder : public ParticleExtruder
{
    Q_OBJECT
    Q_PROPERTY(bool fill READ fill WRITE setFill NOTIFY fillChanged)
public:
    explicit EllipseExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);

    bool fill() const
    {
        return m_fill;
    }

signals:

    void fillChanged(bool arg);

public slots:

    void setFill(bool arg)
    {
        if (m_fill != arg) {
            m_fill = arg;
            emit fillChanged(arg);
        }
    }
private:
    bool m_fill;
};

#endif // ELLIPSEEXTRUDER_H
