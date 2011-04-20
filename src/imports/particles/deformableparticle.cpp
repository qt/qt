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

#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <qsgnode.h>
#include <qsgtexturematerial.h>
#include <qsgtexture.h>
#include <QFile>
#include "deformableparticle.h"
#include <QGLFunctions>
#include <qsgengine.h>

QT_BEGIN_NAMESPACE

const float CONV = 0.017453292519943295;
class DeformableParticleMaterial : public QSGMaterial
{
public:
    DeformableParticleMaterial()
        : timestamp(0)
    {
        setFlag(Blending, true);
    }

    ~DeformableParticleMaterial()
    {
        delete texture;
    }

    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const
    {
        return this - static_cast<const DeformableParticleMaterial *>(other);
    }

    QSGTexture *texture;

    qreal timestamp;
};


class DeformableParticleMaterialData : public QSGMaterialShader
{
public:
    DeformableParticleMaterialData(const char *vertexFile = 0, const char *fragmentFile = 0)
    {
        QFile vf(vertexFile ? vertexFile : ":resources/deformablevertex.shader");
        vf.open(QFile::ReadOnly);
        m_vertex_code = vf.readAll();

        QFile ff(fragmentFile ? fragmentFile : ":resources/deformablefragment.shader");
        ff.open(QFile::ReadOnly);
        m_fragment_code = ff.readAll();

        Q_ASSERT(!m_vertex_code.isNull());
        Q_ASSERT(!m_fragment_code.isNull());
    }

    void deactivate() {
        QSGMaterialShader::deactivate();

        for (int i=0; i<8; ++i) {
            m_program.setAttributeArray(i, GL_FLOAT, chunkOfBytes, 1, 0);
        }
    }

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *)
    {
        DeformableParticleMaterial *m = static_cast<DeformableParticleMaterial *>(newEffect);
        state.context()->functions()->glActiveTexture(GL_TEXTURE0);
        m->texture->bind();

        m_program.setUniformValue(m_opacity_id, state.opacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);

        if (state.isMatrixDirty())
            m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = m_program.uniformLocation("matrix");
        m_opacity_id = m_program.uniformLocation("opacity");
        m_timestamp_id = m_program.uniformLocation("timestamp");
    }

    virtual const char *vertexShader() const { return m_vertex_code.constData(); }
    virtual const char *fragmentShader() const { return m_fragment_code.constData(); }

    virtual char const *const *attributeNames() const {
        static const char *attr[] = {
            "vPos",
            "vTex",
            "vData",
            "vVec",
            "vDeformVec",
            "vRotation",
            0
        };
        return attr;
    }

    virtual bool isColorTable() const { return false; }

    int m_matrix_id;
    int m_opacity_id;
    int m_timestamp_id;

    QByteArray m_vertex_code;
    QByteArray m_fragment_code;

    static float chunkOfBytes[1024];
};
float DeformableParticleMaterialData::chunkOfBytes[1024];


QSGMaterialShader *DeformableParticleMaterial::createShader() const
{
    return new DeformableParticleMaterialData;
}

struct DeformableParticleVertex {
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
    float xx;
    float xy;
    float yx;
    float yy;
    float rotation;
    float autoRotate;//Assume that GPUs prefer floats to bools
};

struct DeformableParticleVertices {
    DeformableParticleVertex v1;
    DeformableParticleVertex v2;
    DeformableParticleVertex v3;
    DeformableParticleVertex v4;
};


DeformableParticle::DeformableParticle(QSGItem* parent)
    : ParticleType(parent)
    , m_do_reset(false)
    , m_rotation(0)
    , m_autoRotation(false)
    , m_xVector(0)
    , m_yVector(0)
    , m_rotationVariation(0)
{
    setFlag(ItemHasContents);
}

void DeformableParticle::setImage(const QUrl &image)
{
    if (image == m_image)
        return;
    m_image = image;
    emit imageChanged();
    reset();
}

void DeformableParticle::setCount(int c)
{
    ParticleType::setCount(c);
    m_pleaseReset = true;
}

void DeformableParticle::reset()
{
    ParticleType::reset();
     m_pleaseReset = true;
}

static QSGGeometry::Attribute DeformableParticle_Attributes[] = {
    { 0, 2, GL_FLOAT },             // Position
    { 1, 2, GL_FLOAT },             // TexCoord
    { 2, 4, GL_FLOAT },             // Data
    { 3, 4, GL_FLOAT },             // Vectors
    { 4, 4, GL_FLOAT },             // DeformationVectors
    { 5, 2, GL_FLOAT }              // Rotation
};

static QSGGeometry::AttributeSet DeformableParticle_AttributeSet =
{
    6, // Attribute Count
    (2 + 2 + 4 + 4 + 4 + 2) * sizeof(float),
    DeformableParticle_Attributes
};

