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

#ifndef DEFORMABLEPARTICLE_H
#define DEFORMABLEPARTICLE_H
#include "particle.h"
#include "varyingvector.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class DeformableParticleMaterial;
class QSGGeometryNode;
struct DeformableParticleVertex;

class DeformableParticle : public ParticleType
{
    Q_OBJECT
    //Note that the particle centering can be less accurate with this one
    Q_PROPERTY(QUrl image READ image WRITE setImage NOTIFY imageChanged)

    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
    Q_PROPERTY(qreal rotationVariation READ rotationVariation WRITE setRotationVariation NOTIFY rotationVariationChanged)
    //If true, then will face the direction of motion. Stacks with rotation, e.g. setting rotation
    //to 180 will lead to facing away from the direction of motion
    Q_PROPERTY(bool autoRotation READ autoRotation WRITE autoRotation NOTIFY autoRotationChanged)

    //###Ought to be vectors, not points. Varying Vectors even?
    //###Call i/j? Makes more sense to those with vector calculus experience, and I could even add the cirumflex in QML?
    //xVector is the vector from the top-left point to the top-right point, and is multiplied by current size
    Q_PROPERTY(VaryingVector* xVector READ xVector WRITE setXVector NOTIFY xVectorChanged)
    //yVector is the same, but top-left to bottom-left. The particle is always a parallelogram.
    Q_PROPERTY(VaryingVector* yVector READ yVector WRITE setYVector NOTIFY yVectorChanged)

    //Do we want to add the tables?
    //Q_PROPERTY(QUrl colorTable READ colortable WRITE setColortable NOTIFY colortableChanged)
    //Q_PROPERTY(QUrl sizeTable READ sizetable WRITE setSizetable NOTIFY sizetableChanged)
    //Q_PROPERTY(QUrl opacityTable READ opacitytable WRITE setOpacitytable NOTIFY opacitytableChanged)

    //Does it need alpha? For convenience only, as images probably don't have it
    //Q_PROPERTY(qreal alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    //Q_PROPERTY(qreal alphaVariation READ alphaVariation WRITE setAlphaVariation NOTIFY alphaVariationChanged)

public:
    explicit DeformableParticle(QSGItem *parent = 0);
    virtual ~DeformableParticle(){}

    virtual void load(ParticleData*);
    virtual void reload(ParticleData*);
    virtual void setCount(int c);

    QUrl image() const { return m_image; }
    void setImage(const QUrl &image);

    qreal rotation() const
    {
        return m_rotation;
    }

    bool autoRotation() const
    {
        return m_autoRotation;
    }

    VaryingVector* xVector() const
    {
        return m_xVector;
    }

    VaryingVector* yVector() const
    {
        return m_yVector;
    }

    qreal rotationVariation() const
    {
        return m_rotationVariation;
    }

signals:

    void imageChanged();
    void rotationChanged(qreal arg);

    void autoRotationChanged(bool arg);

    void xVectorChanged(VaryingVector* arg);

    void yVectorChanged(VaryingVector* arg);

    void rotationVariationChanged(qreal arg);

public slots:
void setRotation(qreal arg)
{
    if (m_rotation != arg) {
        m_rotation = arg;
        emit rotationChanged(arg);
    }
}

void autoRotation(bool arg)
{
    if (m_autoRotation != arg) {
        m_autoRotation = arg;
        emit autoRotationChanged(arg);
    }
}

void setXVector(VaryingVector* arg)
{
    if (m_xVector != arg) {
        m_xVector = arg;
        emit xVectorChanged(arg);
    }
}

void setYVector(VaryingVector* arg)
{
    if (m_yVector != arg) {
        m_yVector = arg;
        emit yVectorChanged(arg);
    }
}

void setRotationVariation(qreal arg)
{
    if (m_rotationVariation != arg) {
        m_rotationVariation = arg;
        emit rotationVariationChanged(arg);
    }
}

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void reset();
    void prepareNextFrame();
    QSGGeometryNode* buildParticleNode();
private:
    void vertexCopy(DeformableParticleVertex &b,const ParticleVertex& a);
    bool m_do_reset;

    QUrl m_image;
    QSGGeometryNode *m_node;
    DeformableParticleMaterial *m_material;

    // derived values...
    int m_last_particle;

    qreal m_render_opacity;
    // generated vars
    qreal m_rotation;
    bool m_autoRotation;
    VaryingVector* m_xVector;
    VaryingVector* m_yVector;
    qreal m_rotationVariation;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // DEFORMABLEPARTICLE_H
