#ifndef MASKEXTRUDER_H
#define MASKEXTRUDER_H
#include "particleextruder.h"
#include <QUrl>

class MaskExtruder : public ParticleExtruder
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit MaskExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);

    QUrl source() const
    {
        return m_source;
    }

signals:

    void sourceChanged(QUrl arg);

public slots:

    void setSource(QUrl arg)
    {
        if (m_source != arg) {
            m_source = arg;
            m_lastHeight = -1;//Trigger reset
            m_lastWidth = -1;
            emit sourceChanged(arg);
        }
    }
private:
    QUrl m_source;

    void ensureInitialized(const QRectF &r);
    int m_lastWidth;
    int m_lastHeight;
    QList<QPointF> m_mask;//TODO: More memory efficient datastructures
};

#endif // MASKEXTRUDER_H
