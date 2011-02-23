#include "trailsemitter.h"
#include "particlesystem.h"

#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <geometry.h>
#include <texturematerial.h>
#include <qsgtexturemanager.h>

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
            "vAnimData",
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





TrailsEmitter::TrailsEmitter(QObject* parent)
    : ParticleEmitter(parent)
    , m_do_reset(false)
    , m_particles_per_second(10)
    , m_particle_duration(1000)
    , m_particle_size(16)
    , m_particle_end_size(-1)
    , m_particle_size_variation(0)
    , m_emitter_x(0)
    , m_emitter_y(0)
    , m_emitter_x_variation(0)
    , m_emitter_y_variation(0)
    , m_emitting(true)
    , m_x_speed(0)
    , m_y_speed(0)
    , m_x_speed_variation(0)
    , m_y_speed_variation(0)
    , m_speed_from_movement(0)
    , m_x_accel(0)
    , m_y_accel(0)
    , m_x_accel_variation(0)
    , m_y_accel_variation(0)
    , m_color(Qt::white)
    , m_color_variation(0.5)
    , m_additive(1)
    , m_node(0)
    , m_material(0)
    , m_particle_count(0)
    , m_reset_last(true)
    , m_last_timestamp(0)
{
//    setFlag(ItemHasContents);
}


