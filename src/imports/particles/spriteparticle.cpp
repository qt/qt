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

#include "spriteparticle.h"
#include "spritestate.h"
#include "spriteengine.h"
#include "particleemitter.h"
#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <qsgnode.h>
#include <qsgtexturematerial.h>
#include <qsgengine.h>
#include <qsgtexture.h>
#include <QFile>
#include <cmath>
#include <qmath.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

class SpriteParticlesMaterial : public QSGMaterial
{
public:
    SpriteParticlesMaterial();
    virtual ~SpriteParticlesMaterial();
    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const
    {
        return this - static_cast<const SpriteParticlesMaterial *>(other);
    }

    QSGTexture *texture;

    qreal timestamp;
    int framecount;
    int animcount;
};

SpriteParticlesMaterial::SpriteParticlesMaterial()
    : timestamp(0)
    , framecount(1)
    , animcount(1)
{
    setFlag(Blending, true);
}

SpriteParticlesMaterial::~SpriteParticlesMaterial()
{
    delete texture;
}

class SpriteParticlesMaterialData : public QSGMaterialShader
{
public:
    SpriteParticlesMaterialData(const char *vertexFile = 0, const char *fragmentFile = 0)
    {
        QFile vf(vertexFile ? vertexFile : ":resources/spritevertex.shader");
        vf.open(QFile::ReadOnly);
        m_vertex_code = vf.readAll();

        QFile ff(fragmentFile ? fragmentFile : ":resources/spritefragment.shader");
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
        SpriteParticlesMaterial *m = static_cast<SpriteParticlesMaterial *>(newEffect);
        m->texture->bind();

        m_program.setUniformValue(m_opacity_id, state.opacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);
        m_program.setUniformValue(m_framecount_id, (float) m->framecount);
        m_program.setUniformValue(m_animcount_id, (float) m->animcount);

        if (state.isMatrixDirty())
            m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = m_program.uniformLocation("matrix");
        m_opacity_id = m_program.uniformLocation("opacity");
        m_timestamp_id = m_program.uniformLocation("timestamp");
        m_framecount_id = m_program.uniformLocation("framecount");
        m_animcount_id = m_program.uniformLocation("animcount");
    }

    virtual const char *vertexShader() const { return m_vertex_code.constData(); }
    virtual const char *fragmentShader() const { return m_fragment_code.constData(); }

    virtual char const *const *attributeNames() const {
        static const char *attr[] = {
            "vPos",
            "vTex",
            "vData",
            "vVec",
            "vAnimData",
            0
        };
        return attr;
    }

    virtual bool isColorTable() const { return false; }

    int m_matrix_id;
    int m_opacity_id;
    int m_timestamp_id;
    int m_framecount_id;
    int m_animcount_id;

    QByteArray m_vertex_code;
    QByteArray m_fragment_code;

    static float chunkOfBytes[1024];
};
float SpriteParticlesMaterialData::chunkOfBytes[1024];

QSGMaterialShader *SpriteParticlesMaterial::createShader() const
{
    return new SpriteParticlesMaterialData;
}

struct SpriteParticleVertex {
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
    float animIdx;
    float frameDuration;
    float frameCount;
    float animT;
};

struct SpriteParticleVertices {
    SpriteParticleVertex v1;
    SpriteParticleVertex v2;
    SpriteParticleVertex v3;
    SpriteParticleVertex v4;
};

SpriteParticle::SpriteParticle(QSGItem *parent) :
    ParticleType(parent)
    , m_node(0)
    , m_material(0)
    , m_spriteEngine(0)
{
    setFlag(ItemHasContents);
 }
QDeclarativeListProperty<SpriteState> SpriteParticle::sprites()
{
    return QDeclarativeListProperty<SpriteState>(this, &m_sprites, spriteAppend, spriteCount, spriteAt, spriteClear);
}

void SpriteParticle::createEngine()
{
    if(m_spriteEngine)
        delete m_spriteEngine;
    if(m_sprites.count())
        m_spriteEngine = new SpriteEngine(m_sprites, this);
    else
        m_spriteEngine = 0;
    reset();//###this is probably out of updatePaintNode and shouldn't be
}

void SpriteParticle::setCount(int c)
{
    ParticleType::setCount(c);
    m_pleaseReset = true;
}

static QSGGeometry::Attribute SpriteParticle_Attributes[] = {
    { 0, 2, GL_FLOAT },             // Position
    { 1, 2, GL_FLOAT },             // TexCoord
    { 2, 4, GL_FLOAT },             // Data
    { 3, 4, GL_FLOAT },             // Vectors
    { 4, 4, GL_FLOAT }              // Colors
};

