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
struct ColoredParticleVertex;

class ColoredParticle : public ParticleType
{
    Q_OBJECT
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)
    Q_PROPERTY(QUrl colortable READ colortable WRITE setColortable NOTIFY colortableChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal colorVariation READ colorVariation WRITE setColorVariation NOTIFY colorVariationChanged)
    Q_PROPERTY(qreal alphaVariation READ alphaVariation WRITE setAlphaVariation NOTIFY alphaVariationChanged)

    Q_PROPERTY(qreal additive READ additive WRITE setAdditive NOTIFY additiveChanged)
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

    QColor color() const { return m_color; }
    void setColor(const QColor &color);

    qreal colorVariation() const { return m_color_variation; }
    void setColorVariation(qreal var);

    qreal additive() const { return m_additive; }
    void setAdditive(qreal additive);

    qreal renderOpacity() const { return m_render_opacity; }

    qreal alphaVariation() const
    {
        return m_alphaVariation;
    }

signals:

    void imageChanged();
    void colortableChanged();

    void colorChanged();
    void colorVariationChanged();
    void additiveChanged();

    void particleDurationChanged();
    void alphaVariationChanged(qreal arg);

public slots:
void setAlphaVariation(qreal arg)
{
    if (m_alphaVariation != arg) {
        m_alphaVariation = arg;
        emit alphaVariationChanged(arg);
    }
}

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


    QColor m_color;
    qreal m_color_variation;
    qreal m_additive;
    qreal m_particleDuration;

    QSGGeometryNode *m_node;
    ParticleTrailsMaterial *m_material;

    // derived values...
    int m_last_particle;

    qreal m_render_opacity;
    qreal m_alphaVariation;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // COLOREDPARTICLE_H
