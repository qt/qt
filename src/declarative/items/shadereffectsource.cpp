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

#include "shadereffectsource.h"

#include "qsgitem_p.h"
#include "qsgcontext.h"
#include "renderer.h"

#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qimagereader.h>

ShaderEffectSource::ShaderEffectSource(QObject *parent)
    : QObject(parent)
    , m_sourceItem(0)
    , m_mipmap(None)
    , m_filtering(Nearest)
    , m_horizontalWrap(ClampToEdge)
    , m_verticalWrap(ClampToEdge)
    , m_margins(0, 0)
    , m_size(0, 0)
    , m_fbo(0)
    , m_multisampledFbo(0)
    , m_renderer(0)
    , m_refs(0)
    , m_dirtyTexture(true)
    , m_dirtySceneGraph(true)
    , m_multisamplingSupported(false)
    , m_checkedForMultisamplingSupport(false)
    , m_live(true)
    , m_hideOriginal(true)
{
    m_context = QSGContext::current;
}

ShaderEffectSource::~ShaderEffectSource()
{
    if (m_refs && m_sourceItem)
        QSGItemPrivate::get(m_sourceItem)->derefFromEffectItem();
    delete m_fbo;
    delete m_multisampledFbo;
    delete m_renderer;
}

void ShaderEffectSource::setSourceItem(QSGItem *item)
{
    if (item == m_sourceItem)
        return;

    if (m_sourceItem) {
        disconnect(m_sourceItem, SIGNAL(widthChanged()), this, SLOT(markSourceSizeDirty()));
        disconnect(m_sourceItem, SIGNAL(heightChanged()), this, SLOT(markSourceSizeDirty()));
        if (m_refs && m_hideOriginal) {
            QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
            d->derefFromEffectItem();
        }
    }

    m_sourceItem = item;

    if (m_sourceItem) {
        if (m_refs && m_hideOriginal) {
            QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
            d->refFromEffectItem();
        }
        connect(m_sourceItem, SIGNAL(widthChanged()), this, SLOT(markSourceSizeDirty()));
        connect(m_sourceItem, SIGNAL(heightChanged()), this, SLOT(markSourceSizeDirty()));
    }

    updateSizeAndTexture();
    emit sourceItemChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setSourceImage(const QUrl &url)
{
    if (url == m_sourceImage)
        return;
    m_sourceImage = url;
    updateSizeAndTexture();
    emit sourceImageChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setMipmap(FilterMode mode)
{
    if (mode == m_mipmap)
        return;
    m_mipmap = mode;
    if (m_mipmap != None) {
        // Mipmap enabled, need to reallocate the textures.
        if (m_fbo && !m_fbo->format().mipmap()) {
            Q_ASSERT(m_sourceItem);
            delete m_fbo;
            m_fbo = 0;
            m_dirtyTexture = true;
        } else if (!m_texture.isNull()) {
            Q_ASSERT(!m_sourceImage.isEmpty());
            if (!m_texture->hasMipmaps())
                updateSizeAndTexture();
        }
    }
    emit mipmapChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setFiltering(FilterMode mode)
{
    if (mode == m_filtering)
        return;
    m_filtering = mode;
    emit filteringChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setHorizontalWrap(WrapMode mode)
{
    if (mode == m_horizontalWrap)
        return;
    m_horizontalWrap = mode;
    emit horizontalWrapChanged();
    emit repaintRequired();
} 
void ShaderEffectSource::setVerticalWrap(WrapMode mode)
{
    if (mode == m_verticalWrap)
        return;
    m_verticalWrap = mode;
    emit verticalWrapChanged();
    emit repaintRequired();
} 

void ShaderEffectSource::setMargins(const QSize &size)
{
    if (size == m_margins)
        return;
    m_margins = size;
    updateSizeAndTexture();
    emit marginsChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setTextureSize(const QSize &size)
{
    if (size == m_textureSize)
        return;
    m_textureSize = size;
    updateSizeAndTexture();
    emit textureSizeChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setLive(bool s)
{
    if (s == m_live)
        return;
    m_live = s;

    emit liveChanged();
    emit repaintRequired();
}

void ShaderEffectSource::setHideOriginal(bool hide)
{
    if (hide == m_hideOriginal)
        return;

    if (m_refs && m_sourceItem && m_hideOriginal) {
        QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
        d->derefFromEffectItem();
    }

    m_hideOriginal = hide;

    if (m_refs && m_sourceItem && m_hideOriginal) {
        QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
        d->refFromEffectItem();
    }

    emit hideOriginalChanged();
    emit repaintRequired();
}

void ShaderEffectSource::bind() const
{
    bool linear = m_filtering == Linear;

    GLint filtering = GL_NEAREST;
    switch (m_mipmap) {
    case Nearest:
        filtering = linear ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
        break;
    case Linear:
        filtering = linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
        break;
    default:
        filtering = linear ? GL_LINEAR : GL_NEAREST;
        break;
    }

    GLuint hwrap = m_horizontalWrap == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    GLuint vwrap = m_verticalWrap == Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE;

#if !defined(QT_OPENGL_ES_2)
    glEnable(GL_TEXTURE_2D);
#endif
    if (m_fbo) {
        glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
    } else if (!m_texture.isNull()) {
        glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
    } else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, hwrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, vwrap);
}

void ShaderEffectSource::refFromEffectItem()
{
    if (m_refs++ == 0) {
        if (m_sourceItem && m_hideOriginal) {
            QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
            d->refFromEffectItem();
        }
        emit activeChanged();
    }
}

void ShaderEffectSource::derefFromEffectItem()
{
    if (--m_refs == 0) {
        if (m_sourceItem && m_hideOriginal) {
            QSGItemPrivate *d = QSGItemPrivate::get(m_sourceItem);
            d->derefFromEffectItem();
        }
        emit activeChanged();
    }
    Q_ASSERT(m_refs >= 0);
}

void ShaderEffectSource::update()
{
    Q_ASSERT(m_refs);
    QSGItemPrivate *src = m_sourceItem ? QSGItemPrivate::get(m_sourceItem) : 0;
    Node::DirtyFlags dirtyFlags = src ? src->itemNode()->dirtyFlags() : Node::DirtyFlags(0);
    if (!m_dirtyTexture && (!(m_dirtySceneGraph || dirtyFlags) || !m_live)) {
        return;
    }

    if (!m_context)
        m_context = QSGContext::current;

    Q_ASSERT(m_context);

    if (m_sourceItem) {
        if (!m_renderer) {
            m_renderer = m_context->createRenderer();
            connect(m_renderer, SIGNAL(sceneGraphChanged()), this, SLOT(markSceneGraphDirty()));
            RootNode *root = new RootNode;
            m_renderer->setRootNode(root);
        }

        RootNode *root = m_renderer->rootNode();
        Node *childrenNode = src->childContainerNode();

        // XXX todo - optimize
        while (childrenNode->childCount()) {
            Node *child = childrenNode->childAtIndex(0);
            childrenNode->removeChildNode(child);
            root->appendChildNode(child);
        }

        const QGLContext *ctx = m_context->glContext();

        if (!m_checkedForMultisamplingSupport) {
            QList<QByteArray> extensions = QByteArray((const char *)glGetString(GL_EXTENSIONS)).split(' ');
            m_multisamplingSupported = extensions.contains("GL_EXT_framebuffer_multisample")
                                       && extensions.contains("GL_EXT_framebuffer_blit");
            m_checkedForMultisamplingSupport = true;
        }


        if (!m_fbo) {
            if (ctx->format().sampleBuffers() && m_multisamplingSupported) {
                // If mipmapping was just enabled, m_fbo might be 0 while m_multisampledFbo != 0.
                if (!m_multisampledFbo) {
                    QGLFramebufferObjectFormat format;
                    format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
                    format.setSamples(ctx->format().samples());
                    m_multisampledFbo = new QGLFramebufferObject(m_size, format);
                }
                {
                    QGLFramebufferObjectFormat format;
                    format.setAttachment(QGLFramebufferObject::NoAttachment);
                    format.setMipmap(m_mipmap != None);
                    m_fbo = new QGLFramebufferObject(m_size, format);
                }
            } else {
                QGLFramebufferObjectFormat format;
                format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
                format.setMipmap(m_mipmap != None);
                m_fbo = new QGLFramebufferObject(m_size, format);
            }
        }

        Q_ASSERT(m_size == m_fbo->size());
        Q_ASSERT(m_multisampledFbo == 0 || m_size == m_multisampledFbo->size());

        QRectF r(0, 0, m_sourceItem->width(), m_sourceItem->height());
        r.adjust(-m_margins.width(), -m_margins.height(), m_margins.width(), m_margins.height());
        m_renderer->setDeviceRect(m_size);
        m_renderer->setViewportRect(m_size);
        m_renderer->setProjectMatrixToRect(r);
        m_renderer->setClearColor(Qt::transparent);

        if (m_multisampledFbo) {
            m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(m_context->glContext()), m_multisampledFbo));
            QRect r(0, 0, m_size.width(), m_size.height());
            QGLFramebufferObject::blitFramebuffer(m_fbo, r, m_multisampledFbo, r);
        } else {
            m_renderer->renderScene(BindableFbo(const_cast<QGLContext *>(m_context->glContext()), m_fbo));
        }
        if (m_mipmap != None) {
            QGLFramebufferObject::bindDefault();
            glBindTexture(GL_TEXTURE_2D, m_fbo->texture());
            m_context->renderer()->glGenerateMipmap(GL_TEXTURE_2D);
        }

        while (root->childCount()) {
            Node *child = root->childAtIndex(0);
            root->removeChildNode(child);
            childrenNode->appendChildNode(child);
        }

        // ### gunnar: If the source is hidden, the itemNode() will never be
        // cleared by the update pass, so we need to manually clear it dirty state.
        // Otherwise, it will always be marked as dirty...
        // However... this trick only works if one source is referencing the item because
        // the reparenting and dirty marking in the scenegraph is just wrong. Until
        // we have proper visibility and effect handling, live with a small hack.
        if (src->effectRefCount == 1) {
            src->itemNode()->clearDirty();
        }


        m_dirtySceneGraph = false;

    } else if (!m_image.isNull() && m_texture.isNull() || m_dirtyTexture) {

        QSGTextureManager *tm = m_context->textureManager();
        m_texture = tm->upload(m_image.mirrored());

        if (m_mipmap) {
            glBindTexture(GL_TEXTURE_2D, m_texture->textureId());
            m_context->renderer()->glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

    }

    m_dirtyTexture = false;
}

void ShaderEffectSource::grab()
{
    m_dirtyTexture = true;
    emit repaintRequired();
}

void ShaderEffectSource::markSceneGraphDirty()
{
    m_dirtySceneGraph = true;
    emit repaintRequired();
}

void ShaderEffectSource::markSourceSizeDirty()
{
    Q_ASSERT(m_sourceItem);
    if (m_textureSize.isEmpty())
        updateSizeAndTexture();
    if (m_refs)
        emit repaintRequired();
}

void ShaderEffectSource::updateSizeAndTexture()
{
    if (m_sourceItem) {
        QSize size = m_textureSize;
        if (size.isEmpty())
            size = QSizeF(m_sourceItem->width(), m_sourceItem->height()).toSize() + 2 * m_margins;
        if (size.width() < 1)
            size.setWidth(1);
        if (size.height() < 1)
            size.setHeight(1);
        if (m_fbo && m_fbo->size() != size) {
            delete m_fbo;
            delete m_multisampledFbo;
            m_fbo = m_multisampledFbo = 0;
        }
        if (m_size.width() != size.width()) {
            m_size.setWidth(size.width());
            emit widthChanged();
        }
        if (m_size.height() != size.height()) {
            m_size.setHeight(size.height());
            emit heightChanged();
        }
        m_dirtyTexture = true;
    } else {
        if (m_fbo) {
            delete m_fbo;
            delete m_multisampledFbo;
            m_fbo = m_multisampledFbo = 0;
        }
        if (!m_sourceImage.isEmpty()) {
            // TODO: Implement async loading and loading over network.
            QImageReader reader(m_sourceImage.toLocalFile());
            if (!m_textureSize.isEmpty())
                reader.setScaledSize(m_textureSize);
            m_image = reader.read();
            if (m_image.isNull())
                qWarning() << reader.errorString();
            if (m_size.width() != m_image.width()) {
                m_size.setWidth(m_image.width());
                emit widthChanged();
            }
            if (m_size.height() != m_image.height()) {
                m_size.setHeight(m_image.height());
                emit heightChanged();
            }

            m_dirtyTexture = true;

        } else {
            if (m_size.width() != 0) {
                m_size.setWidth(0);
                emit widthChanged();
            }
            if (m_size.height() != 0) {
                m_size.setHeight(0);
                emit heightChanged();
            }
        }
    }
}

