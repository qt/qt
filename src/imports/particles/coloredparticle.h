/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef COLOREDPARTICLE_H
#define COLOREDPARTICLE_H
#include "particle.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class ParticleTrailsMaterial;
class QSGGeometryNode;

struct Color4ub {
    uchar r;
    uchar g;
    uchar b;
    uchar a;
};

struct ColoredParticleVertex {
    float x;
    float y;
    float tx;
    float ty;
    float t;
    float lifeSpan;
    float size;
    float endSize;
    float sx;
    float sy;
    float ax;
    float ay;
    Color4ub color;
};

struct ColoredParticleVertices {
    ColoredParticleVertex v1;
    ColoredParticleVertex v2;
    ColoredParticleVertex v3;
    ColoredParticleVertex v4;
};

class ColoredParticle : public ParticleType
{
    Q_OBJECT
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl colorTable READ colortable WRITE setColortable NOTIFY colortableChanged)
    Q_PROPERTY(QUrl sizeTable READ sizetable WRITE setSizetable NOTIFY sizetableChanged)
    Q_PROPERTY(QUrl opacityTable READ opacitytable WRITE setOpacitytable NOTIFY opacitytableChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    //Stacks (added) with individual colorVariations
    Q_PROPERTY(qreal colorVariation READ colorVariation WRITE setColorVariation NOTIFY colorVariationChanged)
    Q_PROPERTY(qreal redVariation READ redVariation WRITE setRedVariation NOTIFY redVariationChanged)
    Q_PROPERTY(qreal greenVariation READ greenVariation WRITE setGreenVariation NOTIFY greenVariationChanged)
    Q_PROPERTY(qreal blueVariation READ blueVariation WRITE setBlueVariation NOTIFY blueVariationChanged)
    //Stacks (multiplies) with the Alpha in the color, mostly here so you can use svg color names (which have full alpha)
    Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(qreal alphaVariation READ alphaVariation WRITE setAlphaVariation NOTIFY alphaVariationChanged)

public:
    explicit ColoredParticle(QSGItem *parent = 0);
    virtual ~ColoredParticle(){}

    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);

    QUrl image() const { return m_image_name; }
    void setImage(const QUrl &image);

    QUrl colortable() const { return m_colortable_name; }
    void setColortable(const QUrl &table);

    QUrl sizetable() const { return m_sizetable_name; }
    void setSizetable (const QUrl &table);

    QUrl opacitytable() const { return m_opacitytable_name; }
    void setOpacitytable(const QUrl &table);

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    qreal colorVariation() const { return m_color_variation; }
    void setColorVariation(qreal var);

    qreal renderOpacity() const { return m_render_opacity; }

    qreal alphaVariation() const
    {
        return m_alphaVariation;
    }

    qreal alpha() const
    {
        return m_alpha;
    }

    qreal redVariation() const
    {
        return m_redVariation;
    }

    qreal greenVariation() const
    {
        return m_greenVariation;
    }

    qreal blueVariation() const
    {
        return m_blueVariation;
    }

signals:

    void imageChanged();
    void colortableChanged();
    void sizetableChanged();
    void opacitytableChanged();

    void colorChanged();
    void colorVariationChanged();

    void particleDurationChanged();
    void alphaVariationChanged(qreal arg);

    void alphaChanged(qreal arg);

    void redVariationChanged(qreal arg);

    void greenVariationChanged(qreal arg);

    void blueVariationChanged(qreal arg);

public slots:
    void setAlphaVariation(qreal arg)
    {
        if (m_alphaVariation != arg) {
            m_alphaVariation = arg;
            emit alphaVariationChanged(arg);
        }
    }

    void setAlpha(qreal arg)
    {
        if (m_alpha != arg) {
            m_alpha = arg;
            emit alphaChanged(arg);
        }
    }

    void setRedVariation(qreal arg)
    {
        if (m_redVariation != arg) {
            m_redVariation = arg;
            emit redVariationChanged(arg);
        }
    }

    void setGreenVariation(qreal arg)
    {
        if (m_greenVariation != arg) {
            m_greenVariation = arg;
            emit greenVariationChanged(arg);
        }
    }

    void setBlueVariation(qreal arg)
    {
        if (m_blueVariation != arg) {
            m_blueVariation = arg;
            emit blueVariationChanged(arg);
        }
    }

    void reloadColor(const Color4ub &c, ParticleData* d);
protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void reset();
    void prepareNextFrame();
    QSGGeometryNode* buildParticleNode();
private:
    void vertexCopy(ColoredParticleVertex &b,const ParticleVertex& a);
    bool m_do_reset;

    QUrl m_image_name;
    QUrl m_colortable_name;
    QUrl m_sizetable_name;
    QUrl m_opacitytable_name;


    QColor m_color;
    qreal m_color_variation;
    qreal m_particleDuration;

    QSGGeometryNode *m_node;
    ParticleTrailsMaterial *m_material;

    // derived values...
    int m_last_particle;

    qreal m_render_opacity;
    qreal m_alphaVariation;
    qreal m_alpha;
    qreal m_redVariation;
    qreal m_greenVariation;
    qreal m_blueVariation;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // COLOREDPARTICLE_H