QSGGeometryNode* DeformableParticle::buildParticleNode()
{
    if (m_count * 4 > 0xffff) {
        printf("DeformableParticle: Too many particles... \n");
        return 0;
    }

    if(m_count <= 0) {
        printf("DeformableParticle: Too few particles... \n");
        return 0;
    }

    QImage image(m_image.toLocalFile());
    if (image.isNull()) {
        printf("DeformableParticle: loading image failed... '%s'\n", qPrintable(m_image.toLocalFile()));
        return 0;
    }

    int vCount = m_count * 4;
    int iCount = m_count * 6;

    QSGGeometry *g = new QSGGeometry(DeformableParticle_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    DeformableParticleVertex *vertices = (DeformableParticleVertex *) g->vertexData();
    for (int p=0; p<m_count; ++p) {

        for (int i=0; i<4; ++i) {
            vertices[i].x = 0;
            vertices[i].y = 0;
            vertices[i].t = -1;
            vertices[i].lifeSpan = 0;
            vertices[i].size = 0;
            vertices[i].endSize = 0;
            vertices[i].sx = 0;
            vertices[i].sy = 0;
            vertices[i].ax = 0;
            vertices[i].ay = 0;
            vertices[i].xx = 1;
            vertices[i].xy = 0;
            vertices[i].yx = 0;
            vertices[i].yy = 1;
            vertices[i].rotation = 0;
            vertices[i].autoRotate = 0;
        }

        vertices[0].tx = 0;
        vertices[0].ty = 0;

        vertices[1].tx = 1;
        vertices[1].ty = 0;

        vertices[2].tx = 0;
        vertices[2].ty = 1;

        vertices[3].tx = 1;
        vertices[3].ty = 1;

        vertices += 4;
    }

    quint16 *indices = g->indexDataAsUShort();
    for (int i=0; i<m_count; ++i) {
        int o = i * 4;
        indices[0] = o;
        indices[1] = o + 1;
        indices[2] = o + 2;
        indices[3] = o + 1;
        indices[4] = o + 3;
        indices[5] = o + 2;
        indices += 6;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    if (!m_material)
        m_material = new DeformableParticleMaterial();


    m_material->texture = sceneGraphEngine()->createTextureFromImage(image);
    m_material->texture->setFiltering(QSGTexture::Linear);

    m_node = new QSGGeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);

    m_last_particle = 0;

    return m_node;
}

QSGNode *DeformableParticle::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
{
    if(m_pleaseReset){
        if(m_node)
            delete m_node;
        if(m_material)
            delete m_material;

        m_node = 0;
        m_material = 0;
        m_pleaseReset = false;
    }

    if(m_system && m_system->isRunning())
        prepareNextFrame();
    if (m_node){
        update();
        m_node->markDirty(QSGNode::DirtyMaterial);
    }

    return m_node;
}

void DeformableParticle::prepareNextFrame()
{
    if (m_node == 0){    //TODO: Staggered loading (as emitted)
        m_node = buildParticleNode();
        if(m_node == 0)
            return;
    }
    uint timeStamp = m_system->systemSync(this);

    qreal time = timeStamp / 1000.;
    m_material->timestamp = time;

}


void DeformableParticle::vertexCopy(DeformableParticleVertex &b,const ParticleVertex& a)
{
    b.x = a.x - m_systemOffset.x();
    b.y = a.y - m_systemOffset.y();
    b.t = a.t;
    b.lifeSpan = a.lifeSpan;
    b.size = a.size;
    b.endSize = a.endSize;
    b.sx = a.sx;
    b.sy = a.sy;
    b.ax = a.ax;
    b.ay = a.ay;
}

void DeformableParticle::reload(ParticleData *d)
{
    if (m_node == 0)
        return;

    DeformableParticleVertices *particles = (DeformableParticleVertices *) m_node->geometry()->vertexData();

    int pos = particleTypeIndex(d);

    DeformableParticleVertices &p = particles[pos];

    //Perhaps we could be more efficient?
    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
    //TODO: Allow for change of deformation data?
}

void DeformableParticle::load(ParticleData *d)
{
    if (m_node == 0)
        return;

    //Deformation Initialization
    DeformableParticleVertices *particles = (DeformableParticleVertices *) m_node->geometry()->vertexData();
    DeformableParticleVertices &p = particles[particleTypeIndex(d)];
    if(m_xVector){
        const QPointF &ret = m_xVector->sample(QPointF(d->pv.x, d->pv.y));
        p.v1.xx = p.v2.xx = p.v3.xx = p.v4.xx = ret.x();
        p.v1.xy = p.v2.xy = p.v3.xy = p.v4.xy = ret.y();
    }
    if(m_yVector){
        const QPointF &ret = m_yVector->sample(QPointF(d->pv.x, d->pv.y));
        p.v1.yx = p.v2.yx = p.v3.yx = p.v4.yx = ret.x();
        p.v1.yy = p.v2.yy = p.v3.yy = p.v4.yy = ret.y();
    }
    p.v1.rotation = p.v2.rotation = p.v3.rotation = p.v4.rotation =
            (m_rotation + (m_rotationVariation - 2*((qreal)rand()/RAND_MAX)*m_rotationVariation) ) * CONV;
    p.v1.autoRotate = p.v2.autoRotate = p.v3.autoRotate = p.v4.autoRotate = m_autoRotation?1.0:0.0;

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}

QT_END_NAMESPACE
