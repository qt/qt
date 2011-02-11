/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "subtree.h"
#include "qsgtexturemanager.h"
#include "qsgitem_p.h"
#include "adaptationlayer.h"
#include "renderer.h"
#include "qsgcanvas_p.h"

#include "qglframebufferobject.h"
#include "qmath.h"

QT_BEGIN_NAMESPACE


static const char qt_material_vertex_code[] =
    "uniform highp mat4 qt_Matrix;                      \n"
    "attribute highp vec4 qt_Vertex;                    \n"
    "attribute highp vec2 qt_MultiTexCoord0;            \n"
    "varying highp vec2 qt_TexCoord0;                   \n"
    "void main() {                                      \n"
    "    qt_TexCoord0 = qt_MultiTexCoord0;              \n"
    "    gl_Position = qt_Matrix * qt_Vertex;           \n"
    "}";

static const char qt_material_fragment_code[] =
    "varying highp vec2 qt_TexCoord0;                   \n"
    "uniform sampler2D qt_Texture;                      \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord0);\n"
    "}";

const char *TextureProviderMaterialShader::vertexShader() const
{
    return qt_material_vertex_code;
}

const char *TextureProviderMaterialShader::fragmentShader() const
{
    return qt_material_fragment_code;
}

AbstractMaterialType TextureProviderMaterialShader::type;

char const *const *TextureProviderMaterialShader::attributeNames() const
{
    static char const *const attr[] = { "qt_Vertex", "qt_MultiTexCoord0", 0 };
    return attr;
}

void TextureProviderMaterialShader::initialize()
{
    m_matrix_id = m_program.uniformLocation("qt_Matrix");
    if (m_program.isLinked()) {
        m_program.bind();
        m_program.setUniformValue("qt_Texture", GLuint(0));
    }
}

void TextureProviderMaterialShader::updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *oldEffect, Renderer::Updates updates)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());
    QSGTextureProvider *tx = static_cast<TextureProviderMaterial *>(newEffect)->texture();
    QSGTextureProvider *oldTx = oldEffect ? static_cast<TextureProviderMaterial *>(oldEffect)->texture() : 0;

    if (oldEffect == 0 || tx->texture()->textureId() != oldTx->texture()->textureId()) {
        renderer->setTexture(0, tx->texture());
        oldEffect = 0; // Force filtering update.
    }

    if (oldEffect == 0 || tx->linearFiltering() != oldTx->linearFiltering()) {
        int filtering = tx->linearFiltering() ? GL_LINEAR : GL_NEAREST;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
    }

    if (oldEffect == 0 || tx->clampToEdge() != oldTx->clampToEdge()) {
        int wrapMode = tx->clampToEdge() ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    }

    if (updates & Renderer::UpdateMatrices)
        m_program.setUniformValue(m_matrix_id, renderer->combinedMatrix());
}


void TextureProviderMaterial::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
    bool opaque = m_texture ? m_texture->opaque() : true;
    setFlag(Blending, !opaque);
}

AbstractMaterialType *TextureProviderMaterial::type() const
{
    return &TextureProviderMaterialShader::type;
}

AbstractMaterialShader *TextureProviderMaterial::createShader() const
{
    return new TextureProviderMaterialShader;
}

int TextureProviderMaterial::compare(const AbstractMaterial *o) const
{
    Q_ASSERT(o && type() == o->type());
    const TextureProviderMaterial *other = static_cast<const TextureProviderMaterial *>(o);
    if (int diff = m_texture->texture()->textureId() - other->texture()->texture()->textureId())
        return diff;
    if (int diff = int(m_texture->linearFiltering()) - int(other->m_texture->linearFiltering()))
        return diff;
    if (int diff = int(m_texture->clampToEdge()) - int(other->m_texture->clampToEdge()))
        return diff;
    return int(m_texture->opaque()) - int(other->m_texture->opaque());
}

bool TextureProviderMaterial::is(const AbstractMaterial *effect)
{
    return effect->type() == &TextureProviderMaterialShader::type;
}