static QSGGeometry::AttributeSet SpriteParticle_AttributeSet =
{
    5, // Attribute Count
    (2 + 2 + 4 + 4 + 4) * sizeof(float),
    SpriteParticle_Attributes
};



QSGGeometryNode* SpriteParticle::buildParticleNode()
{
    if (m_count * 4 > 0xffff) {
        qWarning() << "SpriteParticle: too many particles...";
        return 0;
    }

    if (m_count * 4 == 0) {
        qWarning() << "SpriteParticle: No particles...";
        return 0;
    }

    if (!m_spriteEngine) {
        qWarning() << "SpriteParticle: No sprite engine...";
        return 0;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    m_material = new SpriteParticlesMaterial();

    QImage image = m_spriteEngine->assembledImage();
    if(image.isNull())
        return 0;
    m_material->texture = sceneGraphEngine()->createTextureFromImage(image);
    m_material->texture->setFiltering(QSGTexture::Linear);
    m_material->framecount = m_spriteEngine->maxFrames();
    m_spriteEngine->setCount(m_count);

    int vCount = m_count * 4;
    int iCount = m_count * 6;
    QSGGeometry *g = new QSGGeometry(SpriteParticle_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    SpriteParticleVertex *vertices = (SpriteParticleVertex *) g->vertexData();
    for (int p=0; p<m_count; ++p) {

        for (int i=0; i<4; ++i) {
            vertices[i].x = 0;
            vertices[i].y = 0;
            vertices[i].t = -1;
            vertices[i].lifeSpan = -1;
            vertices[i].size = 0;
            vertices[i].endSize = 0;
            vertices[i].sx = 0;
            vertices[i].sy = 0;
            vertices[i].ax = 0;
            vertices[i].ay = 0;
            vertices[i].animIdx = 0;
            vertices[i].frameDuration = 1;
            vertices[i].frameCount = 1;
            vertices[i].animT = -1;

        }

        vertices[0].tx = 0;
        vertices[0].ty = 0;

        vertices[1].tx = 1.0;
        vertices[1].ty = 0;

        vertices[2].tx = 0;
        vertices[2].ty = 1.0;

        vertices[3].tx = 1.0;
        vertices[3].ty = 1.0;

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


    m_node = new QSGGeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    m_last_particle = 0;
    return m_node;
}

void SpriteParticle::vertexCopy(SpriteParticleVertex &b,const ParticleVertex& a)
{
    b.x = a.x + m_systemOffset.x();
    b.y = a.y + m_systemOffset.y();
    b.t = a.t;
    b.lifeSpan = a.lifeSpan;
    b.size = a.size;
    b.endSize = a.endSize;
    b.sx = a.sx;
    b.sy = a.sy;
    b.ax = a.ax;
    b.ay = a.ay;
}

void SpriteParticle::load(ParticleData *d)
{
    if (m_node == 0) //error creating node
        return;

    SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
    int pos = particleTypeIndex(d);
    SpriteParticleVertices &p = particles[pos];

    // Initial Sprite State
    p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = p.v1.t;
    p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = 0;
    SpriteState* state = m_spriteEngine->state(0);
    p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = state->frames();
    p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = state->duration();
    m_spriteEngine->startSprite(pos);

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);

}

void SpriteParticle::reload(ParticleData *d)
{
    if (m_node == 0) //error creating node
        return;

    SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
    int pos = particleTypeIndex(d);
    SpriteParticleVertices &p = particles[pos];

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}


QSGNode *SpriteParticle::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
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
    if(m_system&& m_system->isRunning())
        prepareNextFrame();
    if (m_node){
        update();
        m_node->markDirty(QSGNode::DirtyMaterial);
    }

    return m_node;
}

void SpriteParticle::prepareNextFrame()
{
    if (m_node == 0){    //TODO: Staggered loading (as emitted) (is it just moving this check to load()?)
        m_node = buildParticleNode();
        if(m_node == 0)
            return;
    }
    uint timeStamp = m_system->systemSync(this);


    qreal time =  timeStamp / 1000.;
    m_material->timestamp = time;
    m_material->animcount = m_spriteEngine->stateCount();

    //Advance State
    SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
    m_spriteEngine->updateSprites(timeStamp);
    for(int i=0; i<m_count; i++){
        SpriteParticleVertices &p = particles[i];
        int curIdx = m_spriteEngine->spriteState(i);
        if(curIdx != p.v1.animIdx){
            p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = curIdx;
            p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = m_spriteEngine->spriteStart(i)/1000.0;
            p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = m_spriteEngine->state(curIdx)->frames();
            p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = m_spriteEngine->state(curIdx)->duration();
        }
    }
}

void SpriteParticle::reset()
{
    ParticleType::reset();
    m_pleaseReset = true;
}

QT_END_NAMESPACE