void TrailsEmitter::setRunning(bool r)
{
    if (r == m_running)
        return;
    m_running = r;

    if (!m_running)
        reset();
    else {
      m_reset_last = true;
    }

    emit runningChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setParticleSize(qreal size)
{
    if (size == m_particle_size)
        return;
    m_particle_size = size;
    emit particleSizeChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setParticleEndSize(qreal size)
{
    if (size == m_particle_end_size)
        return;
    m_particle_end_size = size;
    emit particleEndSizeChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setParticleSizeVariation(qreal var)
{
    if (var == m_particle_size_variation)
        return;
    m_particle_size_variation = var;
    emit particleSizeVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setEmitterX(qreal x)
{
    if (x == m_emitter_x)
        return;
    m_emitter_x = x;
    emit emitterXChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setEmitterY(qreal y)
{
    if (y == m_emitter_y)
        return;
    m_emitter_y = y;
    emit emitterYChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setEmitterXVariation(qreal var)
{
    if (var == m_emitter_x_variation)
        return;
    m_emitter_x_variation = var;
    emit emitterXVariationChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setEmitterYVariation(qreal var)
{
    if (var == m_emitter_y_variation)
        return;
    m_emitter_y_variation = var;
    emit emitterYVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setEmitting(bool emitting)
{
    if (emitting == m_emitting)
        return;
    m_emitting = emitting;
    if (m_emitting)
      m_reset_last = true;
    emit emittingChanged();
    m_system->pleaseUpdate();//XXX
}



void TrailsEmitter::setImage(const QUrl &image)
{
    if (image == m_image_name)
        return;
    m_image_name = image;
    emit imageChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setColortable(const QUrl &table)
{
    if (table == m_colortable_name)
        return;
    m_colortable_name = table;
    emit colortableChanged();
    m_system->pleaseUpdate();//XXX
}



void TrailsEmitter::setParticlesPerSecond(int pps)
{
    if (pps == m_particles_per_second)
        return;
    reset();
    m_particles_per_second = pps;
    emit particlesPerSecondChanged();
}



void TrailsEmitter::setParticleDuration(int dur)
{
    if (dur == m_particle_duration)
        return;
    reset();
    m_particle_duration = dur;
    emit particleDurationChanged();
}


void TrailsEmitter::setXSpeed(qreal x)
{
    if (x == m_x_speed)
        return;
    m_x_speed = x;
    emit xSpeedChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setYSpeed(qreal y)
{
    if (y == m_y_speed)
        return;
    m_y_speed = y;
    emit ySpeedChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setXSpeedVariation(qreal x)
{
    if (x == m_x_speed_variation)
        return;
    m_x_speed_variation = x;
    emit xSpeedVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setYSpeedVariation(qreal y)
{
    if (y == m_y_speed_variation)
        return;
    m_y_speed_variation = y;
    emit ySpeedVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setSpeedFromMovement(qreal t)
{
    if (t == m_speed_from_movement)
        return;
    m_speed_from_movement = t;
    emit speedFromMovementChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setXAccel(qreal x)
{
    if (x == m_x_accel)
        return;
    m_x_accel = x;
    emit xAccelChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setYAccel(qreal y)
{
    if (y == m_y_accel)
        return;
    m_y_accel = y;
    emit yAccelChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setXAccelVariation(qreal x)
{
    if (x == m_x_accel_variation)
        return;
    m_x_accel_variation = x;
    emit xAccelVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setYAccelVariation(qreal y)
{
    if (y == m_y_accel_variation)
        return;
    m_y_accel_variation = y;
    emit yAccelVariationChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    emit colorChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setColorVariation(qreal var)
{
    if (var == m_color_variation)
        return;
    m_color_variation = var;
    emit colorVariationChanged();
    m_system->pleaseUpdate();//XXX
}

void TrailsEmitter::setAdditive(qreal additive)
{
    if (m_additive == additive)
        return;
    m_additive = additive;
    emit additiveChanged();
    m_system->pleaseUpdate();//XXX
}


void TrailsEmitter::reset()
{
     delete m_node;
     delete m_material;

     m_node = 0;
     m_material = 0;
     m_particle_count = 0;
}

Node* TrailsEmitter::buildParticleNode()
{
    QSGContext *sg = QSGContext::current;

    m_particle_count = m_particle_duration * m_particles_per_second / 1000.;

    if (m_particle_count * 4 > 0xffff) {
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
    attr << QSGAttributeDescription(4, 4, GL_FLOAT, 0); // AnimData
    attr << QSGAttributeDescription(5, 4, GL_UNSIGNED_BYTE, 0); // Colors

    Geometry *g = new Geometry(attr);
    g->setDrawingMode(QSG::Triangles);

    int vCount = m_particle_count * 4;
    g->setVertexCount(vCount);
    ParticleVertex *vertices = (ParticleVertex *) g->vertexData();
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

void TrailsEmitter::prepareNextFrame(uint timeStamp)
{
    if (m_node == 0)
        return;

    if (m_reset_last) {
        m_last_emitter = m_last_last_emitter = QPointF(m_emitter_x, m_emitter_y);
        m_reset_last = false;
    }

    qreal time = timeStamp / 1000.;
    m_material->timelength = m_particle_duration / 1000.;
    m_material->timestamp = time;

    ParticleVertices *particles = (ParticleVertices *) m_node->geometry()->vertexData();

    qreal particleRatio = 1. / m_particles_per_second;
    qreal pt = m_last_particle * particleRatio;

    qreal opt = pt; // original particle time
    qreal dt = time - m_last_timestamp; // timestamp delta...

    // emitter difference since last...
    qreal dex = (m_emitter_x - m_last_emitter.x());
    qreal dey = (m_emitter_y - m_last_emitter.y());

    qreal ax = (m_last_last_emitter.x() + m_last_emitter.x()) / 2;
    qreal bx = m_last_emitter.x();
    qreal cx = (m_emitter_x + m_last_emitter.x()) / 2;
    qreal ay = (m_last_last_emitter.y() + m_last_emitter.y()) / 2;
    qreal by = m_last_emitter.y();
    qreal cy = (m_emitter_y + m_last_emitter.y()) / 2;

    float sizeAtEnd = m_particle_end_size >= 0 ? m_particle_end_size : m_particle_size;

    while (pt < time) {
        int pos = m_last_particle % m_particle_count;

        qreal t = 1 - (pt - opt) / dt;

        qreal vx =
          - 2 * ax * (1 - t)
          + 2 * bx * (1 - 2 * t)
          + 2 * cx * t;
        qreal vy =
          - 2 * ay * (1 - t)
          + 2 * by * (1 - 2 * t)
          + 2 * cy * t;

        ParticleVertices &p = particles[pos];

        // Particle timestamp
        p.v1.t = p.v2.t = p.v3.t = p.v4.t = pt;

        // Particle position
        p.v1.x = p.v2.x = p.v3.x = p.v4.x =
                m_last_emitter.x() + dex * (pt - opt) / dt
                - m_emitter_x_variation + rand() / float(RAND_MAX) * m_emitter_x_variation * 2;
        p.v1.y = p.v2.y = p.v3.y = p.v4.y =
                m_last_emitter.y() + dey * (pt - opt) / dt
                - m_emitter_y_variation + rand() / float(RAND_MAX) * m_emitter_y_variation * 2;

        // Particle speed
        p.v1.sx = p.v2.sx = p.v3.sx = p.v4.sx =
                m_x_speed
                - m_x_speed_variation + rand() / float(RAND_MAX) * m_x_speed_variation * 2
                + m_speed_from_movement * vx;
        p.v1.sy = p.v2.sy = p.v3.sy = p.v4.sy =
                m_y_speed
                - m_y_speed_variation + rand() / float(RAND_MAX) * m_y_speed_variation * 2
                + m_speed_from_movement * vy;

        // Particle acceleration
        p.v1.ax = p.v2.ax = p.v3.ax = p.v4.ax =
                m_x_accel - m_x_accel_variation + rand() / float(RAND_MAX) * m_x_accel_variation * 2;
        p.v1.ay = p.v2.ay = p.v3.ay = p.v4.ay =
                m_y_accel - m_y_accel_variation + rand() / float(RAND_MAX) * m_y_accel_variation * 2;

        // Particle size
        float sizeVariation = -m_particle_size_variation
                + rand() / float(RAND_MAX) * m_particle_size_variation * 2;

        float size = m_particle_size + sizeVariation;
        float endSize = sizeAtEnd + sizeVariation;

        p.v1.size = p.v2.size = p.v3.size = p.v4.size = size * float(m_emitting);
        p.v1.endSize = p.v2.endSize = p.v3.endSize = p.v4.endSize = endSize * float(m_emitting);

        // Particle color
        Color4ub color;
        color.r = m_color.red() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
        color.g = m_color.green() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
        color.b = m_color.blue() * (1 - m_color_variation) + rand() % 256 * m_color_variation;
        color.a = (1 - m_additive) * 255;
        p.v1.color = p.v2.color = p.v3.color = p.v4.color = color;

        ++m_last_particle;
        pt = m_last_particle * particleRatio;
    }

    m_last_last_last_emitter = m_last_last_emitter;
    m_last_last_emitter = m_last_emitter;
    m_last_emitter = QPointF(m_emitter_x, m_emitter_y);
    m_last_timestamp = time;
}


uint TrailsEmitter::particleCount()
{
    return m_particle_count;
}

ParticleVertices* TrailsEmitter::particles()
{
    return (ParticleVertices *) m_node->geometry()->vertexData();
}