static const char qt_scenegraph_texture_material_opacity_fragment[] =
    "varying highp vec2 qt_TexCoord0;                   \n"
    "uniform sampler2D qt_Texture;                      \n"
    "uniform lowp float opacity;                        \n"
    "void main() {                                      \n"
    "    gl_FragColor = texture2D(qt_Texture, qt_TexCoord0) * opacity; \n"
    "}";

class TextureProviderMaterialWithOpacityShader : public TextureProviderMaterialShader
{
public:
    virtual void updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *oldEffect, Renderer::Updates updates);
    virtual void initialize();

    static AbstractMaterialType type;

protected:
    virtual const char *fragmentShader() const { return qt_scenegraph_texture_material_opacity_fragment; }

    int m_opacity_id;
};

AbstractMaterialType TextureProviderMaterialWithOpacityShader::type;

bool TextureProviderMaterialWithOpacity::is(const AbstractMaterial *effect)
{
    return effect->type() == &TextureProviderMaterialWithOpacityShader::type;
}

AbstractMaterialType *TextureProviderMaterialWithOpacity::type() const
{
    return &TextureProviderMaterialWithOpacityShader::type;
}

void TextureProviderMaterialWithOpacity::setTexture(QSGTextureProvider *texture)
{
    m_texture = texture;
    setFlag(Blending, true);
}

AbstractMaterialShader *TextureProviderMaterialWithOpacity::createShader() const
{
    return new TextureProviderMaterialWithOpacityShader;
}

void TextureProviderMaterialWithOpacityShader::updateState(Renderer *renderer, AbstractMaterial *newEffect, AbstractMaterial *oldEffect, Renderer::Updates updates)
{
    Q_ASSERT(oldEffect == 0 || newEffect->type() == oldEffect->type());

    if (updates & Renderer::UpdateOpacity)
        m_program.setUniformValue(m_opacity_id, GLfloat(renderer->renderOpacity()));

    TextureProviderMaterialShader::updateState(renderer, newEffect, oldEffect, updates);
}

void TextureProviderMaterialWithOpacityShader::initialize()
{
    TextureProviderMaterialShader::initialize();
    m_opacity_id = m_program.uniformLocation("opacity");
}




TextureNode::TextureNode()
    : m_texture(0)
    , m_sourceRect(0, 1, 1, -1)
    , m_dirtyTexture(false)
    , m_dirtyGeometry(false)
{
    setMaterial(&m_materialO);
    setOpaqueMaterial(&m_material);

    QVector<QSGAttributeDescription> desc = QVector<QSGAttributeDescription>()
        << QSGAttributeDescription(0, 2, GL_FLOAT, 4 * sizeof(float))
        << QSGAttributeDescription(1, 2, GL_FLOAT, 4 * sizeof(float));
    updateGeometryDescription(desc, GL_UNSIGNED_SHORT);
}

