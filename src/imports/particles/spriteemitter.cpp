#include "spriteemitter.h"
#include "particlesystem.h"
#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <geometry.h>
#include <texturematerial.h>
#include <qsgtexturemanager.h>
#include <QFile>
#include <cmath>
#include <qmath.h>

class SpriteParticlesMaterial : public AbstractMaterial
{
public:
    SpriteParticlesMaterial()
        : timestamp(0)
        , timelength(1)
        , framecount(1)
        , animcount(1)
    {
        setFlag(Blending, true);
    }

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

SpriteEmitter::SpriteEmitter(QObject* parent)
    : ParticleEmitter(parent)
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


void SpriteEmitter::setParticleSize(qreal size)
{
    if (size == m_particle_size)
        return;
    m_particle_size = size;
    emit particleSizeChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setParticleEndSize(qreal size)
{
    if (size == m_particle_end_size)
        return;
    m_particle_end_size = size;
    emit particleEndSizeChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setParticleSizeVariation(qreal var)
{
    if (var == m_particle_size_variation)
        return;
    m_particle_size_variation = var;
    emit particleSizeVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setEmitterX(qreal x)
{
    if (x == m_emitter_x)
        return;
    m_emitter_x = x;
    emit emitterXChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setEmitterY(qreal y)
{
    if (y == m_emitter_y)
        return;
    m_emitter_y = y;
    emit emitterYChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setEmitterXVariation(qreal var)
{
    if (var == m_emitter_x_variation)
        return;
    m_emitter_x_variation = var;
    emit emitterXVariationChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setEmitterYVariation(qreal var)
{
    if (var == m_emitter_y_variation)
        return;
    m_emitter_y_variation = var;
    emit emitterYVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setEmitting(bool emitting)
{
    if (emitting == m_emitting)
        return;
    m_emitting = emitting;
    if (m_emitting)
      m_reset_last = true;
    emit emittingChanged();
    m_system->pleaseUpdate();
}



void SpriteEmitter::setImage(const QUrl &image)
{
    if (image == m_image_name)
        return;
    m_image_name = image;
    emit imageChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setColortable(const QUrl &table)
{
    if (table == m_colortable_name)
        return;
    m_colortable_name = table;
    emit colortableChanged();
    m_system->pleaseUpdate();
}



void SpriteEmitter::setParticlesPerSecond(int pps)
{
    if (pps == m_particles_per_second)
        return;
    reset();
    m_particles_per_second = pps;
    emit particlesPerSecondChanged();
}



void SpriteEmitter::setParticleDuration(int dur)
{
    if (dur == m_particle_duration)
        return;
    reset();
    m_particle_duration = dur;
    emit particleDurationChanged();
}


void SpriteEmitter::setXSpeed(qreal x)
{
    if (x == m_x_speed)
        return;
    m_x_speed = x;
    emit xSpeedChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setYSpeed(qreal y)
{
    if (y == m_y_speed)
        return;
    m_y_speed = y;
    emit ySpeedChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setXSpeedVariation(qreal x)
{
    if (x == m_x_speed_variation)
        return;
    m_x_speed_variation = x;
    emit xSpeedVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setYSpeedVariation(qreal y)
{
    if (y == m_y_speed_variation)
        return;
    m_y_speed_variation = y;
    emit ySpeedVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setSpeedFromMovement(qreal t)
{
    if (t == m_speed_from_movement)
        return;
    m_speed_from_movement = t;
    emit speedFromMovementChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setXAccel(qreal x)
{
    if (x == m_x_accel)
        return;
    m_x_accel = x;
    emit xAccelChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setYAccel(qreal y)
{
    if (y == m_y_accel)
        return;
    m_y_accel = y;
    emit yAccelChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setXAccelVariation(qreal x)
{
    if (x == m_x_accel_variation)
        return;
    m_x_accel_variation = x;
    emit xAccelVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setYAccelVariation(qreal y)
{
    if (y == m_y_accel_variation)
        return;
    m_y_accel_variation = y;
    emit yAccelVariationChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    emit colorChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setColorVariation(qreal var)
{
    if (var == m_color_variation)
        return;
    m_color_variation = var;
    emit colorVariationChanged();
    m_system->pleaseUpdate();
}

void SpriteEmitter::setAdditive(qreal additive)
{
    if (m_additive == additive)
        return;
    m_additive = additive;
    emit additiveChanged();
    m_system->pleaseUpdate();
}


void SpriteEmitter::reset()
{
    delete m_node;
    delete m_material;

    m_node = 0;
    m_material = 0;
    m_particle_count = 0;
}

int SpriteEmitter::goalSeek(int curIdx, int dist)
{
    //TODO: caching instead of excessively redoing iterative deepening (which was chosen arbitarily anyways)
    // Paraphrased - implement in an *efficient* manner
    if(m_goalState.isEmpty())
        return curIdx;
    for(int i=0; i<m_states.count(); i++)
        if(m_states[curIdx]->name() == m_goalState)
            return curIdx;
    if(dist < 0)
        dist = m_states.count();
    SpriteState* curState = m_states[curIdx];
    for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
        iter!=curState->m_to.constEnd(); iter++){
        if(iter.key() == m_goalState)
            for(int i=0; i<m_states.count(); i++)
                if(m_states[i]->name() == m_goalState)
                    return i;
    }
    QSet<int> options;
    for(int i=1; i<dist; i++){
        for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
            iter!=curState->m_to.constEnd(); iter++){
            int option = -1;
            for(int j=0; j<m_states.count(); j++)//One place that could be a lot more efficient...
                if(m_states[j]->name() == iter.key())
                    if(goalSeek(j, i) != -1)
                        option = j;
            if(option != -1)
                options << option;
        }
        if(!options.isEmpty()){
            if(options.count()==1)
                return *(options.begin());
            int option = -1;
            qreal r =(qreal) qrand() / (qreal) RAND_MAX;
            qreal total;
            for(QSet<int>::const_iterator iter=options.constBegin();
                iter!=options.constEnd(); iter++)
                total += curState->m_to.value(m_states[(*iter)]->name()).toReal();
            r *= total;
            for(QVariantMap::const_iterator iter = curState->m_to.constBegin();
                iter!=curState->m_to.constEnd(); iter++){
                bool superContinue = true;
                for(int j=0; j<m_states.count(); j++)
                    if(m_states[j]->name() == iter.key())
                        if(options.contains(j))
                            superContinue = false;
                if(superContinue)
                    continue;
                if(r < (*iter).toReal()){
                    bool superBreak = false;
                    for(int j=0; j<m_states.count(); j++){
                        if(m_states[j]->name() == iter.key()){
                            option = j;
                            superBreak = true;
                            break;
                        }
                    }
                    if(superBreak)
                        break;
                }
                r-=(*iter).toReal();
            }
            return option;
        }
    }
    return -1;
}

void SpriteEmitter::addToUpdateList(uint t, int idx)
{
    for(int i=0; i<m_stateUpdates.count(); i++){
        if(m_stateUpdates[i].first==t){
            m_stateUpdates[i].second << idx;
            return;
        }else if(m_stateUpdates[i].first > t){
            QList<int> tmpList;
            tmpList << idx;
            m_stateUpdates.insert(i, qMakePair(t, tmpList));
            return;
        }
    }
    QList<int> tmpList;
    tmpList << idx;
    m_stateUpdates << qMakePair(t, tmpList);
}

bool SpriteEmitter::buildParticleTexture(QSGContext *sg)
{
    int frameHeight = 0;
    int frameWidth = 0;
    m_maxFrames = 0;

    foreach(SpriteState* state, m_states){
        if(state->frames() > m_maxFrames)
            m_maxFrames = state->frames();

        QImage img(state->source().toLocalFile());
        if (img.isNull()) {
            qWarning() << "SpriteEmitter: loading image failed..." << state->source().toLocalFile();
            return false;
        }

        if(frameWidth){
            if(img.width() / state->frames() != frameWidth){
                qWarning() << "SpriteEmitter: Irregular frame width..." << state->source().toLocalFile();
                return false;
            }
        }else{
            frameWidth = img.width() / state->frames();
        }

        if(frameHeight){
            if(img.height()!=frameHeight){
                qWarning() << "SpriteEmitter: Irregular frame height..." << state->source().toLocalFile();
                return false;
            }
        }else{
            frameHeight = img.height();
        }
    }

    QImage image(frameWidth * m_maxFrames, frameHeight * m_states.count(), QImage::Format_ARGB32);
    image.fill(0);
    QPainter p(&image);
    int y = 0;
    foreach(SpriteState* state, m_states){
        QImage img(state->source().toLocalFile());
        p.drawImage(0,y,img);
        y += frameHeight;
    }
    m_material->texture = sg->textureManager()->upload(image);
    return true;
}

Node* SpriteEmitter::buildParticleNode()
{
    QSGContext *sg = QSGContext::current;

    m_particle_count = m_particle_duration * m_particles_per_second / 1000.;

    if (m_particle_count * 4 > 0xffff) {
        qWarning() << "SpriteEmitter: too many particles...";
        return 0;
    }

    if (m_states.isEmpty()) {
        qWarning() << "SpriteEmitter: No sprite states...";
        return 0;
    }

    if (m_material) {
        delete m_material;
        m_material = 0;
    }

    m_material = new SpriteParticlesMaterial();

    if(!buildParticleTexture(sg))//problem loading image files
        return 0;

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


    m_node = new GeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    m_last_particle = 0;
    return m_node;
}

void SpriteEmitter::prepareNextFrame(uint timeInt)
{
    if (m_node == 0) //error creating node
        return;

    if (m_reset_last) {
        m_last_emitter = m_last_last_emitter = QPointF(m_emitter_x, m_emitter_y);
        m_reset_last = false;
    }

    qreal time =  timeInt / 1000.;
    m_material->timelength = m_particle_duration / 1000.;
    m_material->timestamp = time;
    m_material->framecount = m_maxFrames;
    m_material->animcount = m_states.count();

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

//TODO:        foreach(ParticleAffector* a, m_affectors)
//            a->reset(pos);

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
        p.v1.dt = p.v2.dt = p.v3.dt = p.v4.dt = pt;

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

        //TODO: use endSize too,but I currently think the Vs should be split, to allow for size/rot affectors
        p.v2.x += p.v1.size;
        p.v3.y += p.v1.size;
        p.v4.x += p.v1.size;
        p.v4.y += p.v1.size;

        // Initial Sprite State
        p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = pt;
        p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = 0;
        p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = m_states[0]->frames();
        int durVar = 0;
        if(m_states[0]->durationVariance())//TODO: Move to shader, and thus include acceleration over time
            durVar += qrand() % m_states[0]->durationVariance();
        if(m_states[0]->speedModifer())
            durVar += qFloor(m_states[0]->speedModifer() *  qSqrt(p.v1.sx * p.v1.sx + p.v1.sy * p.v1.sy));//Arithmetic mean is faster, right?
        p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = m_states[0]->duration() + durVar;

        for(int i=0; i<m_stateUpdates.count(); i++)
            m_stateUpdates[i].second.removeAll(pos);

        if(m_states[0]->m_to.count() > 1)//Optimizes the single animation case
            addToUpdateList(timeInt + ((m_states[0]->duration()+durVar) * m_states[0]->frames()), pos);

        ++m_last_particle;
        pt = m_last_particle * particleRatio;

    }

    //Sprite State Update
    while(!m_stateUpdates.isEmpty() && timeInt >= m_stateUpdates.first().first){
        foreach(int idx, m_stateUpdates.first().second){
            ParticleVertices &p = particles[idx];
            int stateIdx = p.v1.animIdx;
            int nextIdx = -1;
            int goalPath = -1;
            if(m_goalState.isEmpty() || (goalPath = goalSeek(stateIdx)) == -1){//Random
                qreal r =(qreal) qrand() / (qreal) RAND_MAX;
                qreal total = 0.0;
                for(QVariantMap::const_iterator iter=m_states[stateIdx]->m_to.constBegin();
                    iter!=m_states[stateIdx]->m_to.constEnd(); iter++)
                    total += (*iter).toReal();
                r*=total;
                for(QVariantMap::const_iterator iter= m_states[stateIdx]->m_to.constBegin();
                        iter!=m_states[stateIdx]->m_to.constEnd(); iter++){
                    if(r < (*iter).toReal()){
                        bool superBreak = false;
                        for(int i=0; i<m_states.count(); i++){
                            if(m_states[i]->name() == iter.key()){
                                nextIdx = i;
                                superBreak = true;
                                break;
                            }
                        }
                        if(superBreak)
                            break;
                    }
                    r -= (*iter).toReal();
                }
            }else{//Random out of shortest paths to goal
                nextIdx = goalPath;
            }
            if(nextIdx == -1)//No to states means stay here
                nextIdx = stateIdx;

            p.v1.animT = p.v2.animT = p.v3.animT = p.v4.animT = time;
            p.v1.animIdx = p.v2.animIdx = p.v3.animIdx = p.v4.animIdx = nextIdx;
            p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = m_states[nextIdx]->frames();
            int durVar = 0;
            if(m_states[nextIdx]->durationVariance())
                durVar += (qrand() % (2*m_states[nextIdx]->durationVariance()))-m_states[nextIdx]->durationVariance();
            if(m_states[nextIdx]->speedModifer())
                durVar += qFloor(m_states[nextIdx]->speedModifer() * sqrt(pow(p.v1.sx, 2.0) + pow(p.v1.sy, 2.0)));//Arithmetic mean is faster, right?
            p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = m_states[nextIdx]->duration() + durVar;
            addToUpdateList(timeInt + ((m_states[nextIdx]->duration() + durVar)* m_states[nextIdx]->frames()), idx);
        }
        m_stateUpdates.pop_front();
    }

    m_last_last_last_emitter = m_last_last_emitter;
    m_last_last_emitter = m_last_emitter;
    m_last_emitter = QPointF(m_emitter_x, m_emitter_y);
    m_last_timestamp = time;

}

uint SpriteEmitter::particleCount()
{
    return m_particle_count;
}

ParticleVertices* SpriteEmitter::particles()
{
    return (ParticleVertices *) m_node->geometry()->vertexData();
}
