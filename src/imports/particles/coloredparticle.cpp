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
#include "coloredparticle.h"
#include "particleemitter.h"
#include <QGLFunctions>

QT_BEGIN_NAMESPACE

class ParticleTrailsMaterial : public QSGMaterial
{
public:
    ParticleTrailsMaterial()
        : timestamp(0)
    {
        setFlag(Blending, true);
    }

    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const
    {
        return this - static_cast<const ParticleTrailsMaterial *>(other);
    }

    QSGTextureRef texture;

    qreal timestamp;
};


class ParticleTrailsMaterialData : public QSGMaterialShader
{
public:
    ParticleTrailsMaterialData(const char *vertexFile = 0, const char *fragmentFile = 0)
    {
        QFile vf(vertexFile ? vertexFile : ":resources/trailsvertex.shader");
        vf.open(QFile::ReadOnly);
        m_vertex_code = vf.readAll();

        QFile ff(fragmentFile ? fragmentFile : ":resources/trailsfragment.shader");
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
        ParticleTrailsMaterial *m = static_cast<ParticleTrailsMaterial *>(newEffect);
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
            "vColor",
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
float ParticleTrailsMaterialData::chunkOfBytes[1024];


QSGMaterialShader *ParticleTrailsMaterial::createShader() const
{
    return new ParticleTrailsMaterialData;
}


class ParticleTrailsMaterialCT : public ParticleTrailsMaterial
{
public:
    ParticleTrailsMaterialCT()
    {
    }

    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;

    QSGTextureRef colortable;
};


class ParticleTrailsMaterialDataCT : public ParticleTrailsMaterialData
{
public:
    ParticleTrailsMaterialDataCT()
        : ParticleTrailsMaterialData(":resources/ctvertex.shader", ":resources/ctfragment.shader")
    {
    }

    bool isColorTable() const { return true; }

    virtual void initialize() {
        ParticleTrailsMaterialData::initialize();
        m_colortable_id = m_program.uniformLocation("colortable");
    }

    virtual void updateState(const RenderState &state, QSGMaterial *current, QSGMaterial *old)
    {
        // Bind the texture to unit 1 before calling the base class, so that the
        // base class can set active texture back to 0.
        ParticleTrailsMaterialCT *m = static_cast<ParticleTrailsMaterialCT *>(current);
        state.context()->functions()->glActiveTexture(GL_TEXTURE1);
        m->colortable->bind();
        m_program.setUniformValue(m_colortable_id, 1);

        ParticleTrailsMaterialData::updateState(state, current, old);
    }

    int m_colortable_id;
};


QSGMaterialShader *ParticleTrailsMaterialCT::createShader() const
{
    return new ParticleTrailsMaterialDataCT;
}

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


ColoredParticle::ColoredParticle(QSGItem* parent)
    : ParticleType(parent)
    , m_do_reset(false)
    , m_color(Qt::white)
    , m_color_variation(0.5)
    , m_additive(1)
    , m_node(0)
    , m_material(0)
    , m_alphaVariation(0)
{
    setFlag(ItemHasContents);
}

void ColoredParticle::setImage(const QUrl &image)
{
    if (image == m_image_name)
        return;
    m_image_name = image;
    emit imageChanged();
    //m_system->pleaseReset();//XXX
}


void ColoredParticle::setColortable(const QUrl &table)
{
    if (table == m_colortable_name)
        return;
    m_colortable_name = table;
    emit colortableChanged();
    //m_system->pleaseReset();//XXX
}

void ColoredParticle::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    emit colorChanged();
    //m_system->pleaseReset();//XXX
}

void ColoredParticle::setColorVariation(qreal var)
{
    if (var == m_color_variation)
        return;
    m_color_variation = var;
    emit colorVariationChanged();
    //m_system->pleaseReset();//XXX
}

void ColoredParticle::setAdditive(qreal additive)
{
    if (m_additive == additive)
        return;
    m_additive = additive;
    emit additiveChanged();
    //m_system->pleaseReset();//XXX
}

void ColoredParticle::setCount(int c)
{
    ParticleType::setCount(c);
    m_pleaseReset = true;
}

void ColoredParticle::reset()
{
     m_pleaseReset = true;
}

static QSGGeometry::Attribute ColoredParticle_Attributes[] = {
    { 0, 2, GL_FLOAT },             // Position
    { 1, 2, GL_FLOAT },             // TexCoord
    { 2, 4, GL_FLOAT },             // Data
    { 3, 4, GL_FLOAT },             // Vectors
    { 4, 4, GL_UNSIGNED_BYTE }   // Colors
};

static QSGGeometry::AttributeSet ColoredParticle_AttributeSet =
{
    5, // Attribute Count
    (2 + 2 + 4 + 4) * sizeof(float) + 4 * sizeof(uchar),
    ColoredParticle_Attributes
};

QSGGeometryNode* ColoredParticle::buildParticleNode()
{
    QSGContext *sg = QSGContext::current;

    if (m_count * 4 > 0xffff) {
        printf("ColoredParticle: Too many particles... \n");
        return 0;
    }

    if(m_count <= 0) {
        printf("ColoredParticle: Too few particles... \n");
        return 0;
    }

    QImage image(m_image_name.toLocalFile());
    if (image.isNull()) {
        printf("ParticleTrails: loading image failed... '%s'\n", qPrintable(m_image_name.toLocalFile()));
        return 0;
    }

    int vCount = m_count * 4;
    int iCount = m_count * 6;

    QSGGeometry *g = new QSGGeometry(ColoredParticle_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    ColoredParticleVertex *vertices = (ColoredParticleVertex *) g->vertexData();
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

    if (!m_colortable_name.isEmpty()) {
        QImage table(m_colortable_name.toLocalFile());
        if (!table.isNull()) {
            m_material = new ParticleTrailsMaterialCT();
            static_cast<ParticleTrailsMaterialCT *>(m_material)->colortable = sg->createTexture(table);
        }
    }

    if (!m_material)
        m_material = new ParticleTrailsMaterial();


    m_material->texture = sg->createTexture(image);

    m_node = new QSGGeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    m_node->setFlag(QSGNode::OwnsGeometry);

    m_last_particle = 0;

    return m_node;
}

QSGNode *ColoredParticle::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
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

    if(m_system->isRunning())
        prepareNextFrame();
    if (m_node){
        update();
        m_node->markDirty(QSGNode::DirtyMaterial);
    }

    return m_node;
}

void ColoredParticle::prepareNextFrame()
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


void ColoredParticle::vertexCopy(ColoredParticleVertex &b,const ParticleVertex& a)
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

void ColoredParticle::reload(ParticleData *d)
{
    if (m_node == 0)
        return;

    ColoredParticleVertices *particles = (ColoredParticleVertices *) m_node->geometry()->vertexData();

    int pos = particleTypeIndex(d);


    ColoredParticleVertices &p = particles[pos];

    //Perhaps we could be more efficient?
    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}

void ColoredParticle::load(ParticleData *d)
{
    if (m_node == 0)
        return;

    //Color initialization
    // Particle color
    Color4ub color;
    color.r = m_color.red() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.g = m_color.green() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.b = m_color.blue() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.a = (1 - m_additive) * m_color.alpha() * (1 - m_alphaVariation) + rand() % 256 * m_alphaVariation;
    ColoredParticleVertices *particles = (ColoredParticleVertices *) m_node->geometry()->vertexData();
    ColoredParticleVertices &p = particles[particleTypeIndex(d)];
    p.v1.color = p.v2.color = p.v3.color = p.v4.color = color;

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}

QT_END_NAMESPACE