void TextureNode::setTargetRect(const QRectF &rect)
{
    if (rect == m_targetRect)
        return;
    m_targetRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void TextureNode::setSourceRect(const QRectF &rect)
{
    if (rect == m_sourceRect)
        return;
    m_sourceRect = rect;
    m_dirtyGeometry = true;
    markDirty(DirtyGeometry);
}

void TextureNode::setTexture(QSGTextureProvider *texture)
{
    if (texture == m_texture)
        return;
    if (m_texture)
        disconnect(m_texture, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    m_texture = texture;
    if (m_texture)
        connect(m_texture, SIGNAL(textureChanged()), this, SLOT(markDirtyTexture()));
    markDirtyTexture();
}

void TextureNode::update()
{
    if (m_dirtyGeometry)
        updateGeometry();
    if (m_dirtyTexture)
        updateTexture();
}

void TextureNode::preprocess()
{
    m_texture->updateTexture();
}

void TextureNode::markDirtyTexture()
{
    m_dirtyTexture = true;
    markDirty(DirtyMaterial);
}

void TextureNode::updateGeometry()
{
    Geometry *g = geometry();
    g->setVertexCount(4);
    g->setIndexCount(0);
    g->setDrawingMode(QSG::TriangleStrip);
    struct V { float x, y, u, v; };
    V *v = static_cast<V *>(g->vertexData());

    for (int i = 0; i < 4; ++i) {
        v[i].x = (i & 2 ? m_targetRect.right() : m_targetRect.left());
        v[i].y = (i & 1 ? m_targetRect.bottom() : m_targetRect.top());
        v[i].u = (i & 2 ? m_sourceRect.right() : m_sourceRect.left());
        v[i].v = (i & 1 ? m_sourceRect.bottom() : m_sourceRect.top());
    }
    m_dirtyGeometry = false;
}

void TextureNode::updateTexture()
{
    m_material.setTexture(m_texture);
    m_materialO.setTexture(m_texture);
    m_dirtyTexture = false;
}


SubTreeTextureProvider::SubTreeTextureProvider(QObject *parent)
    : QSGTextureProvider(parent)
    , m_item(0)
    , m_renderer(0)
    , m_fbo(0)
#ifdef QML_SUBTREE_DEBUG
    , m_debugOverlay(0)
#endif
    , m_live(true)
    , m_dirtyTexture(true)
{
}

SubTreeTextureProvider::~SubTreeTextureProvider()
{
    delete m_renderer;
    delete m_fbo;
#ifdef QML_SUBTREE_DEBUG
    delete m_debugOverlay;
#endif
}

void SubTreeTextureProvider::updateTexture()
{
    if (m_dirtyTexture)
        grab();
}

QSGTextureRef SubTreeTextureProvider::texture()
{
    return m_texture;
}

void SubTreeTextureProvider::setItem(Node *item)
{
    if (item == m_item)
        return;
    m_item = item;
    markDirtyTexture();
}

void SubTreeTextureProvider::setRect(const QRectF &rect)
{
    if (rect == m_rect)
        return;
    m_rect = rect;
    markDirtyTexture();
}

void SubTreeTextureProvider::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    markDirtyTexture();
}

void SubTreeTextureProvider::markDirtyTexture()
{
    if (m_live) {
        m_dirtyTexture = true;
        emit textureChanged();
    }
}

void SubTreeTextureProvider::grab()
{
    Q_ASSERT(m_item);
    Node *root = m_item;
    while (root->childCount() && root->type() != Node::RootNodeType)
        root = root->childAtIndex(0);
    if (root->type() != Node::RootNodeType)
        return;

    qreal w = m_rect.width();
    qreal h = m_rect.height();

    if (w <= 0 || h <= 0) {
        m_texture = QSGTextureRef();
        delete m_fbo;
        m_fbo = 0;
        return;
    }

    if (!m_renderer) {
        m_renderer = QSGContext::current->createRenderer();
        connect(m_renderer, SIGNAL(sceneGraphChanged()), this, SLOT(markDirtyTexture()));
    }
    m_renderer->setRootNode(static_cast<RootNode *>(root));

    if (!m_fbo || m_fbo->width() != qCeil(w) || m_fbo->height() != qCeil(h)) {
        delete m_fbo;
        QGLFramebufferObjectFormat format;
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QGLFramebufferObject(qCeil(w), qCeil(h), format);
        QSGTexture *tex = new QSGTexture;
        tex->setTextureId(m_fbo->texture());
        tex->setOwnsTexture(false);
        m_texture = QSGTextureRef(tex);
    }

    // Render texture.
    Node::DirtyFlags dirty = root->dirtyFlags();
    root->markDirty(Node::DirtyNodeAdded); // Force matrix and clip update.
    m_renderer->nodeChanged(root, Node::DirtyNodeAdded); // Force render list update.

#ifdef QML_SUBTREE_DEBUG
    if (!m_debugOverlay)
        m_debugOverlay = QSGContext::current->createRectangleNode();
    m_debugOverlay->setRect(QRectF(0, 0, m_fbo->width(), m_fbo->height()));
    m_debugOverlay->setColor(QColor(0xff, 0x00, 0x80, 0x40));
    m_debugOverlay->setPenColor(QColor());
    m_debugOverlay->setPenWidth(0);
    m_debugOverlay->setRadius(0);
    m_debugOverlay->update();
    root->appendChildNode(m_debugOverlay);
#endif

    const QGLContext *ctx = QSGContext::current->glContext();
    m_renderer->setDeviceRect(m_fbo->size());
    m_renderer->setProjectMatrixToRect(m_rect);
    m_renderer->setClearColor(Qt::transparent);
    m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(ctx), m_fbo));

    root->markDirty(dirty | Node::DirtyNodeAdded); // Force matrix, clip and render list update.

