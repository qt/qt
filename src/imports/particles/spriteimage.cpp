#include "spriteimage.h"
#include "spritestate.h"
#include "spriteengine.h"
#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <texturematerial.h>
#include <qsgtexturemanager.h>
#include <QFile>
#include <cmath>
#include <qmath.h>
#include <QDebug>


class SpriteMaterial : public AbstractMaterial
{
public:
    SpriteMaterial();
    virtual ~SpriteMaterial();
    virtual AbstractMaterialType *type() const { static AbstractMaterialType type; return &type; }
    virtual AbstractMaterialShader *createShader() const;
    virtual int compare(const AbstractMaterial *other) const
    {
        return this - static_cast<const SpriteMaterial *>(other);
    }

    QSGTextureRef texture;

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
}

class SpriteMaterialData : public AbstractMaterialShader
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
        AbstractMaterialShader::deactivate();

        for (int i=0; i<8; ++i) {
            m_program.setAttributeArray(i, GL_FLOAT, chunkOfBytes, 1, 0);
        }
    }

    virtual void updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *, Renderer::Updates updates)
    {
        SpriteMaterial *m = static_cast<SpriteMaterial *>(newEffect);
        Q_ASSERT(m->texture.isReady());
        renderer->setTexture(0, m->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_program.setUniformValue(m_opacity_id, (float) renderer->renderOpacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);
        m_program.setUniformValue(m_framecount_id, (float) m->framecount);
        m_program.setUniformValue(m_animcount_id, (float) m->animcount);
        m_program.setUniformValue(m_width_id, (float) m->width);
        m_program.setUniformValue(m_height_id, (float) m->height);

        if (updates & Renderer::UpdateMatrices)
            m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
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

AbstractMaterialShader *SpriteMaterial::createShader() const
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
    , m_sprite(0)
    , m_spriteEngine(0)
    , m_pleaseReset(false)
    , m_running(true)
{
    setFlag(ItemHasContents);
    connect(this, SIGNAL(runningChanged(bool)),
            this, SLOT(update()));
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

bool SpriteImage::buildParticleTexture(QSGContext *sg)
{
    m_maxFrames = 0;

    if(m_sprite){
        QImage img(m_sprite->source().toLocalFile());
        if (img.isNull()) {
            qWarning() << "SpriteImage: loading image failed..." << m_sprite->source().toLocalFile();
            return false;
        }
        m_maxFrames = m_sprite->frames();
        m_material->texture = sg->textureManager()->upload(img);
        m_material->height = img.height();
        m_material->width = img.width()/m_maxFrames;
    }else{//m_spriteEngine
        m_maxFrames = m_spriteEngine->maxFrames();
        QImage image = m_spriteEngine->assembledImage();
        if(image.isNull())
            return false;
        m_spriteEngine->setCount(1);
        m_material->texture = sg->textureManager()->upload(image);
        m_material->height = image.height()/m_spriteEngine->stateCount();
        m_material->width = image.width()/m_maxFrames;
    }
    m_material->framecount = m_maxFrames;
    return true;
}


GeometryNode* SpriteImage::buildNode()
{
    QSGContext *sg = QSGContext::current;

    if (!m_sprite && !m_spriteEngine) {
        qWarning() << "SpriteImage: No sprite or engine...";
        return 0;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    m_material = new SpriteMaterial();

    if(!buildParticleTexture(sg))//problem loading image files
            return 0;

    int vCount = 4;
    int iCount = 6;
    QSGGeometry *g = new QSGGeometry(SpriteImage_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    SpriteVertices *p = (SpriteVertices *) g->vertexData();
    p->v1.animT = p->v2.animT = p->v3.animT = p->v4.animT = 0;
    p->v1.animIdx = p->v2.animIdx = p->v3.animIdx = p->v4.animIdx = 0;
    SpriteState* state = m_sprite;
    if(!state)
        state = m_spriteEngine->state(0);
    p->v1.frameCount = p->v2.frameCount = p->v3.frameCount = p->v4.frameCount = state->frames();
    p->v1.frameDuration = p->v2.frameDuration = p->v3.frameDuration = p->v4.frameDuration = state->duration();
    if(m_spriteEngine)
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
    m_node = new GeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    return m_node;
}

void SpriteImage::reset()
{
    m_pleaseReset = true;
}

Node *SpriteImage::updatePaintNode(Node *, UpdatePaintNodeData *)
{
    if(m_pleaseReset){
        delete m_node;
        delete m_material;

        m_node = 0;
        m_material = 0;
    }

    prepareNextFrame();

    if(m_running){
        update();
        if (m_node)
            m_node->markDirty(Node::DirtyMaterial);
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
    m_material->animcount = m_sprite?1:m_spriteEngine->stateCount();

    if(!m_sprite){//Advance State
        SpriteVertices *p = (SpriteVertices *) m_node->geometry()->vertexData();
        m_spriteEngine->updateSprites(timeInt);
        int curIdx = m_spriteEngine->spriteState();
        if(curIdx != p->v1.animIdx){
            p->v1.animIdx = p->v2.animIdx = p->v3.animIdx = p->v4.animIdx = curIdx;
            p->v1.animT = p->v2.animT = p->v3.animT = p->v4.animT = time;
            p->v1.frameCount = p->v2.frameCount = p->v3.frameCount = p->v4.frameCount = m_spriteEngine->state(curIdx)->frames();
            p->v1.frameDuration = p->v2.frameDuration = p->v3.frameDuration = p->v4.frameDuration = m_spriteEngine->state(curIdx)->duration();
        }
    }
}

