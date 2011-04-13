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

#include "spriteimage.h"
#include "spritestate.h"
#include "spriteengine.h"
#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <qsgnode.h>
#include <qsgengine.h>
#include <qsgtexturematerial.h>
#include <qsgtexture.h>
#include <QFile>
#include <cmath>
#include <qmath.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

class SpriteMaterial : public QSGMaterial
{
public:
    SpriteMaterial();
    virtual ~SpriteMaterial();
    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const
    {
        return this - static_cast<const SpriteMaterial *>(other);
    }

    QSGTexture *texture;

    qreal timestamp;
    qreal timelength;
    int framecount;
    int animcount;
    int width;
    int height;
};

SpriteMaterial::SpriteMaterial()
    : timestamp(0)
    , timelength(1)
    , framecount(1)
    , animcount(1)
    , width(0)
    , height(0)
{
    setFlag(Blending, true);
}

SpriteMaterial::~SpriteMaterial()
{
    delete texture;
}

class SpriteMaterialData : public QSGMaterialShader
{
public:
    SpriteMaterialData(const char *vertexFile = 0, const char *fragmentFile = 0)
    {
        QFile vf(vertexFile ? vertexFile : ":resources/spriteimagevertex.shader");
        vf.open(QFile::ReadOnly);
        m_vertex_code = vf.readAll();

        QFile ff(fragmentFile ? fragmentFile : ":resources/spriteimagefragment.shader");
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
        SpriteMaterial *m = static_cast<SpriteMaterial *>(newEffect);
        m->texture->bind();

        m_program.setUniformValue(m_opacity_id, state.opacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);
        m_program.setUniformValue(m_framecount_id, (float) m->framecount);
        m_program.setUniformValue(m_animcount_id, (float) m->animcount);
        m_program.setUniformValue(m_width_id, (float) m->width);
        m_program.setUniformValue(m_height_id, (float) m->height);

        if (state.isMatrixDirty())
            m_program.setUniformValue(m_matrix_id, state.combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = m_program.uniformLocation("matrix");
        m_opacity_id = m_program.uniformLocation("opacity");
        m_timestamp_id = m_program.uniformLocation("timestamp");
        m_framecount_id = m_program.uniformLocation("framecount");
        m_animcount_id = m_program.uniformLocation("animcount");
        m_width_id = m_program.uniformLocation("width");
        m_height_id = m_program.uniformLocation("height");
    }

    virtual const char *vertexShader() const { return m_vertex_code.constData(); }
    virtual const char *fragmentShader() const { return m_fragment_code.constData(); }

    virtual char const *const *attributeNames() const {
        static const char *attr[] = {
           "vTex",
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
    int m_width_id;
    int m_height_id;

    QByteArray m_vertex_code;
    QByteArray m_fragment_code;

    static float chunkOfBytes[1024];
};
float SpriteMaterialData::chunkOfBytes[1024];

QSGMaterialShader *SpriteMaterial::createShader() const
{
    return new SpriteMaterialData;
}

struct SpriteVertex {
    float tx;
    float ty;
    float animIdx;
    float frameDuration;
    float frameCount;
    float animT;
};

struct SpriteVertices {
    SpriteVertex v1;
    SpriteVertex v2;
    SpriteVertex v3;
    SpriteVertex v4;
};

SpriteImage::SpriteImage(QSGItem *parent) :
    QSGItem(parent)
    , m_node(0)
    , m_material(0)
    , m_spriteEngine(0)
    , m_pleaseReset(false)
    , m_running(true)
{
    setFlag(ItemHasContents);
    connect(this, SIGNAL(runningChanged(bool)),
            this, SLOT(update()));
}

QDeclarativeListProperty<SpriteState> SpriteImage::sprites()
{
    return QDeclarativeListProperty<SpriteState>(this, &m_sprites, spriteAppend, spriteCount, spriteAt, spriteClear);
}

void SpriteImage::createEngine()
{
    //TODO: delay until component complete
    if(m_spriteEngine)
        delete m_spriteEngine;
    if(m_sprites.count())
        m_spriteEngine = new SpriteEngine(m_sprites, this);
    else
        m_spriteEngine = 0;
    reset();
}

static QSGGeometry::Attribute SpriteImage_Attributes[] = {
    { 0, 2, GL_FLOAT },            // tex
    { 1, 4, GL_FLOAT }             // animData
};

static QSGGeometry::AttributeSet SpriteImage_AttributeSet =
{
    2, // Attribute Count
    (4 + 2) * sizeof(float),
    SpriteImage_Attributes
};

QSGGeometryNode* SpriteImage::buildNode()
{
    if (!m_spriteEngine) {
        qWarning() << "SpriteImage: No sprite engine...";
        return 0;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    m_material = new SpriteMaterial();

    QImage image = m_spriteEngine->assembledImage();
    if(image.isNull())
        return 0;
    m_material->texture = sceneGraphEngine()->createTextureFromImage(image);
    m_material->texture->setFiltering(QSGTexture::Linear);
    m_material->framecount = m_spriteEngine->maxFrames();

    int vCount = 4;
    int iCount = 6;
    QSGGeometry *g = new QSGGeometry(SpriteImage_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    SpriteVertices *p = (SpriteVertices *) g->vertexData();
    p->v1.animT = p->v2.animT = p->v3.animT = p->v4.animT = 0;
    p->v1.animIdx = p->v2.animIdx = p->v3.animIdx = p->v4.animIdx = 0;
    SpriteState* state = m_spriteEngine->state(0);
    p->v1.frameCount = p->v2.frameCount = p->v3.frameCount = p->v4.frameCount = state->frames();
    p->v1.frameDuration = p->v2.frameDuration = p->v3.frameDuration = p->v4.frameDuration = state->duration();
    m_spriteEngine->startSprite(0);

    p->v1.tx = 0;
    p->v1.ty = 0;

    p->v2.tx = 1.0;
    p->v2.ty = 0;

    p->v3.tx = 0;
    p->v3.ty = 1.0;

    p->v4.tx = 1.0;
    p->v4.ty = 1.0;

    quint16 *indices = g->indexDataAsUShort();
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 1;
    indices[4] = 3;
    indices[5] = 2;


    m_timestamp.start();
    m_node = new QSGGeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    return m_node;
}

void SpriteImage::reset()
{
    m_pleaseReset = true;
}

QSGNode *SpriteImage::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
{
    if(m_pleaseReset){
        delete m_node;
        delete m_material;

        m_node = 0;
        m_material = 0;
        m_pleaseReset = false;
    }

    prepareNextFrame();

    if(m_running){
        update();
        if (m_node)
            m_node->markDirty(QSGNode::DirtyMaterial);
    }

    return m_node;
}

void SpriteImage::prepareNextFrame()
{
    if (m_node == 0)
        m_node = buildNode();
    if (m_node == 0) //error creating node
        return;

    uint timeInt = m_timestamp.elapsed();
    qreal time =  timeInt / 1000.;
    m_material->timestamp = time;
    m_material->animcount = m_spriteEngine->stateCount();
    m_material->height = height();
    m_material->width = width();

    //Advance State
    SpriteVertices *p = (SpriteVertices *) m_node->geometry()->vertexData();
    m_spriteEngine->updateSprites(timeInt);
    int curIdx = m_spriteEngine->spriteState();
    if(curIdx != p->v1.animIdx){
        p->v1.animIdx = p->v2.animIdx = p->v3.animIdx = p->v4.animIdx = curIdx;
        p->v1.animT = p->v2.animT = p->v3.animT = p->v4.animT = m_spriteEngine->spriteStart()/1000.0;
        p->v1.frameCount = p->v2.frameCount = p->v3.frameCount = p->v4.frameCount = m_spriteEngine->state(curIdx)->frames();
        p->v1.frameDuration = p->v2.frameDuration = p->v3.frameDuration = p->v4.frameDuration = m_spriteEngine->state(curIdx)->duration();
    }
}

QT_END_NAMESPACE
