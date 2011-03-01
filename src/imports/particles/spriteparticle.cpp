#include "spriteparticle.h"
#include "spritestate.h"
#include "spriteengine.h"
#include "particleemitter.h"
#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <texturematerial.h>
#include <qsgtexturemanager.h>
#include <QFile>
#include <cmath>
#include <qmath.h>
#include <QDebug>

class SpriteParticlesMaterial : public AbstractMaterial
{
public:
    SpriteParticlesMaterial();
    virtual ~SpriteParticlesMaterial();
    virtual AbstractMaterialType *type() const { static AbstractMaterialType type; return &type; }
    virtual AbstractMaterialShader *createShader() const;
    virtual int compare(const AbstractMaterial *other) const
    {
        return this - static_cast<const SpriteParticlesMaterial *>(other);
    }

    QSGTextureRef texture;

    qreal timestamp;
    qreal timelength;
    int framecount;
    int animcount;
};

SpriteParticlesMaterial::SpriteParticlesMaterial()
    : timestamp(0)
    , timelength(1)
    , framecount(1)
    , animcount(1)
{
    setFlag(Blending, true);
}

SpriteParticlesMaterial::~SpriteParticlesMaterial()
{

}

class SpriteParticlesMaterialData : public AbstractMaterialShader
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
        AbstractMaterialShader::deactivate();

        for (int i=0; i<8; ++i) {
            m_program.setAttributeArray(i, GL_FLOAT, chunkOfBytes, 1, 0);
        }
    }

    virtual void updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *, Renderer::Updates updates)
    {
        SpriteParticlesMaterial *m = static_cast<SpriteParticlesMaterial *>(newEffect);
        Q_ASSERT(m->texture.isReady());
        renderer->setTexture(0, m->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_program.setUniformValue(m_opacity_id, (float) renderer->renderOpacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);
        m_program.setUniformValue(m_timelength_id, (float) m->timelength);
        m_program.setUniformValue(m_framecount_id, (float) m->framecount);
        m_program.setUniformValue(m_animcount_id, (float) m->animcount);

        if (updates & Renderer::UpdateMatrices)
            m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = m_program.uniformLocation("matrix");
        m_opacity_id = m_program.uniformLocation("opacity");
        m_timestamp_id = m_program.uniformLocation("timestamp");
        m_timelength_id = m_program.uniformLocation("timelength");
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
    int m_timelength_id;
    int m_framecount_id;
    int m_animcount_id;

    QByteArray m_vertex_code;
    QByteArray m_fragment_code;

    static float chunkOfBytes[1024];
};
float SpriteParticlesMaterialData::chunkOfBytes[1024];

AbstractMaterialShader *SpriteParticlesMaterial::createShader() const
{
    return new SpriteParticlesMaterialData;
}

struct SpriteParticleVertex {
    float x;
    float y;
    float tx;
    float ty;
    float t;
    float size;
    float endSize;
    float dt;
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

SpriteParticle::SpriteParticle(QObject *parent) :
    Particle(parent)
    , m_node(0)
    , m_material(0)
    , m_sprite(0)
    , m_spriteEngine(0)
{
}

void SpriteParticle::setCount(int c)
{
    m_particle_count = c;//### Terribly shoddy
}

bool SpriteParticle::buildParticleTexture(QSGContext *sg)
{
    m_maxFrames = 0;

    if(!m_sprite && !m_spriteEngine){
        qWarning() << "SpriteParticle: No sprite or sprite engine...";
        return false;
    }

    if(m_sprite){
        QImage img(m_sprite->source().toLocalFile());
        if (img.isNull()) {
            qWarning() << "SpriteParticle: loading image failed..." << m_sprite->source().toLocalFile();
            return false;
        }
        m_maxFrames = m_sprite->frames();
        m_material->texture = sg->textureManager()->upload(img);
    }else{//m_spriteEngine
        m_maxFrames = m_spriteEngine->maxFrames();
        QImage image = m_spriteEngine->assembledImage();
        if(image.isNull())
            return false;
        m_spriteEngine->setCount(m_particle_count);
        m_material->texture = sg->textureManager()->upload(image);
     }
    return true;
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



Node* SpriteParticle::buildParticleNode()
{
    QSGContext *sg = QSGContext::current;

    if (m_particle_count * 4 > 0xffff) {
        qWarning() << "SpriteParticle: too many particles...";
        return 0;
    }

    if (m_particle_count * 4 == 0) {
        qWarning() << "SpriteParticle: No particles...";
        return 0;
    }

    if (!m_sprite && !m_spriteEngine) {
        qWarning() << "SpriteParticle: No sprite or engine...";
        return 0;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    m_material = new SpriteParticlesMaterial();

    if(!buildParticleTexture(sg))//problem loading image files
        return 0;

    int vCount = m_particle_count * 4;
    int iCount = m_particle_count * 6;
    QSGGeometry *g = new QSGGeometry(SpriteParticle_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    SpriteParticleVertex *vertices = (SpriteParticleVertex *) g->vertexData();
    for (int p=0; p<m_particle_count; ++p) {

        for (int i=0; i<4; ++i) {
            vertices[i].x = 0;
            vertices[i].y = 0;
            vertices[i].t = -1;
            vertices[i].size = 0;
            vertices[i].endSize = 0;
            vertices[i].dt = -1;
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
    for (int i=0; i<m_particle_count; ++i) {
        int o = i * 4;
        indices[0] = o;
        indices[1] = o + 1;
        indices[2] = o + 2;
        indices[3] = o + 1;
        indices[4] = o + 3;
        indices[5] = o + 2;
        indices += 6;
    }


    m_node = new GeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    m_last_particle = 0;
    return m_node;
}

void vertexCopy(SpriteParticleVertex &b,const ParticleVertex& a)
{
    b.x = a.x;
    b.y = a.y;
    b.t = a.t;
    b.size = a.size;
    b.endSize = a.endSize;
    b.dt = a.dt;
    b.sx = a.sx;
    b.sy = a.sy;
    b.ax = a.ax;
    b.ay = a.ay;
}

void SpriteParticle::load(ParticleData *d)
{
    SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
    SpriteParticleVertices &p = particles[d->systemIndex];

    // Initial Sprite State
    p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = p.v1.t;
    p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = 0;
    SpriteState* state = m_sprite;
    if(!state)
        state = m_spriteEngine->state(0);
    p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = state->frames();
    p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = state->duration();
    if(m_spriteEngine)
        m_spriteEngine->startSprite(d->systemIndex);

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);

    m_particle_duration = d->e->particleDuration();
}

void SpriteParticle::reload(ParticleData *d)
{
    if (m_node == 0) //error creating node
        return;

    SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
    int pos = d->systemIndex;
    SpriteParticleVertices &p = particles[pos];

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}

void SpriteParticle::prepareNextFrame(uint timeInt)
{
    if (m_node == 0) //error creating node
        return;

    qreal time =  timeInt / 1000.;
    m_material->timelength = m_particle_duration / 1000.;
    m_material->timestamp = time;
    m_material->framecount = m_maxFrames;
    m_material->animcount = m_sprite?1:m_spriteEngine->stateCount();

    if(!m_sprite){//Advance State
        SpriteParticleVertices *particles = (SpriteParticleVertices *) m_node->geometry()->vertexData();
        m_spriteEngine->updateSprites(timeInt);
        for(int i=0; i<m_particle_count; i++){
            SpriteParticleVertices &p = particles[i];
            int curIdx = m_spriteEngine->spriteState(i);
            if(curIdx != p.v1.animIdx){
                p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = curIdx;
                p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = time;
                p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = m_spriteEngine->state(curIdx)->frames();
                p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = m_spriteEngine->state(curIdx)->duration();
            }
        }
    }
}

void SpriteParticle::reset()
{
    delete m_node;
    delete m_material;

    m_node = 0;
    m_material = 0;
    m_particle_count = 0;
}
