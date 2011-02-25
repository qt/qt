#include "spriteparticle.h"
#include "spritestate.h"
#include "particleemitter.h"
#include <qsgcontext.h>
#include <adaptationlayer.h>
#include <node.h>
#include <geometry.h>
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
    , m_particle_count(0)
    , m_particle_duration(1000)
{
}

void SpriteParticle::setCount(int c)
{
    m_particle_count = c;
}

int SpriteParticle::goalSeek(int curIdx, int dist)
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

void SpriteParticle::addToUpdateList(uint t, int idx)
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

bool SpriteParticle::buildParticleTexture(QSGContext *sg)
{
    int frameHeight = 0;
    int frameWidth = 0;
    m_maxFrames = 0;

    foreach(SpriteState* state, m_states){
        if(state->frames() > m_maxFrames)
            m_maxFrames = state->frames();

        QImage img(state->source().toLocalFile());
        if (img.isNull()) {
            qWarning() << "SpriteParticle: loading image failed..." << state->source().toLocalFile();
            return false;
        }

        if(frameWidth){
            if(img.width() / state->frames() != frameWidth){
                qWarning() << "SpriteParticle: Irregular frame width..." << state->source().toLocalFile();
                return false;
            }
        }else{
            frameWidth = img.width() / state->frames();
        }

        if(frameHeight){
            if(img.height()!=frameHeight){
                qWarning() << "SpriteParticle: Irregular frame height..." << state->source().toLocalFile();
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

    if (m_states.isEmpty()) {
        qWarning() << "SpriteParticle: No sprite states...";
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


    Geometry *g = new Geometry(attr);
    g->setDrawingMode(QSG::Triangles);

    int vCount = m_particle_count * 4;
    g->setVertexCount(vCount);
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
    p.v1.frameCount = p.v2.frameCount = p.v3.frameCount = p.v4.frameCount = m_states[0]->frames();
    int durVar = 0;
    if(m_states[0]->durationVariance())//TODO: Move to shader, and thus include acceleration over time
        durVar += qrand() % m_states[0]->durationVariance();
    if(m_states[0]->speedModifer())
        durVar += qFloor(m_states[0]->speedModifer() *  sqrt(pow(p.v1.sx, 2.0) + pow(p.v1.sy, 2.0)));//Arithmetic mean is faster, right?
    p.v1.frameDuration = p.v2.frameDuration = p.v3.frameDuration = p.v4.frameDuration = m_states[0]->duration() + durVar;

//    for(int i=0; i<m_stateUpdates.count(); i++)
//        m_stateUpdates[i].second.removeAll(pos);

//    if(m_states[0]->m_to.count() > 1)//Optimizes the single animation case
//        addToUpdateList(timeInt + ((m_states[0]->duration()+durVar) * m_states[0]->frames()), pos);

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
    m_material->animcount = m_states.count();

#if 0
    //Sprite State Update //MOVE TO MAGIC AFFECTOR
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
#endif
}

void SpriteParticle::reset()
{
    delete m_node;
    delete m_material;

    m_node = 0;
    m_material = 0;
    m_particle_count = 0;
}
