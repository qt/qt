#ifndef COLOREDPARTICLE_H
#define COLOREDPARTICLE_H
#include "particle.h"
class ParticleTrailsMaterial;
class GeometryNode;

class ColoredParticle : public ParticleType
{
    Q_OBJECT
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl colortable READ colortable WRITE setColortable NOTIFY colortableChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal colorVariation READ colorVariation WRITE setColorVariation NOTIFY colorVariationChanged)

    Q_PROPERTY(qreal additive READ additive WRITE setAdditive NOTIFY additiveChanged)
public:
    explicit ColoredParticle(QObject *parent = 0);
    virtual ~ColoredParticle(){}

    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);
    virtual Node* buildParticleNode();
    virtual void reset();
    virtual void prepareNextFrame(uint timeStamp);

    QUrl image() const { return m_image_name; }
    void setImage(const QUrl &image);

    QUrl colortable() const { return m_colortable_name; }
    void setColortable(const QUrl &table);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    qreal colorVariation() const { return m_color_variation; }
    void setColorVariation(qreal var);

    qreal additive() const { return m_additive; }
    void setAdditive(qreal additive);

    qreal renderOpacity() const { return m_render_opacity; }

signals:

    void imageChanged();
    void colortableChanged();

    void colorChanged();
    void colorVariationChanged();
    void additiveChanged();

    void particleDurationChanged();
public slots:

protected:

private:

    bool m_do_reset;

    QUrl m_image_name;
    QUrl m_colortable_name;


    QColor m_color;
    qreal m_color_variation;
    qreal m_additive;
    qreal m_particleDuration;

    GeometryNode *m_node;
    ParticleTrailsMaterial *m_material;

    // derived values...
    int m_last_particle;

    qreal m_render_opacity;
};

#endif // COLOREDPARTICLE_H