#ifdef QML_SUBTREE_DEBUG
    root->removeChildNode(m_debugOverlay);
#endif
    m_dirtyTexture = false;
}


TextureItem::TextureItem(QSGItem *parent)
    : QSGItem(parent)
    , m_textureProvider(0)
{
    setFlag(ItemHasContents);
}

QSGTextureProvider *TextureItem::textureProvider() const
{
    return m_textureProvider;
}

void TextureItem::setTextureProvider(QSGTextureProvider *provider, bool requiresPreprocess)
{
    Q_ASSERT(m_textureProvider == 0); // Can only be set once.
    m_textureProvider = provider;
    m_requiresPreprocess = requiresPreprocess;
}

Node *TextureItem::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    TextureNode *node = static_cast<TextureNode *>(oldNode);
    if (!node) {
        node = new TextureNode;
        node->setFlag(Node::UsePreprocess, m_requiresPreprocess);
        node->setTexture(m_textureProvider);
    }

    m_textureProvider->setClampToEdge(true);
    m_textureProvider->setLinearFiltering(QSGItemPrivate::get(this)->smooth);

    node->setTargetRect(QRectF(0, 0, width(), height()));
    node->setSourceRect(QRectF(0, 1, 1, -1));
    node->update();

    return node;
}


SubTree::SubTree(QSGItem *parent)
    : TextureItem(parent)
    , m_item(0)
    , m_margins(0, 0)
    , m_live(true)
{
    setTextureProvider(new SubTreeTextureProvider(this), true);
}

SubTree::~SubTree()
{
    if (m_item)
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
}

QSGItem *SubTree::item() const
{
    return m_item;
}

void SubTree::setItem(QSGItem *item)
{
    if (item == m_item)
        return;
    if (m_item) {
        QSGItemPrivate::get(m_item)->derefFromEffectItem();
        if (m_item->parentItem() == this)
            m_item->setParentItem(0);
    }
    m_item = item;
    if (m_item) {
        // TODO: Find better solution.
        // 'm_item' needs a canvas to get a scenegraph node.
        // The easiest way to make sure it gets a canvas is to
        // make it a part of the same item tree as 'this'.
        if (m_item->parentItem() == 0)
            m_item->setParentItem(this);
        QSGItemPrivate::get(m_item)->refFromEffectItem();
    }
    update();
    emit itemChanged();
}

QSizeF SubTree::margins() const
{
    return m_margins;
}

void SubTree::setMargins(const QSizeF &margins)
{
    if (margins == m_margins)
        return;
    m_margins = margins;
    update();
    emit marginsChanged();
}

bool SubTree::live() const
{
    return m_live;
}

void SubTree::setLive(bool live)
{
    if (live == m_live)
        return;
    m_live = live;
    update();
    emit liveChanged();
}

void SubTree::grab()
{
    if (!m_item)
        return;
    QSGCanvas *canvas = m_item->canvas();
    QSGCanvasPrivate::get(canvas)->updateDirtyNodes();
    QGLContext *glctx = const_cast<QGLContext *>(canvas->context());
    glctx->makeCurrent();
    static_cast<SubTreeTextureProvider *>(textureProvider())->grab();
}

Node *SubTree::updatePaintNode(Node *oldNode, UpdatePaintNodeData *data)
{
    if (!m_item) {
        delete oldNode;
        return 0;
    }

    SubTreeTextureProvider *tp = static_cast<SubTreeTextureProvider *>(textureProvider());
    tp->setItem(QSGItemPrivate::get(m_item)->itemNode());
    QRectF rect(0, 0, m_item->width(), m_item->height());
    rect.adjust(-m_margins.width(), -m_margins.height(), m_margins.width(), m_margins.height());
    tp->setRect(rect);
    tp->setLive(m_live);

    return TextureItem::updatePaintNode(oldNode, data);
}

QT_END_NAMESPACE
