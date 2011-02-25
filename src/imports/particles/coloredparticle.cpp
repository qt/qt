#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <geometry.h>
#include <texturematerial.h>
#include <qsgtexturemanager.h>
#include <QFile>
#include "coloredparticle.h"
#include "particleemitter.h"

class ParticleTrailsMaterial : public AbstractMaterial
{
public:
    ParticleTrailsMaterial()
        : timestamp(0)
        , timelength(1)
    {
        setFlag(Blending, true);
    }

    virtual AbstractMaterialType *type() const { static AbstractMaterialType type; return &type; }
    virtual AbstractMaterialShader *createShader() const;
    virtual int compare(const AbstractMaterial *other) const
    {
        return this - static_cast<const ParticleTrailsMaterial *>(other);
    }

    QSGTextureRef texture;

    qreal timestamp;
    qreal timelength;
};


class ParticleTrailsMaterialData : public AbstractMaterialShader
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
        AbstractMaterialShader::deactivate();

        for (int i=0; i<8; ++i) {
            m_program.setAttributeArray(i, GL_FLOAT, chunkOfBytes, 1, 0);
        }
    }

    virtual void updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *, Renderer::Updates updates)
    {
        ParticleTrailsMaterial *m = static_cast<ParticleTrailsMaterial *>(newEffect);
        Q_ASSERT(m->texture.isReady());
        renderer->setTexture(0, m->texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        m_program.setUniformValue(m_opacity_id, (float) renderer->renderOpacity());
        m_program.setUniformValue(m_timestamp_id, (float) m->timestamp);
        m_program.setUniformValue(m_timelength_id, (float) m->timelength);

        if (updates & Renderer::UpdateMatrices)
            m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = m_program.uniformLocation("matrix");
        m_opacity_id = m_program.uniformLocation("opacity");
        m_timestamp_id = m_program.uniformLocation("timestamp");
        m_timelength_id = m_program.uniformLocation("timelength");
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
    int m_timelength_id;

    QByteArray m_vertex_code;
    QByteArray m_fragment_code;

    static float chunkOfBytes[1024];
};
float ParticleTrailsMaterialData::chunkOfBytes[1024];


AbstractMaterialShader *ParticleTrailsMaterial::createShader() const
{
    return new ParticleTrailsMaterialData;
}


class ParticleTrailsMaterialCT : public ParticleTrailsMaterial
{
public:
    ParticleTrailsMaterialCT()
    {
    }

    virtual AbstractMaterialType *type() const { static AbstractMaterialType type; return &type; }
    virtual AbstractMaterialShader *createShader() const;

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

    virtual void updateState(Renderer *renderer, AbstractMaterial *current, AbstractMaterial *old, Renderer::Updates updates)
    {
        ParticleTrailsMaterialData::updateState(renderer, current, old, updates);
        ParticleTrailsMaterialCT *m = static_cast<ParticleTrailsMaterialCT *>(current);
        Q_ASSERT(m->colortable.isReady());
        renderer->glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m->colortable->textureId());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        m_program.setUniformValue(m_colortable_id, 1);
    }

    int m_colortable_id;
};


AbstractMaterialShader *ParticleTrailsMaterialCT::createShader() const
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
    float size;
    float endSize;
    float dt;
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


ColoredParticle::ColoredParticle(QObject* parent)
    : Particle(parent)
    , m_do_reset(false)
    , m_color(Qt::white)
    , m_color_variation(0.5)
    , m_additive(1)
    , m_node(0)
    , m_material(0)
    , m_particle_count(0)
{
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
    m_particle_count = c;
    //m_system->pleaseReset();//XXX
}

void ColoredParticle::reset()
{
     delete m_node;
     delete m_material;

     m_node = 0;
     m_material = 0;
     m_particle_count = 0;
}

Node* ColoredParticle::buildParticleNode()
{
    QSGContext *sg = QSGContext::current;

    if (m_particle_count * 4 > 0xffff || m_particle_count <= 0) {
        return 0;
    }

    QImage image(m_image_name.toLocalFile());
    if (image.isNull()) {
        printf("ParticleTrails: loading image failed... '%s'\n", qPrintable(m_image_name.toLocalFile()));
        return 0;
    }

    QVector<QSGAttributeDescription> attr;
    attr << QSGAttributeDescription(0, 2, GL_FLOAT, 0); // Position
    attr << QSGAttributeDescription(1, 2, GL_FLOAT, 0); // TexCoord
    attr << QSGAttributeDescription(2, 4, GL_FLOAT, 0); // Data
    attr << QSGAttributeDescription(3, 4, GL_FLOAT, 0); // Vectors..
    attr << QSGAttributeDescription(4, 4, GL_UNSIGNED_BYTE, 0); // Colors

    Geometry *g = new Geometry(attr);
    g->setDrawingMode(QSG::Triangles);

    int vCount = m_particle_count * 4;
    g->setVertexCount(vCount);
    ColoredParticleVertex *vertices = (ColoredParticleVertex *) g->vertexData();
    for (int p=0; p<m_particle_count; ++p) {

        for (int i=0; i<4; ++i) {
            vertices[i].x = 0;
            vertices[i].y = 0;
            vertices[i].t = -1;
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

    int iCount = m_particle_count * 6;
    g->setIndexCount(iCount);
    quint16 *indices = g->ushortIndexData();
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

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    if (!m_colortable_name.isEmpty()) {
        QImage table(m_colortable_name.toLocalFile());
        if (!table.isNull()) {
            m_material = new ParticleTrailsMaterialCT();
            static_cast<ParticleTrailsMaterialCT *>(m_material)->colortable = sg->textureManager()->upload(table);
        }
    }

    if (!m_material)
        m_material = new ParticleTrailsMaterial();

    m_material->texture = sg->textureManager()->upload(image);

    m_node = new GeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);

    m_last_particle = 0;
    return m_node;
}

void ColoredParticle::prepareNextFrame(uint timeStamp)
{
    if (m_node == 0)
        return;

    qreal time = timeStamp / 1000.;
    m_material->timelength = m_particleDuration / 1000.;
    m_material->timestamp = time;

}


void vertexCopy(ColoredParticleVertex &b,const ParticleVertex& a)
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

void ColoredParticle::reload(ParticleData *d)
{
    ColoredParticleVertices *particles = (ColoredParticleVertices *) m_node->geometry()->vertexData();

    int pos = d->systemIndex;

    ColoredParticleVertices &p = particles[pos];

    //TODO: Reinterpret s and a changes so as to end up in the same place?
    //Perhaps we could be more efficient?
    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}

void ColoredParticle::load(ParticleData *d)
{
    m_particleDuration = d->e->particleDuration();//XXX
    //Color initialization
    // Particle color
    Color4ub color;
    color.r = m_color.red() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.g = m_color.green() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.b = m_color.blue() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
    color.a = (1 - m_additive) * 255;
    ColoredParticleVertices *particles = (ColoredParticleVertices *) m_node->geometry()->vertexData();
    ColoredParticleVertices &p = particles[d->systemIndex];
    p.v1.color = p.v2.color = p.v3.color = p.v4.color = color;

    vertexCopy(p.v1, d->pv);
    vertexCopy(p.v2, d->pv);
    vertexCopy(p.v3, d->pv);
    vertexCopy(p.v4, d->pv);
}
